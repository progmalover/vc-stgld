#if !defined(AFX_COLORSWATCH_H__DA3E19D6_6E88_11D5_9A1D_0080C82BC2DE__INCLUDED_)
#define AFX_COLORSWATCH_H__DA3E19D6_6E88_11D5_9A1D_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorSwatch.h : header file
//

#define	SWN_CHANGE		0x1000

/////////////////////////////////////////////////////////////////////////////
// CColorSwatch window

class CColorSwatch : public CStatic
{
// Construction
public:
	CColorSwatch();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSwatch)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetAlwaysSelect(BOOL bAlways);
	void SetColor(COLORREF color);
	COLORREF GetColor();
	void AdjustSize();
	virtual ~CColorSwatch();

	// Generated message map functions
protected:
	BOOL m_bAlwaysSelect;
	BOOL m_bTracking;
	void EndTrack();
	void StartTrack();
	void SwatchChange(const CPoint &point);
	CPoint m_ptSwatch;
	COLORREF m_Color;
	//{{AFX_MSG(CColorSwatch)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnable(BOOL bEnable);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSWATCH_H__DA3E19D6_6E88_11D5_9A1D_0080C82BC2DE__INCLUDED_)
