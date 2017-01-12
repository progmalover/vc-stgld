#if !defined(AFX_PROGRESSSTATUSBAR_H__9A206822_24BB_4358_B49D_26BDE5669E1E__INCLUDED_)
#define AFX_PROGRESSSTATUSBAR_H__9A206822_24BB_4358_B49D_26BDE5669E1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressStatusBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressStatusBar window

class CProgressStatusBar : public CStatusBar
{
// Construction
public:
	CProgressStatusBar();

	DECLARE_SINGLETON(CProgressStatusBar)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressStatusBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetPos(int nPos);
	void StepIt();
	void EndProgress();
	void BeginProgress();
	virtual ~CProgressStatusBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgressStatusBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bRunning;
	CProgressCtrl m_wndProgress;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSSTATUSBAR_H__9A206822_24BB_4358_B49D_26BDE5669E1E__INCLUDED_)
