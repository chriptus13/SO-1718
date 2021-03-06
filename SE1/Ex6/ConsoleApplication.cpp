#include "stdafx.h"
#include "BMPRotation.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define tchar_max sizeof(TCHAR)*CHAR_MAX

void execute(PTCHAR type, PTCHAR in, PTCHAR out) {
	if (_tcscmp(_T("HORIZONTAL"), type)==0)BMP_Flip(in, out, HORIZONTAL);
	else if (_tcscmp(_T("VERTICAL"), type)==0)BMP_Flip(in, out, VERTICAL);
	else {
		printf("Invalid Orientation given \nPlease Try HORIZONTAL or VERTICAL \nPress any keys to exit");
		_getch();
	}
}

int _tmain(int argc, PTCHAR argv[]) {
	//argv version
	if (argc == 4) execute(argv[1], argv[2], argv[3]);
	//console version
	else {
		int c = 0;
		PTCHAR  ori= (TCHAR*)malloc(tchar_max);
		PTCHAR  in = (TCHAR*)malloc(tchar_max);
		PTCHAR  out = (TCHAR*)malloc(tchar_max);
		printf("Insert the orientation that you pretend (HORIZONTAL OR VERTICAL) \n");
		//Microsoft C implementation change
		fgets(ori, tchar_max, stdin);
		printf("Insert the name of the input file (name.bmp) \nPlease make sure that it is in the same directory of the executable \n");
		fgets(in, tchar_max, stdin);
		printf("Insert the name of the output file (name.bmp) \nMake sure that there isn't no other file with the same name in the application folder \n");
		fgets(out, tchar_max, stdin);
		//fgets keeps EOF in file so removal,Maybe a bug 
		if (ori[strlen(ori) - 1] == '\n') ori[strlen(ori) - 1] = '\0';
		if (in[strlen(in) - 1] == '\n') in[strlen(in) - 1] = '\0';
		if (out[strlen(out) - 1] == '\n') out[strlen(out) - 1] = '\0';
		execute(_T(ori), _T(in), _T(out));
		free(ori);
		free(in);
		free(out);
	}
	return 0;
}
