#ifndef _PRINT_LAST_ERROR_H
#define _PRINT_LAST_ERROR_H

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef DEBUG
#define PRESS_TO_GO(s) do {									\
	_tprintf(_T("%s\nPress enter to continue.\n"), s); _gettchar();	\
} while (0)
#else
#define PRESS_TO_GO(s)
#endif														

#ifdef DEBUG
#define PRESS_TO_FINISH(s) do {							\
	_tprintf(_T("%s\nPress enter to finish.\n"), s); _gettchar();	\
} while (0)
#else
#define PRESS_TO_FINISH(s)
#endif


#define PrintLastError() do {	\
	LPTSTR pBuf = NULL;						\
	DWORD dwLastError = GetLastError();	\
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, 0, (LPTSTR)&pBuf, 0, NULL)) \
		_tprintf(_T("LastError (%d) format msg = %s"), dwLastError, pBuf); \
	else								\
		_tprintf(_T("Format message failed with 0x%x error.\n"), GetLastError()); \
	LocalFree((HLOCAL)pBuf);			\
} while (0)

#define PrintLastErrorAndExit() { \
	PrintLastError();		   \
	exit(0);				   \
}

#endif/*_PRINT_LAST_ERROR_H*/