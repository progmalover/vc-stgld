// BalloonToolTip.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BalloonToolTip.h"
#include "Graphics.h"
#include "Help.h"
#include ".\balloontooltip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_W			120
#define ICON_W			13
#define ICON_H			13
#define TEXT_MARGIN		8
#define TRAIL_SIZE		15
#define TRAIL_MARGIN	15

/////////////////////////////////////////////////////////////////////////////
// CBalloonToolTip

IMPLEMENT_SINGLETON(CBalloonToolTip)
CBalloonToolTip::CBalloonToolTip()
{
	m_bUp = TRUE;
	m_bRight = TRUE;
	m_bHoverLink = FALSE;
}

CBalloonToolTip::~CBalloonToolTip()
{
}


BEGIN_MESSAGE_MAP(CBalloonToolTip, CWnd)
	//{{AFX_MSG_MAP(CBalloonToolTip)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
	ON_WM_SETCURSOR()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBalloonToolTip message handlers

WNDPROC CBalloonToolTip::m_lpOldWindowProc = NULL;

LRESULT CALLBACK CBalloonToolTip::ToolTipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = CallWindowProc(m_lpOldWindowProc, hwnd, uMsg, wParam, lParam);

	if (uMsg == WM_ACTIVATEAPP && (BOOL)wParam == FALSE)
		if (CBalloonToolTip::Instance()->IsWindowVisible())
			CBalloonToolTip::Instance()->ShowWindow(SW_HIDE);

	return ret;
}

BOOL CBalloonToolTip::Create(CWnd *pParent)
{
	return CWnd::CreateEx(
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 
		AfxRegisterWndClass(_AfxGetComCtlVersion() >= MAKELONG(0, 6) ? 0x00020000 /*CS_DROPSHADOW*/ : 0, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, 0), 
		NULL, 
		WS_POPUP, 
		CRect(0, 0, 0, 0), 
		pParent, 
		0, 
		NULL);
}

BOOL CBalloonToolTip::Show(int x, int y, UINT nIDText, UINT nIDTitle, LPCTSTR lpszLink)
{
	CString strTitle, strText;
	if (nIDTitle != 0)
		strTitle.LoadString(nIDTitle);
	else
		strTitle.LoadString(AFX_IDS_APP_TITLE);

	strText.LoadString(nIDText);
	
	return Show(x, y, strText, strTitle, lpszLink);
}

BOOL CBalloonToolTip::Show(int x, int y, LPCTSTR lpszText, LPCTSTR lpszTitle, LPCTSTR lpszLink)
{
	if (!::IsWindow(m_hWnd))
	{
		if (!Create(NULL))
			return FALSE;

		// cerate fonts
		LOGFONT lf;

		m_fntText.CreateStockObject(DEFAULT_GUI_FONT);

		m_fntText.GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		m_fntTitle.CreateFontIndirect(&lf);

		m_strLinkText.LoadString(IDS_TELL_ME_MORE);
	}

	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	m_strTitle = lpszTitle != NULL? lpszTitle : AfxGetApp()->m_pszProfileName;
	m_strText = lpszText;
	m_strLink = lpszLink != NULL ? lpszLink : "";

	CClientDC dc(this);

	CFont font;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	CFont *pOldFont = dc.SelectObject(&font);

	m_rc.SetRectEmpty();
	dc.DrawText(lpszText, strlen(lpszText), &m_rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_CALCRECT);
	if (m_rc.Width() < MIN_W)
	{
		m_bSingleLine = TRUE;
		m_rc.right = m_rc.left + MIN_W;
	}
	else
	{
		m_bSingleLine = FALSE;
		m_rc.SetRectEmpty();
		m_rc.right = m_rc.left + nScreenWidth * 2 / 5;
		dc.DrawText(lpszText, strlen(lpszText), &m_rc, DT_WORDBREAK | DT_CALCRECT);
	}

	dc.SelectObject(pOldFont);

	m_rc.InflateRect(TEXT_MARGIN, TEXT_MARGIN + ICON_H + TEXT_MARGIN, TEXT_MARGIN, TEXT_MARGIN);
	m_rc.OffsetRect(-m_rc.left, -m_rc.top);

	if (!m_strLink.IsEmpty())
		m_rc.bottom += (dc.GetTextExtent(m_strLinkText).cy + TEXT_MARGIN);

	m_bUp = (y > m_rc.Height() + TRAIL_SIZE + 20);
	m_bRight = (x + m_rc.Width() - TRAIL_MARGIN < nScreenWidth);

	if (!m_bUp)
		m_rc.OffsetRect(0, TRAIL_SIZE);

	CRect rc;
	CPoint point;
	CalcWindowRgn(rc, point);

	SetWindowRgn((HRGN)m_rgn, TRUE);

	rc.OffsetRect(x - point.x, y - point.y);
	SetWindowPos(&wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);

	return TRUE;
}

BOOL CBalloonToolTip::CalcWindowRgn(CRect &rcBound, CPoint &ptTrail)
{
	CRect rcRgn = m_rc;
	//if (!m_bUp)
	//	rcRgn.OffsetRect(0, TRAIL_SIZE);

	CRgn rgnRect;
	rgnRect.CreateRoundRectRgn(rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom, 11, 11);

	CPoint ptTri[3];

	if (m_bUp)
	{
		if (m_bRight)
		{
			ptTri[0].x = rcRgn.left + TRAIL_MARGIN;
			ptTri[0].y = rcRgn.bottom - 1;

			ptTri[1].x = rcRgn.left + TRAIL_MARGIN;
			ptTri[1].y = rcRgn.bottom + TRAIL_SIZE;

			ptTri[2].x = rcRgn.left + TRAIL_MARGIN + TRAIL_SIZE + 1;
			ptTri[2].y = rcRgn.bottom - 1 - 1;
		}
		else
		{
			ptTri[0].x = rcRgn.right - TRAIL_MARGIN;
			ptTri[0].y = rcRgn.bottom - 1;

			ptTri[1].x = rcRgn.right - TRAIL_MARGIN;
			ptTri[1].y = rcRgn.bottom + TRAIL_SIZE;

			ptTri[2].x = rcRgn.right - TRAIL_MARGIN - TRAIL_SIZE - 1;
			ptTri[2].y = rcRgn.bottom - 1 - 1;
		}
	}
	else
	{
		if (m_bRight)
		{
			ptTri[0].x = rcRgn.left + TRAIL_SIZE;
			ptTri[0].y = rcRgn.top + 1;

			ptTri[1].x = rcRgn.left + TRAIL_SIZE;
			ptTri[1].y = rcRgn.top - TRAIL_SIZE;

			ptTri[2].x = rcRgn.left + TRAIL_SIZE + TRAIL_SIZE;
			ptTri[2].y = rcRgn.top + 1;
		}
		else
		{
			ptTri[0].x = rcRgn.right - TRAIL_SIZE;
			ptTri[0].y = rcRgn.top + 1;

			ptTri[1].x = rcRgn.right - TRAIL_SIZE;
			ptTri[1].y = rcRgn.top - TRAIL_SIZE;

			ptTri[2].x = rcRgn.right - TRAIL_SIZE - TRAIL_SIZE;
			ptTri[2].y = rcRgn.top + 1;
		}
	}

	CRgn rgnTri;
	rgnTri.CreatePolygonRgn(ptTri, 3, ALTERNATE);

	m_rgn.DeleteObject();
	m_rgn.CreateRectRgn(0, 0, 0, 0);
	m_rgn.CombineRgn(&rgnRect, &rgnTri, RGN_OR);

	rcBound = m_rc;
	if (m_bUp)
		rcBound.bottom += TRAIL_SIZE;
	else
		rcBound.top -= TRAIL_SIZE;

	ptTrail = ptTri[1];

	return TRUE;
}

void CBalloonToolTip::GetLinkRect(CRect &rcLink)
{
	if (!m_strLink.IsEmpty())
	{
		CClientDC dc(this);
		CFont *pOldFont = dc.SelectObject(&m_fntText);
		CSize size = dc.GetTextExtent(m_strLinkText);
		dc.SelectObject(pOldFont);
		rcLink.SetRect(m_rc.left + TEXT_MARGIN, m_rc.bottom - TEXT_MARGIN - size.cy, m_rc.left + TEXT_MARGIN + size.cx, m_rc.bottom - TEXT_MARGIN / 2);
	}
	else
	{
		rcLink.SetRectEmpty();
	}
}

void CBalloonToolTip::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	HRGN hRgn = CreateRectRgn(0,0,0,0);
	if (GetWindowRgn(hRgn) == ERROR)
		return;
	CRgn rgn;
	rgn.Attach(hRgn);

	CBrush br1, br2;
	br1.CreateSysColorBrush(COLOR_INFOBK);
	br2.CreateSysColorBrush(COLOR_INFOTEXT);

	dc.FillRgn(&rgn, &br1);
	dc.FrameRgn(&rgn, &br2, 1, 1);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor(COLOR_INFOTEXT));

	CRect rc = m_rc;
	//if (!m_bUp)
	//	rc.OffsetRect(0, TRAIL_SIZE);

	UINT nFormat = m_bSingleLine ? DT_SINGLELINE | DT_VCENTER : DT_WORDBREAK;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_BALLOONTOOLTIP);
	::DrawTransparent(&dc, rc.left + TEXT_MARGIN, rc.top + TEXT_MARGIN, ICON_W, ICON_H, &bmp, 0, 0, RGB(255, 0, 255));

	// draw title
	COLORREF crTextOld = dc.SetTextColor(::GetSysColor(COLOR_INFOTEXT));

	CFont *pFontOld = dc.SelectObject(&m_fntTitle);
	CRect rcTitle;
	CSize size = dc.GetTextExtent(m_strTitle);
	rcTitle.SetRect(m_rc.left + TEXT_MARGIN + ICON_W + TEXT_MARGIN, m_rc.top + TEXT_MARGIN, m_rc.right - TEXT_MARGIN, m_rc.top + TEXT_MARGIN + size.cy);
	dc.DrawText(m_strTitle, &rcTitle, DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);

	// draw link
	dc.SelectObject(&m_fntText);
	if (!m_strLink.IsEmpty())
	{
		dc.SetTextColor(m_bHoverLink ? RGB(255, 0, 0) : RGB(0, 0, 255));
		CRect rcLink;
		GetLinkRect(rcLink);
		dc.DrawText(m_strLinkText, &rcLink, DT_SINGLELINE | DT_LEFT);

		dc.SetTextColor(::GetSysColor(COLOR_INFOTEXT));
	}

	// draw body text
	CRect rcText(rc.left + TEXT_MARGIN, rc.top + TEXT_MARGIN + ICON_H + TEXT_MARGIN, rc.right - TEXT_MARGIN, rc.bottom - TEXT_MARGIN);
	dc.DrawText(m_strText, &rcText, nFormat);

	dc.SelectObject(pFontOld);
	dc.SetTextColor(crTextOld);

	// Do not call CWnd::OnPaint() for painting messages
}

void CBalloonToolTip::RelayEvent(MSG *pMsg)
{
	if (!::IsWindow(m_hWnd) || !IsWindowVisible())
		return;

	CRect rcLink;
	GetLinkRect(rcLink);

	switch( pMsg->message ) 
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
			ShowWindow(SW_HIDE);
			break;

		case WM_LBUTTONDOWN:
		{
			ShowWindow(SW_HIDE);

			if (!m_strLink.IsEmpty())
			{
				CPoint point(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
				if (rcLink.PtInRect(point))
					ShowHelpFile(m_strLink);
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (!m_strLink.IsEmpty())
			{
				BOOL bHoverLink;
				if (pMsg->hwnd == m_hWnd)
				{
					CPoint point(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
					bHoverLink = rcLink.PtInRect(point);
				}
				else
				{
					bHoverLink = FALSE;
				}

				if (bHoverLink != m_bHoverLink)
				{
					m_bHoverLink = bHoverLink;
					InvalidateRect(&rcLink, FALSE);
				}
			}

			break;
		}
	}
}

void CBalloonToolTip::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	DestroyWindow();
}

int CBalloonToolTip::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	return MA_NOACTIVATE;
}

BOOL CBalloonToolTip::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	CPoint point;
	GetCursorPos(&point);

	CRect rcLink;
	GetLinkRect(rcLink);
	ClientToScreen(&rcLink);

	if (rcLink.PtInRect(point))
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_MY_HAND));
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CBalloonToolTip::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here

	if (lpwndpos->flags & SWP_SHOWWINDOW)
	{
		if (m_lpOldWindowProc == NULL)
		{
			CWnd *pWnd = AfxGetMainWnd();
			ASSERT(pWnd);
			if (pWnd)
			{
				m_lpOldWindowProc = (WNDPROC)::GetWindowLong(pWnd->m_hWnd, GWL_WNDPROC);
				ASSERT(m_lpOldWindowProc);
				if (m_lpOldWindowProc)
					SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (long)ToolTipWindowProc);
			}
		}
	}
	else if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		if (m_lpOldWindowProc)
		{
			CWnd *pWnd = AfxGetMainWnd();
			ASSERT(pWnd);
			if (pWnd)
				SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (long)m_lpOldWindowProc);
			m_lpOldWindowProc = NULL;
		}
	}
}
