/////////////////////////////////////////////
// Global.cpp

#include "stdafx.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LPTSTR GetLastErrorMessage(LPTSTR s, int nBufSize)
{
	memset(s, 0, nBufSize);
	int nCode = GetLastError();
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, 
		nCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		s, 
		nBufSize, 
		NULL) == 0)
	{
		_snprintf(s, nBufSize, "(%d)", nCode);
	}

	int len = (int)_tcslen(s);
	if (len >= 2 && s[len - 2] == '\r' && s[len - 1] == '\n')
		s[len - 2] = '\0';

	return s;
}

CString GetLastErrorMessage()
{
	CString strErr;
	GetLastErrorMessage(strErr.GetBuffer(256), 255);
	strErr.ReleaseBuffer();
	return strErr;
}

void ReportLastError()
{
	TCHAR szErr[256];
	GetLastErrorMessage(szErr, 255);
	AfxMessageBox(szErr, MB_ICONERROR);
}

void ReportLastError(UINT fmt, ...)
{
	TCHAR szErr[256];
	GetLastErrorMessage(szErr, 255);

	CString strFormat;
	strFormat.LoadString(fmt);

	CString strBuf;
	strBuf.Format(strFormat, szErr);

	AfxMessageBox(strBuf, MB_ICONERROR);
}

void ReportLastError(LPCTSTR fmt, ...)
{
	TCHAR szErr[256];
	GetLastErrorMessage(szErr, 255);

	CString strBuf;
	strBuf.Format(fmt, szErr);

	AfxMessageBox(strBuf, MB_ICONERROR);
}

int MessageBoxEx(CWnd *pWnd, UINT uType, LPCTSTR fmt, ...)
{
	CString strBuf;

	va_list args;
	va_start(args, fmt);
	strBuf.FormatV(fmt, args);
	va_end (args);

	return pWnd->MessageBox(strBuf, NULL, uType);
}

int MessageBoxEx(CWnd *pWnd, UINT uType, UINT fmt, ...)
{
	CString strFormat;
	strFormat.LoadString(fmt);

	CString strBuf;

	va_list args;
	va_start(args, fmt);
	strBuf.FormatV(strFormat, args);
	va_end (args);

	return pWnd->MessageBox(strBuf, NULL, uType);
}

int AfxMessageBoxEx(int nType, LPCTSTR fmt, ...)
{
	CString strBuf;

	va_list args;
	va_start(args, fmt);
	strBuf.FormatV(fmt, args);
	va_end (args);

	return AfxMessageBox(strBuf, nType);
}

int AfxMessageBoxEx(int nType, UINT fmt, ...)
{
	CString strFormat;
	strFormat.LoadString(fmt);

	CString strBuf;

	va_list args;
	va_start(args, fmt);
	strBuf.FormatV(strFormat, args);
	va_end (args);

	return AfxMessageBox(strBuf, nType);
}

BOOL IsThreadActive(HANDLE hThread)
{
	DWORD dwCode;
	if (!GetExitCodeThread(hThread, &dwCode))
		return FALSE;
	return dwCode == STILL_ACTIVE;
}

//
// PURPOST:			Save windiws's rect to registry
//
void SaveWindowRect(CWnd *pWnd, LPCTSTR szName)
{
	ASSERT(IsWindow(pWnd->GetSafeHwnd()));
	if (!IsWindow(pWnd->GetSafeHwnd()))
		return;

	CString strSection;
	strSection.Format("Windows\\%s", szName);

	// Use GetWindowPlacement() because maybe the windows is minimized.
	WINDOWPLACEMENT wp;
	pWnd->GetWindowPlacement(&wp);

	CWinApp *pApp = AfxGetApp();
	pApp->WriteProfileInt(strSection, "showCmd", pWnd->IsWindowVisible() ? wp.showCmd : SW_HIDE);
	pApp->WriteProfileInt(strSection, "Left", wp.rcNormalPosition.left);
	pApp->WriteProfileInt(strSection, "Top", wp.rcNormalPosition.top);
	pApp->WriteProfileInt(strSection, "Right", wp.rcNormalPosition.right);
	pApp->WriteProfileInt(strSection, "Bottom", wp.rcNormalPosition.bottom);
}

//
// PURPOST:			Load windiws's rect from registry
//
void LoadWindowRect(CWnd *pWnd, LPCTSTR szName)
{
	ASSERT(IsWindow(pWnd->GetSafeHwnd()));
	if (!IsWindow(pWnd->GetSafeHwnd()))
		return;

	CString strSection;
	strSection.Format("Windows\\%s", szName);

	// Get default rect first
	CRect rc;
	pWnd->GetWindowRect(&rc);

	CWinApp *pApp = AfxGetApp();
	UINT showCmd = pApp->GetProfileInt(strSection, "showCmd", SW_SHOW);
	rc.left = pApp->GetProfileInt(strSection, "Left", rc.left);
	rc.top = pApp->GetProfileInt(strSection, "Top", rc.top);
	rc.right = pApp->GetProfileInt(strSection, "Right", rc.right);
	rc.bottom = pApp->GetProfileInt(strSection, "Bottom", rc.bottom);

	CRect rcScreen(0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
	CRect rcIntersect;
	rcIntersect.IntersectRect(&rc, &rcScreen);
	if (rcIntersect.Width() >= 20 && rcIntersect.Height() >= 20)
	{
		UINT nFlags = SWP_NOZORDER;
		if (!(pWnd->GetStyle() & WS_THICKFRAME))
			nFlags |= SWP_NOSIZE;
		pWnd->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), nFlags);
	}

	if (showCmd != SW_HIDE)
	{
		if (showCmd == SW_SHOWMAXIMIZED && pWnd->GetStyle() & WS_MAXIMIZEBOX)
			pWnd->ShowWindow(SW_SHOWMAXIMIZED);
		else
			pWnd->ShowWindow(SW_SHOW);
	}
}

void SaveWindowPlacement(CWnd *pWnd, LPCTSTR szName)
{
	WINDOWPLACEMENT wp;

	memset (&wp, 0, sizeof (wp));
	wp.length = sizeof (wp);
	pWnd->GetWindowPlacement (&wp);

	CString strTemp;
	strTemp.Format("Windows\\%s", szName);

	CWinApp *pApp = AfxGetApp();
	pApp->WriteProfileInt(strTemp, "showCmd", wp.showCmd);
	pApp->WriteProfileInt(strTemp, "Left", wp.rcNormalPosition.left);
	pApp->WriteProfileInt(strTemp, "Top", wp.rcNormalPosition.top);
	pApp->WriteProfileInt(strTemp, "Right", wp.rcNormalPosition.right);
	pApp->WriteProfileInt(strTemp, "Bottom", wp.rcNormalPosition.bottom);
}

void LoadWindowPlacement(CWnd *pWnd, LPCTSTR szName)
{
	CString strTemp;
	strTemp.Format("Windows\\%s", szName);

	WINDOWPLACEMENT wp;
	memset (&wp, 0, sizeof (wp));
	wp.length = sizeof (wp);
	pWnd->GetWindowPlacement(&wp);

	CWinApp *pApp = AfxGetApp();
	UINT showCmd = pApp->GetProfileInt(strTemp, "showCmd", SW_SHOW);
	wp.rcNormalPosition.left = pApp->GetProfileInt(strTemp, "Left", wp.rcNormalPosition.left);
	wp.rcNormalPosition.top = pApp->GetProfileInt(strTemp, "Top", wp.rcNormalPosition.top);
	wp.rcNormalPosition.right = pApp->GetProfileInt(strTemp, "Right", wp.rcNormalPosition.right);
	wp.rcNormalPosition.bottom = pApp->GetProfileInt(strTemp, "Bottom", wp.rcNormalPosition.bottom);

	if (showCmd == SW_SHOWMAXIMIZED && pWnd->GetStyle() & WS_MAXIMIZEBOX)
		wp.showCmd = SW_SHOWMAXIMIZED;
	else
		wp.showCmd = SW_SHOWNORMAL;

	wp.flags = 0;
	pWnd->SetWindowPlacement(&wp);
}

void EnsureWholeRectVisible(CRect &rc)
{
	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	int dx = 0;
	int dy = 0;

	if (rc.left < 0)
		dx = -rc.left;
	else if (rc.right > cxScreen)
		dx = cxScreen - rc.right;

	if (rc.top < 0)
		dy = -rc.top;
	else if (rc.bottom > cyScreen)
		dy = cyScreen - rc.bottom;

	rc.OffsetRect(dx, dy);
}

void DeleteRedundantSeparator(CMenu *pMenu)
{
			// remove duplicate separators
	int count = pMenu->GetMenuItemCount();
	for (int i = count - 1; i > 0; i--)
	{
		if (pMenu->GetMenuItemID(i) == 0 && pMenu->GetMenuItemID(i - 1) == 0)
			pMenu->DeleteMenu(i, MF_BYPOSITION);
	}
	count = pMenu->GetMenuItemCount();
	if (count > 0)
	{
		if (pMenu->GetMenuItemID(count - 1) == 0)
			pMenu->DeleteMenu(count - 1, MF_BYPOSITION);

		if (count > 1)
			if (pMenu->GetMenuItemID(0) == 0)
				pMenu->DeleteMenu(0, MF_BYPOSITION);
	}
}

void UpdateMenuCmdUI(CWnd *pWnd, CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
	if (bSysMenu)
		return;     // don't support system menu

	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(pWnd->m_hWnd)) != NULL)
	{
		CWnd* pParent = pWnd->GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(pWnd, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pWnd, state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void FillComboBox(CComboBox *pComboBox, UINT nStringID)
{
	CString strAll;
	if (strAll.LoadString(nStringID))
	{
		for (int i = 0; ; i++)
		{
			CString strItem;
			if (!AfxExtractSubString(strItem, strAll, i, '\n'))
				return;
			pComboBox->AddString(strItem);
		}
	}
}

void FillListBox(CListBox *pListBox, UINT nStringID)
{
	CString strAll;
	if (strAll.LoadString(nStringID))
	{
		for (int i = 0; ; i++)
		{
			CString strItem;
			if (!AfxExtractSubString(strItem, strAll, i, '\n'))
				return;
			pListBox->AddString(strItem);
		}
	}
}
