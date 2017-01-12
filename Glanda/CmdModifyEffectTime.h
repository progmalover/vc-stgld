#pragma once


#include "command.h"
class gldInstance;
class gldEffect;


class CCmdModifyEffectTime : public TCommand
{
protected:
	gldInstance* m_pInstance;
	gldEffect* m_pEffect;
	int m_nStartTime;
	int m_nLength;
public:
	CCmdModifyEffectTime(gldInstance* pInstance, gldEffect* pEffect, int nStartTime, int nLength);
	virtual ~CCmdModifyEffectTime(void);
	virtual bool Execute();
	virtual bool Unexecute();
	void ExecSwap();
};
