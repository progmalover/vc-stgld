#pragma once
#include "CategoryPage.h"
#include "ResourceExplorerWnd.h"


// CCategoryImportPage dialog

class CCategoryImportPage : public CCategoryPage, public CResourceThumbItemImportAction
{
//	DECLARE_DYNAMIC(CCategoryImportPage)

public:
	CCategoryImportPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCategoryImportPage();

// Dialog Data
	enum { IDD = IDD_CATEGORY_IMPORT_PAGE };
	CResourceExplorerWnd m_ExplorerWnd;

	void SetProfileSection(LPCTSTR lpszSection)
	{
		m_ExplorerWnd.SetProfileSection(lpszSection);
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnSetActive();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
