// StaticSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticSplitter.h"


// CStaticSplitter

IMPLEMENT_DYNAMIC(CStaticSplitter, CStatic)
CStaticSplitter::CStaticSplitter()
{
}

CStaticSplitter::~CStaticSplitter()
{
}


BEGIN_MESSAGE_MAP(CStaticSplitter, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CStaticSplitter message handlers


void CStaticSplitter::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DFACE)));
	if (rc.Width() > rc.Height())
	{
		rc.bottom = rc.top + 1;
		dc.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));
	}
	else
	{
		rc.right = rc.left + 1;
		dc.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));
	}
}
