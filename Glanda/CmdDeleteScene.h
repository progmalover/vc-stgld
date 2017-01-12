#pragma once
#include "command.h"

class gldScene2;

class CCmdChangeCurrentScene;

class CCmdDeleteScene :	public TCommand
{
protected:
	int m_nIndex;	// 被删除的场景的索引号
	gldScene2* m_pScene;
	bool m_bAutoDelete;

public:
	CCmdDeleteScene(gldScene2* pScene, bool bAutoDelete);
	virtual ~CCmdDeleteScene(void);
	virtual bool Execute();
	virtual bool Unexecute();
};
