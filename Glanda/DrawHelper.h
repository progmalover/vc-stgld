#pragma once

#include "gld_graph.h"

class CGuardDrawOnce
{
public:
	CGuardDrawOnce();
	~CGuardDrawOnce();	
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

class gldObj;
class CDrawHelper
{
public:
	CDrawHelper(void);
	~CDrawHelper(void);

	static void UpdateObj(gldObj *pObj);
	static void UpdateCharacter(gld_shape &shape, bool notify = true);
};
