#include "StdAfx.h"
#include "cmdinsertscene.h"
#include "GlandaDoc.h"
#include "SceneListCtrl.h"
#include "CmdChangeCurrentScene.h"

CCmdInsertScene::CCmdInsertScene(LPCTSTR lpszName, int nIndex)
{
	_M_Desc_ID = IDS_CMD_ADDSCENE;

	m_nIndex = nIndex;
	ASSERT(m_nIndex >= 0);

	m_pScene = new gldScene2();
	m_pScene->m_name = lpszName;

	m_bAutoDelete = true;
}

CCmdInsertScene::CCmdInsertScene(gldScene2 *pScene, int nIndex, bool bAutoDelete)
{
	_M_Desc_ID = IDS_CMD_ADDSCENE;

	m_nIndex = nIndex;
	ASSERT(m_nIndex >= 0);

	m_pScene = pScene;
	
	// Move scene command (Delete + Insert) should not delete the scene 
	// in dector
	m_bAutoDelete = bAutoDelete;
}

CCmdInsertScene::~CCmdInsertScene(void)
{
	// 销毁场景
	if(!Done() && m_bAutoDelete)
		delete m_pScene;
	m_pScene = NULL;
}

bool CCmdInsertScene::Execute()
{
	ASSERT(m_pScene);

	// 查找要插入的位置
	GSCENE2_LIST::iterator iter = _GetMainMovie2()->m_sceneList.begin();
	for(int i=m_nIndex; i>0; --i)
	{
		++iter;
	}

	// 插入到数据链中
	_GetMainMovie2()->m_sceneList.insert(iter, m_pScene);
	//插入用户SCENCE浮动面板
	CSceneListCtrl::Instance()->InsertScene(m_nIndex, m_pScene);
	
	return TCommand::Execute();
}

bool CCmdInsertScene::Unexecute()
{
	// 从数据结构和场景列表中删除场景
	_GetMainMovie2()->m_sceneList.remove(m_pScene);
	CSceneListCtrl::Instance()->DeleteItem(m_nIndex);

	return TCommand::Unexecute();
}
