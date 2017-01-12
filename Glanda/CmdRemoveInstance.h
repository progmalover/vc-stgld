#pragma once

#include "CmdUpdateObj.h"
#include "gld_selection.h"
#include "selection.h"

class gldInstance;
class CCmdRemoveInstance : public CCmdUpdateObjUniId
{
public:
	CCmdRemoveInstance(const gld_shape_sel &sel, bool bAutoDelete = true);
	virtual ~CCmdRemoveInstance(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();

protected:
	void UpdateFrameClip(gldFrameClip *pframe);

protected:
	std::list<gldInstance *> m_instanceList;
	gld_list<int> m_lstZorder;
	bool m_bAutoDelete;
};
