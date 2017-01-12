#include "stdafx.h"
#include "Glanda.h"
#include "Clipboard.h"
#include "Utils.h"

#include "BinStream.h"

#include "gldLibrary.h"
#include "gldObj.h"
#include "gldShape.h"
#include "gldFont.h"
#include "gldButton.h"
#include "gldSprite.h"
#include "gldImage.h"
#include "gldSound.h"
#include "gldMorphShape.h"
#include "gldMovieClip.h"

#include "gldDataKeeper.h"
#include "my_app.h"
#include "TextToolEx.h"

#include "SWFProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//DEFINE_CLIPBOARD_FORMAT(CF_GMOVIE_SYMBOL)
//DEFINE_CLIPBOARD_FORMAT(CF_GMOVIE_INSTANCE)
//DEFINE_CLIPBOARD_FORMAT(CF_GMOVIE_FRAMECLIP)
DEFINE_CLIPBOARD_FORMAT(CF_GMOVIE_TEXT2)
//DEFINE_CLIPBOARD_FORMAT(CF_GMOVIE_GUIDLINE)

/*
	os:			output stream.
	objList:	objects to write.
	objListSel: objects that selected in the library tree. 
				these objects should be pasted even if they already exist in gldLibrary.
*/
/*
void WriteSymbolsToBinStream(oBinStream &os, const GOBJECT_LIST &objList, const GOBJECT_LIST &objListSel)
{
	ASSERT(objList.size() > 0);
	os << (U32)objList.size();
	for (GOBJECT_LIST::const_iterator it = objList.begin(); it != objList.end(); it++)
	{
		gldObj *_obj = *it;

		bool bSel = (find(objListSel.begin(), objListSel.end(), _obj) != objListSel.end());

		os << bSel;

		os << _obj->m_uniId.procId;
		os << _obj->m_uniId.docId;
		os << _obj->m_uniId.libId;

        os << (U16)_obj->GetGObjType();
		_obj->WriteToBinStream(os);
	}
}

void ReadSymbolsFromBinStream(iBinStream &is, TCommandGroup *pCmdGroup)
{
	gldLibrary *_lib = gldDataKeeper::Instance()->m_objLib;
	_lib->ClearAllObjID();

	std::list<UniId> idList;

	U32 size;
	is >> size;
	for (U32 i = 0; i < size; i++)
	{
		bool bSel;

		is >> bSel;

		UniId id;
		is >> id.procId;
		is >> id.docId;
		is >> id.libId;

		U16 _gobjType = 0;
        is >> _gobjType;

        gldObj *_obj = NULL;
        switch (_gobjType)
        {
            case gobjShape:
            {
                _obj = new gldShape();
                break;
            }
            case gobjImage:
            {
                _obj = new gldImage();
                break;
            }
            case gobjText:
            {
                _obj = new gldText2();
                break;
            }
            case gobjFont:
            {
                _obj = new gldFont();
                break;
            }
            case gobjButton:
            {
                _obj = new gldButton();
                break;
            }
            case gobjSprite:
            {
                _obj = new gldSprite();
                break;
            }
			case gobjESprite:
			{
				_obj = new gldEffectSprite();
				break;
			}
            case gobjSound:
            {
                _obj = new gldSound();
                break;
            }
            case gobjMShape:
            {
                _obj = new gldMorphShape();
                break;
            }
        }
        _obj->ReadFromBinStream(is);
        if (_obj != NULL)
		{
			_obj->m_uniId = id;

			bool bAdd = FALSE;

			gldObj *_objExist = _lib->FindObj(_obj);
			if (!_objExist)
			{
				bAdd = true;
			}
			else
			{
				if (bSel)	// always add flag
				{
					if (find(idList.begin(), idList.end(), id) == idList.end())	// has it been added in previous loop?
					{
						bAdd = true;
						_obj->UpdateUniId();
					}
				}
			}
			
			if (bAdd)
			{
				if (_lib->CreateObjPtr)
					_lib->CreateObjPtr(NULL, _obj);
				if (_lib->UpdateObjPtr)
					_lib->UpdateObjPtr(_obj);
				
				// generate a unique name
				CString strName = _obj->m_name.c_str();
				const GOBJECT_LIST &objList = _lib->GetObjList();
				for (int i = 0; ;i++)
				{
					bool found = false;
					for (GOBJECT_LIST_CIT it = objList.begin(); it != objList.end(); it++)
					{
						gldObj *pObj = *it;
						if (strName.CompareNoCase(pObj->m_name.c_str()) == 0)
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						_obj->m_name = strName;
						break;
					}
					else
					{
						if (i == 0)
							strName.Format(IDS_STH_COPY1, _obj->m_name.c_str());
						else
							strName.Format(IDS_STH_COPY_N2, _obj->m_name.c_str(), i);
					}
				}

				pCmdGroup->Do(new CCmdInsertObj(_obj, true, false));
				if (_obj->IsGObjInstanceOf(gobjImage))
					pCmdGroup->Do(new CCmdInsertImage(_obj));

				idList.push_back(id);
			}
			else
			{
				_objExist->m_id = _obj->m_id;
				delete _obj;
			}
		}
	}
}
*/

void GetUsedObjsRecursive(gldObj *pObj, gldLibrary *pLib)
{
	GOBJECT_LIST objListTemp;
	pObj->GetUsedObjs(objListTemp);

	for (GOBJECT_LIST_IT it = objListTemp.begin(); it != objListTemp.end(); it++)
		GetUsedObjsRecursive(*it, pLib);

    for (GOBJECT_LIST_IT it = objListTemp.begin(); it != objListTemp.end(); it++)
    {
        gldObj *_obj = (*it);
        ASSERT(_obj != NULL);

		GOBJECT_LIST objList = pLib->GetObjList();
		if (find(objList.begin(), objList.end(), _obj) == objList.end())
            pLib->AddObj(_obj, false);
    }

	GOBJECT_LIST objList = pLib->GetObjList();
    if (find(objList.begin(), objList.end(), pObj) == objList.end())
		pLib->AddObj(pObj, false);
}

BOOL CanPlaceInsideCurObj(gldObj *pObj)
{
	gldObj *pCurObj = CSWFProxy::GetCurObj();
	return CanPlaceInsideObj(pCurObj, pObj);
}

BOOL CanPlaceInsideObj(gldObj *pParentObj, gldObj *pChildObj)
{
	if (pParentObj == pChildObj)
		return FALSE;

	BOOL ret = TRUE;

	gldLibrary lib;
	GetUsedObjsRecursive(pChildObj, &lib);
	const GOBJECT_LIST &objList = lib.GetObjList();
	if (find(objList.begin(), objList.end(), pParentObj) != objList.end())
		ret = FALSE;
	lib.ClearList();

	return ret;
}
