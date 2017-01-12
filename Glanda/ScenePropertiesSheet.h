#pragma once


#include "ResizableSheet.h"
#include "ScenePropertiesPage.h"
#include "SceneActionPage.h"

// CScenePropertiesSheet

class CScenePropertiesSheet : public CResizableSheet
{
public:
	CScenePropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CScenePropertiesSheet();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CScenePropertiesPage m_pageProperties;
	CSceneActionPage m_pageAction;
	static int m_nActivePage;

public:
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal();
	afx_msg void OnDestroy();
};


