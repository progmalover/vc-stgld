#if !defined(AFX_TOOLBAREX_H__6F28AB37_06FF_11D5_9721_0080C82BC2DE__INCLUDED_)
#define AFX_TOOLBAREX_H__6F28AB37_06FF_11D5_9721_0080C82BC2DE__INCLUDED_

#include "ToolBarButton.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolbarEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolBarEx window

class CToolBarEx : public CToolBar
{
// Construction
public:
	CToolBarEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBarEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void Init(LPCTSTR lpszSection, LPCTSTR lpszName);
	void RestoreState();
	virtual ~CToolBarEx();

	// Generated message map functions
protected:
	CImageList *m_pImageList;
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void RecalcLayout();
	void SaveState();
	void SetAdjustable(BOOL bAdjustable);
	void RestoreInitialState();
	void SaveInitialState();
	CToolBarButton m_wndButton;
	BOOL CreateCustomizeButton();
	int m_nButtons;
	TBBUTTON * m_pButtons;
	BOOL GetButtonInfo(WPARAM wParam, LPARAM  lParam, LRESULT* pLResult);
	CString m_strSubKey;
	CString m_strName;
	HKEY m_hKey;
	//{{AFX_MSG(CToolBarEx)
	//}}AFX_MSG
	afx_msg void OnToolBarCustomize();
	afx_msg void OnUpdateToolBarCustomize(CCmdUI* pCmdUI);
	afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnDropDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnSetButtonSize(WPARAM, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	BOOL SetButtonText(UINT nCommand, UINT nStringID);
	void ShowCustomizeMenu(CPoint point);
	void SetDropDownButton(UINT nCommand, UINT nIDMenu);
	void ShowPopupMenu(UINT nCommand, UINT nIDMenu);
	BOOL LoadImage(UINT nBitmapID, int cxButton, int cyButton);
	BOOL LoadImage(CBitmap *pBitmap, int cxButton, int cyButton);
	virtual void DrawBorders(CDC* pDC, CRect& rect);
	BOOL SupportDropDownButton(void);
	BOOL SupportMixedButtons(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBAREX_H__6F28AB37_06FF_11D5_9721_0080C82BC2DE__INCLUDED_)
