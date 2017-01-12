////////////////////////////////////////////////////////////////////////////
//	File:		CCrystalEditView.h
//	Version:	1.0.0.0
//	Created:	29-Dec-1998
//
//	Author:		Stcherbatchenko Andrei
//	E-mail:		windfall@gmx.de
//
//	Interface of the CCrystalEditView class, a part of Crystal Edit - syntax
//	coloring text editor.
//
//	You are free to use, distribute or modify this code
//	but LEAVE THIS HEADER INTACT.
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CCrystalTextView.h"

#ifndef CRYSEDIT_CLASS_DECL
#define CRYSEDIT_CLASS_DECL
#endif


/////////////////////////////////////////////////////////////////////////////
//	Forward class declarations

class CEditDropTargetImpl;


/////////////////////////////////////////////////////////////////////////////
//	CCrystalEditView view

class CRYSEDIT_CLASS_DECL CCrystalEditView : public CCrystalTextView
{
	DECLARE_DYNCREATE(CCrystalEditView)

private:
	int m_nLastInputChar;
	CPoint m_ptLastInputPos;
	BOOL m_bOvrMode;
	BOOL m_bDropPosVisible;
	CPoint m_ptSavedCaretPos;
	CPoint m_ptDropPos;
	BOOL m_bSelectionPushed;
	CPoint m_ptSavedSelStart, m_ptSavedSelEnd;
	bool m_bIme, m_bImeSelChange;
	BOOL DeleteCurrentSelection();

protected:
	CEditDropTargetImpl *m_pDropTarget;
	virtual DROPEFFECT GetDropEffect();
	virtual void OnDropSource(DROPEFFECT de);
	void Paste();
	void Cut();

// Attributes
public:

// Operations
public:
	CCrystalEditView();
	~CCrystalEditView();

	BOOL GetOverwriteMode() const;
	void SetOverwriteMode(BOOL bOvrMode = TRUE);

	void ShowDropIndicator(const CPoint &point);
	void HideDropIndicator();

	BOOL DoDropText(COleDataObject *pDataObject, const CPoint &ptClient);
	void DoDragScroll(const CPoint &point);

	virtual BOOL QueryEditable();
	virtual void UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex = -1);

	BOOL ReplaceSelection(LPCTSTR pszNewText);
	BOOL InsertText(LPCTSTR pszNewText);
	CString GetSelText();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrystalEditView)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
protected:
	virtual void OnCharAfter(UINT nChar);
	virtual BOOL OnCharBefore(UINT nChar);
	virtual int GetIndent(CString &strIndent, int nChar, int x, int y);

	//{{AFX_MSG(CCrystalEditView)
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditDeleteBack();
	afx_msg void OnEditUntab();
	afx_msg void OnEditTab();
	afx_msg void OnEditSwitchOvrmode();
	afx_msg void OnUpdateEditSwitchOvrmode(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditReplace();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnEditRedoall();
	afx_msg void OnEditUndoall();
	//}}AFX_MSG
	afx_msg void OnUpdateIndicatorCol(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorOvr(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorRead(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorPosition(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorCRLF(CCmdUI* pCmdUI);

	//孙黎增加支持ime消息
	void MyEndIme();
	void SetCompositionWindow();
	afx_msg LRESULT OnImeComposition(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnImeNotify(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnImeEndComposition(WPARAM wp, LPARAM lp);
	virtual void OnSelectChange();
	void IMEReplaceText(LPCTSTR pszNewText);

	DECLARE_MESSAGE_MAP()
};

inline BOOL CCrystalEditView::GetOverwriteMode() const
{
	return m_bOvrMode;
}

inline void CCrystalEditView::SetOverwriteMode(BOOL bOvrMode /*= TRUE*/)
{
	m_bOvrMode = bOvrMode;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
