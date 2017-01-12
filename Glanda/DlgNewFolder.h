#if !defined(AFX_DLGNEWFOLDER_H__AF25181F_F2D5_4E83_AB69_E73CFBDD02D8__INCLUDED_)
#define AFX_DLGNEWFOLDER_H__AF25181F_F2D5_4E83_AB69_E73CFBDD02D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewFolder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewFolder dialog

class CDlgNewFolder : public CDialog
{
// Construction
public:
	CDlgNewFolder(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewFolder)
	enum { IDD = IDD_NEW_FOLDER };
	CString	m_strCurrentFolder;
	CString	m_strNewFolder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewFolder)
	virtual void OnOK();
	afx_msg void OnChangeEditNewFolder();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWFOLDER_H__AF25181F_F2D5_4E83_AB69_E73CFBDD02D8__INCLUDED_)
