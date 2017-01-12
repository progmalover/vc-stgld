#pragma once
#include "command.h"

class gldSound;
class gldSoundInstance;

class CCmdReplaceSound : public TCommand
{
	gldSound* m_pSoundObj;
	gldSoundInstance* m_pSoundInstance;
public:
	CCmdReplaceSound(gldSound* pSoundObj, gldSoundInstance* pSoundInstance);
	virtual ~CCmdReplaceSound(void);
	virtual bool Execute();
	virtual bool Unexecute();
	void ExecSwap();
};
