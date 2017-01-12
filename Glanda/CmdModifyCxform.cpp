#include "StdAfx.h"
#include ".\cmdmodifycxform.h"
#include "TransAdaptor.h"
#include "my_app.h"
#include "SWFProxy.h"
#include "DrawHelper.h"

CCmdModifyCxform::CCmdModifyCxform(gldInstance* pInstance, COLOR_TRANS_STYLE cxstyle, COLORREF cxcolor, TCxform& cxform)
: m_pInstance(pInstance)
, m_cxstyle(cxstyle)
, m_cxcolor(cxcolor)
{
	_M_Desc_ID = IDS_CMD_CHANGECXFORM;
	CTransAdaptor::TCX2GCX(cxform, m_cxform);
}

CCmdModifyCxform::~CCmdModifyCxform(void)
{
}

bool CCmdModifyCxform::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdModifyCxform::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdModifyCxform::ExecSwap()
{
	CGuardSelKeeper keeper;

	std::swap(m_pInstance->m_cxStyle, m_cxstyle);
	std::swap(m_pInstance->m_tint, m_cxcolor);
	std::swap(m_pInstance->m_cxform, m_cxform);

	//CTransAdaptor::RebuildCurrentScene(false);

	//gld_shape_sel sel;
	//sel.select(CTraitShape(m_pInstance));
	//my_app.DoSelect(sel);

	gld_shape	shape = CTraitShape(m_pInstance);
	TCxform tcxform;
	CTransAdaptor::GCX2TCX(m_pInstance->m_cxform, tcxform);
	shape.cxform(tcxform);
	CDrawHelper::UpdateCharacter(shape);

	//CDrawHelper::UpdateObj(m_pInstance->m_obj);

	//my_app.Redraw();
	//my_app.Repaint();
}