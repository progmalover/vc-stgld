// EditMCShapeSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCShapeSheet.h"
#include "gldObj.h"
#include "Command.h"
#include "DrawHelper.h"


// CEditMCShapeSheet

IMPLEMENT_DYNAMIC(CEditMCShapeSheet, CPropertySheet)
CEditMCShapeSheet::CEditMCShapeSheet(gldObj* pObj)
	:CPropertySheet(IDS_EDIT_MC_SHAPE_SHEET_CAPTION)
	, m_pObj(pObj)
	, m_pCmd(NULL)
	, m_editLinePage(pObj)
	, m_editFillPage(pObj)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	if(m_editLinePage.HasLines(pObj))
	{
		AddPage(&m_editLinePage);
	}
	if(m_editFillPage.HasFills(pObj))
	{
		AddPage(&m_editFillPage);		
	}
}

CEditMCShapeSheet::~CEditMCShapeSheet()
{
}


BEGIN_MESSAGE_MAP(CEditMCShapeSheet, CPropertySheet)
END_MESSAGE_MAP()


// CEditMCShapeSheet message handlers

INT_PTR CEditMCShapeSheet::DoModal()
{
	INT_PTR nRet = CPropertySheet::DoModal();

	int nCmdCount = 0;
	CGuardDrawOnce xDraw;

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

	if (nRet == IDOK)
	{
		if(nCmdCount)
		{
			TCommandGroup* pCmd = new TCommandGroup();

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

			m_pCmd = new TCommandGroup();
			m_pCmd->Do(pCmd);
		}
	}
	else
	{
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
	}

	return nRet;
}
