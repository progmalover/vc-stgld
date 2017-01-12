#pragma once

#include "ResizableDialog.h"

// CCategoryPage

class CCategoryPage : public CResizableDialog
{
//	DECLARE_DYNAMIC(CCategoryPage)

public:
	CCategoryPage();
	CCategoryPage(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	CCategoryPage(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual ~CCategoryPage();

	virtual void OnSetActive();
	virtual void OnKillActive();
	BOOL InitPage(CWnd* pParentWnd);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

	BOOL m_bFirstActivePage;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


