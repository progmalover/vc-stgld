#pragma once
#include "command.h"
#include "gld_graph.h"
#include "gld_selection.h"
#include "gldObj.h"
#include "CmdUpdateObj.h"

class CCmdChangeLineStyleBase : public CCmdReshapeObj
{
protected:
	struct DrawObjLineStyles
	{
		gld_draw_obj			draw_obj;
		gld_list<TLineStyle *>	line_styles;
	};

	struct EdgeLineStyle
	{
		gld_edge	edge;
		int			ls;	

		EdgeLineStyle() : ls(0)
		{
		}

		EdgeLineStyle(const gld_edge &e, int l) : edge(e), ls(l)
		{
		}
	};

protected:	
	TLineStyle						*m_NewLineStyle;
	gld_list<DrawObjLineStyles *>	m_DrawObjs;
	gld_list<EdgeLineStyle>			m_Edges;
	
public:
	CCmdChangeLineStyleBase(gldObj *pObj, TLineStyle *ls);
	virtual ~CCmdChangeLineStyleBase();

	virtual bool Execute();
	virtual bool Unexecute();

	static bool CanChangeLineStyle(gld_shape &shape, TLineStyle *ls);

};

class CCmdChangeLineStyle : public TCommandGroup
{
public:
	CCmdChangeLineStyle(gld_shape_sel &sel, TLineStyle *ls);
	virtual ~CCmdChangeLineStyle();

	virtual bool Execute();
	virtual bool Unexecute();

protected:
	void FilterSelection(gld_shape_sel &sel, TLineStyle *ls);

};
