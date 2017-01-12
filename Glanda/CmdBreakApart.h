#pragma once
#include "command.h"
#include "gld_graph.h"

class gldScene2;
class gldInstance;
class CCmdBreakApart : public TCommandGroup
{
public:
	CCmdBreakApart(gld_shape &shape);
	virtual ~CCmdBreakApart(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();
	
	static bool CanBreakApart(gld_shape &shape);

protected:
	void DoFirst();
	void BreakApartGroup(gldScene2 *scene, gld_shape &shape);
	void BreakApartText(gldScene2 *scene, gld_shape &shape);

protected:
	gldInstance		*m_pInstance;
};
