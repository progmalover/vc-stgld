#include "StdAfx.h"
#include ".\cmdmodifyeffecttime.h"
#include "gldEffect.h"
#include "Observer.h"

CCmdModifyEffectTime::CCmdModifyEffectTime(gldInstance* pInstance, gldEffect* pEffect, int nStartTime, int nLength)
: m_pInstance(pInstance)
, m_pEffect(pEffect)
, m_nStartTime(nStartTime)
, m_nLength(nLength)
{
	_M_Desc_ID = IDS_CMD_MODIFY_EFFECT_TIME;
}

CCmdModifyEffectTime::~CCmdModifyEffectTime(void)
{
}

bool CCmdModifyEffectTime::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdModifyEffectTime::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdModifyEffectTime::ExecSwap()
{
	std::swap(m_pEffect->m_startTime, m_nStartTime);
	std::swap(m_pEffect->m_length, m_nLength);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);
}