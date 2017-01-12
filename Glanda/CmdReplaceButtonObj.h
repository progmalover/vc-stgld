#pragma once
#include "command.h"
#include <list>
#include "CmdUpdateObj.h"
#include "gldMatrix.h"

class gldCharacterKey;
class gldObj;

class CCmdReplaceButtonObj : public CCmdUpdateObjUniId
{
public:
	CCmdReplaceButtonObj(gldCharacterKey* pKey, gldObj* pObj, float rx = 1.0, float ry = 1.0, int offsetx = 0, int offsety = 0);
	virtual ~CCmdReplaceButtonObj(void);
	virtual bool Execute();
	virtual bool Unexecute();
	void ExecSwap();
protected:
	gldCharacterKey* m_pKey;
	gldObj* m_pObj;
	std::list<gldMatrix> m_matrixList;
};