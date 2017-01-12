// FlexListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FlexListCtrl.h"
#include "MemDC.h"
#include "VisualStylesXP.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int VSB_LINE_SIZE = 1;

#define IsControlDown()			(GetKeyState(VK_CONTROL) & (1 << (sizeof(short int) * 8 - 1)))
#define IsShiftDown()			(GetKeyState(VK_SHIFT) & (1 << (sizeof(short int) * 8 - 1)))

// CFlexListCtrl

CFlexListCtrl::CFlexListCtrl()
{
	m_sizeItem.cx = 32;
	m_sizeItem.cy = 32;

	m_sizeSpace.cx = 0;
	m_sizeSpace.cy = 0;

	m_rcMargin.SetRect(0, 0, 0, 0);

	m_nHover = -1;
	m_nFocus = -1;
	m_nLastSelected = -1;

	m_dwStyle = 0;

	SetBkColor(COLOR_3DFACE, TRUE);
}

CFlexListCtrl::~CFlexListCtrl()
{
	ASSERT(m_items.size() == 0);
}


BEGIN_MESSAGE_MAP(CFlexListCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()



// CFlexListCtrl message handlers


BOOL CFlexListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, AfxGetApp()->LoadStandardCursor(IDC_ARROW), ::GetSysColorBrush(COLOR_WINDOW), NULL);

	dwStyle |= WS_VSCROLL;
	dwStyle &= ~WS_HSCROLL;

	return CWnd::Create(lpszClass, "FlexListCtrl", dwStyle, rect, pParentWnd, nID, NULL);
}

int CFlexListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_tooltip.SetMaxTipWidth(INT_MAX);
	m_tooltip.SetMargin(CRect(5, 5, 5, 5));

	return 0;
}

int CFlexListCtrl::InsertItem(int index, DWORD_PTR dwData)
{
	if (index >= 0 && index <= (int)m_items.size())
	{
		CListItem *pItem = new CListItem();
		pItem->dwData = dwData;
		m_items.insert(m_items.begin() + index, pItem);

		if (m_nFocus == -1)
			m_nFocus = index;

		RecalcScrollPos();
		Invalidate(FALSE);

		return index;
	}

	return -1;
}

void CFlexListCtrl::DeleteItem(int index, BOOL bNotify)
{
	ASSERT(index >= 0 && index < m_items.size());
	if (index >= 0 && index < m_items.size())
	{
		ITEM_LIST::iterator it = m_items.begin() + index;

		if (bNotify)
		{
			OnDeleteItem(index);
		}

		delete *it;
		m_items.erase(it);

		if (IsItemSelected(index))
		{
			// send FLCN_SELCHANGED message
			UnselectItem(index);

			/*
			int index_new;
			if (m_items.size() == 0)
			{
				index_new = -1;
			}
			else
			{
				index_new = index;
				if (index_new == m_items.size())
				{
					index_new--;
				}
			}
			*/
		}
		else
		{
			for (SEL_LIST::iterator it = m_sel.begin(); it != m_sel.end(); it++, index++)
			{
				if (*it > index)
					(*it)--;
			}
		}

		if (m_nFocus == index)
			if (index > m_items.size() - 1)
				m_nFocus == index - 1;

		RecalcScrollPos();
		Invalidate(FALSE);
	}
}

void CFlexListCtrl::OnDeleteItem(int index)
{
}

void CFlexListCtrl::DeleteAllItems()
{
//	bool bSendSelChange = (m_sel.size() > 0);

	if (m_items.size() > 0)
	{
		int index = 0;
		for (ITEM_LIST::iterator it = m_items.begin(); it != m_items.end(); it++, index++)
		{
			OnDeleteItem(index);
			delete *it;
		}
		m_items.clear();
		m_sel.clear();
		m_nFocus = -1;

		RecalcScrollPos();
		Invalidate(FALSE);
	}
	
//	if (bSendSelChange)
//		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_SELCHANGED), (LPARAM)m_hWnd);
}

int CFlexListCtrl::GetFirstVisibleRow()
{
	CRect rc;
	GetClientRect(&rc);

	int nScrollPos = GetScrollPos(SB_VERT);

	int nFirstRow = nScrollPos;
	if (nFirstRow < 0)
		nFirstRow = 0;

	return nFirstRow;
}

int CFlexListCtrl::GetVisibleRows()
{
	CRect rc;
	GetClientRect(&rc);

	// calculate available height, excluding margins.
	int nHeight = max(0, rc.Height() - (m_rcMargin.top + m_rcMargin.bottom));

	// calculate fullly visible rows
	int nRows = nHeight / (m_sizeItem.cy + m_sizeSpace.cy);

	// is there last partially visible row?
	if (nHeight % (m_sizeItem.cy + m_sizeSpace.cy) > 0)
		nRows++;

	return nRows;
}

void CFlexListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CGlandaMemDC dcMem(&dc);

	CRect rc;
	GetClientRect(&rc);

	CBrush brBack;
	if (m_bSysColorBack)
		brBack.CreateSysColorBrush((int)m_crBack);
	else
		brBack.CreateSolidBrush(m_crBack);

	dcMem.FillRect(&rc, &brBack);

	int nScrollPos = GetScrollPos(SB_VERT);

	int nCols = GetColCount();

	int nFirstRow = GetFirstVisibleRow();
	int nLastRow = nFirstRow + GetVisibleRows() - 1;

	//TRACE1("Visible Rows: %d\n", GetVisibleRows());

	CBrush *pbrGrid = NULL;
	if (m_dwStyle & FLCS_GRID)
		pbrGrid = CBrush::FromHandle((HBRUSH)::GetStockObject(BLACK_BRUSH));

	for (int i = nFirstRow; i <= nLastRow; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			int index = i * nCols + j;
			if (index >= (int)m_items.size())
				goto _done;

			CRect rcItem;
			rcItem.left = m_rcMargin.left + (m_sizeItem.cx + m_sizeSpace.cx) * j;
			rcItem.top = m_rcMargin.top + (m_sizeItem.cy + m_sizeSpace.cy) * (i - nScrollPos);
			rcItem.right = rcItem.left + m_sizeItem.cx;
			rcItem.bottom = rcItem.top + m_sizeItem.cy;

			DrawItem(&dcMem, index, &rcItem);

			if (m_dwStyle & FLCS_GRID)
			{
				int cx = m_sizeSpace.cx / 2;
				if (m_sizeSpace.cx % 2 != 0)
					cx++;
				int cy = m_sizeSpace.cy / 2;
				if (m_sizeSpace.cy % 2 != 0)
					cy++;
				rcItem.InflateRect(cx, cy);
				dcMem.FrameRect(&rcItem, pbrGrid);
			}
		}
	}
_done:
	;
}

void CFlexListCtrl::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
#ifdef _DEBUG
	pDC->FillRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_WINDOW)));
	if (IsItemSelected(index))
		pDC->FrameRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_HIGHLIGHT)));
	else
		pDC->FrameRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DFACE)));
#endif
}

BOOL CFlexListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

int CFlexListCtrl::GetRowCount()
{
	int nCols = GetColCount();
	int nRows = (int)m_items.size() / nCols;
	if (m_items.size() % nCols > 0)
		nRows++;
	return nRows;
}

int CFlexListCtrl::GetColCount()
{
	CRect rc;
	GetClientRect(&rc);

	int nCols = 0;
	if ((rc.Width() - m_rcMargin.left) > 0)
		nCols = 1;
	nCols += (rc.Width() - (m_rcMargin.left + m_sizeItem.cx + m_rcMargin.right)) / (m_sizeItem.cx + m_sizeSpace.cx);
	if (nCols == 0)
		nCols++;

	return nCols;
}

void CFlexListCtrl::RecalcScrollPos()
{
	CRect rc;
	GetClientRect(&rc);

	int nRows = GetRowCount();
	int nMax = nRows - 1;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if (m_dwStyle & FLCS_DISABLENOSCROLL)
		si.fMask |= SIF_DISABLENOSCROLL;

	si.nMin = 0;
	si.nMax = nMax;
	si.nPage = GetVisibleRows() - 1;
	si.nPos = GetScrollPos(SB_VERT);
	si.nTrackPos = si.nPos;
	SetScrollInfo(SB_VERT, &si, TRUE);

	if (m_dwStyle & FLCS_DISABLENOSCROLL)
		ShowScrollBar(SB_VERT, TRUE);
}

void CFlexListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	GetClientRect(rc);

	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si, SIF_ALL);

	int nMin = si.nMin;
	int nMax = si.nMax;
	int nPage = si.nPage;
	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = min(nMax - nPosOld, VSB_LINE_SIZE);
		nPosNew = nPosOld + ds;
		break;
	case SB_LINEUP:
		ds = min(nPosOld, VSB_LINE_SIZE);
		nPosNew = nPosOld - ds;
		break;
	case SB_PAGEDOWN:
		ds = min(nMax - nPosOld, nPage);
		nPosNew = nPosOld + ds;
		break;
	case SB_PAGEUP:
		ds = min(nPosOld, nPage);
		nPosNew = nPosOld - ds;
		break;
	case SB_TOP:
		nPosNew = 0;
		break;
	case SB_BOTTOM:
		nPosNew = nMax;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ds = nPos - nPosOld;
		nPosNew = nPosOld + ds;
		break;
	default:
		return;
	}

	ASSERT(nPosNew >= nMin && nPosNew <= nMax);
	nPosNew = min(nMax, max(nMin, nPosNew));

	if (nPosNew != nPosOld)
	{
		si.fMask = SIF_POS;
		si.nPos = nPosNew;
		SetScrollInfo(SB_VERT, &si, TRUE);

		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CFlexListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (::IsWindow(m_hWnd))
		RecalcScrollPos();
}

void CFlexListCtrl::SelectItem(int index, BOOL bNotify)
{
	TRACE("SelectItem(%d)\n", index);

	// Cancel pending delay scroll event
	CancelDelayEnsureVisible();

	if (m_dwStyle & FLCS_MULTISEL)
	{
		if (find(m_sel.begin(), m_sel.end(), index) == m_sel.end())
		{
			m_sel.push_back(index);
			RedrawItem(index);
			UpdateWindow();
			if (bNotify)
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_SELCHANGED), (LPARAM)m_hWnd);
		}
	}
	else
	{
		if (m_sel.size() > 0)
		{
			ASSERT(m_sel.size() == 1);
			int nOld = m_sel[0];
			if (nOld != index)
			{
				m_sel[0] = index;
				RedrawItem(nOld);
				RedrawItem(index);
				UpdateWindow();
				if (bNotify)
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_SELCHANGED), (LPARAM)m_hWnd);
			}
		}
		else
		{
			m_sel.push_back(index);
			RedrawItem(index);
			UpdateWindow();
			if (bNotify)
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_SELCHANGED), (LPARAM)m_hWnd);
		}
	}
}

void CFlexListCtrl::UnselectItem(int index)
{
	TRACE("UnselectItem(%d)\n", index);
	for (int i = 0; i < m_sel.size(); i++)
	{
		if (m_sel[i] == index)
		{
			m_sel.erase(m_sel.begin() + i);
			RedrawItem(m_sel[i]);
			UpdateWindow();

			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_SELCHANGED), (LPARAM)m_hWnd);

			return;
		}
	}
}

void CFlexListCtrl::SetFocusItem(int index)
{
	if (m_nFocus != index)
	{
		if (m_nFocus >= 0)
			RedrawItem(m_nFocus);

		m_nFocus = index;
		if (m_nFocus >= 0)
			RedrawItem(m_nFocus);
	}
}

void CFlexListCtrl::UnselectAllItems()
{
	if (m_sel.size() > 0)
	{
		m_sel.clear();
		Invalidate();
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_SELCHANGED), (LPARAM)m_hWnd);
	}
}

void CFlexListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();
	int index = ItemFromPoint(point);

	if (m_dwStyle & FLCS_MULTISEL)
	{
		if (!IsControlDown() &&  !IsShiftDown())
			UnselectAllItems();

		if (index >= 0)
		{
			SetFocusItem(index);
			if (!IsItemSelected(index))
				SelectItem(index, TRUE);
			else
				UnselectItem(index);
			DelayEnsureVisible(index);
		}
	}
	else
	{
		if (index >= 0)
		{
			SetFocusItem(index);
			SelectItem(index, TRUE);
			DelayEnsureVisible(index);
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

int CFlexListCtrl::ItemFromPoint(CPoint point)
{
	int nScrollPos = GetScrollPos(SB_VERT);

	int row = nScrollPos + (point.y - m_rcMargin.top) / (m_sizeItem.cy + m_sizeSpace.cy);
	int col = (point.x - m_rcMargin.left) / (m_sizeItem.cx + m_sizeSpace.cx);
	if (col < GetColCount())
	{
		int index = row * GetColCount() + col;
		if (index < m_items.size())
		{
			CRect rc;
			GetItemRect(index, &rc);
			if (rc.PtInRect(point))
				return index;
		}
	}

	return -1;
}

void CFlexListCtrl::GetItemRect(int index, RECT *pRect)
{
	ASSERT(index >= 0);
	index = max(0, index);

	int nCols = GetColCount();

	int nRow = index / nCols;
	int nCol = index % nCols;

	int nScrollPos = GetScrollPos(SB_VERT);

	pRect->left = m_rcMargin.left + (m_sizeItem.cx + m_sizeSpace.cx) * nCol;
	pRect->top = m_rcMargin.top + (m_sizeItem.cy + m_sizeSpace.cy) * (nRow - nScrollPos);
	pRect->right = pRect->left + m_sizeItem.cx;
	pRect->bottom = pRect->top + m_sizeItem.cy;
}

void CFlexListCtrl::RedrawItem(int index)
{
	//TRACE1("RedrawItem(%d)\n", index);

	CRect rc;
	GetItemRect(index, &rc);
	InvalidateRect(&rc, FALSE);
}

BOOL CFlexListCtrl::IsItemSelected(int index)
{
	for (int i = 0; i < m_sel.size(); i++)
		if (m_sel[i] == index)
			return TRUE;
	return FALSE;
}

void CFlexListCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

	DeleteAllItems();
}

int CFlexListCtrl::GetCurSel()
{
	ASSERT((m_dwStyle & FLCS_MULTISEL) == 0);
	return m_sel.size() == 1 ? m_sel[0] : -1;
}

void CFlexListCtrl::SetItemData(int index, DWORD dwData)
{
	if (index >= 0 && index < m_items.size())
		m_items[index]->dwData = dwData;
}

DWORD_PTR CFlexListCtrl::GetItemData(int index)
{
	if (index >= 0 && index < m_items.size())
		return m_items[index]->dwData;
	return NULL;
}

void CFlexListCtrl::SetBkColor(COLORREF crBack, BOOL bSysColor)
{
	m_crBack = crBack;
	m_bSysColorBack = bSysColor;
}

void CFlexListCtrl::SetMargin(int l, int t, int r, int b)
{
	m_rcMargin.SetRect(l, t, r, b);
	if (::IsWindow(m_hWnd))
	{
		RecalcScrollPos();
		Invalidate();
	}
}

void CFlexListCtrl::SetItemSpace(int cx, int cy)
{
	m_sizeSpace.cx = cx;
	m_sizeSpace.cy = cy;
	if (::IsWindow(m_hWnd))
	{
		RecalcScrollPos();
		Invalidate();
	}
}

void CFlexListCtrl::EnsureVisible(int index)
{
	ASSERT(index >= 0);
	index = max(0, index);

	CRect rc;
	GetClientRect(&rc);

	CRect rcItem;
	GetItemRect(index, &rcItem);

	int nDelta = 0;
	if (rcItem.top < rc.top)
		nDelta = rcItem.top - rc.top;
	else if (rcItem.bottom > rc.bottom)
		nDelta = rcItem.bottom - rc.bottom;

	if (nDelta != 0)
	{
		int nDeltaRows = nDelta / (m_sizeItem.cy + m_sizeSpace.cy);
		if (nDelta % (m_sizeItem.cy + m_sizeSpace.cy) != 0)
			nDelta > 0 ? nDeltaRows++ : nDeltaRows --;

		int nScrollPos = GetScrollPos(SB_VERT);
		nScrollPos += nDeltaRows;
		SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nScrollPos), NULL);
	}
}

CPoint CFlexListCtrl::GetItemPos(int index)
{
	ASSERT(index >= 0 && index < m_items.size());
	CPoint point;
	int nCols = GetColCount();
	point.x = index % nCols;
	point.y = index / nCols;
	return point;
}

int CFlexListCtrl::GetItemIndex(int nRow, int nCol)
{
	int nCols = GetColCount();
	int index = nRow * nCols + nCol;
	if (index >= 0 && index < m_items.size())
		return index;
	return -1;
}

void CFlexListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (GetItemCount() == 0)
		return;

	int index = GetFocusItem();
	if (index < 0)
	{
		SetFocusItem(0);
		index = 0;
	}

	int nRows = GetRowCount();
	int nCols = GetColCount();
	CPoint point = GetItemPos(index);

	switch (nChar)
	{
	case VK_LEFT:
		if (point.x > 0)
		{
			point.x--;
			index = GetItemIndex(point.y, point.x);
			SetFocusItem(index);
			SelectItem(index, TRUE);
			EnsureVisible(index);
		}
		break;

	case VK_RIGHT:
		if (point.x < nCols - 1)
		{
			point.x++;
			index = GetItemIndex(point.y, point.x);
			if (index >= 0)
			{
				SetFocusItem(index);
				SelectItem(index, TRUE);
				EnsureVisible(index);
			}
		}
		break;

	case VK_UP:
		if (point.y > 0)
		{
			point.y--;
			index = GetItemIndex(point.y, point.x);
			SetFocusItem(index);
			SelectItem(index, TRUE);
			EnsureVisible(index);
		}
		break;

	case VK_DOWN:
		if (point.y < nRows - 1)
		{
			point.y++;
			index = GetItemIndex(point.y, point.x);
			if (index >= 0)
			{
				SetFocusItem(index);
				SelectItem(index, TRUE);
				EnsureVisible(index);
			}
		}
		break;

	case VK_PRIOR:
		SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
		break;

	case VK_NEXT:
		SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CFlexListCtrl::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	// should allow TAB and ENTER/ESCAPE to work
	return DLGC_WANTARROWS;
	//return CWnd::OnGetDlgCode();
}

BOOL CFlexListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si, SIF_ALL);
	int ds = zDelta / WHEEL_DELTA * VSB_LINE_SIZE;
	int nPosNew = max(si.nMin, min(si.nMax, si.nPos - ds));
	if (nPosNew != si.nPos)
	{
		SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPosNew), NULL);
		return TRUE;
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CFlexListCtrl::SetItemSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	m_sizeItem.cx = max(0, cx);
	m_sizeItem.cy = max(0, cy);
	if (::IsWindow(m_hWnd))
	{
		RecalcScrollPos();
		Invalidate(FALSE);
	}
}

BOOL CFlexListCtrl::GetToolTipText(int index, CString &strText)
{
	return FALSE;
}

BOOL CFlexListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		if (GetItemCount() > 0)
			m_tooltip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CFlexListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseMove(nFlags, point);

	int index = ItemFromPoint(point);
	if (index != m_nHover)
	{
		m_nHover = index;
		if (m_nHover >= 0)
		{
			m_tooltip.Pop();
			m_tooltip.DelTool(this, 1);

			CString strText;
			if (GetToolTipText(index, strText))
			{
				AFX_OLDTOOLINFO ti;
				memset(&ti, 0, sizeof(ti));
				ti.cbSize = sizeof(ti);
				ti.hinst = 0;
				ti.hwnd = m_hWnd;
				ti.uFlags = TTF_CENTERTIP;
				ti.lpszText = (LPSTR)(LPCTSTR)strText;
				ti.uId = 1;

				CRect rcItem;
				GetItemRect(index, &rcItem);
				ti.rect = rcItem;

				m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
				m_tooltip.Activate(TRUE);
			}
		}
	}
}

void CFlexListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();

	int index = ItemFromPoint(point);
	if (index >= 0)
	{
		SetFocusItem(index);
		SelectItem(index, TRUE);
		DelayEnsureVisible(index);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CFlexListCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();

	CWnd::OnMButtonDown(nFlags, point);
}

void CFlexListCtrl::DelayEnsureVisible(int index)
{
	m_nLastSelected = index;
	SetTimer(1, GetDoubleClickTime() + 1, NULL);
}

void CFlexListCtrl::CancelDelayEnsureVisible()
{
	KillTimer(1);
}

void CFlexListCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		if (m_nLastSelected >= 0)
			EnsureVisible(m_nLastSelected);
	}

	CWnd::OnTimer(nIDEvent);
}

void CFlexListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CancelDelayEnsureVisible();

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), FLCN_DBLCLK), (LPARAM)m_hWnd);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CFlexListCtrl::OnNcPaint()
{
	Default();

	DWORD dwExStyle = GetExStyle();
	if ((dwExStyle & WS_EX_CLIENTEDGE) || (dwExStyle & WS_EX_STATICEDGE))
	{
		if (g_xpStyle.IsAppThemed())
		{
			HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"TREEVIEW");
			if (hTheme)
			{
				COLORREF crBorder;

				HRESULT hr = g_xpStyle.GetThemeColor(hTheme, 0, 0, TMT_BORDERCOLOR, &crBorder);
				g_xpStyle.CloseThemeData(hTheme);

				if (SUCCEEDED(hr))
				{
					CDC *pDC = GetWindowDC();
					if (pDC)
					{
						CRect rc;
						GetWindowRect(&rc);
						rc.OffsetRect(-rc.left, -rc.top);

						CBrush brBorder(crBorder);
						pDC->FrameRect(&rc, &brBorder);

						if (dwExStyle & WS_EX_CLIENTEDGE)
						{
							rc.InflateRect(-1, -1);

							CBrush brWindow(::GetSysColor(COLOR_WINDOW));
							pDC->FrameRect(&rc, &brWindow);
						}

						ReleaseDC(pDC);
					}
				}
			}
		}
	}
}

void CFlexListCtrl::ModifyFLStyle(DWORD dwRemove, DWORD dwAdd)
{
	m_dwStyle &= ~dwRemove;
	m_dwStyle |= dwAdd;
	if (::IsWindow(m_hWnd))
		Invalidate(FALSE);
}
