#if !defined(AFX_BALLOONTOOLTIP_H__62D90A2F_8250_11D5_9A34_0080C82BC2DE__INCLUDED_)
#define AFX_BALLOONTOOLTIP_H__62D90A2F_8250_11D5_9A34_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BalloonToolTip.h : header file
//

#include "Utils.h"

/////////////////////////////////////////////////////////////////////////////
// CBalloonToolTip window

class CBalloonToolTip : public CWnd
{
// Construction
public:
	CBalloonToolTip();

	DECLARE_SINGLETON(CBalloonToolTip)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBalloonToolTip)
	//}}AFX_VIRTUAL

// Implementation
public:
	void RelayEvent(MSG *pMsg);
	BOOL Show(int x, int y, UINT nIDText, UINT nIDTitle = 0, LPCTSTR lpszLink = NULL);
	BOOL Show(int x, int y, LPCTSTR lpszText, LPCTSTR lpszTitle = NULL, LPCTSTR lpszLink = NULL);
	virtual ~CBalloonToolTip();

	// Generated message map functions
protected:
	CFont m_fntTitle;
	CFont m_fntText;
	BOOL m_bHoverLink;
	BOOL m_bUp;
	BOOL m_bRight;
	CString m_strTitle;
	static LRESULT CALLBACK ToolTipWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static WNDPROC m_lpOldWindowProc;
	BOOL CalcWindowRgn(CRect &rcBound, CPoint &ptTrail);
	CRgn m_rgn;
	CString m_strText;
	CString m_strLink;
	CString m_strLinkText;
	CRect m_rc;
	BOOL m_bSingleLine;
	BOOL Create(CWnd *pParent);
	void GetLinkRect(CRect &rcLink);
	//{{AFX_MSG(CBalloonToolTip)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BALLOONTOOLTIP_H__62D90A2F_8250_11D5_9A34_0080C82BC2DE__INCLUDED_)
