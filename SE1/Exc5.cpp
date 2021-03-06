#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>

#define BytesPerPixel sizeof(RGBTRIPLE)

// Function that prints LastError and exits the program
static VOID WINAPI PrintLastError() {
	LPTSTR pBuf;
	DWORD dwLastError = GetLastError();
	_tprintf(_T("GetLastError = %d\n"), dwLastError);
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, 0, (LPTSTR)&pBuf, 0, NULL))
		_tprintf(_T("Format message failed with 0x%x\n"), GetLastError());
	_tprintf(pBuf);
	LocalFree((HLOCAL)pBuf);
	exit(0);
}

// Function that gets the pointer to the RGBTRIPLE for the 
// BMP File mapped in the 'start' address with 'totalRows', 'totalCols' and 'padding'
// at the 'row' and 'col' position
static PRGBTRIPLE WINAPI getPixel(PCHAR start, DWORD row, DWORD col, DWORD totalRows, DWORD totalCols, DWORD padding) {
	DWORD aux = (totalRows - row - 1)*(totalCols * 3 + padding) + col * 3;
	PRGBTRIPLE pixel = (PRGBTRIPLE)(start + aux);
	return pixel;
}

// Functions that flips horizontally the 
// BMP File mapped in the 'src' address with 'cols' and 'rows'
// into the BMP File mapped in the 'dst' address
static void WINAPI flipHorizontal(PCHAR src, PCHAR dst, DWORD cols, DWORD rows) {
	DWORD nPadding = (4 - ((3 * cols) % 4)) % 4;
	PBITMAPFILEHEADER fileHeader = (PBITMAPFILEHEADER)src;
	PCHAR srcStartPxl = src + fileHeader->bfOffBits;
	printf("No Pixel loading \n");

	_getch();

	memcpy(dst, src, fileHeader->bfOffBits);
	PCHAR dstPxl = dst + fileHeader->bfOffBits;
	for (int i = rows - 1; i >= 0; i--, dstPxl += nPadding)
		for (int j = 0; j < cols; j++, dstPxl += BytesPerPixel)
			memcpy(dstPxl, getPixel(srcStartPxl, i, cols - j - 1, rows, cols, nPadding), BytesPerPixel);
}

// Functions that flips vertically the 
// BMP File mapped in the 'src' address with 'cols' and 'rows'
// into the BMP File mapped in the 'dst' address
static void WINAPI flipVertical(PCHAR src, PCHAR dst, DWORD cols, DWORD rows) {
	DWORD nPadding = (4 - ((3 * cols) % 4)) % 4;
	PBITMAPFILEHEADER fileHeader = (PBITMAPFILEHEADER)src;

	memcpy(dst, src, fileHeader->bfOffBits);
	src += fileHeader->bfOffBits;
	dst += fileHeader->bfOffBits;
	for (int currRow = 0; currRow < rows; currRow++)
		memcpy(dst + currRow * (cols * BytesPerPixel + nPadding), src + (rows - 1 - currRow) * (cols * BytesPerPixel + nPadding), cols * BytesPerPixel);
}

// Functions that flips the 
// BMP File mapped in the 'src' address with 'cols' and 'rows'
// into the BMP File mapped in the 'dst' address
// according with the 'orientation'
void WINAPI flipBMP(PCHAR src, PCHAR dst, DWORD cols, DWORD rows, int orientation) {
	switch (orientation) {
	case 0:
		flipHorizontal(src, dst, cols, rows);
		break;
	case 1:
		flipVertical(src, dst, cols, rows);
		break;
	default:
		printf("Invalid orientation!\n");
	}
}

int _tmain(int argc, PTCHAR argv[]) {
	const PTCHAR rotation = argv[1];
	const PTCHAR in = argv[2];
	const PTCHAR out = argv[3];

	int orientation = -1;

	if (_tcscmp(rotation, _T("HORIZONTAL")) == 0) orientation = 0;
	else if (_tcscmp(rotation, _T("VERTICAL")) == 0) orientation = 1;
	else PrintLastError();

	HANDLE hFile = CreateFile(in, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) PrintLastError();

	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap == NULL) PrintLastError();

	PCHAR imgAddr = (PCHAR)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

	PBITMAPFILEHEADER fileHeader = (PBITMAPFILEHEADER)imgAddr;
	DWORD fileSize = fileHeader->bfSize;
	PBITMAPINFOHEADER infoHeader = (PBITMAPINFOHEADER)(imgAddr + sizeof(BITMAPFILEHEADER));
	DWORD cols = infoHeader->biWidth;
	DWORD rows = infoHeader->biHeight;

	HANDLE hFileRes = CreateFile(out, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileRes == INVALID_HANDLE_VALUE) PrintLastError();

	HANDLE hMapRes = CreateFileMapping(hFileRes, NULL, PAGE_READWRITE, 0, fileSize, NULL);
	if (hMapRes == NULL) PrintLastError();

	PCHAR imgResAddr = (PCHAR)MapViewOfFile(hMapRes, FILE_MAP_WRITE, 0, 0, 0);

	flipBMP(imgAddr, imgResAddr, cols, rows, orientation);
	
	UnmapViewOfFile(imgAddr);
	UnmapViewOfFile(imgResAddr);
	CloseHandle(hMap);
	CloseHandle(hFile);
	CloseHandle(hMapRes);
	CloseHandle(hFileRes);

	return 0;
}