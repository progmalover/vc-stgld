#pragma once
#include "command.h"

class CCmdRenameScene :	public TCommand
{
protected:
	CString m_strName;
	CString m_strOldName;
public:
	CCmdRenameScene(LPCTSTR lpszName);
	virtual ~CCmdRenameScene(void);
	virtual bool Execute();
	virtual bool Unexecute();
};
