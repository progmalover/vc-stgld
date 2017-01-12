#if !defined(AFX_STATICLINK_H__FD87C893_2E07_11D3_B8D5_204C4F4F5020__INCLUDED_)
#define AFX_STATICLINK_H__FD87C893_2E07_11D3_B8D5_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticLink.h : header file
//

#include "StaticText.h"

/////////////////////////////////////////////////////////////////////////////
// CStaticLink window

class CStaticLink : public CStaticText
{
// Construction
public:
	CStaticLink();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLink)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetHoverCursor(HCURSOR hCursor);
	virtual ~CStaticLink();

	// Generated message map functions
protected:
	virtual BOOL Init();
	virtual void OnChangeState(BOOL bHover);
	BOOL m_bMouseCaptured;
	COLORREF m_crNormal;
	COLORREF m_crHover;
	CFont m_Font;
	HCURSOR m_hCursor;
	BOOL m_bHover;
	//{{AFX_MSG(CStaticLink)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICLINK_H__FD87C893_2E07_11D3_B8D5_204C4F4F5020__INCLUDED_)
