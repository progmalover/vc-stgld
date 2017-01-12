#if !defined(AFX_FLATBUTTON_H__D4C15FB5_E5CE_11D2_80B1_204C4F4F5020__INCLUDED_)
#define AFX_FLATBUTTON_H__D4C15FB5_E5CE_11D2_80B1_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlatButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlatButton window

//
// Button state
//
#define STATE_FLAT			0x00000001L
#define	STATE_UP			0x00000002L
#define STATE_DOWN			0x00000003L

//
// Button style
//

#define FBS_FLAT			0x00000001L	<< 0		// Default style
#define FBS_CLIENTEDGE		0x00000001L << 1
#define FBS_STATICEDGE		0x00000001L	<< 2	
#define FBS_TRACKBORDER		0x00000001L	<< 3		// Draw 1 pixel border aound the button?
#define FBS_THICKBORDER		0x00000001L	<< 4		// Draw 2 pixels border?
#define FBS_NOBORDER		0x00000001L << 5

#define FBS_LEFTALIGNTEXT	0x00000001L << 16
#define FBS_CENTERALIGNTEXT	0x00000001L << 17
#define FBS_RIGHTALIGNTEXT	0x00000001L << 18

#define FBS_RIGHTTEXT		0x00000001L << 20
#define FBS_BOTTOMTEXT		0x00000001L << 21

#define FBS_CHECK			0x00000001L << 22
#define FBS_DROPDOWN		0x00000001L << 23
#define FBS_DROPDOWN2		0x00000001L << 24

#define FBS_TRANSPARENT		0x00000001L << 25

#define FBS_NOXPSTYLE		0x00000001L << 26

#define FACE_COLOR_MODE_AUTO				0
#define FACE_COLOR_MODE_SYSCOLOR			1
#define FACE_COLOR_MODE_RGB					2

class CFlatButton : public CButton
{
// Construction
public:
	CFlatButton();
	void SetFBState(DWORD iNewState);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetHover(BOOL bHover);
	int GetCheck();
	void SetCheck(int nCheck);
	void ModifyFBStyle(DWORD dwRemove, DWORD dwAdd);
	DWORD GetFBState();
	DWORD GetFBStyle();
	void SetFBStyle(DWORD dwStyle);
	virtual ~CFlatButton();

	// Generated message map functions
protected:
	void DrawBackground(CDC *pDC, const CRect &rc);
	virtual void DrawStuff(CDC *pDC, const CRect &rc);
	virtual CSize GetStuffSize();
	BOOL m_bHover;
	BOOL m_bChecked;
	DWORD m_dwStyle;
	BOOL m_bTimerSet;
	BOOL m_bLButtonDown;
	DWORD m_dwState;

	//{{AFX_MSG(CFlatButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SetFaceColor(int nFaceColorMode, COLORREF color, BOOL bRedraw = TRUE);
protected:
	int m_nFaceColorMode;
	COLORREF m_crFace;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATBUTTON_H__D4C15FB5_E5CE_11D2_80B1_204C4F4F5020__INCLUDED_)
