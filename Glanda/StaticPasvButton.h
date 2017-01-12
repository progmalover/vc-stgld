#if !defined(AFX_STATICPASVBUTTON_H__ABAE2AC8_862B_11D5_9A38_0080C82BC2DE__INCLUDED_)
#define AFX_STATICPASVBUTTON_H__ABAE2AC8_862B_11D5_9A38_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticPasvButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticPasvButton window

class CStaticPasvButton : public CStatic
{
// Construction
public:
	CStaticPasvButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticPasvButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadBitmap(UINT nID, COLORREF crTrans=RGB(255, 0, 255));
	virtual ~CStaticPasvButton();

	// Generated message map functions
protected:
	int m_nState;
	void SetState(int nState);
	BOOL m_bTimer;
	COLORREF m_crTrans;
	CBitmap m_Bmp;
	//{{AFX_MSG(CStaticPasvButton)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICPASVBUTTON_H__ABAE2AC8_862B_11D5_9A38_0080C82BC2DE__INCLUDED_)
