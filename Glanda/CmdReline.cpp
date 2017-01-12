#include "StdAfx.h"
#include "cmdreline.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "DrawHelper.h"

CCmdReline::CCmdReline(const gld_shape &shape, const gld_draw_obj &obj, const gld_edge &edge, double u, int x, int y)
: CCmdReshapeObj(CExtractObj(shape))
, m_DrawObj(obj)
, m_OldEdge(edge)
{
	_M_Desc_ID = IDS_CMD_RELINE;

	// Get gldObj	
	ASSERT(m_pObj->IsGObjInstanceOf(gobjShape));

	gld_shape	tshape = shape;

	tQBezier	QBezier;
	if (m_OldEdge.edge_type() == S_straight_edge)
	{
		gld_a_point sp = m_OldEdge.s_point();
		gld_a_point ep = m_OldEdge.e_point();
		QBezier.P0.x = sp.x();
		QBezier.P0.y = sp.y();
		QBezier.P2.x = ep.x();
		QBezier.P2.y = ep.y();
		QBezier.P1.x = (sp.x() + ep.x()) >> 1;
		QBezier.P1.y = (sp.y() + ep.y()) >> 1;
	}
	else // quadratic bezier
	{
		gld_q_edge	qe = m_OldEdge;
		gld_a_point sp = qe.s_point();
		gld_c_point cp = qe.c_point();
		gld_a_point ep = qe.e_point();
		QBezier.P0.x = sp.x();
		QBezier.P0.y = sp.y();
		QBezier.P2.x = ep.x();
		QBezier.P2.y = ep.y();
		QBezier.P1.x = cp.x();
		QBezier.P1.y = cp.y();
	}

	tPoint pt;
	pt.x = x;
	pt.y = y;
	TMatrix rmat = tshape.rmatrix();
	rmat.Transform(pt.x, pt.y);
	tQBezierReline(&QBezier, u, &pt);

	tPoint P0, P1, P2;
	P0.x = QBezier.P1.x;
	P0.y = QBezier.P1.y;
	P1.x = QBezier.P0.x;
	P1.y = QBezier.P0.y;
	P2.x = QBezier.P2.x;
	P2.y = QBezier.P2.y;
	double dst = tPointDistance(&P0, &P1, &P2);
	if (fabs(dst) < 41.0) // reline to straight edge
	{
		gld_s_edge	sEdge;
		sEdge.create(m_OldEdge.s_point(), m_OldEdge.e_point()
			, m_OldEdge.line_style(), m_OldEdge.fill_style0(), m_OldEdge.fill_style1());
		m_NewEdge = sEdge;
	}
	else // quadratic bezier
	{
		gld_q_edge	qEdge;
		gld_c_point cPoint;
		cPoint.create(P0.x, P0.y);
		qEdge.create(m_OldEdge.s_point(), cPoint, m_OldEdge.e_point(), m_OldEdge.line_style(),
			m_OldEdge.fill_style0(), m_OldEdge.fill_style1());
		m_NewEdge = qEdge;
	}
}

CCmdReline::~CCmdReline()
{
	if (_M_done)
	{
		int type = m_OldEdge.edge_type();
		if (type == S_qbezier_edge)
		{
			gld_q_edge qe = m_OldEdge;
			qe.c_point().destroy();
			qe.destroy();
		}
		else
		{
			gld_s_edge se = m_OldEdge;
			se.destroy();
		}
	}
	else
	{
		int type = m_NewEdge.edge_type();
		if (type == S_qbezier_edge)
		{
			gld_q_edge qe = m_NewEdge;
			qe.c_point().destroy();
			qe.destroy();
		}
		else
		{
			gld_s_edge se = m_NewEdge;
			se.destroy();
		}
	}
}

bool CCmdReline::Execute()
{
	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);

	m_DrawObj.remove(m_OldEdge);
	if (m_OldEdge.edge_type() == S_qbezier_edge)
	{
		gld_q_edge qe = m_OldEdge;
		m_DrawObj.remove(qe.c_point());
	}
	
	m_DrawObj.insert(m_NewEdge, *(gld_edge_iter(m_OldEdge).prev()));
	if (m_NewEdge.edge_type() == S_qbezier_edge)
	{
		gld_q_edge qe = m_NewEdge;
		m_DrawObj.insert(qe.c_point());
	}

	UpdateRecord();
	UpdateBoundBox();
	UpdateUniId();
	
	CDrawHelper::UpdateObj(m_pObj);

	return TCommand::Execute();
}

bool CCmdReline::Unexecute()
{
	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);

	m_DrawObj.remove(m_NewEdge);
	if (m_NewEdge.edge_type() == S_qbezier_edge)
	{
		gld_q_edge qe = m_NewEdge;
		m_DrawObj.remove(qe.c_point());
	}
	
	m_DrawObj.insert(m_OldEdge, *(gld_edge_iter(m_OldEdge).prev()));
	if (m_OldEdge.edge_type() == S_qbezier_edge)
	{
		gld_q_edge qe = m_OldEdge;
		m_DrawObj.insert(qe.c_point());
	}

	UpdateRecord();
	UpdateBoundBox();
	UpdateUniId();
	
	CDrawHelper::UpdateObj(m_pObj);
	
	return TCommand::Unexecute();
}

