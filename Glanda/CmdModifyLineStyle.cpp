#include "StdAfx.h"
#include ".\cmdmodifylinestyle.h"
#include "DrawHelper.h"
#include "gldObj.h"
#include "my_app.h"

CCmdModifyLineStyle::CCmdModifyLineStyle(gldObj* pObj, gld_draw_obj &drawObj, int index, TLineStyle *newLine)
: CCmdReshapeObj(pObj)
, m_DrawObj(drawObj)
, m_Index(index)
, m_pLine(newLine)
{
	_M_Desc_ID = IDS_CMD_CHANGELINESTYLE;

	m_RemoveLine = (m_pLine == NULL);
	REFRENCE(m_pLine);
}

CCmdModifyLineStyle::~CCmdModifyLineStyle(void)
{
	RELEASE(m_pLine);
}

bool CCmdModifyLineStyle::Execute()
{
	if (m_RemoveLine)
	{
		gld_path_iter ip = m_DrawObj.begin_path();
		for (; ip != m_DrawObj.end_path(); ip++)
		{
			gld_edge_iter ie = (*ip).begin_edge();
			for (; ie != (*ip).end_edge(); ie.next())
			{
				if ((*ie).line_style() == m_Index)
				{
					m_Edges.push_back(_EdgeLine(*ie, (*ie).line_style()));
					if ((*ie).line_style() == m_Index)
						(*ie).line_style(0);					
				}
			}
		}
	}
	else
	{
		TLineStyle *pOldLine = m_DrawObj.line_styles().Get(m_Index);
		REFRENCE(pOldLine);
		ASSERT(m_pLine != NULL);
		m_DrawObj.line_styles().Replace(m_Index, m_pLine);
		RELEASE(m_pLine);
		m_pLine = pOldLine;
	}

	UpdateUniId();
	UpdateRecord();

	CGuardDrawOnce xDraw;
	CDrawHelper::UpdateObj(m_pObj);

	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdModifyLineStyle::Unexecute()
{
	if (m_RemoveLine)
	{
		list<_EdgeLine>::iterator i = m_Edges.begin();
		for (; i != m_Edges.end(); ++i)
		{
			(*i).edge.line_style((*i).ls);			
		}
		m_Edges.clear();
	}
	else
	{
		TLineStyle *pOldLine = m_DrawObj.line_styles().Get(m_Index);
		REFRENCE(pOldLine);
		m_DrawObj.line_styles().Replace(m_Index, m_pLine);
		RELEASE(m_pLine);
		m_pLine = pOldLine;
	}

	UpdateUniId();
	UpdateRecord();

	CGuardDrawOnce xDraw;
	CDrawHelper::UpdateObj(m_pObj);

	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Unexecute();
}