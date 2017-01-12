#include "StdAfx.h"
#include "typeset.h"
#include "my_app.h"
#include "CmdAddShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTypeset::CTypeset(void)
{
}

CTypeset::~CTypeset(void)
{
}

void CTypeset::Format(const gld_shape_sel &sel, CTypeset::TYPESETSTYLE nFormat)
{
	if (sel.count() < 2)
	{
		return;
	}
	
	switch (nFormat)
	{
	case ALIGN_LEFT:
		AlighLeft(sel);
		break;	
	case ALIGN_TOP:
		AlighTop(sel);
		break;
	case ALIGN_RIGHT:
		AlighRight(sel);
		break;
	case ALIGN_BOTTOM:
		AlighBottom(sel);
		break;
	case ALIGN_CENTER:
		AlighCenter(sel);
		break;
	case ALIGN_MIDDLE:
		AlighMiddle(sel);
		break;
	case MAKE_SAME_WIDTH:
		MakeSameWidth(sel);
		break;
	case MAKE_SAME_HEIGHT:
		MakeSameHeight(sel);
		break;
	case MAKE_SAME_SIZE:
		MakeSameSize(sel);
		break;
	case MAKE_HSPACING_EQUAL:
		MakeHSpacingEqual(sel);
		break;
	case MAKE_VSPACING_EQUAL:
		MakeVSpacingEqual(sel);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CTypeset::AlighLeft(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect				rcBase = (*base).effect_box();	
	gld_shape_sel			tmp;
	gld_list<TMatrix>		mats;
	gld_shape_sel::iterator	i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			tmp.select(*i);

			TMatrix		mat = (*i).matrix();
			gld_rect	rc = (*i).effect_box();

			mat.m_dx += rcBase.left - rc.left;
			mats.push_back(mat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::AlighTop(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();	
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			tmp.select(*i);

			TMatrix		mat = (*i).matrix();
			gld_rect	rc = (*i).effect_box();

			mat.m_dy += rcBase.top - rc.top;
			mats.push_back(mat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::AlighRight(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();	
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			tmp.select(*i);

			TMatrix		mat = (*i).matrix();
			gld_rect	rc = (*i).effect_box();

			mat.m_dx += rcBase.right - rc.right;
			mats.push_back(mat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::AlighBottom(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();	
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			tmp.select(*i);

			TMatrix		mat = (*i).matrix();
			gld_rect	rc = (*i).effect_box();

			mat.m_dy += rcBase.bottom - rc.bottom;
			mats.push_back(mat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::AlighCenter(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();	
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			tmp.select(*i);

			TMatrix		mat = (*i).matrix();
			gld_rect	rc = (*i).effect_box();

			mat.m_dx += ((rcBase.left + rcBase.right) - (rc.left + rc.right)) / 2;
			mats.push_back(mat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::AlighMiddle(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();	
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			tmp.select(*i);

			TMatrix		mat = (*i).matrix();
			gld_rect	rc = (*i).effect_box();

			mat.m_dy += ((rcBase.top + rcBase.bottom) - (rc.top + rc.bottom)) / 2;
			mats.push_back(mat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::MakeSameWidth(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			gld_shape	shape = *i;

			tmp.select(shape);

			TMatrix		mat = shape.matrix();
			gld_rect	rc = shape.effect_box();
			gld_point	op;
			shape.trans_origin(op);
			mat.Transform(op.x, op.y);
			float		scalex = (float)max(rcBase.width(), 20) / max(rc.width(), 20);
			TMatrix		smat;
			smat.SetScaleMatrix(scalex, 1.0f, op.x, op.y);

			mats.push_back(mat * smat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::MakeSameHeight(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			gld_shape	shape = *i;

			tmp.select(shape);

			TMatrix		mat = shape.matrix();
			gld_rect	rc = shape.effect_box();
			gld_point	op;
			shape.trans_origin(op);
			mat.Transform(op.x, op.y);
			float		scaley = (float)max(rcBase.height(), 20) / max(rc.height(), 20);
			TMatrix		smat;
			smat.SetScaleMatrix(1.0f, scaley, op.x, op.y);

			mats.push_back(mat * smat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::MakeSameSize(const gld_shape_sel &sel)
{
	gld_shape_sel::iterator base = GetBase(sel);
	gld_rect			rcBase = (*base).effect_box();
	gld_shape_sel		tmp;
	gld_list<TMatrix>	mats;
	gld_shape_sel::iterator			i = sel.begin();

	for(; i != sel.end(); ++i)
	{
		if (i != base)
		{
			gld_shape	shape = *i;

			tmp.select(shape);

			TMatrix		mat = shape.matrix();
			gld_rect	rc = shape.effect_box();
			gld_point	op;
			shape.trans_origin(op);
			mat.Transform(op.x, op.y);
			float		scalex = (float)max(rcBase.width(), 20) / max(rc.width(), 20);
			float		scaley = (float)max(rcBase.height(), 20) / max(rc.height(), 20);
			TMatrix		smat;
			smat.SetScaleMatrix(scalex, scaley, op.x, op.y);

			mats.push_back(mat * smat);
		}
	}

	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(tmp, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::MakeHSpacingEqual(const gld_shape_sel &sel)
{
	gld_list<gld_shape>		tmp;
	gld_list<int>			cxlist;	
	gld_rect				rc;
	gld_shape_sel::iterator	i = sel.begin();

	tmp.push_back(*i);
	rc = (*i).effect_box();
	cxlist.push_back((rc.left + rc.right) / 2);	

	for (++i; i != sel.end(); ++i)
	{		
		rc = (*i).effect_box();
		int	cx = (rc.left + rc.right) / 2;

		gld_list<gld_shape>::iterator	j = tmp.begin();
		gld_list<int>::iterator			k = cxlist.begin();

		for (; j != tmp.end(); ++j, ++k)
		{
			if (*k > cx)
			{
				break;
			}
		}

		tmp.insert(*i, j);
		cxlist.insert(cx, k);		
	}	

	int	x = *cxlist.begin();
	int dst = ((*cxlist.rbegin()) - x) / (sel.count() - 1);
	
	gld_list<TMatrix>				mats;
	gld_list<int>::iterator			j;
	gld_list<gld_shape>::iterator	k;
	gld_shape_sel					obj;

	for (k = ++tmp.begin(), j = ++cxlist.begin(); k != tmp.rbegin(); ++k, ++j)
	{
		x += dst;
		TMatrix mat = (*k).matrix();
		mat.m_dx += x - (*j);
		mats.push_back(mat);
		obj.select(*k);
	}
	
	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(obj, mats);
	my_app.Commands().Do(pCmd);
}

void CTypeset::MakeVSpacingEqual(const gld_shape_sel &sel)
{
	gld_list<gld_shape>		tmp;
	gld_list<int>			cylist;	
	gld_rect				rc;
	gld_shape_sel::iterator	i = sel.begin();

	tmp.push_back(*i);
	rc = (*i).effect_box();
	cylist.push_back((rc.top + rc.bottom) / 2);	

	for (++i; i != sel.end(); ++i)
	{		
		rc = (*i).effect_box();
		int	cy = (rc.top + rc.bottom) / 2;

		gld_list<gld_shape>::iterator	j = tmp.begin();
		gld_list<int>::iterator			k = cylist.begin();

		for (; j != tmp.end(); ++j, ++k)
		{
			if (*k > cy)
			{
				break;
			}
		}

		tmp.insert(*i, j);
		cylist.insert(cy, k);		
	}	

	int	y = *cylist.begin();
	int dst = ((*cylist.rbegin()) - y) / (sel.count() - 1);
	
	gld_list<TMatrix>				mats;
	gld_list<int>::iterator			j;
	gld_list<gld_shape>::iterator	k;
	gld_shape_sel					obj;

	for (k = ++tmp.begin(), j = ++cylist.begin(); k != tmp.rbegin(); ++k, ++j)
	{
		y += dst;
		TMatrix mat = (*k).matrix();
		mat.m_dy += y - (*j);
		mats.push_back(mat);
		obj.select(*k);
	}
	
	CCmdTransformInstance	*pCmd = new CCmdTransformInstance(obj, mats);
	my_app.Commands().Do(pCmd);
}

gld_shape_sel::iterator CTypeset::GetBase(const gld_shape_sel &sel)
{
	ASSERT(!sel.empty());

	if (sel.active() == sel.end())
	{
		return sel.begin();
	}
	else
	{
		return sel.active();
	}
}
