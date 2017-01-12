#include "StdAfx.h"
#include ".\cmdaddsound.h"
#include "gldSound.h"
#include "GlandaDoc.h"
#include "gldSceneSound.h"
#include "EffectWnd.h"
#include "my_app.h"
#include <algorithm>

CCmdAddSound::CCmdAddSound(gldSceneSound *pSound)
{
	_M_Desc_ID = IDS_CMD_CHANGESOUND;

	m_pSceneSound = pSound;
}

CCmdAddSound::~CCmdAddSound(void)
{
	if (!_M_done)
		delete m_pSceneSound;
}

bool CCmdAddSound::Execute()
{
	gldScene2 *pScene = _GetCurScene2();
	pScene->m_soundList.push_back(m_pSceneSound);
	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);

	CEffectWnd::Instance()->SetSoundSelection(m_pSceneSound, TRUE);

	return TCommand::Execute();
}

bool CCmdAddSound::Unexecute()
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

	_GetCurScene2()->m_soundList.pop_back();
	CSubjectManager::Instance()->GetSubject("ChangeSceneSound")->Notify(0);

	return TCommand::Unexecute();
}
