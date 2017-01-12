#pragma once


// COptionsPageAS dialog

class COptionsPageAS : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPageAS)

public:
	COptionsPageAS();
	virtual ~COptionsPageAS();

// Dialog Data
	enum { IDD = IDD_OPTIONS_AS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckShowSelectionMargin();

protected:
	void UpdateControls();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
};
