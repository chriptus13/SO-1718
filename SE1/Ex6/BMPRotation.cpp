// BMPRotate.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "BMPRotation.h"

#define BytesPerPixel sizeof(RGBTRIPLE)


static void WINAPI PrintLastError() {
	LPTSTR pBuf;
	DWORD dwLastError = GetLastError();
	_tprintf(_T("GetLastError = %d\n"), dwLastError);
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError, 0, (LPTSTR)&pBuf, 0, NULL))
		_tprintf(_T("Format message failed with 0x%x\n"), GetLastError());
	_tprintf(pBuf);
	LocalFree((HLOCAL)pBuf);
	exit(0);
}

static PRGBTRIPLE WINAPI getPixel(PCHAR start, DWORD row, DWORD col, DWORD totalRows, DWORD totalCols, DWORD padding) {
	DWORD aux = (totalRows - row - 1)*(totalCols * 3 + padding) + col * 3;
	PRGBTRIPLE pixel = (PRGBTRIPLE)(start + aux);
	return pixel;
}

static void WINAPI flipHorizontal(PCHAR src, PCHAR dst, DWORD cols, DWORD rows) {
	DWORD nPadding = (4 - ((3 * cols) % 4)) % 4;
	PBITMAPFILEHEADER fileHeader = (PBITMAPFILEHEADER)src;
	PCHAR srcStartPxl = src + fileHeader->bfOffBits;

	memcpy(dst, src, fileHeader->bfOffBits);
	PCHAR dstPxl = dst + fileHeader->bfOffBits;
	for (int i = rows - 1; i >= 0; i--, dstPxl += nPadding)
		for (int j = 0; j < cols; j++, dstPxl += BytesPerPixel)
			memcpy(dstPxl, getPixel(srcStartPxl, i, cols - j - 1, rows, cols, nPadding), BytesPerPixel);
}

static void WINAPI flipVertical(PCHAR src, PCHAR dst, DWORD cols, DWORD rows) {
	DWORD nPadding = (4 - ((3 * cols) % 4)) % 4;
	PBITMAPFILEHEADER fileHeader = (PBITMAPFILEHEADER)src;

	memcpy(dst, src, fileHeader->bfOffBits);
	src += fileHeader->bfOffBits;
	dst += fileHeader->bfOffBits;
	for (int currRow = 0; currRow < rows; currRow++)
		memcpy(dst + currRow * (cols * BytesPerPixel + nPadding), src + (rows - 1 - currRow) * (cols * BytesPerPixel + nPadding), cols * BytesPerPixel);
}


static void WINAPI flipBMP(PCHAR src, PCHAR dst, DWORD cols, DWORD rows, DWORD orientation) {
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

void BMP_Flip(PTCHAR filenameIn, PTCHAR filenameOut, FLIP_enum_t type) {
	HANDLE hFile = CreateFile(filenameIn, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) PrintLastError();

	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap == NULL) PrintLastError();

	PCHAR imgAddr = (PCHAR)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

	PBITMAPFILEHEADER fileHeader = (PBITMAPFILEHEADER)imgAddr;
	DWORD fileSize = fileHeader->bfSize;
	PBITMAPINFOHEADER infoHeader = (PBITMAPINFOHEADER)(imgAddr + sizeof(BITMAPFILEHEADER));
	DWORD cols = infoHeader->biWidth;
	DWORD rows = infoHeader->biHeight;

	HANDLE hFileRes = CreateFile(filenameOut, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileRes == INVALID_HANDLE_VALUE) PrintLastError();

	HANDLE hMapRes = CreateFileMapping(hFileRes, NULL, PAGE_READWRITE, 0, fileSize, NULL);
	if (hMapRes == NULL) PrintLastError();

	PCHAR imgResAddr = (PCHAR)MapViewOfFile(hMapRes, FILE_MAP_WRITE, 0, 0, 0);

	flipBMP(imgAddr, imgResAddr, cols, rows, (DWORD)type);

	UnmapViewOfFile(imgAddr);
	UnmapViewOfFile(imgResAddr);
	CloseHandle(hMap);
	CloseHandle(hFile);
	CloseHandle(hMapRes);
	CloseHandle(hFileRes);

}

