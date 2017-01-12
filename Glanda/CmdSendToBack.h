#pragma once
#include "CmdUpdateObj.h"
#include "gld_selection.h"
#include "ChangeZorder.h"

class CCmdSendToBack : public CCmdUpdateObjUniId
{
public:
	CCmdSendToBack(const gld_shape_sel &sel, int type); // type = 0 / 1 0 - send backward : 1 - send to back
	virtual ~CCmdSendToBack(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();

private:
	CChangeZorder		m_Data;
	gld_rect			m_Bound;
	int					m_nType;
};
