#pragma once

#include "GlandaSizingControlBar.h"

class COutputView;

// COutputBar

class COutputBar : public CGlandaSizingControlBar
{
	DECLARE_DYNAMIC(COutputBar)

public:
	COutputBar();
	virtual ~COutputBar();
	COutputView *m_pView;

	DECLARE_SINGLETON(COutputBar)

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnShow(WPARAM wp, LPARAM lp);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void Show(BOOL bShow);
protected:
	virtual void PaintClient(CDC & dc);
	void GetToolBarRect(CRect * pRect);
};
