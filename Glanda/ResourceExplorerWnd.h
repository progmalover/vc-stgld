#pragma once
#include "HSplitter.h"
#include "VSplitter.h"
#include "ResourceThumbListCtrl.h"
#include "ResourceDirTreeCtrl.h"
#include "ResourceExplorerTree.h"
#include "FlatBitmapButton.h"
#include <deque>

// CResourceExplorerWnd

static const TCHAR szDefaultImagesDir[]		= "Resource\\default\\Images";
static const TCHAR szDefaultShapesDir[]		= "Resource\\default\\Shapes";
static const TCHAR szDefaultButtonsDir[]	= "Resource\\default\\Buttons";
static const TCHAR szDefaultSoundsDir[]		= "Resource\\default\\Sounds";
static const TCHAR szDefaultGreetingCardsDir[]		= "Resource\\default\\Greeting cards";
static const TCHAR szDefaultPreloadsDir[]		= "Resource\\default\\Preloaders";
static const TCHAR szDefaultBackgroundsDir[]		= "Resource\\default\\Backgrounds";
static const TCHAR szDefaultResourceDir[]	= "Resource\\default";
static const TCHAR szDefaultCacheDir[]		= "Resource\\Cache";

#define minmax(x, a, b) min(max(x, a), b)

class CResourceExplorerWnd : public CDialog
{
	enum
	{
		IDC_THUMB_LIST_CTRL = 1,
	};
	DECLARE_DYNAMIC(CResourceExplorerWnd)

protected:
	CFlatBitmapButton m_btnBack;
	CFlatBitmapButton m_btnForward;
	CFlatBitmapButton m_btnParentDir;
	CFlatBitmapButton m_btnHomeDir;
	CFlatBitmapButton m_btnRefreshDir;
	CFlatBitmapButton m_btnStockResources;
	CFlatBitmapButton m_btnGotoFolder;

	CHSplitter* m_pHSplitter;
	CVSplitter* m_pVSplitter;
	BOOL m_bWholeTree;
	BOOL m_bHorzLayout;
	BOOL m_bShowStockObjects;
	int m_nResourceType;
	CString m_strProfileSection;
	CString m_strResourceDir;
	CString m_strResourceRootDir;

	double	m_fSplitterPos;

	void RecalcLayout(BOOL bTracked);
	void LoadDirSelection();
	void SaveDirSelection();
	void SetResourceDir(LPCTSTR lpszResourceDir);
	void SetLibraryItems();

	class CTreeHistory
	{
		std::deque<HTREEITEM> m_items;
		int m_nPos;
	public:
		CTreeHistory()
		{
			m_nPos = -1;
		}
		~CTreeHistory()
		{
			Reset();
		}
		BOOL CanBackward()
		{
			return m_nPos > 0;
		}
		BOOL CanForward()
			  
		{
			return m_nPos + 1 < (int)m_items.size() && m_nPos >= 0;
		}
		HTREEITEM Backward()
		{
			ASSERT(CanBackward());
			return m_items[--m_nPos];
		}
		HTREEITEM Forward()
		{
			ASSERT(CanForward());
			return m_items[++m_nPos];
		}
		void Reset()
		{
			m_items.clear();
			m_nPos = -1;
		}
		void Sel(HTREEITEM hItem)
		{
			while(CanForward())
			{
				m_items.pop_back();
			}
			m_items.push_back(hItem);
			++m_nPos;
		}
	}m_treeHistory;
	BOOL m_bHistoryBlocked;
	BOOL m_bThumbRefreshBlocked;

	HACCEL m_hAccel;

public:
	CResourceExplorerWnd(int nResourceType, BOOL bHorzLayout, BOOL bWholeTree, BOOL bShowStockObjects = FALSE);
	virtual ~CResourceExplorerWnd();

	enum { IDD = IDD_RESOURCE_EXPLORER_WND	};
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTvnSelchangedDirTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	void SetResourceRootDir(LPCTSTR lpszResourceRootDir);
	void EnableDragResource(BOOL bEnable)
	{
		m_ThumbListCtrl.EnableDragResouurce(bEnable);
	}
	void SetActionObject(CResourceThumbItemAction* pActionObject)
	{
		m_ThumbListCtrl.SetActionObject(pActionObject);
	}
	void SetProfileSection(LPCTSTR lpszSection)
	{
		m_strProfileSection = lpszSection;
	}
	void RefreshDir();
	void EnsureSelectionVisible();

	CRect m_rcDir;
	CRect m_rcThumb;

	CToolTipCtrl m_tooltip;
	CResourceThumbItem* GetSelectedResource();

	CResourceDirTreeCtrl* m_pDirTreeCtrl;
	CResourceExplorerTree* m_pShellTreeCtrl;
	CResourceThumbListCtrl m_ThumbListCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual void OnCancel();

	void UpdateUI();
public:
	afx_msg void OnBnClickedButtonParentDir();
	afx_msg void OnBnClickedButtonHomeDir();
	afx_msg void OnBnClickedButtonRefreshDir();
	afx_msg void OnBnClickedButtonStockResources();
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnBnClickedButtonForward();
	afx_msg void OnBnClickedButtonGotoFolder();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnGotoHomeDirectory();
	afx_msg void OnGotoStockResources();
	afx_msg void OnGotoDesktop();
	afx_msg void OnGotoMyDocuments();
	afx_msg void OnGotoMyComputer();
	afx_msg void OnGotoNetworkPlace();
	afx_msg void OnUpdateThumbPlaySound(CCmdUI *pCmdUI);
	afx_msg void OnUpdateThumbCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThumbDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThumbExplore(CCmdUI* pCmdUI);

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnResourceTreeExplore();
	afx_msg void OnUpdateResourceTreeExplore(CCmdUI* pCmdUI);
	afx_msg void OnResourceTreeRefresh();
	afx_msg void OnUpdateResourceTreeRefresh(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
};


