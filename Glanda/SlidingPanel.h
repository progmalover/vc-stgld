#pragma once

#include "FlatBitmapButton.h"

class CCategoryPage;
typedef CCategoryPage CSlidingPage;
// CSlidingPanel


class CSlidingPanel : public CWnd
{
	DECLARE_DYNAMIC(CSlidingPanel)

protected:
	class CSlidingPanelTitleButton : public CFlatButton
	{
		CSlidingPanel* m_pSlidingPanel;
		CBitmap m_bmpArrow;
		BOOL m_bExpanded;
	public:
		CSlidingPanelTitleButton(CSlidingPanel* pSlidingPanel);
		void Expand(BOOL bExpand);
	protected:
		virtual CSize GetStuffSize();
		virtual void DrawStuff(CDC* pDC, const CRect &rc);
	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	}m_btnSlidingArrow;

	enum { BORDER = 1};
	UINT m_nIDSlidingArrow;
	UINT m_nIDSlidingArrowExpanded;
	BOOL m_bExpanded;
	CSlidingPage* m_pSlidingPage;
	UINT m_nIDCaption;

public:
	CSlidingPanel(CSlidingPage* pSlidingPage, int nImage, UINT nIDCaption);
	virtual ~CSlidingPanel();

	void UnloadBitmap(void);
	BOOL LoadBitmap(UINT nIDBitmap);
	void Expand();
	void Collapse();
	void RecalcLayout();
	static int GetTitleHeight();
	void GetSlidingArrowRect(CRect& rc);
	BOOL IsExpanded()
	{
		return m_bExpanded;
	}

	int m_nImage;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSlidingArrow();
	afx_msg void OnPaint();
};


