// MiniDockFrameWndEx.cpp : implementation file
//

#include "stdafx.h"
#include "MiniDockFrameWndEx.h"
#include ".\minidockframewndex.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CMiniDockFrameWndEx

IMPLEMENT_DYNCREATE(CMiniDockFrameWndEx, CMiniDockFrameWnd)

CMiniDockFrameWndEx::CMiniDockFrameWndEx()
{
	m_bEnableSizing = TRUE;
}

CMiniDockFrameWndEx::~CMiniDockFrameWndEx()
{
}


BEGIN_MESSAGE_MAP(CMiniDockFrameWndEx, CMiniDockFrameWnd)
	ON_WM_MOUSEACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CMiniDockFrameWndEx message handlers

int CMiniDockFrameWndEx::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	SetForegroundWindow();
	//return MA_ACTIVATE;

	return CMiniDockFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

LRESULT CMiniDockFrameWndEx::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	UINT nRet = CMiniDockFrameWnd::OnNcHitTest(point);
	if (nRet >= HTSIZEFIRST && nRet <= HTSIZELAST && !m_bEnableSizing)
		return HTERROR;
	return nRet;
}

int CMiniDockFrameWndEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	ModifyStyle(0, WS_CLIPCHILDREN);

	return 0;
}

BOOL CMiniDockFrameWndEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	//////////////////////////////////////////////////////////////////
	// For unknown reason, this code will cause error (send unexpected 
	// WM_GETDLGCODE and get deadlock)

	//WNDCLASS wc;
	//wc.lpszMenuName = 0;
	//if (!GetClassInfo(AfxGetInstanceHandle(), "#32770", &wc))
	//{
	//	TRACE0("GetClassInfo failed.\n");
	//	return FALSE;
	//}
	//wc.lpszClassName = "SothinkFloatingFrame";
	//wc.hbrBackground = ::GetSysColorBrush(COLOR_3DFACE);
	//if (!AfxRegisterClass(&wc))
	//{
	//	TRACE0("AfxRegisterClass failed.\n");
	//	return FALSE;
	//}

	//cs.lpszClass = "SothinkFloatingFrame";

	return CMiniDockFrameWnd::PreCreateWindow(cs);
}
