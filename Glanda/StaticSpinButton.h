#if !defined(AFX_STATICSPINBUTTON_H__CFB99A36_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_)
#define AFX_STATICSPINBUTTON_H__CFB99A36_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_

#include "PopupSliderCtrl.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticSpinButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticSpinButton window

class CStaticSpinButton : public CStatic
{
// Construction
public:
	CStaticSpinButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSpinButton)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSliderStyle(BOOL bHorz, int nDirection);
	void SetBuddyWindow(HWND hWnd);
	void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE);
	virtual ~CStaticSpinButton();

	// Generated message map functions
protected:
	int m_nDirection;
	BOOL m_bHorz;
	CPopupSliderCtrl m_wndSlider;
	//{{AFX_MSG(CStaticSpinButton)
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICSPINBUTTON_H__CFB99A36_6C2A_11D5_9A1C_0080C82BC2DE__INCLUDED_)
