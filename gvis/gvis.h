#ifndef GVIS_H
#define GVIS_H

#include <windows.h>
#include "gbsplib.h"

typedef struct {
	char mapName[MAX_PATH];
	VisParms vis;
} CompilerParms;

void InitCompilerParms(CompilerParms *parms) {
	parms->vis.Verbose = GE_FALSE;
	parms->vis.FullVis = GE_FALSE;
	parms->vis.SortPortals = GE_FALSE;
}

void ParseCmdArgs(int, char *[], CompilerParms *);
void ShowUsage(void);
void ShowSettings(CompilerParms parms);

#endif // GVIS_H