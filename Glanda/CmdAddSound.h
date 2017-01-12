#pragma once
#include "command.h"

class gldSceneSound;

class CCmdAddSound :
	public TCommand
{
public:
	CCmdAddSound(gldSceneSound *pSound);
	~CCmdAddSound(void);

	virtual bool Execute();
	virtual bool Unexecute();

private:
	gldSceneSound *m_pSceneSound;
};
