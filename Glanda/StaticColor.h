#if !defined(AFX_STATICCOLOR_H__CFB99A37_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_)
#define AFX_STATICCOLOR_H__CFB99A37_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticColor window

class CStaticColor : public CStatic
{
// Construction
public:
	CStaticColor();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticColor)
	//}}AFX_VIRTUAL

// Implementation
public:
	COLORREF GetColor();
	void SetColor(COLORREF color);
	virtual ~CStaticColor();

	// Generated message map functions
protected:
	COLORREF m_Color;
	//{{AFX_MSG(CStaticColor)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnable(BOOL bEnable);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICCOLOR_H__CFB99A37_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_)
