#include "StdAfx.h"
#include ".\objectmap.h"
#include "TransAdaptor.h"
#include "gldLibrary.h"
#include "gldFont.h"
#include "gldDataKeeper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CObjectMap::CObjectMap(void)
{
	m_nMaxId = 1;
	m_nUniId = 1;
}

CObjectMap::~CObjectMap(void)
{
	FreeAllSymbol();
}

void CObjectMap::FreeSymbol(gldObj **ppObj)
{
	ASSERT(ppObj != NULL);
	ASSERT(*ppObj != NULL);

	CTransAdaptor::DestroyTShapePtr(*ppObj);
	delete *ppObj;
	*ppObj = NULL;
}

void CObjectMap::FreeAllSymbol()
{
	for (_ObjectMapIter i = m_Map.begin(); i != m_Map.end(); ++i)
	{
		gldObj *pObj = (*i).first;
		FreeSymbol(&pObj);
	}
	m_Map.clear();
	m_nMaxId = 1;
}

void CObjectMap::CopyTo(gldLibrary *pLibrary)
{
	for (_ObjectMapIter i = m_Map.begin(); i != m_Map.end(); ++i)
		pLibrary->AddObj((*i).first);
}

gldObj *CObjectMap::FindObjectByID(WORD wId)
{
	for (_ObjectMapIter i = m_Map.begin(); i != m_Map.end(); ++i)
	{		
		if ((*i).first->m_id == wId)
			return (*i).first;
	}
	return NULL;
}

gldObj *CObjectMap::FindObjectByUniid(const UniId &id)
{
	for (_ObjectMapIter i = m_Map.begin(); i != m_Map.end(); ++i)
	{		
		if ((*i).first->m_uniId == id)
			return (*i).first;
	}
	return NULL;
}

gldFont *CObjectMap::FindFont(LPCTSTR fontName, bool isBold, bool isItalic, bool isWideCodes)
{
	gldFont *font = NULL;
    for (_ObjectMapIter i = m_Map.begin(); i != m_Map.end(); ++i)
    {
        gldObj *obj = (*i).first;
        if (obj->IsGObjInstanceOf(gobjFont))
        {
            gldFont *tmpfont = (gldFont *)obj;
            if (tmpfont->IsSameFont(fontName, isBold, isItalic))
            {
                font = tmpfont;
                break;
            }
        }
    }
    return font;
}

//* OBJECTMAP°æFindShapesUseImage
//* Modified by Liujun (Add a member function)
bool CObjectMap::FindShapesUseImage(gldImage *pImage, GOBJECT_LIST *pObjList)
{
	for(_ObjectMap::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		gldObj* pObj = it->first;
		if(pObj->IsGObjInstanceOf(gobjShape))
		{
			GOBJECT_LIST objUsed;
			pObj->GetUsedObjs(objUsed);
			if (find(objUsed.begin(), objUsed.end(), (gldObj*)pImage) != objUsed.end())
			{
				if (pObjList)
					pObjList->push_back(pObj);
				else
					return true;
			}
		}
	}

	return pObjList && pObjList->size() > 0;
}

UniId CObjectMap::GenUniId()
{	
	UniId id;

	id.procId = gldDataKeeper::Instance()->m_procId;
	id.docId = gldDataKeeper::Instance()->m_docId;
	id.libId = m_nUniId++;

	return id;
}

void CObjectMap::UpdateUniId(gldObj *pObj)
{
	ASSERT(pObj != NULL);

	pObj->m_uniId = GenUniId();
}

