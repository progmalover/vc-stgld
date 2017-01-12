// CheckBoxTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBoxTreeCtrl.h"
#include "CBTreeRc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckBoxTreeCtrl

CCheckBoxTreeCtrl::CCheckBoxTreeCtrl()
	: m_bAutoSynchronize(TRUE)
{
	m_bCheckBox = TRUE;
	m_imgState.Create(IDB_CBTREE_STATE, 13, 1, RGB(255 ,255, 255));
}

CCheckBoxTreeCtrl::~CCheckBoxTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CCheckBoxTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCheckBoxTreeCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckBoxTreeCtrl message handlers

void CCheckBoxTreeCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	SetImageList(&m_imgState, TVSIL_STATE);

	CTreeCtrl::PreSubclassWindow();
}

int CCheckBoxTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	// Note: the code in PreSubclassWindow() will failed if the control is 
	// created on-the-fly. So we must check it...
	if (!GetImageList(TVSIL_STATE))
		SetImageList(&m_imgState, TVSIL_STATE);
	
	return 0;
}

BOOL CCheckBoxTreeCtrl::CheckChildren(HTREEITEM hItem)
{
	if (ItemHasChildren(hItem))
	{
		int nState = GetCheck(hItem) ? CHECKED : UNCHECKED;

		HTREEITEM hChild = GetChildItem(hItem);
		while (hChild)
		{
			SetItemState(hChild, INDEXTOSTATEIMAGEMASK(nState + 1), TVIS_STATEIMAGEMASK);
			CheckChildren(hChild);
			hChild = GetNextItem(hChild, TVGN_NEXT);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CCheckBoxTreeCtrl::CheckParent(HTREEITEM hItem)
{
	int nTotal = 0;
	int nChecked = 0;
	int nUnchecked = 0;
	BOOL bIndeterminate = FALSE;
	HTREEITEM hParent = GetParentItem(hItem);
	if (hParent)
	{
		HTREEITEM hChild = GetChildItem(hParent);
		while (hChild)
		{
			nTotal++;
			switch (GetCheck(hChild))
			{
			case CHECKED:
				nChecked++;
				break;
			case UNCHECKED:
				nUnchecked++;
				break;
			default:
				bIndeterminate = TRUE;
				break;
			}

			if (bIndeterminate || (nChecked > 0 && nUnchecked > 0))
				break;

			hChild = GetNextItem(hChild, TVGN_NEXT);
		}

		int nState;

		ASSERT(nTotal > 0);
		if (nTotal == nChecked)
			nState = CHECKED;
		else if (nTotal == nUnchecked)
			nState = UNCHECKED;
		else
			nState = m_bAutoSynchronize ? INDETERMINATE : CHECKED;

		SetItemState(hParent, INDEXTOSTATEIMAGEMASK(nState + 1), TVIS_STATEIMAGEMASK);

		CheckParent(hParent);
	}

	return FALSE;
}

BOOL CCheckBoxTreeCtrl::OnPreSetCheck(HTREEITEM hItem, int nState)
{
	return TRUE;
}

void CCheckBoxTreeCtrl::OnPostSetCheck(HTREEITEM hItem, int nState)
{
}

BOOL CCheckBoxTreeCtrl::SetCheck(HTREEITEM hItem, int nState)
{
	ASSERT(nState >= UNCHECKED && nState <= INDETERMINATE);

	if (!OnPreSetCheck(hItem, nState))
		return FALSE;

	if (SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState + 1), TVIS_STATEIMAGEMASK))
	{
		if (m_bAutoSynchronize)
		{
			//SetRedraw(FALSE);

			CheckChildren(hItem);
			CheckParent(hItem);

			//SetRedraw();
		}

		OnPostSetCheck(hItem, nState);

		return TRUE;
	}

	return FALSE;
}

BOOL CCheckBoxTreeCtrl::ToggleCheck(HTREEITEM hItem)
{
	int nState = GetCheck(hItem);

	if (nState == CHECKED)
		nState = UNCHECKED;
	else
		nState = CHECKED;

	return SetCheck(hItem, nState);
}

LRESULT CCheckBoxTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (m_bCheckBox)
	{
		if (message == TVM_INSERTITEM)
		{

			HTREEITEM hItem = (HTREEITEM)CTreeCtrl::WindowProc(message, wParam, lParam);
			if (hItem)
				SetItemState(hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);

			return (LRESULT)hItem;
		}
		else if (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK)
		{
			CPoint point(LOWORD(lParam), HIWORD(lParam));

			UINT uFlags=0;
			HTREEITEM hItem = HitTest(point,&uFlags);

			if( uFlags & TVHT_ONITEMSTATEICON )
			{
				ToggleCheck(hItem);
				return TRUE;
			}
		}
	}

	return CTreeCtrl::WindowProc(message, wParam, lParam);
}

int CCheckBoxTreeCtrl::GetCheck(HTREEITEM hItem)
{
	int iImage = GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12;
	return --iImage;
}

void CCheckBoxTreeCtrl::EnableCheckBox(BOOL bEnable)
{
	m_bCheckBox = bEnable;
}

BOOL CCheckBoxTreeCtrl::IsParentItem(HTREEITEM hParent, HTREEITEM hChild)
{
	HTREEITEM hItem = GetParentItem(hChild);
	while (hItem != NULL)
	{
		if (hItem == hParent)
			return TRUE;
		hItem = GetParentItem(hItem);
	}
	return FALSE;
}

void CCheckBoxTreeCtrl::EnableAutoSynchronize(BOOL bEnable)
{
	m_bAutoSynchronize = bEnable;
}
