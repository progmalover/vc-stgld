#pragma once


// CMiniDockFrameWndEx frame

class CMiniDockFrameWndEx : public CMiniDockFrameWnd
{
	DECLARE_DYNCREATE(CMiniDockFrameWndEx)
protected:
	CMiniDockFrameWndEx();           // protected constructor used by dynamic creation
	virtual ~CMiniDockFrameWndEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnNcHitTest(CPoint point);

public:
	BOOL m_bEnableSizing;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


