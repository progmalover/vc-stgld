// EffectBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EffectBar.h"
#include "EffectWnd.h"
#include "DlgEffectPanel.h"
#include "DeferWindowPos.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CEffectBar

IMPLEMENT_DYNAMIC(CEffectBar, CGlandaSizingControlBar)
CEffectBar::CEffectBar()
{
}

CEffectBar::~CEffectBar()
{
	CEffectWnd::ReleaseInstance();
}


BEGIN_MESSAGE_MAP(CEffectBar, CGlandaSizingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CEffectBar message handlers


int CEffectBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGlandaSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!CEffectWnd::Instance()->CreateEx(0, NULL,	NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL, CRect(0, 0, 0, 0), this, 1))
		return -1;

	if (!m_dlgEffect.Create(IDD_EFFECT_PANEL, this))
		return -1;
	m_dlgEffect.ShowWindow(SW_SHOW);

	return 0;
}

void CEffectBar::OnSize(UINT nType, int cx, int cy)
{
	CGlandaSizingControlBar::OnSize(nType, cx, cy);

	if (::IsWindow(m_hWnd))
	{
		CDeferWindowPos dw;

		CRect rcAvail;
		GetClientRectExcludeTitle(&rcAvail);

		int nMargin = IsFloating() ? 4 : 1;
		rcAvail.InflateRect(-nMargin, -nMargin);

		CRect rcDlg;
		m_dlgEffect.GetWindowRect(&rcDlg);
		int cxPane = rcDlg.Width();
		int cyPane = rcDlg.Height();

		dw.DeferWindowPos(m_dlgEffect.m_hWnd, NULL, 
			rcAvail.left, rcAvail.top, max(m_dlgEffect.m_rcInitial.Width(), rcAvail.Width()), cyPane, SWP_NOZORDER);

		dw.DeferWindowPos(CEffectWnd::Instance()->m_hWnd, NULL, 
			rcAvail.left, rcAvail.top + rcDlg.Height() + 1, rcAvail.Width(), rcAvail.Height() - rcDlg.Height() - 1, SWP_NOZORDER);

		dw.EndDeferWindowPos();
	}
}

BOOL CEffectBar::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//if (CEffectWnd::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	return CGlandaSizingControlBar::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CEffectBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CGlandaSizingControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHandler);

	CEffectWnd::Instance()->SetActive(m_bActive);

	m_dlgEffect.UpdateDialogControls(pTarget, bDisableIfNoHandler);
}

void CEffectBar::PaintClient(CDC& dc)
{
	CRect rc;

	CEffectWnd::Instance()->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.InflateRect(1, 1);
	dc.Draw3dRect(&rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

BOOL CEffectBar::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

//	return CGlandaSizingControlBar::PreTranslateMessage(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void CEffectBar::OnSetFocus(CWnd* pOldWnd)
{
	CGlandaSizingControlBar::OnSetFocus(pOldWnd);


	CEffectWnd::Instance()->SetFocus();
}
