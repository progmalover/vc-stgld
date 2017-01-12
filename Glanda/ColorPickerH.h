#if !defined(AFX_COLORPICKERH_H__3909FD26_6A99_11D5_9A1A_0080C82BC2DE__INCLUDED_)
#define AFX_COLORPICKERH_H__3909FD26_6A99_11D5_9A1A_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPickerH.h : header file
//

#define CPN_HCHANGE		0x1000

/////////////////////////////////////////////////////////////////////////////
// CColorPickerH window

class CColorPickerH : public CStatic
{
// Construction
public:
	CColorPickerH();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerH)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsTracking() {return m_bTracking;}
	void SetStyle(BOOL bHorz);
	void GetHValueFromPoint(const CPoint &point);
	void SetHValue(int nHValue);
	int GetHValue();
	virtual ~CColorPickerH();

	// Generated message map functions
protected:
	void GetPointFromHValue(CPoint &point);
	void EndTrack();
	void StartTrack();
	BOOL m_bTracking;
	int m_nHValue;
	BOOL m_bHorz;
	//{{AFX_MSG(CColorPickerH)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnEnable(BOOL bEnable);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKERH_H__3909FD26_6A99_11D5_9A1A_0080C82BC2DE__INCLUDED_)
