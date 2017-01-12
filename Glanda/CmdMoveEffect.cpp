#include "StdAfx.h"
#include "CmdMoveEffect.h"
#include "gldInstance.h"
#include "gldEffect.h"
#include "../GlandaCommandRes/resource.h"
#include "Observer.h"
#include <algorithm>

CCmdMoveEffect::CCmdMoveEffect(gldInstance* pInstance, gldEffect* pEffect, gldEffect* pEffectMoveToBefore)
: m_pInstance(pInstance)
, m_pEffect(pEffect)
, m_pEffectMoveToBefore(pEffectMoveToBefore)
{
	_M_Desc_ID = IDS_CMD_MOVE_EFFECT;


	GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
	ASSERT(iter!=pInstance->m_effectList.end());

	++iter;

	if(iter==pInstance->m_effectList.end())
	{
		m_pEffectMoveFromBefore = NULL;
	}
	else
	{
		m_pEffectMoveFromBefore = *iter;
	}
}

CCmdMoveEffect::~CCmdMoveEffect(void)
{
}

bool CCmdMoveEffect::Execute()
{
	GEFFECT_LIST& effectList = m_pInstance->m_effectList;
	effectList.remove(m_pEffect);
	effectList.insert(std::find(effectList.begin(), effectList.end(), m_pEffectMoveToBefore), m_pEffect);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);
	return TCommand::Execute();
}

bool CCmdMoveEffect::Unexecute()
{
	GEFFECT_LIST& effectList = m_pInstance->m_effectList;
	effectList.remove(m_pEffect);
	effectList.insert(std::find(effectList.begin(), effectList.end(), m_pEffectMoveFromBefore), m_pEffect);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);
	return TCommand::Unexecute();
}