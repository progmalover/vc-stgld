#include "StdAfx.h"
#include "changezorder.h"
#include "GlandaDoc.h"
#include "SWFProxy.h"

CChangeZorder::CChangeZorder(void)
{
	m_pScene = NULL;
}

CChangeZorder::~CChangeZorder(void)
{
}

void CChangeZorder::BuildFromSelection(const gld_shape_sel &sel)
{
	m_pScene = _GetCurScene2();
	
	ASSERT(m_pScene != NULL);

	m_lstZorder.clear();

	gld_shape_sel::iterator i = sel.begin();

	for (; i != sel.end(); ++i)
	{
		AddData(CTraitInstance(*i));		
	}	
}

void CChangeZorder::AddData(gldInstance *pInst)
{
	int zorder = 0;

	GINSTANCE_LIST::iterator ick = m_pScene->m_instanceList.begin();

	for (; ick != m_pScene->m_instanceList.end(); ++ick, ++zorder)
	{
		if (*ick == pInst)
		{
			break;
		}
	}

	ASSERT(ick != m_pScene->m_instanceList.end());
	
	// Insert new zorder to list
	list<int>::iterator iz = m_lstZorder.begin();

	for (; iz != m_lstZorder.end(); ++iz)
	{
		if (*iz > zorder)
		{
			break;
		}
	}

	m_lstZorder.insert(iz, zorder);	
}