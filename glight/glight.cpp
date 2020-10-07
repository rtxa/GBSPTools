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
#include "utils.h"

int main(int argc, char *argv[]) {
	printf("glight v%.1f (%s)\n", GBSPTOOLS_VERSION, __DATE__);
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

	ShowSettings(compParms);

	std::string bspPath(compParms.mapName);

	// make path readable for GBSPLib and use this extension if not provided
	GBSPTools::PathToUnix(bspPath);
	GBSPTools::DefaultExtension(bspPath, ".bsp");

	if (compFHook->GBSP_LightGBSPFile(bspPath.c_str(), &compParms.light) == GBSP_ERROR) {
		fprintf(stdout, "Warning: GBSP_LightGBSPFile failed for file: %s, GBSPLib.Dll.\n", bspPath.c_str());
		return COMPILER_ERROR_BSPFAIL;
	}

	printf("\n");

	FreeLibrary(compHandle);

	return COMPILER_ERROR_NONE;
}

//========================================================================================
//	ParseCmdArgs()
//	This parse command line arguments to load them into the compiler parameters
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
					fprintf(stdout, "\nError: Bad arguments for -minlight\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stdout, "\nError: Missing arguments for -minlight\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-lightscale")) {
			printf(" -lightscale");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.LightScale = strtof(argv[++i], NULL);
				if (errno == ERANGE) {
					fprintf(stdout, "\nError: Bad argument for -lightscale\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stdout, "\nError: Missing argument for -lightscale\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-reflectscale")) {
			printf(" -reflectscale");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.ReflectiveScale = strtof(argv[++i], NULL);
				if (errno == ERANGE) {
					fprintf(stdout, "\nError: Bad argument for -reflectscale\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stdout, "\nError: Missing argument for -reflectscale\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-patchsize")) {
			printf(" -patchsize");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.PatchSize = strtof(argv[++i], NULL);
				if (errno == ERANGE) {
					fprintf(stdout, "\nError: Bad argument for -patchsize\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stdout, "\nError: Missing argument for -patchsize\n\n\n\n");
				exit(COMPILER_ERROR_BADARG);
			}
		} else if (!strcmp(argv[i], "-bounce")) {
			printf(" -bounce");
			if (i + 1 < argc) {
				printf(" %s", argv[i + 1]);
				parms->light.NumBounce = strtol(argv[++i], NULL, 10);
				if (errno == ERANGE) {
					fprintf(stdout, "\nError: Bad argument for -bounce\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			} else {
				fprintf(stdout, "\nError: Missing argument for -bounce\n\n\n\n");
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

//========================================================================================
// ShowSettings()
// This shows information about which compile paramters are enabled
//========================================================================================
void ShowSettings(CompilerParms parms) {
	CompilerParms defaultParms;
	InitCompilerParms(&defaultParms);

	// used only to format minlight parameters
	char buffer1[32], buffer2[32];
	sprintf_s(buffer1, "%.0f %.0f %.0f", parms.light.MinLight.X, parms.light.MinLight.Y, parms.light.MinLight.Z);
	sprintf_s(buffer2, "%.0f %.0f %.0f", defaultParms.light.MinLight.X, defaultParms.light.MinLight.Y, defaultParms.light.MinLight.Z);

	printf("\nCURRENT glight SETTINGS:\n");
	printf("%-20s|%12s |%12s \n", "Name", "Setting", "Default");
	printf("%-20s|%13s|%13s\n", "--------------------", "-------------", "-------------");
	printf("%-20s|%12s |%12s \n", "verbose", parms.light.Verbose ? "on" : "off", defaultParms.light.Verbose ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "minlight", buffer1, buffer2);
	printf("%-20s|%12s |%12s \n", "lightscale", std::to_string(parms.light.LightScale).c_str(), std::to_string(defaultParms.light.LightScale).c_str());
	printf("%-20s|%12s |%12s \n", "reflectscale", std::to_string(parms.light.ReflectiveScale).c_str(), std::to_string(defaultParms.light.ReflectiveScale).c_str());
	printf("%-20s|%12s |%12s \n", "extra", parms.light.ExtraSamples ? "on" : "off", defaultParms.light.ExtraSamples ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "radiosity", parms.light.Radiosity ? "on" : "off", defaultParms.light.Radiosity ? "on" : "off");
	printf("%-20s|%12s |%12s \n", "bounce", std::to_string(parms.light.NumBounce).c_str(), std::to_string(defaultParms.light.NumBounce).c_str());
	printf("%-20s|%12s |%12s \n", "patchsize", std::to_string(parms.light.PatchSize).c_str(), std::to_string(defaultParms.light.PatchSize).c_str());
	printf("%-20s|%12s |%12s \n", "fastpatch", parms.light.FastPatch ? "on" : "off", defaultParms.light.FastPatch ? "on" : "off");

	printf("\n");
};
