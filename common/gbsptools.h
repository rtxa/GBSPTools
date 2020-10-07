/****************************************************************************************/
/*  gbsptools.h
/*
/*  Author: rtxa
/*  Description: Common functions and structs used on every gbsp tool (gbsp, gvis and glight)
/*
/****************************************************************************************/

#ifndef GBSPTOOLS_H
#define GBSPTOOLS_H

#include <stdio.h>

#define GBSPTOOLS_VERSION 0.9
#define GBSPTOOLS_AUTHOR "rtxa"
#define GBSPTOOLS_CONTACT "usertxa@gmail.com"

typedef enum {
	COMPILER_ERROR_NONE,
	// Errors returned by DLL loader
	COMPILER_ERROR_NODLL,			// couldn't find the DLL
	COMPILER_ERROR_MISSINGFUNC,		// the DLL is missing a required function
	// Errors returned by GBSP functions
	COMPILER_ERROR_BSPFAIL,			// unable to compile the BSP
	COMPILER_ERROR_BSPSAVE,			// unable to save the compiled BSP
	// Errors returned by ParseCmdArgs
	COMPILER_ERROR_BADARG
} CompilerErrorEnum;

static void Compiler_PrintfCallback(char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vprintf_s(format, argptr);
	va_end(argptr);
}

static void Compiler_ErrorfCallback(char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vfprintf_s(stderr, format, argptr);
	va_end(argptr);	
}

CompilerErrorEnum Compiler_LoadCompilerDLL(GBSP_FuncHook* &pFHook, HINSTANCE& pHandle, ERROR_CB ErrorCallbackFcn, PRINTF_CB PrintfCallbackFcn) {
	// load compiler dll
	GBSP_Hook compHook;

	compHook.Error = ErrorCallbackFcn;
	compHook.Printf = PrintfCallbackFcn;

	pHandle = LoadLibrary("gbsplib.dll");
	if (pHandle == nullptr) {
		fprintf(stderr, "Compile Failed: Unable to load GBSPLib.Dll!");
		return COMPILER_ERROR_NODLL;
	}

	GBSP_INIT* pCompInit = (GBSP_INIT*)GetProcAddress(pHandle, "GBSP_Init");
	if (pCompInit == nullptr) {
		fprintf(stderr, "Compile Failed: Couldn't initialize GBSP_Init, GBSPLib.Dll.\n");
		FreeLibrary(pHandle);
		return COMPILER_ERROR_MISSINGFUNC;
	}

	pFHook = pCompInit(&compHook);
	if (pFHook == nullptr) {
		fprintf(stderr, "Compile Failed: GBSP_Init returned NULL Hook!, GBSPLib.Dll.\n");
		FreeLibrary(pHandle);
		return COMPILER_ERROR_MISSINGFUNC;
	}

	return COMPILER_ERROR_NONE;
}

#endif // GBSPTOOLS_H