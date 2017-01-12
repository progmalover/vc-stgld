#include "StdAfx.h"
#include "cmdchangelinestyle.h"
#include "gldObj.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "TransAdaptor.h"
#include "DrawHelper.h"

CCmdChangeLineStyleBase::CCmdChangeLineStyleBase(gldObj *pObj, TLineStyle *ls)
: CCmdReshapeObj(pObj), m_NewLineStyle(ls)
{
	REFRENCE(ls);
}

CCmdChangeLineStyleBase::~CCmdChangeLineStyleBase()
{
	gld_list<DrawObjLineStyles *>::iterator dols_i;
	gld_list<TLineStyle *>::iterator		ls_i;

	for (dols_i = m_DrawObjs.begin(); dols_i != m_DrawObjs.end(); ++dols_i)
	{	
		int i = 1;
		DrawObjLineStyles *dls = *dols_i;
		for (ls_i = dls->line_styles.begin(); ls_i != dls->line_styles.end(); ++ls_i)
		{			
			RELEASE(*ls_i);
		}
		delete dls;
	}

	RELEASE(m_NewLineStyle);
}

bool CCmdChangeLineStyleBase::Execute()
{
	gld_draw_obj_iter				do_i;
	gld_path_iter					path_i;
	gld_edge_iter					edge_i;

	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);

	if (m_NewLineStyle == 0)
	{	// 设置为无线型
		// Get shape
		gldObj		*pObj = m_pObj;
		ASSERT(pObj->m_ptr);
		gld_shape	shape = *(gld_shape *)pObj->m_ptr;

		// Update fill style table
		for (do_i = shape.begin_draw_obj(); do_i != shape.end_draw_obj(); ++do_i)
		{
			gld_draw_obj _do = *do_i;
			if (_do.line_styles().Count() > 0)
			{
				// 清空线型表
				DrawObjLineStyles *dols = new DrawObjLineStyles;
				dols->draw_obj = _do;
				for (int i = 1; i < _do.line_styles().Count() + 1; i++)
				{
					TLineStyle *pls = _do.line_styles().Get(i);
					REFRENCE(pls);
					dols->line_styles.push_back(pls);
				}
				_do.line_styles().Clear();
				m_DrawObjs.push_back(dols);

				for (path_i = _do.begin_path(); path_i != _do.end_path(); ++path_i)
				{
					for (edge_i = (*path_i).begin_edge(); edge_i != (*path_i).end_edge(); edge_i.next())
					{
						gld_edge e = *edge_i;
						if (e.line_style() != 0)
						{
							m_Edges.push_back(EdgeLineStyle(e, e.line_style()));
							e.line_style(0);
						}
					}
				}
			}				
		}	
	}
	else
	{
		// Get shape
		gldObj		*pObj = m_pObj;
		ASSERT(pObj->m_ptr);
		gld_shape	shape = *(gld_shape *)pObj->m_ptr;		

		for (do_i = shape.begin_draw_obj(); do_i != shape.end_draw_obj(); ++do_i)
		{
			gld_draw_obj d = *do_i;				
			if (d.line_styles().Count() > 0)
			{
				// 清空线型表
				DrawObjLineStyles *dols = new DrawObjLineStyles;
				dols->draw_obj = d;
				for (int i = 1; i < d.line_styles().Count() + 1; i++)
				{
					TLineStyle *pls = d.line_styles().Get(i);
					REFRENCE(pls);
					dols->line_styles.push_back(pls);
				}
				d.line_styles().Clear();
				m_DrawObjs.push_back(dols);

				d.line_styles().Add(m_NewLineStyle);

				// 设置所有边的线型为索引1
				for (path_i = d.begin_path(); path_i != d.end_path(); ++path_i)
				{
					for (edge_i = (*path_i).begin_edge(); edge_i != (*path_i).end_edge(); edge_i.next())
					{
						gld_edge e = *edge_i;
						if (e.line_style() != 1)
						{									
							m_Edges.push_back(EdgeLineStyle(e, e.line_style()));
							e.line_style(1);
						}
					}
				}					
			}
			else
			{	// 原draw object没有线型
				int lsindex = d.line_styles().Add(m_NewLineStyle);
				for (path_i = d.begin_path(); path_i != d.end_path(); ++path_i)
				{
					for (edge_i = (*path_i).begin_edge(); edge_i != (*path_i).end_edge(); edge_i.next())
					{
						gld_edge e = *edge_i;									
						m_Edges.push_back(EdgeLineStyle(e, e.line_style()));
						e.line_style(lsindex);							
					}
				}
			}
		}		
	}

	UpdateUniId();
	UpdateBoundBox();
	UpdateRecord();
	
	CDrawHelper::UpdateObj(m_pObj);

	return TCommand::Execute();
}

bool CCmdChangeLineStyleBase::Unexecute()
{
	gld_list<DrawObjLineStyles *>::iterator dols_i;
	gld_list<TLineStyle *>::iterator ls_i;
	gld_list<EdgeLineStyle>::iterator edge_i;

	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);

	// 恢复填充表
	for (dols_i = m_DrawObjs.begin(); dols_i != m_DrawObjs.end(); ++dols_i)
	{			
		DrawObjLineStyles *dls = *dols_i;
		dls->draw_obj.line_styles().Clear();
		for (ls_i = dls->line_styles.begin(); ls_i != dls->line_styles.end(); ++ls_i)
		{
			dls->draw_obj.line_styles().Add(*ls_i);
			RELEASE(*ls_i);
		}
		delete dls;
	}

	// 恢复边
	for (edge_i = m_Edges.begin(); edge_i != m_Edges.end(); ++edge_i)
	{
		EdgeLineStyle els = *edge_i;
		els.edge.line_style(els.ls);
	}

	m_DrawObjs.clear();
	m_Edges.clear();

	UpdateUniId();
	UpdateBoundBox();
	UpdateRecord();
	
	CDrawHelper::UpdateObj(m_pObj);

	return TCommand::Unexecute();
}

bool CCmdChangeLineStyleBase::CanChangeLineStyle(gld_shape &shape, TLineStyle *ls)
{
	return shape.validate() && (shape.capabilities() & SC_DIRECTFETCH) != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

CCmdChangeLineStyle::CCmdChangeLineStyle(gld_shape_sel &sel, TLineStyle *ls)
{
	_M_Desc_ID = IDS_CMD_CHANGELINESTYLE;

	gld_shape_sel	tmp = sel;

	FilterSelection(tmp, ls);

	list<gldObj *>	lstObj;
	
	for (gld_shape_sel::iterator i = tmp.begin(); i != tmp.end(); ++i)
	{	
		gldObj *pObj = CExtractObj(*i);

		if (find(lstObj.begin(), lstObj.end(), pObj) == lstObj.end())
		{
			lstObj.push_back(pObj);

			m_lstCommand.push_back(new CCmdChangeLineStyleBase(pObj, ls));
		}
	}
}

CCmdChangeLineStyle::~CCmdChangeLineStyle()
{		
}

bool CCmdChangeLineStyle::Execute()
{
	CGuardDrawOnce	xDraw;

	return TCommandGroup::Execute();
}

bool CCmdChangeLineStyle::Unexecute()
{
	CGuardDrawOnce	xDraw;

	return TCommandGroup::Unexecute();	
}

void CCmdChangeLineStyle::FilterSelection(gld_shape_sel &sel, TLineStyle *ls)
{
	gld_shape_sel	tmp;

	for (gld_shape_sel::iterator i = sel.begin(); i != sel.end(); ++i)
	{
		if (CCmdChangeLineStyleBase::CanChangeLineStyle(*i, ls))
		{
			tmp.select(*i);
		}
	}

	sel = tmp;
}