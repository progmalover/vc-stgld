// UndoListBox.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "UndoListBox.h"
#include "MemDC.h"

#include "DlgUndoRedo.h"

#include "Command.h"
#include "my_app.h"
#include "Global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CUndoListBox

IMPLEMENT_DYNAMIC(CUndoListBox, CListBox)
CUndoListBox::CUndoListBox()
{
}

CUndoListBox::~CUndoListBox()
{
}


BEGIN_MESSAGE_MAP(CUndoListBox, CListBox)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// CUndoListBox message handlers

void CUndoListBox::LoadList(BOOL bUndo)
{
	SetRedraw(FALSE);
	ResetContent();

	m_nSelIndex = -1;

	CString strDesc;
	TCommands &cmds = my_app.Commands();
	if (bUndo)
	{
		//for (int i = 1; i <= pManager->GetPos(); i++)
		int index = 0;
		for (TCommands::CmdIterator it = cmds.Last(); it != cmds.End(); it--)
		{
			::GetCommandDesc(cmds.GetCommandDescID(it), strDesc);
			InsertString(index++, strDesc);
		}
	}
	else
	{
		//int index = 0;
		//for (int i = pManager->GetPos() + 1; i < pManager->GetSize(); i++)
		//	InsertString(index++, pManager->GetDescription(i, FALSE, bUndo));
		int index = 0;
		TCommands::CmdIterator it = ++cmds.Last();
		for (; it != cmds.End(); it++)
		{
			::GetCommandDesc(cmds.GetCommandDescID(it), strDesc);
			InsertString(index++, strDesc);
		}
	}

	SetRedraw(TRUE);

	SetCurSel(0);
	OnSelChange();
}

BOOL CUndoListBox::TrackMessage()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
			case WM_KEYDOWN:
			{
				if (msg.wParam == VK_ESCAPE)
				{
					GetParent()->ShowWindow(SW_HIDE);
					return FALSE;
				}
				break;
			}
			
			case WM_SYSKEYDOWN:
			{
				GetParent()->ShowWindow(SW_HIDE);
				return FALSE;
			}

			case WM_HIDELISTBOX:
			{
				if (GetFocus() != this)
				{
					GetParent()->ShowWindow(SW_HIDE);
					return FALSE;
				}
				break;
			}
			
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_NCLBUTTONDOWN:
			case WM_NCMBUTTONDOWN:
			case WM_NCRBUTTONDOWN:
			{
				if (!PtInWindow(msg.pt))
				{
					GetParent()->ShowWindow(SW_HIDE);
					return FALSE;
				}
				break;
			}

			case WM_MOUSEMOVE:
			{
				if (PtInWindow(msg.pt))
				{
					if (msg.wParam & MK_LBUTTON)
						if (GetCapture() != this)
							SendMessage(WM_LBUTTONDOWN, msg.wParam, msg.lParam);
				}
				else
				{
					if (msg.hwnd != m_hWnd)
						continue;
				}
			
				break;
			}

			case WM_LBUTTONUP:
			{
				if (GetCapture() == this)
				{
					GetParent()->ShowWindow(SW_HIDE);
					return TRUE;
				}

				if (PtInWindow(msg.pt))
				{
					GetParent()->ShowWindow(SW_HIDE);
					return TRUE;
				}
				break;
			}
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return FALSE;
}

int CUndoListBox::ItemFromPointExact(CPoint &point)
{
	BOOL bOutSide;
	int index = ItemFromPoint(point, bOutSide);
	if (index >= 0)
	{
		// Is cursor really in the item[index]?
		CRect rcItem;
		GetItemRect(index, &rcItem);
		if (!PtInRect(&rcItem, point))
			index = -1;
	}

	return index;
}

void CUndoListBox::DrawItem(LPDRAWITEMSTRUCT lp)
{
	// TODO:  Add your code to draw the specified item

	return;
}

void CUndoListBox::DrawItem(CDC *pDC, int index, CRect &rcItem)
{
	//TRACE1("DrawItem: %d\n", index);

	BOOL bSelected = (index <= GetCaretIndex());

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

	CString strText;
	GetText(index, strText);

	CFont *pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	CFont *pFontOld = pDC->SelectObject(pFont);
	pDC->DrawText(strText, rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);
	pDC->SelectObject(pFontOld);
}

void CUndoListBox::OnSelChange()
{
	int index = GetCaretIndex();
	if (index == m_nSelIndex)
		return;

	m_nSelIndex = index;
	
	TRACE2("index = %d, %d\n", index, time(NULL));

	Invalidate(FALSE);
	UpdateWindow();

	((CDlgUndo *)GetParent())->UpdateStatus(index);
}

void CUndoListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CListBox::OnLButtonDown(nFlags, point);
	OnSelChange();
}

void CUndoListBox::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x + rand(), point.y));
}

void CUndoListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetRedraw(FALSE);
	CListBox::OnMouseMove(nFlags, point);
	SetRedraw(TRUE);
	RedrawWindow(0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
	OnSelChange();
}

void CUndoListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	TRACE0("OnVScroll\n");

	//SetRedraw(FALSE);
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
	//SetRedraw(TRUE);
	RedrawWindow(0, 0, /*RDW_FRAME |*/ RDW_INVALIDATE | RDW_UPDATENOW);
	//OnSelChange();
}

void CUndoListBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CListBox::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);

	CRgn rgn;
	rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	dc.SelectClipRgn(&rgn);

	CGlandaMemDC dcMem(&dc);

	dcMem.FillSolidRect(&rc, ::GetSysColor(COLOR_WINDOW));

	int index = GetCaretIndex();
	if (index >= 0)
	{
		CRect rcSel;
		GetItemRect(index, &rcSel);

		if (rcSel.bottom > rc.top)
		{
			rcSel.top = rc.top;
			if (IsRectVisible(rc, rcSel))
				dcMem.FillSolidRect(&rcSel, ::GetSysColor(COLOR_HIGHLIGHT));
		}
	}

	BOOL bVisible = FALSE;
	for (int i = 0; i < GetCount(); i++)
	{
		CRect rcItem;
		GetItemRect(i, &rcItem);

		if (IsRectVisible(rc, rcItem))
		{
			bVisible = TRUE;
			DrawItem(&dcMem, i, rcItem);
		}
		else
		{
			if (bVisible)
				break;
		}
	}
}

BOOL CUndoListBox::PtInWindow(POINT point)
{
	CRect rc;
	GetWindowRect(&rc);
	return (rc.PtInRect(point));
}

BOOL CUndoListBox::IsRectVisible(CRect &rcClient, CRect &rcItem)
{
	CRect rcTemp;
	rcTemp.IntersectRect(&rcClient, &rcItem);
	return !rcTemp.IsRectEmpty();
}

BOOL CUndoListBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	//return CListBox::OnEraseBkgnd(pDC);
}
