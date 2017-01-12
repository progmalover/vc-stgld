#pragma once
#include "command.h"

class gldSceneSound;

class CCmdModifySceneSoundTime : public TCommand
{
	gldSceneSound* m_pSound;
	int m_nStartTime;
	int m_nLength;
public:
	CCmdModifySceneSoundTime(gldSceneSound* pSound, int nStartTime, int nLength);
	virtual ~CCmdModifySceneSoundTime(void);

	virtual bool Execute();
	virtual bool Unexecute();

	void ExecSwap();
};
