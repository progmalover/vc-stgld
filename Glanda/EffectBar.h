#pragma once

#include "GlandaSizingControlBar.h"
#include "Singleton.h"
#include "DlgEffectPanel.h"

// CEffectBar

class CEffectBar : public CGlandaSizingControlBar, public CSingleton<CEffectBar>
{
	DECLARE_DYNAMIC(CEffectBar)

public:
	CEffectBar();
	virtual ~CEffectBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);
protected:
	virtual void PaintClient(CDC& dc);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	CDlgEffectPanel m_dlgEffect;
};
