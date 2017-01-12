// InstanceActionSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "InstanceActionSheet.h"
#include ".\instanceactionsheet.h"

#include "Command.h"
#include "my_app.h"

// CInstanceActionSheet

CInstanceActionSheet::CInstanceActionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_pageAction);
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CInstanceActionSheet::~CInstanceActionSheet()
{
}


BEGIN_MESSAGE_MAP(CInstanceActionSheet, CResizableSheet)
END_MESSAGE_MAP()


// CInstanceActionSheet message handlers

INT_PTR CInstanceActionSheet::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class

	INT_PTR ret = CResizableSheet::DoModal();
	if (ret == IDOK)
	{
		TCommandGroup *pCmd = new TCommandGroup(IDS_CMD_CHANGEINSTANCEACTION);
		
		if (m_pageAction.m_pCmd)
			pCmd->Do(m_pageAction.m_pCmd);
		
		my_app.Commands().Do(pCmd);
	}

	return ret;
}

BOOL CInstanceActionSheet::OnInitDialog()
{
	BOOL bResult = CResizableSheet::OnInitDialog();

	// TODO:  Add your specialized code here

	EnableSaveRestore("Instance Action");

	return bResult;
}
