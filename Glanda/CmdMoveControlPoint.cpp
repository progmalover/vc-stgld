#include "StdAfx.h"
#include "cmdmovecontrolpoint.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "DrawHelper.h"

CCmdMoveControlPoint::CCmdMoveControlPoint(const gld_shape &shape, const gld_draw_obj &obj, const gld_c_point &pt, int x, int y)
: CCmdReshapeObj(CExtractObj(shape))
{
	_M_Desc_ID = IDS_CMD_MOVECPOINT;
	
	ASSERT(m_pObj->IsGObjInstanceOf(gobjShape));

	m_DrawObj = obj;
	m_Point = pt;
	m_x = x;
	m_y = y;
}

CCmdMoveControlPoint::~CCmdMoveControlPoint(void)
{
}

bool CCmdMoveControlPoint::Execute()
{
	Do();

	return TCommand::Execute();
}

bool CCmdMoveControlPoint::Unexecute()
{
	Do();
	
	return TCommand::Unexecute();
}

void CCmdMoveControlPoint::Do()
{	
	int x = m_Point.x();
	int y = m_Point.y();
	m_DrawObj.remove(m_Point);
	m_Point.x(m_x);
	m_Point.y(m_y);
	m_DrawObj.insert(m_Point);
	m_x = x;
	m_y = y;

	CGuardDrawOnce	xDraw;
	
	CDrawHelper::UpdateObj(m_pObj);

	UpdateRecord();
	UpdateBoundBox();
	UpdateUniId();

	CDrawHelper::UpdateObj(m_pObj);
}