#include "StdAfx.h"
#include "cmdchangefillstyle.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "TransAdaptor.h"
#include "Image.h"
#include "my_app.h"
#include "DrawHelper.h"
#include "SWFProxy.h"

CCmdChangeFillStyleBase::CCmdChangeFillStyleBase(gldObj *pObj, TFillStyle *fs)
: CCmdReshapeObj(pObj), m_NewFillStyle(fs)
{
	REFRENCE(fs)
}

CCmdChangeFillStyleBase::~CCmdChangeFillStyleBase()
{
	gld_list<DrawObjFillStyles *>::iterator dofs_i;
	gld_list<TFillStyle *>::iterator		fs_i;

	for (dofs_i = m_DrawObjs.begin(); dofs_i != m_DrawObjs.end(); ++dofs_i)
	{	
		DrawObjFillStyles *dfs = *dofs_i;
		for (fs_i = dfs->fill_styles.begin(); fs_i != dfs->fill_styles.end(); ++fs_i)
		{			
			RELEASE(*fs_i);
		}
		delete dfs;
	}

	RELEASE(m_NewFillStyle)
}

bool CCmdChangeFillStyleBase::Execute()
{
	gld_shape shape = CTraitShape(m_pObj);
	ASSERT(shape.validate());

	if (m_NewFillStyle == 0) // 设置为无填充
	{	
		RemoveFill(shape);
	}
	else if (HasSimpleFill(shape))
	{
		ReplaceFill(shape);
	}
	else if (IsSimpleShape(shape))
	{
		ChangeFill(shape);
	}
	else
	{
		return false;
	}

	UpdateUniId();
	UpdateRecord();

	CGuardDrawOnce	xDraw;
	CDrawHelper::UpdateObj(m_pObj);

	return TCommand::Execute();
}

bool CCmdChangeFillStyleBase::Unexecute()
{
	gld_list<DrawObjFillStyles *>::iterator dofs_i;
	gld_list<TFillStyle *>::iterator		fs_i;
	gld_list<EdgeFillStyle>::iterator		edge_i;

	// 恢复填充表
	for (dofs_i = m_DrawObjs.begin(); dofs_i != m_DrawObjs.end(); ++dofs_i)
	{		
		DrawObjFillStyles *dfs = *dofs_i;
		dfs->draw_obj.fill_styles().Clear();
		for (fs_i = dfs->fill_styles.begin(); fs_i != dfs->fill_styles.end(); ++fs_i)
		{
			dfs->draw_obj.fill_styles().Add(*fs_i);
			RELEASE(*fs_i);
		}
		delete dfs;
	}

	// 恢复边
	for (edge_i = m_Edges.begin(); edge_i != m_Edges.end(); ++edge_i)
	{
		EdgeFillStyle efs = *edge_i;
		efs.edge.fill_style0(efs.fs0);
		efs.edge.fill_style1(efs.fs1);
	}

	m_DrawObjs.clear();
	m_Edges.clear();
	
	UpdateUniId();
	UpdateRecord();

	CGuardDrawOnce	xDraw;
	CDrawHelper::UpdateObj(m_pObj);
	
	return TCommand::Unexecute();
}

bool CCmdChangeFillStyleBase::CanChangeFill(gld_shape &shape, TFillStyle *fs)
{
	return ((shape.capabilities() & SC_DIRECTFETCH) != 0) && ((fs == NULL) || HasSimpleFill(shape) || IsSimpleShape(shape));
}

bool CCmdChangeFillStyleBase::HasSimpleFill(gld_shape &shape)
{
	return (shape.begin_draw_obj() != shape.end_draw_obj()) && (++shape.begin_draw_obj() == shape.end_draw_obj())
			&& ((*shape.begin_draw_obj()).fill_styles().Count() == 1);
}	

bool CCmdChangeFillStyleBase::IsSimpleShape(gld_shape &shape)
{
	for (gld_draw_obj_iter do_i = shape.begin_draw_obj(); do_i != shape.end_draw_obj(); ++do_i)
	{
		gld_draw_obj d = *do_i;				
		// 判断是否能够设置填充				
		// 一条简单的封闭路径可以设置填充
		gld_path_iter path_i = d.begin_path();
		if (path_i != d.end_path() && (++path_i) == d.end_path())
		{
			gld_edge		lastedge;
			gld_path		p = *(d.begin_path());

			gld_edge_iter	edge_i = p.begin_edge();
			while (edge_i != p.end_edge())
			{
				lastedge = *edge_i;
				edge_i.next();
			}

			if (lastedge.validate())
			{
				if (p.start_x() != lastedge.e_point().x()
					|| p.start_y() != lastedge.e_point().y())
				{
					return false;
				}				
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

void CCmdChangeFillStyleBase::RemoveFill(gld_shape &shape)
{	
	for (gld_draw_obj_iter do_i = shape.begin_draw_obj(); do_i != shape.end_draw_obj(); ++do_i)
	{
		gld_draw_obj obj = *do_i;
		if (obj.fill_styles().Count() > 0)
		{
			// 清除所有边的填充
			for (gld_path_iter path_i = obj.begin_path(); path_i != obj.end_path(); ++path_i)
			{
				gld_path	path = *path_i;
				for (gld_edge_iter edge_i = path.begin_edge(); edge_i != path.end_edge(); edge_i.next())
				{
					gld_edge e = *edge_i;
					if (e.fill_style0() != 0 || e.fill_style1() != 0)
					{
						m_Edges.push_back(EdgeFillStyle(e, e.fill_style0(), e.fill_style1()));
						e.fill_style0(0);
						e.fill_style1(0);
					}
				}
			}

			// 清空填充表
			DrawObjFillStyles *dofs = new DrawObjFillStyles;
			dofs->draw_obj = obj;
			for (int i = 1; i < obj.fill_styles().Count() + 1; i++)
			{
				TFillStyle *pfs = obj.fill_styles().Get(i);
				REFRENCE(pfs);
				dofs->fill_styles.push_back(pfs);
			}
			obj.fill_styles().Clear();
			m_DrawObjs.push_back(dofs);
		}				
	}
}

void CCmdChangeFillStyleBase::ReplaceFill(gld_shape &shape)
{
	gld_draw_obj	obj = *shape.begin_draw_obj();

	DrawObjFillStyles *dofs = new DrawObjFillStyles;
	dofs->draw_obj = obj;
	TFillStyle *pfs = obj.fill_styles().Get(1);
	REFRENCE(pfs);
	dofs->fill_styles.push_back(pfs);
	m_DrawObjs.push_back(dofs);

	obj.fill_styles().Clear();
	obj.fill_styles().Add(m_NewFillStyle);
}

void CCmdChangeFillStyleBase::ChangeFill(gld_shape &shape)
{
	for (gld_draw_obj_iter do_i = shape.begin_draw_obj(); do_i != shape.end_draw_obj(); ++do_i)
	{
		gld_draw_obj d = *do_i;

		if (d.fill_styles().Count() > 0)							
		{
			// 清空填充表
			DrawObjFillStyles *dofs = new DrawObjFillStyles;
			dofs->draw_obj = d;
			for (int i = 1; i < d.fill_styles().Count() + 1; i++)
			{
				TFillStyle *pfs = d.fill_styles().Get(i);
				REFRENCE(pfs);
				dofs->fill_styles.push_back(pfs);
			}
			d.fill_styles().Clear();
			m_DrawObjs.push_back(dofs);								
		}

		d.fill_styles().Add(m_NewFillStyle);

		gld_path p = *d.begin_path();
		for (gld_edge_iter edge_i = p.begin_edge(); edge_i != p.end_edge(); edge_i.next())
		{
			gld_edge e = *edge_i;
			if (e.fill_style0() != 1 || e.fill_style1() != 0)
			{
				m_Edges.push_back(EdgeFillStyle(e, e.fill_style0(), e.fill_style1()));
				e.fill_style0(1);
				e.fill_style1(0);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CCmdChangeFillStyle::CCmdChangeFillStyle(gld_shape_sel &sel, TFillStyle *fs, bool ForceClone)
{
	_M_Desc_ID = IDS_CMD_CHANGEFILLSTYLE;

	gld_shape_sel	tmp = sel;

	FilterSelection(tmp, fs);

	list<gldObj *>	lstObj;

	for (gld_shape_sel::iterator i = tmp.begin(); i != tmp.end(); ++i)
	{	
		gldObj *pObj = CExtractObj(*i);

		if (find(lstObj.begin(), lstObj.end(), pObj) == lstObj.end())
		{
			lstObj.push_back(pObj);

			if (ForceClone && TSETrivial::FillStyleUseMatrix(fs))
			{			
				fs = CloneFillStyle(*i, fs);			
			}

			m_lstCommand.push_back(new CCmdChangeFillStyleBase(pObj, fs));
		}
	}
}

void CCmdChangeFillStyle::FilterSelection(gld_shape_sel &sel, TFillStyle *fs)
{
	gld_shape_sel	tmp;

	for(gld_shape_sel::iterator i = sel.begin(); i != sel.end(); ++i)	
	{
		if (CCmdChangeFillStyleBase::CanChangeFill(*i, fs))
		{
			tmp.select(*i);
		}		
	}
	sel = tmp;
}

CCmdChangeFillStyle::~CCmdChangeFillStyle()
{	
}

bool CCmdChangeFillStyle::Execute()
{
	CGuardDrawOnce	xDraw;

	return TCommandGroup::Execute();
}

bool CCmdChangeFillStyle::Unexecute()
{
	CGuardDrawOnce	xDraw;

	return TCommandGroup::Unexecute();
}

TFillStyle *CCmdChangeFillStyle::CloneFillStyle(gld_shape &shape, TFillStyle *fs)
{
	TFillStyle *cf = (TFillStyle *)fs->Clone();

	if (CCmdChangeFillStyleBase::HasSimpleFill(shape))
	{
		TFillStyle	*rf = (*shape.begin_draw_obj()).fill_styles().Get(1);
		ASSERT(rf);
		if (rf->GetType() == cf->GetType() 
			&& cf->GetType() != TFillStyle::clipped_bitmap_fill 
			&& cf->GetType() != TFillStyle::tiled_bitmap_fill)
		{
			cf->SetMatrix(rf->GetMatrix());

			return cf;
		}
	}
	
	TSETrivial::SetFillStyleMatrix(cf, shape.bound());	

	return cf;
}