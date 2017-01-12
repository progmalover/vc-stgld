#pragma once
#include "command.h"
#include "gld_selection.h"

#include "gldInstance.h"

class gldSprite;
class CCmdGroupInstance : public TCommandGroup
{
public:
	CCmdGroupInstance(const gld_shape_sel &sel);
	virtual ~CCmdGroupInstance(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();
};
