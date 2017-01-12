// StaticColor.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticColor.h"
#include ".\staticcolor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticColor

CStaticColor::CStaticColor()
{
	m_Color = RGB(0, 0, 0);
}

CStaticColor::~CStaticColor()
{
}


BEGIN_MESSAGE_MAP(CStaticColor, CStatic)
	//{{AFX_MSG_MAP(CStaticColor)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_ENABLE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticColor message handlers

void CStaticColor::SetColor(COLORREF color)
{
	m_Color = color;
	if (::IsWindow(m_hWnd))
		Invalidate();
}

COLORREF CStaticColor::GetColor()
{
	return m_Color;
}

void CStaticColor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);

	dc.FillSolidRect(&rc, m_Color);

	// Do not call CStatic::OnPaint() for painting messages
}

void CStaticColor::OnEnable(BOOL bEnable)
{
	// avoid repainting.
	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
