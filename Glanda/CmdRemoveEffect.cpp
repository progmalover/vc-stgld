#include "StdAfx.h"
#include "cmdremoveeffect.h"
#include "gldInstance.h"
#include "gldEffect.h"
#include <algorithm>
#include "EffectWnd.h"
#include "my_app.h"
#include "SWFProxy.h"


CCmdRemoveEffect::CCmdRemoveEffect(gldInstance* pInstance, gldEffect* pEffect)
: m_pInstance(pInstance)
, m_pEffect(pEffect)
{
	_M_Desc_ID = IDS_CMD_REMOVEEFFECT;

	GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
	ASSERT(iter != pInstance->m_effectList.end());

	++iter;
	m_pEffectRemovedAfter = (iter==pInstance->m_effectList.end()) ? NULL : *iter;
}

CCmdRemoveEffect::~CCmdRemoveEffect(void)
{
	if(Done())
	{
		delete m_pEffect;
		m_pEffect = NULL;
	}
}

bool CCmdRemoveEffect::Execute()
{
	GEFFECT_LIST& effectList = m_pInstance->m_effectList;
	if(effectList.size() > 1)
	{
		GEFFECT_LIST::iterator iter = std::find(effectList.begin(), effectList.end(), m_pEffect);
		if(++iter == effectList.end())
		{
			--iter;
			--iter;
		}
		CEffectWnd::Instance()->SetEffectSelection(m_pInstance, *iter, TRUE);
	}
	else
	{
		CEffectWnd::Instance()->SetInstSelection(m_pInstance, TRUE);
	}

	m_pInstance->m_effectList.remove(m_pEffect);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);

	return TCommand::Execute();
}

bool CCmdRemoveEffect::Unexecute()
{
	GEFFECT_LIST& effectList = m_pInstance->m_effectList;
	effectList.insert(std::find(effectList.begin(), effectList.end(), m_pEffectRemovedAfter), m_pEffect);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);

	CEffectWnd::Instance()->SetEffectSelection(m_pInstance, m_pEffect, TRUE);

	return TCommand::Unexecute();
}

CCmdRemoveAllEffects::CCmdRemoveAllEffects(gldInstance* pInstance)
: m_pInstance(pInstance)
{
	_M_Desc_ID = IDS_CMD_REMOVEEFFECT;
}

CCmdRemoveAllEffects::~CCmdRemoveAllEffects()
{
	for(GEFFECT_LIST::iterator iter = m_effectList.begin(); iter != m_effectList.end(); ++iter)
	{
		gldEffect* pEffect = *iter;
		delete pEffect;
	}
}

bool CCmdRemoveAllEffects::Execute()
{
	CEffectWnd::Instance()->SetInstSelection(m_pInstance, TRUE);

	std::swap(m_pInstance->m_effectList, m_effectList);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);

	return TCommand::Execute();
}

bool CCmdRemoveAllEffects::Unexecute()
{
	std::swap(m_pInstance->m_effectList, m_effectList);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);

	CEffectWnd::Instance()->SetInstSelection(m_pInstance, TRUE);

	return TCommand::Unexecute();
}