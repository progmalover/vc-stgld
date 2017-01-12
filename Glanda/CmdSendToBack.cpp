#include "StdAfx.h"
#include "cmdsendtoback.h"
#include "TransAdaptor.h"
#include "gldMovieClip.h"
#include "gldDataKeeper.h"
#include "SWFProxy.h"
#include "GroupUpdate.h"
#include "Selection.h"
#include "my_app.h"
#include "gldScene2.h"
#include "DrawHelper.h"

CCmdSendToBack::CCmdSendToBack(const gld_shape_sel &sel, int nType)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
{
	m_Data.BuildFromSelection(sel);

	ASSERT(nType == 0 || nType == 1);

	m_nType = nType;
	if (m_nType == 0)
	{
		_M_Desc_ID = IDS_CMD_SENDBACKWARD;
	}
	else
	{
		_M_Desc_ID = IDS_CMD_SENDTOBACK;
	}

	m_Bound = sel.SurroundBox();
}

CCmdSendToBack::~CCmdSendToBack(void)
{
}

bool CCmdSendToBack::Execute()
{
	if (m_nType == 0) // Send backward
	{
		gldScene2 *pScene = m_Data.m_pScene;
		list<int>::iterator	iz = m_Data.m_lstZorder.begin();
		int					zorder = 0;
		GINSTANCE_LIST::iterator ick = pScene->m_instanceList.begin();

		for (; iz != m_Data.m_lstZorder.end(); ++iz)
		{
			for (; zorder != *iz; ++zorder, ++ick)
			{
				ASSERT(ick != pScene->m_instanceList.end());
			}
			GINSTANCE_LIST::iterator pos = ick;
			if (--pos != pScene->m_instanceList.end())
			{
				pScene->m_instanceList.insert(pos, *ick);
				pScene->m_instanceList.erase(ick);
				ick = pos;
			}
			++ick;
			++zorder;
		}		
	}
	else	// Send to back
	{
		gldScene2 *pScene = m_Data.m_pScene;
		list<int>::iterator	iz = --m_Data.m_lstZorder.end();
		int					zorder = (int)pScene->m_instanceList.size() - 1;
		GINSTANCE_LIST::iterator ick = --pScene->m_instanceList.end();

		for (; iz != m_Data.m_lstZorder.end(); --iz)
		{
			for (; zorder != *iz; --zorder, --ick)
			{
				ASSERT(ick != pScene->m_instanceList.end());
			}
			pScene->m_instanceList.push_front(*ick);
			pScene->m_instanceList.erase(ick--);
			--zorder;
		}		
	}

	UpdateUniId();

	CGuardDrawOnce xDraw;
	CGuardSelKeeper	xSel;

	CTransAdaptor::RebuildCurrentScene();

	my_app.Redraw(m_Bound);
	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdSendToBack::Unexecute()
{
	if (m_nType == 0) // Send backward
	{
		gldScene2 *pScene = m_Data.m_pScene;
		list<int>::iterator	iz = --m_Data.m_lstZorder.end();
		int					zorder = (int)pScene->m_instanceList.size() - 1;
		GINSTANCE_LIST::iterator ick = --pScene->m_instanceList.end();

		for (; iz != m_Data.m_lstZorder.end(); --iz)
		{
			for (; zorder != *iz; --zorder, --ick)
			{
				ASSERT(ick != pScene->m_instanceList.end());
			}
			GINSTANCE_LIST::iterator val = ick;
			--val;
			if (val != pScene->m_instanceList.end())
			{
				GINSTANCE_LIST::iterator pos = ick;
				++pos;
				pScene->m_instanceList.insert(pos, *val);
				pScene->m_instanceList.erase(val);
			}
			else
			{
				--ick;
			}
			--zorder;
		}		
	}
	else	// Send to back
	{
		gldScene2 *pScene = m_Data.m_pScene;
		list<int>::iterator	iz = m_Data.m_lstZorder.begin();
		int					zorder = 0;
		GINSTANCE_LIST::iterator ick1 = pScene->m_instanceList.begin();
		GINSTANCE_LIST::iterator ick2 = pScene->m_instanceList.begin();
		int					count = (int)m_Data.m_lstZorder.size();

		while ((count--) > 0)
		{
			++ick1;
		}

		for (; iz != m_Data.m_lstZorder.end(); ++iz)
		{
			for (; zorder != *iz; ++zorder, ++ick1)
			{
				ASSERT(ick1 != pScene->m_instanceList.end());
			}
			if (ick1 != ick2)
			{
				pScene->m_instanceList.insert(ick1, *ick2);
				pScene->m_instanceList.erase(ick2++);
			}
			else
			{
				ick1++;
				ick2++;
			}
			++zorder;
		}		
	}

	UpdateUniId();

	CGuardDrawOnce xDraw;
	CGuardSelKeeper	xSel;

	CTransAdaptor::RebuildCurrentScene();

	my_app.Redraw(m_Bound);
	my_app.Repaint();

	return TCommand::Unexecute();
}