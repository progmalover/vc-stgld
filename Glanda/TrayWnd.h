#if !defined(AFX_TRAYWND_H__EA0F9CD7_8571_11D5_9A37_0080C82BC2DE__INCLUDED_)
#define AFX_TRAYWND_H__EA0F9CD7_8571_11D5_9A37_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrayWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrayWnd window

class CTrayWnd : public CWnd
{
// Construction
public:
	CTrayWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CRect rc);
	virtual ~CTrayWnd();

	void SetChild(CWnd *pWnd);
	void SetPadding(int nPadding);
	int GetPadding() {return m_nPadding;}

private:
	CWnd * m_pChild;
	int m_nPadding;
	void RecalcLayout();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrayWnd)
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAYWND_H__EA0F9CD7_8571_11D5_9A37_0080C82BC2DE__INCLUDED_)
