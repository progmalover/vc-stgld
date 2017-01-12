#if !defined(AFX_DLGWEBBROWSER_H__A80E94C6_8DC4_11D2_8CA5_0088CC081531__INCLUDED_)
#define AFX_DLGWEBBROWSER_H__A80E94C6_8DC4_11D2_8CA5_0088CC081531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWebBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWebBrowser dialog
#include "Browsers.h"

class CDlgWebBrowser : public CDialog
{
// Construction
public:
	CDlgWebBrowser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWebBrowser)
	enum { IDD = IDD_BROWSERS };
	CListBox	m_lstWellknown;
	CListBox	m_lstCustomized;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWebBrowser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWebBrowser)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnSelChangeListCustomized();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWEBBROWSER_H__A80E94C6_8DC4_11D2_8CA5_0088CC081531__INCLUDED_)
