#if !defined(AFX_DLGABOUTLAME_H__4331B403_9DE9_11D5_9A4C_0080C82BC2DE__INCLUDED_)
#define AFX_DLGABOUTLAME_H__4331B403_9DE9_11D5_9A4C_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAboutLame.h : header file
//

#include "StaticLink.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAboutLame dialog

class CDlgAboutLame : public CDialog
{
// Construction
public:
	CDlgAboutLame(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAboutLame)
	enum { IDD = IDD_ABOUT_LAME };
	CStaticLink	m_stcLink;
	CString	m_strLicense;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAboutLame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAboutLame)
	afx_msg void OnUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGABOUTLAME_H__4331B403_9DE9_11D5_9A4C_0080C82BC2DE__INCLUDED_)
