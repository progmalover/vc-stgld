#if !defined(AFX_STATICTEXT_H__FD87C899_2E07_11D3_B8D5_204C4F4F5020__INCLUDED_)
#define AFX_STATICTEXT_H__FD87C899_2E07_11D3_B8D5_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticText.h : header file
//

#include "Graphics.h"

#define STS_AUTOSIZE			1L << 0

#define STS_BOLD				1L << 16
#define STS_ITALIC				1L << 17
#define STS_UNDERLINE			1L << 18

/////////////////////////////////////////////////////////////////////////////
// CStaticText window

class CStaticText : public CStatic
{
// Construction
public:
	CStaticText();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticText)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create( LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff );
	BOOL ModifySTStyle(DWORD dwRemove, DWORD dwAdd);
	COLORREF GetTextColor();
	COLORREF GetBkColor();
	BOOL SetTextColor(COLORREF crText);
	BOOL SetBkColor(COLORREF rgb);
	BOOL SetFontDirect(CFont *pFont);
	BOOL SetFontFaceName(LPCTSTR lpszFaceName);
	BOOL SetFontUnderline(BOOL bUnderline);
	BOOL SetFontItalic(BOOL bItalic);
	BOOL SetFontBold(BOOL bBold);
	BOOL SetFontSize(int nSize);
	virtual ~CStaticText();

protected:
	BOOL m_bDynamicCreate;
	BOOL m_bInit;
	DWORD m_dwStyle;

	COLORREF m_crText;
	COLORREF m_crBack;
	
	CString m_strFaceName;
	int m_nSize;
	BOOL m_bBold;
	BOOL m_bItalic;
	BOOL m_bUnderline;

	virtual BOOL Init();
	void AutoSize();
	void CreateFont(CFont &font);

	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICTEXT_H__FD87C899_2E07_11D3_B8D5_204C4F4F5020__INCLUDED_)
