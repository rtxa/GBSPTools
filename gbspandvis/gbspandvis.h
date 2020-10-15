#ifndef GBSP_H
#define GBSP_H

#include <windows.h>
#include "gbsplib.h"

typedef struct {
	char mapName[MAX_PATH];
	char bspName[MAX_PATH];
	BspParms bsp;
	VisParms vis;
	geBoolean updateEnts;
} CompilerParms;

void InitCompilerParms(CompilerParms *parms) {
	parms->bsp.Verbose = GE_FALSE;
	parms->bsp.EntityVerbose = GE_FALSE;
	parms->vis.FullVis = GE_FALSE;
	parms->vis.Verbose = GE_FALSE;
	parms->vis.SortPortals = GE_FALSE;
	parms->updateEnts = GE_FALSE;
	parms->bspName[0] = '\0';
}

void ParseCmdArgs(int, char *[], CompilerParms *);
void ShowUsage(void);
void ShowSettings(CompilerParms parms);
void ShowSettingsVis(CompilerParms parms);

#endif // GBSP_H