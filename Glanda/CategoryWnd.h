#pragma once
#include "Singleton.h"

// CCategoryWnd

#include "CategoryTabCtrl.h"
#include "CategoryResourcePage.h"
#include "CategoryImportPage.h"
#include "CategoryToolsPage.h"

#define INDEX_RESOURCE_PAGE		0
#define INDEX_FILES_PAGE		1
#define INDEX_TOOLS_PAGE		2

class CCategoryWnd : public CWnd, public CSingleton<CCategoryWnd>
{

	DECLARE_DYNAMIC(CCategoryWnd)

public:
	CCategoryWnd();
	virtual ~CCategoryWnd();

	CCategoryTabCtrl m_CategoryTabCtrl;
	CCategoryResourcePage m_CategoryResourcePage;
	CCategoryImportPage m_CategoryImportPage;
	CCategoryToolsPage m_CategoryToolsPage;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void RecalcLayout();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();

	int GetActivePage() {return m_CategoryTabCtrl.GetActivePage();}
	void SetActivePage(int index) {return m_CategoryTabCtrl.SetActivePage(index);}

	void LoadActivePage();
	void SaveActivePage();
};
