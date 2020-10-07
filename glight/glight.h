#ifndef GLIGHT_H
#define GLIGHT_H

#include <windows.h>
#include "gbsplib.h"

typedef struct {
	char mapName[MAX_PATH];
	LightParms light;
} CompilerParms;

void InitCompilerParms(CompilerParms *parms) {
	parms->light.Verbose = GE_FALSE;
	parms->light.ExtraSamples = GE_FALSE;
	parms->light.MinLight = { 0.0, 0.0, 0.0 };
	parms->light.LightScale = 1.0;
	parms->light.ReflectiveScale = 1.0;
	parms->light.Radiosity = GE_FALSE;
	parms->light.NumBounce = 10;
	parms->light.PatchSize = 128.0;
	parms->light.FastPatch = GE_FALSE;
}

void ParseCmdArgs(int, char *[], CompilerParms *);
void ShowUsage(void);
void ShowSettings(CompilerParms parms);

#endif // GLIGHT_H