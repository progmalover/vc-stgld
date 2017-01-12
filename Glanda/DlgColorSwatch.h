#if !defined(AFX_DLGCOLORSWATCH_H__ABAE2AC7_862B_11D5_9A38_0080C82BC2DE__INCLUDED_)
#define AFX_DLGCOLORSWATCH_H__ABAE2AC7_862B_11D5_9A38_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColorSwatch.h : header file
//

#include "ColorSwatch.h"
#include "StaticColorAlpha.h"
#include "StaticPasvButton.h"
#include "Resource.h"

extern UINT WM_COLORCHANGE;
extern UINT WM_COLORADVANCED;


/////////////////////////////////////////////////////////////////////////////
// CDlgColorSwatch dialog

class CDlgColorSwatch : public CDialog
{
// Construction
public:
	COLORREF GetColor();
	void RelayEvent(MSG *pMsg);
	void SetColor(COLORREF color, int alpha);
	CDlgColorSwatch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorSwatch)
	enum { IDD = IDD_COLOR_SWATCH };
	CStaticPasvButton	m_btnAdvanced;
	CStaticColorAlpha	m_stcColor;
	CColorSwatch		m_stcSwatch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorSwatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnSwatchChange();

	// Generated message map functions
	//{{AFX_MSG(CDlgColorSwatch)
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	virtual BOOL OnInitDialog();
	afx_msg void OnAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	static CDlgColorSwatch * Instance(void);
	static void ReleaseInstance(void);
private:
	static CDlgColorSwatch *m_pInstance;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	CPoint m_ptScreenCapturer;
	BOOL m_bSendColorChanged;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORSWATCH_H__ABAE2AC7_862B_11D5_9A38_0080C82BC2DE__INCLUDED_)
