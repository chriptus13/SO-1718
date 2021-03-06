// ASyncCopyLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <assert.h>

#include "AsyncCopyLib.h"

#define BUFFER_SIZE 64
#define STATUS_OK 0

typedef VOID(*TerminateSub)();
DWORD amountRunning = 0;

typedef struct OperCtx {
	OVERLAPPED ovr;
	HANDLE file;
	LPVOID buffer;
	UINT64 currPos;
	AsyncCallback cb;
	LPVOID userCtx;
	BOOL rNw;
	BOOL* Stop;
	DWORD* lastFix;
	LPVOID other;
	TerminateSub test;
	BOOL terminate;
} OPER_CTX, *POPER_CTX;

HANDLE ioport;

//IOCP Worker Function---------------------------------------------

VOID DestroyOpContext(POPER_CTX ctx) {
	CloseHandle(ctx->file);
	if (ctx->rNw) {
		free(ctx->buffer);
		free(ctx->Stop);
		free(ctx->lastFix);
		ctx->test();
	}
	free(ctx);
}

VOID DispatchAndReleaseOper(POPER_CTX opCtx, DWORD status) {
	opCtx->cb(opCtx->userCtx, status, opCtx->currPos);
	DestroyOpContext(opCtx);
}

VOID ChangeContext(POPER_CTX opCtx) {
	LARGE_INTEGER li;
	InterlockedAdd((volatile LONG*)(&(opCtx->currPos)), *opCtx->lastFix);
	li.QuadPart = opCtx->currPos;
	// adjust overlapped offset
	InterlockedExchange(&opCtx->ovr.Offset, li.LowPart);
	InterlockedExchange(&opCtx->ovr.OffsetHigh, li.HighPart);
}

BOOL ReadAsync(DWORD toTransfer, POPER_CTX opCtx) {
	if (!ReadFile(opCtx->file, opCtx->buffer, toTransfer, 0, &opCtx->ovr)
		&& GetLastError() != ERROR_IO_PENDING) return FALSE;
	ChangeContext(opCtx);
	return TRUE;
}

BOOL WriteAsync(DWORD toTransfer, POPER_CTX opCtx) {
	if (!WriteFile(opCtx->file, opCtx->buffer, *opCtx->lastFix, 0, &opCtx->ovr)
		&& GetLastError() != ERROR_IO_PENDING) return FALSE;
	ChangeContext(opCtx);
	return TRUE;
}

VOID ProcessRequest(POPER_CTX opCtx, DWORD transferedBytes) {
	if (transferedBytes == 0 || *opCtx->Stop) {
		DispatchAndReleaseOper(opCtx, STATUS_OK);
		return;
	}

	if (opCtx->rNw) {
		if (!WriteAsync(BUFFER_SIZE, opCtx)) {
			//error on operation, abort calling user callback
			DispatchAndReleaseOper(opCtx, GetLastError());
		}
		return;
	}
	if (!ReadAsync(BUFFER_SIZE, opCtx)) {
		*opCtx->Stop = TRUE;
		//error on operation, abort calling user callback
		DispatchAndReleaseOper(opCtx, GetLastError());
	}
}

unsigned WINAPI IOCP_ThreadFunc(LPVOID arg) {
	DWORD transferedBytes;
	ULONG_PTR completionKey;
	POPER_CTX opCtx;

	while (TRUE) {
		BOOL work = GetQueuedCompletionStatus(ioport,
			&transferedBytes, &completionKey, (LPOVERLAPPED *)&opCtx, INFINITE);
		if (opCtx->terminate == TRUE) { opCtx->test(); return 0; }
		if (!work) {
			transferedBytes = 0;
			DWORD error = GetLastError();
			*opCtx->Stop = TRUE;
			if (error != ERROR_HANDLE_EOF) {
				// operation error, abort calling callback
				DispatchAndReleaseOper(opCtx, error);
				continue;
			}
		}
		if (!opCtx->rNw) {
			((POPER_CTX)(opCtx->other))->rNw = TRUE;
			if (transferedBytes != 0) *opCtx->lastFix = transferedBytes;
		}
		ProcessRequest((POPER_CTX)opCtx->other, transferedBytes);
	}
	return 0;
}

POPER_CTX CreateOpContext(HANDLE file, AsyncCallback cb, LPVOID userCtx, LPVOID buffer, BOOL * stop, DWORD * lastFix) {
	POPER_CTX op = (POPER_CTX)calloc(1, sizeof(OPER_CTX));
	op->cb = cb;
	op->userCtx = userCtx;
	op->file = file;
	op->buffer = buffer;
	op->Stop = stop;
	op->rNw = FALSE;
	op->lastFix = lastFix;
	op->test = []() {InterlockedDecrement(&amountRunning); };
	return op;
}

DWORD GetNumberOfProcessors() {
	SYSTEM_INFO sysi;
	GetSystemInfo(&sysi);
	return sysi.dwNumberOfProcessors;
}

// IOCP Functions -----------------------------------------------

HANDLE CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads) {
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE hComplPort, HANDLE hDevice, DWORD CompletionKey) {
	return CreateIoCompletionPort(hDevice, hComplPort, CompletionKey, 0) == hComplPort;
}

HANDLE OpenAsync(PCTSTR fName, DWORD permissions, DWORD openType) {
	HANDLE hFile = CreateFile(fName, permissions,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		openType,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) return NULL;
	if (!AssociateDeviceWithCompletionPort(ioport, hFile, (ULONG_PTR)hFile)) {
		CloseHandle(hFile);
		return NULL;
	}
	return hFile;
}

//Public ----------------------------------------------------------------
DWORD NumberOfThreads = 0;

BOOL AsyncInit() {
	if (ioport != 0) return TRUE;
	ioport = CreateNewCompletionPort(0);
	assert(ioport != 0);

	NumberOfThreads = GetNumberOfProcessors();
	for (int i = 0; i < NumberOfThreads; i++)
		if (!_beginthreadex(NULL, 0, &IOCP_ThreadFunc, ioport, 0, 0)) return FALSE;

	return TRUE;
}

BOOL CopyFileAsync(PCTSTR srcFile, PCTSTR dstFile, AsyncCallback cb, LPVOID userCtx) {
	//Create file handles and associate them with IOCP
	HANDLE src = OpenAsync(srcFile, GENERIC_READ, OPEN_EXISTING);
	HANDLE dst = OpenAsync(dstFile, GENERIC_WRITE, CREATE_ALWAYS);
	if (src == NULL || dst == NULL) return FALSE;

	//Buffer shared between contexts
	LPVOID buffer = malloc(BUFFER_SIZE);
	ZeroMemory(buffer, BUFFER_SIZE);

	//Boolean shared between contexts
	BOOL* Stop = (BOOL*)malloc(sizeof(BOOL));
	*Stop = FALSE;

	//Number of bytes transfered
	DWORD *lastFix = (DWORD*)malloc(sizeof(DWORD));
	*lastFix = BUFFER_SIZE;

	//Create two contexts, one for read and other for write operations
	POPER_CTX opCtxIn = CreateOpContext(src, cb, userCtx, buffer, Stop, lastFix);
	POPER_CTX opCtxOut = CreateOpContext(dst, cb, userCtx, buffer, Stop, lastFix);

	//Contexts know each other
	opCtxIn->other = opCtxOut;
	opCtxOut->other = opCtxIn;

	//Starts a read operation with buffer size
	if (ReadAsync(BUFFER_SIZE, opCtxIn)) {
		++amountRunning;
		return TRUE;
	}
	DestroyOpContext(opCtxIn);
	DestroyOpContext(opCtxOut);
	return FALSE;
}


VOID AsyncTerminate() {
	if (ioport == 0) return;
	POPER_CTX op = (POPER_CTX)calloc(1, sizeof(OPER_CTX));
	op->terminate = TRUE;
	op->test = []() {InterlockedDecrement(&NumberOfThreads); };
	while (NumberOfThreads != 0 && (amountRunning == 0 ? PostQueuedCompletionStatus(ioport, 0, 0, (LPOVERLAPPED)op) : 1));
	CloseHandle(ioport);
	free(op);
	printf("Struct Terminated \n");
}

