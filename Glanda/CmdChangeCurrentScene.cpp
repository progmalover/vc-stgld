#include "StdAfx.h"
#include "cmdchangecurrentscene.h"
#include "gldScene2.h"
#include "gldDataKeeper.h"
#include "GlandaDoc.h"
#include "SceneListCtrl.h"
#include "TransAdaptor.h"
#include "my_app.h"


CCmdChangeCurrentScene::CCmdChangeCurrentScene(gldScene2* pScene)
: m_pNewScene(pScene)
, m_pOldScene(_GetCurScene2())
{
	_M_Desc_ID = IDS_CMD_CHANGECURRENTSCENE;
}

CCmdChangeCurrentScene::~CCmdChangeCurrentScene(void)
{
}

bool CCmdChangeCurrentScene::Execute()
{
	if (m_pOldScene)
		CTransAdaptor::ClearCurrentScene();
	
	_GetMainMovie2()->SetCurScene(m_pNewScene, TRUE);

	if (m_pNewScene)
		CTransAdaptor::BuildCurrentScene(false);

	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdChangeCurrentScene::Unexecute()
{
	if (m_pNewScene)
		CTransAdaptor::ClearCurrentScene();
	
	_GetMainMovie2()->SetCurScene(m_pOldScene, TRUE);
	
	if (m_pOldScene)
		CTransAdaptor::BuildCurrentScene(false);

	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Unexecute();
}

bool CCmdChangeCurrentScene::CanMix(TCommand *pOther)
{
	return (pOther && pOther->GetDescID() == _M_Desc_ID);	
}

TCommand *CCmdChangeCurrentScene::Mix(TCommand *pOther)
{
	if (pOther && pOther->GetDescID() == _M_Desc_ID)
	{
		CCmdChangeCurrentScene *cmd = (CCmdChangeCurrentScene *)pOther;
		
		m_pNewScene = cmd->m_pNewScene;

		delete cmd;

		return this;		
	}

	return NULL;
}