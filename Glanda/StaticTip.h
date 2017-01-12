#pragma once


// CStaticTip

class CStaticTip : public CStatic
{
	DECLARE_DYNAMIC(CStaticTip)

public:
	CStaticTip();
	virtual ~CStaticTip();

protected:
	DECLARE_MESSAGE_MAP()
	BOOL m_bLButtonDown;
	BOOL CanClick();
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
