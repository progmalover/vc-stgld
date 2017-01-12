#pragma once

#include "ResizableSheet.h"
#include "InstanceActionPage.h"

// CInstanceActionSheet

class CInstanceActionSheet : public CResizableSheet
{
public:
	CInstanceActionSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CInstanceActionSheet();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CInstanceActionPage m_pageAction;
public:
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
};
