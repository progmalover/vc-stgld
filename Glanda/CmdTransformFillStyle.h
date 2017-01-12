#pragma once
#include "CmdUpdateObj.h"
#include "gld_graph.h"

class TFillStyle;
class CCmdTransformFillStyle : public CCmdReshapeObj
{
protected:	
	TFillStyle		*m_pFillStyle;	
	TMatrix			m_Matrix;

	void Do();

public:
	CCmdTransformFillStyle(const gld_shape &shape, TFillStyle *fs, const TMatrix &mat);
	virtual ~CCmdTransformFillStyle();

	virtual bool Execute();
	virtual bool Unexecute();
};