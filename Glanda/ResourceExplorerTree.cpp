// ResourceExplorerTree.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceExplorerTree.h"
#include "filepath.h"
#include "Resource.h"
#include "SWFParse.h"
#include "gldLibrary.h"
#include "gldSWFImporter.h"
#include "gldDataKeeper.h"
#include "gldMovieClip.h"
#include "gldObj.h"
#include "gldImage.h"
#include "gldSound.h"
#include "Glanda_i.h"
#include "C2I.h"
#include "TransAdaptor.h"
#include "TextToolEx.h"
#include "ITextTool.h"
#include "toolsdef.h"
#include <mmsystem.h>
#include "BinStream.h"
#include "Options.h"
#include "CategoryBar.h"
#include "DlgImportingSWF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CResourceExplorerTree

#define ANICURSOR_FRAMES 10
#ifdef BORDER_SIZE
#undef BORDER_SIZE
#endif
#define BORDER_SIZE 3
#define PROGRESS_HEIGHT 16
#define PROGRESS_SIZE  50
#define STEP_SIZE 10

CResourceExplorerTree::CResourceExplorerTree()
{
	m_nStockResourceImage = -1;

	m_bDrawSnapshot = FALSE;

	m_sWaitMsg = _T("Parsing...");
	m_bShowWaitMsg = TRUE;
	m_hIconMsg = NULL;	// default: blank icon
	m_hRedrawEvent = NULL;
	m_hTimerEvent = NULL;
	m_hThread = NULL;
	m_hItemToPopulate = NULL;
	m_hItemMsg = NULL;
	m_nTimerDelay = 125;
	VERIFY(m_ilHourglass.Create(IDB_HOURGLASS, 32, 0, RGB(255, 0, 255)));	
}

CResourceExplorerTree::~CResourceExplorerTree()
{
}

BEGIN_MESSAGE_MAP(CResourceExplorerTree, CShellTree)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnTvnItemexpanding)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnTvnItemexpanded)		
END_MESSAGE_MAP()



// CResourceExplorerTree message handlers

void CResourceExplorerTree::OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//Let's check if it's the "Library" item, whose lptvid is NULL.
	LPITEMDATA lptvid = (LPITEMDATA)pNMTreeView->itemOld.lParam;
	if (lptvid != NULL)
	{
		CShellTree::OnDeleteItem(pNMHDR, pResult);
		return;
	}

	*pResult = 0;
}

void CResourceExplorerTree::PopulateTree()
{
	CShellTree::PopulateTree();
}

void CResourceExplorerTree::AppendStockResourcesTreeItem(LPCTSTR lpszItemText)
{
	size_t nLength = _tcslen(lpszItemText);
	TCHAR* szText = new TCHAR[nLength + 1];
	memset(szText, 0, sizeof(TCHAR)*sizeof(nLength+1));
	_tcscpy(szText, lpszItemText);

	TVITEM tvi;

	tvi.mask= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.pszText = szText;
	tvi.cchTextMax = (int)_tcslen(tvi.pszText) + 1;
	tvi.lParam = NULL;

	LPCTSTR lpszPath = ::GetModuleFilePath();
	tvi.iImage = GetStockResourceImage();
	tvi.iSelectedImage = GetStockResourceImage() + 1;

	TVINSERTSTRUCT tvins;
	tvins.item = tvi;
	tvins.hParent = TVI_ROOT;
	tvins.hInsertAfter = TVI_LAST;

	// Add the item to the tree
	InsertItem(&tvins);

	delete [] szText;
}

void CResourceExplorerTree::GotoFolder(int nFolder)
{
	LPITEMIDLIST pidl = NULL;
	if (SHGetSpecialFolderLocation(m_hWnd, nFolder, &pidl) == NOERROR)
	{
		Locate(pidl);

		LPMALLOC pMalloc = NULL;
		if (SHGetMalloc(&pMalloc) == NOERROR)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	else
	{
		AfxMessageBox(IDS_FOLDER_NOT_FOUND);
	}
}

BOOL CResourceExplorerTree::GetFolderPath(HTREEITEM hItem, CString& strFolderPath)
{
	LPITEMDATA		lptvid;
	TCHAR			szBuff[_MAX_PATH];
	BOOL			bRet = FALSE;

	lptvid=(LPITEMDATA)GetItemData(hItem);

	if (lptvid)
	{
		if (lptvid->lpsfParent)
		{
			if (lptvid->lpi)
			{				
				if(SHGetPathFromIDList(lptvid->lpifq, szBuff))
				{
					strFolderPath = szBuff;
					bRet = true;
				}		
			}
		}
		else	// it's the root: desktop
		{
			LPITEMIDLIST pidl = NULL;
			if (SUCCEEDED(SHGetSpecialFolderLocation(m_hWnd, CSIDL_DESKTOP, &pidl)))
			{
				if (SHGetPathFromIDList(pidl, szBuff))
				{
					strFolderPath = szBuff;
					bRet = true;
				}
				CoTaskMemFree(pidl);
			}
		}
	}
	
	return bRet;
}

BOOL CResourceExplorerTree::FolderIncludeFileType(LPSHELLFOLDER lpsf, LPCTSTR fileType)
{
	ASSERT(lpsf != NULL && fileType != NULL);

	BOOL bRet = FALSE;

	CComPtr<IEnumIDList> peList;
	HRESULT hr = lpsf->EnumObjects(NULL, 
		SHCONTF_INCLUDEHIDDEN | SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,
		&peList);
	if (SUCCEEDED(hr))
	{
		LPITEMIDLIST pidlSub;
		TCHAR szPath[MAX_PATH];
		while (peList->Next(1, &pidlSub, NULL) == S_OK)
		{		
			if (GetName(lpsf, pidlSub, SHGDN_FORPARSING, szPath))
			{
				LPTSTR p = szPath + _tcslen(szPath);
				while (p > szPath && *p != _T('.') && *p != _T('\\') && *p != _T('/'))
					p--;				
				if (*p == _T('.') && (_tcsicmp(p, ".swf") == 0))
				{
					bRet = TRUE;
					CoTaskMemFree(pidlSub);
					break;
				}
			}
			CoTaskMemFree(pidlSub);
		}
	}

	return bRet;
}

ULONG CResourceExplorerTree::GetItemAttrs(LPSHELLFOLDER lpsf, LPITEMIDLIST pidl)
{
	ASSERT(lpsf != NULL && pidl != NULL);

	ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM | SFGAO_FOLDER;

	lpsf->GetAttributesOf(1, (LPCITEMIDLIST *)&pidl, &ulAttrs);

	if (ulAttrs & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR))
	{
		if (ulAttrs & SFGAO_FOLDER)
		{
			if ((ulAttrs & SFGAO_HASSUBFOLDER) == 0) // check if have some swf file
			{
				CComPtr<IShellFolder> psfThis;				
				HRESULT hr = lpsf->BindToObject(pidl, 0, IID_IShellFolder, (LPVOID *)&psfThis);
				if (SUCCEEDED(hr))
				{
					if (FolderIncludeFileType(psfThis, ".swf"))
						ulAttrs	|= SFGAO_HASSUBFOLDER;
				}
			}
			return ulAttrs;
		}
		else	// may be swf file
		{
			if (IsSWFFile(lpsf, pidl))
				return ulAttrs | SFGAO_HASSUBFOLDER;			
		}
	}
	
	return 0;	
}

BOOL CResourceExplorerTree::IsSWFFile(LPITEMIDLIST pFullIDL)
{
	ASSERT(pFullIDL != NULL);

	CComPtr<IShellFolder> pfDesktop;
	if (FAILED(SHGetDesktopFolder(&pfDesktop)))
		return FALSE;

	return IsSWFFile(pfDesktop, pFullIDL);
}

BOOL CResourceExplorerTree::IsSWFFile(LPSHELLFOLDER psf, LPITEMIDLIST pidl)
{
	ASSERT(psf != NULL && pidl != NULL);

	ULONG uAttrs = SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM | SFGAO_FOLDER;
	psf->GetAttributesOf(1, (LPCITEMIDLIST *)&pidl, &uAttrs);
	if ((uAttrs & (SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM)) && ((uAttrs & SFGAO_FOLDER) == 0)) // is file
	{
		TCHAR szPath[MAX_PATH];
		if (GetName(psf, pidl, SHGDN_FORPARSING, szPath))
		{
			LPTSTR p = szPath + _tcslen(szPath);
			while (p > szPath && *p != _T('.') && *p != _T('\\') && *p != _T('/'))
				p--;				
			return (*p == _T('.') && (_tcsicmp(p, ".swf") == 0));			
		}
	}

	return FALSE;
}

BOOL CResourceExplorerTree::IsSWFItem(HTREEITEM hItem)
{
	ASSERT(hItem != NULL);

	LPITEMDATA lptvid = (LPITEMDATA)GetItemData(hItem);

	if (lptvid != NULL && lptvid->lpi != NULL)
	{
		CComPtr<IShellFolder> psf;
		if (lptvid->lpsfParent)
			psf = lptvid->lpsfParent;
		else
			SHGetDesktopFolder(&psf);

		if (psf != NULL)
			return IsSWFFile(psf, lptvid->lpi);
	}

	return FALSE;
}

BOOL CResourceExplorerTree::PIDLFromPath(const CString &sPath, LPSHELLFOLDER *ppSF, LPITEMIDLIST *ppFullIDL)
{
	ASSERT(ppSF != NULL && ppFullIDL != NULL);

	CComPtr<IShellFolder> psfDesktop;	
	SHGetDesktopFolder(&psfDesktop);
	if (psfDesktop != NULL)
	{
		USES_CONVERSION;
		if (SUCCEEDED(psfDesktop->ParseDisplayName(NULL, NULL, A2W(sPath), NULL, ppFullIDL, NULL)))
		{
			ASSERT(*ppFullIDL != NULL);
			psfDesktop->BindToObject(*ppFullIDL, NULL, IID_IShellFolder, (void **)ppSF);
			if (*ppSF != NULL)
			{				
				return TRUE;
			}
			CoTaskMemFree(*ppFullIDL);
			*ppFullIDL = NULL;
		}
	}
	return FALSE;
}

void CResourceExplorerTree::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	CWaitCursor xWait;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// TODO: Add your control notification handler code here
	if (pNMTreeView->action & TVE_EXPAND)
	{
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		if (IsSWFItem(hItem))
		{
			DeleteChildren(hItem);
			PreExpandItem(hItem);
			return;
		}
	}

	CShellTree::OnItemExpanding(pNMHDR, pResult);
}

void CResourceExplorerTree::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	// TODO: Add your control notification handler code here
	if (pNMTreeView->action & TVE_EXPAND)
	{
		if (m_hItemToPopulate != NULL)
		{
			ExpandItem(pNMTreeView->itemNew.hItem);
			m_hItemToPopulate = NULL;
		}
	}
	
	*pResult = 0;
}

BOOL CResourceExplorerTree::PopulateItem(HTREEITEM hParent)
{
	ASSERT(hParent != NULL);
	LPITEMDATA pid = (LPITEMDATA)GetItemData(hParent);	
	ASSERT(pid != NULL);
	ASSERT(pid->lpsfParent != NULL);
	ASSERT(pid->lpi != NULL);
	TCHAR szPath[MAX_PATH];
	if (GetName(pid->lpsfParent, pid->lpi, SHGDN_FORPARSING, szPath))
	{
		m_iItemCount = 200;

		CDlgImportingSWF dlg(szPath);
		if (dlg.DoModal() == IDOK)
		{	
			CComPtr<IShellFolder> pSFThis;
			LPITEMIDLIST pIDL = NULL;
			if (PIDLFromPath(dlg.GetExportPath(), &pSFThis, &pIDL))
			{
				ASSERT(pSFThis != NULL && pIDL != NULL);			
				FillTreeView(pSFThis, pIDL, hParent);			
				CoTaskMemFree(pIDL);
			}			
		}		
	}

	return FALSE;
}

// progress tree method
void CResourceExplorerTree::PreAnimation(HTREEITEM hItemMsg)
{
	m_iAniStep = 0;
	// get msg image rect
	GetItemImageRect(hItemMsg, &m_rcIcon);
	// create background brush with current bg color (take rgb part only)
	m_hBrush = CreateSolidBrush(GetBkColor() & 0x00FFFFFF);

	// get msg item rect
	GetItemRect(hItemMsg, &m_rcBorder, TRUE);
	int margin = (m_rcBorder.Height() - PROGRESS_HEIGHT) / 2;
	margin = (margin < 0) ? 0 : margin;
	// set height
	m_rcBorder.top += margin;
	m_rcBorder.bottom -= margin;
	// calc progress rect
	m_rcBorder.OffsetRect(m_rcBorder.Width()+BORDER_SIZE, 0);
	m_rcProg = m_rcBorder;
	m_rcProg.DeflateRect(BORDER_SIZE, BORDER_SIZE);
	// set width
	m_rcProg.right = m_rcProg.left;
	m_rcBorder.right = m_rcBorder.left + BORDER_SIZE*2 + PROGRESS_SIZE;
	// create brushes
	m_hBorderBrush = CreateSolidBrush(RGB(128,128,128));
	m_hFillBrush = CreateSolidBrush(RGB(0,0,0));
}

void CResourceExplorerTree::PostAnimation()
{
	DeleteObject(m_hBrush);
	DeleteObject(m_hBorderBrush);
	DeleteObject(m_hFillBrush);
}

void CResourceExplorerTree::DoAnimation(BOOL bTimerEvent, int iMaxSteps, int iStep)
{
	CClientDC dc(this);

	if (bTimerEvent)
	{
		m_iAniStep = (m_iAniStep + 1) % ANICURSOR_FRAMES;
		HICON hIcon = m_ilHourglass.ExtractIcon(m_iAniStep);
		ASSERT(hIcon != NULL);
		if (hIcon != NULL)
		{
			DrawIconEx(dc.GetSafeHdc(), m_rcIcon.left, m_rcIcon.top, hIcon,
				m_rcIcon.Width(), m_rcIcon.Height(), 0, m_hBrush, DI_NORMAL);
			DestroyIcon(hIcon);
		}
	}
	//else
	//{		
	//	FrameRect(dc.GetSafeHdc(), &m_rcBorder, m_hBorderBrush);

	//	if (iMaxSteps > 0)	// linear progress
	//		m_rcProg.right = m_rcProg.left + iStep*PROGRESS_SIZE/iMaxSteps;
	//	else	// endless progress
	//		m_rcProg.right = m_rcProg.left + (LONG)(PROGRESS_SIZE*(1 + 
	//			STEP_SIZE*(double)iStep/PROGRESS_SIZE - 
	//			log(exp(2.0) + exp(2*STEP_SIZE*(double)iStep/PROGRESS_SIZE) - 1)/2));

	//	FillRect(dc.GetSafeHdc(), &m_rcProg, m_hFillBrush);
	//}
}

int CResourceExplorerTree::GetPopulationCount(int *piMaxSubItems)
{
	if (piMaxSubItems != NULL)
		*piMaxSubItems = m_iItemCount;
	return m_iItemIndex;
}

void CResourceExplorerTree::SetPopulationCount(int iMaxSubItems, int iFirstSubItem)
{
	m_iItemCount = iMaxSubItems;
	m_iItemIndex = iFirstSubItem;

	SetEvent(m_hRedrawEvent);
}

void CResourceExplorerTree::UpdatePopulation(int iSubItems)
{
	m_iItemIndex = __min(iSubItems, m_iItemCount);

	SetEvent(m_hRedrawEvent);
}

void CResourceExplorerTree::IncreasePopulation(int iSubItemsToAdd)
{
	m_iItemIndex += iSubItemsToAdd;

	SetEvent(m_hRedrawEvent);
}

void CResourceExplorerTree::SetAnimationDelay(UINT nMilliseconds)
{
	// if greater than zero, periodic DoAnimation() will be called
	m_nTimerDelay = nMilliseconds;
}

DWORD WINAPI CResourceExplorerTree::AnimationThreadProc(LPVOID pThis)
{
	CResourceExplorerTree* me = (CResourceExplorerTree*)pThis;

	HANDLE events[2] = { me->m_hTimerEvent, me->m_hRedrawEvent };
	
	while (!me->m_bAbortAnimation)
	{
		DWORD wait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
		
		if (me->m_bAbortAnimation || wait == WAIT_FAILED)
			break;

		if (wait == WAIT_OBJECT_0)	// timer event
			me->DoAnimation(TRUE, me->m_iItemCount, me->m_iItemIndex);
		else	// redraw event
			me->DoAnimation(FALSE, me->m_iItemCount, me->m_iItemIndex);
	}

	return 0;
}

void CResourceExplorerTree::StartAnimation()
{
	// user-defined setup
	PreAnimation(m_hItemMsg);

	// animation can go
	m_bAbortAnimation = FALSE;
	// automatic reset events, signaled
	m_hTimerEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	m_hRedrawEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// start animation thread
	DWORD dwThreadID = 0;
	m_hThread = CreateThread(NULL, 0, AnimationThreadProc, this,
		THREAD_PRIORITY_HIGHEST, &dwThreadID);
	// setup timer, if specified
	if (m_nTimerDelay > 0)
		m_nTimerID = (UINT)timeSetEvent(m_nTimerDelay, 5, (LPTIMECALLBACK)m_hTimerEvent,
			0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
}

void CResourceExplorerTree::StopAnimation()
{
	// stop and destroy timer
	timeKillEvent(m_nTimerID);
	// signal thread to terminate
	m_bAbortAnimation = TRUE;
	SetEvent(m_hRedrawEvent);	// make sure it can see the signal
	// wait thread termination
	WaitForSingleObject(m_hThread, INFINITE);
	// clean up
	CloseHandle(m_hTimerEvent);
	m_hTimerEvent = NULL;
	CloseHandle(m_hRedrawEvent);
	m_hRedrawEvent = NULL;
	CloseHandle(m_hThread);
	m_hThread = NULL;

	// user-defined cleanup
	PostAnimation();
}

void CResourceExplorerTree::PopulateRoot()
{
	PreExpandItem(TVI_ROOT);
	ExpandItem(TVI_ROOT);
	// force update, don't scroll
	SetRedraw(FALSE);
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	EnsureVisible(GetChildItem(TVI_ROOT));
	SetScrollInfo(SB_HORZ, &si, FALSE);
	SetRedraw();
}

void CResourceExplorerTree::PreExpandItem(HTREEITEM hItem)
{
	if (!NeedsChildren(hItem))
	{
		if (WantsRefresh(hItem))
		{
			// delete child items before populating
			DeleteChildren(hItem);
		}
		else
		{
			// doesn't want new items
			m_hItemToPopulate = NULL;
			return;
		}
	}
	// if it wants new child items, go on
	m_hItemToPopulate = hItem;

	// fix redraw when expanded programatically
	UpdateWindow();
	// hide changes until it's expanded
	SetRedraw(FALSE);
	// add wait msg, to allow item expansion
	m_hItemMsg = InsertItem(m_sWaitMsg, m_hItemToPopulate);
	// zero progress
	m_iItemCount = 1;
	m_iItemIndex = 0;
}

void CResourceExplorerTree::ExpandItem(HTREEITEM hItem)
{
	if (m_hItemToPopulate == NULL)
		return;	// just expand, doesn't want new items

	ASSERT(hItem == m_hItemToPopulate);	// should never fail!!!

	if (m_bShowWaitMsg)
	{
		// display wait msg now, make sure it's visible
		SetRedraw();
		EnsureVisible(m_hItemMsg);
		UpdateWindow();
	}
	// setup animation thread, call PreAnimation
	StartAnimation();
	// draw icon
	if (m_bShowWaitMsg)
		DrawUserIcon();
	// delay redraw after populating
	SetRedraw(FALSE);
	// take a snapshot of the background
	TakeSnapshot();
	// del temporary item (wait msg still shown)
	DeleteItem(m_hItemMsg);
	// fill in with sub items
	BOOL bCheckChildren = PopulateItem(hItem);
	// clean up animation thread, call PostAnimation
	StopAnimation();
	// change parent to reflect current children number
	if (hItem != TVI_ROOT)
	{
		TVITEM item;
		item.hItem = hItem;
		item.mask = TVIF_HANDLE | TVIF_CHILDREN;
		item.cChildren = NeedsChildren(hItem) ? 0 : 1;
		if (bCheckChildren)
			SetItem(&item);
		else if (item.cChildren == 0)
			// restore item's plus button if no children inserted
			SetItemState(hItem, 0, TVIS_EXPANDED);
	}
	// clean up snapshot
	DestroySnapshot();
	// redraw now
	SetRedraw(TRUE);
	// scroll like in a standard expansion
	HTREEITEM hChild = GetChildItem(hItem);
	while (hChild != NULL && GetFirstVisibleItem() != hItem)
	{
		EnsureVisible(hChild);
		hChild = GetNextSiblingItem(hChild);
	}
}

BOOL CResourceExplorerTree::WantsRefresh(HTREEITEM hItem)
{
	UNREFERENCED_PARAMETER(hItem);

	// default implementation, no refresh
	return FALSE;
}

BOOL CResourceExplorerTree::GetItemImageRect(HTREEITEM hItem, LPRECT pRect)
{
	if (GetImageList(TVSIL_NORMAL) == NULL)
		return FALSE;	// no images

	CRect rc;
	// get item rect
	if (!GetItemRect(hItem, &rc, TRUE))
		return FALSE;

	int cx = GetSystemMetrics(SM_CXSMICON);
	int cy = GetSystemMetrics(SM_CYSMICON);

	// move onto the icon space
	int margin = (rc.Height()-cy)/2;
	rc.OffsetRect(-cx-3 , margin);
	rc.right = rc.left + cx;	// make it square
	rc.bottom = rc.top + cy;	// make it square

	*pRect = rc;
	return TRUE;
}

void CResourceExplorerTree::DrawUserIcon()
{
	// draw user defined icon

	CRect rcIcon;
	if (!GetItemImageRect(m_hItemMsg, &rcIcon))
		return;	// no image

	// create background brush with current bg color (take rgb part only)
	HBRUSH hBrush = CreateSolidBrush(GetBkColor() & 0x00FFFFFF);

	CClientDC dc(this);

	if (m_hIconMsg != NULL)
		DrawIconEx(dc.GetSafeHdc(), rcIcon.left, rcIcon.top, m_hIconMsg,
			rcIcon.Width(), rcIcon.Height(), 0, hBrush, DI_NORMAL);
	else
		FillRect(dc.GetSafeHdc(), &rcIcon, hBrush);

	DeleteObject(hBrush);
}

void CResourceExplorerTree::SetWaitMessage(LPCTSTR pszText, HICON hIcon)
{
	m_sWaitMsg = pszText;
	m_hIconMsg = hIcon;
}

void CResourceExplorerTree::RefreshSubItems(HTREEITEM hParent)
{
	if (hParent != TVI_ROOT && !ItemHasChildren(hParent))
		return;

	SetRedraw(FALSE);
	DeleteChildren(hParent);
	if (hParent == TVI_ROOT)
		PopulateRoot();
	else
	{
		PreExpandItem(hParent);
		ExpandItem(hParent);
	}
	SetRedraw(TRUE);
}

inline BOOL CResourceExplorerTree::NeedsChildren(HTREEITEM hParent)
{
	return (GetChildItem(hParent) == NULL);
}

void CResourceExplorerTree::DeleteChildren(HTREEITEM hParent)
{
	HTREEITEM hChild = GetChildItem(hParent);
	HTREEITEM hNext;

	while (hChild != NULL)
	{
		hNext = GetNextSiblingItem(hChild);
		DeleteItem(hChild);
		hChild = hNext;
	}
}

BOOL CResourceExplorerTree::OnEraseBkgnd(CDC* pDC) 
{
	if (!m_bDrawSnapshot)
		return CTreeCtrl::OnEraseBkgnd(pDC);

	DrawSnapshot(pDC);
	SetEvent(m_hRedrawEvent);

	return TRUE;
}

void CResourceExplorerTree::TakeSnapshot()
{
	CClientDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);
	int width = rcClient.Width(), height = rcClient.Height();

	// create the snapshot
	CDC dcSnapshot;
	dcSnapshot.CreateCompatibleDC(&dc);
	m_bmpSnapshot.CreateCompatibleBitmap(&dc, width, height);
	// copy the control's background
	CBitmap* pOldBmp = dcSnapshot.SelectObject(&m_bmpSnapshot);
	dcSnapshot.BitBlt(0, 0, width, height, &dc, 0, 0, SRCCOPY);
	dcSnapshot.SelectObject(pOldBmp);

	m_bDrawSnapshot = TRUE;
}

void CResourceExplorerTree::DrawSnapshot(CDC *pDC)
{
	BITMAP bm;
	m_bmpSnapshot.GetBitmap(&bm);

	// prepare the snapshot
	CDC dcSnapshot;
	dcSnapshot.CreateCompatibleDC(pDC);
	// copy to the control's background
	CBitmap* pOldBmp = dcSnapshot.SelectObject(&m_bmpSnapshot);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcSnapshot, 0, 0, SRCCOPY);
	dcSnapshot.SelectObject(pOldBmp);
}

void CResourceExplorerTree::DestroySnapshot()
{
	m_bmpSnapshot.DeleteObject();

	m_bDrawSnapshot = FALSE;
}

int CResourceExplorerTree::GetStockResourceImage()
{
	if(m_nStockResourceImage==-1)
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_STOCK_RESOURCE_ICON);
		m_nStockResourceImage = GetImageList(TVSIL_NORMAL)->Add(&bmp, RGB(255, 0, 255));
	}

	return m_nStockResourceImage;
}