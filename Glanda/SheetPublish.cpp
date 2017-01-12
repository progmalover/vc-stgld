// SheetPublish.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "SheetPublish.h"


// CSheetPublish

IMPLEMENT_DYNAMIC(CSheetPublish, CPropertySheet)
CSheetPublish::CSheetPublish(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_hbmHeader = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_PUBLISH_HEADER));

	Construct(nIDCaption, pParentWnd, 0, m_hbmHeader, NULL, m_hbmHeader);

	m_psh.dwFlags |= PSH_WIZARD97;

	AddPage(&m_pageSelect);
	AddPage(&m_pageExport);
	AddPage(&m_pageHTMLOptions);
	AddPage(&m_pageViewCode);
	AddPage(&m_pageSelectHTML);
	AddPage(&m_pageInsertCode);
	AddPage(&m_pageFinished);
}

CSheetPublish::CSheetPublish(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CSheetPublish::~CSheetPublish()
{
	if (m_hbmHeader != NULL)
		::DeleteObject(m_hbmHeader);
}


BEGIN_MESSAGE_MAP(CSheetPublish, CPropertySheet)
END_MESSAGE_MAP()


// CSheetPublish message handlers
