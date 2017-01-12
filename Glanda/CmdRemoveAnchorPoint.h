#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"

class CCmdRemoveAnchorPoint : public CCmdReshapeObj
{
private:
	gld_draw_obj		m_DrawObj;
	gld_a_point			m_APoint;
	gld_list<gld_edge>	m_RemoveEdges;
	gld_list<gld_edge>	m_AddEdges;
	gld_list<gld_edge>	m_PlaceEdges;	

	void DestroyEdge(gld_edge &edge);

public:
	CCmdRemoveAnchorPoint(const gld_shape &shape, const gld_draw_obj &obj, const gld_a_point &point);
	virtual ~CCmdRemoveAnchorPoint(void);

	virtual bool Execute();
	virtual bool Unexecute();	
};
