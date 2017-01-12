// MenuTreeBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OutputBar.h"

#include "DeferWindowPos.h"

#include "OutputDoc.h"
#include "OutputView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_REGISTERED_MESSAGE(WM_OUTPURBAR_SHOW);

// COutputBar

IMPLEMENT_DYNAMIC(COutputBar, CGlandaSizingControlBar)
IMPLEMENT_SINGLETON(COutputBar)
COutputBar::COutputBar()
{
	m_pView = FALSE;
}

COutputBar::~COutputBar()
{
}


BEGIN_MESSAGE_MAP(COutputBar, CGlandaSizingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(WM_OUTPURBAR_SHOW, OnShow)
END_MESSAGE_MAP()



// COutputBar message handlers


int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGlandaSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CCreateContext ctx;
	ctx.m_pCurrentDoc = new COutputDoc;
	ctx.m_pNewViewClass = RUNTIME_CLASS(COutputView);
	m_pView = (COutputView *)((CFrameWnd*)this)->CreateView(&ctx);
	ASSERT(m_pView);
	m_pView->ShowWindow(SW_NORMAL);
	if (m_pView)
	{
		m_pView->SetWindowText("Output");

		CCrystalTextBuffer *pBuf = m_pView->LocateTextBuffer();
		pBuf->InitNew();
		pBuf->SetReadOnly(TRUE);
		pBuf->EnableUndo(FALSE);

		m_pView->OnInitialUpdate();

		m_pView->AttachToBuffer(pBuf);

		CRect rc;
		GetClientRect(&rc);

		rc.InflateRect(-3, -5, -2, -2);
		
		m_pView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

		m_pView->MoveWindow(&rc);
	}

	return 0;
}

void COutputBar::GetToolBarRect(CRect * pRect)
{
	pRect->SetRectEmpty();
}

void COutputBar::OnSize(UINT nType, int cx, int cy)
{
	CGlandaSizingControlBar::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (::IsWindow(m_hWnd))
	{
		CDeferWindowPos dw;

		CRect rcAvail;
		GetClientRectExcludeTitle(&rcAvail);

		int nMargin = IsFloating() ? 3 : 1;
		rcAvail.InflateRect(-nMargin, -nMargin);

		dw.DeferWindowPos(m_pView->m_hWnd, NULL, rcAvail.left + 1, rcAvail.top + 1, rcAvail.Width() - 2, rcAvail.Height() - 2, SWP_NOZORDER);

		dw.EndDeferWindowPos();
	}
}

void COutputBar::PaintClient(CDC & dc)
{
	CRect rc;
	m_pView->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.InflateRect(1, 1);
	dc.Draw3dRect(&rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void COutputBar::OnSetFocus(CWnd* pOldWnd)
{
	CGlandaSizingControlBar::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	m_pView->SetFocus();
}

BOOL COutputBar::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	// default implemention handles the status bar text incorrectly
	return CWnd::PreTranslateMessage(pMsg);

	//return CGlandaSizingControlBar::PreTranslateMessage(pMsg);
}

// Thread safe method to show/hide the bar
void COutputBar::Show(BOOL bShow)
{
	TRACE1("Show - Thread: %d\n", GetCurrentThreadId());
	if (!IsWindowVisible())
		SendMessage(WM_OUTPURBAR_SHOW, (WPARAM)bShow, 0L);
}

LRESULT COutputBar::OnShow(WPARAM wp, LPARAM lp)
{
	TRACE1("OnShow - Thread: %d\n", GetCurrentThreadId());
	CFrameWnd *pFrame = (CFrameWnd *)AfxGetMainWnd();
	pFrame->ShowControlBar(this, (BOOL)wp, FALSE);
	return 0;
}
