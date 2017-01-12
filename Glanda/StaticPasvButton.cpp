// StaticPasvButton.cpp : implementation file
//

#include "stdafx.h"
#include "StaticPasvButton.h"
#include "Graphics.h"

#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticPasvButton

CStaticPasvButton::CStaticPasvButton()
{
	m_nState = 0;
	m_bTimer = FALSE;
}

CStaticPasvButton::~CStaticPasvButton()
{
}


BEGIN_MESSAGE_MAP(CStaticPasvButton, CStatic)
	//{{AFX_MSG_MAP(CStaticPasvButton)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticPasvButton message handlers

void CStaticPasvButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);

	BITMAP bm;
	m_Bmp.GetBitmap(&bm);
	int x = (rc.Width() - bm.bmWidth) / 2;
	int y = (rc.Height() - bm.bmHeight) / 2;
	if (m_nState != 0)
	{
		x++;
		y++;
	}

	if (g_xpStyle.IsAppThemed())
	{
		HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"TOOLBAR");
		if (hTheme)
		{
			g_xpStyle.DrawThemeParentBackground(m_hWnd, dc, &rc);
			g_xpStyle.DrawThemeBackground(hTheme, dc, TP_BUTTON, m_nState == 0 ? TS_HOT : TS_PRESSED, &rc, NULL);

			g_xpStyle.CloseThemeData(hTheme);
		}
	}
	else
	{
		dc.Draw3dRect(&rc, 
			::GetSysColor(m_nState == 0 ? COLOR_3DHILIGHT : COLOR_3DSHADOW), 
			::GetSysColor(m_nState == 0 ? COLOR_3DSHADOW : COLOR_3DHILIGHT));

		rc.InflateRect(-1, -1);
		dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
	}

	::DrawTransparent(&dc, x, y, bm.bmWidth, bm.bmHeight, &m_Bmp, 0, 0, m_crTrans);

	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CStaticPasvButton::LoadBitmap(UINT nID, COLORREF crTrans)
{
	m_crTrans = crTrans;

	if (m_Bmp.m_hObject)
		m_Bmp.DeleteObject();

	return m_Bmp.LoadBitmap(nID);
}

void CStaticPasvButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (!m_bTimer)
	{
		if ((GetKeyState(VK_LBUTTON) >> (sizeof(SHORT) - 1)))
		{
			SetState(1);
			SetTimer(1, 100, NULL);
			m_bTimer = TRUE;
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CStaticPasvButton::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	CPoint point;
	GetCursorPos(&point);

	CRect rc;
	GetWindowRect(&rc);

	if (!PtInRect(&rc, point))
	{
		KillTimer(1);
		m_bTimer = FALSE;
		SetState(0);
	}

	CStatic::OnTimer(nIDEvent);
}

void CStaticPasvButton::SetState(int nState)
{
	if (m_nState != nState)
	{
		m_nState = nState;
		Invalidate();
	}
}

void CStaticPasvButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	ClientToScreen(&point);

	CRect rc;
	GetWindowRect(&rc);

	if (PtInRect(&rc, point))
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);

	CStatic::OnLButtonUp(nFlags, point);
}
