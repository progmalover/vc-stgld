#pragma once
#include "command.h"
#include "gldSceneSound.h"
#include "gldSceneSound.h"

class CCmdRemoveSound :
	public TCommand
{
	gldSceneSound* m_pSceneSound;
	gldSceneSound* m_pSceneSoundRemovedAfter;
public:
	CCmdRemoveSound(gldSceneSound* pSceneSound);
	virtual ~CCmdRemoveSound(void);

	virtual bool Execute();
	virtual bool Unexecute();
};


class CCmdRemoveAllSounds : public TCommand
{
	GSCENESOUND_LIST m_soundList;
public:
	CCmdRemoveAllSounds();
	virtual ~CCmdRemoveAllSounds();

	virtual bool Execute();
	virtual bool Unexecute();
};