#include "../Include/app.h"
#include "../Include/traversedir.h"


static BOOL hasExtension(LPCSTR fileName, LPCSTR extension) {
	char *strend = (char *) fileName + strlen(fileName) - 1;
	char *p = strend;
	while (p >= fileName && *p != '.') --p;
	if (p < fileName) return FALSE;
	while (p <= strend && *extension != 0 && *p == *extension) {
		p++;
		extension++;
	}
	return *extension == 0;
}

/**
 *	Traverses recursivelly a folder and executes an action per found file
 *	For simplicity, it deals only with ascii char names
 *	Parameters:
 *		dir			- the directory to traverse
 *		action		- the pointer to executed code per file
 *		arg			- the argument passed to action
 *		extension   - the filter to select files to process (ex: ".bmp")
 **/
BOOL TraverseDirTree(LPCSTR dir, LPCSTR extension, ACTION  action, LPVOID arg) {

	CHAR buffer[MAX_PATH];		// auxiliary buffer
								// the buffer is needed to define a match string that guarantees 
								// a priori selection for all files
	sprintf_s(buffer, "%s\\%s", dir, "*.*");

	WIN32_FIND_DATA fileData;
	HANDLE fileIter = FindFirstFile(buffer, &fileData);
	if (fileIter == INVALID_HANDLE_VALUE) return FALSE;

	// Iterate through current directory and sub directories
	do {
		sprintf_s(buffer, "%s/%s", dir, fileData.cFileName);
		if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			// Not processing "." and ".." files!
			if (strcmp(fileData.cFileName, ".") && strcmp(fileData.cFileName, "..")) {
				// Recursively process child directory
				TraverseDirTree(buffer, extension, action, arg);
			}
		}
		else {
			// Process only  archives with selected extension
			if (hasExtension(buffer,extension))
				if (!action(buffer, arg)) break;
		}
	} while (FindNextFile(fileIter, &fileData) == TRUE);

	FindClose(fileIter);
	return TRUE;
}