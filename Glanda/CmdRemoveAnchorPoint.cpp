#include "StdAfx.h"
#include "cmdremoveanchorpoint.h"
#include "gldObj.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "DrawHelper.h"

CCmdRemoveAnchorPoint::CCmdRemoveAnchorPoint(const gld_shape &shape, const gld_draw_obj &obj, const gld_a_point &point)
: CCmdReshapeObj(CExtractObj(shape))
, m_DrawObj(obj)
, m_APoint(point)
{
	_M_Desc_ID = IDS_CMD_REMOVEANCHORPOINT;

	ASSERT(m_pObj->IsGObjInstanceOf(gobjShape));

	// Get replace, remove, add edges
	gld_path_iter	iPath = m_DrawObj.begin_path();
	for (;iPath != m_DrawObj.end_path(); ++iPath)
	{
		gld_path path = *iPath;
		gld_edge_iter iEdge = path.begin_edge();
		if (iEdge != path.end_edge())
		{			
			gld_edge	edge = *iEdge;
			while (edge.s_point() == m_APoint)
			{				
				m_RemoveEdges.push_back(edge);				
				edge = *(gld_edge_iter(edge).next());
			}
			for (; iEdge != path.end_edge(); iEdge.next())
			{
				edge = *iEdge;
				if (edge.e_point() == m_APoint)
				{
					gld_edge_iter iNextEdge(edge);
					iNextEdge.next();					
					
					if (iNextEdge != path.end_edge())
					{
						gld_edge_iter iPrevEdge(edge);
						iPrevEdge.prev();

						m_RemoveEdges.push_back(edge);						
						m_RemoveEdges.push_back(*iNextEdge);

						gld_s_edge	newEdge;
						newEdge.create(edge.s_point(), (*iNextEdge).e_point(), 
							edge.line_style(), edge.fill_style0(), edge.fill_style1());
						m_AddEdges.push_back(newEdge);
						m_PlaceEdges.push_back(*iPrevEdge);
					}
					else
					{
						m_RemoveEdges.push_back(edge);					
					}
				}
			}
		}
	}	
}

CCmdRemoveAnchorPoint::~CCmdRemoveAnchorPoint(void)
{
	if (_M_done)
	{
		gld_list<gld_edge>::iterator iEdge = m_RemoveEdges.begin();
		for(; iEdge != m_RemoveEdges.end(); ++iEdge)
		{
			DestroyEdge(*iEdge);
		}
		m_APoint.destroy();
	}
	else
	{
		gld_list<gld_edge>::iterator iEdge = m_AddEdges.begin();
		for(; iEdge != m_AddEdges.end(); ++iEdge)
		{
			DestroyEdge(*iEdge);
		}
	}
}

bool CCmdRemoveAnchorPoint::Execute()
{
	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);	

	gld_list<gld_edge>::iterator iRemoveEdge = m_RemoveEdges.rbegin();
	for(; iRemoveEdge != m_RemoveEdges.rend(); --iRemoveEdge)
	{
		m_DrawObj.remove(*iRemoveEdge);
	}
	m_DrawObj.remove(m_APoint);

	gld_list<gld_edge>::iterator iPlace = m_PlaceEdges.begin();
	gld_list<gld_edge>::iterator iAdd = m_AddEdges.begin();
	while (iAdd != m_AddEdges.end())
	{
		m_DrawObj.insert(*iAdd, *iPlace);

		++iAdd;
		++iPlace;
	}

	UpdateRecord();
	UpdateBoundBox();
	UpdateUniId();
	
	CDrawHelper::UpdateObj(m_pObj);	
	
	return TCommand::Execute();
}

bool CCmdRemoveAnchorPoint::Unexecute()
{
	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);	

	gld_list<gld_edge>::iterator iAddEdge = m_AddEdges.begin();	
	for(; iAddEdge != m_AddEdges.end(); ++iAddEdge)
	{
		m_DrawObj.remove(*iAddEdge);
	}

	m_DrawObj.insert(m_APoint);

	gld_list<gld_edge>::iterator iRemoveEdge = m_RemoveEdges.begin();
	for(; iRemoveEdge != m_RemoveEdges.end(); ++iRemoveEdge)
	{
		gld_edge	edge = *iRemoveEdge;
		m_DrawObj.insert(edge, *(gld_edge_iter(edge).prev()));
	}
	
	UpdateRecord();
	UpdateBoundBox();
	UpdateUniId();

	CDrawHelper::UpdateObj(m_pObj);	
	
	return TCommand::Unexecute();
}

void CCmdRemoveAnchorPoint::DestroyEdge(gld_edge &edge)
{
	int type = edge.edge_type();

	if (type == S_straight_edge)
	{
		gld_s_edge	se = edge;
		se.destroy();
	}
	else if (type == S_qbezier_edge)
	{
		gld_q_edge qe = edge;
		qe.destroy();
	}
	else
	{
		ASSERT(false);
	}
}
