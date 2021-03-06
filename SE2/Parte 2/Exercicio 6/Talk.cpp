#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <process.h>

#define BUFSIZE 1024

VOID WINAPI CreateChildProcess();
VOID WINAPI PrintLastError();
VOID WINAPI WriteToPipe(HANDLE);
unsigned WINAPI ReadFromPipe(void *);

typedef struct _arg{
	HANDLE h;
} ARG;

HANDLE readPai, writePai, readFilho, writeFilho, inStd, outStd;

PTCHAR process;
BOOL end;

DWORD _tmain(DWORD argc, PTCHAR argv[]) {
	ARG arg;
	
	if(argc==1) {
		process = _T("Child");

		SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
		
		if(!CreatePipe(&readPai, &writePai, &sa, 0)) PrintLastError();
		if(!CreatePipe(&readFilho, &writeFilho, &sa, 0)) PrintLastError();
		
		inStd = GetStdHandle(STD_INPUT_HANDLE);
		outStd = GetStdHandle(STD_OUTPUT_HANDLE);
		
		CreateChildProcess();
		
		arg = {readFilho};
		
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ReadFromPipe, &arg, 0, NULL);
		
		WriteToPipe(writePai);
		CloseHandle(hThread);
	} else if (argc==3) {
		printf("\n ** This is the child process. ** \n\n");
		process = _T("Daddy");
	
		writeFilho = (HANDLE) atoi(argv[1]);
		readPai = (HANDLE) atoi(argv[2]);
		outStd = GetStdHandle(STD_OUTPUT_HANDLE);
		inStd = GetStdHandle(STD_INPUT_HANDLE);
		
		arg = {readPai};
		
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &ReadFromPipe, &arg, 0, NULL);
		
		WriteToPipe(writeFilho);
		CloseHandle(hThread);
	}

	return 0;
}

VOID WINAPI CreateChildProcess() {
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi;

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.lpTitle = _T("Child Process");
	
	TCHAR commandLine[100];
	PTCHAR childName = _T("Talk.exe ");
	_stprintf(commandLine, "%s %lld %lld", childName, (UINT_PTR)writeFilho, (UINT_PTR)readPai);
	
	if(!CreateProcess(NULL, commandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) PrintLastError();
	
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

VOID WINAPI WriteToPipe(HANDLE hWrite) {
	DWORD dwRead;
	TCHAR chBuf[BUFSIZE];

	do {
		ZeroMemory(chBuf, BUFSIZE);
		if(!ReadFile(inStd, chBuf, BUFSIZE, &dwRead, NULL)) PrintLastError();
		if(!WriteFile(hWrite, chBuf, dwRead, NULL, NULL)) PrintLastError();
	} while(!end && _tcsicmp(chBuf, _T("EXIT\r\n")));
	end = true;
	CloseHandle(hWrite);
}

unsigned WINAPI ReadFromPipe(void *args) {
	DWORD dwRead;
	TCHAR chBuf[BUFSIZE];
	
	HANDLE hRead = ((ARG *)args)->h;

	do {
		ZeroMemory(chBuf, BUFSIZE);
		if(!ReadFile(hRead, chBuf, BUFSIZE, &dwRead, NULL)) PrintLastError();
		_tprintf("%s said: %s", process, chBuf);
	} while(!end && _tcsicmp(chBuf, _T("EXIT\r\n")));
	end = true;
	CloseHandle(hRead);
	return 0;
}

VOID WINAPI PrintLastError() {
	LPTSTR pBuf;
	DWORD dwLastError = GetLastError();
	_tprintf(_T("GetLastError = %d\n"), dwLastError);
	if(!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, 0, (LPTSTR) &pBuf, 0, NULL))
		_tprintf(_T("Format message failed with 0x%x\n"), GetLastError());
	_tprintf(pBuf);
	LocalFree((HLOCAL) pBuf);
	exit(0);
}