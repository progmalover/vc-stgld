#if !defined(AFX_DLGABOUT_H__76010823_5EF0_11D5_9A0D_0080C82BC2DE__INCLUDED_)
#define AFX_DLGABOUT_H__76010823_5EF0_11D5_9A0D_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAbout.h : header file
//

#include "StaticLink.h"
#include "StaticTip.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout dialog

class CDlgAbout : public CDialog
{
// Construction
public:
	CDlgAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAbout)
	enum { IDD = IDD_ABOUTBOX };
	CStaticText	m_stcProduct;
	CStaticLink		m_stcWebsite;
	CStaticLink		m_stcSupport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAbout)
	virtual BOOL OnInitDialog();
	afx_msg void OnSupport();
	afx_msg void OnWebsite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGABOUT_H__76010823_5EF0_11D5_9A0D_0080C82BC2DE__INCLUDED_)
