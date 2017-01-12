////////////////////////////////////////////////////////////////
// 1999 Microsoft Systems Journal. 
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//
// Compiles with Visual C++ 5.0 on Windows 9x
// 

// Reversion: 2.0
// Author: Chen Hao, SourceTec Software Co., LTD

////////////////////////////////////////////////////////////////////////
// CMruFileManager - class to manage multiple recent file lists in MFC

#include "StdAfx.h"
#include "Resource.h"
#include "MruMgr.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SINGLETON(CMruFileManager)
IMPLEMENT_DYNAMIC(CMruFileManager, CCmdTarget)

BEGIN_MESSAGE_MAP(CMruFileManager, CCmdTarget)
	// MRU - most recently used file menu
	ON_UPDATE_COMMAND_UI_RANGE(ID_MY_RECENT_FILES, ID_MY_RECENT_FILES9, OnUpdateRecentFileMenu)
	ON_COMMAND_EX_RANGE(ID_MY_RECENT_FILES, ID_MY_RECENT_FILES9, OnOpenRecentFile)
END_MESSAGE_MAP()

CMruFileManager::CMruFileManager()
{
}

//////////////////
// Cleanup: destroy all the recent file lists
//
CMruFileManager::~CMruFileManager()
{
	while (!m_listRfl.IsEmpty())
	{
		CRecentFileList2* prfl = (CRecentFileList2*)m_listRfl.RemoveHead();
		prfl->WriteList();
		delete prfl;
	}
}

//////////////////
// Add a recent file list to the MRU manager
//
BOOL CMruFileManager::Add(UINT nBaseID,	// base command ID
	LPCTSTR lpszSection,							// registry section (keyname)
	LPCTSTR lpszEntryFormat,					// registry value name
	MRUFILEFN pMruFn,								// filter function
	UINT nMaxMRU,									// max num menu entries
	BOOL bNoLoad,									// don't load from reg (rarely used)
	int nMaxDispLen,								// display length
	UINT nStart)									// size to start
{
	CRecentFileList2* prfl = new CRecentFileList2(nBaseID,
		lpszSection, lpszEntryFormat, pMruFn, nMaxMRU, nMaxDispLen, nStart);
	if (!bNoLoad)
		prfl->ReadList();						 // load from registry
	return (m_listRfl.AddTail(prfl) != NULL);				 // add to my list
}

//////////////////
// Remove a recent file list. WARNING: never tested!!
//
BOOL CMruFileManager::Remove(DWORD dwRfl)
{
	POSITION pos = m_listRfl.Find((void*)dwRfl);
	if (pos)
	{
		m_listRfl.RemoveAt(pos);
		return TRUE;
	}
	return FALSE;
}

//////////////////
// Update recent file menu; that is, convert menuitem to
// sequence of items with each file name.
//
void CMruFileManager::OnUpdateRecentFileMenu(CCmdUI* pCmdUI)
{
	CRecentFileList2* prfl = FindRFL(pCmdUI->m_nID);
	if (prfl)
	{
		pCmdUI->Enable(prfl->GetSize()>0);
		prfl->UpdateMenu(pCmdUI);
	}
	else
	{
		pCmdUI->ContinueRouting();
	}
}

//////////////////
// Open one of the recent files
//
BOOL CMruFileManager::OnOpenRecentFile(UINT nID)
{
	CRecentFileList2* prfl = FindRFL(nID);
	if (prfl)
	{
		int nIndex = nID - prfl->m_nBaseID; // index of file name in list
		ASSERT((*prfl)[nIndex].GetLength() != 0);
		LPCTSTR lpszFile = (*prfl)[nIndex];
		TRACE2("CMruFileManager: open file (%d) '%s'.\n", nIndex + 1, lpszFile);

		if (!::PathFileExists(lpszFile))
		{
			AfxMessageBoxEx(MB_ICONWARNING, IDS_FILE_NOT_EXIST1, lpszFile);
			prfl->Remove(nIndex);
			return FALSE;
		}

		if (AfxGetApp()->OpenDocumentFile(lpszFile) == NULL)
		{
			// if user the previous document is not saved, and user 
			// select "Cancel" from the save prompt dialog, FALSE will 
			// be returned, too. We should not remove the file from the MRU
			// in this case.
			//prfl->Remove(nIndex);
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

//////////////////
// Helper to find the recent file list that goes with a command ID
//
CRecentFileList2* CMruFileManager::FindRFL(UINT nID)
{
	POSITION pos = m_listRfl.GetHeadPosition();
	while (pos)
	{
		CRecentFileList2* prfl = (CRecentFileList2*)m_listRfl.GetNext(pos);
		if (prfl->m_nBaseID <= nID && nID < prfl->m_nBaseID + prfl->GetSize())
			return prfl;
	}
	return NULL;
}

// MFC private fn, but that's no reason not to call it!
extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

//////////////////
// Add a file name to one of the mru lists. Return FALSE if it doesn't belong.
//
BOOL CMruFileManager::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// fully qualify the path name
	TCHAR szTemp[_MAX_PATH];
	AfxFullPath(szTemp, lpszPathName);

	POSITION pos = m_listRfl.GetHeadPosition();
	while (pos)
	{
		CRecentFileList2* prfl = (CRecentFileList2*)m_listRfl.GetNext(pos);
		if (prfl->IsMyKindOfFile(szTemp)) // does it belong here?
		{
			prfl->Add(szTemp);
			return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////
//
//IMPLEMENT_DYNAMIC(CRecentFileList2, CRecentFileList)

CRecentFileList2::CRecentFileList2(UINT nBaseID, LPCTSTR lpszSection,
	LPCTSTR lpszEntryFormat, MRUFILEFN pMruFn,
	int nMaxMRU, int nMaxDispLen, UINT nStart)
		: CRecentFileList(nStart, lpszSection, lpszEntryFormat, nMaxMRU)
{
		m_nBaseID = nBaseID;
		m_pMruFn = pMruFn;
		UNREFERENCED_PARAMETER(nMaxMRU);
		UNREFERENCED_PARAMETER(nMaxDispLen);
}

CRecentFileList2::~CRecentFileList2()
{
}

//////////////////
// Test if file name belongs in my list: call callback fn.
//
BOOL CRecentFileList2::IsMyKindOfFile(LPCTSTR lpszPathName)
{
	return m_pMruFn && (*m_pMruFn)(lpszPathName);
}

//////////////////
// Update the menu
//
void CRecentFileList2::UpdateMenu(CCmdUI* pCmdUI)
{
	// If the menu is really a submenu, MFC can't handle it, so fake out
	// MFC by temporarily setting m_pMenu = m_pSubMenu.
	//
	CMenu* pMenu = pCmdUI->m_pMenu;
	if (pCmdUI->m_pSubMenu)
		pCmdUI->m_pMenu = pCmdUI->m_pSubMenu;
	CRecentFileList::UpdateMenu(pCmdUI);
	if (pCmdUI->m_pSubMenu)
		pCmdUI->m_pMenu = pMenu;
}
