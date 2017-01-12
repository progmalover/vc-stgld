#include "StdAfx.h"
#include "cmdreplacebuttonobj.h"
#include "gldCharacter.h"
#include "gldObj.h"
#include "GlandaDoc.h"
#include "my_app.h"
#include "DrawHelper.h"
#include "SWFProxy.h"
#include "TransAdaptor.h"
#include "gldFrameClip.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "SWFProxy.h"
#include "CommandDrawOnce.h"

CCmdReplaceButtonObj::CCmdReplaceButtonObj(gldCharacterKey* pKey, gldObj* pObj, float rx, float ry, int offsetx, int offsety)
: CCmdUpdateObjUniId(pKey->m_parentFC->m_parentLayer->m_parentMC->m_parentObj)
, m_pKey(pKey)
, m_pObj(pObj)
{
	_M_Desc_ID = IDS_CMD_REPLACEOBJ;

	gld_shape shape = CTraitShape(pKey->GetObj());
	gld_rect rect1 = shape.bound();
	shape = CTraitShape(pObj);
	gld_rect rect2 = shape.bound();

	float scalex = (float)rect1.width() / (float)rect2.width();
	float scaley = (float)rect1.height() / (float)rect2.height();
	scalex *= rx;
	scaley *= ry;

	gldMatrix amx;
	int cx1, cy1, cx2, cy2;
	rect1.center(cx1, cy1);
	rect2.center(cx2, cy2);
	amx.m_e11 = scalex;
	amx.m_e22 = scaley;
	amx.m_x = cx1 - cx2 * scalex;
	amx.m_y = cy1 - cy2 * scaley;
	amx.m_x += offsetx;
	amx.m_y += offsety;

	for(int i = 0;i < m_pKey->GetCharacterCount();i++)
		m_matrixList.push_back(m_pKey->GetCharacter(i)->m_matrix);
 
}
CCmdReplaceButtonObj::~CCmdReplaceButtonObj(void)
{
}

bool CCmdReplaceButtonObj::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdReplaceButtonObj::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdReplaceButtonObj::ExecSwap()
{
	gldObj* pObj = m_pKey->GetObj();
	m_pKey->SetObj(m_pObj);
	m_pObj = pObj;

	//GCHARACTER_LIST::iterator iC = m_pKey->GetCharacter(0);
	std::list<gldMatrix>::iterator iM = m_matrixList.begin();
	for(int i = 0; iM != m_matrixList.end(); ++iM, ++i)
	{
		gldCharacter* pCharacter =  m_pKey->GetCharacter(i);
		std::swap(pCharacter->m_matrix, *iM);
	}

	my_app.Redraw();
	my_app.Repaint();
}