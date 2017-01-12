// TrayWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TrayWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrayWnd

CTrayWnd::CTrayWnd()
{
	m_nPadding = 0;
	m_pChild = NULL;
}

CTrayWnd::~CTrayWnd()
{
}


BEGIN_MESSAGE_MAP(CTrayWnd, CWnd)
	//{{AFX_MSG_MAP(CTrayWnd)
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTrayWnd message handlers

BOOL CTrayWnd::Create(CRect rc)
{
	if (!CWnd::Create(AfxRegisterWndClass(0), "", WS_CHILD, rc, GetDesktopWindow(), 0, NULL))
		return FALSE;

	ModifyStyleEx(0, WS_EX_TOOLWINDOW);

	return TRUE;
}

void CTrayWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);
	dc.DrawEdge(&rc, EDGE_RAISED, BF_RECT);
	if (m_nPadding > 0)
	{
		rc.InflateRect(-2, -2);
		dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
	}

	// Do not call CWnd::OnPaint() for painting messages
}

void CTrayWnd::SetChild(CWnd *pWnd)
{
	m_pChild = pWnd;
	RecalcLayout();
}

void CTrayWnd::SetPadding(int nPadding)
{
	m_nPadding = nPadding;
	RecalcLayout();
}

void CTrayWnd::RecalcLayout()
{
	if (::IsWindow(m_hWnd) && m_pChild != NULL && ::IsWindow(m_pChild->m_hWnd))
	{
		CRect rc;
		m_pChild->GetWindowRect(&rc);
		rc.InflateRect(2 + m_nPadding, 2 + m_nPadding);
		SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOMOVE);

		rc.OffsetRect(-rc.left, -rc.top);
		rc.InflateRect(-(2 + m_nPadding), -(2 + m_nPadding));
		m_pChild->MoveWindow(&rc);
	}
}

void CTrayWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);
	
	// TODO: Add your message handler code here
	
	if (lpwndpos->flags & SWP_SHOWWINDOW)
	{
		if (m_pChild)
		{
			RecalcLayout();
			m_pChild->ShowWindow(SW_SHOW);
		}
	}
	else if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		m_pChild->ShowWindow(SW_HIDE);
	}
}
