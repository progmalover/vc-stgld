#include "StdAfx.h"
#include ".\cmdmodifyfillstyle.h"
#include "DrawHelper.h"
#include "gldObj.h"

CCmdModifyFillStyle::CCmdModifyFillStyle(gld_shape &shape, gldObj* pObj, gld_draw_obj &drawObj, int index, TFillStyle *newFill)
: CCmdReshapeObj(pObj)
, m_Shape(shape)
, m_DrawObj(drawObj)
, m_Index(index)
, m_pFill(newFill)
{
	_M_Desc_ID = IDS_CMD_CHANGEFILLSTYLE;
	m_RemoveFills = (m_pFill == NULL);
	REFRENCE(m_pFill);
}

CCmdModifyFillStyle::~CCmdModifyFillStyle(void)
{
	RELEASE(m_pFill);
}

bool CCmdModifyFillStyle::Execute()
{
	if (m_RemoveFills)
	{
		gld_path_iter ip = m_DrawObj.begin_path();
		for (; ip != m_DrawObj.end_path(); ip++)
		{
			gld_edge_iter ie = (*ip).begin_edge();
			for (; ie != (*ip).end_edge(); ie.next())
			{
				if ((*ie).fill_style0() == m_Index || (*ie).fill_style1() == m_Index)
				{
					m_Edges.push_back(_EdgeFills(*ie, (*ie).fill_style0(), (*ie).fill_style1()));
					if ((*ie).fill_style0() == m_Index)
						(*ie).fill_style0(0);
					if ((*ie).fill_style1() == m_Index)
						(*ie).fill_style1(0);
				}				
			}
		}
	}
	else
	{
		TFillStyle *pOldFill = m_DrawObj.fill_styles().Get(m_Index);
		REFRENCE(pOldFill);
		ASSERT(m_pFill != NULL);
		m_DrawObj.fill_styles().Replace(m_Index, m_pFill);
		RELEASE(m_pFill);
		m_pFill = pOldFill;
	}

	UpdateUniId();
	UpdateRecord();

	CGuardDrawOnce xDraw;
	CDrawHelper::UpdateObj(m_pObj);

	return TCommand::Execute();
}

bool CCmdModifyFillStyle::Unexecute()
{
	if (m_RemoveFills)
	{
		list<_EdgeFills>::iterator i = m_Edges.begin();
		for (; i != m_Edges.end(); ++i)
		{
			(*i).edge.fill_style0((*i).fs0);
			(*i).edge.fill_style1((*i).fs1);
		}
		m_Edges.clear();
	}
	else
	{
		TFillStyle *pOldFill = m_DrawObj.fill_styles().Get(m_Index);
		REFRENCE(pOldFill);
		m_DrawObj.fill_styles().Replace(m_Index, m_pFill);
		RELEASE(m_pFill);
		m_pFill = pOldFill;
	}

	UpdateUniId();
	UpdateRecord();

	CGuardDrawOnce xDraw;
	CDrawHelper::UpdateObj(m_pObj);

	return TCommand::Unexecute();
}