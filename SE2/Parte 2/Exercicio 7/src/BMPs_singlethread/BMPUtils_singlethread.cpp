
#include "../Include/app.h"
#include "../Include/traversedir.h"



// contexts for traverse folders

// the global context
/*typedef struct  {
	LPCSTR pathOutFiles;	// The root dir of transformed files
	PLIST_ENTRY resultList;	// A list of matched reference file. 
							// Each reference file contains the list of matching transformed files.
	FLIP_enum_t flipType;	// The flip type to match
	INT errorCode;			// 0(OPER_SUCCESS) means the operation concludes successfully
} MUTATIONS_RESULT_CTX, *PMUTATIONS_RESULT_CTX;

// the per reference file context
typedef struct {
	PMUTATIONS_RESULT_CTX global; // pointer to global context
	PFILE_MAP refMap;			  // the  (transformed) reference file mapping
	LIST_ENTRY fileMatchingList;  // the list for identical transformed files
} REFIMAGE_MUTLIST_CTX, *PREFIMAGE_MUTLIST_CTX;


static VOID InitGlobalCtx(PMUTATIONS_RESULT_CTX ctx, LPCSTR pathOutFiles, 
								 FLIP_enum_t flipType, PLIST_ENTRY result) {
	ctx->pathOutFiles = pathOutFiles;
	ctx->flipType = flipType;
	InitializeListHead(result);
	ctx->resultList = result;
	ctx->errorCode = OPER_SUCCESS; // optimistic initialization
}

static VOID InitRefImageMutListCtx(PREFIMAGE_MUTLIST_CTX ctx, PFILE_MAP refMap, 
									PMUTATIONS_RESULT_CTX global) {
	ctx->refMap = refMap;
	ctx->global = global;
	InitializeListHead(&ctx->fileMatchingList);
}

FORCEINLINE static VOID OperMarkError(PMUTATIONS_RESULT_CTX ctx, INT code) {
	ctx->errorCode = code;
}

FORCEINLINE static BOOL OperHasError(PMUTATIONS_RESULT_CTX ctx) {
	return ctx->errorCode != OPER_SUCCESS;
}

*/
/*
 * Called for each found bmp in transformed files dir
 * Parameters:
 *	filePath - the transformed file path
 *	_ctx     - a pointer to the per ref file context structure
 **/
/*BOOL ProcessTransformedFile(LPCSTR filePath, LPVOID _ctx) {
	PREFIMAGE_MUTLIST_CTX ctx = (PREFIMAGE_MUTLIST_CTX)_ctx;
	FILE_MAP fileMap;
	
	if (!FileMapOpen(&fileMap, filePath)) {
		OperMarkError(ctx->global, OPER_MAP_ERROR);
		return FALSE;
	}
	 	
	// Insert filename into list if equals to temporary flip result
	if (fileMap.mapSize == ctx->refMap->mapSize &&
		memcmp(ctx->refMap->hView, fileMap.hView, fileMap.mapSize) == 0) {
		PFILENAME_NODE newNode = (PFILENAME_NODE)malloc(sizeof(FILENAME_NODE));
		newNode->filename = _strdup(filePath);
		InsertTailList(&ctx->fileMatchingList, &newNode->link);
	}
	FileMapClose(&fileMap);
	return TRUE;
}
*/
/*
 * Called for each found bmp in reference files dir
 * Parameters:
 *	filePath - the reference file path
 *	_ctx     - a pointer to the global operation context structure
 **/
/*BOOL BMP_GetFlipsOfRefFile(LPCSTR filePath, LPVOID _ctx) {
	PMUTATIONS_RESULT_CTX global = (PMUTATIONS_RESULT_CTX)_ctx;
	
	printf("BMPUtils_singlethread.BMP_get_flip_reprodutions_of_file: \n"
		"\tFile to analyse = \"%s\"\n"
		"\tDirectory of files to search for reprodutions = \"%s\"\n"
		"\tFlip type = %s\n", filePath, global->pathOutFiles, 
							  global->flipType == FLIP_VERTICALLY ? "Flip vertically" : "Flip horizontally");

								
	// Map file into process address space and create flip view
	FILE_MAP refFileMap, flipFileMap;
	BOOL ret = TRUE;

	// map reference file
	if (!FileMapOpen(&refFileMap, filePath)) {
		OperMarkError(global, OPER_MAP_ERROR);
		return FALSE;
	}

	// do the flip to a temprary map
	if (!FileMapTemp(&flipFileMap, refFileMap.mapSize)) {
		OperMarkError(global, OPER_MAP_ERROR);
		FileMapClose(&refFileMap);
		return FALSE;
	}
		
	// do the selected flip
	BMP_FlipMem((PUCHAR)refFileMap.hView, (PUCHAR)flipFileMap.hView, global->flipType);

	REFIMAGE_MUTLIST_CTX ctx;
	InitRefImageMutListCtx(&ctx, &flipFileMap, global);
	
	// Iterate through pathOutFiles directory and sub directories,
	// invoking de processor (ProcessTransformedFile) for each transformed file found
	if (!TraverseDirTree(global->pathOutFiles, ".bmp", ProcessTransformedFile, &ctx)) {
		if (!OperHasError(global))
			OperMarkError(global, OPER_TRAVERSE_ERROR);
	}
	if (OperHasError(global)) {
		ret = FALSE;
		goto terminate;
	}
		
	if (!IsListEmpty(&ctx.fileMatchingList)) {
		// Accumulate filename list to final result
		PRES_NODE newNode = (PRES_NODE)malloc(sizeof(RES_NODE));
		newNode->filename = _strdup(filePath);
		InitializeListHead(&newNode->files);
		InsertRangeTailList(&newNode->files, &ctx.fileMatchingList);
		InsertTailList(global->resultList, &newNode->link);
	}
terminate:
	// Cleanup file resources
	FileMapClose(&refFileMap);
	FileMapClose(&flipFileMap);
	return ret;
}

*/
/*
 *  Recursively traverses the folder with the reference images and 
 *  find matches in transformed files dir
 *	Parameters:
 *		pathRefFiles - the reference files directory
 *		pathOutFiles - the transformed files directory
 *		flipType	 - what transformation to match (FLIP_HORIZONTALLY or FLIP_VERTICALLY)
 *		res			 - sentinel node of result list
 */
/*INT BMP_GetFlipsOfFilesInRefDir(LPCSTR pathRefFiles, LPCSTR pathOutFiles, FLIP_enum_t flipType, PLIST_ENTRY res) {

	MUTATIONS_RESULT_CTX ctx;
	InitGlobalCtx(&ctx, pathOutFiles, flipType, res);

	// Iterate through pathRefFiles directory and sub directories
	// invoking de processor (BMP_GetFlipsOfRefFile) for each ref file
	if (!TraverseDirTree(pathRefFiles, ".bmp", BMP_GetFlipsOfRefFile, &ctx)) {
		if (!OperHasError(&ctx))
			OperMarkError(&ctx, OPER_TRAVERSE_ERROR);
	}
	return ctx.errorCode;
}*/