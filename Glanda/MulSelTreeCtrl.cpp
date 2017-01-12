// MulSelTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MulSelTreeCtrl.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

#define IDT_DELAY_EXPAND		1
#define IDT_DELAY_SCROLL		2
#define IDT_DELAY_EDIT_LABEL	3
#define IDT_CHAR_INPUT			4

#define IsControlDown()			(GetKeyState(VK_CONTROL) & (1 << (sizeof(short int) * 8 - 1)))
#define IsShiftDown()			(GetKeyState(VK_SHIFT) & (1 << (sizeof(short int) * 8 - 1)))

/////////////////////////////////////////////////////////////////////////////
// CMulSelTreeCtrl

CMulSelTreeCtrl::CMulSelTreeCtrl()
{
	m_bLButtonDown = FALSE;
	m_bDragging = FALSE;
	m_nSelectType = typeNone;

	m_hItemCaret = NULL;
	m_hItemDelayExpand = NULL;
	m_hItemDelayScroll = NULL;
	m_hItemDelayEditLabel = NULL;
	m_hItemDrop = NULL;

	m_hItemInserted = NULL;

	m_bEnableDragDrop = TRUE;
	m_bExtDragDrop = TRUE;

	m_bDisableSelChange = FALSE;
}

CMulSelTreeCtrl::~CMulSelTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMulSelTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMulSelTreeCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TVM_INSERTITEM, OnInsertItem)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMulSelTreeCtrl message handlers


void CMulSelTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bDragging)
	{
		ReleaseCapture();
		return;
	}

	CancelDelayEditLabel();

	UINT flags = 0;
	HTREEITEM hItem = HitTest(point, &flags);
	if (hItem)
	{
		switch (flags)
		{
		case TVHT_ONITEMICON:
		case TVHT_ONITEMSTATEICON:
		case TVHT_ONITEMLABEL:
			break;

		default:
			hItem = NULL;
			break;
		}
	}

	if (!hItem || !IsItemSelected(hItem))
		UnselectAllItems();

	if (hItem)
	{
		SetCaretItem(hItem);
		SelectItem(hItem);
	}

	if (GetFocus() != this)
		SetFocus();

	NMHDR nmhdr;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	nmhdr.code = NM_RCLICK;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&nmhdr);

	//CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CMulSelTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CancelDelayEditLabel();

	UINT flags = 0;
	HTREEITEM hItem = HitTest(point, &flags);
	if (hItem)
	{
		switch (flags)
		{
		case TVHT_ONITEMICON:
		case TVHT_ONITEMSTATEICON:
		case TVHT_ONITEMLABEL:
			break;

		case TVHT_ONITEMBUTTON:
			CTreeCtrl::OnLButtonDown(nFlags, point);
			return;

		default:
			hItem = NULL;
			break;
		}
	}

	if (!hItem)
	{
		UnselectAllItems();
	}
	else
	{
		SetCapture();

		m_bLButtonDown = TRUE;
		m_ptLButtonDown = point;

		m_bItemSelectedBeforeLButtonDown = IsItemSelected(hItem);
		m_bFocusedBeforeLButtonDown = (GetFocus() == this);

		if (IsControlDown())
		{	
			m_nSelectType = typeControl;

			SetCaretItem(hItem);
			if (!m_bItemSelectedBeforeLButtonDown)
				SelectItem(hItem);
		}
		else if (IsShiftDown())
		{
			m_bDisableSelChange = TRUE;

			HTREEITEM hFirstItem = m_hItemCaret;
			if (!hFirstItem)
				hFirstItem = GetRootItem();

			UnselectAllItems();

			if (hFirstItem)
			{
				CRect rcFirstItem, rcItem;
				GetItemRect(hFirstItem, &rcFirstItem, FALSE);
				GetItemRect(hItem, &rcItem, FALSE);
				UINT nCode = rcFirstItem.top < rcItem.top ? TVGN_NEXTVISIBLE : TVGN_PREVIOUSVISIBLE;
				HTREEITEM hItemNext = hFirstItem;
				while (hItemNext && hItemNext != hItem)
				{
					SelectItem(hItemNext);
					hItemNext = GetNextItem(hItemNext, nCode);
				}
			}

			m_nSelectType = typeShift;

			SelectItem(hItem);

			m_bDisableSelChange = FALSE;
			OnSelChanged();
		}
		else
		{
			m_nSelectType = typeNormal;

			if (!IsItemSelected(hItem))
			{
				m_bDisableSelChange = TRUE;
				UnselectAllItems();
				m_bDisableSelChange = FALSE;
			}
			SetCaretItem(hItem);
			SelectItem(hItem);
		}
	}

	if (GetFocus() != this)
		SetFocus();

	NMHDR nmhdr;
	nmhdr.hwndFrom = m_hWnd;
	nmhdr.idFrom = GetDlgCtrlID();
	nmhdr.code = NM_CLICK;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&nmhdr);

	//CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CMulSelTreeCtrl::SelectItem(HTREEITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return FALSE;

	// if already in correct state?
	if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
		return TRUE;

	if (!SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED))
		return FALSE;

	//TRACE2("SelectItem(%08x):%s\n", hItem, GetItemText(hItem));

	m_listSelectedItems.push_back(hItem);


	item_list::iterator it = m_listSelectedItems.begin();
	for (; it != m_listSelectedItems.end(); it++)
	{
		HTREEITEM hItem = *it;
		//TRACE1("\t%08x\n", hItem);
	}

	if (!m_bDisableSelChange)
		OnSelChanged();

	return TRUE;
}

BOOL CMulSelTreeCtrl::UnselectItem(HTREEITEM hItem, BOOL bRemoveFromList)
{
	//TRACE2("UnselectItem(%08x):%s\n", hItem, GetItemText(hItem));

	ASSERT(hItem);
	if (!hItem)
		return FALSE;

	// if already in correct state?
	if (GetItemState(hItem, TVIS_SELECTED) == 0)
		return TRUE;

	if (!SetItemState(hItem, 0, TVIS_SELECTED))
		return FALSE;

	//TRACE2("UnelectItem(%08xd):%s\n", hItem, GetItemText(hItem));

	if (bRemoveFromList)
		m_listSelectedItems.remove(hItem);

	if (!m_bDisableSelChange)
		OnSelChanged();

	return TRUE;
}

int CMulSelTreeCtrl::GetChildItemCount(HTREEITEM hItem)
{
	int count = 0;

	HTREEITEM hChildItem = NULL;
	if (hItem == TVI_ROOT)
		hChildItem = GetRootItem();
	else
		hChildItem = GetChildItem(hItem);

	while (hChildItem)
	{
		count++;
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}

	return count;
}

BOOL CMulSelTreeCtrl::GetChildItems(HTREEITEM hItem, item_list &items)
{
	HTREEITEM hChildItem = NULL;
	if (hItem == TVI_ROOT)
		hChildItem = GetNextItem(TVI_ROOT, TVGN_ROOT);
	else
		hChildItem = GetChildItem(hItem);
	while (hChildItem)
	{
		items.push_back(hChildItem);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	return TRUE;
}

BOOL CMulSelTreeCtrl::SelectChildItems(HTREEITEM hItem)
{
	if (!ItemHasChildren(hItem))
		return FALSE;

	HTREEITEM hChildItem = GetChildItem(hItem);
	ASSERT(hChildItem);
	while (hChildItem)
	{
		SelectItem(hChildItem);
		SelectChildItems(hChildItem);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}

	return TRUE;
}

BOOL CMulSelTreeCtrl::UnselectChildItems(HTREEITEM hItem, BOOL bRemoveFromList)
{
	if (!ItemHasChildren(hItem))
		return FALSE;

	HTREEITEM hChildItem = GetChildItem(hItem);
	ASSERT(hChildItem);
	while (hChildItem)
	{
		UnselectItem(hChildItem, bRemoveFromList);
		UnselectChildItems(hChildItem, bRemoveFromList);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}

	return TRUE;
}

BOOL CMulSelTreeCtrl::UnselectAllItems()
{
	if (m_listSelectedItems.size() > 0)
	{
		BOOL bDisableSelChange = m_bDisableSelChange;

		m_bDisableSelChange = TRUE;

		item_list::iterator it = m_listSelectedItems.begin();
		for (; it != m_listSelectedItems.end(); it++)
		{
			HTREEITEM hItem = *it;
			UnselectItem(hItem, FALSE);
		}

		m_listSelectedItems.clear();

		if (!bDisableSelChange)
		{
			m_bDisableSelChange = FALSE;
			OnSelChanged();
		}
	}

	return TRUE;
}

BOOL CMulSelTreeCtrl::SelectAllItems()
{
	HTREEITEM hRoot = GetRootItem();
	while (hRoot)
	{
		SelectItem(hRoot);
		SelectChildItems(hRoot);
		hRoot = GetNextItem(hRoot, TVGN_NEXT);
	}
	return TRUE;
}

void CMulSelTreeCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CTreeCtrl::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here

	RedrawAllSelectedItems();
}

void CMulSelTreeCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CTreeCtrl::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
	RedrawAllSelectedItems();
}

BOOL CMulSelTreeCtrl::IsItemSelected(HTREEITEM hItem)
{
	return (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED);
}

BOOL CMulSelTreeCtrl::SetCaretItem(HTREEITEM hItem)
{
	if (m_hItemCaret == hItem)
		return TRUE;

	if (m_hItemCaret)
		RedrawItem(m_hItemCaret, TRUE);

	if (hItem)
		RedrawItem(hItem, TRUE);

	m_hItemCaret = hItem;

	return TRUE;
}

HTREEITEM CMulSelTreeCtrl::GetCaretItem()
{
	return m_hItemCaret;
}

void CMulSelTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bLButtonDown)
	{
		m_bLButtonDown = FALSE;

		if (!m_bDragging)
		{
			HTREEITEM hItem;
			BOOL bSelChange = FALSE;
			switch (m_nSelectType)
			{
			case typeNormal:
				hItem = m_hItemCaret;
				ASSERT(IsItemSelected(hItem));

				m_bDisableSelChange = TRUE;

				if (m_listSelectedItems.size() > 1 || *m_listSelectedItems.begin() != hItem)
				{
					bSelChange = TRUE;
					UnselectAllItems();
				}

				if (hItem)
				{
					if (bSelChange)
						SelectItem(hItem);
					if (m_bItemSelectedBeforeLButtonDown && m_bFocusedBeforeLButtonDown)
					{
						UINT flags;
						HitTest(m_ptLButtonDown, &flags);
						if (flags & TVHT_ONITEMLABEL)
							DelayEditLabel(hItem);
					}
				}

				m_bDisableSelChange = FALSE;

				if (bSelChange)
					OnSelChanged();

				break;

			case typeControl:
				hItem = m_hItemCaret;
				if (hItem && m_bItemSelectedBeforeLButtonDown)
					UnselectItem(hItem, TRUE);
				break;

			case typeShift:
				break;

			default:
				ASSERT(FALSE);
			}

		}
		else
		{
			EndDrag(TRUE, point);
		}

		ReleaseCapture();
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CMulSelTreeCtrl::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here

	if (pWnd != this && m_bDragging)
		EndDrag(FALSE, CPoint(0, 0));

	CTreeCtrl::OnCaptureChanged(pWnd);
}

void CMulSelTreeCtrl::BeginDrag()
{
	m_bDragging = TRUE;
	m_nDropPos = dropNone;

	if (GetStyle() & TVS_INFOTIP)
	{
		CToolTipCtrl *pTT = GetToolTips();
		if (pTT)
			pTT->Activate(FALSE);
	}
}

void CMulSelTreeCtrl::EndDrag(BOOL bDrop, CPoint point)
{
	ASSERT(m_bEnableDragDrop);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	CancelDelayExpand();
	CancelDelayScroll();

	m_bLButtonDown = FALSE;
	m_bDragging = FALSE;

	if (bDrop)
	{
		if (m_hItemDrop)
		{
			if (CanDrop(m_hItemDrop, m_nDropPos))
				OnDrop(m_hItemDrop, m_nDropPos);
		}
		else
		{
			if (CanDropOuter(point))
				OnDropOuter(point);
		}
	}

	SetDropTarget(NULL, dropNone);

	if (GetStyle() & TVS_INFOTIP)
	{
		CToolTipCtrl *pTT = GetToolTips();
		if (pTT)
			pTT->Activate(TRUE);
	}
}

void CMulSelTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bDragging)
	{
		HTREEITEM hItem = HitTest(point, NULL);
		if (!hItem)
		{
			CRect rc;
			GetClientRect(&rc);
			if (rc.PtInRect(point))
				hItem = TVI_ROOT;
		}

		if (hItem)
		{
			if (m_hItemDelayExpand && hItem != m_hItemDelayExpand)
				CancelDelayExpand();

			if (m_hItemDelayScroll && hItem != m_hItemDelayScroll)
				CancelDelayScroll();

			if (hItem != TVI_ROOT)
				DelayExpand(hItem);
			DelayScroll(hItem);

			DROPPOS nDropPos = dropInner;
			if (hItem != TVI_ROOT)
			{
				CRect rcItem;
				GetItemRect(hItem, &rcItem, FALSE);

				if (m_bExtDragDrop)
				{
					if (point.y < rcItem.top + 5)
						nDropPos = dropBefore;
					else if (point.y > rcItem.bottom - 5)
						nDropPos = dropAfter;
				}
			}
			else
			{
				hItem = GetRootItem();
				while (true)
				{
					HTREEITEM hNextItem = GetNextSiblingItem(hItem);
					if (!hNextItem)
						break;
					hItem = hNextItem;
				}
				nDropPos = dropAfter;
			}

			SetDropTarget(hItem, nDropPos);
		}
		else
		{
			CancelDelayExpand();
			CancelDelayScroll();

			SetDropTarget(NULL, dropNone);
		}

		SetCursor(point);
	}
	else if (m_bLButtonDown)
	{
		if (m_bEnableDragDrop)
		{
			int dx = point.x - m_ptLButtonDown.x;
			int dy = point.y - m_ptLButtonDown.y;
			if (dx > 5 || dx < -5 || dy > 5 || dy < -5)
				BeginDrag();
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

HTREEITEM CMulSelTreeCtrl::GetFirstSelectedItem()
{
	HTREEITEM hItem = GetNextItem(TVI_ROOT, TVGN_FIRSTVISIBLE);
	while (hItem)
	{
		if (IsItemSelected(hItem))
			return hItem;
		hItem = GetNextItem(hItem, TVGN_NEXTVISIBLE);
	}
	return NULL;
}

BOOL CMulSelTreeCtrl::IsParentItemSelected(HTREEITEM hItem)
{
	while (hItem = GetNextItem(hItem, TVGN_PARENT))
	{
		if (IsItemSelected(hItem))
			return TRUE;
	}
	return FALSE;
}

BOOL CMulSelTreeCtrl::CanDrop(HTREEITEM hItem, DROPPOS nDropPos)
{
	return (hItem == TVI_ROOT) || (!IsItemSelected(hItem) && !IsParentItemSelected(hItem));
}

BOOL CMulSelTreeCtrl::CanDropOuter(CPoint point)
{
	return FALSE;
}

BOOL CMulSelTreeCtrl::OnDropOuter(CPoint point)
{
	return FALSE;
}

void CMulSelTreeCtrl::SetDropTarget(HTREEITEM hItem, DROPPOS nDropPos)
{
	ASSERT(hItem || (!hItem && nDropPos == dropNone));

	if (hItem != m_hItemDrop || nDropPos != m_nDropPos)
	{
		if (m_hItemDrop)
			RedrawItem(m_hItemDrop, FALSE);

		if (hItem)
			RedrawItem(hItem, FALSE);
	}

	m_hItemDrop = hItem;
	m_nDropPos = nDropPos;
}

void CMulSelTreeCtrl::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	switch (nIDEvent)
	{
		case IDT_DELAY_EXPAND:
		{
			KillTimer(IDT_DELAY_EXPAND);

			ASSERT(m_hItemDelayExpand);

			if (ItemHasChildren(m_hItemDelayExpand))
			{
				TRACE1("Expand(%x)\n", m_hItemDelayExpand);
				Expand(m_hItemDelayExpand, TVE_EXPAND);
				if (m_nDropPos == dropAfter)
				{
					HTREEITEM hItem = GetChildItem(m_hItemDelayExpand);
					ASSERT (hItem);
					while (TRUE)
					{
						HTREEITEM hItemNext = GetNextSiblingItem(hItem);
						if (!hItemNext)
						{
							TRACE1("LastChildItem: %s\n", GetItemText(hItem));
							RedrawItem(hItem, FALSE);
							break;
						}
						hItem = hItemNext;
					}
				}
			}

			m_hItemDelayExpand = NULL;

			CPoint point;
			GetCursorPos(&point);
			ScreenToClient(&point);
			SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));

			break;
		}
		case IDT_DELAY_SCROLL:
		{
			KillTimer(IDT_DELAY_SCROLL);

			ASSERT(m_hItemDelayScroll);

			//HTREEITEM hItem = GetNextVisibleItem(m_hItemDelayScroll);

			//TRACE1("OnTimer(IDT_DELAY_SCROLL), GetNextVisibleItem()=%x\n", hItem);

			//m_hItemDelayScroll = NULL;

			SetDropTarget(NULL, dropNone);

			//if (hItem)
			//	EnsureVisible(hItem);
			SendMessage(WM_VSCROLL, m_nScrollDir == dirUp ? SB_LINEUP : SB_LINEDOWN, NULL);

			POINT point;
			GetCursorPos(&point);
			ScreenToClient(&point);
			SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));

			break;
		}
		case IDT_DELAY_EDIT_LABEL:
		{
			KillTimer(IDT_DELAY_EDIT_LABEL);
			BeginEditLabel(m_hItemDelayEditLabel);

			break;
		}
		case IDT_CHAR_INPUT:
		{
			KillTimer(IDT_CHAR_INPUT);
			m_strCharInput.Empty();

			break;
		}
	}

	CTreeCtrl::OnTimer(nIDEvent);
}

void CMulSelTreeCtrl::DelayExpand(HTREEITEM hItem)
{
	ASSERT(hItem);

	if (hItem && (GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED))
		return;

	if (m_hItemDelayExpand != hItem)
	{
		if (m_hItemDelayExpand)
			KillTimer(IDT_DELAY_EXPAND);

		m_hItemDelayExpand = hItem;

		SetTimer(IDT_DELAY_EXPAND, 1000, NULL);
	}
}

void CMulSelTreeCtrl::CancelDelayExpand()
{
	if (m_hItemDelayExpand)
	{
		KillTimer(IDT_DELAY_EXPAND);
		m_hItemDelayExpand = NULL;
	}
}

void CMulSelTreeCtrl::DelayScroll(HTREEITEM hItem)
{
	ASSERT(hItem);

	TRACE2("m_hItemDelayScroll=%d, hItem=%x\n", m_hItemDelayScroll, hItem);

	if (m_hItemDelayScroll != hItem)
	{
		/*
		int height = GetItemHeight();

		CRect rcClient;
		GetClientRect(&rcClient);

		CRect rcItem;
		GetItemRect(hItem, &rcItem, FALSE);

		if (rcItem.top > height * 2 && rcItem.top < rcClient.Height() - height * 2)
			return;
			*/

		HTREEITEM hItemFirstVisible = GetFirstVisibleItem();
		if (hItem == hItemFirstVisible)
		{
			m_nScrollDir = dirUp;
		}
		else
		{
			HTREEITEM hItemLastVisible = GetLastVisibleItem();
			if (hItem == hItemLastVisible)
			{
				m_nScrollDir = dirDown;
			}
			else
			{
				hItemFirstVisible = GetNextVisibleItem(hItemFirstVisible);
				if (hItem == hItemFirstVisible)
				{
					m_nScrollDir = dirUp;
				}
				else
				{
					hItemLastVisible = GetPrevVisibleItem(hItemLastVisible);
					if (hItem == hItemLastVisible)
					{
						m_nScrollDir = dirDown;
					}
					else
					{
						return;
					}
				}
			}
		}

		ASSERT(!m_hItemDelayScroll);
		if (m_hItemDelayScroll)
			KillTimer(IDT_DELAY_SCROLL);

		m_hItemDelayScroll = hItem;

		//TRACE1("m_hItemDelayScroll=%x\n", m_hItemDelayScroll);

		SetTimer(IDT_DELAY_SCROLL, 100, NULL);
	}
}

void CMulSelTreeCtrl::CancelDelayScroll()
{
	if (m_hItemDelayScroll)
	{
		KillTimer(IDT_DELAY_SCROLL);
		m_hItemDelayScroll = NULL;
	}
}

void CMulSelTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bDragging)
	{
		switch (nChar)
		{
			case VK_CONTROL:
			{
				CPoint point;
				GetCursorPos(&point);
				ScreenToClient(&point);
				SetCursor(point);
				break;
			}

			case VK_ESCAPE:
			{
				ReleaseCapture();
				break;
			}
		}
		
		return;
	}

	CancelDelayEditLabel();

	switch (nChar)
	{
	case VK_UP:
		if (m_hItemCaret)
		{
			HTREEITEM hItem = GetPrevVisibleItem(m_hItemCaret);
			if (hItem)
			{
				m_bDisableSelChange = TRUE;

				UnselectAllItems();
				SetCaretItem(hItem);
				SelectItem(hItem);
				EnsureVisible(hItem);

				m_bDisableSelChange = FALSE;
				OnSelChanged();

				UpdateWindow();
			}
		}
		break;

	case VK_DOWN:
		if (m_hItemCaret)
		{
			HTREEITEM hItem = GetNextVisibleItem(m_hItemCaret);
			if (hItem)
			{
				m_bDisableSelChange = TRUE;

				UnselectAllItems();
				SetCaretItem(hItem);
				SelectItem(hItem);
				EnsureVisible(hItem);

				m_bDisableSelChange = FALSE;
				OnSelChanged();

				UpdateWindow();
			}
		}
		break;

	case VK_LEFT:
		if (m_hItemCaret)
		{
			if ((ItemHasChildren(m_hItemCaret) && GetItemState(m_hItemCaret, TVIS_EXPANDED) & TVIS_EXPANDED))
			{
				Expand(m_hItemCaret, TVE_COLLAPSE);
				EnsureVisible(m_hItemCaret);
			}
			else
			{
				HTREEITEM hItem = GetParentItem(m_hItemCaret);
				if (hItem)
				{
					UnselectAllItems();
					SetCaretItem(hItem);
					SelectItem(hItem);
					EnsureVisible(hItem);

					UpdateWindow();
				}
			}
		}
		break;

	case VK_RIGHT:
		if (m_hItemCaret)
		{
			if (ItemHasChildren(m_hItemCaret))
			{
				if (!(GetItemState(m_hItemCaret, TVIS_EXPANDED) & TVIS_EXPANDED))
				{
					Expand(m_hItemCaret, TVE_TOGGLE);
					EnsureVisible(m_hItemCaret);
				}
				else
				{
					HTREEITEM hItem = GetNextItem(m_hItemCaret, TVGN_CHILD);
					if (hItem)
					{
						UnselectAllItems();
						SetCaretItem(hItem);
						SelectItem(hItem);
						EnsureVisible(hItem);

						UpdateWindow();
					}
				}
			}
		}
		break;

	case VK_PRIOR:
		if (m_hItemCaret)
		{
			HTREEITEM hItem = m_hItemCaret;
			HTREEITEM hItemPrev;
			int count = GetVisibleCount();
			for (int i = 0; i < count - 1; i++)
			{
				hItemPrev = GetPrevVisibleItem(hItem);
				if (!hItemPrev)
					break;

				hItem = hItemPrev;
			}

			UnselectAllItems();
			SetCaretItem(hItem);
			SelectItem(hItem);
			EnsureVisible(hItem);

			UpdateWindow();
		}

		break;

	case VK_NEXT:
		if (m_hItemCaret)
		{
			HTREEITEM hItem = m_hItemCaret;
			HTREEITEM hItemNext;
			int count = GetVisibleCount();
			for (int i = 0; i < count - 1; i++)
			{
				hItemNext = GetNextVisibleItem(hItem);
				if (!hItemNext)
					break;

				hItem = hItemNext;
			}

			UnselectAllItems();
			SetCaretItem(hItem);
			SelectItem(hItem);
			EnsureVisible(hItem);

			UpdateWindow();
		}

		break;

	case VK_HOME:
		{
			SendMessage(WM_VSCROLL, SB_TOP, NULL);
			HTREEITEM hItem = GetFirstVisibleItem();
			if (hItem)
			{
				UnselectAllItems();
				SetCaretItem(hItem);
				SelectItem(hItem);
				EnsureVisible(hItem);

				UpdateWindow();
			}
		}

		break;

	case VK_END:
		{
			SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
			HTREEITEM hItem = GetLastVisibleItem();
			if (hItem)
			{
				UnselectAllItems();
				SetCaretItem(hItem);
				SelectItem(hItem);
				EnsureVisible(hItem);

				UpdateWindow();
			}
		}

		break;

	default:
		return;
		//CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CMulSelTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bDragging)
	{
		switch (nChar)
		{
			case VK_CONTROL:
			{
				CPoint point;
				GetCursorPos(&point);
				ScreenToClient(&point);
				SetCursor(point);
				break;
			}
		}
		
		return;
	}

	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMulSelTreeCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	// if itemNew != itemOld

	// Preven from changing
	*pResult = 1;
}

void CMulSelTreeCtrl::OnPaint() 
{
	CRect rcUpdate;
	GetUpdateRect(&rcUpdate);
	
	// TODO: Add your message handler code here
	
	// Do not call CTreeCtrl::OnPaint() for painting messages

	//CTreeCtrl::OnPaint();

	//if (rcUpdate.IsRectEmpty())
	//	return;

    CPaintDC paintDc(this);
    CDC dc;
    dc.CreateCompatibleDC(&paintDc);
    CRect clientRect;
    GetClientRect(&clientRect);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&paintDc, clientRect.right, clientRect.bottom);
    CBitmap *pOldBitmap = dc.SelectObject(& bitmap);
    CWnd::DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);

	CWnd *pFocus = GetFocus();
	BOOL bFocused = (pFocus == this);

	//CClientDC dc(this);
	dc.SetBkMode(TRANSPARENT);

	CFont *pFont = GetFont();
	CFont *pOldFont = dc.SelectObject(pFont);

	CBrush brItem(::GetSysColor(COLOR_WINDOW));
	CBrush brHighlight(::GetSysColor(COLOR_HIGHLIGHT));

	CRect rcItem;

	HTREEITEM hItemVisible = GetFirstVisibleItem();
	while (hItemVisible)
	{
		GetItemRect(hItemVisible, &rcItem, TRUE);

		CRect rcInter;
		if (rcInter.IntersectRect(&rcUpdate, &rcItem))
		{
			if (hItemVisible != m_hItemEditLabel)
			{
				dc.FillRect(&rcItem, (bFocused && IsItemSelected(hItemVisible)) ? &brHighlight : &brItem);
				
				CRect rcText(rcItem.left + TEXT_PADDING, rcItem.top, rcItem.right - TEXT_PADDING, rcItem.bottom);
				CString strText = GetItemText(hItemVisible);
				dc.SetTextColor(::GetSysColor((bFocused && IsItemSelected(hItemVisible)) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
				dc.DrawText(strText, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP);

				if (!bFocused && IsItemSelected(hItemVisible))
					dc.FrameRect(&rcItem, &brHighlight);
			}
		}
	
		hItemVisible = GetNextVisibleItem(hItemVisible);
	}

	if (bFocused)
	{
		if (m_hItemCaret)
		{
			CRect rcCaret;
			GetItemRect(m_hItemCaret, &rcCaret, TRUE);

			CRect rcInter;
			if (rcInter.IntersectRect(&rcUpdate, &rcCaret))
			{
				dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				dc.DrawFocusRect(&rcCaret);
			}
		}
	}

	if (m_hItemDrop && m_hItemDrop != TVI_ROOT)
	{
		CRect rcDrop;
		GetItemRect(m_hItemDrop, &rcDrop, FALSE);

		CRect rcInter;
		if (rcInter.IntersectRect(&rcUpdate, &rcDrop))
		{
			if (m_bExtDragDrop)
			{
				if (m_nDropPos == dropBefore)
				{
					rcDrop.left += GetItemIndent(m_hItemDrop) + 16;
					rcDrop.right -= 6;
					rcDrop.top--;
					rcDrop.bottom = rcDrop.top + 1;
					
					DrawSeprator(&dc, rcDrop);
				}
				else if (m_nDropPos == dropAfter)
				{
					rcDrop.left += GetItemIndent(m_hItemDrop) + 16;
					rcDrop.right -= 6;
					rcDrop.top = rcDrop.bottom - 1;

					DrawSeprator(&dc, rcDrop);
				}
			}

			if (!m_bExtDragDrop || m_nDropPos == dropInner)
			{
				GetItemRect(m_hItemDrop, &rcDrop, TRUE);
				dc.FrameRect(&rcDrop, &brHighlight);
			}
		}
	}


	paintDc.BitBlt(0, 0, clientRect.right, clientRect.bottom, &dc, 0, 0, SRCCOPY);

	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldBitmap);
}

void CMulSelTreeCtrl::RedrawItem(HTREEITEM hItem, BOOL bTextOnly)
{
	ASSERT(hItem);
	if (!hItem || hItem == TVI_ROOT)
		return;

	CRect rcItem;
	GetItemRect(hItem, &rcItem, bTextOnly);

	//if (m_nDropPos == dropBefore)
	//	rcItem.top -= 3;
	//if (m_nDropPos == dropAfter)
	//	rcItem.bottom += 3;

	rcItem.InflateRect(0, 3);

	InvalidateRect(&rcItem);
}

void CMulSelTreeCtrl::RedrawAllSelectedItems()
{
	CRect rcItem;
	HTREEITEM hItemVisible = GetFirstVisibleItem();
	while (hItemVisible)
	{
		if (IsItemSelected(hItemVisible) || hItemVisible == m_hItemCaret)
		{
			GetItemRect(hItemVisible, &rcItem, TRUE);
			InvalidateRect(&rcItem);
		}
	
		hItemVisible = GetNextVisibleItem(hItemVisible);
	}
}

HTREEITEM CMulSelTreeCtrl::GetLastVisibleItem()
{
	HTREEITEM hItem = GetFirstVisibleItem();
	if (!hItem)
		return NULL;

	UINT count = GetVisibleCount();
	HTREEITEM hItemNext;
	for (UINT i = 0; i < count; i++)
	{
		hItemNext = GetNextVisibleItem(hItem);
		if (!hItemNext)
			break;

		hItem = hItemNext;
	}

	//TRACE1("GetLastVisibleItem()=%s\n", GetItemText(hItem));

	return hItem;
}

void CMulSelTreeCtrl::PreDrop(HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL bCopy)
{
}

void CMulSelTreeCtrl::PostDrop(HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL bCopy)
{
}

BOOL CMulSelTreeCtrl::OnDrop(HTREEITEM hItemDrop, DROPPOS nDropPos)
{
	SetRedraw(FALSE);

	ASSERT(m_hItemDrop);
	ASSERT(m_nDropPos != dropNone);

	BOOL bCopy = (m_bEnableDragDropCopy && IsControlDown());

	HTREEITEM hItemDropReal;
	HTREEITEM hItemInsertAfter;
	if (m_nDropPos == dropBefore)
	{
		hItemDropReal = GetParentItem(m_hItemDrop);
		if (hItemDropReal == NULL)
			hItemDropReal = TVI_ROOT;

		HTREEITEM hItemPrev = GetPrevSiblingItem(m_hItemDrop);
		if (hItemPrev)
		{
			hItemInsertAfter = hItemPrev;
		}
		else
		{
			hItemInsertAfter = TVI_FIRST;
		}
	}
	else if (m_nDropPos == dropAfter)
	{
		hItemDropReal = GetParentItem(m_hItemDrop);
		if (hItemDropReal == NULL)
			hItemDropReal = TVI_ROOT;

		hItemInsertAfter = m_hItemDrop;
	}
	else
	{
		hItemDropReal = m_hItemDrop;
		hItemInsertAfter = TVI_LAST;
	}

	PreDrop(hItemDropReal, hItemInsertAfter, bCopy);

	item_list listToMove, listToDelete;

	TRACE0("Filter before move -----------\n");
	item_list::iterator it = m_listSelectedItems.begin();
	for (; it != m_listSelectedItems.end(); it++)
	{
		HTREEITEM hItem = *it;
#ifdef _DEBUG
		CString strText = GetItemText(hItem);
		TRACE1("\t%s\n", strText);
#endif
		if (!IsParentItemSelected(hItem))
			if (!(GetParentItem(hItem) == hItemDropReal && m_nDropPos == dropInner))
				listToMove.push_back(hItem);
	}

	TRACE0("Begin Moving -------------\n");

	ASSERT(m_listInsertedItems.size() == 0);
	ASSERT(m_listMovedItems.size() == 0);

	m_nIndent = 0;

	BOOL bMoved = FALSE;
	item_list::iterator it2 = listToMove.begin();
	for (; it2 != listToMove.end(); it2++)
	{
		HTREEITEM hItem = *it2;
#ifdef _DEBUG
		CString strText = GetItemText(hItem);
		TRACE1("\t%s\n", strText);
#endif
		if (!MoveItem(hItem, hItemDropReal, hItemInsertAfter, bCopy))
			break;
	}

	listToMove.clear();

	if (m_listMovedItems.size() > 0)
	{
		Expand(hItemDropReal, TVE_EXPAND);

		UnselectAllItems();

		if (!bCopy)
		{
			for (item_list::iterator it3 = m_listMovedItems.begin(); it3 != m_listMovedItems.end(); it3++)
			{
				if (!ItemHasChildren(*it3))
				{
					if (PreDeleteItem(*it3))
						DeleteItem(*it3);
				}
			}
		}

		if (m_listItemsToSelect.size() > 0)
		{
			item_list::iterator it4 = m_listItemsToSelect.begin();
			SetCaretItem(*it4);
			for (; it4 != m_listItemsToSelect.end(); it4++)
				SelectItem(*it4);

			m_listItemsToSelect.clear();
		}
		else
		{
			SetCaretItem(hItemDropReal);
			SelectItem(hItemDropReal);
		}

		SortChildrenEx(hItemDropReal, TRUE);

		m_listInsertedItems.clear();
	}

	m_listMovedItems.clear();

	PostDrop(hItemDropReal, hItemInsertAfter, bCopy);

	SetRedraw(TRUE);

	return TRUE;
}

BOOL CMulSelTreeCtrl::MoveItem(HTREEITEM hItemSrc, HTREEITEM hItemNewParent, HTREEITEM hItemInsertAfter, BOOL bCopy)
{
	//SetRedraw(FALSE);
#ifdef _DEBUG
	for (int i = 0; i < m_nIndent; i++)
		TRACE0("  ");
	TRACE1("MoveItem: %s\n", GetItemText(hItemSrc));
#endif

	BOOL bSkipMoving = FALSE;
	BOOL bReplace = FALSE;

	if (!PreMoveItem(hItemSrc, hItemNewParent, bSkipMoving, bReplace))
		return FALSE;

	if (bSkipMoving)
		return TRUE;

	HTREEITEM hItemSrcNew = NULL;

	// Sometims bReplace == TRUE, bute no matched items can be found.
	// e.g. a duplicate file exists int the target dir when moving, but 
	// the content of the treectrl is not refreshed
	if (bReplace)
	{
		// try to find the mached item
		CString str1, str2;
		str1 = GetItemText(hItemSrc);
		hItemSrcNew = GetChildItem(hItemNewParent);
		str2 = GetItemText(hItemSrcNew);
		while (hItemSrcNew && str2.CompareNoCase(str1) != 0)
		{
			hItemSrcNew = GetNextSiblingItem(hItemSrcNew);
			str2 = GetItemText(hItemSrcNew);
		}
		ASSERT(hItemSrcNew);
	}

	if (!hItemSrcNew)
	{
		bReplace = FALSE;

		TV_INSERTSTRUCT tvis;

		tvis.item.hItem = hItemSrc;
		tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		GetItem(&tvis.item);

		tvis.hParent = hItemNewParent;
		tvis.hInsertAfter = hItemInsertAfter;

		hItemSrcNew = InsertItem(&tvis);
		if (hItemSrcNew)
		{
			CString str = GetItemText(hItemSrc);
			SetItemText(hItemSrcNew, str);
		}
	}

	PostMoveItem(hItemSrc, hItemSrcNew, bCopy, bReplace);

	HTREEITEM hChild = GetChildItem(hItemSrc);
	m_nIndent++;
	while (hChild)
	{
		if (!MoveItem(hChild, hItemSrcNew, TVI_LAST, bCopy))
			return FALSE;
		hChild = GetNextSiblingItem(hChild);
	}
	m_nIndent--;

	m_listMovedItems.push_back(hItemSrc);
	m_listInsertedItems.push_back(hItemSrcNew);

	if (GetItemState(hItemSrc, TVIS_EXPANDED) & TVIS_EXPANDED)
		Expand(hItemSrcNew, TVE_EXPAND);

	if (GetItemState(hItemSrc, TVIS_SELECTED) & TVIS_SELECTED)
		m_listItemsToSelect.push_back(hItemSrcNew);

	//for (i = 0; i < m_nIndent; i++)
	//	TRACE0("  ");
	//TRACE1("m_listMovedItems.push_back: %s\n", GetItemText(hItemSrc));

	//SetRedraw(TRUE);

	return TRUE;
}

BOOL CMulSelTreeCtrl::PreMoveItem(HTREEITEM hItemSrc, HTREEITEM hItemNewParent, BOOL &bSkip, BOOL &bReplace)
{
	return TRUE;
}

void CMulSelTreeCtrl::PostMoveItem(HTREEITEM hItemOld, HTREEITEM hItemNew, BOOL bCopy, BOOL bReplace)
{
}

BOOL CMulSelTreeCtrl::SortChildrenEx(HTREEITEM hItem, BOOL bRecursive)
{
	return TRUE;
}

void CMulSelTreeCtrl::BeginEditLabel(HTREEITEM hItem)
{

}

void CMulSelTreeCtrl::DelayEditLabel(HTREEITEM hItem)
{
	m_hItemDelayEditLabel = hItem;
	SetTimer(IDT_DELAY_EDIT_LABEL, GetDoubleClickTime() * 2, NULL);
}

void CMulSelTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CancelDelayEditLabel();

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

BOOL CMulSelTreeCtrl::PreDeleteItem(HTREEITEM hItem)
{
	return TRUE;
}

int CMulSelTreeCtrl::GetItemIndent(HTREEITEM hItem)
{
	int count = 0;
	while ((hItem = GetParentItem(hItem)) != NULL)
		count++;

	return GetIndent() * count;
}

void CMulSelTreeCtrl::DrawSeprator(CDC *pDC, CRect &rc)
{
	pDC->MoveTo(rc.left - 2, rc.top - 2);
	pDC->LineTo(rc.left - 2, rc.bottom + 3);
	pDC->MoveTo(rc.left - 1, rc.top - 1);
	pDC->LineTo(rc.left - 1, rc.bottom + 2);

	pDC->MoveTo(rc.left, rc.top);
	pDC->LineTo(rc.right + 1, rc.top);
	pDC->MoveTo(rc.left, rc.bottom);
	pDC->LineTo(rc.right + 1, rc.bottom);
	
	pDC->MoveTo(rc.right + 2, rc.top - 2);
	pDC->LineTo(rc.right + 2, rc.bottom + 3);
	pDC->MoveTo(rc.right + 1, rc.top - 1);
	pDC->LineTo(rc.right + 1, rc.bottom + 2);
}

item_list & CMulSelTreeCtrl::GetSelectedItems()
{
	return m_listSelectedItems;
}

void CMulSelTreeCtrl::SetExtDragDrop(BOOL bExt)
{
	m_bExtDragDrop = bExt;
}

BOOL CMulSelTreeCtrl::DeleteAllItems()
{
	UnselectAllItems();
	return __super::DeleteAllItems();
}

void CMulSelTreeCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;

	UnselectItem(hItem, TRUE);
	if (hItem == m_hItemCaret)
	{
		HTREEITEM hRoot = GetRootItem();
		SetCaretItem(hRoot == hItem ? NULL : hRoot);
	}

	// must refresh the item's 'plus sign' if this is the last child
	HTREEITEM hParentItem = GetParentItem(hItem);
	if (hParentItem 
		&& ((hItem = GetChildItem(hParentItem)) != NULL) 
		&& (GetNextSiblingItem(hItem) == NULL))
	{
		// the last item
		TVITEM tvi;
		tvi.mask = TVIF_CHILDREN;
		tvi.hItem = hParentItem;
		GetItem(&tvi);
		tvi.cChildren = 0;
		SetItem(&tvi);
	}

	*pResult = 0;
}

void CMulSelTreeCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	ModifyStyle(TVS_SHOWSELALWAYS, 0);

	CToolTipCtrl *pTT = GetToolTips();
	if (pTT)
		pTT->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	CTreeCtrl::PreSubclassWindow();
}

UINT CMulSelTreeCtrl::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return DLGC_WANTALLKEYS;
	//return CTreeCtrl::OnGetDlgCode();
}

BOOL CMulSelTreeCtrl::IsDragging()
{
	return m_bDragging;
}

void CMulSelTreeCtrl::CancelDelayEditLabel()
{
	if (m_hItemDelayEditLabel)
	{
		KillTimer(IDT_DELAY_EDIT_LABEL);
		m_hItemDelayEditLabel = NULL;
	}
}

void CMulSelTreeCtrl::SetCursor(const CPoint &point)
{
	UINT nIDCursor = IDC_NO_DROP;

	if (m_bDragging)
	{
		HTREEITEM hItem = HitTest(point, NULL);
		if (!hItem)
		{
			CRect rc;
			GetClientRect(&rc);
			if (rc.PtInRect(point))
				hItem = TVI_ROOT;
		}

		if ((hItem != NULL && CanDrop(hItem, m_nDropPos)) || (hItem == NULL && CanDropOuter(point)))
		{
			if (m_listSelectedItems.size() == 1)
				nIDCursor = (m_bEnableDragDropCopy && IsControlDown()) ? IDC_DRAG_SINGLE_COPY : IDC_DRAG_SINGLE;
			else
				nIDCursor = (m_bEnableDragDropCopy && IsControlDown()) ? IDC_DRAG_MULTIPLE_COPY : IDC_DRAG_MULTIPLE;
		}
	}

	::SetCursor(AfxGetApp()->LoadCursor(nIDCursor));
}

BOOL CMulSelTreeCtrl::DeleteItemRecursive(HTREEITEM hItem)
{
	HTREEITEM hChild;
	while (hChild = GetChildItem(hItem))
	{
		if (!DeleteItemRecursive(hChild))
			return FALSE;
	}
	if (!PreDeleteItem(hItem))
		return FALSE;

	return DeleteItem(hItem);
}

void CMulSelTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if (!m_strCharInput.IsEmpty())
		KillTimer(IDT_CHAR_INPUT);

	m_strCharInput += (TCHAR)nChar;
	TRACE1("Input: %s\n", m_strCharInput);

	HTREEITEM hItem = FindItem(NULL, m_strCharInput, FALSE, FALSE, TRUE, TRUE, NULL);
	if (hItem)
	{
		UnselectAllItems();
		SetCaretItem(hItem);
		SelectItem(hItem);
		EnsureVisible(hItem);
	}

	SetTimer(IDT_CHAR_INPUT, 1000, NULL);

	return;
	//CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

int CMulSelTreeCtrl::ComparePosition(HTREEITEM hItem1, HTREEITEM hItem2)
{
	TRACE1("\tCompare: %s\n", GetItemText(hItem1));
	TRACE1("\t         %s\n", GetItemText(hItem2));

	if (hItem1 != hItem2)
	{
		if (IsParentItem(hItem1, hItem2))
			return -1;
		if (IsParentItem(hItem2, hItem1))
			return 1;

		HTREEITEM hParent1 = GetParentItem(hItem1);
		HTREEITEM hParent2 = GetParentItem(hItem2);

		if (hParent1 == hParent2)
		{
			HTREEITEM hChild;

			if (!hParent1)
				hChild = GetRootItem();
			else
				hChild = GetChildItem(hParent1);

			ASSERT(hChild);
			while (hChild)
			{
				if (hChild == hItem1)
					return -1;
				if (hChild == hItem2)
					return 1;
				hChild = GetNextSiblingItem(hChild);
			}
		}
		else
		{
			vector <HTREEITEM> vItems;
			vItems.push_back(hItem1);

			HTREEITEM hParent;

			hParent = hItem1;
			while (hParent = GetParentItem(hParent))
				vItems.push_back(hParent);

			hParent = hItem2;
			HTREEITEM hParentSave = hItem2;
			while (hParent = GetParentItem(hParent))
			{
				for (UINT i = 0; i < vItems.size(); i++)
				{
					if (hParent == vItems[i])
					{
						TRACE1("\tPublic parent: %s\n", GetItemText(hParent));
						ASSERT(i > 0);
						return ComparePosition(vItems[i - 1], hParentSave);
					}
				}
				hParentSave = hParent;
			}
			ASSERT(FALSE);
		}
	}

	return 0;	
}

BOOL CMulSelTreeCtrl::IsParentItem(HTREEITEM hParent, HTREEITEM hChild)
{
	HTREEITEM hItem = hChild;
	while (hItem = GetNextItem(hItem, TVGN_PARENT))
	{
		if (hItem == hParent)
			return TRUE;
	}
	return FALSE;
}

HTREEITEM CMulSelTreeCtrl::FindItem(HTREEITEM hParent, LPCTSTR lpszText, 
	BOOL bMatchWholeWord, BOOL bRegExp, 
	BOOL bExpandedOnly, BOOL bRecursive, HTREEITEM hBelowItem)
{
	ASSERT(lpszText);

	// if hParent == NULL, always search the root items
	if (hParent)
		if (bExpandedOnly && !(GetItemState(hParent, TVIS_EXPANDED) & TVIS_EXPANDED))
			return NULL;

	HTREEITEM hChild = hParent ? GetChildItem(hParent) : GetRootItem();
	for (; hChild != NULL; hChild = GetNextItem(hChild, TVGN_NEXT))
	{
		CString strText = GetItemText(hChild);
		BOOL bFound = FALSE;
		if (!bMatchWholeWord)
		{
			if (!bRegExp)
			{
				bFound = (strText.Left(_tcslen(lpszText)).CompareNoCase(lpszText) == 0);
			}
			else
			{
				//Regexx re;
				//bFound = re.exec((LPCTSTR)strText, lpszText, Regexx::nocase);
			}
		}
		else
		{
			bFound = (strText.CompareNoCase(lpszText) == 0);
		}

		if (bFound)
		{
			if (hBelowItem && ComparePosition(hChild, hBelowItem) <= 0)
				TRACE1("Found above item: %s\n", GetItemText(hChild));
			else
				return hChild;
		}

		if (bRecursive)
		{
			HTREEITEM hItem = FindItem(hChild, lpszText, bMatchWholeWord, bRegExp, bExpandedOnly, bRecursive, hBelowItem);
			if (hItem)
				return hItem;
		}
	}

	return NULL;
}

LRESULT CMulSelTreeCtrl::OnInsertItem(WPARAM wp, LPARAM lp)
{
	TVINSERTSTRUCT *pTVIS = (TVINSERTSTRUCT *)lp;
	BOOL bFirstChild = FALSE;
	if (pTVIS->hParent != TVI_ROOT)
		bFirstChild = !ItemHasChildren(pTVIS->hParent);

	m_hItemInserted = (HTREEITEM)Default();
	if (!m_hItemCaret)
		if (GetCount() == 1)
			SetCaretItem((HTREEITEM)m_hItemInserted);

	// must refresh the parent item's "plus sign" if it's the first child
	if (m_hItemInserted && bFirstChild)
	{
		TVITEM tvi;
		tvi.mask = TVIF_CHILDREN;
		tvi.hItem = pTVIS->hParent;
		GetItem(&tvi);
		tvi.cChildren = 1;
		SetItem(&tvi);
	}

	return (LRESULT)m_hItemInserted;
}

void CMulSelTreeCtrl::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	// see document and define of TVE_COLLAPSERESET to find out why 
	// do i AND action code with 0x0003
	if ((pNMTreeView->action & 0x0003) == TVE_COLLAPSE)
	{
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		
		item_list list;
		item_list::iterator it = m_listSelectedItems.begin();
		for (; it != m_listSelectedItems.end(); it++)
		{
			if (IsParentItem(hItem, *it))
			{
				VERIFY(UnselectItem(*it, FALSE));
				list.push_back(*it);
			}
		}
		for (it = list.begin(); it != list.end(); it++)
			m_listSelectedItems.remove(*it);
		list.clear();

		if (m_hItemCaret && IsParentItem(hItem, m_hItemCaret))
			SetCaretItem(hItem);
	}

	*pResult = 0;
}

void CMulSelTreeCtrl::OnSelChanged()
{
	NMTREEVIEW nm;
	memset(&nm, 0, sizeof(nm));
	nm.hdr.code = TVN_SELCHANGED;
	nm.hdr.hwndFrom = m_hWnd;
	nm.hdr.idFrom = GetDlgCtrlID();

	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nm);
}

void CMulSelTreeCtrl::EnableSelChange(BOOL bEnable)
{
	m_bDisableSelChange = !bEnable;
}

BOOL CMulSelTreeCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}
