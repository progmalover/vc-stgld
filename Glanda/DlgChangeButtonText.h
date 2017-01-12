#pragma once


// CDlgChangeButtonText dialog

class CDlgChangeButtonText : public CDialog
{
	DECLARE_DYNAMIC(CDlgChangeButtonText)

public:
	CDlgChangeButtonText(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChangeButtonText();

// Dialog Data
	enum { IDD = IDD_CHANGE_BUTTON_TEXT };

	CString m_strText;
	int m_Method;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
