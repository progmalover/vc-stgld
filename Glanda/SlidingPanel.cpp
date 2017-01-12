// SlidingPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "SlidingPanel.h"
#include "CategoryPage.h"
#include "DeferWindowPos.h"
#include "SlidingPanelGroup.h"
#include "Graphics.h"
#include "VisualStylesXP.h"
#include "SlidingPanelGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSlidingPanel
const UINT ID_SLIDING_ARROW = 1;

IMPLEMENT_DYNAMIC(CSlidingPanel, CWnd)
CSlidingPanel::CSlidingPanel(CSlidingPage* pSlidingPage, int nImage, UINT nIDCaption)
: m_pSlidingPage(pSlidingPage)
, m_nImage(nImage)
, m_bExpanded(FALSE)
, m_nIDCaption(nIDCaption)
, m_btnSlidingArrow(this)
{
}

CSlidingPanel::~CSlidingPanel()
{
}


BEGIN_MESSAGE_MAP(CSlidingPanel, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_SLIDING_ARROW, OnSlidingArrow)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CSlidingPanel message handlers

void CSlidingPanel::Expand()
{
	if(!m_bExpanded)
	{
		CWaitCursor wc;

		m_bExpanded = TRUE;
		m_btnSlidingArrow.Expand(TRUE);

		if(!::IsWindow(m_pSlidingPage->m_hWnd))
		{
			m_pSlidingPage->InitPage(this);
		}

		CRect rc;
		GetClientRect(&rc);
		rc.top += GetTitleHeight();

		m_pSlidingPage->SetWindowPos(&wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
		m_pSlidingPage->OnSetActive();
		m_pSlidingPage->SetFocus();
	}
}

void CSlidingPanel::Collapse()
{
	if(m_bExpanded)
	{
		m_bExpanded = FALSE;
		m_btnSlidingArrow.Expand(FALSE);

		if(::IsWindow(m_pSlidingPage->m_hWnd))
		{
			m_pSlidingPage->OnKillActive();
			m_pSlidingPage->ShowWindow(SW_HIDE);
		}
		else
		{
			// Invalid page
		}
	}
}

int CSlidingPanel::GetTitleHeight()
{
	return ::GetSystemMetrics(SM_CYMENU);
}

int CSlidingPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	CRect rc;
	GetSlidingArrowRect(rc);

	CString strCaption;
	strCaption.LoadString(m_nIDCaption);
	m_btnSlidingArrow.Create(strCaption, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, rc, this, ID_SLIDING_ARROW);

	return 0;
}

void CSlidingPanel::OnDestroy()
{
	CWnd::OnDestroy();

	if(m_pSlidingPage && ::IsWindow(m_pSlidingPage->m_hWnd))
	{
		if(::IsWindow(m_pSlidingPage->m_hWnd))
		{
			m_pSlidingPage->DestroyWindow();
		}
		m_btnSlidingArrow.DestroyWindow();
	}
}

void CSlidingPanel::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CSlidingPanel::RecalcLayout()
{
	CDeferWindowPos dw;
	dw.BeginDeferWindowPos();

	CRect rc;
	GetSlidingArrowRect(rc);
	dw.DeferWindowPos(m_btnSlidingArrow.m_hWnd, NULL, rc, SWP_NOZORDER);
	
	if(m_bExpanded)
	{
		if(::IsWindow(m_pSlidingPage->m_hWnd))
		{
			CRect rc;
			GetClientRect(&rc);
			rc.top += GetTitleHeight();
			dw.DeferWindowPos(m_pSlidingPage->m_hWnd, NULL, rc, SWP_NOZORDER);
		}
	}
	dw.EndDeferWindowPos();
}

void CSlidingPanel::OnSlidingArrow()
{
	CSlidingPanelGroup* pPanelGroup = (CSlidingPanelGroup*)GetParent();
	if(pPanelGroup)
	{
		if(!m_bExpanded)
		{
			pPanelGroup->SetActivePanel(this);
		}
		else
		{
		}
	}
}

void CSlidingPanel::OnPaint()
{

	CPaintDC dc(this);

	CRect rc;
	GetClientRect(&rc);
	rc.top += GetTitleHeight();

	CBrush Brush(GetSysColor(COLOR_BTNFACE));

	for(int i = 0; i < BORDER; ++i)
	{
		dc.FrameRect(&rc, &Brush);
		rc.DeflateRect(1,1);
	}
}

void CSlidingPanel::GetSlidingArrowRect(CRect& rc)
{
	GetClientRect(&rc);
	int nTitleHeight = GetTitleHeight();
	/*
	int nSpacing = max(0, (nTitleHeight - 16) /2);

	rc.top += nSpacing;
	rc.bottom = rc.top + 16;
	rc.right -= nSpacing / 2;
	rc.left = rc.right - 16;
	*/
	rc.bottom = rc.top + nTitleHeight;
}

CSlidingPanel::CSlidingPanelTitleButton::CSlidingPanelTitleButton(CSlidingPanel* pSlidingPanel)
: m_pSlidingPanel(pSlidingPanel)
{
	m_dwStyle = FBS_NOBORDER | FBS_NOXPSTYLE;
	m_bExpanded = FALSE;
	m_bmpArrow.LoadBitmap(IDB_SLIDING_ARROW);
}

BEGIN_MESSAGE_MAP(CSlidingPanel::CSlidingPanelTitleButton, CFlatButton)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CSlidingPanel::CSlidingPanelTitleButton::Expand(BOOL bExpand)
{
	if (m_bExpanded != bExpand)
	{
		m_bExpanded = bExpand;
		Invalidate(FALSE);
	}
}

CSize CSlidingPanel::CSlidingPanelTitleButton::GetStuffSize()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	return rcClient.Size();
}

void CSlidingPanel::CSlidingPanelTitleButton::DrawStuff(CDC* pDC, const CRect &rc)
{
	//if(g_xpStyle.IsAppThemed())
	//{
	//	CRect rc2;
	//	GetClientRect(&rc2);
	//	pDC->FillSolidRect(&rc2, ::GetSysColor(COLOR_WINDOW));
	//	HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"EXPLORERBAR");
	//	if (hTheme)
	//	{
	//		g_xpStyle.DrawThemeBackground(hTheme, pDC->m_hDC, EBP_NORMALGROUPHEAD, EBNGE_HOT, &rc2, NULL);

	//		CRect rc3(rc2);
	//		rc3.left = rc3.right - rc3.Height();
	//		rc3.OffsetRect(-2, 0);

	//		int iPartId;
	//		if(m_pSlidingPanel->m_bExpanded)
	//		{
	//			iPartId = EBP_NORMALGROUPEXPAND;
	//		}
	//		else
	//		{
	//			iPartId = EBP_NORMALGROUPCOLLAPSE;
	//		}

	//		int iStateId;
	//		switch(m_dwState)
	//		{
	//		case STATE_FLAT:
	//			iStateId = EBNGE_HOT;
	//			break;
	//		case STATE_UP:
	//			iStateId = EBNGE_NORMAL;
	//			break;
	//		case STATE_DOWN:
	//			iStateId = EBNGE_PRESSED;
	//			break;
	//		}
	//		g_xpStyle.DrawThemeBackground(hTheme, pDC->m_hDC, iPartId, iStateId, &rc3, NULL);

	//		CString strText;
	//		GetWindowText(strText);
	//		BSTR bstrText = strText.AllocSysString();
	//		CRect rc4(rc2);
	//		rc4.left += 3;
	//		//g_xpStyle.GetThemeBackgroundContentRect(hTheme, pDC->m_hDC, iPartId, iStateId, &rc2, &rc4);
	//		g_xpStyle.DrawThemeText(hTheme, pDC->m_hDC, iPartId, iStateId, bstrText, SysStringLen(bstrText), DT_LEFT|DT_VCENTER, NULL, &rc4);
	//		SysFreeString(bstrText);


	//		g_xpStyle.CloseThemeData(hTheme);
	//	}
	//}
	//else
	{
		// draw background
		DrawGradientRect(pDC, rc, RGB(0xff, 0xff, 0xff), ::GetSysColor(COLOR_3DFACE), FALSE);

		//draw image
		/*
		CRect rcImage = rc;
		rcImage.left += 4;
		if (m_pSlidingPanel->m_nImage >= 0)
		{
			CSlidingPanelGroup *pGroup = (CSlidingPanelGroup *)m_pSlidingPanel->GetParent(); 
			CImageList *pImageList = pGroup->GetImageList();
			if (pImageList)
			{
				IMAGEINFO ii;
				pImageList->GetImageInfo(m_pSlidingPanel->m_nImage, &ii);
				CPoint point;
				point.x = rcImage.left;
				point.y = rcImage.top + (rcImage.Height() - (ii.rcImage.bottom - ii.rcImage.top)) / 2;
				pImageList->Draw(pDC, m_pSlidingPanel->m_nImage, point, ILD_TRANSPARENT);
			}
		}
		*/

		// draw text
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkMode(TRANSPARENT);
		CString str;
		GetWindowText(str);

		CRect rcText = rc;

		rcText.left += /*rcText.Height() +*/ 4;
		rcText.right -= rcText.Height();

		CFont *pOldFont = pDC->SelectObject(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

		pDC->DrawText(str, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

		//if (GetFocus() == this)
		//{
		//	CRect rcFocus = rcClient;
		//	rcFocus.InflateRect(-2, -1);
		//	pDC->DrawFocusRect(&rcFocus);
		//}

		pDC->SelectObject(pOldFont);

		BITMAP bm;
		m_bmpArrow.GetBitmap(&bm);
		int x = rc.right - bm.bmWidth / 2;
		int y = rc.top + (rc.Height() - bm.bmHeight) / 2;
		::DrawTransparent(pDC, x, y, bm.bmWidth / 2, bm.bmHeight, 
			&m_bmpArrow, m_bExpanded ? bm.bmWidth / 2 : 0, 0, RGB(255, 0, 255));
	}
}

BOOL CSlidingPanel::CSlidingPanelTitleButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	::SetCursor(::AfxGetApp()->LoadCursor(IDC_MY_HAND));
	return TRUE;

	//return CFlatBitmapButton::OnSetCursor(pWnd, nHitTest, message);
}
