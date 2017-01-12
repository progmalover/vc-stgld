#include "StdAfx.h"
#include "cmddeletescene.h"
#include "GlandaDoc.h"
#include "SceneListCtrl.h"

CCmdDeleteScene::CCmdDeleteScene(gldScene2* pScene, bool bAutoDelete)
: m_pScene(pScene)
{
	_M_Desc_ID = IDS_CMD_DELETESCENE;

	// 记录场景的索引号
	m_nIndex = CSceneListCtrl::Instance()->FindScenePos(pScene);
	ASSERT(m_nIndex>=0);
	
	// Move scene command (Delete + Insert) should not delete the scene 
	// in dector
	m_bAutoDelete = bAutoDelete;
}

CCmdDeleteScene::~CCmdDeleteScene(void)
{
	if (Done() && m_bAutoDelete)
	{
		delete m_pScene;
		m_pScene = NULL;
	}
}

bool CCmdDeleteScene::Execute()
{
	// 从数据结构和场景列表中删除场景
	CSceneListCtrl* pSceneListCtrl = CSceneListCtrl::Instance();
	_GetMainMovie2()->m_sceneList.remove(m_pScene);
	pSceneListCtrl->DeleteItem(m_nIndex);

	return TCommand::Execute();
}

bool CCmdDeleteScene::Unexecute()
{
	// 查找要插入的位置
	GSCENE2_LIST::iterator iter = _GetMainMovie2()->m_sceneList.begin();
	for(int i=m_nIndex; i>0; --i)
	{
		++iter;
	}

	// 将场景重新插入到数据结构和场景列表中
	_GetMainMovie2()->m_sceneList.insert(iter, m_pScene);
	CSceneListCtrl::Instance()->InsertScene(m_nIndex, m_pScene);

	return TCommand::Unexecute();
}