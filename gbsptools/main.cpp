/****************************************************************************************/
/*  main.cpp
/*
/*  Author: rtxa
/*  Description: Creates a BSP file from a MAP file
/*
/****************************************************************************************/

#include <stdio.h>
#include <windows.h> // needed for "LoadLibrary" declarations, get current directory, etc.
#include "main.h"
#include "gbsplib.h"
#include "gbsptools.h"
#include "utils.h"

int main(int argc, char* argv[]) {
	printf("gbsptools v%.1f (%s)\n", GBSPTOOLS_VERSION, __DATE__);
	printf("Genesis 3D BSP Tools - Author: %s\n", GBSPTOOLS_AUTHOR);
	printf("Check README.md for more info abouts these tools.\n");
	printf("Submit detailed bug reports to %s\n", GBSPTOOLS_CONTACT);

	// Get current directory
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	printf("Command line: \"%s\"\n", path);

	// Initialize compiler parameters
	CompilerParms compParms;
	InitCompilerParms(&compParms);
	ParseCmdArgs(argc, argv, &compParms);

	// Load the compiler DLL (GBSPLib.dll)
	HINSTANCE compHandle;
	GBSP_FuncHook* compFHook;
	CompilerErrorEnum result = Compiler_LoadCompilerDLL(compFHook, compHandle, Compiler_ErrorfCallback, Compiler_PrintfCallback);

	if (result != CompilerErrorEnum::COMPILER_ERROR_NONE) {
		return result;
	}

	std::string mapPath(compParms.mapName);
	std::string bspPath;

	// Create destination name if not specified
	if (!compParms.bspName[0]) {
		bspPath = std::string(mapPath);
		GBSPTools::StripExtension(bspPath);
		bspPath.append(".bsp");
	}
	else {
		bspPath = std::string(compParms.bspName);
	}

	// Convert paths to Unix format (GBSPLib expects that) and set defaults
	GBSPTools::PathToUnix(mapPath);
	GBSPTools::PathToUnix(bspPath);
	GBSPTools::DefaultExtension(mapPath, ".map");
	GBSPTools::DefaultExtension(bspPath, ".bsp");

	// Begin with GBSP
	if (compParms.isBspEnabled) {
		ShowSettingsBsp(compParms);
		if (compParms.updateEnts == GE_TRUE) {
			if (compFHook->GBSP_UpdateEntities(mapPath.c_str(), bspPath.c_str()) != GE_TRUE) {
				fprintf(stdout, "Compile Failed:  GBSP_UpdateEntities returned an error, GBSPLib.Dll.\n");
				return COMPILER_ERROR_BSPFAIL;
			}
		}
		else {
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
		}
		compFHook->GBSP_FreeBSP();
		printf("\n");
	}

	// Begin with GVIS
	if (compParms.isVisEnabled) {
		ShowSettingsVis(compParms);
		if (compFHook->GBSP_VisGBSPFile(bspPath.c_str(), &compParms.vis) == GBSP_ERROR) {
			fprintf(stderr, "Warning: GBSP_VisGBSPFile failed for file : %s, GBSPLib.Dll.\n", bspPath.c_str());
			return COMPILER_ERROR_BSPFAIL;
		}
		printf("\n");
	}

	// Begin with GLIGHT
	if (compParms.isLightEnabled) {
		ShowSettingsLight(compParms);
		if (compFHook->GBSP_LightGBSPFile(bspPath.c_str(), &compParms.light) == GBSP_ERROR) {
			fprintf(stdout, "Warning: GBSP_LightGBSPFile failed for file: %s, GBSPLib.Dll.\n", bspPath.c_str());
			return COMPILER_ERROR_BSPFAIL;
		}
		printf("\n");
	}

	FreeLibrary(compHandle);

	return COMPILER_ERROR_NONE;
}

//========================================================================================
//	ParseCmdArgs()
//	This parses command line arguments to load them into the compiler parameters
//========================================================================================
void ParseCmdArgs(int argc, char* argv[], CompilerParms* parms) {
	if (argc < 2)
		ShowUsage();

	bool hasLoadMap = false;
	bool hasLoadBsp = false;

	const int READING_BSP = 1;
	const int READING_VIS = 2;
	const int READING_LIGHT = 3;

	int currentFlag = 0;

	printf("Arguments:");
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-gbsp")) {
			parms->isBspEnabled = true;
			currentFlag = READING_BSP;
			printf(" -gbsp");
			continue;
		}
		else if (!strcmp(argv[i], "-gvis")) {
			parms->isVisEnabled = true;
			currentFlag = READING_VIS;
			printf(" -gvis");
			continue;
		}
		else if (!strcmp(argv[i], "-glight")) {
			parms->isLightEnabled = true;
			currentFlag = READING_LIGHT;
			printf(" -glight");
			continue;
		}

		// reading paths to .map and .bsp
		if (argv[i][0] != '-') {
			if (!hasLoadMap) {
				strcpy_s(parms->mapName, argv[i]);
				printf(" %s", parms->mapName);
				hasLoadMap = true;
			}
			else if (!hasLoadBsp) {
				strcpy_s(parms->bspName, argv[i]);
				printf(" %s", parms->bspName);
				hasLoadBsp = true;
			}
		}

		// reading options for bsp/vis/light
		if (currentFlag == READING_BSP) {
			if (!strcmp(argv[i], "-verbose")) {
				parms->bsp.Verbose = GE_TRUE;
				printf(" -verbose");
			}
			else if (!strcmp(argv[i], "-entverbose")) {
				parms->bsp.EntityVerbose = GE_TRUE;
				printf(" -entverbose");
			}
			else if (!strcmp(argv[i], "-onlyents")) {
				parms->updateEnts = GE_TRUE;
				printf(" -onlyents");
			}
		}
		else if (currentFlag == READING_VIS) {
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
			}
		}
		else if (currentFlag == READING_LIGHT) {
			if (!strcmp(argv[i], "-verbose")) {
				parms->light.Verbose = GE_TRUE;
				printf(" -verbose");
			}
			else if (!strcmp(argv[i], "-extra")) {
				parms->light.ExtraSamples = GE_TRUE;
				printf(" -extra");
			}
			else if (!strcmp(argv[i], "-radiosity")) {
				parms->light.Radiosity = GE_TRUE;
				printf(" -radiosity");
			}
			else if (!strcmp(argv[i], "-fastpatch")) {
				parms->light.FastPatch = GE_TRUE;
				printf(" -fastpatch");
			}
			else if (!strcmp(argv[i], "-minlight")) {
				printf(" -minlight");
				for (int j = i + 1; j <= i + 3; j++) {
					if (j < argc) {
						printf(" %s", argv[j]);
					}
				}

				if (i + 3 < argc) {
					parms->light.MinLight.X = strtof(argv[++i], NULL);
					parms->light.MinLight.Y = strtof(argv[++i], NULL);
					parms->light.MinLight.Z = strtof(argv[++i], NULL);

					if (errno == ERANGE) {
						fprintf(stdout, "\nError: Bad arguments for -minlight\n\n\n\n");
						exit(COMPILER_ERROR_BADARG);
					}
				}
				else {
					fprintf(stdout, "\nError: Missing arguments for -minlight\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			}
			else if (!strcmp(argv[i], "-lightscale")) {
				printf(" -lightscale");
				if (i + 1 < argc) {
					printf(" %s", argv[i + 1]);
					parms->light.LightScale = strtof(argv[++i], NULL);
					if (errno == ERANGE) {
						fprintf(stdout, "\nError: Bad argument for -lightscale\n\n\n\n");
						exit(COMPILER_ERROR_BADARG);
					}
				}
				else {
					fprintf(stdout, "\nError: Missing argument for -lightscale\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			}
			else if (!strcmp(argv[i], "-reflectscale")) {
				printf(" -reflectscale");
				if (i + 1 < argc) {
					printf(" %s", argv[i + 1]);
					parms->light.ReflectiveScale = strtof(argv[++i], NULL);
					if (errno == ERANGE) {
						fprintf(stdout, "\nError: Bad argument for -reflectscale\n\n\n\n");
						exit(COMPILER_ERROR_BADARG);
					}
				}
				else {
					fprintf(stdout, "\nError: Missing argument for -reflectscale\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			}
			else if (!strcmp(argv[i], "-patchsize")) {
				printf(" -patchsize");
				if (i + 1 < argc) {
					printf(" %s", argv[i + 1]);
					parms->light.PatchSize = strtof(argv[++i], NULL);
					if (errno == ERANGE) {
						fprintf(stdout, "\nError: Bad argument for -patchsize\n\n\n\n");
						exit(COMPILER_ERROR_BADARG);
					}
				}
				else {
					fprintf(stdout, "\nError: Missing argument for -patchsize\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			}
			else if (!strcmp(argv[i], "-bounce")) {
				printf(" -bounce");
				if (i + 1 < argc) {
					printf(" %s", argv[i + 1]);
					parms->light.NumBounce = strtol(argv[++i], NULL, 10);
					if (errno == ERANGE) {
						fprintf(stdout, "\nError: Bad argument for -bounce\n\n\n\n");
						exit(COMPILER_ERROR_BADARG);
					}
				}
				else {
					fprintf(stdout, "\nError: Missing argument for -bounce\n\n\n\n");
					exit(COMPILER_ERROR_BADARG);
				}
			}
			else {
				if (!hasLoadMap) {
					strcpy_s(parms->mapName, argv[i]);
					printf(" %s", parms->mapName);
					hasLoadMap = GE_TRUE;
				}
				else {
					ShowUsage();
				}
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
	printf("    %-20s : %s\n", "mapname", "The .map file to process.");
	printf("    %-20s : %s\n", "[destname]", "The .bsp output file path (optional).");
	printf("    %-20s : %s\n", "-verbose", "Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-entverbose", "Outputs detailed entity information.");
	printf("    %-20s : %s\n", "-onlyents", "Do an entity update from .map to .bsp.");
	printf("\n");

	printf("\n--- gvis Options ---\n");
	printf("    %-20s : %s\n", "mapname", "The .map file to process.");
	printf("    %-20s : %s\n", "-verbose", "Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-full", "Performs full visibility calculations. Use it only in final compiles.");
	printf("    %-20s : %s\n", "-sortportals", "Sort the portals with MightSee.");
	printf("\n");

	printf("\n--- glight Options ---\n");
	printf("    %-20s : %s\n", "mapname", "The .map file to process.");
	printf("    %-20s : %s\n", "-verbose", "Outputs detailed compilation progress information.");
	printf("    %-20s : %s\n", "-minlight r g b", "Illuminates all surfaces with the light color specified.");
	printf("    %-20s : %s\n", "-lightscale #", "Light intensity multiplier for the entire level (higher = brighter, lower = darker).");
	printf("    %-20s : %s\n", "-reflectscale #", "Face reflectivity multiplier. Higher numbers make the level brighter and more colorful.");
	printf("    %-20s : %s\n", "-extra", "Uses more samples to give finer lighting effects.");
	printf("    %-20s : %s\n", "-radiosity", "Performs radiosity lighting of the level.");
	printf("    %-20s : %s\n", "-bounce #", "Set number of radiosity bounces.");
	printf("    %-20s : %s\n", "-patchsize #", "Set radiosity patch size grid (larger = lower quality, smaller = higher quality).");
	printf("    %-20s : %s\n", "-fastpatch", "Set fast patching for fast compiles.");
	printf("\n");

	exit(0);
};


//========================================================================================
// ShowSettings()
// This shows information about which compile paramters are enabled
//========================================================================================
void ShowSettingsBsp(CompilerParms parms) {
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

//========================================================================================
// ShowSettings()
// This shows information about which compile paramters are enabled
//========================================================================================
void ShowSettingsLight(CompilerParms parms) {
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