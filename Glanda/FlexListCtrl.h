#pragma once

#include <vector>

#define FLCS_MULTISEL			(1 << 0)
#define FLCS_HOTTRACK			(1 << 1)
#define FLCS_GRID				(1 << 2)
#define FLCS_DISABLENOSCROLL	(1 << 3)

#define FLCN_SELCHANGED			1
#define FLCN_HOVERCHANGED		2
#define FLCN_DBLCLK				3


// CFlexListCtrl

class CFlexListCtrl : public CWnd
{
protected:
	class CListItem
	{
	public:
		CListItem()
		{
			dwData = 0;
		};

		DWORD_PTR dwData;	// User data
	};
	typedef std::vector<CFlexListCtrl::CListItem *> ITEM_LIST;
	typedef std::vector<int> SEL_LIST;

	DWORD m_dwStyle;

	ITEM_LIST m_items;
	SEL_LIST m_sel;

	CSize m_sizeItem;
	CSize m_sizeSpace;
	CRect m_rcMargin;

	int m_nHover;
	int m_nFocus;
	int m_nLastSelected;

	CToolTipCtrl m_tooltip;

	COLORREF m_crBack;
	BOOL m_bSysColorBack;


public:
	CFlexListCtrl();
	virtual ~CFlexListCtrl();

public:
	void ModifyFLStyle(DWORD dwRemove, DWORD dwAdd);
	int InsertItem(int index, DWORD_PTR dwData);
	void DeleteItem(int index, BOOL bNotify = true);
	void DeleteAllItems();
	int ItemFromPoint(CPoint point);

	int GetColCount();
	int GetRowCount();
	int GetFirstVisibleRow();
	int GetVisibleRows();

	void SelectItem(int index, BOOL bNotify = FALSE);
	void UnselectItem(int index);
	void UnselectAllItems();
	void SetFocusItem(int index);

	void GetItemRect(int index, RECT *pRect);
	void RedrawItem(int index);

	BOOL IsItemSelected(int index);
	int GetCurSel();
	int GetFocusItem() {return m_nFocus;}
	int GetHoverItem() {return m_nHover;}

	void SetItemData(int index, DWORD dwData);
	DWORD_PTR GetItemData(int index);
	int GetItemCount() {return (int)m_items.size();}

	void SetMargin(int l, int t, int r, int b);
	void SetItemSpace(int cx, int cy);
	void SetItemSize(int cx, int cy);

	CPoint GetItemPos(int index);
	int GetItemIndex(int nRow, int nCol);
	void EnsureVisible(int index);

protected:
	void RecalcScrollPos();
	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);
	virtual void OnDeleteItem(int index);
	virtual BOOL GetToolTipText(int index, CString &strText);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	void SetBkColor(COLORREF crBack, BOOL bSysColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


protected:
	void DelayEnsureVisible(int index);
	void CancelDelayEnsureVisible();
};
