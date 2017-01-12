// MoviePropertiesSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "MoviePropertiesSheet.h"
#include "my_app.h"
#include ".\moviepropertiessheet.h"


// CMoviePropertiesSheet

int CMoviePropertiesSheet::m_nActivePage = 0;

IMPLEMENT_DYNAMIC(CMoviePropertiesSheet, CPropertySheet)
CMoviePropertiesSheet::CMoviePropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, m_nActivePage)
{
	AddPage(&m_dlgProperties);
	AddPage(&m_dlgSound);
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

CMoviePropertiesSheet::~CMoviePropertiesSheet()
{
}


BEGIN_MESSAGE_MAP(CMoviePropertiesSheet, CPropertySheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMoviePropertiesSheet message handlers

INT_PTR CMoviePropertiesSheet::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class

	INT_PTR ret = CPropertySheet::DoModal();

	if(m_dlgSound.m_pCmdAddObj)
	{
		m_dlgSound.m_pCmdAddObj->Unexecute();
	}

	if (ret==IDOK)
	{
		TCommandGroup *pCmd = new TCommandGroup(IDS_CMD_CHANGEMOVIEPROPERTIES);

		if (m_dlgProperties.m_pCmd)
		{
			pCmd->Do(m_dlgProperties.m_pCmd);
			m_dlgProperties.m_pCmd = NULL;
		}

		if(m_dlgSound.m_pCmdAddObj)
		{
			pCmd->Do(m_dlgSound.m_pCmdAddObj);
			m_dlgSound.m_pCmdAddObj = NULL;
		}

		if (m_dlgSound.m_pCmd)
		{
			pCmd->Do(m_dlgSound.m_pCmd);
			m_dlgSound.m_pCmd = NULL;
		}

		my_app.Commands().Do(pCmd);
	}
	else
	{
		if (m_dlgProperties.m_pCmd)
		{
			delete m_dlgProperties.m_pCmd;
			m_dlgProperties.m_pCmd = NULL;
		}

		if (m_dlgSound.m_pCmd)
		{
			delete m_dlgSound.m_pCmd;
			m_dlgSound.m_pCmd = NULL;
		}

		if(m_dlgSound.m_pCmdAddObj)
		{
			delete m_dlgSound.m_pCmdAddObj;
			m_dlgSound.m_pCmdAddObj = NULL;
		}
	}

	return ret;
}

void CMoviePropertiesSheet::OnDestroy()
{
	CPropertySheet::OnDestroy();

	// TODO: Add your message handler code here

	m_nActivePage = GetActiveIndex();
}
