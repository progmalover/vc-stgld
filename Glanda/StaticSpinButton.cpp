// StaticSpinButton.cpp : implementation file
//

#include "stdafx.h"
#include "StaticSpinButton.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinButton

CStaticSpinButton::CStaticSpinButton()
{
	m_bHorz = FALSE;
	m_nDirection = m_bHorz ? LEFTRIGHT : BOTTOMTOP;
}

CStaticSpinButton::~CStaticSpinButton()
{
}


BEGIN_MESSAGE_MAP(CStaticSpinButton, CStatic)
	//{{AFX_MSG_MAP(CStaticSpinButton)
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinButton message handlers

void CStaticSpinButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);

	if (g_xpStyle.IsAppThemed())
	{
		HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"TOOLBAR");
		if (hTheme)
		{
			g_xpStyle.DrawThemeParentBackground(m_hWnd, dc, &rc);
			g_xpStyle.DrawThemeBackground(hTheme, dc, TP_BUTTON, TS_HOT, &rc, NULL);
			g_xpStyle.CloseThemeData(hTheme);
		}
	}
	else
	{
		CWnd *pParent = GetParent();
		
		CPoint pt(0, 0);
		MapWindowPoints(pParent, &pt, 1);
		pt = dc.OffsetWindowOrg(pt.x, pt.y);
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rc);
		dc.SelectClipRgn(&rgn, RGN_COPY);
		LRESULT lResult = pParent->SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC, 0L);
		dc.SelectClipRgn(NULL, RGN_COPY);
		dc.SetWindowOrg(pt.x, pt.y);
			
		dc.Draw3dRect(&rc, ::GetSysColor(COLOR_3DHILIGHT), ::GetSysColor(COLOR_3DSHADOW));
		rc.InflateRect(-1, -1);
	}

	int x = rc.right / 2;
	int y = rc.bottom / 2;

	COLORREF cr = ::GetSysColor(IsWindowEnabled() ? RGB(255, 0, 0) : COLOR_3DSHADOW);

	if (m_bHorz)
	{
		dc.SetPixel(x, y - 2, cr);
		dc.SetPixel(x, y - 1, cr);
		dc.SetPixel(x, y, cr);
		dc.SetPixel(x, y + 1, cr);
		dc.SetPixel(x, y + 2, cr);
		
		dc.SetPixel(x + 1, y - 1, cr);
		dc.SetPixel(x + 1, y, cr);
		dc.SetPixel(x + 1, y + 1, cr);
		
		dc.SetPixel(x + 2, y, cr);

		if (!IsWindowEnabled())
		{
			cr = ::GetSysColor(COLOR_3DHILIGHT);
			dc.SetPixel(x + 2, y + 1, cr);
			dc.SetPixel(x + 1, y + 2, cr);
		}
	}
	else
	{
		dc.SetPixel(x - 2, y, cr);
		dc.SetPixel(x - 1, y, cr);
		dc.SetPixel(x, y, cr);
		dc.SetPixel(x + 1, y, cr);
		dc.SetPixel(x + 2, y, cr);
		
		dc.SetPixel(x - 1, y + 1, cr);
		dc.SetPixel(x, y + 1, cr);
		dc.SetPixel(x + 1, y + 1, cr);
		
		dc.SetPixel(x, y + 2, cr);

		if (!IsWindowEnabled())
		{
			cr = ::GetSysColor(COLOR_3DHILIGHT);
			dc.SetPixel(x + 2, y + 1, cr);
			dc.SetPixel(x + 1, y + 2, cr);
		}
	}

	// Do not call CStatic::OnPaint() for painting messages
}

void CStaticSpinButton::OnEnable(BOOL bEnable) 
{
	//CStatic::OnEnable(bEnable);
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);
	rc.InflateRect(-1, -1);
	InvalidateRect(&rc, FALSE);
}

void CStaticSpinButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	m_wndSlider.Create(m_bHorz, m_nDirection);
	m_wndSlider.SetOwner(this);

	CStatic::PreSubclassWindow();
}

void CStaticSpinButton::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	ASSERT(::IsWindow(m_wndSlider.m_hWnd));
	m_wndSlider.SetRange(nMin, nMax, bRedraw);
}

void CStaticSpinButton::SetBuddyWindow(HWND hWnd)
{
	ASSERT(::IsWindow(m_wndSlider.m_hWnd));
	m_wndSlider.SetBuddyWindow(hWnd);
}

void CStaticSpinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint point2 = point;
	ClientToScreen(&point2);
	m_wndSlider.Activate(point2);

	CStatic::OnLButtonDown(nFlags, point);
}

void CStaticSpinButton::SetSliderStyle(BOOL bHorz, int nDirection)
{
	ASSERT(!::IsWindow(m_hWnd));	// muset set style before create window
	m_bHorz = bHorz;
	m_nDirection = nDirection;
}

void CStaticSpinButton::OnNcDestroy()
{
	CStatic::OnNcDestroy();

	// TODO: Add your message handler code here

	m_wndSlider.DestroyWindow();
}
