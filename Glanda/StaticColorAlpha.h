#if !defined(AFX_STATICCOLORALPHA_H__CFB99A38_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_)
#define AFX_STATICCOLORALPHA_H__CFB99A38_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticColorAlpha.h : header file
//

#include "StaticColor.h"

/////////////////////////////////////////////////////////////////////////////
// CStaticColorAlpha window

class CStaticColorAlpha : public CStaticColor
{
// Construction
public:
	CStaticColorAlpha();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticColorAlpha)
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetAlpha();
	void SetAlpha(int alpha);
	virtual ~CStaticColorAlpha();

	// Generated message map functions
protected:
	int m_Alpha;
	//{{AFX_MSG(CStaticColorAlpha)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICCOLORALPHA_H__CFB99A38_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_)
