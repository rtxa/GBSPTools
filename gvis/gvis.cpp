/****************************************************************************************/
/*  gvis.cpp
/*
/*  Author: rtxa
/*  Description: Creates VIS for BSP file
/*
/*	Generates the visibility matrix (specifies which polygons the player can't or might be able to see)
/*	for the map and helps speed up its rendering.
/*
/****************************************************************************************/

#include <stdio.h>
#include <windows.h> // needed for "LoadLibrary" declarations, get current directory, etc.
#include "gvis.h"
#include "gbsplib.h"
#include "gbsptools.h"

int main(int argc, char *argv[]) {
	printf("gvis v%.1f (%s)\n", GBSPTOOLS_VERSION, __DATE__);
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

	// finished loading dll, begin with VIS compilation
	char bspFullPath[MAX_PATH];
	sprintf_s(bspFullPath, "%s\\%s%s", path, compParms.mapName, ".bsp");

	printf("---- %s ----\n", "BEGIN gvis");
	
	if (compFHook->GBSP_VisGBSPFile(bspFullPath, &compParms.vis) == GBSP_ERROR) {
		fprintf(stderr, "Warning: GBSP_VisGBSPFile failed for file : %s, GBSPLib.Dll.\n", bspFullPath);
		return COMPILER_ERROR_BSPFAIL;
	}

	printf("---- %s ----\n\n\n\n", "END gvis");

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
			parms->vis.Verbose = GE_TRUE;
			printf(" -verbose");
		}
		else if (!strcmp(argv[i], "-full")) {
			parms->vis.FullVis = GE_TRUE;
			printf(" -full");
		}
		else if (!strcmp(argv[i], "-sortportals")) {
			parms->vis.SortPortals = GE_FALSE;
			printf(" -sortportals");
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
	printf("\n--- gvis Options ---\n");
	printf("    %-20s : %s\n", "mapname",		"The .map file to process.");
	printf("    %-20s : %s\n", "-verbose",		"Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-full",			"Performs full visibility calculations. Use it only in final compiles.");
	printf("    %-20s : %s\n", "-sortportals",	"Sort the portals with MightSee.");
	printf("\n");
	exit(0);
};
