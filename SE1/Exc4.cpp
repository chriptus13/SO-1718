#include "stdafx.h"
#include <windows.h>
#include <psapi.h>
#include <conio.h>


#define THRESHOLD 1 << 29	// 512MB
#define ByteToMB(x) x >> 20	// Byte to MegaByte converter

// Function that returns the size of a Page for the current System
DWORD WINAPI GetPageSize() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwPageSize;
}

// Function that checks if the process with the 'pid' id 
// has any memory leaks (if size of total private pages is bigger than the THRESHOLD)
// each 5 seconds
void WINAPI CheckMemLeaks(DWORD pid) {
	const DWORD PAGESIZE = GetPageSize();
	printf("Page Size = %d Bytes\n", PAGESIZE);

	HANDLE currHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);
	if(currHandle == NULL) {
		printf("\nError! No process found with that id! Press ENTER to exit!\n");
		_getch();
		return;
	}

	TCHAR buffer[MAX_PATH];
	GetModuleFileNameEx(currHandle, 0, buffer, MAX_PATH);
	_tprintf(_T("The current process path is : %s\n\n"), buffer);

	PSAPI_WORKING_SET_INFORMATION wsi_1, *wsi;
	DWORD wsi_size;

	do {
		QueryWorkingSet(currHandle, &wsi_1, sizeof(wsi));
		wsi_size = wsi_1.NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK) + sizeof(PSAPI_WORKING_SET_INFORMATION);
		wsi = (PSAPI_WORKING_SET_INFORMATION *) malloc(wsi_size);

		if(!QueryWorkingSet(currHandle, wsi, wsi_size)) {
			printf("QueryWorkingSet failed: %lu\nPress ENTER to exit!", GetLastError());
			CloseHandle(currHandle);
			_getch();
			break;
		}

		DWORD currPages = 0;
		for(DWORD i = 0; i < wsi->NumberOfEntries; i++) if(wsi->WorkingSetInfo[i].Shared) currPages++;
		DWORD workingSet_size = currPages * PAGESIZE;

		printf("The current process has %d private pages and it occupies %d MB\n", currPages, ByteToMB(workingSet_size));

		if(workingSet_size > THRESHOLD) {
			CloseHandle(currHandle);
			printf("The current process has execeded the threshold! Press ENTER to exit!\n");
			_getch();
			break;
		}

		Sleep(5000);
	} while(1);

	free(wsi);
}

int main() {
	DWORD id;
	printf("Insert the process id that you want to analyze: \n");
	scanf_s("%d", &id);

	CheckMemLeaks(id);

	return 0;
}