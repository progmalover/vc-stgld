#pragma once

#include "sizecbar.h"
#include "flatbitmapbutton.h"

#define TITLE_SIZE		16

// CSizingControlBarEx

class CSizingControlBarEx : public CSizingControlBar
{
	DECLARE_DYNAMIC(CSizingControlBarEx)

public:
	CSizingControlBarEx();
	virtual ~CSizingControlBarEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickedClose();

public:
	virtual void GetTitleRect(CRect *pRect);
	virtual void PaintTitle(CDC &dc);
	void GetClientRectExcludeBorder(CRect *pRect);
	void GetClientRectExcludeTitle(CRect *pRect);
	void GetTitleTextRect(CRect *pRect);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	CFlatBitmapButton m_btnClose;
	BOOL m_bShowCloseButton;
	BOOL m_bShowTitle;
	BOOL m_bActive;

	virtual void LoadCloseButtonBitmap(BOOL bActive);

	virtual void RecalcLayout();
	virtual void PaintClient(CDC &dc);
public:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	void CheckActive(void);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void ShowTitle(BOOL bShow);
	BOOL HasTitle(void);
};
