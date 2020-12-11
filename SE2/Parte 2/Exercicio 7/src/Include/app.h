#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <process.h>
#include "../Include/PrintUtils.h"
#include "../Include/BmpUtils.h"
#include "../Include/FileUtils.h"
#include "../Include/List.h"

// Operation Errors
#define OPER_TRAVERSE_ERROR	1
#define OPER_MAP_ERROR		2
#define OPER_SUCCESS		0

INT BMP_GetFlipsOfFilesInRefDir(PCSTR pathRefFiles, PCSTR pathOutFiles, FLIP_enum_t flipType, PLIST_ENTRY res);

// Node for matching transformed files list
typedef struct {
	PSTR filename;		// Flip prodution filename
	LIST_ENTRY link;	// Link list
} FILENAME_NODE, *PFILENAME_NODE;

// Node for final result list
typedef struct {
	PSTR filename;		// Original filename
	LIST_ENTRY files;	// Sentinel of list of results
	LIST_ENTRY link;	// Link list
} RES_NODE, *PRES_NODE;

