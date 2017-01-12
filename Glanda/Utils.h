//////////////////////////////////////////////////////////////
// Utils.h

#ifndef __UTILS_H__
#define __UTILS_H__

#include "shlwapi.h"

LPTSTR GetLastErrorMessage(LPTSTR s, int nBufSize);
CString GetLastErrorMessage();
void ReportLastError();
void ReportLastError(LPCTSTR fmt, ...);
void ReportLastError(UINT fmt, ...);

int MessageBoxEx(CWnd *pWnd, UINT uType, LPCTSTR fmt, ...);
int MessageBoxEx(CWnd *pWnd, UINT uType, UINT fmt, ...);
int AfxMessageBoxEx(int nType, LPCTSTR fmt, ...);
int AfxMessageBoxEx(int nType, UINT fmt, ...);

BOOL IsThreadActive(HANDLE hThread);

void SaveWindowRect(CWnd *pWnd, LPCTSTR szName);
void LoadWindowRect(CWnd *pWnd, LPCTSTR szName);
void SaveWindowPlacement(CWnd *pWnd, LPCTSTR szName);
void LoadWindowPlacement(CWnd *pWnd, LPCTSTR szName);
void EnsureWholeRectVisible(CRect &rc);

void DeleteRedundantSeparator(CMenu *pMenu);
void UpdateMenuCmdUI(CWnd *pWnd, CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
void FillComboBox(CComboBox *pComboBox, UINT nStringID);
void FillListBox(CListBox *pListBox, UINT nStringID);

#define round(f) (((f) > 0 ? (f) + 0.5 : (f) - 0.5))

// print a message while compiling
// usage: #pragma mesage(__FILELINE__, "message-text")
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __FILELINE__ __FILE__ "("__STR1__(__LINE__)") : "

// singleton
#define DECLARE_SINGLETON(x)	\
static x *m_pInstance;			\
static x *Instance()			\
{								\
	if (!m_pInstance)			\
		m_pInstance = new x;	\
	return m_pInstance;			\
}								\
static void ReleaseInstance()	\
{								\
	delete m_pInstance;			\
	m_pInstance = NULL;			\
}

#define IMPLEMENT_SINGLETON(x)	x *x::m_pInstance = NULL;

#define DEFINE_REGISTERED_MESSAGE(message)	UINT message = ::RegisterWindowMessage("9CAF76A6-1449-11d4-BC50-00400533C4C1:"#message);
#define DEFINE_CLIPBOARD_FORMAT(format)		UINT format = ::RegisterClipboardFormat("9CAF76A6-1449-11d4-BC50-00400533C4C1:"#format);

#endif
