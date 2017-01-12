#if !defined(AFX_CHECKBOXTREECTRL_H__C4CDE4E7_5D11_11D4_BFE6_0080C82BC2DE__INCLUDED_)
#define AFX_CHECKBOXTREECTRL_H__C4CDE4E7_5D11_11D4_BFE6_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckBoxTreeCtrl.h : header file
//

#define UNCHECKED		0
#define CHECKED			1
#define INDETERMINATE	2
 
/////////////////////////////////////////////////////////////////////////////
// CCheckBoxTreeCtrl window

class CCheckBoxTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CCheckBoxTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckBoxTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableCheckBox(BOOL bEnable);
	int GetCheck(HTREEITEM hItem);
	BOOL SetCheck(HTREEITEM hItem, int nState);
	virtual BOOL OnPreSetCheck(HTREEITEM hItem, int nState);
	virtual void CCheckBoxTreeCtrl::OnPostSetCheck(HTREEITEM hItem, int nState);
	virtual ~CCheckBoxTreeCtrl();

	// Generated message map functions
protected:
	BOOL CheckParent(HTREEITEM hItem);
	BOOL IsParentItem(HTREEITEM hParent, HTREEITEM hChild);
	BOOL CheckChildren(HTREEITEM hItem);
	BOOL m_bCheckBox;
	BOOL ToggleCheck(HTREEITEM hItem);
	CImageList m_imgState;
	//{{AFX_MSG(CCheckBoxTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnSetCheck(HTREEITEM hItem, int nState);
	BOOL m_bAutoSynchronize;
	void EnableAutoSynchronize(BOOL bEnable);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKBOXTREECTRL_H__C4CDE4E7_5D11_11D4_BFE6_0080C82BC2DE__INCLUDED_)
