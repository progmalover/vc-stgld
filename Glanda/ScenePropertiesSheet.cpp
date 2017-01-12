// ScenePropertiesSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include ".\scenepropertiessheet.h"

#include "Command.h"
#include "my_app.h"

// CScenePropertiesSheet

int CScenePropertiesSheet::m_nActivePage	= 0;

CScenePropertiesSheet::CScenePropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CResizableSheet(pszCaption, pParentWnd, m_nActivePage)
{
	AddPage(&m_pageProperties);
	AddPage(&m_pageAction);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CScenePropertiesSheet::~CScenePropertiesSheet()
{
}


BEGIN_MESSAGE_MAP(CScenePropertiesSheet, CResizableSheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CScenePropertiesSheet message handlers

BOOL CScenePropertiesSheet::OnInitDialog()
{
	BOOL bResult = CResizableSheet::OnInitDialog();

	// TODO:  Add your specialized code here

	EnableSaveRestore("Scene Properties");

	return bResult;
}

INT_PTR CScenePropertiesSheet::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class

	INT_PTR ret = CResizableSheet::DoModal();
	if (ret == IDOK)
	{
		TCommandGroup *pCmd = new TCommandGroup(IDS_CMD_CHANGESCENEPROPERTIES);
		
		if (m_pageProperties.m_pCmd)
			pCmd->Do(m_pageProperties.m_pCmd);

		if (m_pageAction.m_pCmd)
			pCmd->Do(m_pageAction.m_pCmd);
		
		my_app.Commands().Do(pCmd);
	}

	return ret;
}

void CScenePropertiesSheet::OnDestroy()
{
	CResizableSheet::OnDestroy();

	// TODO: Add your message handler code here

	m_nActivePage = GetActiveIndex();
}
