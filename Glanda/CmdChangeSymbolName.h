#pragma once
#include "command.h"
#include "gldObj.h"

class CCmdChangeSymbolName :
	public TCommand
{
	std::string m_sName;
	gldObj*		m_pObj;
public:
	CCmdChangeSymbolName(gldObj* pObj, LPCTSTR lpszName);
	virtual ~CCmdChangeSymbolName(void);

	virtual bool Execute();
	virtual bool Unexecute();
	void ExecSwap();
};
