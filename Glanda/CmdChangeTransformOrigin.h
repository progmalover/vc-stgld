#pragma once

#include "gld_graph.h"
#include "CmdUpdateObj.h"

class gldInstance;
class CCmdChangeTransformOrigin : public CCmdUpdateObjUniId
{
private:
	gldInstance	*m_pInstance;
	gld_point m_Point;

	void Do();

public:
	CCmdChangeTransformOrigin(const gld_shape &shape, const gld_point &setto);
	virtual ~CCmdChangeTransformOrigin();

	virtual bool Execute();
	virtual bool Unexecute();

	virtual bool CanMix(TCommand *pOther);
	virtual TCommand *Mix(TCommand *pOther);
};
