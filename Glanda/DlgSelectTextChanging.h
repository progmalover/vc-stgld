#pragma once


// CDlgSelectTextChanging dialog
#include <list>

class CDlgSelectTextChanging : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectTextChanging)

public:
	CDlgSelectTextChanging(std::list<CString> *texts, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectTextChanging();
	inline CString &GetSelText() { return m_strText; }

// Dialog Data
	enum { IDD = IDD_SELECT_TEXT_CHANGING };

protected:
	std::list<CString> *m_Texts;
	CListBox m_lstTexts;
	CString m_strText;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
public:
	afx_msg void OnLbnSelchangeListTexts();
};
