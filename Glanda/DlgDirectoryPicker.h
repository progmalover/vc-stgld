#if !defined(AFX_DLGDIRECTORYPICKER_H__EB7B4D04_3073_4C31_ABA9_B481ACE174D9__INCLUDED_)
#define AFX_DLGDIRECTORYPICKER_H__EB7B4D04_3073_4C31_ABA9_B481ACE174D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDirectoryPicker.h : header file
//

#include "ShellTree.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgDirectoryPicker dialog

class CDlgDirectoryPicker : public CDialog
{
// Construction
public:
	BOOL SetPath(LPCTSTR lpszPath);
	CString GetPath();
	CDlgDirectoryPicker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDirectoryPicker)
	enum { IDD = IDD_DIRECTORY_PICKER };
	CShellTree	m_wndTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDirectoryPicker)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strPath;
	BOOL m_bPopulating;

	// Generated message map functions
	//{{AFX_MSG(CDlgDirectoryPicker)
	afx_msg void OnNewFolder();
	afx_msg void OnLocate();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnChangeEditPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIRECTORYPICKER_H__EB7B4D04_3073_4C31_ABA9_B481ACE174D9__INCLUDED_)
