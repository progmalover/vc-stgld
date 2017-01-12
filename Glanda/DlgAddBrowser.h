#if !defined(AFX_DLGADDBROWSER_H__EEBABED6_565E_11D5_9A04_0080C82BC2DE__INCLUDED_)
#define AFX_DLGADDBROWSER_H__EEBABED6_565E_11D5_9A04_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddBrowser dialog

class CDlgAddBrowser : public CDialog
{
// Construction
public:
	CDlgAddBrowser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddBrowser)
	enum { IDD = IDD_BROWSER_ADD };
	CString	m_strName;
	CString	m_strFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddBrowser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddBrowser)
	afx_msg void OnLocate();
	virtual void OnOK();
	afx_msg void OnChangeNameFile();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDBROWSER_H__EEBABED6_565E_11D5_9A04_0080C82BC2DE__INCLUDED_)
