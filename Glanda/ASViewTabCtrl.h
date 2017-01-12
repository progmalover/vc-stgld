#pragma once

#include "CuteTabCtrl.h"

// CASViewTabCtrl

class CASView;

class CASViewTabCtrl : public CCuteTabCtrl
{
public:
	CASViewTabCtrl();
	virtual ~CASViewTabCtrl();

	int AddView(CASView *pView, LPCTSTR lpszCaption);
	CASView *GetView(int index);
	CASView *GetActiveView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	virtual BOOL OnActivatePage(int nItemOld, int nItemNew);
	CFont m_font;

	virtual HFONT GetFontHandle(void)
	{
		return m_font;
	}

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


