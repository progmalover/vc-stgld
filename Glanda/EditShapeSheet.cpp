// EditShapeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditShapeSheet.h"
#include "SWFProxy.h"
#include "TransAdaptor.h"
#include ".\EditShapeSheet.h"
#include "my_app.h"
#include "CmdModifyCxform.h"
#include "CmdModifyMatrix.h"
#include "DrawHelper.h"


// CEditShapeSheet

IMPLEMENT_DYNAMIC(CEditShapeSheet, CPropertySheet)

CEditShapeSheet::CEditShapeSheet(gldInstance* pInstance)
: CPropertySheet(IDS_EDIT_SHAPE_SHEET_CAPTION)
, m_pInstance(pInstance)
, m_editGeneralPage(pInstance)
, m_editLinePage(pInstance->m_obj)
, m_editFillPage(pInstance->m_obj)
, m_editMatrixPage(pInstance)
, m_editCxformPage(pInstance)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_editGeneralPage);
	if(pInstance->m_obj->IsGObjInstanceOf(gobjShape))
	{
		if(m_editLinePage.HasLines(pInstance->m_obj))
		{
			AddPage(&m_editLinePage);
		}
		if(m_editFillPage.HasFills(pInstance->m_obj))
		{
			AddPage(&m_editFillPage);		
		}
	}
	AddPage(&m_editMatrixPage);
	AddPage(&m_editCxformPage);
}

gld_shape CEditShapeSheet::GetEditShape()
{
	return CTraitShape(m_pInstance);
}

CEditShapeSheet::~CEditShapeSheet()
{
}

BEGIN_MESSAGE_MAP(CEditShapeSheet, CPropertySheet)
END_MESSAGE_MAP()


// CEditShapeSheet message handlers
INT_PTR CEditShapeSheet::DoModal()
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

	// Unexec Line
	for(std::map<int, CCmdModifyLineStyle*>::iterator iLine = m_editLinePage.m_CmdModifyLineStyleList.begin();
		iLine != m_editLinePage.m_CmdModifyLineStyleList.end(); ++iLine)
	{
		++nCmdCount;
		iLine->second->Unexecute();
	}

	// Unexec Fill
	if(m_editFillPage.m_pCmdDeleteImages)
	{
		++nCmdCount;
		m_editFillPage.m_pCmdDeleteImages->Unexecute();
	}
	for(std::map<int, CCmdModifyFillStyle*>::iterator iFill = m_editFillPage.m_CmdModifyFillStyleList.begin();
		iFill != m_editFillPage.m_CmdModifyFillStyleList.end(); ++iFill)
	{
		++nCmdCount;
		iFill->second->Unexecute();
	}
	if(m_editFillPage.m_pCmdImportImages)
	{
		++nCmdCount;
		m_editFillPage.m_pCmdImportImages->Unexecute();
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
			TCommandGroup* pCmd = new TCommandGroup(m_pInstance ? IDS_CMD_MODIFY_INSTANCE : IDS_CMD_MODIFY_BUTTON_MOVIECLIP_SHAPE);

			if(m_editGeneralPage.m_pCmd)
				pCmd->Do(m_editGeneralPage.m_pCmd);
			// Add Line Command
			for(std::map<int, CCmdModifyLineStyle*>::iterator iLine = m_editLinePage.m_CmdModifyLineStyleList.begin();
				iLine != m_editLinePage.m_CmdModifyLineStyleList.end(); ++iLine)
				pCmd->Do(iLine->second);

			// Add Fill Command
			if (m_editFillPage.m_pCmdImportImages)
				pCmd->Do(m_editFillPage.m_pCmdImportImages);
			for(std::map<int, CCmdModifyFillStyle*>::iterator iFill = m_editFillPage.m_CmdModifyFillStyleList.begin();
				iFill != m_editFillPage.m_CmdModifyFillStyleList.end(); ++iFill)
				pCmd->Do(iFill->second);
			if(m_editFillPage.m_pCmdDeleteImages)
				pCmd->Do(m_editFillPage.m_pCmdDeleteImages);

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

		// Delete Line Command
		for(std::map<int, CCmdModifyLineStyle*>::iterator iLine = m_editLinePage.m_CmdModifyLineStyleList.begin();
			iLine != m_editLinePage.m_CmdModifyLineStyleList.end(); ++iLine)
			delete iLine->second;

		// Delete Fill Command
		if(m_editFillPage.m_pCmdDeleteImages)
			delete m_editFillPage.m_pCmdDeleteImages;
		for(std::map<int, CCmdModifyFillStyle*>::iterator iFill = m_editFillPage.m_CmdModifyFillStyleList.begin();
			iFill != m_editFillPage.m_CmdModifyFillStyleList.end(); ++iFill)
			delete iFill->second;
		if(m_editFillPage.m_pCmdImportImages)
			delete m_editFillPage.m_pCmdImportImages;

		// Delete Matrix Command
		if(m_editMatrixPage.m_pCmd)
			delete m_editMatrixPage.m_pCmd;
		// Delete Cxform Command
		if(m_editCxformPage.m_pCmd)
			delete m_editCxformPage.m_pCmd;
	}

	return nRet;
}
