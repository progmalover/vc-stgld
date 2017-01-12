#include "StdAfx.h"
#include "cmdinsertanchorpoint.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "TransAdaptor.h"
#include "my_app.h"

CCmdInsertAnchorPoint::CCmdInsertAnchorPoint(const gld_shape &shape, const gld_draw_obj &obj, const gld_edge &edge, int x, int y)
: CCmdReshapeObj(CExtractObj(shape))
, m_DrawObj(obj)
, m_OldEdge(edge)
{
	_M_Desc_ID = IDS_CMD_INSERTANCHORPOINT;
	
	ASSERT(m_pObj->IsGObjInstanceOf(gobjShape));

	// Split edge
	int type = m_OldEdge.edge_type();
	
	if (type == S_straight_edge)
	{
		tQBezier	QBezier;

		gld_a_point sp = m_OldEdge.s_point();
		gld_a_point ep = m_OldEdge.e_point();
		int x1 = sp.x();
		int y1 = sp.y();
		int x2 = ep.x();
		int y2 = ep.y();
		QBezier.P0.x = x1;
		QBezier.P0.y = y1;
		QBezier.P1.x = (x1 + x2) >> 1;
		QBezier.P1.y = (y1 + y2) >> 1;
		QBezier.P2.x = x2;
		QBezier.P2.y = y2;
		double	u;
		tPoint	pt;
		pt.x = x;
		pt.y = y;
		tPointDistance(&pt, &QBezier, &u);
		tQBezierPoint(&QBezier, u, &pt);
		gld_a_point newanchor;
		newanchor.create(pt.x, pt.y);
		
		gld_s_edge	Edge1;
		gld_s_edge  Edge2;
		Edge1.create(sp, newanchor, m_OldEdge.line_style(), m_OldEdge.fill_style0(), m_OldEdge.fill_style1());
		Edge2.create(newanchor, ep, m_OldEdge.line_style(), m_OldEdge.fill_style0(), m_OldEdge.fill_style1());	

		m_NewEdge1 = Edge1;
		m_NewEdge2 = Edge2;
	}
	else if (type == S_qbezier_edge)
	{
		tQBezier	QBezier;
		gld_q_edge  qe = m_OldEdge;

		gld_a_point sp = qe.s_point();
		gld_a_point ep = qe.e_point();
		gld_c_point cp = qe.c_point();		
		QBezier.P0.x = sp.x();
		QBezier.P0.y = sp.y();
		QBezier.P1.x = cp.x();
		QBezier.P1.y = cp.y();
		QBezier.P2.x = ep.x();
		QBezier.P2.y = ep.y();
		double	u;
		tPoint	pt;
		pt.x = x;
		pt.y = y;
		tPointDistance(&pt, &QBezier, &u);
		tQBezierPoint(&QBezier, u, &pt);
		tPoint P1, P2;
		tQBezierSplit(&QBezier, &pt, &P1, &P2);
		gld_c_point cp1;
		gld_c_point cp2;
		gld_a_point newanchor;
		cp1.create(P1.x, P1.y);
		cp2.create(P2.x, P2.y);
		newanchor.create(pt.x, pt.y);
		gld_q_edge	Edge1;
		gld_q_edge  Edge2;
		Edge1.create(sp, cp1, newanchor, m_OldEdge.line_style(), m_OldEdge.fill_style0(), m_OldEdge.fill_style1());
		Edge2.create(newanchor, cp2, ep, m_OldEdge.line_style(), m_OldEdge.fill_style0(), m_OldEdge.fill_style1());

		m_NewEdge1 = Edge1;
		m_NewEdge2 = Edge2;
	}
	else
	{
		ASSERT(false);
	}
}

CCmdInsertAnchorPoint::~CCmdInsertAnchorPoint()
{
	if (_M_done)
	{	
		if (m_OldEdge.edge_type() == S_qbezier_edge)
		{
			gld_q_edge qe = m_OldEdge;
			qe.c_point().destroy();
		}		
		DestroyEdge(m_OldEdge);
	}
	else
	{
		m_NewEdge1.e_point().destroy();
		if (m_NewEdge1.edge_type() == S_qbezier_edge)
		{
			gld_q_edge qe1 = m_NewEdge1;
			gld_q_edge qe2 = m_NewEdge2;
			qe1.c_point().destroy();
			qe2.c_point().destroy();
		}
		DestroyEdge(m_NewEdge1);
		DestroyEdge(m_NewEdge2);		
	}
}

bool CCmdInsertAnchorPoint::Execute()
{
	gld_edge PrevEdge = *(gld_edge_iter(m_OldEdge).prev());
	m_DrawObj.remove(m_OldEdge);
	int type = m_OldEdge.edge_type();
	if (type == S_qbezier_edge)
	{
		gld_q_edge qe = m_OldEdge;
		m_DrawObj.remove(qe.c_point());
		
		gld_q_edge qe1 = m_NewEdge1;
		gld_q_edge qe2 = m_NewEdge2;
		m_DrawObj.insert(qe1.c_point());
		m_DrawObj.insert(qe2.c_point());
		m_DrawObj.insert(qe1.e_point());		
	}
	else
	{
		m_DrawObj.insert(m_NewEdge1.e_point());
	}
	m_DrawObj.insert(m_NewEdge1, PrevEdge);
	m_DrawObj.insert(m_NewEdge2, m_NewEdge1);

	// Update gldObj
	UpdateRecord();
	UpdateUniId();	

	my_app.Repaint();

	// 图形没有变化所以仅仅刷新窗口就可以了
	return TCommand::Execute();
}

bool CCmdInsertAnchorPoint::Unexecute()
{	
	int			type = m_NewEdge1.edge_type();
	gld_edge	prevEdge = *(gld_edge_iter(m_NewEdge1).prev());

	m_DrawObj.remove(m_NewEdge1);
	m_DrawObj.remove(m_NewEdge2);
	if (type == S_qbezier_edge)
	{
		gld_q_edge	qEdge1 = m_NewEdge1;
		gld_q_edge	qEdge2 = m_NewEdge2;
		m_DrawObj.remove(qEdge1.c_point());
		m_DrawObj.remove(qEdge2.c_point());

		gld_q_edge	qEdge = m_OldEdge;
		m_DrawObj.insert(qEdge.c_point());
	}	
	m_DrawObj.remove(m_NewEdge1.e_point());
	m_DrawObj.insert(m_OldEdge, prevEdge);
	
	// Update gldObj
	UpdateRecord();
	UpdateUniId();	

	my_app.Repaint();

	return TCommand::Unexecute();
}

void CCmdInsertAnchorPoint::DestroyEdge(gld_edge &edge)
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
