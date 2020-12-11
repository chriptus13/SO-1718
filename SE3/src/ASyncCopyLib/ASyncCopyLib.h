#pragma once

#include <Windows.h>

#ifdef ASYNCCOPYLIB_EXPORTS
#define ASYNCCOPYLIB_API  __declspec(dllexport)
#else
#define ASYNCCOPYLIB_API __declspec(dllimport)
#endif 

#ifdef UNICODE
#define CreateFile CreateFileW
#else
#define CreateFile CreateFileA
#endif

ASYNCCOPYLIB_API typedef VOID(*AsyncCallback)(
	LPVOID userCtx,
	DWORD status,	//0 or GetLastError result
	UINT64 transferedBytes
);

#ifdef __cplusplus
extern "C" {
#endif
	ASYNCCOPYLIB_API BOOL AsyncInit();
	ASYNCCOPYLIB_API BOOL CopyFileAsync(PCTSTR srcFile, PCTSTR dstFile, AsyncCallback cb, LPVOID userCtx);
	ASYNCCOPYLIB_API VOID AsyncTerminate();
#ifdef __cplusplus
}
#endif