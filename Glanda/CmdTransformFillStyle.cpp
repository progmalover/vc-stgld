#include "StdAfx.h"
#include "cmdtransformfillstyle.h"
#include "gldObj.h"
#include "gldCharacter.h"
#include "gldShape.h"
#include "DrawHelper.h"

CCmdTransformFillStyle::CCmdTransformFillStyle(const gld_shape &shape, TFillStyle *fs, const TMatrix &mat)
: CCmdReshapeObj(CExtractObj(shape))
, m_pFillStyle(fs)
, m_Matrix(mat)
{
	_M_Desc_ID = IDS_CMD_TRANSFORMFILLSTYLE;

	// Get gldObj
	ASSERT(m_pObj->IsGObjInstanceOf(gobjShape));

	REFRENCE(fs);
}

CCmdTransformFillStyle::~CCmdTransformFillStyle()
{
	RELEASE(m_pFillStyle);
}

bool CCmdTransformFillStyle::Execute()
{
	Do();

	return TCommand::Execute();
}

bool CCmdTransformFillStyle::Unexecute()
{
	Do();

	return TCommand::Unexecute();
}

void CCmdTransformFillStyle::Do()
{
	CGuardDrawOnce	xDraw;

	CDrawHelper::UpdateObj(m_pObj);

	TMatrix	mat = m_pFillStyle->GetMatrix();
	m_pFillStyle->SetMatrix(m_Matrix);	
	m_Matrix = mat;

	UpdateRecord();
	UpdateUniId();
	
	CDrawHelper::UpdateObj(m_pObj);
}