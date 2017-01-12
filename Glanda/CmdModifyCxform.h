#pragma once
#include "command.h"

#include "gldInstance.h"
#include "cxform.h"

class CCmdModifyCxform :
	public TCommand
{
	gldInstance* m_pInstance;

	COLOR_TRANS_STYLE m_cxstyle;
	gldCxform m_cxform;
	COLORREF m_cxcolor;

	void ExecSwap();
public:
	CCmdModifyCxform(gldInstance* pInstance, COLOR_TRANS_STYLE cxstyle, COLORREF cxcolor, TCxform& cxform);
	virtual ~CCmdModifyCxform(void);

	virtual bool Execute();
	virtual bool Unexecute();
};
