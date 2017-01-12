////////////////////////////////////////////////////////////////
// 1999 Microsoft Systems Journal. 
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 5.0+ on Windows 9x
//

#ifndef __MRUMGR_H__
#define __MRUMGR_H__

#include <afxadv.h>

////////////////////////////////////////////////////////////////////////
// CMruFileManager - class to manage multiple recent file lists in MFC

// Function to test whether file belongs to a particular MRU file list
typedef BOOL (CALLBACK* MRUFILEFN)(LPCTSTR);

//////////////////
// Modified CRecentFileList adds ID range and fixes a bug in MFC.
//
class CRecentFileList2 : public CRecentFileList {
public:
	UINT			m_nBaseID;					 // base command ID
	MRUFILEFN	m_pMruFn;					 // function to test filename

	CRecentFileList2(UINT nBaseID,
		LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat,
		MRUFILEFN pMruFn,
		int nMaxMRU = 4,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN,
		UINT nStart = 0);
	~CRecentFileList2();

	virtual void UpdateMenu(CCmdUI* pCmdUI);
	virtual BOOL IsMyKindOfFile(LPCTSTR lpszPathName);
};

//////////////////
// Manager class manages multiple recent file lists. To use:
//
//		* instantiate one of these in your app;
//		* override your app's OnCmdMsg to pass to mru file manager;
//		* override your app's AddToRecentFileList to call mru manager;
//		* in your InitInstance, call Add to add each type of file.
//
class CMruFileManager : public CCmdTarget {
public:
	CMruFileManager();
	virtual ~CMruFileManager();

	DECLARE_SINGLETON(CMruFileManager)

	// call this from your app's AddToRecentFileList, return if TRUE
	BOOL AddToRecentFileList(LPCTSTR lpszPathName);

	// call from InitInstance to add each MRU file type (eg, file/project)
	BOOL Add(UINT nBaseID,
		LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat,
		MRUFILEFN pMruFn,
		UINT nMaxMRU = 4,
		BOOL bNoLoad = FALSE,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN, UINT nStart = 0);
	BOOL Remove(DWORD dwRfl);

protected:
	CPtrList	m_listRfl;						 // list of CRecentFileList2's

	CRecentFileList2* FindRFL(UINT nID);

	DECLARE_DYNAMIC(CMruFileManager)
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnOpenRecentFile(UINT nID);
};

#endif