/****************************************************************************************/
/*  gbsp.cpp																			
/*                                                                                      
/*  Author: rtxa																		
/*  Description: Creates a BSP file from a MAP file
/*                                                                                      
/****************************************************************************************/

#include <stdio.h>
#include <windows.h> // needed for "LoadLibrary" declarations, get current directory, etc.
#include "gbsp.h"
#include "gbsplib.h"
#include "gbsptools.h"

int main(int argc, char *argv[]) {
	printf("gbsp v%.1f (%s)\n", GBSPTOOLS_VERSION, __DATE__);
	printf("Genesis 3D BSP Tools - Made by %s\n", GBSPTOOLS_AUTHOR);
	printf("Check readme.md for more info abouts these tools.\n");
	printf("Submit detailed bug reports to %s\n", GBSPTOOLS_CONTACT);

	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	printf("Command line: \"%s\"\n", path);

	CompilerParms compParms;
	InitCompilerParms(&compParms);
	ParseCmdArgs(argc, argv, &compParms);

	// load gbsplib.dll to access the map compiler functions
	HINSTANCE compHandle;
	GBSP_FuncHook* compFHook;

	CompilerErrorEnum result = Compiler_LoadCompilerDLL(compFHook, compHandle, Compiler_ErrorfCallback, Compiler_PrintfCallback);

	if (result != CompilerErrorEnum::COMPILER_ERROR_NONE) {
		return result;
	}

	// finished loading dll, begin with BSP compilation
	char mapFullPath[MAX_PATH];
	sprintf_s(mapFullPath, "%s\\%s%s", path, compParms.mapName, ".map");

	char bspFullPath[MAX_PATH];
	sprintf_s(bspFullPath, "%s\\%s%s", path, compParms.mapName, ".bsp");

	printf("---- %s ----\n", "BEGIN gbsp");

	if (compParms.updateEnts == GE_TRUE) {
		if (compFHook->GBSP_UpdateEntities(mapFullPath, bspFullPath) == GE_TRUE) {
			printf("---- %s ----\n\n\n\n", "END gbsp");
			return COMPILER_ERROR_NONE;
		} else {
			fprintf(stderr, "Compile Failed:  GBSP_UpdateEntities returned an error, GBSPLib.Dll.\n");
			printf("---- %s ----\n\n\n\n", "END gbsp");
			return COMPILER_ERROR_BSPFAIL;
		}
	}

	GBSP_RETVAL gbspResult = compFHook->GBSP_CreateBSP(mapFullPath, &compParms.bsp);
	if (gbspResult == GBSP_ERROR) {
		fprintf(stderr, "Compile Failed: GBSP_CreateBSP encountered an error, GBSPLib.Dll.\n.");
		printf("---- %s ----\n\n\n\n", "END bsp");
		return COMPILER_ERROR_BSPFAIL;
	}

	gbspResult = compFHook->GBSP_SaveGBSPFile(bspFullPath);
	if (gbspResult == GBSP_ERROR) {
		fprintf(stderr, "Compile Failed: GBSP_SaveGBSPFile for file: %s, GBSPLib.Dll.\n", bspFullPath);
		printf("---- %s ----\n\n\n\n", "END gbsp");
		return COMPILER_ERROR_BSPSAVE;
	}

	compFHook->GBSP_FreeBSP();

	printf("---- %s ----\n\n\n\n", "END gbsp");

	FreeLibrary(compHandle);

	return COMPILER_ERROR_NONE;
}

//========================================================================================
//	ParseCmdArgs()
//	This parse command line arguments for load them into compiler parameters
//========================================================================================
void ParseCmdArgs(int argc, char *argv[], CompilerParms *parms) {
	if (argc < 2)
		ShowUsage();

	geBoolean hasLoadMap = GE_FALSE;

	printf("Arguments:");
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-verbose")) {
			parms->bsp.Verbose = GE_TRUE;
			printf(" -verbose");
		} else if (!strcmp(argv[i], "-entverbose")) {
			parms->bsp.EntityVerbose = GE_TRUE;
			printf(" -entverbose");
		} else if (!strcmp(argv[i], "-updateents")) {
			parms->updateEnts = GE_TRUE;
			printf(" -updateents");
		} else {
			if (!hasLoadMap) {
				strcpy_s(parms->mapName, argv[i]);
				printf(" %s", parms->mapName);
				hasLoadMap = GE_TRUE;
			} else {
				ShowUsage();
			}
		}
	}
	if (!hasLoadMap) 
		ShowUsage();
	printf("\n");
}

//========================================================================================
// ShowUsage()
// This shows information about the compiler commands
//========================================================================================
void ShowUsage(void) {
	printf("\n--- gbsp Options ---\n");
	printf("    %-20s : %s\n", "mapname",		"The .map file to process.");
	printf("    %-20s : %s\n", "-verbose",		"Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-entverbose",	"Outputs detailed entity information.");
	printf("    %-20s : %s\n", "-updateents",	"Do an entity update from .map to .bsp.");
	printf("\n");
	exit(0);
};
