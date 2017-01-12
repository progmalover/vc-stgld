// ResourceExplorerWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ResourceExplorerWnd.h"
#include "DeferWindowPos.h"
#include "filepath.h"

// CResourceExplorerWnd

IMPLEMENT_DYNAMIC(CResourceExplorerWnd, CDialog)
CResourceExplorerWnd::CResourceExplorerWnd(int nResourceType, BOOL bHorzLayout, BOOL bWholeTree, BOOL bShowStockObjects)
: m_bHorzLayout(bHorzLayout)
, m_bWholeTree(bWholeTree)
, m_nResourceType(nResourceType)
, m_ThumbListCtrl(nResourceType)
, m_bShowStockObjects(bShowStockObjects)
, m_bHistoryBlocked(FALSE)
, m_bThumbRefreshBlocked(FALSE)
, m_hAccel(NULL)
{
	m_rcDir.SetRectEmpty();
	m_rcThumb.SetRectEmpty();

	if(bHorzLayout)
	{
		m_pHSplitter = new CHSplitter();
	}
	else
	{
		m_pVSplitter = new CVSplitter();
	}
	if(bWholeTree)
	{
		m_pShellTreeCtrl = new CResourceExplorerTree();
	}
	else
	{
		m_pDirTreeCtrl = new CResourceDirTreeCtrl();
	}
}

CResourceExplorerWnd::~CResourceExplorerWnd()
{
	if(m_bHorzLayout)
	{
		delete m_pHSplitter;
	}
	else
	{
		delete m_pVSplitter;
	}
	if(m_bWholeTree)
	{
		delete m_pShellTreeCtrl;
	}
	else
	{
		delete m_pDirTreeCtrl;
	}
}


BEGIN_MESSAGE_MAP(CResourceExplorerWnd, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_NOTIFY(TVN_SELCHANGED, IDC_DIR_TREE_CTRL, OnTvnSelchangedDirTreeCtrl)
	ON_BN_CLICKED(IDC_BUTTON_PARENT_DIR, OnBnClickedButtonParentDir)
	ON_BN_CLICKED(IDC_BUTTON_HOME_DIR, OnBnClickedButtonHomeDir)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_DIR, OnBnClickedButtonRefreshDir)
	ON_BN_CLICKED(IDC_BUTTON_STOCK_RESOURCES, OnBnClickedButtonStockResources)
	ON_BN_CLICKED(IDC_BUTTON_BACK, OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_FORWARD, OnBnClickedButtonForward)
	ON_BN_CLICKED(IDC_BUTTON_GOTO_FOLDER, OnBnClickedButtonGotoFolder)
	ON_COMMAND(ID_GOTO_HOME_DIRECTORY, OnGotoHomeDirectory)
	ON_COMMAND(ID_GOTO_STOCK_RESOURCES, OnGotoStockResources)
	ON_COMMAND(ID_GOTO_DESKTOP, OnGotoDesktop)
	ON_COMMAND(ID_GOTO_MY_DOCUMENTS, OnGotoMyDocuments)
	ON_COMMAND(ID_GOTO_MY_COMPUTER, OnGotoMyComputer)
	ON_COMMAND(ID_GOTO_NETWORK_PLACE, OnGotoNetworkPlace)
	ON_UPDATE_COMMAND_UI(ID_THUMB_PLAY_SOUND, OnUpdateThumbPlaySound)
	ON_UPDATE_COMMAND_UI(ID_THUMB_COPY, OnUpdateThumbCopy)
	ON_UPDATE_COMMAND_UI(ID_THUMB_DELETE, OnUpdateThumbDelete)
	ON_UPDATE_COMMAND_UI(ID_THUMB_EXPLORE, OnUpdateThumbExplore)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_RESOURCE_TREE_EXPLORE, OnResourceTreeExplore)
	ON_UPDATE_COMMAND_UI(ID_RESOURCE_TREE_EXPLORE, OnUpdateResourceTreeExplore)
	ON_COMMAND(ID_RESOURCE_TREE_REFRESH, OnResourceTreeRefresh)
	ON_UPDATE_COMMAND_UI(ID_RESOURCE_TREE_REFRESH, OnUpdateResourceTreeRefresh)
	//ON_WM_PAINT()
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()



// CResourceExplorerWnd message handlers


BOOL CResourceExplorerWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetRedraw(FALSE);

	m_btnGotoFolder.LoadBitmap(IDB_BUTTON_GOTO_FOLDER, 2);
	m_btnGotoFolder.ModifyFBStyle(0, FBS_RIGHTTEXT | FBS_DROPDOWN);
	m_btnBack.LoadBitmap(IDB_BUTTON_BACK, 2);
	m_btnForward.LoadBitmap(IDB_BUTTON_FORWARD, 2);
	m_btnParentDir.LoadBitmap(IDB_BUTTON_PARENT_DIR, 2);
	m_btnHomeDir.LoadBitmap(IDB_BUTTON_HOME_DIR, 2);
	m_btnRefreshDir.LoadBitmap(IDB_BUTTON_REFRESH_DIR, 2);
	m_btnStockResources.LoadBitmap(IDB_BUTTON_STOCK_RESOURCES, 2);

	CString str		= AfxGetApp()->GetProfileString(m_strProfileSection, "SplitterPosFloat", "0.5");
	m_fSplitterPos	= atof(str);
	m_fSplitterPos	= minmax(m_fSplitterPos, 0.1, 0.9);

	if(m_bWholeTree)
	{
		m_pShellTreeCtrl->MoveWindow(&m_rcDir);
		m_btnBack.ShowWindow(SW_SHOW);
		m_btnForward.ShowWindow(SW_SHOW);
		m_btnParentDir.ShowWindow(SW_SHOW);
		m_btnHomeDir.ShowWindow(SW_SHOW);
		m_btnRefreshDir.ShowWindow(SW_SHOW);
		if(m_bShowStockObjects)
		{
			m_btnStockResources.ShowWindow(SW_SHOW);
			m_btnGotoFolder.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_pDirTreeCtrl->MoveWindow(&m_rcDir);
	}
	m_ThumbListCtrl.Create(WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, IDC_THUMB_LIST_CTRL);

	CTreeCtrl *pTreeCtrl = (m_bWholeTree ?  (CTreeCtrl *)m_pShellTreeCtrl : (CTreeCtrl *)m_pDirTreeCtrl);
	if (m_bHorzLayout)
	{
		pTreeCtrl->ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
		m_ThumbListCtrl.ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	}
	else
	{
		pTreeCtrl->ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
		m_ThumbListCtrl.ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
	}

	int nTextHeight = 12;
	CFont *pFont = GetFont();
	if (pFont == NULL)
		pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	if (pFont)
	{
		CClientDC dc(this);
		CFont *pFontOld = dc.SelectObject(pFont);
		TEXTMETRIC tm;
		memset(&tm, 0, sizeof(tm));
		dc.GetTextMetrics(&tm);
		dc.SelectObject(pFont);

		nTextHeight = tm.tmHeight;
	}
	m_ThumbListCtrl.SetItemSize(56, 56 + 4 + nTextHeight);

	if(m_bWholeTree)
	{
		m_pShellTreeCtrl->EnableImages();
		m_pShellTreeCtrl->PopulateTree();
		if(m_bShowStockObjects)
		{
			CString str;
			str.LoadString(IDS_STOCK_RESOURCES_TREE_ITEM_TEXT);
			m_pShellTreeCtrl->AppendStockResourcesTreeItem(str);
		}
	}
	else
	{
		m_pDirTreeCtrl->LoadImageList();
	}

	SetResourceRootDir(m_strResourceRootDir);

	RecalcLayout(FALSE);

	SetRedraw(TRUE);
	Invalidate();

	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	#define ADDTIP(nID)											\
	{															\
		CString strText;										\
		GetDlgItemText(nID,strText);							\
		VERIFY(m_tooltip.AddTool(GetDlgItem(nID), strText));	\
	}
	ADDTIP(IDC_BUTTON_BACK);
	ADDTIP(IDC_BUTTON_FORWARD);
	ADDTIP(IDC_BUTTON_PARENT_DIR);
	ADDTIP(IDC_BUTTON_HOME_DIR);
	ADDTIP(IDC_BUTTON_REFRESH_DIR);
	ADDTIP(IDC_BUTTON_GOTO_FOLDER);
	ADDTIP(IDC_BUTTON_STOCK_RESOURCES);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_GOTO_FOLDER), IDS_BUTTON_TIP_GOTO_FOLDER);

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_RESOURCE_EXPLORER));

	return TRUE;
}

void CResourceExplorerWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(::IsWindow(m_ThumbListCtrl.m_hWnd) && cx > 0 && cy > 0)
	{
		RecalcLayout(FALSE);
	}
}

void CResourceExplorerWnd::OnDestroy()
{
	CDialog::OnDestroy();

	SaveDirSelection();
	CString str;
	str.Format("%f", m_fSplitterPos);
	AfxGetApp()->WriteProfileString(m_strProfileSection, "SplitterPosFloat", str);
}

void CResourceExplorerWnd::RecalcLayout(BOOL bTracked)
{
	CRect rc;
	GetClientRect(&rc);

	CRect rcButton;
	m_btnGotoFolder.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);
	rcButton.OffsetRect(rc.right - rcButton.right, 0);

	if(m_bWholeTree)
	{
		rc.top = rcButton.bottom + 3;
	}

	// Hide left & right & bottom edge
	if (!m_bHorzLayout && !m_bWholeTree)
	{
		rc.left--;
		rc.right++;
		rc.bottom++;
	}

	m_rcDir = rc;
	m_rcThumb = rc;

	if(m_bHorzLayout)
	{
		int nPos = m_pHSplitter->left;
		if(bTracked)
		{
			m_fSplitterPos = double(nPos - rc.left) / double(rc.Width() - 5);
		}
		else
		{
			nPos = int(m_fSplitterPos * double(rc.Width() - 5) + 0.5 + double(rc.left));
			//ASSERT(nPos >= rc.left && nPos <= rc.right);
		}

		m_pHSplitter->SetSize(5);
		m_pHSplitter->SetRange(rc.left, rc.right);
		m_pHSplitter->SetRect(nPos, rc.top, nPos + 5, rc.bottom);

		m_rcDir.right = nPos;
		m_rcThumb.left = nPos + 5;
	}
	else
	{
		int nPos = m_pVSplitter->top;
		if(bTracked)
		{
			m_fSplitterPos = double(nPos - rc.top) / double(rc.Height() - 5);
		}
		else
		{
			nPos = int(m_fSplitterPos * double(rc.Height() - 5) + 0.5 + double(rc.top));
			//ASSERT(nPos >= rc.top && nPos <= rc.bottom);
		}

		m_pVSplitter->SetSize(5);
		m_pVSplitter->SetRange(rc.top, rc.bottom);
		m_pVSplitter->SetRect(rc.left, nPos, rc.right, nPos + 5);

		m_rcDir.bottom = nPos;
		m_rcThumb.top = nPos + 5;
	}

	CDeferWindowPos dw;
	dw.BeginDeferWindowPos();
	dw.DeferWindowPos(m_ThumbListCtrl.m_hWnd, NULL, m_rcThumb, SWP_NOZORDER);
	dw.DeferWindowPos(m_btnGotoFolder.m_hWnd, NULL, rcButton, SWP_NOSIZE | SWP_NOZORDER);
	if(m_bWholeTree)
	{
		dw.DeferWindowPos(m_pShellTreeCtrl->m_hWnd, NULL, m_rcDir, SWP_NOZORDER);
	}
	else
	{
		dw.DeferWindowPos(m_pDirTreeCtrl->m_hWnd, NULL, m_rcDir, SWP_NOZORDER);
	}
	dw.EndDeferWindowPos();
}

void CResourceExplorerWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bHorzLayout)
	{
		if(PtInRect(m_pHSplitter, point))
		{
			if (m_pHSplitter->Track(this, point))
			{
				RecalcLayout(TRUE);
			}
			return;
		}
	}
	else
	{
		if(PtInRect(m_pVSplitter, point))
		{
			if (m_pVSplitter->Track(this, point))
			{
				RecalcLayout(TRUE);
			}
			return;
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CResourceExplorerWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if(m_bHorzLayout)
	{
		if (PtInRect(m_pHSplitter, point))
		{
			::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR));
			return TRUE;
		}
	}
	else
	{
		if (PtInRect(m_pVSplitter, point))
		{
			::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR));
			return TRUE;
		}
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CResourceExplorerWnd::LoadDirSelection()
{
	if (::IsWindow(m_hWnd))
	{
		if(m_bWholeTree)
		{
			m_pShellTreeCtrl->SelectItem(NULL);
		}
		else
		{
			m_pDirTreeCtrl->SelectItem(NULL);
		}

		m_treeHistory.Reset();
		LPITEMIDLIST pidl = NULL;
		UINT uSize = 0;

		if (m_bWholeTree)
		{
			HTREEITEM hItem = NULL;

			if(m_bShowStockObjects && AfxGetApp()->GetProfileInt(m_strProfileSection, "LastDirIsStockResource", 0))
			{
				hItem = m_pShellTreeCtrl->GetNextSiblingItem(m_pShellTreeCtrl->GetRootItem());
				ASSERT(hItem);
				m_pShellTreeCtrl->SelectItem(hItem);
			}
			else if(AfxGetApp()->GetProfileBinary(m_strProfileSection, "LastDir", (BYTE **)&pidl, &uSize))
			{
				AfxGetApp()->WriteProfileBinary(m_strProfileSection, "LastDir", NULL, 0);
				if(uSize > 0)
				{
					hItem = m_pShellTreeCtrl->Locate(pidl);
				}
				else
				{
					hItem = m_pShellTreeCtrl->GetRootItem();
					ASSERT(hItem);
					m_pShellTreeCtrl->SelectItem(hItem);
				}
			}


			if (pidl != NULL)
			{
				delete[] pidl;
			}

			if (hItem == NULL)	// 无选中项,要选中默认的目录初始化
			{
				m_pShellTreeCtrl->Locate(m_strResourceRootDir);				
			}
		}
		else
		{
			CString strLastDir = AfxGetApp()->GetProfileString(m_strProfileSection, "LastDir", m_strResourceRootDir);
			HTREEITEM hItem = m_pDirTreeCtrl->ItemFromDirectory(strLastDir);
			if(hItem)
			{
				m_pDirTreeCtrl->SelectItem(hItem);
			}
			else
			{
				m_pDirTreeCtrl->SelectItem(m_pDirTreeCtrl->GetRootItem());
			}
		}
	}
}

void CResourceExplorerWnd::SaveDirSelection()
{
	if (m_bWholeTree)
	{
		HTREEITEM hRoot		= m_pShellTreeCtrl->GetRootItem();
		HTREEITEM hStock	= m_pShellTreeCtrl->GetNextSiblingItem(hRoot);
		HTREEITEM hItem		= m_pShellTreeCtrl->GetSelectedItem();
		
		if (hItem != NULL)
		{
			if(m_bShowStockObjects && hItem == hStock)
			{
				AfxGetApp()->WriteProfileInt(m_strProfileSection, "LastDirIsStockResource", 1);
			}
			else
			{
				AfxGetApp()->WriteProfileInt(m_strProfileSection, "LastDirIsStockResource", 0);

				HTREEITEM hParent = m_pShellTreeCtrl->GetParentItem(hItem);
				if (hParent != NULL)
				{
					if (m_pShellTreeCtrl->IsSWFItem(hParent)) // if swf export cache directory
						hItem = hParent;
				}

				CShellTree::ITEMDATA *pid = (CShellTree::ITEMDATA *)(m_pShellTreeCtrl->GetItemData(hItem));

				if (pid != NULL)
				{
					AfxGetApp()->WriteProfileBinary(m_strProfileSection, "LastDir", (BYTE *)pid->lpifq, m_pShellTreeCtrl->GetSize(pid->lpifq));
				}
				else
				{
					AfxGetApp()->WriteProfileBinary(m_strProfileSection, "LastDir", NULL, 0);
				}
			}
		}
	}
	else
	{
		AfxGetApp()->WriteProfileString(m_strProfileSection, "LastDir", m_strResourceDir);
	}
}

void CResourceExplorerWnd::SetResourceDir(LPCTSTR lpszResourceDir)
{
	m_strResourceDir = lpszResourceDir;

	CWaitCursor wc;
	if(m_bThumbRefreshBlocked++==FALSE)
	{
		m_ThumbListCtrl.SetRedraw(FALSE);
	}
	m_ThumbListCtrl.SetResourceDir(lpszResourceDir);
	if(--m_bThumbRefreshBlocked==FALSE)
	{
		m_ThumbListCtrl.SetRedraw(TRUE);
		m_ThumbListCtrl.Invalidate();
	}
	m_ThumbListCtrl.UpdateWindow();

	if(m_bHistoryBlocked==FALSE)
	{
		CTreeCtrl* pTreeCtrl = m_bWholeTree ? (CTreeCtrl*)m_pShellTreeCtrl : (CTreeCtrl*)m_pDirTreeCtrl;
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if(hItem)
		{
			m_treeHistory.Sel(hItem);
		}
	}

	UpdateUI();
}

void CResourceExplorerWnd::SetLibraryItems()
{
	CWaitCursor wc;
	if(m_bThumbRefreshBlocked++==FALSE)
	{
		m_ThumbListCtrl.SetRedraw(FALSE);
	}
	m_ThumbListCtrl.SetLibraryItems();
	if(--m_bThumbRefreshBlocked==FALSE)
	{
		m_ThumbListCtrl.SetRedraw(TRUE);
		m_ThumbListCtrl.Invalidate();
	}
	m_ThumbListCtrl.UpdateWindow();

	HTREEITEM hItem = m_pShellTreeCtrl->GetSelectedItem();
	if(m_bHistoryBlocked==FALSE)
	{
		if(hItem)
		{
			m_treeHistory.Sel(hItem);
		}
	}

	UpdateUI();
}

void CResourceExplorerWnd::OnTvnSelchangedDirTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CString strPath;
	if(m_bWholeTree)
	{
		HTREEITEM hItem = m_pShellTreeCtrl->GetSelectedItem();
		if(hItem)
		{
			CShellTree::ITEMDATA *pid = (CShellTree::ITEMDATA *)(m_pShellTreeCtrl->GetItemData(hItem));
			if(!pid)
			{
				ASSERT(m_bShowStockObjects);
				SetLibraryItems();
			}
			else if(m_pShellTreeCtrl->GetSelectedFolderPath(strPath) && !strPath.IsEmpty())
			{
				SetResourceDir(strPath);
			}
			else
			{
				SetResourceDir("");
			}
		}
		else
		{
			SetResourceDir("");
		}
	}
	else
	{
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		if(hItem)
		{
			DWORD_PTR data = m_pDirTreeCtrl->GetItemData(hItem);
			CString str = *(CString*)data;
			SetResourceDir(str);
		}
		else
		{
			SetResourceDir("");
		}
	}

	*pResult = 0;
}

void CResourceExplorerWnd::SetResourceRootDir(LPCTSTR lpszResourceRootDir)
{
	ASSERT(_tcslen(lpszResourceRootDir));

	m_strResourceRootDir = lpszResourceRootDir;
	if(::IsWindow(m_hWnd))
	{
		if(!m_bWholeTree)
		{
			m_pDirTreeCtrl->SetResourceRootDir(m_strResourceRootDir);
		}
		LoadDirSelection();
	}
}

CResourceThumbItem* CResourceExplorerWnd::GetSelectedResource()
{
	int nIndex = m_ThumbListCtrl.GetCurSel();
	if(nIndex >= 0)
	{
		DWORD_PTR data = m_ThumbListCtrl.GetItemData(nIndex);
		return (CResourceThumbItem*)data;
	}
	return NULL;
}

void CResourceExplorerWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_BACK, m_btnBack);
	DDX_Control(pDX, IDC_BUTTON_FORWARD, m_btnForward);
	DDX_Control(pDX, IDC_BUTTON_PARENT_DIR, m_btnParentDir);
	DDX_Control(pDX, IDC_BUTTON_HOME_DIR, m_btnHomeDir);
	DDX_Control(pDX, IDC_BUTTON_REFRESH_DIR, m_btnRefreshDir);
	DDX_Control(pDX, IDC_BUTTON_STOCK_RESOURCES, m_btnStockResources);
	DDX_Control(pDX, IDC_BUTTON_GOTO_FOLDER, m_btnGotoFolder);
	if(m_bWholeTree)
	{
		DDX_Control(pDX, IDC_DIR_TREE_CTRL, *m_pShellTreeCtrl);
	}
	else
	{
		DDX_Control(pDX, IDC_DIR_TREE_CTRL, *m_pDirTreeCtrl);
	}
}

void CResourceExplorerWnd::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CResourceExplorerWnd::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CResourceExplorerWnd::OnBnClickedButtonParentDir()
{
	CTreeCtrl* pTreeCtrl;
	if(m_bWholeTree)
	{
		pTreeCtrl = m_pShellTreeCtrl;
	}
	else
	{
		pTreeCtrl = m_pDirTreeCtrl;
	}

	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	if(hItem)
	{
		hItem = pTreeCtrl->GetParentItem(hItem);
		if(hItem)
		{
			pTreeCtrl->SelectItem(hItem);
		}
	}
}

void CResourceExplorerWnd::OnBnClickedButtonHomeDir()
{
	OnGotoHomeDirectory();
}

void CResourceExplorerWnd::OnBnClickedButtonRefreshDir()
{
	RefreshDir();
}

void CResourceExplorerWnd::OnBnClickedButtonStockResources()
{
	OnGotoStockResources();
}

void CResourceExplorerWnd::OnBnClickedButtonBack()
{
	if(m_treeHistory.CanBackward())
	{
		HTREEITEM hItem = m_treeHistory.Backward();
		if(hItem)
		{
			m_bHistoryBlocked++;
			if(m_bWholeTree)
			{
				m_pShellTreeCtrl->SelectItem(hItem);
			}
			else
			{
				m_pDirTreeCtrl->SelectItem(hItem);
			}
			m_bHistoryBlocked--;
		}
	}
}

void CResourceExplorerWnd::OnBnClickedButtonForward()
{
	if(m_treeHistory.CanForward())
	{
		HTREEITEM hItem = m_treeHistory.Forward();
		m_bHistoryBlocked++;
		if(hItem)
		{
			if(m_bWholeTree)
			{
				m_pShellTreeCtrl->SelectItem(hItem);
			}
			else
			{
				m_pDirTreeCtrl->SelectItem(hItem);
			}
		}
		m_bHistoryBlocked--;
	}
}

void CResourceExplorerWnd::OnBnClickedButtonGotoFolder()
{
	CRect rc;
	GetDlgItem(IDC_BUTTON_GOTO_FOLDER)->GetWindowRect(&rc);

	CMenu menu;
	menu.LoadMenu(IDR_POPUP_RESOURCE_GOTO_FOLDER);


	CMenu* pPopup = menu.GetSubMenu(0);
	if(pPopup)
	{
		const UINT ITEM_GOTO_STOCK_RESOURCES	= 2;
		ASSERT(pPopup->GetMenuItemID(ITEM_GOTO_STOCK_RESOURCES)==ID_GOTO_STOCK_RESOURCES);
		if(m_bShowStockObjects == FALSE)
		{
			pPopup->DeleteMenu(ITEM_GOTO_STOCK_RESOURCES + 1, MF_BYPOSITION);
			pPopup->DeleteMenu(ITEM_GOTO_STOCK_RESOURCES, MF_BYPOSITION);
		}

		m_btnGotoFolder.SetCheck(BST_CHECKED);
		pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTALIGN | TPM_RIGHTBUTTON, rc.right, rc.bottom, this);
		m_btnGotoFolder.SetCheck(BST_UNCHECKED);
	}
}

BOOL CResourceExplorerWnd::PreTranslateMessage(MSG* pMsg)
{
	if(::IsWindow(m_tooltip.m_hWnd))
	{
		if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
			m_tooltip.RelayEvent(pMsg);
	}

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;

	// return FALSE so the parent can process the accelerators.
	return CWnd::PreTranslateMessage(pMsg);
}

void CResourceExplorerWnd::OnGotoHomeDirectory()
{
	HTREEITEM hItem = m_pShellTreeCtrl->Locate(m_strResourceRootDir);
	if(hItem && m_pShellTreeCtrl->ItemHasChildren(hItem))
	{
		m_pShellTreeCtrl->Expand(hItem, TVE_EXPAND);
	}
}

void CResourceExplorerWnd::OnGotoStockResources()
{
	HTREEITEM hItem = m_pShellTreeCtrl->GetRootItem();
	hItem = m_pShellTreeCtrl->GetNextSiblingItem(hItem);
	ASSERT(hItem);
	m_pShellTreeCtrl->SelectItem(hItem);
}

void CResourceExplorerWnd::OnGotoDesktop()
{
	if(m_bWholeTree)
	{
		m_pShellTreeCtrl->GotoFolder(CSIDL_DESKTOP);
	}
}

void CResourceExplorerWnd::OnGotoMyDocuments()
{
	if(m_bWholeTree)
	{
		m_pShellTreeCtrl->GotoFolder(CSIDL_PERSONAL);
	}
}

void CResourceExplorerWnd::OnGotoMyComputer()
{
	if(m_bWholeTree)
	{
		m_pShellTreeCtrl->GotoFolder(CSIDL_DRIVES);
	}
}

void CResourceExplorerWnd::OnGotoNetworkPlace()
{
	if(m_bWholeTree)
	{
		m_pShellTreeCtrl->GotoFolder(CSIDL_NETWORK);
	}
}

void CResourceExplorerWnd::OnUpdateThumbPlaySound(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CResourceExplorerWnd::OnUpdateThumbCopy(CCmdUI* pCmdUI)
{
	if(::IsWindow(m_ThumbListCtrl.m_hWnd))
	{
		int nCurSel = m_ThumbListCtrl.GetCurSel();
		if(nCurSel < 0)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CResourceThumbItem* pThumbItem = (CResourceThumbItem*)m_ThumbListCtrl.GetItemData(nCurSel);
			pCmdUI->Enable(pThumbItem->m_bStockResource==FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CResourceExplorerWnd::OnUpdateThumbDelete(CCmdUI* pCmdUI)
{
	if(::IsWindow(m_ThumbListCtrl.m_hWnd))
	{
		int nCurSel = m_ThumbListCtrl.GetCurSel();
		if(nCurSel < 0)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CResourceThumbItem* pThumbItem = (CResourceThumbItem*)m_ThumbListCtrl.GetItemData(nCurSel);
			pCmdUI->Enable(pThumbItem->m_bStockResource==FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CResourceExplorerWnd::OnUpdateThumbExplore(CCmdUI* pCmdUI)
{
	if(::IsWindow(m_ThumbListCtrl.m_hWnd))
	{
		int nCurSel = m_ThumbListCtrl.GetCurSel();
		if(nCurSel < 0)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CResourceThumbItem* pThumbItem = (CResourceThumbItem*)m_ThumbListCtrl.GetItemData(nCurSel);
			pCmdUI->Enable(pThumbItem->m_bStockResource==FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CResourceExplorerWnd::UpdateUI()
{
	m_btnBack.EnableWindow(m_treeHistory.CanBackward());
	m_btnForward.EnableWindow(m_treeHistory.CanForward());
	CTreeCtrl* pTreeCtrl = m_bWholeTree ? (CTreeCtrl*)m_pShellTreeCtrl : (CTreeCtrl*)m_pDirTreeCtrl;
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	m_btnParentDir.EnableWindow(hItem && pTreeCtrl->GetParentItem(hItem));
}

void CResourceExplorerWnd::RefreshDir()
{
	if (m_bWholeTree)
	{
		m_pShellTreeCtrl->SetRedraw(FALSE);
		if(m_bThumbRefreshBlocked++==FALSE)
		{
			m_ThumbListCtrl.SetRedraw(FALSE);
		}

		SaveDirSelection();

		m_pShellTreeCtrl->DeleteAllItems();
		m_pShellTreeCtrl->PopulateTree();
		if (m_bShowStockObjects)
		{
			CString str;
			str.LoadString(IDS_STOCK_RESOURCES_TREE_ITEM_TEXT);
			m_pShellTreeCtrl->AppendStockResourcesTreeItem(str);
		}
		LoadDirSelection();

		m_pShellTreeCtrl->SetRedraw(TRUE);
		m_pShellTreeCtrl->Invalidate(FALSE);

		if(--m_bThumbRefreshBlocked==FALSE)
		{
			m_ThumbListCtrl.SetRedraw(TRUE);
			m_ThumbListCtrl.Invalidate();
		}
		m_ThumbListCtrl.UpdateWindow();
	}
	else
	{
		m_pDirTreeCtrl->SetRedraw(FALSE);
		if(m_bThumbRefreshBlocked++==FALSE)
		{
			m_ThumbListCtrl.SetRedraw(FALSE);
		}

		SaveDirSelection();
		m_pDirTreeCtrl->SetResourceRootDir(m_strResourceRootDir);
		LoadDirSelection();

		m_pDirTreeCtrl->SetRedraw(TRUE);
		m_pDirTreeCtrl->Invalidate(FALSE);

		if(--m_bThumbRefreshBlocked==FALSE)
		{
			m_ThumbListCtrl.SetRedraw(TRUE);
			m_ThumbListCtrl.Invalidate();
		}
		m_ThumbListCtrl.UpdateWindow();
	}
}

void CResourceExplorerWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{

	CTreeCtrl* pTreeCtrl = m_bWholeTree ? (CTreeCtrl*)m_pShellTreeCtrl : (CTreeCtrl*)m_pDirTreeCtrl;
	if(pWnd == pTreeCtrl)
	{
		HTREEITEM hItemHilight = pTreeCtrl->GetNextItem(TVI_ROOT, TVGN_DROPHILITE);

		CPoint ptClient = point;
		pTreeCtrl->ScreenToClient(&ptClient);

		CRect rc;
		GetClientRect(&rc);
		if(rc.PtInRect(ptClient))
		{
			UINT nFlags;
			HTREEITEM hItem = pTreeCtrl->HitTest(ptClient, &nFlags);
			if (hItem && (nFlags & TVHT_ONITEM))
			{
				pTreeCtrl->Select(hItem, TVGN_DROPHILITE);
			}

			CMenu menu;
			if (menu.LoadMenu(IDR_POPUP_RESOURCE_TREE))
			{
				CMenu *pMenu = menu.GetSubMenu(0);
				if (pMenu)
				{
					UINT nID = pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
					if (nID > 0)
						SendMessage(WM_COMMAND, MAKEWPARAM(nID, 0), 0L);
				}
			}

			if (pTreeCtrl->GetNextItem(TVI_ROOT, TVGN_DROPHILITE) != hItemHilight)
				pTreeCtrl->Select(hItemHilight, TVGN_DROPHILITE);
		}
	}
}

void CResourceExplorerWnd::OnResourceTreeExplore()
{
	if(m_bWholeTree)
	{
		HTREEITEM hItem = m_pShellTreeCtrl->GetNextItem(TVI_ROOT, TVGN_DROPHILITE);
		if(hItem && m_pShellTreeCtrl->GetItemData(hItem))
		{
			CString strPath;
			if(m_pShellTreeCtrl->GetFolderPath(hItem, strPath))
			{
				CString strCommand;
				if(m_pShellTreeCtrl->IsSWFItem(hItem))
				{
					strCommand.Format("/n,/select,%s", (LPCTSTR)strPath);
				}
				else
				{
					strCommand.Format("/e, \"%s\"", (LPCTSTR)strPath);
				}

				ShellExecute(NULL, "open", "explorer.exe", strCommand, NULL, SW_SHOWNORMAL);
			}
		}
	}
	else
	{
		HTREEITEM hItem = m_pDirTreeCtrl->GetNextItem(TVI_ROOT, TVGN_DROPHILITE);
		if (hItem)
		{
			CString strPath = *(CString*)m_pDirTreeCtrl->GetItemData(hItem);

			CString strCommand;
			strCommand.Format("/e, \"%s\"", (LPCTSTR)strPath);
			ShellExecute(NULL, "open", "explorer.exe", strCommand, NULL, SW_SHOWNORMAL);
		}
	}
}

void CResourceExplorerWnd::OnUpdateResourceTreeExplore(CCmdUI* pCmdUI)
{
	if(m_bWholeTree)
	{
		HTREEITEM hItem = m_pShellTreeCtrl->GetNextItem(TVI_ROOT, TVGN_DROPHILITE);
		if(hItem && m_pShellTreeCtrl->GetItemData(hItem))
		{
			CString strPath;
			if(m_pShellTreeCtrl->GetFolderPath(hItem, strPath))
			{
				pCmdUI->Enable(TRUE);
				return ;
			}
		}
	}
	else
	{
		HTREEITEM hItem = m_pDirTreeCtrl->GetNextItem(TVI_ROOT, TVGN_DROPHILITE);
		if (hItem)
		{
			pCmdUI->Enable(TRUE);
			return ;
		}
	}

	pCmdUI->Enable(FALSE);
}

void CResourceExplorerWnd::OnResourceTreeRefresh()
{
	RefreshDir();
}

void CResourceExplorerWnd::OnUpdateResourceTreeRefresh(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CResourceExplorerWnd::OnPaint()
{
	//CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

    //CDC dcMem;
    //dcMem.CreateCompatibleDC(&dc);
    //CRect rc;
    //GetClientRect(&rc);
    //CBitmap bitmap;
    //bitmap.CreateCompatibleBitmap(&dc, rc.right, rc.bottom);
    //CBitmap *pOldBitmap = dcMem.SelectObject(& bitmap);
    
	//CWnd::DefWindowProc(WM_PAINT, (WPARAM)dcMem.m_hDC, 0);

	Default();

	CClientDC dc(this);

    //CRect rc;
    //GetClientRect(&rc);

	//CBrush brBack(RGB(255, 0, 0));
	//dc.FillRect(&rc, &brBack);

	CBrush *pBrush = CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW));

	if(!m_rcDir.IsRectEmpty())
	{
		CRect rcBorder = m_rcDir;
		rcBorder.InflateRect(0, 1);
		dc.FrameRect(&rcBorder, pBrush);
	}

	if(!m_rcThumb.IsRectEmpty())
	{
		CRect rcBorder = m_rcThumb;
		rcBorder.InflateRect(0, 1);
		dc.FrameRect(&rcBorder, pBrush);
	}

	//dc.BitBlt(0, 0, rc.right, rc.bottom, &dcMem, 0, 0, SRCCOPY);

	//dcMem.SelectObject(pOldBitmap);
}

void CResourceExplorerWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	UpdateMenuCmdUI(this, pPopupMenu, nIndex, bSysMenu);
}


void CResourceExplorerWnd::EnsureSelectionVisible()
{
	CTreeCtrl* pTreeCtrl;
	if(m_bWholeTree)
	{
		pTreeCtrl = m_pShellTreeCtrl;
	}
	else
	{
		pTreeCtrl = m_pDirTreeCtrl;
	}

	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	if(hItem)
	{
		pTreeCtrl->EnsureVisible(hItem);
	}
}