#if !defined(AFX_TOOLBARBUTTON_H__C135F525_07A1_11D5_9722_0080C82BC2DE__INCLUDED_)
#define AFX_TOOLBARBUTTON_H__C135F525_07A1_11D5_9722_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolBarButton.h : header file
//

#include "FlatBitmapButton.h"

/////////////////////////////////////////////////////////////////////////////
// CToolBarButton window

class CToolBarButton : public CFlatBitmapButton
{
// Construction
public:
	CToolBarButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBarButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolBarButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolBarButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBARBUTTON_H__C135F525_07A1_11D5_9722_0080C82BC2DE__INCLUDED_)
