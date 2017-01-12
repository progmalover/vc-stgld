#pragma once

#include "ASViewBase.h"

class CASDoc;
class gldMovieClip;
class gldFrameClip;
class gldCharacterKey;
class CVarInfo;
enum ActionSegmentType;

class CASView : public CASViewBase
{
protected: // create from serialization only
	CASView();
	DECLARE_DYNCREATE(CASView)

// Attributes
public:
	CASDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CASView)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CASView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL QueryEditable();
	virtual BOOL OnCharBefore(UINT nChar);
	virtual void OnCharAfter(UINT nChar);
	virtual void UpdateCaret();
	virtual int GetIndent(CString &strIndent, int nChar, int x, int y);
	int GetColorIndex(const CPoint &point);

	CVarInfo *FindInstance(LPCTSTR lpszVar);
	CVarInfo *FindVar(LPCTSTR lpszVar, BOOL &bDeleteVar);

	CPoint m_ptAutoCompleteStart;
	CPoint m_ptAutoCompleteEnd;
	BOOL m_bMemberListVisible;

	HACCEL m_hAccel;
	
	CToolTipCtrl m_tooltip;
	CString m_strWordLast;

public:
	void WordFromPos(CPoint point, CString &strWord);
	BOOL ShowQuickInfo();
	BOOL ShowParameterInfo();
	BOOL ShowMemberList(BOOL bAutoComplete = FALSE);
	BOOL ShowHelp();
	void HideMemberList(BOOL bUpdate);

	void Submit();

	void *m_pActionContainer;
	ActionSegmentType m_nActionType;
	CString m_strDisabledText;

	BOOL m_bSubmiting;

// Generated message map functions
protected:
	//{{AFX_MSG(CASView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnPaint();

	afx_msg void OnEditListMembers();
	afx_msg void OnEditParameterInfo();
	afx_msg LRESULT OnMemberListMessage(WPARAM wp, LPARAM lp);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnEditReference();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnEditCompleteWord();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in SampleView.cpp
inline CASDoc* CASView::GetDocument()
   { return (CASDoc*)m_pDocument; }
#endif

void LeaveActionScriptEdit();