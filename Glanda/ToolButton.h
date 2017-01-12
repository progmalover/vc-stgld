#pragma once

#include "FlatBitmapButton.h"

class CToolButtonPopup;

#define TOOL_BUTTON_CTLID_BASE 1000
#define BN_EXPAND	1000

// CToolButton

class CToolButton : public CFlatBitmapButton
{
public:
	CToolButton();
	virtual ~CToolButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);

public:
	void AddTool(UINT nID, UINT nIDBitmap, UINT nStringID);
	void SetCurrentTool(int index);
	BOOL HasSubTools() {return m_pPopup != NULL;}
	BOOL GetToolTip(CString &strTip);

protected:
	CToolButtonPopup *m_pPopup;
public:
	afx_msg void OnDestroy();
};


