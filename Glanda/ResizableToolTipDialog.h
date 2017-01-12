#pragma once

#include "resizabledialog.h"
#include <string>
#include <map>

// CResizableToolTipDialog dialog

class CResizableToolTipDialog : public CResizableDialog
{
	typedef std::map<HWND, std::string> MAP_TOOLTIP;

public:
	CResizableToolTipDialog(UINT nIDD, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResizableToolTipDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	MAP_TOOLTIP m_mapTips;

	void AddToolTip(UINT nIDCtrl, UINT nIDString = 0);
	void AddToolTip(UINT nIDCtrl, LPCTSTR lpszString);
	void AddToolTip(CWnd *pWnd, UINT nIDString = 0);
	void AddToolTip(CWnd *pWnd, LPCTSTR lpszString);

	BOOL GetToolTipText(HWND hWnd, CString &strTip);
	BOOL OnToolTipText(UINT nID, NMHDR * pNMHDR, LRESULT * pResult);

	virtual BOOL OnInitDialog();
};
