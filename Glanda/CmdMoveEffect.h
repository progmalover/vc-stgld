#pragma once
#include "command.h"

class gldInstance;
class gldEffect;

class CCmdMoveEffect :
	public TCommand
{
protected:
	gldInstance* m_pInstance;
	gldEffect* m_pEffect;
	gldEffect* m_pEffectMoveToBefore;
	gldEffect* m_pEffectMoveFromBefore;
public:
	CCmdMoveEffect(gldInstance* pInstance, gldEffect* pEffect, gldEffect* pEffectMoveToBefore);
	virtual ~CCmdMoveEffect(void);
	virtual bool Execute();
	virtual bool Unexecute();
};
