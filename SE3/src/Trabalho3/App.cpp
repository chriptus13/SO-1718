// SE3.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "stdafx.h"
#include "AsyncCopyLib.h"

VOID CallBackCopyFolder(LPVOID userCtx, DWORD status, UINT64 transferedBytes) {
	if (status != 0) {
		DWORD *x = (DWORD *)userCtx;
		*x++;
	}
}

DWORD CopyFolder(PCTSTR origFolder, PCTSTR dstFolder) {
	AsyncInit();

	TCHAR buffer[MAX_PATH];
	TCHAR bufferDst[MAX_PATH];


	_stprintf_s(buffer, _T("%s\\%s"), origFolder, _T("*.*"));

	WIN32_FIND_DATA fileData;
	HANDLE fileIter = FindFirstFile(buffer, &fileData);
	if (fileIter == INVALID_HANDLE_VALUE) return 0;

	// Iterate through current directory

	DWORD counter = 0;
	do {

		_stprintf_s(buffer, _T("%s/%s"), origFolder, fileData.cFileName);
		if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			_stprintf_s(bufferDst, _T("%s/%s"), dstFolder, fileData.cFileName);

			if (!CopyFileAsync(buffer, bufferDst, &CallBackCopyFolder, &counter)) counter++;
		}
	} while (FindNextFile(fileIter, &fileData));

	FindClose(fileIter);
	AsyncTerminate();
	return counter;
}

int main() {
	DWORD res = CopyFolder("../FolderSrc","../FolderDst");
	printf("%d\n", res);
	printf("Press any key to end ...");
	getchar();
	return 0;
}

