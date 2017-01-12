#pragma once

#include "CmdUpdateObj.h"
#include "gld_graph.h"

class CCmdInsertAnchorPoint : public CCmdReshapeObj
{
private:	
	gld_draw_obj	m_DrawObj;
	gld_edge		m_OldEdge;
	gld_edge		m_NewEdge1;
	gld_edge		m_NewEdge2;	

public:
	CCmdInsertAnchorPoint(const gld_shape &shape, const gld_draw_obj &obj, const gld_edge &edge, int x, int y);
	virtual ~CCmdInsertAnchorPoint();

	virtual bool Execute();
	virtual bool Unexecute();

protected:
	void DestroyEdge(gld_edge &edge);

};