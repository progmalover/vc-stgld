#include "StdAfx.h"
#include "cmdmoveanchorpoint.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "DrawHelper.h"

CCmdMoveAnchorPoint::CCmdMoveAnchorPoint(gld_shape &s, const gld_draw_obj &d, gld_list<gld_a_point> &p, int x, int y)
: CCmdReshapeObj(CTraitInstance(s))
{
	_M_Desc_ID = IDS_CMD_MOVEAPOINT;
	
	ASSERT(m_pObj->IsGObjInstanceOf(gobjShape));

	m_DrawObj = d;

	gld_list<gld_a_point>::iterator i = p.begin();
	for (; i != p.end(); ++i) 
	{
		m_Points.push_back(*i);
	}

	m_x = x;
	m_y = y;	
}

CCmdMoveAnchorPoint::~CCmdMoveAnchorPoint(void)
{
}

bool CCmdMoveAnchorPoint::Execute()
{
	Do();

	return CCmdUpdateObj::Execute();
}

bool CCmdMoveAnchorPoint::Unexecute()
{
	Do();

	return CCmdUpdateObj::Unexecute();
}

void CCmdMoveAnchorPoint::Do()
{
	if (m_Points.empty())
	{
		return;
	}

	gld_a_point	ap = *m_Points.begin();
	int x = ap.x();
	int y = ap.y();

	CGuardDrawOnce	xDraw;
	
	CDrawHelper::UpdateObj(m_pObj);

	list<gld_a_point>::iterator i = m_Points.begin();

	for (; i != m_Points.end(); ++i) 
	{
		gld_a_point a = *i;
		m_DrawObj.remove(a);
		a.x(m_x);
		a.y(m_y);
		m_DrawObj.insert(a);
	}

	UpdateBoundBox();
	UpdateRecord();
	UpdateUniId();
	
	CDrawHelper::UpdateObj(m_pObj);
	
	m_x = x;
	m_y = y;
}

