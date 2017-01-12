// ToolButtonPopup.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ToolButtonPopup.h"

#include "MemDC.h"
#include "Graphics.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CToolButtonPopup

IMPLEMENT_DYNCREATE(CToolButtonPopup, CWnd)

CToolButtonPopup::CToolButtonPopup()
{
	m_nCurrent = -1;
	m_nHover = -1;
	m_bLButtonDown = FALSE;
	m_nMaxWidth = 0;
}

CToolButtonPopup::~CToolButtonPopup()
{
}


BEGIN_MESSAGE_MAP(CToolButtonPopup, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CToolButtonPopup message handlers

void CToolButtonPopup::SetCurrentTool(int index)
{
	m_nCurrent = index;
}

int CToolButtonPopup::GetCurrentTool()
{
	return m_nCurrent;
}


void CToolButtonPopup::SetHover(int index)
{
	if (m_nHover != index)
	{
		m_nHover = index;
		Invalidate();
	}
}

void CToolButtonPopup::AddTool(UINT nID, UINT nIDBitmap, LPCTSTR lpszText)
{
	CSubTool tool(nID, nIDBitmap, lpszText);
	m_list.push_back(tool);
}

void CToolButtonPopup::RemoveAll()
{
	m_list.clear();
}

void CToolButtonPopup::GetItemRect(int index, CRect &rc)
{
	rc.SetRectEmpty();

	int nItemHeight = GetItemHeight();

	int y = 0;
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CSubTool tool = m_list[i];

		BOOL bSep = (tool.m_nID == 0);

		if (i == index)
		{
			rc.SetRect(0, y, m_nMaxWidth, y + (bSep ? nItemHeight / 2: nItemHeight));
			return;
		}

		if (bSep)
			y += nItemHeight / 2;
		else
			y += nItemHeight;
	}
}

int CToolButtonPopup::HitTest(CPoint point, BOOL &bSep)
{
	int nItemHeight = GetItemHeight();

	CRect rc;
	int y = 0;
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CSubTool tool = m_list[i];

		bSep = (tool.m_nID == 0);

		rc.SetRect(0, y, m_nMaxWidth, y + (bSep ? nItemHeight / 2: nItemHeight));
		if (rc.PtInRect(point))
			return i;

		if (bSep)
			y += nItemHeight / 2;
		else
			y += nItemHeight;
	}

	return -1;
}

void CToolButtonPopup::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	int nItemHeight = GetItemHeight();

	CGlandaMemDC dcMem(&dc);

	CRect rc;
	GetClientRect(&rc);

	dcMem.FillRect(&rc, CBrush::FromHandle(GetSysColorBrush(COLOR_3DFACE)));

	CBitmap bmpCheck;
	bmpCheck.LoadBitmap(IDB_MENU_CHECK);

	CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont *pOldFont = dcMem.SelectObject(pFont);
	dcMem.SetBkMode(TRANSPARENT);

	CRect rcItem;
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CSubTool tool = m_list[i];

		GetItemRect(i, rcItem);

		if (tool.m_nID != 0)
		{
			if (i == m_nHover)
			{
				CRect rcBack = rcItem;
				rcBack.InflateRect(-1, 0);
				dcMem.FillRect(&rcBack, CBrush::FromHandle(GetSysColorBrush(COLOR_3DHIGHLIGHT)));
			}

			BITMAP bm;
			if (i == m_nCurrent)
			{
				bmpCheck.GetBitmap(&bm);
				::DrawTransparent(&dcMem, rcItem.left + (nItemHeight - bm.bmWidth / 2) / 2, rcItem.top + (nItemHeight - bm.bmHeight) / 2, bm.bmWidth / 2, bm.bmHeight, &bmpCheck, 0, 0, RGB(255, 0, 255));
			}

			rcItem.left += nItemHeight;

			CBitmap bmp;
			bmp.LoadBitmap(tool.m_nIDBitmap);
			bmp.GetBitmap(&bm);
			::DrawTransparent(&dcMem, rcItem.left + (nItemHeight - bm.bmWidth / 2) / 2, rcItem.top + (nItemHeight - bm.bmHeight) / 2, bm.bmWidth / 2, bm.bmHeight, &bmp, i == m_nHover ? bm.bmWidth / 2 : 0, 0, RGB(255, 0, 255));

			rcItem.left += nItemHeight + 8;

			dcMem.DrawText(tool.m_strText, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
		}
		else
		{
			rcItem.left += 3;
			rcItem.top += nItemHeight / 4 - 1;
			rcItem.right -= 3;
			rcItem.bottom = rcItem.top + 2;
			dcMem.Draw3dRect(&rcItem, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DLIGHT));
		}
	}

	dcMem.SelectObject(pOldFont);
}

void CToolButtonPopup::RecalcWindowSize(CSize &size)
{
	int nItemHeight = GetItemHeight();

	CClientDC dc(this);

	size.cx = 0;
	size.cy = 0;
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CSubTool tool = m_list[i];
		if (tool.m_nID != 0)
		{
			CSize sizeText = dc.GetTextExtent(tool.m_strText);
			size.cx = max(size.cx, nItemHeight * 2 + sizeText.cx + nItemHeight);
			size.cy += nItemHeight;
		}
		else
		{
			size.cy += nItemHeight / 2;
		}
	}

	m_nMaxWidth = size.cx;

	int cxFrame = ::GetSystemMetrics(SM_CXFIXEDFRAME);
	int cyFrame = ::GetSystemMetrics(SM_CYFIXEDFRAME);

	size.cx += cxFrame * 2;
	size.cy += cyFrame * 2 + 1;
}

BOOL CToolButtonPopup::Create()
{
	LPCTSTR lpszClass = AfxRegisterWndClass(0, NULL, ::GetSysColorBrush(COLOR_3DFACE), NULL);

	if (!CWnd::Create(lpszClass, 
		"", 
		WS_CHILD | WS_DLGFRAME, 
		CRect(0, 0, 0, 0), 
		GetDesktopWindow(),
		1))
		return FALSE;

	ModifyStyleEx(0, WS_EX_TOOLWINDOW, SWP_FRAMECHANGED);
	
	return TRUE;
}

BOOL CToolButtonPopup::Track(CPoint point)
{
	CSize size;
	RecalcWindowSize(size);
	SetWindowPos(&wndTopMost, point.x, point.y, size.cx, size.cy, SWP_SHOWWINDOW | SWP_NOACTIVATE);

	CRect rc;
	GetClientRect(&rc);

	SetCapture();
	SetFocus();

	TRACE0("Start Tracking.\n");

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
			case WM_LBUTTONDOWN:
			{
				int x = (int)(short)LOWORD(msg.lParam);
				int y = (int)(short)HIWORD(msg.lParam);
				CPoint pt(x, y);
				if (rc.PtInRect(pt))
				{
					m_bLButtonDown = TRUE;
					continue;
				}

				ReleaseCapture();

				ClientToScreen(&pt);
				HWND hWnd = ::WindowFromPoint(pt);
				if (hWnd)
				{
					::ScreenToClient(hWnd, &pt);
					::PostMessage(hWnd, WM_LBUTTONDOWN, msg.wParam, MAKELPARAM(pt.x, pt.y));
				}

				goto _exit;
			}

			case WM_MOUSEMOVE:
			{
				int x = (int)(short)LOWORD(msg.lParam);
				int y = (int)(short)HIWORD(msg.lParam);

				CPoint pt(x, y);
				BOOL bSep;
				int index = HitTest(pt, bSep);		

				SetHover(bSep ? -1 : index);

				continue;
			}

			case WM_LBUTTONUP:
			{
				int x = (int)(short)LOWORD(msg.lParam);
				int y = (int)(short)HIWORD(msg.lParam);

				CPoint pt(x, y);
				BOOL bSep;
				int index = HitTest(pt, bSep);
				if (index >= 0 && !bSep)
				{
					SetCurrentTool(index);
					ReleaseCapture();
					return TRUE;;
				}
				continue;
			}

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
				continue;

			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				ReleaseCapture();
				break;

			default:
				break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (GetCapture() != this)
			break;
	}

_exit:
	TRACE0("End Tracking.\n");
	return FALSE;
}

BOOL CToolButtonPopup::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

int CToolButtonPopup::FindTool(UINT nID)
{
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CSubTool tool = m_list[i];
		if (tool.m_nID == nID)
			return i;
	}
	ASSERT(FALSE);
	return -1;
}

int CToolButtonPopup::GetItemHeight()
{
	return ::GetSystemMetrics(SM_CYMENU);
}

void CToolButtonPopup::GetItemText(int index, CString &strText)
{
	CSubTool tool = m_list[index];
	strText = tool.m_strText;
}
