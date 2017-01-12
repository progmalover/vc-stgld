#pragma once
#include "statictip.h"


// COptionsPageGeneral dialog

class COptionsPageGeneral : public CPropertyPage
{
public:
	COptionsPageGeneral();
	virtual ~COptionsPageGeneral();

// Dialog Data
	enum { IDD = IDD_OPTIONS_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonResetAllTips();
	afx_msg void OnBnClickedButtonEmptyCache();
};
