#pragma once

#include "ResizableToolTipDialog.h"

typedef std::map<HWND, std::string> MAP_TOOLTIP;

// CDlgToolOption dialog

class CDlgToolOption : public CResizableToolTipDialog
{
public:
	CDlgToolOption(UINT nIDD, CWnd* pParent);   // standard constructor
	virtual ~CDlgToolOption();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual void UpdateControls() = 0;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
