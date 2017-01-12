#if !defined(AFX_FLATBITMAPBUTTON_H__D5025BA5_6ED4_11D3_A03F_0080C8776EA2__INCLUDED_)
#define AFX_FLATBITMAPBUTTON_H__D5025BA5_6ED4_11D3_A03F_0080C8776EA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlatBitmapButton.h : header file
//

#include "FlatButton.h"

/////////////////////////////////////////////////////////////////////////////
// CFlatBitmapButton window

class CFlatBitmapButton : public CFlatButton
{
// Construction
public:
	CFlatBitmapButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatBitmapButton)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadBitmap(HINSTANCE hInst, UINT nBitmapID, int nImageCount = 2, COLORREF crTrans = RGB(255, 0, 255));
	BOOL LoadBitmap(LPCTSTR lpszFile, int nImageCount = 2, COLORREF crTrans = RGB(255, 0, 255));
	BOOL LoadBitmap(UINT nBitmapID, int nImageCount = 2, COLORREF crTrans = RGB(255, 0, 255));
	UINT GetBitmapID();
	BOOL IsBitmapLoaded();
	void UnloadBitmap(BOOL bRedraw = TRUE);
	virtual ~CFlatBitmapButton();
	int GetWidth(){return m_nWidth;}
	int GetHeight(){return m_nHeight;}

	// Generated message map functions
protected:
	virtual CSize GetStuffSize();
	virtual void DrawStuff(CDC *pDC, const CRect &rc);
	UINT m_nBitmapID;
	CBitmap m_Bmp;
	int m_nWidth;
	int m_nHeight;
	COLORREF m_crTrans;
	int m_nImageCount;
	//{{AFX_MSG(CFlatBitmapButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATBITMAPBUTTON_H__D5025BA5_6ED4_11D3_A03F_0080C8776EA2__INCLUDED_)
