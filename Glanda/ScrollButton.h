// CJButton.h : header file
//
// Owner drawn button control.
//
// Copyright © 1998 Written by Kirk Stowell   
//		mailto:kstowel@sprynet.com
//		http://www.geocities.com/SiliconValley/Haven/8230
//
// This code may be used in compiled form in any way you desire. This  
// file may be redistributed unmodified by any means PROVIDING it is   
// not sold for profit without the authors written consent, and   
// providing that this notice and the authors name and all copyright   
// notices remains intact. If the source code in this file is used in   
// any  commercial application then a statement along the lines of   
// "Portions Copyright © 1998 Kirk Stowell" must be included in   
// the startup banner, "About" box or printed documentation. An email   
// letting me know that you are using it would be nice as well. That's   
// not much to ask considering the amount of work that went into this.  
//  
// This file is provided "as is" with no expressed or implied warranty.  
// The author accepts no liability for any damage/loss of business that  
// this product may cause.  
//
// ==========================================================================
// HISTORY:	
// ==========================================================================
//			1.00	17 Oct 1998	- Initial re-write and release.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(SCROLLBUTTON_H_INCLUDED)
#define SCROLLBUTTON_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define DIR_LEFT		1
#define DIR_RIGHT		2

/////////////////////////////////////////////////////////////////////////////
// CScrollButton class

//##ModelId=3B2040AA00D1
class CScrollButton : public CButton
{
	DECLARE_DYNAMIC(CScrollButton)

// Construction
public:
	//##ModelId=3B2040AA0106
	CScrollButton();

// Attributes
protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollButton)
	public:
	//##ModelId=3B2040AA0103
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3B2040AA00E6
	int m_iDirection;
//DEL 	void DisableFlatLook() { m_bFlatLook = false; }
	//##ModelId=3B2040AA00FC
	virtual ~CScrollButton();
// Generated message map functions
protected:
	//{{AFX_MSG(CScrollButton)
	//##ModelId=3B2040AA00F9
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//##ModelId=3B2040AA00EF
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(SCROLLBUTTON_H_INCLUDED)

