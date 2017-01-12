// SizingControlBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "SizingControlBarEx.h"
#include "Resource.h"

#include "MemDC.h"

// CSizingControlBarEx

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ID_BUTTON_CLOSE		1000

IMPLEMENT_DYNAMIC(CSizingControlBarEx, CSizingControlBar)
CSizingControlBarEx::CSizingControlBarEx() :
	m_bShowTitle(TRUE)
	, m_bShowCloseButton(TRUE)
	, m_bActive(FALSE)
{
}

CSizingControlBarEx::~CSizingControlBarEx()
{
}


BEGIN_MESSAGE_MAP(CSizingControlBarEx, CSizingControlBar)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BUTTON_CLOSE, OnClickedClose)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CSizingControlBarEx message handlers

void CSizingControlBarEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CSizingControlBar::OnLButtonDblClk(nFlags, point);
	//CWnd::OnLButtonDblClk(nFlags, point);
}

void CSizingControlBarEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CSizingControlBar::OnLButtonDown(nFlags, point);
	//CWnd::OnLButtonDown(nFlags, point);
}

int CSizingControlBarEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_btnClose.Create("", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(0, 0, 0, 0), this, ID_BUTTON_CLOSE);
	m_btnClose.ModifyFBStyle(0, FBS_NOXPSTYLE | FBS_TRANSPARENT);
	
	LoadCloseButtonBitmap(FALSE);

	return 0;
}

void CSizingControlBarEx::GetTitleRect(CRect *pRect)
{
	if (!HasTitle())
	{
		pRect->SetRectEmpty();
	}
	else
	{
		GetClientRectExcludeBorder(pRect);

		if (IsHorzDocked())
			pRect->right = pRect->left + TITLE_SIZE;
		else			
			pRect->bottom = pRect->top + TITLE_SIZE;
	}
}

void CSizingControlBarEx::GetTitleTextRect(CRect *pRect)
{
	ASSERT(HasTitle());

	CString strText;
	GetWindowText(strText);

	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
	CSize size = dc.GetTextExtent(strText);
	dc.SelectObject(pOldFont);

	GetTitleRect(pRect);

	if (IsHorzDocked())
	{
		pRect->bottom -= 5;
		pRect->top = pRect->bottom - size.cx;
	}
	else
	{
		pRect->left += 5;
		pRect->right = pRect->left + size.cx;
	}
}

void CSizingControlBarEx::GetClientRectExcludeBorder(CRect *pRect)
{
	GetClientRect(pRect);
	//pRect->InflateRect(-2, -2);
}

void CSizingControlBarEx::GetClientRectExcludeTitle(CRect *pRect)
{
	CRect rcClient, rcTitle;
	GetClientRectExcludeBorder(&rcClient);
	GetTitleRect(&rcTitle);
	pRect->SubtractRect(&rcClient, rcTitle);
}

void CSizingControlBarEx::PaintClient(CDC & dc)
{
}

void CSizingControlBarEx::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RecalcLayout();
}

void CSizingControlBarEx::RecalcLayout()
{
	if (::IsWindow(m_hWnd))
	{
		const int MARGIN = 2;
		if (HasTitle())
		{
			CRect rc;
			GetTitleRect(&rc);

			int nButtonSize;
			if (IsHorzDocked())
			{
				nButtonSize = rc.Width() - MARGIN * 2;
				m_btnClose.MoveWindow(rc.left + MARGIN, rc.top + MARGIN, nButtonSize, nButtonSize);
			}
			else
			{
				nButtonSize = rc.Height() - MARGIN * 2;
				m_btnClose.MoveWindow(rc.right - TITLE_SIZE + MARGIN, rc.top + MARGIN, nButtonSize, nButtonSize);
			}
		}

		m_btnClose.ShowWindow(HasTitle() && m_bShowCloseButton ? SW_SHOW : SW_HIDE);

		Invalidate();
	}
}

BOOL CSizingControlBarEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CSizingControlBar::PreCreateWindow(cs);
}

void CSizingControlBarEx::OnClickedClose()
{
	GetParentFrame()->ShowControlBar(this, FALSE, FALSE);
}

void CSizingControlBarEx::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	// TODO: Add your specialized code here and/or call the base class

	CSizingControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHandler);

	CheckActive();
}

BOOL CSizingControlBarEx::HasTitle(void)
{
	return (!IsFloating() && m_bShowTitle);
}

void CSizingControlBarEx::ShowTitle(BOOL bShow)
{
	m_bShowTitle = TRUE;
	if (!IsFloating())
		RecalcLayout();
}

int CSizingControlBarEx::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	//TRACE0("CSizingControlBarEx::OnMouseActivate()\n");
	//HWND hWnd = ::GetFocus();
	//BOOL bActive = (hWnd && (hWnd == m_hWnd || ::IsChild(m_hWnd, hWnd)));
	//if (!bActive)
	//{
	//	SetFocus();
	//	CheckActive();
	//}

	return CSizingControlBar::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CSizingControlBarEx::CheckActive(void)
{
	HWND hWnd = ::GetFocus();
	BOOL bActive = (hWnd && (hWnd == m_hWnd || ::IsChild(m_hWnd, hWnd)));
	if (bActive != m_bActive)
	{
		m_bActive = bActive;

		if (!IsFloating())
		{
			if (m_bShowTitle)
			{
				if (m_bShowCloseButton)
					LoadCloseButtonBitmap(m_bActive);

				CRect rcTitle;
				GetTitleRect(&rcTitle);
				InvalidateRect(&rcTitle);
			}
		}
	}
}

BOOL CSizingControlBarEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	if (nHitTest == HTCLIENT)
	{
		CRect rc;
		GetTitleRect(&rc);
		ClientToScreen(&rc);
		CPoint point;
		GetCursorPos(&point);

		if (rc.PtInRect(point))
		{
			BOOL bIn = TRUE;
			if (m_bShowCloseButton)
			{
				CRect rcButton;
				m_btnClose.GetWindowRect(&rcButton);
				rcButton.InflateRect(2, 2);
				if (rcButton.PtInRect(point))
					bIn = FALSE;
			}

			if (bIn)
			{
				HCURSOR hCursor = ::LoadCursor(NULL, IDC_SIZEALL);
				::SetCursor(hCursor);
				return TRUE;
			}
		}
	}

	return CSizingControlBar::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CSizingControlBarEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CSizingControlBar::OnEraseBkgnd(pDC);
	
	CGlandaMemDC dcMem(pDC);

	CRect rc;
	GetClientRect(&rc);

	dcMem.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));

	PaintClient(dcMem);

	if (HasTitle())
		PaintTitle(dcMem);

	return TRUE;
}

void CSizingControlBarEx::PaintTitle(CDC &dc)
{
	CRect rcTitle;
	GetTitleRect(&rcTitle);

	if (m_bActive)
		dc.FillSolidRect(&rcTitle, ::GetSysColor(COLOR_3DSHADOW));

	dc.Draw3dRect(&rcTitle, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));

	CString strText;
	GetWindowText(strText);

	int nBkModeOld = dc.SetBkMode(TRANSPARENT);
	int nTextColorOld = dc.SetTextColor(::GetSysColor(m_bActive ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
	
	if (IsHorzDocked())
	{
		rcTitle.InflateRect(0, -5);

		//LOGFONT lf;
		//memset(&lf, 0, sizeof(lf));
		//CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT))->GetLogFont(&lf);
		//lf.lfEscapement = 900;
		//lf.lfWeight = 700;

		//CFont font;
		//font.CreateFontIndirect(&lf);

		//CFont *pOldFont = dc.SelectObject(&font);
		//dc.ExtTextOut(rcTitle.left, rcTitle.bottom, ETO_CLIPPED, rcTitle, strText, 0);
	
		//dc.SelectObject(pOldFont);
	}
	else
	{
		rcTitle.InflateRect(-5, 0);

		CFont *pOldFont = dc.SelectObject(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
		dc.DrawText(strText, &rcTitle, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
		dc.SelectObject(pOldFont);
	}

	dc.SetBkMode(nBkModeOld);
	dc.SetTextColor(nTextColorOld);
}

void CSizingControlBarEx::LoadCloseButtonBitmap(BOOL bActive)
{
	m_btnClose.LoadBitmap(bActive ? IDB_BUTTON_CLOSE : IDB_BUTTON_CLOSE_HIGHLIGHT, 2);
}
