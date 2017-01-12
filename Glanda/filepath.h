#ifndef __FILEPATH_H__
#define __FILEPATH_H__

#include "shlwapi.h"

BOOL IsValidPath(LPCTSTR lpszPath);
BOOL IsValidFileName(LPCTSTR lpszFileName);
BOOL PrepareDirectory(LPCTSTR lpszPath, BOOL bPrompt);
void ReplaceInvalidCharsInFileName(LPTSTR lpszFileName, TCHAR ch);
BOOL CreateDirectoryNest(LPCTSTR lpszPath);
BOOL RemoveReadOnlyAttribute(LPCTSTR lpszName);
BOOL DeleteFile(LPCTSTR lpszName, BOOL bChangeAttrsIfNeed);
BOOL RemoveDirectory(LPCTSTR lpszName, BOOL bForce);
BOOL CopyFile(LPCTSTR lpszExisting, LPCTSTR lpszNew, BOOL bFailedIfExists, BOOL bForce);

////////////////////////////////////////////////////////////
// CString version of shlwapi functions
CString FileStripFileName(LPCTSTR lpszPathName);
CString FileStripPath(LPCTSTR lpszPathName);
CString GetRelativePath(LPCTSTR lpszFrom, LPCTSTR lpszTo);

// aaa[1].htm, aaa[2].htm
CString RemoveSequenceNumber(LPCTSTR lpszFile);
CString GenerateSequenceNumber(LPCTSTR lpszFile);

// return value is a pointer to a static variable
LPCTSTR GetModuleFilePath();

// Pattern: lpszPath\lpszPrefix[0-9]+.lpszExt
CString GetTempFile(LPCTSTR lpszPath, LPCTSTR lpszPrefix, LPCTSTR lpszExt);

// compare 2 files' content.
// 0 : same contents
// !0: different contents
int CompareFile(LPCSTR lpszFile1, LPCSTR lpszFile2);

CString URLToPath(LPCTSTR lpszUrl);

BOOL IsDirWritable(LPCTSTR lpszPath);

void ReplaceInvalidCharsInFileName(LPTSTR lpszFileName, TCHAR ch);

#endif
