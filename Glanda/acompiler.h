#pragma once

#include <windows.h>

#define codeMC		1
#define codeFrame	2
#define codeButton	3

#define CF_DEBUG	0x1
#define CF_VERBOSE	0x2

// scene start time is 0 based
typedef int (* PtrGetSceneStartFrameByName)(const char *scene);

// scene start time and scene index is 0 based
typedef int (* PtrGetSceneStartFrameByIndex)(int index);

// get frame label start time (0 based) related to specified scene.
typedef int (* PtrGetFrameLabelStartFrame)(int scene_start_frame, const char *label);

typedef int (* PtrCompile)(DWORD flag, int type, const char *source);

typedef int (* PtrCompileEx)(DWORD flag, int type, const char *source, int current_scene, 
	PtrGetSceneStartFrameByName lpfnGetSceneStartFrameByName, 
	PtrGetSceneStartFrameByIndex lpfnGetSceneStartFrameByIndex, 
	PtrGetFrameLabelStartFrame lpfnGetFrameLabelStartFrame);

typedef const unsigned char * (* PtrGetBinCode)(unsigned int *size);
typedef const char * (* PtrGetOutputMsg)(unsigned int *size);
typedef void (* PtrFreeAllBuffers)();
