#pragma once

#define WM_HIDELISTBOX	(WM_USER + 1000)

// CUndoListBox

class CUndoListBox : public CListBox
{
	DECLARE_DYNAMIC(CUndoListBox)

public:
	CUndoListBox();
	virtual ~CUndoListBox();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void LoadList(BOOL bUndo);
	BOOL TrackMessage();

protected:
	int m_nSelIndex;
	int ItemFromPointExact(CPoint &point);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void DrawItem(CDC *pDC, int index, CRect &rc);
	BOOL IsRectVisible(CRect &rcClient, CRect &rcItem);
	BOOL PtInWindow(POINT point);

public:
	afx_msg void OnSelChange();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
