#include "StdAfx.h"
#include "cmdmodifyscenesoundtime.h"
#include "gldSceneSound.h"
#include "Observer.h"

CCmdModifySceneSoundTime::CCmdModifySceneSoundTime(gldSceneSound* pSound, int nStartTime, int nLength)
: m_pSound(pSound)
, m_nStartTime(nStartTime)
, m_nLength(nLength)
{
	_M_Desc_ID = IDS_CMD_MODIFY_SCENE_SOUND_TIME;
}

CCmdModifySceneSoundTime::~CCmdModifySceneSoundTime(void)
{
}

bool CCmdModifySceneSoundTime::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdModifySceneSoundTime::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdModifySceneSoundTime::ExecSwap()
{
	std::swap(m_pSound->m_startTime, m_nStartTime);
	std::swap(m_pSound->m_length, m_nLength);
	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);
}