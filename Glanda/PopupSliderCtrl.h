#if !defined(AFX_POPUPSLIDERCTRL_H__98E340D6_6B69_11D5_9A1B_0080C82BC2DE__INCLUDED_)
#define AFX_POPUPSLIDERCTRL_H__98E340D6_6B69_11D5_9A1B_0080C82BC2DE__INCLUDED_

#include "TrayWnd.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PopupSliderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPopupSliderCtrl window

#define LEFTRIGHT		1
#define BOTTOMTOP		2
#define RIGHTLEFT		3
#define TOPBOTTOM		4

class CPopupSliderCtrl : public CSliderCtrl
{
// Construction
public:
	CPopupSliderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopupSliderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Activate(CPoint point);
	BOOL Create(BOOL bHorz, int nDirection);
	void SetBuddyWindow(HWND hWnd);
	virtual ~CPopupSliderCtrl();

	// Generated message map functions
protected:
	CTrayWnd m_wndTray;
	int m_nDirection;
	int GetPosImpl();
	void SetPosImpl(int nPos);
	HWND m_hBuddy;
	BOOL m_bFirstMove;
	//{{AFX_MSG(CPopupSliderCtrl)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPUPSLIDERCTRL_H__98E340D6_6B69_11D5_9A1B_0080C82BC2DE__INCLUDED_)
