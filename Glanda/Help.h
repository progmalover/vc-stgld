#ifndef _HELP_H
#define _HELP_H

#include "htmlhelp.h"

struct _HELP_INFO
{
	UINT nID;
	LPCTSTR pszFile;
};

extern const _HELP_INFO g_HelpInfo[];
extern const int g_cHelpInfo;
extern const TCHAR g_szHelpFilePrefix[];

LPCTSTR GetHelpFileOfID(UINT nID);
BOOL ShowHelpFile(LPCTSTR lpszFile);

#endif
