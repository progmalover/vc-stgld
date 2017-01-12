// MenuTreeBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "SceneListBar.h"

#include "DeferWindowPos.h"

#include "SceneListCtrl.h"
#include "GlandaDoc.h"

#include "my_app.h"
#include "CmdChangeCurrentScene.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSceneListBar

IMPLEMENT_DYNAMIC(CSceneListBar, CGlandaSizingControlBar)
IMPLEMENT_SINGLETON(CSceneListBar)
CSceneListBar::CSceneListBar()
{
	//m_bShowTitle = TRUE;
	//m_bShowCloseButton = FALSE;
}

CSceneListBar::~CSceneListBar()
{
}


BEGIN_MESSAGE_MAP(CSceneListBar, CGlandaSizingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_CONTROL(FLCN_SELCHANGED, 1, OnSceneListSelChanged)
END_MESSAGE_MAP()



// CSceneListBar message handlers


int CSceneListBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGlandaSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CSceneListCtrl *pSceneList = CSceneListCtrl::Instance();
	if (!pSceneList->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL, CRect(0, 0, 0, 0), this, 1))
		return -1;

	pSceneList->ModifyStyle(WS_VSCROLL, 0);
	pSceneList->SetBkColor(COLOR_WINDOW, TRUE);

	return 0;
}

void CSceneListBar::GetToolBarRect(CRect * pRect)
{
	pRect->SetRectEmpty();
}

void CSceneListBar::OnSize(UINT nType, int cx, int cy)
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

		dw.DeferWindowPos(CSceneListCtrl::Instance()->m_hWnd, NULL, rcAvail.left + 1, rcAvail.top + 1, rcAvail.Width() - 2, rcAvail.Height() - 2, SWP_NOZORDER);

		dw.EndDeferWindowPos();
	}
}

void CSceneListBar::PaintClient(CDC & dc)
{
	CRect rc;
	CSceneListCtrl::Instance()->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.InflateRect(1, 1);
	dc.Draw3dRect(&rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CSceneListBar::OnSetFocus(CWnd* pOldWnd)
{
	CGlandaSizingControlBar::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	CSceneListCtrl::Instance()->SetFocus();
}

BOOL CSceneListBar::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	// default implemention handles the status bar text incorrectly
	return CWnd::PreTranslateMessage(pMsg);

	//return CGlandaSizingControlBar::PreTranslateMessage(pMsg);
}

void CSceneListBar::OnDestroy()
{
	CGlandaSizingControlBar::OnDestroy();

	// TODO: Add your message handler code here

	CSceneListCtrl::Instance()->DestroyWindow();
	CSceneListCtrl::ReleaseInstance();
}


void CSceneListBar::OnSceneListSelChanged()
{
	gldScene2* pOldScene = _GetCurScene2();
	gldScene2* pNewScene = CSceneListCtrl::Instance()->GetSelectedScene();

	// 若不存在当前场景,是因为调用了UnselectItem,此时不应作处理
	if (pNewScene && pOldScene != pNewScene)
		my_app.Commands().Do(new CCmdChangeCurrentScene(pNewScene));
}
