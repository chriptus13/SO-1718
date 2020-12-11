#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include <Windows.h>

#ifdef DLL_FILE_UTILS_EXPORTS
#define DLL_FILE_UTILS_API __declspec(dllexport)
#else
#define DLL_FILE_UTILS_API __declspec(dllimport)
#endif

typedef struct {
	HANDLE hFile;
	HANDLE hMap;
	PVOID  hView;
	DWORD  mapSize;
} FILE_MAP, *PFILE_MAP;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNICODE
#define FileMapOpen FileMapOpenW
#define FileMapCreate FileMapCreateW
#else
#define FileMapOpen FileMapOpenA
#define FileMapCreate FileMapCreateA
#endif

DLL_FILE_UTILS_API BOOL FileMapOpenW(PFILE_MAP out, LPCWSTR file);
DLL_FILE_UTILS_API BOOL FileMapCreateW(PFILE_MAP out, LPCWSTR file, DWORD fileSize);
DLL_FILE_UTILS_API BOOL FileMapOpenA(PFILE_MAP out, LPCSTR file);
DLL_FILE_UTILS_API BOOL FileMapCreateA(PFILE_MAP out, LPCSTR file, DWORD fileSize);
DLL_FILE_UTILS_API BOOL FileMapTemp(PFILE_MAP out, DWORD fileSize);
DLL_FILE_UTILS_API BOOL FileMapClose(PFILE_MAP in);

#ifdef __cplusplus
}
#endif

#endif/*_FILE_UTILS_H*/