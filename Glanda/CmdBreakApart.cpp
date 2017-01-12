#include "StdAfx.h"
#include "cmdbreakapart.h"
#include "gldObj.h"
#include "gldCharacter.h"
#include "gldMovieClip.h"
#include "gldSprite.h"
#include "gldText.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldDataKeeper.h"
#include "CmdRemoveInstance.h"
#include "SWFProxy.h"
#include "DrawHelper.h"

#include "gldLibrary.h"

#include "GlandaDoc.h"
#include "gldScene2.h"
#include "gldInstance.h"
#include "CmdAddShape.h"

CCmdBreakApart::CCmdBreakApart(gld_shape &shape)
: m_pInstance(CTraitInstance(shape))
{
	_M_Desc_ID = IDS_CMD_BREAKAPART;

	ASSERT(CanBreakApart(shape));	
}

CCmdBreakApart::~CCmdBreakApart(void)
{
}

bool CCmdBreakApart::Execute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	if (m_lstCommand.size() == 0)
	{
		DoFirst();
	}

	return TCommandGroup::Execute();	
}

bool CCmdBreakApart::Unexecute()
{
	CGuardDrawOnce	xDraw;
	CGuardSelAppend	xSel;

	return TCommandGroup::Unexecute();	
}

void CCmdBreakApart::DoFirst()
{
	gldObj *pObj = m_pInstance->m_obj;
	if (pObj->IsGObjInstanceOf(gobjSprite))
	{
		BreakApartGroup(_GetCurScene2(), (gld_shape)CTraitShape(m_pInstance));
	}
	else
	{
		BreakApartText(_GetCurScene2(), (gld_shape)CTraitShape(m_pInstance));
	}

	gld_shape_sel	sel;
	sel.select(CTraitShape(m_pInstance));
	Do(new CCmdRemoveInstance(sel));
}

bool CCmdBreakApart::CanBreakApart(gld_shape &shape)
{	
	gldObj *pObj = CTraitInstance(shape);
	if (pObj->IsGObjInstanceOf(gobjSprite))
	{
		return CSWFProxy::GObjIsValidGroup(pObj);
	}
	else
	{
		return pObj->IsGObjInstanceOf(gobjText);
	}
}

void CCmdBreakApart::BreakApartGroup(gldScene2 *scene, gld_shape &shape)
{	
	gldObj *pObj = CTraitInstance(shape);
	ASSERT(pObj->IsGObjInstanceOf(gobjSprite));	
	gldSprite *pSprite = (gldSprite *)pObj;
	ASSERT(pSprite->m_isGroup);

	gldInstance *pInstance = CTraitInstance(shape);
	gldMatrix mx = pInstance->m_matrix;
	gldCxform cx = pInstance->m_cxform;	
	
	gldFrameClip *pFrame = *(*(pSprite->m_mc->m_layerList.begin()))->m_frameClipList.begin();
	GCHARACTERKEY_LIST::iterator iKey = pFrame->m_characterKeyList.begin();
	for (; iKey != pFrame->m_characterKeyList.end(); ++iKey)
	{
		gldCharacterKey *pKey = *iKey;		
		gldCharacter *pBaseChar = pKey->GetCharacter(0);	

		Do(new CCmdAddInstance(scene, pKey->GetObj(), pBaseChar->m_matrix * mx, pBaseChar->m_cxform * cx));
	}	
}

void CCmdBreakApart::BreakApartText(gldScene2 *scene, gld_shape &shape)
{
	OBJINSTANCE_LIST	lstInst;
	
	CSWFProxy::BreakApartTextToShapes(shape, lstInst);

	// Insert folder to library tree
	gldObj *pObj = CTraitInstance(shape);
	//Do(new CCmdInsertFolder(pObj->m_name.c_str(), true, false));

	// Get last create folder
	//HTREEITEM hItem = CLibraryTree::Instance()->GetLastInsertedItem();
	//ASSERT(hItem);
	//ITEM_PATH path;
	//CLibraryTree::Instance()->GetItemPath(hItem, path);
	
	int cnt = 0;
	for (OBJINSTANCE_LIST_IT i = lstInst.begin(); i != lstInst.end(); ++i)
	{
		//(*i).m_obj->UpdateUniId();
		_GetObjectMap()->UpdateUniId((*i).m_obj);
		//gldDataKeeper::Instance()->m_objLib->GenerateNextSymbolName((*i).m_obj->m_name);		
		_GetObjectMap()->GenerateNextSymbolName((*i).m_obj->m_name);
		//ITEM_PATH path_item = path;
		//path_item.push_back(cnt++);
		//Do(new CCmdInsertObj(path_item, (*i).m_obj, true, false));
		Do(new CCmdAddObj((*i).m_obj));
		//Do(new CCmdInsertInstance(pFC, (*i).m_obj, (*i).m_mx, (*i).m_cx));
		Do(new CCmdAddInstance(scene, (*i).m_obj, (*i).m_mx, (*i).m_cx));
	}
}
