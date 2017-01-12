#pragma once
#include "ShellTree.h"

// CResourceExplorerTree
class CResourceExplorerTree : public CShellTree
{
//	DECLARE_DYNAMIC(CResourceExplorerTree)
	
public:
	CResourceExplorerTree();
	virtual ~CResourceExplorerTree();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void PopulateTree();
	void AppendStockResourcesTreeItem(LPCTSTR lpszItemText);
	afx_msg void OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
	void GotoFolder(int nFolder);
	BOOL GetFolderPath(HTREEITEM hItem, CString& strFolderPath);
	BOOL FolderIncludeFileType(LPSHELLFOLDER lpsf, LPCTSTR fileType = ".swf");

	virtual ULONG GetItemAttrs(LPSHELLFOLDER lpsf, LPITEMIDLIST pidl);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL IsSWFItem(HTREEITEM hItem);
	BOOL IsSWFFile(LPSHELLFOLDER psf, LPITEMIDLIST pidl);
	BOOL IsSWFFile(LPITEMIDLIST pFullIDL);

protected:	
	BOOL PIDLFromPath(const CString &sPath, LPSHELLFOLDER *ppSF, LPITEMIDLIST *ppFullIDL);

private:	
	HANDLE m_hThread;				// secondary thread for animations
	HANDLE m_hTimerEvent;			// signaled at each timer period
	HANDLE m_hRedrawEvent;			// signaled at each population update
	volatile BOOL m_bAbortAnimation;	// request to terminate secondary thread
	BOOL m_bDrawSnapshot;			// whether to draw background during populating
	CBitmap m_bmpSnapshot;			// snapshot bitmap
	UINT m_nTimerID;				// animation timer id
	UINT m_nTimerDelay;				// animation timer period (ms)
	CString m_sWaitMsg;				// text for the wait message
	HICON m_hIconMsg;				// icon for the wait message
	BOOL m_bShowWaitMsg;			// wether to show the wait message
	int m_iItemIndex;				// population progress index
	int m_iItemCount;				// population progress max index
	HTREEITEM m_hItemMsg;			// wait message item
	HTREEITEM m_hItemToPopulate;	// item being populated	

	HBRUSH m_hBrush;
	CRect m_rcIcon;
	int m_iAniStep;	
	CRect m_rcBorder;
	CRect m_rcProg;
	HBRUSH m_hBorderBrush;
	HBRUSH m_hFillBrush;
	CImageList m_ilHourglass;

	// secondary thread entry point
	static DWORD WINAPI AnimationThreadProc(LPVOID pThis);
	
	void DestroySnapshot();
	void DrawSnapshot(CDC* pDC);
	void TakeSnapshot();

	void StartAnimation();		// set up animation thread
	void StopAnimation();		// animation clean up
	
	void PreExpandItem(HTREEITEM hItem);	// before expanding
	void ExpandItem(HTREEITEM hItem);		// after expanded

	BOOL NeedsChildren(HTREEITEM hParent);	// true if no child items
	void DeleteChildren(HTREEITEM hParent);

	void DrawUserIcon();	// draw wait message icon

public:
	void RefreshSubItems(HTREEITEM hParent);
	void SetWaitMessage(LPCTSTR pszText, HICON hIcon = NULL);
	void ShowWaitMessage()
	{
		m_bShowWaitMsg = TRUE;
	};

protected:
	// animation functions (with timer)
	void SetAnimationDelay(UINT nMilliseconds);

	// animation functions (with or without timer)
	virtual void PreAnimation(HTREEITEM hItemMsg);
	virtual void DoAnimation(BOOL bTimerEvent, int iMaxSteps, int iStep);
	virtual void PostAnimation();
	BOOL GetItemImageRect(HTREEITEM hItem, LPRECT pRect);

	// tree content functions
	void PopulateRoot();
	virtual BOOL WantsRefresh(HTREEITEM hItem);
	virtual BOOL PopulateItem(HTREEITEM hParent);

	// tree content functions (for animations without timer)
	int GetPopulationCount(int *piMaxSubItems = NULL);
	void SetPopulationCount(int iMaxSubItems, int iFirstSubItem = 0);
	void IncreasePopulation(int iSubItemsToAdd = 1);
	void UpdatePopulation(int iSubItems);

	int GetStockResourceImage();
	int m_nStockResourceImage;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
};
