#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"
#include <list>

class gldObj;

class CCmdModifyLineStyle : public CCmdReshapeObj
{	
public:
	CCmdModifyLineStyle(gldObj* pObj, gld_draw_obj &drawObj, int index, TLineStyle *newLine);
	virtual ~CCmdModifyLineStyle(void);

	virtual bool Execute();
	virtual bool Unexecute();

protected:
	struct _EdgeLine
	{
		gld_edge edge;
		int ls;

		_EdgeLine(const gld_edge &e, int l) : edge(e), ls(l) {}
	};

	bool m_RemoveLine;

	gld_draw_obj m_DrawObj;
	int m_Index;
	TLineStyle *m_pLine;
	
	list<_EdgeLine> m_Edges;
};
