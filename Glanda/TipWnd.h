#if !defined(AFX_TIPWND_H__330F7C03_707E_11D5_918D_204C4F4F5020__INCLUDED_)
#define AFX_TIPWND_H__330F7C03_707E_11D5_918D_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TipWnd.h : header file
//

#include "afxtempl.h"
#include "Utils.h"
#include "CodeTip.h"

/////////////////////////////////////////////////////////////////////////////
// CTipWnd window

class CTipWnd : public CWnd
{
	typedef std::vector<CRect> RECT_LIST;

// Construction
public:
	CTipWnd();

	DECLARE_SINGLETON(CTipWnd)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void RelayEvent(MSG *pMsg);
	void SetParameterInfo(CFuncInfo *pFunc, int nCurrent);
	BOOL Create();
	virtual ~CTipWnd();

	// Generated message map functions
protected:
	CFont m_fntBold;
	CFont m_fntNormal;
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	CRect m_rcDesc;
	CRect m_rcName;
	BOOL RecalcLayout();
	int m_nMaxWidth;
	int m_nCurrent;
	CString m_strName;
	RECT_LIST m_aParamRect;
	CFuncInfo *m_pFuncInfo;
	//{{AFX_MSG(CTipWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPWND_H__330F7C03_707E_11D5_918D_204C4F4F5020__INCLUDED_)
