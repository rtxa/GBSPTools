/****************************************************************************************/
/*  glight.cpp
/*
/*  Author: rtxa
/*  Description: Add lighting effects to the level.
/*
/*	Generates and applies all lighting effects for the map, such as light entities
/*	and the sky, and makes it look good.
/*
/****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // needed for "LoadLibrary" declarations, get current directory, etc.
#include "glight.h"
#include "gbsplib.h"
#include "gbsptools.h"

int main(int argc, char *argv[]) {
	printf("glight v%.1f (%s)\n", GBSPTOOLS_VERSION, __DATE__);
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

	// finished loading dll, begin with LIGHT compilation
	char bspFullPath[MAX_PATH];
	sprintf_s(bspFullPath, "%s\\%s%s", path, compParms.mapName, ".bsp");

	printf("---- %s ----\n", "BEGIN glight");

	if (compFHook->GBSP_LightGBSPFile(bspFullPath, &compParms.light) == GBSP_ERROR) {
		fprintf(stderr, "Warning: GBSP_LightGBSPFile failed for file: %s, GBSPLib.Dll.\n", bspFullPath);
		return COMPILER_ERROR_BSPFAIL;
	}

	printf("---- %s ----\n\n\n\n", "END glight");

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
			parms->light.Verbose = GE_TRUE;
			printf(" -verbose");
		} else if (!strcmp(argv[i], "-extra")) {
			parms->light.ExtraSamples = GE_TRUE;
			printf(" -extra");
		} else if (!strcmp(argv[i], "-radiosity")) {
			parms->light.Radiosity = GE_TRUE;
			printf(" -radiosity");
		} else if (!strcmp(argv[i], "-fastpatch")) {
			parms->light.FastPatch = GE_TRUE;
			printf(" -fastpatch");
		} else if (!strcmp(argv[i], "-minlight")) {
			printf(" -minlight");
			for (int j = i + 1; j <= i + 3; j++)
				if (j < argc)
					printf(" %s", argv[j]);
			if (i + 3 < argc) {
				parms->light.MinLight.X = strtof(argv[++i], NULL);
				parms->light.MinLight.Y = strtof(argv[++i], NULL);
				parms->light.MinLight.Z = strtof(argv[++i], NULL);

				if (errno == ERANGE) {
					fprintf(stderr, "\nError: Bad arguments for -minlight\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stderr, "\nError: Missing arguments for -minlight\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-lightscale")) {
			printf(" -lightscale");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.LightScale = strtof(argv[++i], NULL);
				if (errno == ERANGE) {
					fprintf(stderr, "\nError: Bad argument for -lightscale\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stderr, "\nError: Missing argument for -lightscale\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-reflectscale")) {
			printf(" -reflectscale");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.ReflectiveScale = strtof(argv[++i], NULL);
				if (errno == ERANGE) {
					fprintf(stderr, "\nError: Bad argument for -reflectscale\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stderr, "\nError: Missing argument for -reflectscale\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-patchsize")) {
			printf(" -patchsize");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.PatchSize = strtof(argv[++i], NULL);
				if (errno == ERANGE) {
					fprintf(stderr, "\nError: Bad argument for -patchsize\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stderr, "\nError: Missing argument for -patchsize\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-bounce")) {
			printf(" -bounce");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.NumBounce = strtol(argv[++i], NULL, 10);
				if (errno == ERANGE) {
					fprintf(stderr, "\nError: Bad argument for -bounce\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stderr, "\nError: Missing argument for -bounce\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
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
	printf("\n--- glight Options ---\n");
	printf("    %-20s : %s\n", "mapname",			"The .map file to process.");
	printf("    %-20s : %s\n", "-verbose",			"Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-minlight r g b",	"Illuminates all surfaces with the light color specified.");
	printf("    %-20s : %s\n", "-lightscale #",		"Light intensity multiplier for the entire level (higher = brighter, lower = darker).");
	printf("    %-20s : %s\n", "-reflectscale #",	"Face reflectivity multiplier. Higher numbers make the level brighter and more colorful.");
	printf("    %-20s : %s\n", "-extra",			"Uses more samples to give finer lighting effects.");
	printf("    %-20s : %s\n", "-radiosity",		"Performs radiosity lighting of the level.");
	printf("    %-20s : %s\n", "-bounce #",			"Set number of radiosity bounces.");
	printf("    %-20s : %s\n", "-patchsize #",		"Set radiosity patch size grid (larger = lower quality, smaller = higher quality).");
	printf("    %-20s : %s\n", "-fastpatch",		"Set fast patching for fast compiles.");
	printf("\n");
	exit(0);
};
