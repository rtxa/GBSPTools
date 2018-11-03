#ifndef GBSP_H
#define GBSP_H

#include <windows.h>
#include "gbsplib.h"

typedef struct {
	char mapName[MAX_PATH];
	BspParms bsp;
	geBoolean updateEnts;
} CompilerParms;

void InitCompilerParms(CompilerParms *parms) {
	parms->bsp.Verbose = GE_FALSE;
	parms->bsp.EntityVerbose = GE_FALSE;
	parms->updateEnts = GE_FALSE;
}

void ParseCmdArgs(int, char *[], CompilerParms *);
void ShowUsage(void);

#endif // GBSP_H