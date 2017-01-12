#include "StdAfx.h"
#include ".\cmdmodifymatrix.h"
#include "TransAdaptor.h"
#include "my_app.h"
#include "SWFProxy.h"
#include "DrawHelper.h"

CCmdModifyMatrix::CCmdModifyMatrix(gldInstance* pInstance, TMatrix& matrix)
: m_pInstance(pInstance)
{
	_M_Desc_ID = IDS_CMD_TRANSFORM;

	CTransAdaptor::TM2GM(matrix, m_matrix);
}

CCmdModifyMatrix::~CCmdModifyMatrix(void)
{
}

bool CCmdModifyMatrix::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdModifyMatrix::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdModifyMatrix::ExecSwap()
{
	std::swap(m_pInstance->m_matrix, m_matrix);

	CTransAdaptor::RebuildCurrentScene(false);

	gld_shape_sel sel;
	sel.select(CTraitShape(m_pInstance));
	my_app.DoSelect(sel);

	CGuardDrawOnce xDraw;
	CDrawHelper::UpdateObj(m_pInstance->m_obj);

	my_app.Redraw();
	my_app.Repaint();
}