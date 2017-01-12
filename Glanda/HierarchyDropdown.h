#pragma once


// CHierarchyDropdown
#include "HierarchyPopup.h"

#define HN_SEL_CHANGED 1000

class CHierarchyDropdown : public CWnd
{
	DECLARE_DYNAMIC(CHierarchyDropdown)

private:	
	int _cur_sel;
	CHierarchyPopup _popup;
	int _cx_drop;
	int _cy_drop;

private:
	CRect GetDropButtonRect() const;
	CSize GetDefaultDropSize() const;
	BOOL IsDropdown() const;
	void Popup();

public:
	CHierarchyDropdown();
	virtual ~CHierarchyDropdown();	

	int AddString(const CString &text, int level = 0, int icon = -1, BOOL disabled = FALSE, LPARAM lParam = 0)
	{ return _popup.AddItem(text, level, icon, disabled, lParam); }
	CImageList *SetImageList(CImageList *pIL)
	{ return _popup.SetImageList(pIL); }	
	void SetCurSel(int item);
	int GetCurSel() const { return _cur_sel; }
	void SetItemData(int item, LPARAM data) { _popup.SetItemData(item, data); }
	LPARAM GetItemData(int item) const { return _popup.GetItemData(item); }
	
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNcPaint();
};