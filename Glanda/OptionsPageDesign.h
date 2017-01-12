#pragma once


// COptionsPageDesign dialog

class COptionsPageDesign : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPageDesign)

public:
	COptionsPageDesign();
	virtual ~COptionsPageDesign();

// Dialog Data
	enum { IDD = IDD_OPTIONS_DESIGN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckShowCanvasTip();

protected:
	void UpdateControls();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
};
