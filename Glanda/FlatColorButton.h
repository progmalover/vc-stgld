#if !defined(AFX_FLATCOLORBUTTON_H__A92BFBA2_8628_11D5_9A38_0080C82BC2DE__INCLUDED_)
#define AFX_FLATCOLORBUTTON_H__A92BFBA2_8628_11D5_9A38_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlatColorButton.h : header file
//

#include "FlatButton.h"
#include "DlgColorSwatch.h"	// Added by ClassView


#define BN_COLORCHANGE			0x1000
#define BN_COLORSWATCHDROPDOWN	0x1001

/////////////////////////////////////////////////////////////////////////////
// CFlatColorButton window

class CFlatColorButton : public CFlatButton
{
// Construction
public:
	CFlatColorButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatColorButton)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableAlpha(BOOL bEnable);
	int GetAlpha();
	COLORREF GetColor();
	void SetColor(COLORREF color, int alpha);
	virtual ~CFlatColorButton();

	// Generated message map functions
protected:
	BOOL m_bEnableAlpha;
	LRESULT afx_msg OnColorAdvanced(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnColorChange(WPARAM wp, LPARAM lp);
	virtual void DrawStuff(CDC *pDC, const CRect &rc);
	virtual CSize GetStuffSize();
	int m_nColor;
	int m_nAValue;

	//{{AFX_MSG(CFlatColorButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATCOLORBUTTON_H__A92BFBA2_8628_11D5_9A38_0080C82BC2DE__INCLUDED_)
