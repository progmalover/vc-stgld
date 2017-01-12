#include "StdAfx.h"
#include "cmdrenamescene.h"
#include "GlandaDoc.h"
#include "Observer.h"

CCmdRenameScene::CCmdRenameScene(LPCTSTR lpszName)
: m_strName(lpszName)
, m_strOldName(_GetCurScene2()->m_name.c_str())
{
	_M_Desc_ID = IDS_CMD_RENAMESCENE;
}

CCmdRenameScene::~CCmdRenameScene(void)
{
}

bool CCmdRenameScene::Execute()
{
	_GetCurScene2()->m_name = (LPCTSTR)m_strName;
	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);
	return TCommand::Execute();
}

bool CCmdRenameScene::Unexecute()
{
	_GetCurScene2()->m_name = (LPCTSTR)m_strOldName;
	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);
	return TCommand::Unexecute();
}
