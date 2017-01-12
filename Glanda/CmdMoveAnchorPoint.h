#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"
#include <list>

using namespace std;

class CCmdMoveAnchorPoint : public CCmdReshapeObj
{
public:
	CCmdMoveAnchorPoint(gld_shape &s, const gld_draw_obj &d, gld_list<gld_a_point> &p, int x, int y);
	virtual ~CCmdMoveAnchorPoint(void);

	virtual bool Execute();
	virtual bool Unexecute();	

protected:
	void Do();

private:
	gld_draw_obj			m_DrawObj;
	list<gld_a_point>		m_Points;
	int						m_x, m_y;	
};
