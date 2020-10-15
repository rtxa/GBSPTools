/****************************************************************************************/
/*  gbspandvis.cpp																			
/*                                                                                      
/*  Author: rtxa																		
/*  Description: Creates a BSP file from a MAP file or VIS a BSP file.
/*                                                                                      
/****************************************************************************************/

#include <stdio.h>
#include <windows.h> // needed for "LoadLibrary" declarations, get current directory, etc.
#include "gbspandvis.h"
#include "gbsplib.h"
#include "gbsptools.h"
#include "utils.h"

int main(int argc, char *argv[]) {
	printf("gbspandvis v%.1f (%s)\n", GBSPTOOLS_VERSION, __DATE__);
	printf("Genesis 3D BSP Tools - Author: %s\n", GBSPTOOLS_AUTHOR);
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

	std::string mapPath(compParms.mapName);

	std::string bspPath;

	// create destination name if not specified
	if (!compParms.bspName[0]) {
		bspPath = std::string(mapPath);
		GBSPTools::StripExtension(bspPath);
		bspPath.append(".bsp");
	} else {
		bspPath = std::string(compParms.bspName);
	}

	// convert path to unix so GBSPLib can read it correctly
	GBSPTools::PathToUnix(mapPath);
	GBSPTools::PathToUnix(bspPath);

	// use this extension if specified files don't provide them
	GBSPTools::DefaultExtension(mapPath, ".map");
	GBSPTools::DefaultExtension(bspPath, ".bsp");

	ShowSettings(compParms);

	// Begin with GBSP

	if (compParms.updateEnts == GE_TRUE) {
		if (compFHook->GBSP_UpdateEntities(mapPath.c_str(), bspPath.c_str()) == GE_TRUE) {
			return COMPILER_ERROR_NONE;
		} else {
			fprintf(stdout, "Compile Failed:  GBSP_UpdateEntities returned an error, GBSPLib.Dll.\n");
			return COMPILER_ERROR_BSPFAIL;
		}
	}

	compFHook->GBSP_FreeBSP();

	GBSP_RETVAL gbspResult = compFHook->GBSP_CreateBSP(mapPath.c_str(), &compParms.bsp);
	if (gbspResult == GBSP_ERROR) {
		fprintf(stdout, "Compile Failed: GBSP_CreateBSP encountered an error, GBSPLib.Dll.\n");
		return COMPILER_ERROR_BSPFAIL;
	}

	gbspResult = compFHook->GBSP_SaveGBSPFile(bspPath.c_str());
	if (gbspResult == GBSP_ERROR) {
		fprintf(stdout, "Compile Failed: GBSP_SaveGBSPFile for file: %s, GBSPLib.Dll.\n", bspPath.c_str());
		return COMPILER_ERROR_BSPSAVE;
	}


	printf("\n");

	ShowSettingsVis(compParms);

	if (compFHook->GBSP_VisGBSPFile(bspPath.c_str(), &compParms.vis) == GBSP_ERROR) {
		fprintf(stderr, "Warning: GBSP_VisGBSPFile failed for file : %s, GBSPLib.Dll.\n", bspPath.c_str());
		return COMPILER_ERROR_BSPFAIL;
	}

	FreeLibrary(compHandle);

	return COMPILER_ERROR_NONE;
}

//========================================================================================
//	ParseCmdArgs()
//	This parses command line arguments to load them into the compiler parameters
//========================================================================================
void ParseCmdArgs(int argc, char *argv[], CompilerParms *parms) {
	if (argc < 2)
		ShowUsage();

	bool hasLoadMap = false;
	bool hasLoadBsp = false;

	bool readingBsp = false;
	bool readingVis = false;

	printf("Arguments:");
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-gbsp")) {
			readingBsp = true;
			readingVis = false;
			printf(" -gbsp");
			continue;
		} else if (!strcmp(argv[i], "-gvis")) {
			readingBsp = false;
			readingVis = true;
			printf(" -gvis");
			continue;
		}

		// reading paths to .map and .bsp
		if (argv[i][0] != '-') {
			if (!hasLoadMap) {
				strcpy_s(parms->mapName, argv[i]);
				printf(" %s", parms->mapName);
				hasLoadMap = true;
			} else if (!hasLoadBsp) {
				strcpy_s(parms->bspName, argv[i]);
				printf(" %s", parms->bspName);
				hasLoadBsp = true;
			}
		}

		// reading options for bsp and vis
		if (readingBsp) {
			if (!strcmp(argv[i], "-verbose")) {
				parms->bsp.Verbose = GE_TRUE;
				printf(" -verbose");
			} else if (!strcmp(argv[i], "-entverbose")) {
				parms->bsp.EntityVerbose = GE_TRUE;
				printf(" -entverbose");
			} else if (!strcmp(argv[i], "-onlyents")) {
				parms->updateEnts = GE_TRUE;
				printf(" -onlyents");
			} 
		} else if (readingVis) {
			if (!strcmp(argv[i], "-verbose")) {
				parms->vis.Verbose = GE_TRUE;
				printf(" -verbose");
			} else if (!strcmp(argv[i], "-full")) {
				parms->vis.FullVis = GE_TRUE;
				printf(" -full");
			} else if (!strcmp(argv[i], "-sortportals")) {
				parms->vis.SortPortals = GE_FALSE;
				printf(" -sortportals");
			}
		}	
	}

	if (!hasLoadMap) {
		ShowUsage();
	}

	printf("\n");
}

//========================================================================================
// ShowUsage()
// This shows information about the compiler commands
//========================================================================================
void ShowUsage(void) {
	printf("\n--- gbsp Options ---\n");
	printf("    %-20s : %s\n", "mapname",		"The .map file to process.");
	printf("    %-20s : %s\n", "[destname]",	"The .bsp output file path (optional).");
	printf("    %-20s : %s\n", "-verbose",		"Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-entverbose",	"Outputs detailed entity information.");
	printf("    %-20s : %s\n", "-onlyents",		"Do an entity update from .map to .bsp.");
	printf("\n");

	printf("\n--- gvis Options ---\n");
	printf("    %-20s : %s\n", "mapname", "The .map file to process.");
	printf("    %-20s : %s\n", "-verbose", "Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-full", "Performs full visibility calculations. Use it only in final compiles.");
	printf("    %-20s : %s\n", "-sortportals", "Sort the portals with MightSee.");
	printf("\n");

	exit(0);
};


//========================================================================================
// ShowSettings()
// This shows information about which compile paramters are enabled
//========================================================================================
void ShowSettings(CompilerParms parms) {
	CompilerParms defaultParms;
	InitCompilerParms(&defaultParms);
	printf("\nCURRENT gbsp SETTINGS:\n");
	printf("%-20s|%12s |%12s \n", "Name", "Setting", "Default");
	printf("%-20s|%13s|%13s\n", "--------------------", "-------------", "-------------");
	printf("%-20s|%12s |%12s \n", "verbose", parms.bsp.Verbose ? "on" : "off", defaultParms.bsp.Verbose ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "entverbose", parms.bsp.EntityVerbose ? "on" : "off", defaultParms.bsp.EntityVerbose ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "onlyents", parms.updateEnts ? "on" : "off", defaultParms.updateEnts ? "on" : "off");
	printf("\n");
};

//========================================================================================
// ShowSettings()
// This shows information about which compile paramters are enabled
//========================================================================================
void ShowSettingsVis(CompilerParms parms) {
	CompilerParms defaultParms;
	InitCompilerParms(&defaultParms);
	printf("\nCURRENT gvis SETTINGS:\n");
	printf("%-20s|%12s |%12s \n", "Name", "Setting", "Default");
	printf("%-20s|%13s|%13s\n", "--------------------", "-------------", "-------------");
	printf("%-20s|%12s |%12s \n", "verbose", parms.vis.Verbose ? "on" : "off", defaultParms.vis.Verbose ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "full", parms.vis.FullVis ? "on" : "off", defaultParms.vis.FullVis ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "sortportals", parms.vis.SortPortals ? "on" : "off", defaultParms.vis.SortPortals ? "on" : "off");
	printf("\n");
};