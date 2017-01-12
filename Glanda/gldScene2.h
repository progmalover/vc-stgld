#pragma once

#include <string>
#include <list>
#include <vector>
#include "gldInstance.h"
#include "gldSceneSound.h"
#include "Command.h"
#include "Observer.h"
#include "gldObj.h"

class CBackground;
class gldScene2
{
public:
	gldScene2(void);
	~gldScene2(void);

	enum
	{
		MAX_SCENE_NAME = 255, 
		MIN_SCENE_TIME = 0, 
		MAX_SCENE_TIME = USHRT_MAX
	};

public:
	int m_specifiedFrames;
	std::string m_name;
	GINSTANCE_LIST m_instanceList;
	std::string m_actionEnter;
	std::string m_actionExit;
	CBackground *m_backgrnd;

	GSCENESOUND_LIST m_soundList;
	BOOL m_soundExpanded;

	std::vector<int> m_hLines;
    std::vector<int> m_vLines;

public:
	gldInstance *GetInstance(int index);
	gldSceneSound *GetSound(int index);
	int GetMaxTime(BOOL bIgnoreSpecifiedTimed);
	int GetEffectCount();
	void GetUsedObjs(GOBJECT_LIST &lstObj);	
	HRESULT WriteToStream(IStream *pStm, void *pUnk);
	HRESULT ReadFromStream(IStream *pStm, void *pUnk);
};

typedef std::list <gldScene2*> GSCENE2_LIST;

#include "Command.h"
class CCmdSetAction : public TCommand
{
public:
	CCmdSetAction(gldInstance *pInstance, LPCTSTR lpszAction)
	{
		_M_Desc_ID = IDS_CMD_SETACTIONSCRIPT;

		m_inst = pInstance;
		m_old = pInstance->m_action;
		m_new = lpszAction;
	}
	bool Execute()
	{
		m_inst->m_action = m_new;
		return TCommand::Execute();
	}
	bool Unexecute()
	{
		m_inst->m_action = m_old;
		return TCommand::Unexecute();
	}

private:
	gldInstance *m_inst;
	std::string m_old;
	std::string m_new;
};

class CCmdSetSceneAction : public TCommand
{
public:
	CCmdSetSceneAction(gldScene2 *pScene, LPCTSTR lpszAction, bool bEnter /*EnterSceneActionScript ot ExitSceneActionScript*/)
	{
		_M_Desc_ID = IDS_CMD_SETACTIONSCRIPT;

		m_scene = pScene;
		m_enter = bEnter;
		m_old = m_enter ? pScene->m_actionEnter : pScene->m_actionExit;
		m_new = lpszAction;
	}
	bool Execute()
	{
		if (m_enter)
			m_scene->m_actionEnter = m_new;
		else
			m_scene->m_actionExit = m_new;
		return TCommand::Execute();
	}
	bool Unexecute()
	{
		if (m_enter)
			m_scene->m_actionEnter = m_old;
		else
			m_scene->m_actionExit = m_old;
		return TCommand::Unexecute();
	}

private:
	gldScene2 *m_scene;
	bool m_enter;
	std::string m_old;
	std::string m_new;
};

class CCmdChangeSceneName : public TCommand
{
public:
	CCmdChangeSceneName(gldScene2 *pScene, LPCTSTR lpszNewName)
	{
		_M_Desc_ID = IDS_CMD_CHANGESCENENAME;

		m_pScene = pScene;
		m_sNameOld = pScene->m_name;
		m_sNameNew = lpszNewName;
	}
	~CCmdChangeSceneName()
	{
	}
	virtual bool Execute()
	{
		m_pScene->m_name = m_sNameNew;

		CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);

		return TCommand::Execute();
	}
	virtual bool Unexecute()
	{
		m_pScene->m_name = m_sNameOld;

		CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);

		return TCommand::Unexecute();
	}
private:
	gldScene2 *m_pScene;
	std::string m_sNameOld;
	std::string m_sNameNew;
};


class CCmdChangeSceneLength : public TCommand
{
public:
	CCmdChangeSceneLength(gldScene2 *pScene, int nFrames)
	{
		_M_Desc_ID = IDS_CMD_CHANGESCENEPROPERTIES;

		m_pScene = pScene;
		m_nFramesOld = pScene->m_specifiedFrames;
		m_nFramesNew = nFrames;
	}
	~CCmdChangeSceneLength()
	{
	}
	virtual bool Execute()
	{
		m_pScene->m_specifiedFrames = m_nFramesNew;

		CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);

		return TCommand::Execute();
	}
	virtual bool Unexecute()
	{
		m_pScene->m_specifiedFrames = m_nFramesOld;

		CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);

		return TCommand::Unexecute();
	}
private:
	gldScene2 *m_pScene;
	int m_nFramesOld;
	int m_nFramesNew;
};
