#pragma once
#include "command.h"
#include "Matrix.h"
#include "gldInstance.h"

class CCmdModifyMatrix :
	public TCommand
{
	gldInstance* m_pInstance;
	gldMatrix m_matrix;

	void ExecSwap();
public:
	CCmdModifyMatrix(gldInstance* pInstance, TMatrix& matrix);
	virtual ~CCmdModifyMatrix(void);

	virtual bool Execute();
	virtual bool Unexecute();
};
