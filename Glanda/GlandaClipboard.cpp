#include "StdAfx.h"
#include "glandaclipboard.h"
#include "EffectWnd.h"
#include "DesignWnd.h"
#include "SceneListCtrl.h"
#include "my_app.h"
#include "GlandaDoc.h"
#include "CmdInsertEffect.h"
#include "Clipboard.h"
#include "SWFStream.h"
#include "CmdAddShape.h"
#include "gldShape.h"
#include "gldImage.h"
#include "TextToolEx.h"
#include "gldButton.h"
#include "gldSprite.h"
#include "gldMorphShape.h"
#include "gldSound.h"
#include "gldFont.h"
#include "CmdInsertScene.h"
#include "CmdDeleteScene.h"
#include "CmdChangeCurrentScene.h"
#include "CmdRemoveInstance.h"
#include "filepath.h"
#include "EffectBar.h"
#include "CmdAddSound.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include <vector>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGlandaClipboard Clipboard;

#ifdef _DEBUG
int CGlandaClipboard::s_Count = 0;
#endif

CGlandaClipboard::CbFmtEntry CGlandaClipboard::s_FmtMap[] = {
	{"Sothink.Glanda.Instance", 0},	// CbFmtInstance
	{"Sothink.Glanda.Scene", 0},	// CbFmtScene
	{"Sothink.Glanda.Effect", 0},	// CbFmtEffect
	{"Sothink.Glanda.Sound", 0},	// CbFmtEffect
};

///////////////////////////////////////////////////////////////////////////////
//
// Helper class
//
///////////////////////////////////////////////////////////////////////////////
CReplaceLibrary::CReplaceLibrary(gldLibrary *pLib)
: m_pOldLib(NULL)
{
	if (pLib != NULL)
	{
		m_pOldLib = gldDataKeeper::Instance()->m_objLib;
		gldDataKeeper::Instance()->m_objLib = pLib;
	}
}

CReplaceLibrary::~CReplaceLibrary()
{
	if (m_pOldLib != NULL)
		gldDataKeeper::Instance()->m_objLib = m_pOldLib;
}

void GObjPlaceList::FreeSymbols()
{
	for (iterator i = begin(); i != end(); ++i)
	{
		if ((*i).place == GObjPlace::IS_NEW)
			CObjectMap::FreeSymbol(&(*i).pObj);
	}
	clear();
}

void GObjPlaceList::CopyTo(gldLibrary *pLib)
{
	for (iterator i = begin(); i != end(); ++i)
		pLib->AddObj((*i).pObj, false);
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Helper function
//
//////////////////////////////////////////////////////////////////////////////////////
gldObj *CreateObject(GObjType type)
{
	gldObj *pObj = NULL;
    switch (type)
    {
        case gobjShape:
        {
            pObj = new gldShape();
            break;
        }
        case gobjImage:
        {
            pObj = new gldImage();
            break;
        }
        case gobjText:
        {
            pObj = new gldText2();
            break;
        }
        case gobjFont:
        {
            pObj = new gldFont();
            break;
        }
        case gobjButton:
        {
            pObj = new gldButton();
            break;
        }
        case gobjSprite:
        {
            pObj = new gldSprite();
            break;
        }
        case gobjSound:
        {
            pObj = new gldSound();
            break;
        }
        case gobjMShape:
        {
            pObj = new gldMorphShape();
            break;
        }
    }
	return pObj;
}

void NameCopyScene(gldScene2 *pScene)
{
	ASSERT(pScene != NULL);
	gldMainMovie2 *mainMovie = _GetMainMovie2();
	ASSERT(mainMovie != NULL);
	CString name;
	name.Format(IDS_COPY_OF_X, pScene->m_name.c_str());
	int num = 1;
	while (mainMovie->FindSceneByName(name) != NULL)
	{
		name.Format(IDS_COPY_OF_X_N, pScene->m_name.c_str(), num++);		
	}
	pScene->m_name = name;
}

BOOL IsReusable(GObjType type)
{
	return type == gobjImage || type == gobjSound;
}

void DeepSearchRefs(GOBJECT_LIST &lstObj, GOBJECT_LIST::iterator iStart)
{	
	for (GOBJECT_LIST::iterator it = iStart; it != lstObj.end(); ++it)
		(*it)->GetUsedObjs(lstObj);
}

void AddObjAndRefs(gldObj *pObj, GOBJECT_LIST &lstObj)
{
	if (find(lstObj.begin(), lstObj.end(), pObj) == lstObj.end())
	{
		lstObj.push_back(pObj);
		DeepSearchRefs(lstObj, --lstObj.end());
	}
}

void SortObjsByRef(GOBJECT_LIST &lstObj)
{	
	for (GOBJECT_LIST::iterator it = lstObj.begin(); it != lstObj.end(); ++it)
	{
		gldObj *pObj = *it;
		GOBJECT_LIST lstTmp;
		pObj->GetUsedObjs(lstTmp);		
		for (GOBJECT_LIST::iterator itt = lstTmp.begin(); itt != lstTmp.end(); ++itt)
		{
			GOBJECT_LIST::iterator itf;
			itf = find(lstObj.begin(), it, *itt);
			if (itf != it)
			{
				lstObj.push_back(*itf);
				lstObj.erase(itf);
			}
		}
	}
	lstObj.reverse();
}

BOOL WriteObjListToBinStream(GOBJECT_LIST &lstObj, oBinStream &os)
{
	SortObjsByRef(lstObj);

	int id = 1;	
	os << (WORD)lstObj.size();
	GOBJECT_LIST::iterator ito = lstObj.begin();
	for (; ito != lstObj.end(); ++ito)
	{
		gldObj *pObj = *ito;		
		os << pObj->m_uniId.procId;
		os << pObj->m_uniId.docId;
		os << pObj->m_uniId.libId;
		os << (WORD)pObj->GetGObjType();
		pObj->m_id = id++;
		pObj->WriteToBinStream(os);
	}

	return TRUE;
}

inline gldFont *FindFont(gldFont *pFont)
{
	return _GetObjectMap()->FindFont(
				pFont->GetFontName().c_str(), 
				pFont->IsBold(), pFont->IsItalic(), 
				pFont->IsWidwCodes());			
}

BOOL ReadObjListFromBinStream(iBinStream &is, GObjPlaceList &lstObj)
{	
	ASSERT(lstObj.size() == 0);

	gldLibrary lib;
	CReplaceLibrary xRplLib(&lib);	

	WORD cntObj = 0;
	is >> cntObj;
	WORD index = 1;
	for (; index < cntObj + 1; index++)
	{
		UniId id;
		is >> id.procId;
		is >> id.docId;
		is >> id.libId;
		WORD type;
		is >> type;
		gldObj *pObj = CreateObject((GObjType)type);
		if (pObj == NULL)
			break;
		pObj->ReadFromBinStream(is);
		pObj->m_ptr = NULL;
		CSWFProxy::PrepareTShapeForGObj(pObj);
		int nPlace = GObjPlace::IS_NEW;
		if (IsReusable(pObj->GetGObjType()))
		{
			gldObj *pExitObj = _GetObjectMap()->FindObjectByUniid(id);
			if (pExitObj != NULL)
			{
				CObjectMap::FreeSymbol(&pObj);
				pObj = pExitObj;
				nPlace = GObjPlace::IN_LIBRARY;
			}
		}		
		pObj->m_uniId = id;
		pObj->m_id = index;		
		lib.AddObj(pObj, false);
		if (!pObj->IsGObjInstanceOf(gobjFont))
			lstObj.push_back(GObjPlace(pObj, nPlace));
	}
	
	// gldFont is exception
	GOBJECT_LIST::const_iterator ito = lib.GetObjList().begin();
	for (; ito != lib.GetObjList().end(); ++ito)
	{
		if ((*ito)->IsGObjInstanceOf(gobjText))
		{
			gldText *pText = (gldText *)(*ito);
			gldFont *pRplFont = pText->m_dynaInfo.m_font;
			if (pRplFont != NULL)
			{
				gldFont *pFont = FindFont(pRplFont);
				if (pFont != NULL)
					pText->m_dynaInfo.m_font = pFont;
			}
			list<gldTextGlyphRecord *>::iterator itg = pText->m_glyphRecords.begin();
			for (; itg != pText->m_glyphRecords.end(); ++itg)
			{
				pRplFont = (*itg)->m_font;;
				if (pRplFont != NULL)
				{
					gldFont *pFont = FindFont(pRplFont);
					if (pFont != NULL)
						(*itg)->m_font = pFont;
				}
			}
		}		
	}
	
	ito = lib.GetObjList().begin();
	for (; ito != lib.GetObjList().end(); ++ito)
	{
		if ((*ito)->IsGObjInstanceOf(gobjFont))
		{
			GObjPlace op;
			gldFont *pFont = (gldFont *)(*ito);
			gldFont *pRplFont = FindFont(pFont);			
			if (pRplFont == NULL)
			{
				op.pObj = pFont;
				op.place = GObjPlace::IS_NEW;				
			}
			else
			{
				op.pObj = pRplFont;
				op.place = GObjPlace::IN_LIBRARY;
				CObjectMap::FreeSymbol((gldObj **)&pFont);
			}
			lstObj.push_back(op);
		}
	}

	lib.ClearList();

	if (index < cntObj + 1)
	{
		lstObj.FreeSymbols();
		return FALSE;
	}

	return TRUE;
}

HRESULT WriteSceneToGlobal(gldScene2 *pScene, HGLOBAL *phMem)
{
	ASSERT(pScene != NULL);
	ASSERT(phMem != NULL);
	CComPtr<IStream> pIStm;
	HRESULT hr = CreateStreamOnHGlobal(NULL, FALSE, &pIStm);
	if (SUCCEEDED(hr))
	{
		/* write library*/	
		GOBJECT_LIST lstObj;
		pScene->GetUsedObjs(lstObj);
		DeepSearchRefs(lstObj, lstObj.begin());
		oBinStream os;
		VERIFY(WriteObjListToBinStream(lstObj, os));
		hr = ::WriteToStream(os, pIStm);
		if (SUCCEEDED(hr))
		{
			/* write scene */
			hr = pScene->WriteToStream(pIStm, NULL);
			if (SUCCEEDED(hr))
			{	
				hr = GetHGlobalFromStream(pIStm, phMem);			
			}		
		}
	}
	return hr;
}

HRESULT AddSceneFromGlobal(HGLOBAL hMem, int nIndex)
{
	ASSERT(hMem != NULL);	
	gldScene2 *pScene = NULL;
	HRESULT hr = S_OK;
	GObjPlaceList lstObj;
	gldLibrary lib;
	CReplaceLibrary xRplLib(&lib);

	CComPtr<IStream> pIStm;
	hr = CreateStreamOnHGlobal(hMem, FALSE, &pIStm);
	if (SUCCEEDED(hr))
	{
		// read library
		iBinStream is;
		hr = ::ReadFromStream(pIStm, is);
		if (SUCCEEDED(hr))
		{			
			VERIFY(ReadObjListFromBinStream(is, lstObj));			
			lstObj.CopyTo(&lib);			
			// read scene
			pScene = new gldScene2;	
			hr = pScene->ReadFromStream(pIStm, (void *)&lib);			
		} // library stream read succeeded
	} // CreateStreamOnHGlobal succeeded	

	lib.ClearList();

	if (FAILED(hr))
	{
		// free objects
		//.. objects will be freed by libNewObj's destructor				
		if (pScene != NULL)
			delete pScene;
		lstObj.FreeSymbols();
		return hr;
	}
	else
	{
		CGuardDrawOnce xDraw;
		CGuardSelAppend xSel;

		typedef CCmdGroupX<CGuardDrawOnce, CGuardSelAppend> _MyCmdGroupX;

		_MyCmdGroupX *cmd = new _MyCmdGroupX(IDS_CMD_INSERTINSTANCE);

		// insert objects
		GObjPlaceList::iterator ito = lstObj.begin();
		for (; ito != lstObj.end(); ++ito)
		{
			if ((*ito).place == GObjPlace::IS_NEW)
				VERIFY(cmd->Do(new CCmdAddObj((*ito).pObj)));
			
		}
		
		// insert instance
		ASSERT(pScene != NULL);
		NameCopyScene(pScene);
		VERIFY(cmd->Do(new CCmdInsertScene(pScene, nIndex)));
		VERIFY(cmd->Do(new CCmdChangeCurrentScene(pScene)));
		if (my_app.Commands().Do(cmd))
		{
			CSceneListCtrl *pList = CSceneListCtrl::Instance();
			pList->EnsureVisible(pList->GetItemCount() - 1);
		}

		return S_OK;
	}
}

HRESULT WriteInstancesToGlobal(gld_shape_sel &sel, HGLOBAL *phMem)
{
	ASSERT(phMem != NULL);

	HRESULT hr = S_OK;

	CComPtr<IStream> pIStm;
	hr = CreateStreamOnHGlobal(NULL, FALSE, &pIStm);
	if (SUCCEEDED(hr))
	{	
		// write library
		GOBJECT_LIST lstObj;
		gld_shape_sel &sel = my_app.CurSel();
		for (gld_shape_sel::iterator its = sel.begin(); its != sel.end(); ++its)
		{
			gldInstance *pInst = CTraitInstance(*its);
			gldObj *pObj = pInst->m_obj;
			ASSERT(pObj != NULL);
			AddObjAndRefs(pObj, lstObj);				
		}
		SortObjsByRef(lstObj);			
		oBinStream os;
		VERIFY(WriteObjListToBinStream(lstObj, os));
		hr = ::WriteToStream(os, pIStm);
		if (SUCCEEDED(hr))
		{
			IStreamWrapper stm(pIStm);
			hr = stm.Write((WORD)sel.count());
			if (SUCCEEDED(hr))
			{
				// write instances
				gldLibrary lib;
				for (GOBJECT_LIST::iterator ito = lstObj.begin(); ito != lstObj.end(); ++ito)
					lib.AddObj(*ito, false);

				for (gld_shape_sel::iterator its = sel.begin(); its != sel.end(); ++its)
				{
					gldInstance *pInst = CTraitInstance(*its);
					hr = pInst->WriteToStream(pIStm, (void *)&lib);
					if (FAILED(hr))
						break;
				}
				if (SUCCEEDED(hr))
				{
					hr = GetHGlobalFromStream(pIStm, phMem);					
				}

				lib.ClearList();
			}
			if (FAILED(hr)) // free global memory
			{
				HGLOBAL hMem = NULL;
				GetHGlobalFromStream(pIStm, &hMem);
				if (hMem != NULL)
					GlobalFree(hMem);
			}
		}
	}

	return hr;
}

HRESULT AddInstancesFromGlobal(HGLOBAL hMem, int dx /*= 0*/, int dy /*= 0*/)
{
	vector<gldInstance *> insts;
	
	GObjPlaceList lstObj;
	gldLibrary lib;
	CReplaceLibrary xRplLib(&lib);

	HRESULT hr = S_OK;
	CComPtr<IStream> pIStm;
	hr = CreateStreamOnHGlobal(hMem, FALSE, &pIStm);
	if (SUCCEEDED(hr))
	{
		iBinStream is;
		hr = ::ReadFromStream(pIStm, is);
		if (SUCCEEDED(hr))
		{
			/* read library */					
			VERIFY(ReadObjListFromBinStream(is, lstObj));					
			lstObj.CopyTo(&lib);		
			// read instances						
			WORD count = 0;
			IStreamWrapper stm(pIStm);
			hr = stm.Read(count);
			if (SUCCEEDED(hr))
			{
				for (int i = 0; i < count; i++)
				{
					gldInstance *pInst = new gldInstance;
					insts.push_back(pInst);
					hr = pInst->ReadFromStream(pIStm, (void *)&lib);
					if (FAILED(hr))
						break;
					pInst->m_matrix.m_x += dx;
					pInst->m_matrix.m_y += dy;
				}
			}				
		} // library stream read succeeded
	} // CreateStreamOnHGlobal succeeded		

	lib.ClearList();

	if (FAILED(hr))
	{
		// free objects
		//.. objects will be freed by libNewObj's destructor
		// free instances
		vector<gldInstance *>::iterator iti = insts.begin();
		for (; iti != insts.end(); ++iti)
			delete *iti;
		insts.clear();
		lstObj.FreeSymbols();		
	}
	else
	{
		CGuardDrawOnce xDraw;
		CGuardSelAppend xSel;

		typedef CCmdGroupX<CGuardDrawOnce, CGuardSelAppend> _MyCmdGroupX;

		_MyCmdGroupX *cmd = new _MyCmdGroupX(IDS_CMD_INSERTINSTANCE);
		// insert objects		
		GObjPlaceList::iterator ito = lstObj.begin();
		for (; ito != lstObj.end(); ++ito)
		{
			if ((*ito).place == GObjPlace::IS_NEW)
				VERIFY(cmd->Do(new CCmdAddObj((*ito).pObj)));			
		}		
		// insert instance
		vector<gldInstance *>::iterator iti = insts.begin();
		for (; iti != insts.end(); ++iti)
			VERIFY(cmd->Do(new CCmdAddInstance(_GetCurScene2(), *iti)));
		my_app.Commands().Do(cmd);
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// CGlandaClipboard
//
///////////////////////////////////////////////////////////////////////////////////////
CGlandaClipboard::CGlandaClipboard(void)
{
#ifdef _DEBUG
	s_Count++;
	ASSERT(s_Count == 1);
#endif
}

CGlandaClipboard::~CGlandaClipboard(void)
{
#ifdef _DEBUG
	s_Count--;
	ASSERT(s_Count == 0);
#endif
}

BOOL CGlandaClipboard::Initialize()
{
	UINT nFmt = 0;
	for (int i = 0; i < sizeof(s_FmtMap) / sizeof(s_FmtMap[0]); i++)
	{		
		if ((nFmt = RegisterClipboardFormat(s_FmtMap[i].name)) == 0)
		{
			ReportLastError();
			return FALSE;
		}
		s_FmtMap[i].id = nFmt;
	}
	return TRUE;
}

void CGlandaClipboard::Copy()
{
	// Modifyed by Liujun (08/17/2004)
	if (!IsDataAvailable(ForCopy))
		return;

	if (CWnd::GetFocus() == CEffectWnd::Instance())
	{
		CEffectWnd::SEL_TYPE type = (CEffectWnd::SEL_TYPE)CEffectWnd::Instance()->GetSelType();
		switch (type)
		{
			case CEffectWnd::SEL_EFFECT:
				CopyEffect();
				break;
			case CEffectWnd::SEL_SOUND:
				CopySound();
				break;
			default:
				ASSERT(FALSE);
				break;
		}
	}
	else if (CWnd::GetFocus() == CDesignWnd::Instance())
	{
		CopyInstance();
	}
	else if (CWnd::GetFocus() == CSceneListCtrl::Instance())
	{
		CopyScene();
	}
}

void CGlandaClipboard::Paste()
{	
	// Modified by Liujun (08/17/2004)
	if (!IsDataAvailable(ForPaste))
		return;

	if (CWnd::GetFocus() == CEffectWnd::Instance())
	{
		if (IsClipboardFormatAvailable(s_FmtMap[CbFmtSound].id))
		{
			PasteSound();
		}
		else
		{
			CEffectWnd::SEL_TYPE type = (CEffectWnd::SEL_TYPE)CEffectWnd::Instance()->GetSelType();
			if (type == CEffectWnd::SEL_INST || type == CEffectWnd::SEL_EFFECT)
				PasteEffect();
		}
	}
	else if (CWnd::GetFocus() == CDesignWnd::Instance())
	{
		if (::IsClipboardFormatAvailable(CF_DIB))
			PasteDIB();
		else
			PasteInstance();
	}
	else if (CWnd::GetFocus() == CSceneListCtrl::Instance())
	{
		PasteScene();
	}
}

void CGlandaClipboard::Cut()
{
	if (!IsDataAvailable(ForCut))
		return;

	if (CWnd::GetFocus() == CEffectWnd::Instance())
	{
		CEffectWnd::SEL_TYPE type = (CEffectWnd::SEL_TYPE)CEffectWnd::Instance()->GetSelType();
		switch (type)
		{
			case CEffectWnd::SEL_EFFECT:
				CutEffect();
				break;
			case CEffectWnd::SEL_SOUND:
				CutSound();
				break;
			default:
				ASSERT(FALSE);
				break;
		}
	}
	else if (CWnd::GetFocus() == CDesignWnd::Instance())
	{
		CutInstance();
	}
	else if (CWnd::GetFocus() == CSceneListCtrl::Instance())
	{
		CutScene();
	}
}

void CGlandaClipboard::Delete()
{
	if (!IsDataAvailable(ForDelete))
		return;

	if (CWnd::GetFocus() == CEffectWnd::Instance())
	{
		CEffectWnd::SEL_TYPE type = (CEffectWnd::SEL_TYPE)CEffectWnd::Instance()->GetSelType();
		switch (type)
		{
			case CEffectWnd::SEL_INST: 
			case CEffectWnd::SEL_EFFECT:
				DeleteEffect();
				break;

			case CEffectWnd::SEL_SCAPT:
			case CEffectWnd::SEL_SOUND:
				DeleteSound();
				break;

			default:
				ASSERT(FALSE);
				break;
		}
	}
	else if (CWnd::GetFocus() == CDesignWnd::Instance())
	{
		DeleteInstance();
	}
	else if (CWnd::GetFocus() == CSceneListCtrl::Instance())
	{
		DeleteScene();
	}
}

BOOL CGlandaClipboard::IsDataAvailable(int opt /*= ForCopy*/)
{
	if (opt == ForCopy || opt == ForCut || opt == ForDelete)
	{
		if (CWnd::GetFocus() == CEffectWnd::Instance())
		{
			CEffectWnd::SEL_TYPE type = CEffectWnd::Instance()->GetSelType();
			if (opt == ForCopy || opt == ForCut)
				return  type == CEffectWnd::SEL_EFFECT || type == CEffectWnd::SEL_SOUND;
			else
				return  type != CEffectWnd::SEL_VOID;
		}
		else if (CWnd::GetFocus() == CDesignWnd::Instance())		
		{
			return my_app.CurSel().count() > 0;		
		}
		else if (CWnd::GetFocus() == CSceneListCtrl::Instance())
		{
			return _GetMainMovie2()->GetCurScene() != NULL;
		}
	}
	else if (opt == ForPaste)
	{
		if (CWnd::GetFocus() == CEffectWnd::Instance())
		{
			if (IsClipboardFormatAvailable(s_FmtMap[CbFmtSound].id))
				return TRUE;

			CEffectWnd::SEL_TYPE type = CEffectWnd::Instance()->GetSelType();
			return (type == CEffectWnd::SEL_INST || type == CEffectWnd::SEL_EFFECT) && IsClipboardFormatAvailable(s_FmtMap[CbFmtEffect].id);
		}
		else if (CWnd::GetFocus() == CDesignWnd::Instance())
		{
			return _GetCurScene2() != NULL &&
				(::IsClipboardFormatAvailable(s_FmtMap[CbFmtInstance].id) ||
				::IsClipboardFormatAvailable(CF_DIB));
		}
		else if (CWnd::GetFocus() == CSceneListCtrl::Instance())
		{
			return IsClipboardFormatAvailable(s_FmtMap[CbFmtScene].id);
		}
	}

	return FALSE;
}

BOOL CGlandaClipboard::CopyEffect()
{	
	BOOL Failed = TRUE;

	if (OpenClipboard(NULL))
	{
		HRESULT hr;		
		gldEffect *pEffect = CEffectWnd::Instance()->GetSelEffect();
		ASSERT(pEffect != NULL);
		CComPtr<IStream> pIStm;
		hr = CreateStreamOnHGlobal(NULL, FALSE, &pIStm);
		if (SUCCEEDED(hr))
		{				
			hr = pEffect->WriteToStream(pIStm, NULL);
			if (SUCCEEDED(hr))
			{	
				HGLOBAL hMem = NULL;
				GetHGlobalFromStream(pIStm, &hMem);
				if (hMem != NULL)
				{
					EmptyClipboard();
					Failed = (SetClipboardData(s_FmtMap[CbFmtEffect].id, hMem) == NULL);
				}
			}
			if (Failed) // free global memory
			{
				HGLOBAL hMem = NULL;
				GetHGlobalFromStream(pIStm, &hMem);
				if (hMem != NULL)
					GlobalFree(hMem);
			}
		}
		CloseClipboard();
	}

	if (Failed)
		AfxMessageBox(IDS_FAILED_SET_CLIPBOARD_DATA);

	return !Failed;
}

BOOL CGlandaClipboard::CopySound()
{	
	BOOL Failed = TRUE;

	if (OpenClipboard(NULL))
	{
		HRESULT hr;		
		gldSceneSound *pSceneSound = CEffectWnd::Instance()->GetSelSound();
		ASSERT(pSceneSound);

		gldSound *pSound = pSceneSound->m_sound->m_soundObj;

		CComPtr<IStream> pIStm;
		hr = CreateStreamOnHGlobal(NULL, FALSE, &pIStm);
		if (SUCCEEDED(hr))
		{				
			// Write gldSound to stream
			oBinStream os;
			os << pSound->m_uniId.procId;
			os << pSound->m_uniId.docId;
			os << pSound->m_uniId.libId;

			pSound->m_id = 1;
			pSound->WriteToBinStream(os);
			hr = ::WriteToStream(os, pIStm);
			if (SUCCEEDED(hr))
			{
				hr = pSceneSound->WriteToStream(pIStm, NULL);
				if (SUCCEEDED(hr))
				{	
					HGLOBAL hMem = NULL;
					GetHGlobalFromStream(pIStm, &hMem);
					if (hMem != NULL)
					{
						EmptyClipboard();
						Failed = (SetClipboardData(s_FmtMap[CbFmtSound].id, hMem) == NULL);
					}
				}
				if (Failed) // free global memory
				{
					HGLOBAL hMem = NULL;
					GetHGlobalFromStream(pIStm, &hMem);
					if (hMem != NULL)
						GlobalFree(hMem);
				}
			}
		}

		CloseClipboard();
	}

	if (Failed)
		AfxMessageBox(IDS_FAILED_SET_CLIPBOARD_DATA);

	return !Failed;
}

BOOL CGlandaClipboard::CopyInstance()
{
	HGLOBAL hMem = NULL;
	
	WriteInstancesToGlobal(my_app.CurSel(), &hMem);
	
	if (hMem != NULL)
	{
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			if (SetClipboardData(s_FmtMap[CbFmtInstance].id, hMem) != NULL)
			{
				CloseClipboard();
				return TRUE; // succeeded set instances to clipboard
			}
			CloseClipboard();
		}
		GlobalFree(hMem);
	}

	AfxMessageBox(IDS_FAILED_SET_CLIPBOARD_DATA);

	return FALSE;	
}

BOOL CGlandaClipboard::CopyScene()
{
	HGLOBAL hMem = NULL;
	BOOL Failed = TRUE;	

	if (SUCCEEDED(WriteSceneToGlobal(_GetCurScene2(), &hMem)))
	{
		if (OpenClipboard(NULL))
		{
			ASSERT(hMem != NULL);
			EmptyClipboard();
			Failed = (SetClipboardData(s_FmtMap[CbFmtScene].id, hMem) == NULL);
			CloseClipboard();
		}
	}
	
	if (Failed) // free global memory
	{		
		if (hMem != NULL)
			GlobalFree(hMem);

		AfxMessageBox(IDS_FAILED_SET_CLIPBOARD_DATA);
	}

	return !Failed;			
}

void CGlandaClipboard::PasteDIB()
{
	if (!::OpenClipboard(NULL))
	{
		::ReportLastError();
		return;
	}

	CString sTempPath = GetTempFile(NULL, "dib", "bmp");
	FILE *fp;
	if ((fp = fopen(sTempPath, "wb")) == NULL)
	{
		VERIFY(::CloseClipboard());
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_CREATE_TEMP_FILE);
		return;
	}	

	HGLOBAL hData = ::GetClipboardData(CF_DIB);
	if (hData == NULL)
	{
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);		
		AfxMessageBox(IDS_E_GET_CLIPBOARD_DATA);
		return;
	}
	
	size_t dwSize = ::GlobalSize(hData);
	void *pData = ::GlobalLock(hData);	
	ASSERT(pData);
	if (dwSize < sizeof(BITMAPINFOHEADER))
	{		
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_CLIPBOARD_FORMAT);
		return;
	}
	BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)pData;
	if (bih->biSize != sizeof(BITMAPINFOHEADER))
	{		
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_CLIPBOARD_FORMAT);
		return;
	}
	size_t cbPalette = 0;
	if (bih->biBitCount <=8 )
	{
		if (bih->biClrUsed != 0)
			cbPalette = sizeof(RGBQUAD) * bih->biClrUsed;
		else
			cbPalette = (1 << bih->biBitCount) * sizeof(RGBQUAD);
	}
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 0x4d42;
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + (DWORD)dwSize;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + cbPalette;
	if (fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER))
	{		
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_WRITE_TEMP_FILE);
		return;
	}
	if (fwrite(pData, 1, dwSize, fp) < dwSize)
	{
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_WRITE_TEMP_FILE);
		return;
	}	
	VERIFY(::GlobalUnlock(hData));
	VERIFY(::CloseClipboard());
	fclose(fp);
	CSize szView = my_app.GetViewSize();	
	_GetCurDocument()->ImportImage(sTempPath, CPoint(szView.cx / 2, szView.cy / 2), TRUE);
	DeleteFile(sTempPath);
}

void CGlandaClipboard::PasteEffect()
{
	BOOL Failed = TRUE;

	if (OpenClipboard(NULL))
	{
		HANDLE hMem = GetClipboardData(s_FmtMap[CbFmtEffect].id);
		if (hMem != NULL)
		{
			HRESULT hr;		
			CComPtr<IStream> pIStm;
			hr = CreateStreamOnHGlobal(hMem, FALSE, &pIStm);
			if (SUCCEEDED(hr))
			{
				gldEffect *pEffect = new gldEffect;
				
				hr = pEffect->ReadFromStream(pIStm, NULL);
				if (SUCCEEDED(hr))
				{
					// insert before current selected effect
					gldInstance *pInst = CEffectWnd::Instance()->GetSelInstance();
					ASSERT(pInst != NULL);	
					VERIFY(my_app.Commands().Do(new CCmdInsertEffect(pInst, 
						pEffect, CEffectWnd::Instance()->GetSelEffect())));
					Failed = FALSE;
				}
				
				if (Failed)
				{
					delete pEffect;				
				}
			}
		}

		CloseClipboard();
	}

	if (Failed)
		AfxMessageBox(IDS_FAILED_GET_CLIPBOARD_DATA);
}

void CGlandaClipboard::PasteSound()
{
	BOOL Failed = TRUE;

	if (OpenClipboard(NULL))
	{
		HANDLE hMem = GetClipboardData(s_FmtMap[CbFmtSound].id);
		if (hMem != NULL)
		{
			HRESULT hr;		
			CComPtr<IStream> pIStm;
			hr = CreateStreamOnHGlobal(hMem, FALSE, &pIStm);
			if (SUCCEEDED(hr))
			{
				gldLibrary lib;
				CReplaceLibrary xRplLib(&lib);

				iBinStream is;
				hr = ::ReadFromStream(pIStm, is);
				if (SUCCEEDED(hr))
				{
					gldSound *pSound = new gldSound;

					is >> pSound->m_uniId.procId;
					is >> pSound->m_uniId.docId;
					is >> pSound->m_uniId.libId;

					pSound->ReadFromBinStream(is);

					gldSound *pSoundExisting = (gldSound *)_GetObjectMap()->FindObjectByUniid(pSound->m_uniId);
					if (pSoundExisting != NULL)
					{
						delete pSound;
						pSound = pSoundExisting;
					}

					pSound->m_id = 1;
					lib.AddObj(pSound, false);

					gldSceneSound *pSceneSound = new gldSceneSound;
					hr = pSceneSound->ReadFromStream(pIStm, NULL);
					if (SUCCEEDED(hr))
					{
						TCommandGroup *pCmd = new TCommandGroup(IDS_CMD_CHANGESOUND);
						// if it's a new symbol, must add it to Object Map.
						if (pSoundExisting == NULL)
							pCmd->Do(new CCmdAddObj(pSound));
						pCmd->Do(new CCmdAddSound(pSceneSound));
						my_app.Commands().Do(pCmd);
						Failed = FALSE;						
					}

					if (Failed)
					{
						delete pSceneSound;
						if (pSoundExisting == NULL)
							delete pSound;
					}
				}

				// must remove the sound just added from the objList, so it will 
				// not be deleted.
				lib.ClearList();
			}
		}

		CloseClipboard();
	}

	if (Failed)
		AfxMessageBox(IDS_FAILED_GET_CLIPBOARD_DATA);
}

void CGlandaClipboard::PasteInstance()
{	
	vector<gldInstance *> insts;
	
	GObjPlaceList lstObj;
	gldLibrary lib;
	CReplaceLibrary xRplLib(&lib);

	BOOL Failed = TRUE;

	if (OpenClipboard(NULL))
	{
		HGLOBAL hMem = GetClipboardData(s_FmtMap[CbFmtInstance].id);
		if (hMem != NULL)
		{
			Failed = FAILED(AddInstancesFromGlobal(hMem));
		}
		CloseClipboard();
	}

	if (Failed)
		AfxMessageBox(IDS_FAILED_GET_CLIPBOARD_DATA);	
}

void CGlandaClipboard::PasteScene()
{	
	if (OpenClipboard(NULL))
	{
		HANDLE hMem = GetClipboardData(s_FmtMap[CbFmtScene].id);
		if (hMem != NULL)
		{
			if (FAILED(AddSceneFromGlobal(hMem, _GetMainMovie2()->m_sceneList.size())))
				AfxMessageBox(IDS_FAILED_GET_CLIPBOARD_DATA);
		}
		CloseClipboard();
	}	
}

void CGlandaClipboard::CutEffect()
{
	if (CopyEffect())
		DeleteEffect();
}

void CGlandaClipboard::CutSound()
{
	if (CopySound())
		DeleteSound();
}

void CGlandaClipboard::CutInstance()
{
	if (CopyInstance())
		DeleteInstance();
}

void CGlandaClipboard::CutScene()
{
	if (CopyScene())
		DeleteScene();
}

void CGlandaClipboard::DeleteEffect()
{
	CEffectBar::Instance()->m_dlgEffect.OnBnClickedBtnRemove();
}

void CGlandaClipboard::DeleteSound()
{
	CEffectBar::Instance()->m_dlgEffect.OnBnClickedBtnRemove();
}

void CGlandaClipboard::DeleteInstance()
{
	if (my_app.CurSel().count() > 0)
	{		
		my_app.Commands().Do(new CCmdRemoveInstance(my_app.CurSel()));		
	}
}

void CGlandaClipboard::DeleteScene()
{
	gldMainMovie2 *pMovie = _GetMainMovie2();
	gldScene2 *pScene = _GetCurScene2();
	if (pScene)
	{
		CSceneListCtrl *pList = CSceneListCtrl::Instance();

		int index = pList->GetCurSel();
		int count = pList->GetItemCount();
		
		gldScene2 *pSceneNew = pMovie->GetScene(index < count - 1 ? index + 1 : index - 1);

		pList->SetRedraw(FALSE);

		TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_DELETESCENE);
		pCmdGroup->Do(new CCmdChangeCurrentScene(pSceneNew));
		pCmdGroup->Do(new CCmdDeleteScene(pScene, true));
		my_app.Commands().Do(pCmdGroup);

		pList->SetRedraw(TRUE);
		pList->Invalidate(FALSE);
	}
}
