// StaticLink.cpp : implementation file
//

#include "stdafx.h"
#include ".\StaticLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticLink

CStaticLink::CStaticLink()
{
	m_bMouseCaptured = FALSE;

	m_bHover = FALSE;
	m_hCursor = NULL;
	m_crNormal = RGB(0, 0, 255);
	m_crHover = RGB(255, 0, 0);
}

CStaticLink::~CStaticLink()
{
}

BEGIN_MESSAGE_MAP(CStaticLink, CStaticText)
	//{{AFX_MSG_MAP(CStaticLink)
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticLink message handlers


BOOL CStaticLink::Init()
{
	if (CStaticText::Init())
	{
		ModifyStyle(0, 	SS_NOTIFY);
		SetFontUnderline(TRUE);
		OnChangeState(FALSE);

		return TRUE;
	}

	return FALSE;
}

void CStaticLink::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CStaticText::OnMouseMove(nFlags, point);

	//TRACE0("OnMouseMove()\n");

	CRect rc;
	GetClientRect(&rc);
	BOOL bHover = PtInRect(&rc, point);
	if (m_bHover != bHover)
	{
		m_bHover = bHover;
		if (m_hCursor)
			::SetCursor(m_hCursor);
		OnChangeState(bHover);
	}

	if (bHover && !m_bMouseCaptured)
	{
		SetCapture();
		m_bMouseCaptured = TRUE;
	}
	else if (!bHover && m_bMouseCaptured)
	{
		ReleaseCapture();
	}
}

void CStaticLink::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here

	TRACE0("OnCaptureChanged()\n");

	if (m_bMouseCaptured)
	{
		m_bMouseCaptured  = FALSE;
		if (m_bHover)
		{
			m_bHover = FALSE;
			OnChangeState(FALSE);
		}
	}

	CStaticText::OnCaptureChanged(pWnd);
}

void CStaticLink::SetHoverCursor(HCURSOR hCursor)
{
	m_hCursor = hCursor;
}

void CStaticLink::OnChangeState(BOOL bHover)
{
	if (bHover)
	{
		SetTextColor(RGB(0, 0, 255));
		SetFontUnderline(TRUE);
	}
	else
	{
		SetTextColor(RGB(0, 0, 192));
		SetFontUnderline(TRUE);
	}
}

void CStaticLink::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	ReleaseCapture();

	CStaticText::OnLButtonDown(nFlags, point);
}
