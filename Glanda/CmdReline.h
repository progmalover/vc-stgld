#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"

class CCmdReline : public CCmdReshapeObj
{
private:	
	gld_draw_obj	m_DrawObj;
	gld_edge		m_OldEdge;
	gld_edge		m_NewEdge;

public:
	CCmdReline(const gld_shape &shape, const gld_draw_obj &obj, const gld_edge &edge, double u, int x, int y);
	virtual ~CCmdReline();

	virtual bool Execute();
	virtual bool Unexecute();
};
