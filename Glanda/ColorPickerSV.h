#if !defined(AFX_COLORPICKERSV_H__F8D6A6D3_6AC0_11D5_9A1A_0080C82BC2DE__INCLUDED_)
#define AFX_COLORPICKERSV_H__F8D6A6D3_6AC0_11D5_9A1A_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPickerSV.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorPickerSV window

#define CPN_SVCHANGE		0x1000

class CColorPickerSV : public CStatic
{
// Construction
public:
	CColorPickerSV();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerSV)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsTracking() {return m_bTracking;}
	int GetVValue();
	int GetSValue();
	int GetHValue();
	void SetVValue(int nVValue, BOOL bRedraw = TRUE);
	void SetSValue(int nSValue, BOOL bRedraw = TRUE);
	void SetHValue(int nHValue, BOOL bRedraw = TRUE);
	virtual ~CColorPickerSV();

	// Generated message map functions
protected:
	void GetPointFromSVValue(CPoint &point);
	void GetSVValueFromPoint(const CPoint &point);
	BOOL m_bTracking;
	void EndTrack();
	void StartTrack();
	int m_nHValue;
	int m_nSValue;
	int m_nVValue;
	//{{AFX_MSG(CColorPickerSV)
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

#endif // !defined(AFX_COLORPICKERSV_H__F8D6A6D3_6AC0_11D5_9A1A_0080C82BC2DE__INCLUDED_)
