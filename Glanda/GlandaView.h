// GlandaView.h : interface of the CGlandaView class
//


#pragma once


class CGlandaView : public CView
{
protected: // create from serialization only
	CGlandaView();
	DECLARE_DYNCREATE(CGlandaView)

// Attributes
public:
	CGlandaDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	void RepositionChildren();

// Implementation
public:
	virtual ~CGlandaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool m_bInit;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void OnInitialUpdate();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnViewRulers();
	afx_msg void OnUpdateViewRulers(CCmdUI *pCmdUI);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};

#ifndef _DEBUG  // debug version in GlandaView.cpp
inline CGlandaDoc* CGlandaView::GetDocument() const
   { return reinterpret_cast<CGlandaDoc*>(m_pDocument); }
#endif

