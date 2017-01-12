#pragma once

#include "GlandaSizingControlBar.h"

// CSceneListBar

class CSceneListBar : public CGlandaSizingControlBar
{
	DECLARE_DYNAMIC(CSceneListBar)

public:
	CSceneListBar();
	virtual ~CSceneListBar();

	DECLARE_SINGLETON(CSceneListBar)

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSceneListSelChanged();
protected:
	virtual void PaintClient(CDC & dc);
	void GetToolBarRect(CRect * pRect);
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};


