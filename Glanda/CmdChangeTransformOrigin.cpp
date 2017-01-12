#include "StdAfx.h"
#include "cmdchangetransformorigin.h"
#include "Observer.h"
#include "gldCharacter.h"
#include "my_app.h"
#include "DrawHelper.h"
#include "Selection.h"
#include "SWFProxy.h"
#include "gldInstance.h"

CCmdChangeTransformOrigin::CCmdChangeTransformOrigin(const gld_shape &shape, const gld_point &setto)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
{
	_M_Desc_ID = IDS_CMD_CHANGETRANSORIGIN;	
	gld_shape	tshape = shape;
	m_pInstance = CTraitInstance(tshape);
	ASSERT(m_pInstance);
	m_Point = setto;
}

CCmdChangeTransformOrigin::~CCmdChangeTransformOrigin()
{
}

bool CCmdChangeTransformOrigin::Execute()
{
	Do();
	
	return TCommand::Execute();
}

bool CCmdChangeTransformOrigin::Unexecute()
{
	Do();	

	return TCommand::Unexecute();
}

void CCmdChangeTransformOrigin::Do()
{
	CGuardDrawOnce	xDraw;
	CGuardSelKeeper	xSel;

	swap(m_pInstance->m_tx, m_Point.x);
	swap(m_pInstance->m_ty, m_Point.y);	
	//CTransAdaptor::RecalcMotion(m_pKey);

	UpdateUniId();

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();

	gld_shape shape = CTraitShape(m_pInstance);
	CSubjectManager::Instance()->GetSubject("Transform")->Notify(&shape);
}

bool CCmdChangeTransformOrigin::CanMix(TCommand *pOther)
{
	if (pOther && pOther->GetDescID() == _M_Desc_ID)
	{
		CCmdChangeTransformOrigin *cmd = (CCmdChangeTransformOrigin *)pOther;

		return (cmd->m_pInstance == m_pInstance);
	}

	return false;
}

TCommand *CCmdChangeTransformOrigin::Mix(TCommand *pOther)
{
	if (pOther && pOther->GetDescID() == _M_Desc_ID)
	{
		CCmdChangeTransformOrigin *cmd = (CCmdChangeTransformOrigin *)pOther;
		
		if (cmd->m_pInstance == m_pInstance)
		{			
			delete cmd;

			return this;
		}
	}

	return NULL;
}