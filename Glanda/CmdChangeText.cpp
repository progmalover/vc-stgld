#include "StdAfx.h"
#include "cmdchangetext.h"
#include "TextToolEx.h"
#include "DrawHelper.h"
#include "my_app.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCmdChangeText::CCmdChangeText(gldText2 *pOld, gldText2 *pNew)
: CCmdReshapeObj(pOld)
, m_pOldData(NULL)
, m_pNewData(NULL)
, m_pText(pOld)
{
	ASSERT(pOld && pNew);

	_M_Desc_ID = IDS_CMD_CHANGETEXT;

	VERIFY((m_nOldSize = CSWFProxy::GetObjRowData(pOld, &m_pOldData)) > 0);
	VERIFY((m_nNewSize = CSWFProxy::GetObjRowData(pNew, &m_pNewData)) > 0);

	delete pNew;
}

CCmdChangeText::~CCmdChangeText(void)
{
	delete[] m_pOldData;
	delete[] m_pNewData;
}

void SelectObject(gldObj *pObj)
{
	gld_frame	   frame = my_app.CurFrame();
	gld_layer_iter iLayer = frame.begin_layer();
	gld_shape_sel  sel;
	for (; iLayer != frame.end_layer(); ++iLayer)
	{		
		for (gld_shape_iter iShape = (*iLayer).begin_shape(); iShape != (*iLayer).end_shape(); ++iShape)
		{
			if ((*iShape).ptr() != NULL && (((gldInstance *)CTraitInstance(*iShape))->m_obj == pObj))
				sel.select(*iShape);
		}		
	}
	my_app.DoSelect(sel);
}

bool CCmdChangeText::Execute()
{
	CGuardDrawOnce	xDraw;
	
	CDrawHelper::UpdateObj(m_pText);

	iBinStream	is;
	is.ReadFromMemory(m_nNewSize, m_pNewData);
	m_pText->ClearAll();	
	m_pText->ReadFromBinStream(is);

	UpdateUniId();

	CTransAdaptor::RebuildCurrentScene();
	CDrawHelper::UpdateObj(m_pText);

	SelectObject(m_pText);

	return CCmdReshapeObj::Execute();
}

bool CCmdChangeText::Unexecute()
{
	CGuardDrawOnce	xDraw;
	
	CDrawHelper::UpdateObj(m_pText);

	iBinStream	is;
	is.ReadFromMemory(m_nOldSize, m_pOldData);
	m_pText->ClearAll();	
	m_pText->ReadFromBinStream(is);

	UpdateUniId();

	CTransAdaptor::RebuildCurrentScene();
	CDrawHelper::UpdateObj(m_pText);

	SelectObject(m_pText);

	return CCmdReshapeObj::Unexecute();
}
