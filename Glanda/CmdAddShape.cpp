#include "StdAfx.h"
#include "cmdaddshape.h"
#include "gldLayer.h"
#include "gldMovieClip.h"
#include "gldShape.h"
#include "TransAdaptor.h"
#include "SWFProxy.h"
#include "DrawHelper.h"

#include "gldDataKeeper.h"
#include "gldLibrary.h"

#include "TextToolEx.h"
#include "my_app.h"

#include "Tips.h"
#include "GlandaDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CCmdAddObj::CCmdAddObj(gldObj *pObj)
{
	_M_Desc_ID = IDS_CMD_INSERTSYMBOL;

	ASSERT(pObj);

	//pObj->UpdateUniId();	

	m_obj = pObj;
	m_nRef = 0;
}

CCmdAddObj::~CCmdAddObj(void)
{
	if (!_M_done)
	{
		if (m_nRef == 0)
			CObjectMap::FreeSymbol(&m_obj);
	}
}

bool CCmdAddObj::Execute()
{
	m_nRef = _GetObjectMap()->Use(m_obj);
	return TCommand::Execute();
}

bool CCmdAddObj::Unexecute()
{
	m_nRef = _GetObjectMap()->Unuse(m_obj);
	return TCommand::Unexecute();
}

//////////////////////////////
// CCmdRemoveObj
CCmdRemoveObj::CCmdRemoveObj(gldObj* obj)
: m_obj(obj)
{
	_M_Desc_ID = IDS_CMD_REMOVESYMBOL;
}

CCmdRemoveObj::~CCmdRemoveObj()
{
	//* Modified by Liujun (07/15/2004)
	if(Done())
	{
		if(m_nRef == 0)
		{
			CObjectMap::FreeSymbol(&m_obj);
		}
	}
}

bool CCmdRemoveObj::Execute()
{
	m_nRef = _GetObjectMap()->Unuse(m_obj);
	return TCommand::Execute();
}

bool CCmdRemoveObj::Unexecute()
{
	m_nRef = _GetObjectMap()->Use(m_obj);
	return TCommand::Unexecute();
}

///////////////////////////////////
// CCmdAddInstance
CCmdAddInstance::CCmdAddInstance(gldScene2 *pScene, gldObj *pObj, const gldMatrix &matrix, const gldCxform &cxform)
: m_bAutoDelete(true)
{
	_M_Desc_ID = IDS_CMD_INSERTINSTANCE;

	ASSERT(pScene != NULL);
	ASSERT(pObj != NULL);

	m_scene = pScene;	

	m_instance = new gldInstance(pObj, matrix, cxform);
}

CCmdAddInstance::CCmdAddInstance(gldScene2 *pScene, gldInstance *pInst, bool bAutoDelete)
: m_bAutoDelete(bAutoDelete)
{
	_M_Desc_ID = IDS_CMD_INSERTINSTANCE;

	ASSERT(pScene != NULL);

	m_scene = pScene;

	m_instance = pInst;
}

CCmdAddInstance::~CCmdAddInstance(void)
{
	if (!_M_done && m_bAutoDelete)
		delete m_instance;
}

bool CCmdAddInstance::Execute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	gldScene2 *pScene = _GetCurScene2();
	pScene->m_instanceList.push_back(m_instance);

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdAddInstance::Unexecute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelKeeper	xSel;

	gldScene2 *pScene = _GetCurScene2();
	pScene->m_instanceList.remove(m_instance);

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Unexecute();
}

////////////////////////////////////////
// CCmdTransformInstance
CCmdTransformInstance::CCmdTransformInstance(const gld_shape_sel &sel, const gld_list<TMatrix> &matrixList)
{
	_M_Desc_ID = IDS_CMD_TRANSFORM;

	ASSERT(_GetCurScene2() != NULL);

	for (gld_shape_sel::iterator it = sel.begin(); it != sel.end(); it++)
	{
		gldInstance *pInstance = CTraitInstance(*it);
		m_instanceList.push_back(pInstance);
	}

	ASSERT(m_instanceList.size() == sel.count());

	//m_matrixList = matrixList;
	for (gld_list<TMatrix>::iterator it = matrixList.begin(); it != matrixList.end(); ++it)
		m_matrixList.push_back(*it);
}

CCmdTransformInstance::~CCmdTransformInstance(void)
{
}

bool CCmdTransformInstance::Execute()
{
	Do();
	return TCommand::Execute();
}

bool CCmdTransformInstance::Unexecute()
{
	Do();
	return TCommand::Unexecute();
}

bool CCmdTransformInstance::CanMix(TCommand *pOther)
{
	if (GetDescID() != pOther->GetDescID())
	{
		return false;
	}

	CCmdTransformInstance *pThis = (CCmdTransformInstance *)pOther;
	GINSTANCE_LIST::iterator i1 = m_instanceList.begin();
	GINSTANCE_LIST::iterator i2 = pThis->m_instanceList.begin();
	for (;i1 != m_instanceList.end() && i2 != pThis->m_instanceList.end(); ++i1, ++i2)
	{
		if (*i1 != *i2)
			break;
	}

	return i1 == m_instanceList.end() && i2 == pThis->m_instanceList.end();	
}

TCommand *CCmdTransformInstance::Mix(TCommand *pOther)
{
	if (!CanMix(pOther))
		return NULL;
	
	delete pOther;

	return this;
}

void CCmdTransformInstance::Do()
{
	GINSTANCE_LIST::iterator i = m_instanceList.begin();
	gld_list <TMatrix>::iterator j = m_matrixList.begin();

	CGuardDrawOnce	xDraw;

	for (; i != m_instanceList.end(); i++, j++)
	{
		gldInstance *pInstance = *i;

		ASSERT(pInstance->m_ptr);

		gld_shape shape(pInstance->m_ptr);

		CDrawHelper::UpdateCharacter(shape, false);

		TMatrix mat = shape.matrix();
		shape.matrix(*j);
		*j = mat;
		
		CDrawHelper::UpdateCharacter(shape, true);

		CTransAdaptor::TM2GM(shape.matrix(), pInstance->m_matrix);		

		CSubjectManager::Instance()->GetSubject("Transform")->Notify(&shape);
	}

	my_app.Repaint();
}

////////////////////////////////////////
// CCmdMoveInstance
CCmdMoveInstance::CCmdMoveInstance(const gld_shape_sel &sel, int dx, int dy)
{
	_M_Desc_ID = IDS_CMD_MOVESHAPE;

	ASSERT(_GetCurScene2() != NULL);

	for (gld_shape_sel::iterator it = sel.begin(); it != sel.end(); it++)
	{
		gldInstance *pInstance = CTraitInstance(*it);
		m_instanceList.push_back(pInstance);
	}

	ASSERT(m_instanceList.size() == sel.count());

	m_dx = dx;
	m_dy = dy;
}

CCmdMoveInstance::~CCmdMoveInstance(void)
{
}

bool CCmdMoveInstance::Execute()
{
	Do(m_dx, m_dy);
	return TCommand::Execute();
}

bool CCmdMoveInstance::Unexecute()
{
	Do(-m_dx, -m_dy);
	return TCommand::Unexecute();
}

bool CCmdMoveInstance::CanMix(TCommand *pOther)
{
	if (GetDescID() != pOther->GetDescID())
	{
		return false;
	}

	CCmdMoveInstance *pThis = (CCmdMoveInstance *)pOther;
	GINSTANCE_LIST::iterator i1 = m_instanceList.begin();
	GINSTANCE_LIST::iterator i2 = pThis->m_instanceList.begin();
	for (;i1 != m_instanceList.end() && i2 != pThis->m_instanceList.end(); ++i1, ++i2)
	{
		if (*i1 != *i2)
			break;
	}

	return i1 == m_instanceList.end() && i2 == pThis->m_instanceList.end();	
}

TCommand *CCmdMoveInstance::Mix(TCommand *pOther)
{
	if (!CanMix(pOther))
		return NULL;

	CCmdMoveInstance *pThis = (CCmdMoveInstance *)pOther;
	m_dx += pThis->m_dx;
	m_dy += pThis->m_dy;
	delete pOther;

	return this;
}

void CCmdMoveInstance::Do(int dx, int dy)
{
	GINSTANCE_LIST::iterator i = m_instanceList.begin();

	CGuardDrawOnce	xDraw;

	for (; i != m_instanceList.end(); i++)
	{
		gldInstance *pInstance = *i;

		ASSERT(pInstance->m_ptr);

		gld_shape shape(pInstance->m_ptr);

		CDrawHelper::UpdateCharacter(shape, false);

		TMatrix matrix = shape.matrix();
		matrix.m_dx += dx;
		matrix.m_dy += dy;
		shape.matrix(matrix);

		CDrawHelper::UpdateCharacter(shape, true);

		CTransAdaptor::TM2GM(matrix, pInstance->m_matrix);		

		CSubjectManager::Instance()->GetSubject("Transform")->Notify(&shape);
	}

	my_app.Repaint();
}

//////////////////////////////////////////
// CCmdAddShape
CCmdAddShape::CCmdAddShape(gld_shape &shape)
{
	_M_Desc_ID = IDS_CMD_ADDSHAPE;

	//ASSERT(CSWFProxy::GetCurFrameClip());
	
	gldShape	*gshape = new gldShape;
	gld_shape	*tshape = new gld_shape(shape);
	gshape->m_ptr = tshape;
	CTransAdaptor::TShape2GShape(*tshape, *gshape);

	gldMatrix	mx;
	gldCxform	cx;
	CTransAdaptor::TM2GM(shape.matrix(), mx);
	CTransAdaptor::TCX2GCX(shape.cxform(), cx);

	//gshape->UpdateUniId();	// do only once
	_GetObjectMap()->UpdateUniId(gshape);
	_GetObjectMap()->GenerateNextSymbolName(gshape->m_name);	// do only once
	
	m_lstCommand.push_back(new CCmdAddObj(gshape));	
	m_lstCommand.push_back(new CCmdAddInstance(_GetCurScene2(), gshape, mx, cx));
	//m_lstCommand.push_back(new CCmdInsertInstance(CSWFProxy::GetCurFrameClip(), gshape, mx, cx));
}

CCmdAddShape::~CCmdAddShape(void)
{
}

bool CCmdAddShape::Execute()
{
	CGuardDrawOnce	xDraw;
	//CGuardSelAppend	xSel;

	//bool ret = TCommandGroup::Execute();
	//if (ret)
	//	ShowAddShapeTip();
	//return ret;

	return TCommandGroup::Execute();
}

bool CCmdAddShape::Unexecute()
{
	CGuardDrawOnce	xDraw;
	//CGuardSelKeeper	xSel;

	return TCommandGroup::Unexecute();
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

CCmdAddText::CCmdAddText(gldText2 *pText)
{
	_M_Desc_ID = IDS_CMD_ADDTEXT;

	ASSERT(pText->m_ptr == NULL);
	CSWFProxy::PrepareTShapeForGObj(pText);
	//pText->UpdateUniId();
	_GetObjectMap()->UpdateUniId(pText);
	_GetObjectMap()->GenerateNextSymbolName(pText->m_name);

	m_lstCommand.push_back(new CCmdAddObj(pText));	
	m_lstCommand.push_back(new CCmdAddInstance(_GetCurScene2(), pText, pText->m_matrix, gldCxform()));
}

CCmdAddText::~CCmdAddText(void)
{
}

bool CCmdAddText::Execute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;	

	if (!my_app.LockTool())
	{
		my_app.SetCurTool(IDT_TRANSFORM);
	}

	return TCommandGroup::Execute();
}

bool CCmdAddText::Unexecute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelKeeper	xSel;

	return TCommandGroup::Unexecute();
}
