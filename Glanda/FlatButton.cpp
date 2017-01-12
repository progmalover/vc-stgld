// FlatButton.cpp : implementation file
//

#include "stdafx.h"
#include "FlatButton.h"
#include "Graphics.h"

#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatButton

CFlatButton::CFlatButton()
{
	m_dwState = STATE_FLAT;
	m_dwStyle = FBS_FLAT | FBS_CENTERALIGNTEXT;

	m_bTimerSet = FALSE;
	m_bHover = FALSE;
	m_bLButtonDown = FALSE;
	m_bChecked = FALSE;

	m_nFaceColorMode = FACE_COLOR_MODE_AUTO;
	m_crFace = CLR_INVALID;
}

CFlatButton::~CFlatButton()
{
}


BEGIN_MESSAGE_MAP(CFlatButton, CButton)
	//{{AFX_MSG_MAP(CFlatButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlatButton message handlers

void CFlatButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	// Remove WS_TABSTOP && BS_PUSHBUTTON - BS_AUTORADIOBUTTON
	ModifyStyle(WS_TABSTOP | 0x0000001f, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}

void CFlatButton::SetFBState(DWORD dwNewState)
{
	if(dwNewState != m_dwState)
	{
		m_dwState = dwNewState;
		Invalidate();
	}
}

DWORD CFlatButton::GetFBState()
{
	return m_dwState;
}

void CFlatButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//TRACE0("\r\n\r\nOnLButtonDown()\r\n");

	m_bLButtonDown = TRUE;
	SetCapture();
	SetFBState(STATE_DOWN);

	//CButton::OnLButtonDown(nFlags, point);
}

void CFlatButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//TRACE0("\r\n\r\nOnMouseMove()\r\n");

	if (!m_bTimerSet)
	{
		SetTimer(1, 1, NULL);
		m_bTimerSet = TRUE;
	}

	//CRect rc;
	//GetWindowRect(&rc);
	//rc.OffsetRect(-rc.left, -rc.top);
	
	ClientToScreen(&point);
	HWND hWnd = ::WindowFromPoint(point);

	if (m_bLButtonDown)
	{
		if (hWnd == m_hWnd)
		{
			SetFBState(STATE_DOWN);
			SetHover(TRUE);
		}
		else
		{
			SetFBState(m_bChecked ? STATE_DOWN : STATE_FLAT);
			SetHover(FALSE);
		}
	}
	else	// !m_bLButtonDown
	{
		if (hWnd == m_hWnd)
		{
			SetFBState(m_bChecked ? STATE_DOWN : STATE_UP);
			SetHover(TRUE);
		}
		else
		{
			SetFBState(m_bChecked ? STATE_DOWN : STATE_FLAT);
			SetHover(FALSE);
		}
	}

	//CButton::OnMouseMove(nFlags, point);
}

void CFlatButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bLButtonDown)
	{
		// ReleaseCapture() will set m_bLButtonDown to FALSE
		ReleaseCapture();

		//CRect rc;
		//GetWindowRect(&rc);
		//rc.OffsetRect(-rc.left, -rc.top);

		ClientToScreen(&point);
		HWND hWnd = ::WindowFromPoint(point);
		if (hWnd == m_hWnd)
		{
			if (m_dwStyle & FBS_CHECK)
			{
				m_bChecked = !m_bChecked;
				SetCheck(m_bChecked ? 1 : 0);
				Invalidate();
			}

			SetFBState(m_bChecked ? STATE_DOWN : STATE_FLAT);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
	}

	//CButton::OnLButtonUp(nFlags, point);
}

void CFlatButton::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (nIDEvent == 1)
	{
		ASSERT(m_bTimerSet);

		if ((m_dwStyle & FBS_FLAT) || m_dwState == STATE_DOWN || (m_dwStyle & FBS_FLAT) == 0)
		{
			CPoint point;
			GetCursorPos(&point);
			/*
			CRect rc, rcParent;
			GetWindowRect(&rc);
			GetParent()->GetWindowRect(&rcParent);
			*/
			HWND hWnd = ::WindowFromPoint(point);
			if (hWnd != m_hWnd)
			{
				SetFBState(m_bChecked ? STATE_DOWN : STATE_FLAT);
				KillTimer(1);
				m_bTimerSet = FALSE;
				SetHover(FALSE);
			}
		}
	}

	CButton::OnTimer(nIDEvent);
}

void CFlatButton::OnDestroy() 
{
	CButton::OnDestroy();
	
	// TODO: Add your message handler code here
	
	if (m_bTimerSet)
	{
		KillTimer(1);
		m_bTimerSet = FALSE;
	}
}

void CFlatButton::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	if (m_bLButtonDown)
	{
		m_bLButtonDown = FALSE;
	}

	CButton::OnCaptureChanged(pWnd);
}

void CFlatButton::DrawItem(LPDRAWITEMSTRUCT lp) 
{
	// TODO: Add your code to draw the specified item

	const int BORDER = 3;
	const int MARGIN = 3;

	HTHEME hTheme = NULL;
	if (!(m_dwStyle & FBS_NOXPSTYLE) && g_xpStyle.IsAppThemed())
		hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"TOOLBAR");

	CDC *pDC=CDC::FromHandle(lp->hDC);
	CRect rc = lp->rcItem;

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

	CBitmap *pOldBmp = dcMem.SelectObject(&bmp);
	CFont *pOldFont = dcMem.SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
	
	dcMem.SetBkMode(TRANSPARENT);

	int x = 0, y = 0;
	CString strText;
	GetWindowText(strText);
	CRect rcText = rc;
	rcText.InflateRect(-BORDER, -BORDER);

	CSize sizeStuff = GetStuffSize();
	BOOL bStuff = (sizeStuff.cx > 0 && sizeStuff.cy > 0);
	if (bStuff)
	{
		if ((m_dwStyle & FBS_RIGHTTEXT) || (m_dwStyle & FBS_DROPDOWN))
		{
			x = BORDER + MARGIN;
			y = (rc.Height() - sizeStuff.cy)/2;
			
			rcText.left += MARGIN + sizeStuff.cx + MARGIN;
			if (m_dwStyle & FBS_DROPDOWN)
				rcText.right -= (MARGIN + 5);
		}
		else
		{
			x = (rc.Width() - sizeStuff.cx) / 2;
			y = (rc.Height() - sizeStuff.cy) / 2;
		}
	}
	else
	{
		if (m_dwStyle & FBS_TRACKBORDER)
		{
			rcText.InflateRect(-1, -1);
		}
		if (m_dwStyle & FBS_THICKBORDER)
		{
			rcText.InflateRect(-1, -1);
		}
	}


	// draw background and borders
	switch(m_dwState)
	{
	case STATE_FLAT:
		if (hTheme)
		{
			if (m_dwStyle & FBS_FLAT)
			{
				g_xpStyle.DrawThemeParentBackground(m_hWnd, dcMem, &rc);

				if (!(m_dwStyle & FBS_NOBORDER))
					g_xpStyle.DrawThemeBackground(hTheme, dcMem, TP_BUTTON, TS_NORMAL, &rc, NULL);
				
				break;
			}
			else
			{
				// Don't break, should handle as STATE_UP
			}
		}
		else
		{
			if (m_dwStyle & FBS_FLAT)
			{
				if (!(m_dwStyle & FBS_NOBORDER))
				{
					if (m_dwStyle & FBS_STATICEDGE)
					{
						dcMem.Draw3dRect(&rc, SHADOW_COLOR, HILIGHT_COLOR);
						rc.InflateRect(-1, -1);
					}
				}

				//dcMem.FillSolidRect(&rc, FACE_COLOR);
				DrawBackground(&dcMem, rc);
	
				break;
			}
			else
			{
				// Don't break, should handle as STATE_UP
			}
		}

	case STATE_UP:
		if (hTheme)
		{
			g_xpStyle.DrawThemeParentBackground(m_hWnd, dcMem, &rc);

			if (!(m_dwStyle & FBS_NOBORDER))
				g_xpStyle.DrawThemeBackground(hTheme, dcMem, TP_BUTTON, TS_HOT, &rc, NULL);
		}
		else
		{
			if (!(m_dwStyle & FBS_NOBORDER))
			{
				if (m_dwStyle & FBS_STATICEDGE)
				{
					dcMem.Draw3dRect(&rc, SHADOW_COLOR, HILIGHT_COLOR);
					rc.InflateRect(-1, -1);
				}
				if (m_dwStyle & FBS_TRACKBORDER)
				{
					dcMem.Draw3dRect(&rc, DKSHADOW_COLOR, DKSHADOW_COLOR);
					rc.InflateRect(-1, -1);
				}
				if (m_dwStyle & FBS_THICKBORDER)
				{
					dcMem.Draw3dRect(&rc, HILIGHT_COLOR, DKSHADOW_COLOR);
					rc.InflateRect(-1, -1);
					dcMem.Draw3dRect(&rc, LIGHT_COLOR, SHADOW_COLOR);
					rc.InflateRect(-1, -1);
				}
				else
				{
					dcMem.Draw3dRect(&rc, HILIGHT_COLOR, SHADOW_COLOR);
					rc.InflateRect(-1, -1);
				}
			}

			DrawBackground(&dcMem, rc);
		}

		break;
	
	case STATE_DOWN:
		if (hTheme)
		{
			g_xpStyle.DrawThemeParentBackground(m_hWnd, dcMem, &rc);

			if (!(m_dwStyle & FBS_NOBORDER))
				g_xpStyle.DrawThemeBackground(hTheme, dcMem, TP_BUTTON, m_bHover ? (m_bLButtonDown ? TS_PRESSED : TS_HOTCHECKED) : TS_CHECKED, &rc, NULL);
		}
		else
		{
			if (!(m_dwStyle & FBS_NOBORDER))
			{
				if (m_dwStyle & FBS_STATICEDGE)
				{
					dcMem.Draw3dRect(&rc, SHADOW_COLOR, HILIGHT_COLOR);
					rc.InflateRect(-1, -1);
				}
				if (m_dwStyle & FBS_TRACKBORDER)
				{
					dcMem.Draw3dRect(&rc, DKSHADOW_COLOR, DKSHADOW_COLOR);
					rc.InflateRect(-1, -1);
				}
				if (m_dwStyle & FBS_THICKBORDER)
				{
					dcMem.Draw3dRect(&rc, DKSHADOW_COLOR, HILIGHT_COLOR);
					rc.InflateRect(-1, -1);
					dcMem.Draw3dRect(&rc, SHADOW_COLOR, LIGHT_COLOR);
					rc.InflateRect(-1, -1);
				}
				else
				{
					dcMem.Draw3dRect(&rc, SHADOW_COLOR, HILIGHT_COLOR);
					rc.InflateRect(-1, -1);
				}
			}
			
			DrawBackground(&dcMem, rc);
		}

		x++;
		y++;
		rcText.OffsetRect(1, 1);
		break;
	
	default:
		ASSERT(FALSE);
	}

	// draw bitmap or other stuffs
	if (bStuff)
		DrawStuff(&dcMem, CRect(x, y, x + sizeStuff.cx, y + sizeStuff.cy));

	// draw text
	x += sizeStuff.cx;
	if (m_dwStyle & FBS_RIGHTTEXT)
	{
		int cxText = dcMem.GetTextExtent(strText).cx;

		UINT nFormat = DT_SINGLELINE | DT_VCENTER;
		if (m_dwStyle & FBS_LEFTALIGNTEXT)
		{
			//rcText.left += 5;
			//rcText.right -= 2;
			nFormat |= DT_LEFT;

			//x = rcText.left + cxText;
		}
		else if (m_dwStyle & FBS_CENTERALIGNTEXT)
		{
			//rcText.right -= 2;
			nFormat |= DT_CENTER;

			//x = rcText.left + (rcText.Width() - cxText) / 2 + cxText;
		}
		else
		{
			//rcText.right -= 2;
			nFormat |= DT_RIGHT;

			//x = rcText.right;
		}

		if (!IsWindowEnabled())
		{
			rcText.OffsetRect(1, 1);
			dcMem.SetTextColor(HILIGHT_COLOR);
			dcMem.DrawText(strText, &rcText, nFormat);
			rcText.OffsetRect(-1, -1);
			dcMem.SetTextColor(SHADOW_COLOR);
			dcMem.DrawText(strText, &rcText, nFormat);
		}
		else
		{
			dcMem.SetTextColor(TEXT_COLOR);
			dcMem.DrawText(strText, &rcText, nFormat);
		}
	}

	// draw drop down arrow
	if ((m_dwStyle & FBS_DROPDOWN) || (m_dwStyle & FBS_DROPDOWN2))
	{
		COLORREF crArrow = ::GetSysColor(IsWindowEnabled() ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT);
		CPen penArrow(PS_SOLID, 1, crArrow);

		CPen *pOldPen = dcMem.SelectObject(&penArrow);

		if (m_dwStyle & FBS_DROPDOWN)
		{
			x = rcText.right + MARGIN;
			y = (lp->rcItem.bottom - lp->rcItem.top) / 2 + 1;

			if (m_dwState == STATE_DOWN)
				y++;

			dcMem.MoveTo(x - 2, y - 1);
			dcMem.LineTo(x + 2 + 1, y - 1);
			dcMem.MoveTo(x - 1, y);
			dcMem.LineTo(x + 1 + 1, y);	
			dcMem.SetPixel(x, y + 1, crArrow);
		}
		else if (m_dwStyle & FBS_DROPDOWN2)
		{
			int x1 = lp->rcItem.right - 6;
			int y1 = lp->rcItem.bottom - 4;
			int x2 = lp->rcItem.right - 3;
			int y2 = lp->rcItem.bottom - 7;
			if (m_dwState == STATE_DOWN)
			{
				x1++;
				y1++;
				x2++;
				y2++;
			}

			dcMem.MoveTo(x1, y1);
			dcMem.LineTo(x2, y2);
			dcMem.MoveTo(x1 + 1, y1);
			dcMem.LineTo(x2, y2 + 1);
			dcMem.SetPixel(x1 + 2, y1, crArrow);
		}

		dcMem.SelectObject(pOldPen);
	}

	pDC->BitBlt(
		0, 
		0, 
		lp->rcItem.right - lp->rcItem.left + 1, 
		lp->rcItem.bottom - lp->rcItem.top + 1, 
		&dcMem, 
		0, 
		0, 
		SRCCOPY);

	dcMem.SelectObject(pOldFont);
	dcMem.SelectObject(pOldBmp);

	if (hTheme)
		g_xpStyle.CloseThemeData(hTheme);
}

void CFlatButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));

	//CButton::OnLButtonDblClk(nFlags, point);
}

void CFlatButton::SetFBStyle(DWORD dwStyle)
{
	m_dwStyle = dwStyle;
	Invalidate();
}

DWORD CFlatButton::GetFBStyle()
{
	return m_dwStyle;
}

void CFlatButton::ModifyFBStyle(DWORD dwRemove, DWORD dwAdd)
{
	SetFBStyle(GetFBStyle() & ~dwRemove | dwAdd);
}

void CFlatButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	// TODO: Add your message handler code here
	
	SetFBState(m_bChecked ? STATE_DOWN : STATE_FLAT);
}

void CFlatButton::SetHover(BOOL bHover)
{
	if (m_bHover != bHover)
	{
		m_bHover = bHover;
		CRect rc;
		GetClientRect(&rc);
		if (!(m_dwStyle & FBS_THICKBORDER))
			rc.InflateRect(-1, -1);
		else
			rc.InflateRect(-2, -2);

		InvalidateRect(&rc);
	}
}

void CFlatButton::SetCheck(int nCheck)
{
	if (m_bChecked != (nCheck != BST_UNCHECKED))
	{
		m_bChecked = !m_bChecked;
		SetFBState(m_bChecked ? STATE_DOWN : STATE_FLAT);
	}
}

int CFlatButton::GetCheck()
{
	return m_bChecked ? 1 : 0;
}

CSize CFlatButton::GetStuffSize()
{
	return CSize(0, 0);	
}

void CFlatButton::DrawStuff(CDC *pDC, const CRect &rc)
{

}

void CFlatButton::DrawBackground(CDC *pDC, const CRect &rc)
{
	if (!(m_dwStyle & FBS_TRANSPARENT))
	{
		switch (m_nFaceColorMode)
		{
			case FACE_COLOR_MODE_AUTO:
				pDC->FillSolidRect(&rc, m_bChecked && !m_bHover ? DITHER_COLOR : FACE_COLOR);
				break;
			case FACE_COLOR_MODE_SYSCOLOR:
				pDC->FillSolidRect(&rc, GetSysColor((int)m_crFace));
				break;
			case FACE_COLOR_MODE_RGB:
				pDC->FillSolidRect(&rc, m_crFace);
				break;
			default:
				ASSERT(FALSE);
		}
	}
	else
	{
		CWnd *pParent = GetParent();
		
		CPoint pt(0, 0);
		MapWindowPoints(pParent, &pt, 1);
		pt = pDC->OffsetWindowOrg(pt.x, pt.y);
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rc);
		pDC->SelectClipRgn(&rgn, RGN_COPY);
		LRESULT lResult = pParent->SendMessage(WM_ERASEBKGND, (WPARAM)pDC->m_hDC, 0L);
		pDC->SelectClipRgn(NULL, RGN_COPY);
		pDC->SetWindowOrg(pt.x, pt.y);
	}
}

BOOL CFlatButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
	//return CButton::OnEraseBkgnd(pDC);
}

void CFlatButton::SetFaceColor(int nFaceColorMode, COLORREF color, BOOL bRedraw)
{
	m_nFaceColorMode = nFaceColorMode;
	m_crFace = color;
	if (bRedraw)
	{
		ASSERT(m_hWnd);
		Invalidate();
	}
}
