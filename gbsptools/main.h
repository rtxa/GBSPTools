#pragma once

#ifndef GBSP_H
#define GBSP_H

#include <windows.h>
#include "gbsplib.h"

typedef struct {
	char mapName[MAX_PATH];
	char bspName[MAX_PATH];
	bool isBspEnabled;
	bool isVisEnabled;
	bool isLightEnabled;
	BspParms bsp;
	VisParms vis;
	LightParms light;
	geBoolean updateEnts;
} CompilerParms;

void InitCompilerParms(CompilerParms* parms) {
	parms->isBspEnabled = false;
	parms->isVisEnabled = false;
	parms->isLightEnabled = false;
	parms->bsp.Verbose = GE_FALSE;
	parms->bsp.EntityVerbose = GE_FALSE;
	parms->vis.FullVis = GE_FALSE;
	parms->vis.Verbose = GE_FALSE;
	parms->vis.SortPortals = GE_FALSE;
	parms->light.Verbose = GE_FALSE;
	parms->light.ExtraSamples = GE_FALSE;
	parms->light.MinLight = { 0.0, 0.0, 0.0 };
	parms->light.LightScale = 1.0;
	parms->light.ReflectiveScale = 1.0;
	parms->light.Radiosity = GE_FALSE;
	parms->light.NumBounce = 10;
	parms->light.PatchSize = 128.0;
	parms->light.FastPatch = GE_FALSE;
	parms->updateEnts = GE_FALSE;
	parms->bspName[0] = '\0';
}

void ParseCmdArgs(int, char* [], CompilerParms*);
void ShowUsage(void);
void ShowSettingsBsp(CompilerParms parms);
void ShowSettingsVis(CompilerParms parms);
void ShowSettingsLight(CompilerParms parms);

#endif // GBSP_H