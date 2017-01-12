#pragma once
#include "command.h"
#include "gld_selection.h"
#include "ChangeZorder.h"
#include "CmdUpdateObj.h"

class CCmdBringToFront : public CCmdUpdateObjUniId
{
public:
	CCmdBringToFront(const gld_shape_sel &sel, int type); // type = 0 / 1 0 - bring forward : 1 - bring to front
	virtual ~CCmdBringToFront(void);
	
public:
	virtual bool Execute();
	virtual bool Unexecute();

private:
	CChangeZorder		m_Data;
	gld_rect			m_Bound;
	int					m_nType;
};
