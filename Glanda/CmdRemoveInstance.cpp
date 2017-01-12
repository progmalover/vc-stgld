#include "StdAfx.h"
#include "cmdremoveinstance.h"
#include "gldCharacter.h"
#include "gldFrameClip.h"
#include "gldMovieClip.h"
#include "TransAdaptor.h"
#include "gldLayer.h"
#include "my_app.h"
#include "GroupUpdate.h"
#include "SWFProxy.h"
#include "DrawHelper.h"

#include "GlandaDoc.h"
#include "gldScene2.h"
#include "gldInstance.h"

CCmdRemoveInstance::CCmdRemoveInstance(const gld_shape_sel &sel, bool bAutoDelete)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
, m_bAutoDelete(bAutoDelete)
{
	_M_Desc_ID = IDS_CMD_REMOVEINSTANCE;

	for (gld_shape_sel::iterator it = sel.begin(); it != sel.end(); it++)
		m_instanceList.push_back(CTraitInstance(*it));
}

CCmdRemoveInstance::~CCmdRemoveInstance(void)
{
	if (m_bAutoDelete)
	{
		if (_M_done)
			for (GINSTANCE_LIST::iterator it = m_instanceList.begin(); it != m_instanceList.end(); ++it)
				delete *it;
	}	
}

bool CCmdRemoveInstance::Execute()
{
	//CGroupUpdate	xUpdate;

	gldScene2 *pScene = _GetCurScene2();

	m_lstZorder.clear();

	GINSTANCE_LIST::iterator it = m_instanceList.begin();

	for (; it != m_instanceList.end(); ++it)
	{
		//gldCharacterKey *pInstance = *it;
		gldInstance *pInstance = *it;
		//gldFrameClip	*pframe = pInstance->m_parentFC;
		//gldLayer		*player = pframe->m_parentLayer;		

		//CDrawHelper::UpdateCharacter((gld_shape)CTraitShape(pInstance));

		// remove character key from frame clip and save zorder
		//GCHARACTERKEY_LIST::iterator iobj = pframe->m_characterKeyList.begin();
		int	nZorder = 0;
		GINSTANCE_LIST::iterator iobj = pScene->m_instanceList.begin();
		for (; iobj != pScene->m_instanceList.end(); ++iobj)
		{
			if (*iobj == pInstance)
			{
				break;
			}

			nZorder++;
		}
		ASSERT(iobj != pScene->m_instanceList.end());

		pScene->m_instanceList.erase(iobj);		

		m_lstZorder.push_back(nZorder);

		//UpdateFrameClip(pframe);

		// send notify
		//CSubjectChangeMovie *pSubject = (CSubjectChangeMovie *)CSubjectManager::Instance()->GetSubject("ChangeMovie");
		//pSubject->Assign(typeRemoveInstance, player->m_parentMC->m_parentObj,
		//	player, pframe, pInstance);
		//pSubject->Notify(pSubject);
	}	

	UpdateUniId();
	
	//if (CSWFProxy::TimeInFrameClip(pfc, CSWFProxy::GetCurTime()))
	//{
		CTransAdaptor::RebuildCurrentScene();
		//CDrawHelper::UpdateCharacter((gld_shape)CTraitShape(m_pCharKey));
		my_app.Redraw();
		my_app.Repaint();
	//}
	//CTransAdaptor::RebuildCurrentScene();

	return TCommand::Execute();
}

bool CCmdRemoveInstance::Unexecute()
{
	gldScene2 *pScene = _GetCurScene2();

	GINSTANCE_LIST::iterator it = --m_instanceList.end();
	gld_list<int>::iterator iOrder = m_lstZorder.rbegin();

	for (; it != m_instanceList.end(); --it, --iOrder)
	{
		// insert key at some position
		//gldCharacterKey *pInstance = *it;
		//gldFrameClip	*pframe = pInstance->m_parentFC;
		//gldLayer		*player = pframe->m_parentLayer;
		gldInstance *pInstance = *it;
		
		// find insert position
		//GCHARACTERKEY_LIST::iterator iobj = pframe->m_characterKeyList.begin();
		int	nZorder = 0;
		GINSTANCE_LIST::iterator iobj = pScene->m_instanceList.begin();
		while (nZorder < *iOrder)
		{
			ASSERT(iobj != pScene->m_instanceList.end());

			++iobj;
			++nZorder;
		}

		// Add the key to the key list
		pScene->m_instanceList.insert(iobj, pInstance);
		
		//UpdateFrameClip(pframe);

		// send notify
		//CSubjectChangeMovie *pSubject = (CSubjectChangeMovie *)CSubjectManager::Instance()->GetSubject("ChangeMovie");
		//pSubject->Assign(typeInsertInstance, player->m_parentMC->m_parentObj,
		//	player, pframe, pInstance);
		//pSubject->Notify(pSubject);
	}

	UpdateUniId();

	//CGroupUpdate	xUpdate;
	//CTransAdaptor::RebuildCurrentScene();	
	//for (it = m_lstKey.begin(); it != m_lstKey.end(); ++it)
	//{
	//	CDrawHelper::UpdateCharacter((gld_shape)CTraitShape(*it));
	//}
	//if (CSWFProxy::TimeInFrameClip(pfc, CSWFProxy::GetCurTime()))
	//{
		CTransAdaptor::RebuildCurrentScene();
		//CDrawHelper::UpdateCharacter((gld_shape)CTraitShape(m_pCharKey));
		my_app.Redraw();
		my_app.Repaint();
	//}

	return TCommand::Unexecute();
}

void CCmdRemoveInstance::UpdateFrameClip(gldFrameClip *pframe)
{	
	ASSERT(FALSE);

	/*
	if (pframe->m_characterKeyList.size() == 0)
	{
		pframe->m_flag &= ~FC_CHARACTER;
	}
	else
	{
		pframe->m_flag |= FC_CHARACTER;
	}

	CSWFProxy::UpdateFrameClip(pframe);
	gldFrameClip *pPrevFC = NULL;
	if (pPrevFC = CSWFProxy::GetPrevFrameClip(pframe))
	{
		CSWFProxy::UpdateFrameClip(pPrevFC);
	}
	*/
}
