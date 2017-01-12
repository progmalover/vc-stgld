#pragma once
#include "command.h"

class gldScene2;
class CCmdChangeCurrentScene;

class CCmdInsertScene :	public TCommand
{
protected:
	gldScene2* m_pScene;
	int m_nIndex;	// 要插入的位置
	bool m_bAutoDelete;
public:
	CCmdInsertScene(LPCTSTR lpszName, int nIndex);	// >= m_sceneList.size()为添加到尾部
	CCmdInsertScene(gldScene2 *pScene, int nIndex, bool bAutoDelete = true);
	virtual ~CCmdInsertScene(void);
	virtual bool Execute();
	virtual bool Unexecute();
};
