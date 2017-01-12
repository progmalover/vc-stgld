#pragma once

#include "gld_graph.h"
#include "gld_selection.h"
#include "gldObj.h"
#include "CmdUpdateObj.h"

class CCmdChangeFillStyleBase : public CCmdReshapeObj
{
protected:
	struct DrawObjFillStyles
	{
		gld_draw_obj			draw_obj;
		gld_list<TFillStyle *>	fill_styles;
	};

	struct EdgeFillStyle
	{
		gld_edge	edge;
		int			fs0;
		int			fs1;

		EdgeFillStyle() : fs0(0), fs1(0)
		{
		}

		EdgeFillStyle(const gld_edge &e, int f0, int f1) : edge(e), fs0(f0), fs1(f1)
		{
		}
	};

public:
	CCmdChangeFillStyleBase(gldObj *pObj, TFillStyle *fs);
	virtual ~CCmdChangeFillStyleBase();

	virtual bool Execute();
	virtual bool Unexecute();

public:
	static bool HasSimpleFill(gld_shape &shape);
	static bool IsSimpleShape(gld_shape &shape);
	static bool CanChangeFill(gld_shape &shape, TFillStyle *fs);

protected:
	void RemoveFill(gld_shape &shape);
	void ReplaceFill(gld_shape &shape);
	void ChangeFill(gld_shape &shape);

protected:
	gld_list<DrawObjFillStyles *>	m_DrawObjs;
	gld_list<EdgeFillStyle>			m_Edges;
	TFillStyle						*m_NewFillStyle;	
};

class CCmdChangeFillStyle : public TCommandGroup
{
public:
	CCmdChangeFillStyle(gld_shape_sel &sel, TFillStyle *fs, bool ForceClone = true);
	virtual ~CCmdChangeFillStyle();

	virtual bool Execute();
	virtual bool Unexecute();

protected:
	void FilterSelection(gld_shape_sel &sel, TFillStyle *fs);

protected:
	TFillStyle *CloneFillStyle(gld_shape &shape, TFillStyle *fs);

};