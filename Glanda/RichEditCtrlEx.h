#if !defined(AFX_RICHEDITCTRLEX_H__F1A59A16_4649_11D4_BFCC_0080C82BC2DE__INCLUDED_)
#define AFX_RICHEDITCTRLEX_H__F1A59A16_4649_11D4_BFCC_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichEditCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window

class CRichEditCtrlEx : public CRichEditCtrl
{
// Construction
public:
	CRichEditCtrlEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Redo();
	BOOL CanRedo();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	static DWORD CALLBACK EditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
	static DWORD CALLBACK EditStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);
	void SetRTFText(LPCTSTR szText);
	void GetRTFText(CString& strText);
	virtual ~CRichEditCtrlEx();

	// Generated message map functions
protected:
	static BOOL m_bRichEdit20;
	//{{AFX_MSG(CRichEditCtrlEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITCTRLEX_H__F1A59A16_4649_11D4_BFCC_0080C82BC2DE__INCLUDED_)
