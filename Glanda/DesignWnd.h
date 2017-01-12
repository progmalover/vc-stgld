#pragma once

#include "RuleWnd.h"

class gldCharacterKey;
class gld_shape;

// CDesignWnd
class CDesignWnd : public CWnd
{
	DECLARE_DYNAMIC(CDesignWnd)

public:
	CDesignWnd();
	virtual ~CDesignWnd();

	DECLARE_SINGLETON(CDesignWnd)

	BOOL m_bEraseBkgnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL CanDraw(bool bAlert = true);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	BOOL m_bInit;
	HACCEL m_hAccel;
	CToolTipCtrl m_tooltip;
	gldObj *m_pObj;
	CMoveRuleTool m_MoveRuleTool;	

	void CDesignWnd::EditInstance(gld_shape *pShape);
public:
	BOOL Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID);
	void RecalcScrollPos();
	void Reset();
	void RefreshScrollBar();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditDelete();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	void DrawAnimatedRects(CRect *pRectStart, CRect *pRectEnd);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDebugTestundoredo();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnCreateEffect();
	afx_msg void OnUpdateCreateEffect(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);

	afx_msg void OnTextDeleteBack();
	afx_msg void OnUpdateTextDeleteBack(CCmdUI *pCmdUI);
	afx_msg void OnTextUndo();
	afx_msg void OnUpdateTextUndo(CCmdUI *pCmdUI);
	afx_msg void OnTextRedo();
	afx_msg void OnUpdateTextRedo(CCmdUI *pCmdUI);
	afx_msg void OnTextGotoBeginOfText();
	afx_msg void OnTextGotoEndOfText();

	afx_msg LRESULT OnBeginDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCancelDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDragDrop(WPARAM wParam, LPARAM lParam);
#ifdef _DEBUG
	afx_msg void OnDebugTestdrawing();
	afx_msg void OnTest();
#endif
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditActionScript();
	afx_msg void OnUpdateEditActionScript(CCmdUI *pCmdUI);
	afx_msg void OnEditInstanceProperties();
	afx_msg void OnUpdateEditInstanceProperties(CCmdUI *pCmdUI);

	afx_msg void OnEditPrevInstance();
	afx_msg void OnUpdateEditPrevInstance(CCmdUI *pCmdUI);
	afx_msg void OnEditNextInstance();
	afx_msg void OnUpdateEditNextInstance(CCmdUI *pCmdUI);

	afx_msg void OnSaveElementAsTemplate();
	afx_msg void OnUpdateSaveElementAsTemplate(CCmdUI *pCmdUI);

	afx_msg void OnApplyTemplateToElement();
	afx_msg void OnUpdateApplyTemplateToElement(CCmdUI *pCmdUI);

	afx_msg void OnChangeButtonText();
	afx_msg void OnUpdateChangeButtonText(CCmdUI *pCmdUI);

	afx_msg void OnSetAsBackground();
	afx_msg void OnUpdateSetAsBackground(CCmdUI *pCmdUI);
	afx_msg void OnEmptyBackground();
	afx_msg void OnUpdateEmptyBackground(CCmdUI *pCmdUI);
	afx_msg void OnRestoreBackground();
	afx_msg void OnUpdateRestoreBackground(CCmdUI *pCmdUI);
	afx_msg void OnChangeBackgroundPos();
	afx_msg void OnUpdateChangeBackgroundPos(CCmdUI *pCmdUI);
};
