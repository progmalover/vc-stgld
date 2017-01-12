#include "StdAfx.h"
#include "cmdremovesound.h"
#include "EffectWnd.h"
#include "gldSound.h"
#include "glandadoc.h"
#include "my_app.h"
#include <algorithm>

CCmdRemoveSound::CCmdRemoveSound(gldSceneSound* pSceneSound)
: m_pSceneSound(pSceneSound)
{
	_M_Desc_ID = IDS_CMD_CHANGESOUND;

	gldScene2* pScene = _GetCurScene2();
	GSCENESOUND_LIST::iterator iter = std::find(pScene->m_soundList.begin(), pScene->m_soundList.end(), pSceneSound);
	ASSERT(iter != pScene->m_soundList.end());

	++iter;
	m_pSceneSoundRemovedAfter = (iter==pScene->m_soundList.end()) ? NULL : *iter;
}

CCmdRemoveSound::~CCmdRemoveSound(void)
{
	if(Done())
	{
		delete m_pSceneSound;
		m_pSceneSound = NULL;
	}
}

bool CCmdRemoveSound::Execute()
{
	GSCENESOUND_LIST& soundList = _GetCurScene2()->m_soundList;
	if(soundList.size() > 1)
	{
		GSCENESOUND_LIST::iterator iter = std::find(soundList.begin(), soundList.end(), m_pSceneSound);
		if(++iter == soundList.end())
		{
			--iter;
			--iter;
		}
		CEffectWnd::Instance()->SetSoundSelection(*iter, TRUE);
	}
	else
	{
		CEffectWnd::Instance()->SetVoidSelection(TRUE);
	}

	_GetCurScene2()->m_soundList.remove(m_pSceneSound);
	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);

	return TCommand::Execute();
}

bool CCmdRemoveSound::Unexecute()
{
	GSCENESOUND_LIST& soundList = _GetCurScene2()->m_soundList;
	soundList.insert(std::find(soundList.begin(), soundList.end(), m_pSceneSoundRemovedAfter), m_pSceneSound);

	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);
	CEffectWnd::Instance()->SetSoundSelection(m_pSceneSound, TRUE);

	return TCommand::Unexecute();
}


CCmdRemoveAllSounds::CCmdRemoveAllSounds()
{
	_M_Desc_ID = IDS_CMD_CHANGESOUND;
}

CCmdRemoveAllSounds::~CCmdRemoveAllSounds()
{
	for(GSCENESOUND_LIST::iterator iter = m_soundList.begin(); iter != m_soundList.end(); ++iter)
	{
		gldSceneSound* pSceneSound = *iter;
		delete pSceneSound;
	}
}

bool CCmdRemoveAllSounds::Execute()
{
	CEffectWnd::Instance()->SetVoidSelection(TRUE);

	std::swap(_GetCurScene2()->m_soundList, m_soundList);
	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);

	return TCommand::Execute();
}

bool CCmdRemoveAllSounds::Unexecute()
{
	std::swap(_GetCurScene2()->m_soundList, m_soundList);
	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);

	CEffectWnd::Instance()->SetScaptSelection(TRUE);

	return TCommand::Unexecute();
}