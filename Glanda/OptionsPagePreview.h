#pragma once
#include "afxwin.h"


// COptionsPagePreview dialog

class COptionsPagePreview : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPagePreview)

public:
	COptionsPagePreview();
	virtual ~COptionsPagePreview();

// Dialog Data
	enum { IDD = IDD_OPTIONS_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnLbnSelchangeList();
	CListBox m_list;
	virtual BOOL OnInitDialog();
	void UpdateControls();
	virtual void OnOK();
};
