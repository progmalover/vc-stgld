#if !defined(AFX_BORINGBAR_H__C148FE53_3B75_11D5_99E9_0080C82BC2DE__INCLUDED_)
#define AFX_BORINGBAR_H__C148FE53_3B75_11D5_99E9_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BoringBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaneBar window

class CPaneBar : public CControlBar
{
// Construction
private:
	CPaneBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaneBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	BOOL Create(CWnd *pParentWnd, LPCTSTR pszTitle, UINT nID, DWORD dwStyle);
	virtual ~CPaneBar();

	DECLARE_SINGLETON(CPaneBar)

// Generated message map functions
protected:
	virtual void OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler);
	//{{AFX_MSG(CPaneBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	void ShowText(LPCTSTR lpszText);

private:
	CString m_strText;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BORINGBAR_H__C148FE53_3B75_11D5_99E9_0080C82BC2DE__INCLUDED_)
