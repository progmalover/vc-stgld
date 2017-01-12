#pragma once


// CDlgComment dialog

class CDlgComment : public CDialog
{
	DECLARE_DYNAMIC(CDlgComment)

public:
	CDlgComment(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgComment();

// Dialog Data
	enum { IDD = IDD_COMMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strComment;
};
