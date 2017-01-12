#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"

class CCmdMoveControlPoint : public CCmdReshapeObj
{
public:
	CCmdMoveControlPoint(const gld_shape &shape, const gld_draw_obj &obj, const gld_c_point &pt, int x, int y);
	virtual ~CCmdMoveControlPoint(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();

protected:
	void Do();

protected:	
	gld_draw_obj	m_DrawObj;
	gld_c_point		m_Point;
	int				m_x, m_y;	
};
