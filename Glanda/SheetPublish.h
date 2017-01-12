#pragma once

#include "PagePublishSelect.h"
#include "PagePublishExport.h"
#include "PagePublishHTMLOptions.h"
#include "PagePublishViewCode.h"
#include "PagePublishSelectHTML.h"
#include "PagePublishInsertCode.h"
#include "PagePublishFinished.h"

#define FORMAT_UNDEFINED	-1
#define FORMAT_WIDNOWS		0
#define FORMAT_UNIX			1
#define FORMAT_MAC			2

// CSheetPublish

class CSheetPublish : public CPropertySheet
{
	DECLARE_DYNAMIC(CSheetPublish)

public:
	CSheetPublish(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSheetPublish(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CSheetPublish();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	HBITMAP m_hbmHeader;

public:
	CPagePublishSelect m_pageSelect;
	CPagePublishExport m_pageExport;
	CPagePublishHTMLOptions m_pageHTMLOptions;
	CPagePublishViewCode m_pageViewCode;
	CPagePublishSelectHTML m_pageSelectHTML;
	CPagePublishInsertCode m_pageInsertCode;
	CPagePublishFinished m_pageFinished;
};


