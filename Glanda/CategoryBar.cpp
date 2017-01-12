// CategoryBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryBar.h"
#include "CategoryWnd.h"
#include "DeferWindowPos.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CCategoryBar

IMPLEMENT_DYNAMIC(CCategoryBar, CGlandaSizingControlBar)
CCategoryBar::CCategoryBar()
{
}

CCategoryBar::~CCategoryBar()
{
	CCategoryWnd::ReleaseInstance();
}


BEGIN_MESSAGE_MAP(CCategoryBar, CGlandaSizingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CCategoryBar message handlers


int CCategoryBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGlandaSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	if (!CCategoryWnd::Instance()->CreateEx(0, NULL,	NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1))
		return -1;

	return 0;
}

void CCategoryBar::OnSize(UINT nType, int cx, int cy)
{
	CGlandaSizingControlBar::OnSize(nType, cx, cy);

	if (::IsWindow(m_hWnd))
	{
		CDeferWindowPos dw;

		CRect rcAvail;
		GetClientRectExcludeTitle(&rcAvail);

		int nMargin = IsFloating() ? 3 : 0;
		rcAvail.InflateRect(-nMargin, -nMargin);

		dw.DeferWindowPos(CCategoryWnd::Instance()->m_hWnd, NULL, rcAvail, SWP_NOZORDER);
		dw.EndDeferWindowPos();
	}
}
