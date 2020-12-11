
#include "../Include/app.h"

DWORD main(DWORD argc, char *argv[]) {

	if (argc != 4) {
		printf("Use: %s <pathname in> <pathname out> <operation>\n"
					"\tpathname in: pathname to file to analyse\n"
			        "\tpathname out: pathname to search for outputs resulting of flip operation\n"
					"\toperation: VER | HOR\n", argv[0]);
		return 1;
	}

	PCSTR operArg = argv[3];
	PCSTR pathnameIn = argv[1];
	PCSTR pathnameOut = argv[2];
	FLIP_enum_t flipType = _tcscmp((LPTCH)operArg, _T("HOR")) == 0 ? FLIP_HORIZONTALLY : FLIP_VERTICALLY;

	LIST_ENTRY sent;

	INT res = BMP_GetFlipsOfFilesInRefDir(pathnameIn, pathnameOut, FLIP_HORIZONTALLY, &sent);
	if (res != OPER_SUCCESS) {
		printf("Error %d on match operation!\n", res);
		return 1;
	}
	
	//Show results and free allocated memory

	while (IsListEmpty(&sent) == false) {
		PRES_NODE res = (PRES_NODE)CONTAINING_RECORD(RemoveHeadList(&sent), RES_NODE, link);
		if (!IsListEmpty(&(res->files))) {
			printf("\nMatched files for reference image %s:\n", res->filename);
			do {
				PFILENAME_NODE res2 =
					(PFILENAME_NODE)CONTAINING_RECORD(RemoveHeadList(&(res->files)), FILENAME_NODE, link);
				PSTR res2Name = res2->filename;
				printf("\t%s\n", res2Name);
				free(res2Name);
				free(res2);
				
			} while (!IsListEmpty(&res->files));
		}
		free(res);
	}

	PRESS_TO_FINISH("\nDone!");
	return 0;
}
