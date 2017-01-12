#include "StdAfx.h"
#include "cmdchangesymbolname.h"
#include "Observer.h"

CCmdChangeSymbolName::CCmdChangeSymbolName(gldObj* pObj, LPCTSTR lpszName)
: m_pObj(pObj)
, m_sName(lpszName)
{
	_M_Desc_ID = IDS_CMD_CHANGE_SYMBOL_NAME;
}

CCmdChangeSymbolName::~CCmdChangeSymbolName(void)
{
}

bool CCmdChangeSymbolName::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdChangeSymbolName::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdChangeSymbolName::ExecSwap()
{
	std::swap(m_pObj->m_name, m_sName);

	// Some UI elements, such as timeline, need to update
	CSubjectManager::Instance()->GetSubject("RenameSymbol")->Notify((void *)m_pObj);
}
