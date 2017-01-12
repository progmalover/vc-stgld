#pragma once

#include "CategoryPage.h"
#include "ResourceExplorerWnd.h"

// CCategoryExplorerPage dialog

class CCategoryExplorerPage : public CCategoryPage, public CResourceThumbItemImportAction
{
//	DECLARE_DYNAMIC(CCategoryExplorerPage)
public:
	CCategoryExplorerPage(int nResourceType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCategoryExplorerPage();

// Dialog Data
	enum { IDD = IDD_CATEGORY_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnSetActive();

	CResourceExplorerWnd m_ExplorerWnd;
	DECLARE_MESSAGE_MAP()


public:
	virtual BOOL OnInitDialog();
};
