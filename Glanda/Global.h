#pragma once

class gldObj;
class gldCharacterKey;
enum ActionSegmentType;

extern const char *g_szAppName;

BOOL ExportMovie(CWnd *pParentWnd, LPCTSTR lpszFilePath, BOOL bCompressed, BOOL bDebug);
BOOL GetCommandDesc(UINT nID, CString &strDesc);

// Compile the action script in gldObject recursively. Pass NULL if it is the main movie
bool CompileActionScript(gldObj *pObj, BOOL bDebug);
bool CompileActionScript(int actionType, BOOL bDebug, const char *sourceCode, unsigned char **byteCode, int *byteCodeSize, CString &strError);
CString FormatCompilingError(int nFrame, gldCharacterKey *pCharacterKey, LPCTSTR lpszError);
