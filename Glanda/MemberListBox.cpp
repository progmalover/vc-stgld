// MemberListBox.cpp : implementation file
// See MemberListBox.h for details on how to use this class
//

#include "stdafx.h"
#include "Glanda.h"
#include "MemberListBox.h"

#include "VersionInfo.h"
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define IMAGE_SIZE			16
#define TIMER_ID			1
#define TIMER_INTERVAL		40

DEFINE_REGISTERED_MESSAGE(WM_MEMBERLISTBOX)


/////////////////////////////////////////////////////////////////////////////
// CMemberListBox

HHOOK CMemberListBox::m_hHook				= NULL;

IMPLEMENT_SINGLETON(CMemberListBox)

CMemberListBox::CMemberListBox()
{
	// You can't actually add anything to the list box here since the
	// associated CWnd hasn't been created yet.  Save any initialization
	// you need to do for after the CWnd has been constructed.  The TODO: 
	// comments in MemberListBox.h explain how to do this for 
	// CMemberListBox as implemented here.

	m_nItemHeight = 16;
	m_bUseImage = TRUE;
	m_bTrackingLButton = FALSE;
	m_nCurFocus = -1;
}

CMemberListBox::~CMemberListBox()
{
}


BEGIN_MESSAGE_MAP(CMemberListBox, CListBox)
	//{{AFX_MSG_MAP(CMemberListBox)
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMemberListBox message handlers

// This function draws a color bar in the client space of this list box 
// item.  If your list box item contains something other than color bars, 
// you need to replace the contents of this function with your own code 
// for drawing it.  If you are working with strings, the default MFC
// implementation of this function will probably work just fine for you.  
// (Example of a string where you need to do the drawing yourself: one 
// displayed in a different font than the rest of the dialog.)
void CMemberListBox::DrawItem(LPDRAWITEMSTRUCT lp) 
{
	CDC* pDC = CDC::FromHandle(lp->hDC);

	// White space
	CRect rcItem(lp->rcItem);

	BOOL bDrawEntire = (lp->itemAction & ODA_DRAWENTIRE);
	BOOL bSelected = (((lp->itemAction & ODA_DRAWENTIRE) && ((int)lp->itemID == GetCurSel())) || ((lp->itemAction & ODA_SELECT) && (lp->itemState & ODS_SELECTED)));

	//TRACE3("index=%d, m_nCurFocus=%d, bSelect=%d\n", lp->itemID, m_nCurFocus, bSelected);

	if (m_bUseImage)
	{
		if (bDrawEntire)
		{
			// Draw image background
			pDC->FillSolidRect(rcItem.left, rcItem.top, IMAGE_SIZE + 4, rcItem.bottom, ::GetSysColor(COLOR_WINDOW));

			int nImage = (int)GetItemData(lp->itemID);
			if (nImage >= 0)
			{
				m_imgList.Draw(pDC, nImage, CPoint(rcItem.left + 2, rcItem.top + (rcItem.Height() - IMAGE_SIZE) / 2), ILD_TRANSPARENT);
			}
		}
		rcItem.left += IMAGE_SIZE + 4;
	}

	// ToDo: set both CurSel and CurFocus to one item and then decide how to draw it!!!
	if (bSelected || (int)lp->itemID == m_nCurFocus)
	{
		pDC->FillSolidRect(rcItem.left + 1, rcItem.top + 1, rcItem.Width() - 2, rcItem.Height() - 2, ::GetSysColor(bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW));
		//pDC->FillSolidRect(rcItem, bSelected ? RGB(0, 0, 0) : ::GetSysColor(COLOR_WINDOW));
		//pDC->SetTextColor(RGB(255, 255, 255));
		pDC->Draw3dRect(&rcItem, ::GetSysColor(COLOR_WINDOW), ::GetSysColor(COLOR_WINDOW));
		pDC->DrawFocusRect(&rcItem);
	}
	else
	{
		pDC->FillSolidRect(rcItem, ::GetSysColor(COLOR_WINDOW));
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

	CString strText;
	GetText(lp->itemID, strText);
	pDC->DrawText(strText, rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);
}

// This function compares two colors to each other. If you're doing 
// something different, like comparing two bitmaps to each other, use 
// a different algorithm.  If you are working with strings, the default 
// implementation provided by MFC will probably do the job just fine.  
// (Example of a string where you need to do your own comparisons: if 
// your sorting scheme is different than a standard collating sequence, 
// such as one where the comparisons need to be case-insensitive.)
int CMemberListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	// return -1 = item 1 sorts before item 2
	// return 0 = item 1 and item 2 sort the same
	// return 1 = item 1 sorts after item 2

	return 0;
}

void CMemberListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

BOOL CMemberListBox::Create()
{
	if (!m_wndTray.Create(CRect(0, 0, 0, 0)))
		return FALSE;

	DWORD dwStyle = WS_CHILD | WS_OVERLAPPED | WS_VSCROLL | LBS_HASSTRINGS | LBS_OWNERDRAWVARIABLE | LBS_SORT;
	
	if (!CListBox::Create(dwStyle, CRect(0, 0, 0, 0), &m_wndTray, 1))
		return FALSE;

	m_wndTray.SetPadding(2);
	m_wndTray.SetChild(this);

	return TRUE;
}

int CMemberListBox::AddItem(LPCTSTR lpszItem, int nImage)
{
	int index = AddString(lpszItem);
	if (index >= 0)
		SetItemData(index, (DWORD)nImage);

	return index;
}

int CMemberListBox::GetMaxItemWidth()
{
	CDC *pDC = GetDC();
	CFont *pFont = GetFont();
	CFont *pOldFont = pDC->SelectObject(pFont);

	int width = 0;
	int count = GetCount();
	for (int i = 0; i < count; i++)
	{
		CString strItem;
		GetText(i, strItem);
		CSize size = pDC->GetTextExtent(strItem);
		if (size.cx > width)
			width = size.cx;
	}


	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	if (m_bUseImage)
		width += IMAGE_SIZE + 4;

	return width;
}

//
// return value: *REAL* height applied.
//
int CMemberListBox::SetItemHeight(int nHeight)
{
	ASSERT(GetStyle() & LBS_OWNERDRAWVARIABLE);
	m_nItemHeight = max(IMAGE_SIZE, nHeight);
	return m_nItemHeight;
}

void CMemberListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (PtInWindow(point))
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		if (m_bTrackingLButton)
		{
			int index;
			if (GetCount() == 0)
			{
				index = -1;
			}
			else
			{
				BOOL bOutSide;
				index = ItemFromPoint(point, bOutSide);

				// Is cursor really in the item[index]?
				CRect rcItem;
				GetItemRect(index, &rcItem);
				if (!PtInRect(&rcItem, point))
					index = -1;
			}
			if (index != -1)
				SetCurSelEx(index);
		}
	}

	CListBox::OnMouseMove(nFlags, point);
}

void CMemberListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (PtInWindow(point))
	{
		int index;
		if (GetCount() == 0)
		{
			index = -1;
		}
		else
		{
			BOOL bOutSide;
			index = ItemFromPoint(point, bOutSide);

			// Is cursor really in the item[index]?
			CRect rcItem;
			GetItemRect(index, &rcItem);
			if (!PtInRect(&rcItem, point))
				index = -1;
		}
		if (index != -1)
		{
			SetCurSelEx(index);
			StartTrackLButton();
		}
	}

	CListBox::OnLButtonDown(nFlags, point);
}

void CMemberListBox::StartTrackLButton()
{
	m_bTrackingLButton = TRUE;
	SetCapture();
	SetTimer(TIMER_ID, TIMER_INTERVAL, NULL);
}

void CMemberListBox::EndTrackLButton()
{
	m_bTrackingLButton = FALSE;
	KillTimer(TIMER_ID);
}

void CMemberListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bTrackingLButton)
		ReleaseCapture();

	CListBox::OnLButtonUp(nFlags, point);
}

void CMemberListBox::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	if (m_bTrackingLButton)
		EndTrackLButton();

	CListBox::OnCaptureChanged(pWnd);
}

void CMemberListBox::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == TIMER_ID)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);

		if (!PtInWindow(point))
		{
			CRect rc;
			GetClientRect(&rc);
			if (point.y <= rc.top || point.y >= rc.bottom)
			{
				int iOld = GetCurSel();
				int iNew;
				if (point.y <= rc.top)
					iNew = max(iOld - 1, 0);
				else
					iNew = min(iOld + 1, GetCount() - 1);
				SetCurSelEx(iNew);
			}
		}
	}

	CListBox::OnTimer(nIDEvent);
}

int CMemberListBox::SetCurSelEx(int nSel)
{
	if (m_nCurFocus >= 0 && nSel >= 0)
		SetCurFocus(-1);

	int nCurSel = GetCurSel();
	if (nSel != nCurSel)
		return SetCurSel(nSel);

	return nCurSel;
}

void CMemberListBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CListBox::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	

	if (bShow)
	{
		//m_hHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, NULL, GetCurrentThreadId());
		//ASSERT(m_hHook);

		if (CVersionInfo::Instance()->IsWin98orLater())
		{
		#if !defined SPI_GETLISTBOXSMOOTHSCROLLING
			#define SPI_GETLISTBOXSMOOTHSCROLLING       0x1006
		#endif
		#if !defined  SPI_SETLISTBOXSMOOTHSCROLLING       
			#define SPI_SETLISTBOXSMOOTHSCROLLING       0x1007
		#endif

			SystemParametersInfo(SPI_GETLISTBOXSMOOTHSCROLLING, 0, &m_bListBoxSmoothScrolling, 0);
			if (m_bListBoxSmoothScrolling)
				SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, 0, (LPVOID)FALSE, 0);
		}
	}
	else
	{
		//UnhookWindowsHookEx(m_hHook);

		if (CVersionInfo::Instance()->IsWin98orLater() && m_bListBoxSmoothScrolling)
			SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, 0, (LPVOID)m_bListBoxSmoothScrolling, 0);
	}
}

//
// point must be client-related coord
//
BOOL CMemberListBox::PtInWindow(CPoint &point)
{
	CRect rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	return rc.PtInRect(point);
}

LRESULT CMemberListBox::SendOwnerMessage(WPARAM wParam)
{
	CWnd *pWnd = GetOwner();
	ASSERT(IsWindow(pWnd->m_hWnd));
	if (IsWindow(pWnd->m_hWnd))
		return pWnd->PostMessage(WM_MEMBERLISTBOX, wParam, (LPARAM)m_hWnd);
	return 0;
}

LRESULT CALLBACK CMemberListBox::MouseProc(int nCode, WPARAM wp, LPARAM lp)
{
	LRESULT ret = CallNextHookEx(m_hHook, nCode, wp, lp);

	if (nCode >= 0)
	{

		switch (wp)
		{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:

		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCRBUTTONDBLCLK:
			if (((MOUSEHOOKSTRUCT *)lp)->hwnd != CMemberListBox::Instance()->m_hWnd &&
				((MOUSEHOOKSTRUCT *)lp)->hwnd != CMemberListBox::Instance()->GetParent()->m_hWnd)
				CMemberListBox::Instance()->SendOwnerMessage(LBM_SELCANCEL);
			break;
		}
	}

	return ret;
}

void CMemberListBox::RelayEvent(MSG *pMsg)
{
	if (!::IsWindow(m_hWnd) || !IsWindowVisible())
		return;
	
	switch (pMsg->message)
	{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:

		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCRBUTTONDBLCLK:
			if (pMsg->hwnd != m_hWnd && pMsg->hwnd != GetParent()->m_hWnd)
				CMemberListBox::Instance()->SendOwnerMessage(LBM_SELCANCEL);
			break;
	}
}

int CMemberListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	m_imgList.Create(IDB_MEMBER_LIST, IMAGE_SIZE, 0, RGB(255, 0, 255));

	return 0;
}

void CMemberListBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListBox::OnLButtonDblClk(nFlags, point);

	SendOwnerMessage(LBM_SELCHANGE);
}

//
// if m_nCurFocus != -1, means the item is partial matched input string
//
int CMemberListBox::FindStringEx(LPCTSTR lpszText)
{
	TRACE1("FindStringEx: %s\n", lpszText);

	int index = FindString(-1, lpszText);
	if (index < 0)
	{
		int nCurFocus = -1;
		int len = strlen(lpszText);
		char *psz = new char[len + 1];
		if (psz)
		{
			strcpy(psz, lpszText);
			char *p = psz + strlen(psz) - 1;
			while (p > psz)
			{
				*p = 0;
				nCurFocus = FindString(-1, psz);
				if (nCurFocus >= 0)
				{
					TRACE1("nCurFocus=%d\n", nCurFocus);
					break;
				}
				p--;
			}

			delete psz;
		}

		if (nCurFocus == -1)
			nCurFocus = 0;

		SetCurFocus(nCurFocus);
	}

	SetCurSelEx(index);

	return index;
}

int CMemberListBox::SetCurFocus(int nCurFocus)
{
	// 总是调用 EnsureVisible(), 否则连续选择(部分匹配)同一个 Item 时，第二次可能无法自动显示为可见的
	//if (m_nCurFocus == nCurFocus)
	//	return nCurFocus;

	int nOldFocus = m_nCurFocus;
	m_nCurFocus = nCurFocus;
	if (nOldFocus >= 0)
		RedrawItem(nOldFocus, FALSE);
	if (m_nCurFocus >= 0)
	{
		EnsureVisible(m_nCurFocus);
		RedrawItem(m_nCurFocus, FALSE);
	}

	return nCurFocus;
}

void CMemberListBox::RedrawItem(int index, BOOL bImage)
{
	CRect rc;
	GetItemRect(index, &rc);
	if (!bImage && m_bUseImage)
		rc.left += (IMAGE_SIZE + 4);
	InvalidateRect(&rc, TRUE);
	UpdateWindow();
}

void CMemberListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_nCurFocus >= 0)
	{
		switch (nChar)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_HOME:
		case VK_END:
			SetCurSelEx(m_nCurFocus);
			return;
		}
	}

	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);

	if (m_nCurFocus >= 0 && GetCurSel() >= 0)
		SetCurFocus(-1);
}

BOOL CMemberListBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
	//return CListBox::OnEraseBkgnd(pDC);
}

void CMemberListBox::EnsureVisible(int index)
{
	//SetCaretIndex(index, FALSE);
	//SetTopIndex(index);

	CRect rc;
	GetClientRect(&rc);
	int cVisible = rc.Height() / m_nItemHeight;
	ASSERT(cVisible == 10 || cVisible == GetCount());

	int nTop = GetTopIndex();
	ASSERT(nTop >= 0);

	if (index < nTop || index > nTop + cVisible - 1)
	{
		int nTopNew;
		if (nTop < index)
			nTopNew = max(0, index - cVisible + 1);
		else
			nTopNew = index;
		SetTopIndex(nTopNew);
	}
}

void CMemberListBox::CenterItem(int index)
{
	CRect rc;
	GetClientRect(&rc);
	int cVisible = rc.Height() / m_nItemHeight;
	ASSERT(cVisible == 10 || cVisible == GetCount());

	int nTop = GetTopIndex();
	ASSERT(nTop >= 0);

	int nTopNew = max(0, index - cVisible / 2 + 1);

	SetTopIndex(nTopNew);
}
