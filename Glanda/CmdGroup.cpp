#include "StdAfx.h"
#include "cmdgroup.h"
#include "gldObj.h"
#include "gldButton.h"
#include "gldSprite.h"
#include "gldDataKeeper.h"
#include "gldLibrary.h"
#include "gldMovieClip.h"
#include "gldFrameClip.h"
#include "gldLayer.h"
#include "TransAdaptor.h"
#include "CmdRemoveInstance.h"
#include "SWFProxy.h"
#include "DrawHelper.h"

#include "GlandaDoc.h"
#include "CmdAddShape.h"
#include "SWFProxy.h"

CCmdGroupInstance::CCmdGroupInstance(const gld_shape_sel &sel)
{
	_M_Desc_ID = IDS_CMD_GROUP;

	gld_rect bnd = sel.SurroundBox();
	int offsetX = 0, offsetY = 0;
	bnd.center(offsetX, offsetY);

	gldSprite *group = new gldSprite();
	//group->UpdateUniId();
	_GetObjectMap()->UpdateUniId(group);

	group->m_isGroup = true;
	group->m_mc->m_editFlags = gldMovieClip::FLAG_GROUP;

	ASSERT(group->m_mc->m_layerList.size() == 0);

	gldLayer *pLayer = new gldLayer("Group", 1);
	group->m_mc->AddLayer(pLayer);

	gldFrameClip *pFrame = *pLayer->m_frameClipList.begin();

	gld_shape_sel::iterator ishape = sel.rbegin();
	for (; ishape != sel.rend(); --ishape)
	{
		gldInstance *pInst = CTraitInstance(*ishape);
		gldMatrix mx = pInst->m_matrix;
		mx.m_x -= offsetX;
		mx.m_y -= offsetY;
		pFrame->AddCharacter(pInst->m_obj, mx, pInst->m_cxform);
	}

	CSWFProxy::PrepareTShapeForGObj(group);

	m_lstCommand.push_back(new CCmdRemoveInstance(sel));
	m_lstCommand.push_back(new CCmdAddObj(group));
	m_lstCommand.push_back(new CCmdAddInstance(_GetCurScene2(), group, gldMatrix(1, 1, 0, 0, offsetX, offsetY), gldCxform()));
}

CCmdGroupInstance::~CCmdGroupInstance(void)
{
}

bool CCmdGroupInstance::Execute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	return TCommandGroup::Execute();
}

bool CCmdGroupInstance::Unexecute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	return TCommandGroup::Unexecute();
}
