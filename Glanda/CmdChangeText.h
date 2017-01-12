#pragma once

#include "CmdUpdateObj.h"

class gldText2;
class CCmdChangeText : public CCmdReshapeObj
{
public:
	CCmdChangeText(gldText2 *pOld, gldText2 *pNew);
	~CCmdChangeText(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();

protected:
	U8			*m_pNewData;
	size_t		m_nNewSize;
	U8			*m_pOldData;
	size_t		m_nOldSize;
	gldText2	*m_pText;
};
