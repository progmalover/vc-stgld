#pragma once
#include "command.h"
#include "gldEffect.h"


class gldInstance;

class CCmdRemoveEffect : public TCommand
{
protected:
	gldInstance* m_pInstance;
	gldEffect* m_pEffect;
	gldEffect* m_pEffectRemovedAfter;
public:
	CCmdRemoveEffect(gldInstance* pInstance, gldEffect* pEffect);
	virtual ~CCmdRemoveEffect(void);
	virtual bool Execute();
	virtual bool Unexecute();
};

class CCmdRemoveAllEffects : public TCommand
{
protected:
	gldInstance* m_pInstance;
	GEFFECT_LIST m_effectList;
public:
	CCmdRemoveAllEffects(gldInstance* pInstance);
	virtual ~CCmdRemoveAllEffects();
	virtual bool Execute();
	virtual bool Unexecute();
};