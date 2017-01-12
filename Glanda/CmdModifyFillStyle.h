#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"
#include <list>

class gldObj;

class CCmdModifyFillStyle : public CCmdReshapeObj
{	
public:
	CCmdModifyFillStyle(gld_shape &shape, gldObj* pObj, gld_draw_obj &drawObj, int index, TFillStyle *newFill);
	virtual ~CCmdModifyFillStyle(void);

	virtual bool Execute();
	virtual bool Unexecute();


protected:
	struct _EdgeFills
	{
		gld_edge edge;
		int fs0;
		int fs1;

		_EdgeFills(const gld_edge &e, int f0, int f1) : edge(e), fs0(f0), fs1(f1) {}
	};

	bool m_RemoveFills;

	gld_shape m_Shape;
	gld_draw_obj m_DrawObj;
	int m_Index;
	TFillStyle *m_pFill;
	
	list<_EdgeFills> m_Edges;

};
