#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CCrystalEditView.h"

class COutputDoc;

class COutputView : public CCrystalEditView
{
protected: // create from serialization only
	COutputView();
	DECLARE_DYNCREATE(COutputView)

// Attributes
public:
	COutputDoc* GetDocument();

	virtual CCrystalTextBuffer *LocateTextBuffer();

protected:
	virtual DWORD ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(COutputView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnGetDlgCode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	void AppendLog(LPCTSTR fmt, ...);
	void ClearLog();

public:
	afx_msg void OnEditClearAll();
	afx_msg void OnUpdateEditClearAll(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
};

#ifndef _DEBUG  // debug version in SampleView.cpp
inline COutputDoc* COutputView::GetDocument()
   { return (COutputDoc*)m_pDocument; }
#endif
