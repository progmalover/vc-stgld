#pragma once

#include <list>

typedef std::list <HTREEITEM> item_list;

enum SELECTTYPE
{
	typeNone,
	typeNormal,
	typeControl,
	typeShift
};

enum SCROLLDIR
{
	dirNone,
	dirUp,
	dirDown
};

enum DROPPOS
{
	dropNone,
	dropBefore,
	dropInner,
	dropAfter
};

/////////////////////////////////////////////////////////////////////////////
// CMulSelTreeCtrl window

class CMulSelTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CMulSelTreeCtrl();
	virtual ~CMulSelTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMulSelTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL DeleteAllItems();
	void EnableSelChange(BOOL bEnable);
	virtual void OnSelChanged();
	HTREEITEM FindItem(HTREEITEM hParent, LPCTSTR lpszText, BOOL bMatchWholeWord, BOOL bRegExp, BOOL bExpandedOnly, BOOL bRecursive, HTREEITEM hBelowItem);
	BOOL IsParentItem(HTREEITEM hParent, HTREEITEM hChild);
	int ComparePosition(HTREEITEM hItem1, HTREEITEM hItem2);
	BOOL IsDragging();
	void SetExtDragDrop(BOOL bExt);
	item_list & GetSelectedItems();
	int GetItemIndent(HTREEITEM hItem);
	virtual void BeginEditLabel(HTREEITEM hItem);
	virtual BOOL SortChildrenEx(HTREEITEM hItem, BOOL bRecursive);
	BOOL UnselectChildItems(HTREEITEM hItem, BOOL bRemoveFromList);
	BOOL UnselectItem(HTREEITEM hItem, BOOL bRemoveFromList);
	BOOL MoveItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop, HTREEITEM hItemInsertAfter, BOOL bCopy);
	BOOL UnselectAllItems();
	HTREEITEM GetLastVisibleItem();
	virtual BOOL CanDrop(HTREEITEM hItem, DROPPOS nDropPos);
	BOOL IsParentItemSelected(HTREEITEM hItem);
	HTREEITEM GetFirstSelectedItem();
	HTREEITEM GetCaretItem();
	BOOL SetCaretItem(HTREEITEM hItem);
	BOOL IsItemSelected(HTREEITEM hItem);
	BOOL SelectAllItems();
	BOOL SelectChildItems(HTREEITEM hItem);
	BOOL SelectItem(HTREEITEM hItem);
	int GetChildItemCount(HTREEITEM hItem);
	HTREEITEM GetLastInsertedItem() {return m_hItemInserted;}
	void EnableDragDrop(BOOL bEnable) {m_bEnableDragDrop = bEnable;}
	void EnableDragDropCopy(BOOL bEnable)
	{
		m_bEnableDragDropCopy = bEnable;
	}

	enum
	{
		TEXT_PADDING = 2
	};

	// Generated message map functions
protected:
	virtual BOOL CanDropOuter(CPoint point);
	virtual BOOL OnDropOuter(CPoint point);
	afx_msg LRESULT OnInsertItem(WPARAM wp, LPARAM lp);
	BOOL DeleteItemRecursive(HTREEITEM hItem);
	void SetCursor(const CPoint &point);
	void CancelDelayEditLabel();
	void DrawSeprator(CDC *pDC, CRect &rc);
	virtual BOOL PreDeleteItem(HTREEITEM hItem);
	void DelayEditLabel(HTREEITEM hItem);
	HTREEITEM m_hItemEditLabel;
	virtual void PostMoveItem(HTREEITEM hItemOld, HTREEITEM hItemNew, BOOL bCopy, BOOL bReplace);
	virtual BOOL PreMoveItem(HTREEITEM hItemFrom, HTREEITEM hItemTo, BOOL &bSkipMoving, BOOL &bReplace);
	virtual BOOL OnDrop(HTREEITEM hItemDrop, DROPPOS nDropPos);
	virtual void PreDrop(HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL bCopy);
	virtual void PostDrop(HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL bCopy);
	void CancelDelayScroll();
	void DelayScroll(HTREEITEM hItem);
	void RedrawAllSelectedItems();
	void RedrawItem(HTREEITEM hItem, BOOL bTextOnly);
	void CancelDelayExpand();
	void DelayExpand(HTREEITEM hItem);
	void SetDropTarget(HTREEITEM hItem, DROPPOS nDropPos);
	void EndDrag(BOOL bDrop, CPoint point);
	void BeginDrag();
	BOOL GetChildItems(HTREEITEM hItem, item_list &items);
	//{{AFX_MSG(CMulSelTreeCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bDisableSelChange;
	CString m_strCharInput;
	BOOL m_bExtDragDrop;
	int m_nIndent;
	item_list m_listItemsToSelect;
	item_list m_listInsertedItems;
	item_list m_listMovedItems;
	item_list m_listSelectedItems;
	HTREEITEM m_hItemDelayEditLabel;
	BOOL m_bFocusedBeforeLButtonDown;
	BOOL m_bItemSelectedBeforeLButtonDown;
	CPoint m_ptLButtonDown;
	BOOL m_bDragging;
	BOOL m_bLButtonDown;
	HTREEITEM m_hItemDelayScroll;
	HTREEITEM m_hItemDelayExpand;
	SELECTTYPE m_nSelectType;
	DROPPOS m_nDropPos;
	HTREEITEM m_hItemDrop;
	HTREEITEM m_hItemCaret;
	SCROLLDIR m_nScrollDir;
		
	HTREEITEM m_hItemInserted;
	BOOL m_bEnableDragDrop;
	BOOL m_bEnableDragDropCopy;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
