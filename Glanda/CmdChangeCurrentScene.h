#pragma once
#include "command.h"

class gldScene2;

class CCmdChangeCurrentScene :
	public TCommand
{
protected:
	gldScene2* m_pNewScene;
	gldScene2* m_pOldScene;
public:
	CCmdChangeCurrentScene(gldScene2* pScene);
	virtual ~CCmdChangeCurrentScene(void);
	virtual bool Execute();
	virtual bool Unexecute();
	virtual bool CanMix(TCommand *pOther);
	virtual TCommand *Mix(TCommand *pOther);
};
