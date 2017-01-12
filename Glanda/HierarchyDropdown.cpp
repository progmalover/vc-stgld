// HierarchyComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "HierarchyDropdown.h"
#include "SysParas.h"
#include "VisualStylesXP.h"

// CHierarchyDropdown

IMPLEMENT_DYNAMIC(CHierarchyDropdown, CWnd)

CHierarchyDropdown::CHierarchyDropdown()
{
	_cur_sel = -1;
	_cx_drop = -1;
	_cy_drop = -1;
}

CHierarchyDropdown::~CHierarchyDropdown()
{
}

// public interface
void CHierarchyDropdown::SetCurSel(int item)
{
	if (item < -1 || item > _popup.GetCount())
	{
		item = -1;
	}
	if (_cur_sel != item)
	{
		_cur_sel = item;
		Invalidate(FALSE);
		GetParent()->SendMessage(WM_COMMAND
			, MAKEWPARAM(GetDlgCtrlID(), HN_SEL_CHANGED), (LPARAM)m_hWnd);
	}
}

// private implement
CRect CHierarchyDropdown::GetDropButtonRect() const
{
	CRect rect;
	GetClientRect(&rect);
	int width = GetSystemMetrics(SM_CXVSCROLL);
	return CRect(rect.right - width,
		rect.top, rect.right, rect.bottom);
}

CSize CHierarchyDropdown::GetDefaultDropSize() const
{
	ASSERT(m_hWnd != NULL);
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width() + 4 * SysParas.cx_border;
	int cy = 12 * _popup.GetItemHeight() + 2 * SysParas.cy_border;
	return CSize(cx, cy);
}

BOOL CHierarchyDropdown::IsDropdown() const
{
	return _popup.m_hWnd != NULL;
}

void CHierarchyDropdown::Popup()
{
	CRect rect;
	GetWindowRect(&rect);
	int x = rect.left;
	int y = rect.bottom;
	int cx = _cx_drop;
	int cy = _cy_drop;
	if (cx < 0 || cy < 0)
	{
		CSize sz_drop = GetDefaultDropSize();
		cx = sz_drop.cx;
		cy = sz_drop.cy;
	}
	_popup.Popup(this, x, y, cx, cy);
	_popup.SetCurSel(_cur_sel);
}

BEGIN_MESSAGE_MAP(CHierarchyDropdown, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(CHierarchyPopup::MSG_SELCHANGED, OnSelChanged)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

// CHierarchyDropdown message handlers
void CHierarchyDropdown::OnPaint()
{
	CRect rect_client;
	GetClientRect(&rect_client);
	CPaintDC dc(this);
	CDC dc_mem;
	if (!dc_mem.CreateCompatibleDC(&dc))
	{
		return;
	}
	CBitmap	bitmap;
	if (!bitmap.CreateCompatibleBitmap(&dc, rect_client.Width(), rect_client.Height()))
	{
		return;
	}
	CBitmap *old_bitmap = dc_mem.SelectObject(&bitmap);
	if (old_bitmap == NULL)
	{
		return;
	}
	dc_mem.FillSolidRect(rect_client, SysParas.cl_window);
	CRect rect_drop = GetDropButtonRect();
	BOOL xpStyle = FALSE;
	if (g_xpStyle.IsAppThemed())
	{
		HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"COMBOBOX");
		if (hTheme)
		{
			HRESULT hr = g_xpStyle.DrawThemeBackground(hTheme, dc_mem, CP_DROPDOWNBUTTON, CBXS_NORMAL, &rect_drop, NULL);
			xpStyle = SUCCEEDED(hr);
			g_xpStyle.CloseThemeData(hTheme);			
		}
	}
	if (!xpStyle)
	{
		dc_mem.DrawFrameControl(rect_drop, DFC_SCROLL, DFCS_SCROLLDOWN);
	}	
	CRect rect_item = rect_client;
	rect_item.right = rect_drop.left;
	if (rect_item.right > rect_item.left)
	{
		if (_cur_sel > -1 && _cur_sel < _popup.GetCount())
		{
			CRgn clip;
			clip.CreateRectRgn(rect_item.left
				, rect_item.top, rect_item.right
				, rect_item.bottom);
			dc_mem.SelectClipRgn(&clip, RGN_COPY);
			UINT state = 0;
			if (GetFocus() == this)
			{
				state = CHierarchyPopup::SELECT;
			}
			CFont *old_font = dc_mem.SelectObject(SysParas.font_default);				
			_popup.DrawItem(&dc_mem, _cur_sel
				, rect_item, state, FALSE);
			dc_mem.SelectObject(old_font);
			dc_mem.SelectClipRgn(NULL, RGN_COPY);
		}
		else
		{
			if (GetFocus() == this)
			{				
				rect_item.DeflateRect(1, 1, 1, 1);
				dc_mem.FillSolidRect(rect_item, SysParas.cl_hilight);
			}			
		}
	}
	dc.BitBlt(0, 0, rect_client.Width(), rect_client.Height()
		, &dc_mem, 0, 0, SRCCOPY);
	dc_mem.SelectObject(old_bitmap);
}

void CHierarchyDropdown::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if (_popup.m_hWnd == NULL)
	{
		Popup();
	}
	else
	{
		_popup.Close(CHierarchyPopup::CANCEL);
	}
}

LRESULT CHierarchyDropdown::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	SetCurSel((int)lParam);
	
	return 0;
}

void CHierarchyDropdown::OnKillFocus(CWnd *pNewWnd)
{
	Invalidate(FALSE);	
	CWnd::OnKillFocus(pNewWnd);
}

void CHierarchyDropdown::OnSetFocus(CWnd *pOldWnd)
{
	Invalidate(FALSE);	
	CWnd::OnSetFocus(pOldWnd);
}

void CHierarchyDropdown::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!IsDropdown())
	{
		if (nChar == VK_SPACE)
		{
			Popup();
		}
		else
		{
			int next = _cur_sel;
			if (nChar == VK_DOWN)
			{
				next = _popup.NextItem(_cur_sel);
			}
			else if (nChar == VK_UP)
			{
				next = _popup.PrevItem(_cur_sel);
			}
			if (next != _cur_sel && next != -1)
			{
				SetCurSel(next);				
			}
		}
	}
}

BOOL CHierarchyDropdown::PreTranslateMessage(MSG* pMsg)
{
	BOOL ret = FALSE;
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN && IsDropdown()) // prevent close dialog
		{			
			ret = TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE && IsDropdown()) // prevent close dialog
		{
			ret = TRUE;
		}
		else if (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN) // prevent switch tab order
		{			
			DispatchMessage(pMsg);
			ret = TRUE;
		}
	}
	return ret;
}

void CHierarchyDropdown::OnNcPaint()
{
	Default();

	DWORD dwExStyle = GetExStyle();
	if ((dwExStyle & WS_EX_CLIENTEDGE) || (dwExStyle & WS_EX_STATICEDGE))
	{
		if (g_xpStyle.IsAppThemed())
		{
			HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"COMBOBOX");
			if (hTheme)
			{
				COLORREF crBorder;

				HRESULT hr = g_xpStyle.GetThemeColor(hTheme, 0, 0, TMT_BORDERCOLOR, &crBorder);
				g_xpStyle.CloseThemeData(hTheme);

				if (SUCCEEDED(hr))
				{
					CDC *pDC = GetWindowDC();
					if (pDC)
					{
						CRect rc;
						GetWindowRect(&rc);
						rc.OffsetRect(-rc.left, -rc.top);

						CBrush brBorder(crBorder);
						pDC->FrameRect(&rc, &brBorder);

						if (dwExStyle & WS_EX_CLIENTEDGE)
						{
							rc.InflateRect(-1, -1);

							CBrush brWindow(::GetSysColor(COLOR_WINDOW));
							pDC->FrameRect(&rc, &brWindow);
						}

						ReleaseDC(pDC);
					}
				}
			}
		}
	}
}
