// PopupSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PopupSliderCtrl.h"
#include "Utils.h"

#include <afxpriv.h>
#include <..\src\mfc\afximpl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CX_SLIDER		14
#define CY_SLIDER		120

#define GetBuddyWnd()			(CWnd::FromHandle(m_hBuddy))

DEFINE_REGISTERED_MESSAGE(WM_SLIDERPOSINIT)
DEFINE_REGISTERED_MESSAGE(WM_SLIDERPOSCHANGED)
DEFINE_REGISTERED_MESSAGE(WM_SLIDERENDTRACKING)

/////////////////////////////////////////////////////////////////////////////
// CPopupSliderCtrl

CPopupSliderCtrl::CPopupSliderCtrl()
{
	m_hBuddy = NULL;
	m_bFirstMove = TRUE;
}

CPopupSliderCtrl::~CPopupSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CPopupSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CPopupSliderCtrl)
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopupSliderCtrl message handlers

void CPopupSliderCtrl::SetBuddyWindow(HWND hWnd)
{
	m_hBuddy = hWnd;
}

void CPopupSliderCtrl::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	if (GetCapture() != this)
	{
		m_wndTray.ShowWindow(SW_HIDE);
		if (m_hBuddy)
		{
			CWnd *pWnd = GetBuddyWnd();
			ASSERT(pWnd != NULL);
			ASSERT(::IsWindow(pWnd->m_hWnd));
			if (_AfxCompareClassName(pWnd->m_hWnd, "EDIT"))
				((CEdit *)pWnd)->SetSel(0, -1);
			pWnd->SetFocus();

			::SendMessage(m_hBuddy, WM_SLIDERENDTRACKING, (WPARAM)0, (LPARAM)this);
		}
	}

	CSliderCtrl::OnCaptureChanged(pWnd);
}

void CPopupSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	ReleaseCapture();

	CSliderCtrl::OnLButtonUp(nFlags, point);
}

void CPopupSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	TRACE1("CPopupSliderCtrl::OnMouseMove(): m_bFirstMove == %d\n", m_bFirstMove);

	// ignore the first WM_MOUSEMOVE
	if (m_bFirstMove)
	{
		m_bFirstMove = FALSE;
		return;
	}

	if (m_hBuddy)
	{
		int nPos = GetPosImpl();

		if (::SendMessage(m_hBuddy, WM_SLIDERPOSCHANGED, (WPARAM)nPos, (LPARAM)this) == 0)
		{
			CString str;
			str.Format("%d", nPos);
			AfxSetWindowText(m_hBuddy, str);
			::UpdateWindow(m_hBuddy);
		}
	}
	
	CSliderCtrl::OnMouseMove(nFlags, point);
}

void CPopupSliderCtrl::SetPosImpl(int nPos)
{
	TRACE1("SetPosImpl(%d)\n", nPos);
	if (m_nDirection == BOTTOMTOP || m_nDirection == RIGHTLEFT)
	{
		int nMin, nMax;
		GetRange(nMin, nMax);
		nPos = min(nMax, max(nMin, nPos));
		SetPos(nMax - nPos + nMin);
	}
	else
	{
		SetPos(nPos);
	}
}

int CPopupSliderCtrl::GetPosImpl()
{
	if (m_nDirection == BOTTOMTOP || m_nDirection == RIGHTLEFT)
	{
		TRACE1("GetPos()=%d\n", GetPos());
		int nMin, nMax;
		GetRange(nMin, nMax);
		return nMax - GetPos() + nMin;
	}
	else
	{
		return GetPos();
	}
}

BOOL CPopupSliderCtrl::Create(BOOL bHorz, int nDirection)
{
	m_nDirection = nDirection;

	if (!m_wndTray.Create(CRect(0, 0, 0, 0)))
		return FALSE;

	if (!CSliderCtrl::Create(WS_CHILD | WS_VISIBLE | (bHorz ? TBS_HORZ : TBS_VERT) | TBS_NOTICKS | TBS_LEFT, 
		bHorz ? CRect(0, 0, CY_SLIDER, CX_SLIDER) : CRect(0, 0, CX_SLIDER, CY_SLIDER), 
		&m_wndTray, 
		0))
		return FALSE;

	m_wndTray.SetChild(this);

	return TRUE;
}

void CPopupSliderCtrl::Activate(CPoint point)
{
	ASSERT(m_hBuddy);
	GetBuddyWnd()->SetFocus();

	// must ignore the firs WM_MOUSEMOVE
	m_bFirstMove = TRUE;

	int nPos = 0;
	if (::SendMessage(m_hBuddy, WM_SLIDERPOSINIT, 0, (LPARAM)&nPos) == 0)
	{
		CString str;
		GetBuddyWnd()->GetWindowText(str);
		nPos = atoi(str);
	}

	SetPosImpl(nPos);


	int nMin = GetRangeMin();
	SetRangeMin(nMin, TRUE);

	CRect rc;
	GetThumbRect(&rc);
	ClientToScreen(&rc);
	int x = (rc.left + rc.right) / 2;
	int y = (rc.top + rc.bottom) / 2;

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(point.x - x, point.y - y);
	if (!(GetStyle() & TBS_VERT))
	{
		CRect rcOwner;
		GetOwner()->GetWindowRect(&rcOwner);
		rcWindow.OffsetRect(0, rcOwner.top - rcWindow.bottom - 1);
	}

	::EnsureWholeRectVisible(rcWindow);

	m_wndTray.SetWindowPos(&wndTopMost, rcWindow.left, rcWindow.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	GetThumbRect(&rc);
	x = (rc.left + rc.right) / 2;
	y = (rc.top + rc.bottom) / 2;
	SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(x, y));

	SetPosImpl(nPos);
}
