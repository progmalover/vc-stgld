#if !defined(AFX_CHECKMESSAGEBOX_H__36D6A09F_DFCD_4619_BD43_3D7F524B5F9C__INCLUDED_)
#define AFX_CHECKMESSAGEBOX_H__36D6A09F_DFCD_4619_BD43_3D7F524B5F9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckMessageBox.h : header file
//

UINT CheckMessageBox(LPCTSTR lpszText, LPCTSTR lpszCheckText, BOOL &bChecked, UINT nType = MB_ICONINFORMATION | MB_OK, LPCTSTR lpszCaption = NULL, CWnd* pParent = NULL);
UINT CheckMessageBox(UINT nIDText, UINT nIDCheckText, BOOL &bChecked, UINT nType = MB_ICONINFORMATION | MB_OK, UINT nIDCaption = 0, CWnd* pParent = NULL);

/////////////////////////////////////////////////////////////////////////////
// CCheckMessageBox dialog

class CCheckMessageBox : public CDialog
{
protected:
	struct CButtonInfo
	{
		UINT nID;
		CButton button;
	};

// Construction
protected:
	CCheckMessageBox(LPCTSTR lpszCaption, LPCTSTR lpszText, LPCTSTR lpszCheckText, BOOL bChecked = FALSE, UINT nType = MB_ICONINFORMATION | MB_OK, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCheckMessageBox)
	enum { IDD = IDD_CHECK_MESSAGE_BOX };
	CStatic	m_stcIcon;
	CString	m_strText;
	BOOL	m_bChecked;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckMessageBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RecalcLayout();
	afx_msg void OnButtonClick(UINT nID);
	int m_cButtons;
	CButtonInfo m_buttons[3];
	CString m_strCaption;
	CString m_strCheckText;
	UINT m_nType;

	// Generated message map functions
	//{{AFX_MSG(CCheckMessageBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend UINT CheckMessageBox(LPCTSTR lpszText, LPCTSTR lpszCheckText, BOOL &bChecked, UINT nType, LPCTSTR lpszCaption, CWnd* pParent);
	friend UINT CheckMessageBox(UINT nIDText, UINT nIDCheckText, BOOL &bChecked, UINT nType, UINT nIDCaption, CWnd* pParent);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKMESSAGEBOX_H__36D6A09F_DFCD_4619_BD43_3D7F524B5F9C__INCLUDED_)
