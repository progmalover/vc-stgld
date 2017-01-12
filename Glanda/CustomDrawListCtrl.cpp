// CustomDrawListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CustomDrawListCtrl.h"
#include "Resource.h"
#include "Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT IDT_HOVER = 1000;

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawListCtrl

CCustomDrawListCtrl::CCustomDrawListCtrl()
{
	m_bShowLabel = TRUE;

	m_sizeItem.cx = ::GetSystemMetrics(SM_CXICON);
	m_sizeItem.cy = ::GetSystemMetrics(SM_CYICON);

	m_bDragging = FALSE;
	m_nDragItem = -1;
	
	m_hDropTarget = NULL;

	m_nHover = -1;
}

CCustomDrawListCtrl::~CCustomDrawListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCustomDrawListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCustomDrawListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnGetInfoTip)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawListCtrl message handlers
BOOL CCustomDrawListCtrl::Load()
{

	SetRedraw(FALSE);
	DeleteAllItems();
	SetItemSize(m_sizeItem);
	BOOL ret = LoadAllItems();
	EnsureVisible(0, FALSE);
	SetRedraw(TRUE);

	return ret;
}

void CCustomDrawListCtrl::SetItemSize(const CSize &size)
{
	// check if is the specified size already
	if (m_imgList.m_hImageList && m_sizeItem == size)
		return;

	m_sizeItem = size;

	CDC *pDC = GetDC();

	LOGFONT lf;
	::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
	CFont font;
	font.CreateFontIndirect(&lf);
	CFont *pOldFont = pDC->SelectObject(&font);
	int cyText = pDC->GetTextExtent("X").cy + 6; 
	pDC->SelectObject(pOldFont);



	if (m_imgList.m_hImageList)
		m_imgList.DeleteImageList();
	m_imgList.Create(size.cx, size.cy - cyText, ILC_COLOR32, 0, 1);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, size.cx, size.cy - cyText);
	m_imgList.Add(&bmp, (CBitmap *)NULL);

	SetImageList(&m_imgList, LVSIL_NORMAL);

	SetIconSpacing(size.cx, size.cy);

	ReleaseDC(pDC);

	SetExtendedStyle(LVS_EX_INFOTIP);

	Arrange(LVA_ALIGNLEFT | LVA_ALIGNTOP);
}

void CCustomDrawListCtrl::OnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVGETINFOTIPA* lp = (NMLVGETINFOTIPA*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	strncpy(lp->pszText, GetItemText(lp->iItem), lp->cchTextMax);
	lp->pszText[lp->cchTextMax - 1] = 0;

	*pResult = 0;
}

BOOL CCustomDrawListCtrl::LoadAllItems()
{
	ASSERT(MessageBeep(0));
	return TRUE;
}

void CCustomDrawListCtrl::DrawImage(CDC *pDC, int nItem, const CRect &rc)
{

}

void CCustomDrawListCtrl::ClearCache(int nItem)
{
	CItemData *pData = (CItemData *)CListCtrl::GetItemData(nItem);
	if (pData && pData->pBmp)
	{
		delete pData->pBmp;
		pData->pBmp = NULL;
	}
}

void CCustomDrawListCtrl::RedrawItem(int nItem, BOOL bCreateCache)
{
	if (bCreateCache)
		ClearCache(nItem);

	CRect rcAll;
	GetItemRect(nItem, &rcAll, LVIR_BOUNDS);
	InvalidateRect(&rcAll, FALSE);
}

void CCustomDrawListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	NMLVCUSTOMDRAW *lp = (NMLVCUSTOMDRAW *)pNMHDR;

	// Take the default processing unless we set this to something else below.
    *pResult = CDRF_SKIPDEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
	if (lp->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (lp->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
		int nItem = (int)lp->nmcd.dwItemSpec;
		ASSERT(nItem >= 0);

		int nState = GetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED);
		
		//TRACE2("DrawItem: nItem = %d, State = %d\n", nItem, nState);


		COLORREF crWindow = ::GetSysColor(COLOR_WINDOW);
		COLORREF crFace = ::GetSysColor(COLOR_3DFACE);	/*RGB(214, 214, 214)*/
		COLORREF crHilight = ::GetSysColor(COLOR_3DHILIGHT);
		COLORREF crShadow = ::GetSysColor(COLOR_3DSHADOW);
		COLORREF crHighlight = ::GetSysColor(COLOR_HIGHLIGHT);
		COLORREF crHighlightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);


		CDC*  pDC = CDC::FromHandle ( lp->nmcd.hdc );

		CRect rcIcon;
		CRect rcText;
		if (m_bShowLabel)
		{
			GetItemRect(nItem, &rcIcon, LVIR_ICON);
			GetItemRect(nItem, &rcText, LVIR_LABEL);

			rcText.left = rcIcon.left;
			rcText.right = rcIcon.right;
		}
		else
		{
			GetItemRect(nItem, &rcIcon, LVIR_BOUNDS);
		}

		int x = rcIcon.left;
		int y = rcIcon.top;
		int cx = rcIcon.Width();
		int cy = rcIcon.Height();

		rcIcon.OffsetRect(-rcIcon.left, -rcIcon.top);

		CDC dcMem;
		dcMem.CreateCompatibleDC(pDC);
		
		BOOL bCached = TRUE;
		CItemData *pData = (CItemData *)CListCtrl::GetItemData(nItem);
		if (!pData->pBmp)
		{
			bCached = FALSE;
			pData->pBmp = new CBitmap;
			pData->pBmp->CreateCompatibleBitmap(pDC, cx, cy);
		}
		CBitmap *pBmpOld = dcMem.SelectObject(pData->pBmp);

		if (!bCached)
		{
			dcMem.FillSolidRect(&rcIcon, crWindow);
			rcIcon.InflateRect(-4, -4);
			DrawImage(&dcMem, nItem, rcIcon);
		}
		else
		{
			rcIcon.InflateRect(-4, -4);
		}


		// Draw 3D border
		CRect rcTemp = rcIcon;

#if 0
		dcMem.Draw3dRect(&rcTemp, ::GetSysColor(COLOR_3DLIGHT), Lighten(::GetSysColor(COLOR_3DFACE)));
		rcTemp.InflateRect(-1, -1);
		dcMem.Draw3dRect(&rcTemp, ::GetSysColor(COLOR_3DHILIGHT), Lighten(::GetSysColor(COLOR_3DSHADOW)));
#endif

		CBrush *pBrush = CBrush::FromHandle(::GetSysColorBrush((nState & LVIS_SELECTED) ? COLOR_HIGHLIGHT : COLOR_WINDOW));
		rcTemp = rcIcon;
		rcTemp.InflateRect(2, 2);
		dcMem.FrameRect(&rcTemp, pBrush);
		rcTemp.InflateRect(1, 1);
		dcMem.FrameRect(&rcTemp, pBrush);
		
		rcTemp.InflateRect(1, 1);
		if (nState & LVIS_SELECTED)
			dcMem.FrameRect(&rcTemp, pBrush);
		else
			dcMem.Draw3dRect(&rcTemp, ::GetSysColor(COLOR_WINDOW), ::GetSysColor(COLOR_HIGHLIGHT));

		pDC->BitBlt(x, y, cx, cy, &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pBmpOld);
	
		if (m_bShowLabel)
		{
			// Draw text
			COLORREF crTextOld;
			int nBkModeOld;

			CString strText = GetItemText(nItem);
			int nWidth = GetStringWidth(strText);

			CRect rcTextIdeal = rcText;
			pDC->DrawText(strText, &rcTextIdeal, DT_NOPREFIX | DT_SINGLELINE| DT_VCENTER | DT_END_ELLIPSIS | (nWidth < rcTextIdeal.Width() ? DT_CENTER : DT_LEFT) | DT_CALCRECT);
			rcTextIdeal.top = rcText.top;
			rcTextIdeal.bottom = rcTextIdeal.bottom;
			if (rcTextIdeal.Width() <= rcText.Width() - 2)
			{
				rcTextIdeal.OffsetRect((rcText.Width() - rcTextIdeal.Width()) / 2, 0);
				rcTextIdeal.InflateRect(1, 0);
			}

			if (nState & LVIS_SELECTED)
			{
				pDC->FillSolidRect(&rcTextIdeal, crHighlight);
				crTextOld = pDC->SetTextColor(crHighlightText);
			}
			else
			{
				pDC->FillSolidRect(&rcTextIdeal, crWindow);
				crTextOld = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			}

			nBkModeOld = pDC->SetBkMode(TRANSPARENT);

			rcTextIdeal.InflateRect(-1, 0);
			pDC->DrawText(strText, &rcTextIdeal, DT_NOPREFIX | DT_SINGLELINE| DT_VCENTER | DT_END_ELLIPSIS | (nWidth < rcTextIdeal.Width() ? DT_CENTER : DT_LEFT));

			pDC->SetBkMode(nBkModeOld);
			pDC->SetTextColor(crTextOld);
			
			rcTextIdeal.InflateRect(1, 0);
			if (nState & LVIS_SELECTED)
				pDC->Draw3dRect(&rcTextIdeal, crShadow, crHilight);

			if (nState & LVIS_FOCUSED)
				pDC->DrawFocusRect(&rcTextIdeal);
		}

		*pResult = CDRF_SKIPDEFAULT;
	}
	//else if (lp->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
	//{
	//	int nItem = (int)lp->nmcd.dwItemSpec;
	//	*pResult = CDRF_SKIPDEFAULT;
	//}
}

int CCustomDrawListCtrl::InsertItem(int index, LPCTSTR lpszText, DWORD dwData)
{
	int i = CListCtrl::InsertItem(index, "", 0);
	ASSERT(i >= 0);
	if (i >= 0)
	{
		CItemData *pData = new CItemData();
		pData->index = i;
		pData->strText = lpszText;
		pData->dwUserData = dwData;
		CListCtrl::SetItemData(i, (DWORD_PTR)pData);
	}

	return i;
}

void CCustomDrawListCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	ASSERT(pNMListView->iItem >= 0);
	delete (CItemData *)CListCtrl::GetItemData(pNMListView->iItem);

	if (m_nHover == pNMListView->iItem)
	{
		KillTimer(IDT_HOVER);
		m_nHover = -1;
	}

	*pResult = 0;
}

void CCustomDrawListCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if (m_hDropTarget && CanDrag(pNMListView->iItem))
	{
		// save the index of the item being dragged in m_nDragItem
		m_nDragItem = pNMListView->iItem;
		m_bDragging = TRUE;
		SetCapture();
		SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_SINGLE));

		CPoint point;
		GetCursorPos(&point);
		m_ptDrag = point;
		DrawDragRect(m_ptDrag);
	}

	*pResult = 0;
}

void CCustomDrawListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bDragging)
	{
		::ReleaseCapture();

		if (PtInDropTarget(point))
			OnDrop(point);
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

void CCustomDrawListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bDragging)
	{
		DrawDragRect(m_ptDrag);
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);
		m_ptDrag = ptScreen;

		DrawDragRect(m_ptDrag);
	}
	else
	{
		UINT nFlags = 0;
		int index = HitTest(point, &nFlags);
		if (m_nHover != index)
		{
			if (m_nHover == -1 && index >= 0)
				SetTimer(IDT_HOVER, 1, NULL);
			if (m_nHover >= 0 && index == -1)
				KillTimer(IDT_HOVER);

			if (m_nHover >= 0)
				RedrawItem(m_nHover, TRUE);
			m_nHover = index;
			if (m_nHover >= 0)
				RedrawItem(m_nHover, TRUE);
		}
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CCustomDrawListCtrl::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		m_nDragItem = -1;
		DrawDragRect(m_ptDrag);
	}

	CListCtrl::OnCaptureChanged(pWnd);
}

void CCustomDrawListCtrl::DrawDragRect(const CPoint &point /*Screen*/)
{
	CWnd *pDropTarget = CWnd::FromHandle(m_hDropTarget);
	CRect rcTarget;
	pDropTarget->GetWindowRect(&rcTarget);

	CRect rcFocus(CPoint(point.x - m_sizeItem.cx / 2, point.y - m_sizeItem.cy / 2), m_sizeItem);
	pDropTarget->ScreenToClient(&rcFocus);

	CDC *pDC = pDropTarget->GetDC();
	ASSERT(pDC);
	//pDC->DrawFocusRect(&rcFocus);
	pDC->SetROP2(R2_NOT);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen *pPenOld = pDC->SelectObject(&pen);
	pDC->MoveTo(rcFocus.left, rcFocus.bottom);
	pDC->LineTo(rcFocus.left, rcFocus.top);
	pDC->LineTo(rcFocus.right, rcFocus.top);
	pDC->LineTo(rcFocus.right, rcFocus.bottom);
	pDC->LineTo(rcFocus.left, rcFocus.bottom);

	pDC->SelectObject(pPenOld);

	pDropTarget->ReleaseDC(pDC);
}

void CCustomDrawListCtrl::RegisterDropTarget(HWND hWnd)
{
	m_hDropTarget = hWnd;
}

CString CCustomDrawListCtrl::GetItemText(int index)
{
	ASSERT(index >= 0 && index < GetItemCount());
	return ((CItemData *)CListCtrl::GetItemData(index))->strText;
}

void CCustomDrawListCtrl::SetItemText(int index, LPCTSTR lpszText)
{
	ASSERT(index >= 0 && index < GetItemCount());
	ASSERT(lpszText);

	((CItemData *)CListCtrl::GetItemData(index))->strText = lpszText;;

	CRect rc;
	GetItemRect(index, &rc, LVIR_BOUNDS);
	InvalidateRect(&rc);
}

BOOL CCustomDrawListCtrl::PtInDropTarget(const CPoint point/*Client*/)
{
	if (m_hDropTarget && ::IsWindowVisible(m_hDropTarget))
	{
		CWnd *pDropTarget = CWnd::FromHandle(m_hDropTarget);
		CRect rcTarget;
		pDropTarget->GetWindowRect(&rcTarget);
		ScreenToClient(&rcTarget);
		return PtInRect(&rcTarget, point);
	}

	return FALSE;
}

void CCustomDrawListCtrl::OnDrop(const CPoint &point)
{
	ASSERT(MessageBeep(0));
}

void CCustomDrawListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
	
	// TODO: Add your message handler code here
	
	DeleteAllItems();
}

void CCustomDrawListCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	SetBkColor(::GetSysColor(COLOR_3DFACE));

	CListCtrl::PreSubclassWindow();
}


BOOL CCustomDrawListCtrl::CanDrag(int index)
{
	return FALSE;
}

BOOL CCustomDrawListCtrl::SetUserData(int nItem, DWORD_PTR dwData)
{
	CItemData *pData = (CItemData *)CListCtrl::GetItemData(nItem);
	ASSERT(pData);
	if (pData)
	{
		pData->dwUserData = dwData;
		return TRUE;
	}
	return FALSE;
}

DWORD_PTR CCustomDrawListCtrl::GetUserData(int nItem)
{
	CItemData *pData = (CItemData *)CListCtrl::GetItemData(nItem);
	ASSERT(pData);
	if (pData)
		return pData->dwUserData;
	return 0;
}

DWORD_PTR CCustomDrawListCtrl::GetItemData(int nItem) const
{
	// You should call GetUserData() instead
	ASSERT(FALSE);
	return 0;
}

BOOL CCustomDrawListCtrl::SetItemData(int nItem, DWORD dwData)
{
	// You should call SetUserData() instead
	ASSERT(FALSE);
	return FALSE;
}

void CCustomDrawListCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == IDT_HOVER)
	{
		ASSERT(m_nHover >= 0);

		TRACE1("m_nHover = %d\n", m_nHover);

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		
		CRect rc;
		GetClientRect(&rc);
		if (!rc.PtInRect(point))
		{
			RedrawItem(m_nHover, TRUE);
			m_nHover = -1;
			KillTimer(IDT_HOVER);
		}

		return;
	}

	CListCtrl::OnTimer(nIDEvent);
}

BOOL CCustomDrawListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DFACE)));

	return TRUE;
	//return CListCtrl::OnEraseBkgnd(pDC);
}

int CCustomDrawListCtrl::GetHover()
{
	return m_nHover;
}
