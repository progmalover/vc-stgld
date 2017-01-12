// EditTextSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditTextSheet.h"
#include "DrawHelper.h"
#include "Command.h"
#include "CmdModifyMatrix.h"
#include "CmdModifyCxform.h"
#include "my_app.h"


// CEditTextSheet

IMPLEMENT_DYNAMIC(CEditTextSheet, CPropertySheet)
CEditTextSheet::CEditTextSheet(gldInstance* pInstance)
	:CPropertySheet(IDS_EDIT_TEXT_SHEET_CAPTION)
	, m_pInstance(pInstance)
	, m_editGeneralPage(pInstance)
	, m_editMatrixPage(pInstance)
	, m_editCxformPage(pInstance)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_editGeneralPage);
	AddPage(&m_editMatrixPage);
	AddPage(&m_editCxformPage);
}

CEditTextSheet::~CEditTextSheet()
{
}


BEGIN_MESSAGE_MAP(CEditTextSheet, CPropertySheet)
END_MESSAGE_MAP()


// CEditTextSheet message handlers

INT_PTR CEditTextSheet::DoModal()
{
	INT_PTR nRet = CPropertySheet::DoModal();

	int nCmdCount = 0;
	CGuardDrawOnce xDraw;

	// Unexec General
	if(m_editGeneralPage.m_pCmd)
	{
		++nCmdCount;
		m_editGeneralPage.m_pCmd->Unexecute();
	}
	// Unexec Matrix
	if(m_editMatrixPage.m_pCmd)
	{
		++nCmdCount;
		m_editMatrixPage.m_pCmd->Unexecute();
	}
	// Unexec Cxform
	if(m_editCxformPage.m_pCmd)
	{
		++nCmdCount;
		m_editCxformPage.m_pCmd->Unexecute();
	}

	if (nRet == IDOK)
	{
		if(nCmdCount)
		{
			TCommandGroup* pCmd = new TCommandGroup(IDS_CMD_MODIFY_TEXT);
			if(m_editGeneralPage.m_pCmd)
				pCmd->Do(m_editGeneralPage.m_pCmd);
			// Add Matrix Command
			if (m_editMatrixPage.m_pCmd)
				pCmd->Do(m_editMatrixPage.m_pCmd);
			// Add Cxform Command
			if(m_editCxformPage.m_pCmd)
				pCmd->Do(m_editCxformPage.m_pCmd);

			my_app.Commands().Do(pCmd);
		}
	}
	else
	{
		if(m_editGeneralPage.m_pCmd)
			delete m_editGeneralPage.m_pCmd;
		// Delete Matrix Command
		if(m_editMatrixPage.m_pCmd)
			delete m_editMatrixPage.m_pCmd;
		// Delete Cxform Command
		if(m_editCxformPage.m_pCmd)
			delete m_editCxformPage.m_pCmd;
	}

	return nRet;
}
