#include "StdAfx.h"
#include "Glanda.h"
#include "TextToolEx.h"
#include ".\gldinstance.h"

#include "gldSprite.h"
#include "gldMovieClip.h"
#include "gldEffect.h"
#include "Observer.h"

#include "gldLayer.h"
#include "gldFrameClip.h"

#include "I2C.h"
#include "C2I.h"

#include <algorithm>

#include "SWFProxy.h"
#include "ObjInstance.h"

#include <set>
#include "GlandaDoc.h"
#include "ObjectMap.h"

#include "GLD_PropertySheet.h"

#include "../Glanda/EffectCategory.h"

#include "EffectCommonParameters.h"
#include "SWFStream.h"
#include "gldLibrary.h"
#include "my_app.h"
#include "glandaclipboard.h"
#include "ComUtils.h"
#include "CoException.h"
#include "MoviePreview.h"
#include "filepath.h"
#include "CmdInsertEffect.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

gldInstance::gldInstance()
{
	m_obj = NULL;
	m_tx = 0;
	m_ty = 0;
	m_ptr = NULL;
	m_mc = NULL;

	m_cxStyle = INST_COLOR_TRANS_STYLE_NONE;
	m_tint = 0x00ffffff;

	m_mask = FALSE;
	m_expanded = TRUE;

	VERIFY(SUCCEEDED(m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters))));
	if (m_pIParas != NULL)
		PutParameter(m_pIParas, EP_LOOP, TRUE);
}

gldInstance::gldInstance(gldObj *pObj, const gldMatrix &matrix, const gldCxform &cxform)
{
	m_obj = pObj;
	m_matrix = matrix;
	m_cxform = cxform;
	m_tx = 0;
	m_ty = 0;
	m_ptr = NULL;
	m_mc = NULL;

	m_cxStyle = INST_COLOR_TRANS_STYLE_NONE;
	m_tint = 0x00ffffff;

	m_mask = FALSE;
	m_expanded = TRUE;

	VERIFY(SUCCEEDED(m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters))));
	if (m_pIParas != NULL)
		PutParameter(m_pIParas, EP_LOOP, TRUE);
}

gldInstance::~gldInstance(void)
{
	if (m_mc)
		delete m_mc;

	for (GEFFECT_LIST::iterator it = m_effectList.begin(); it != m_effectList.end(); it++)
		delete *it;

	m_effectList.clear();
}

void ReleaseMovieClipUseObjects(gldMovieClip *pMovie)
{
	GOBJECT_LIST lstObj;
	pMovie->GetUsedObjs(lstObj);

	set<gldObj *> setObj;
	GOBJECT_LIST::iterator i = lstObj.begin();
	for (; i != lstObj.end(); ++i)
		setObj.insert(*i);

	CObjectMap *mapObj = _GetObjectMap();
	set<gldObj *>::iterator j = setObj.begin();
	for (; j != setObj.end(); ++j)
	{
		gldObj *pObj = *j;
		ULONG nRef = mapObj->Unuse(pObj);
		if (nRef == 0)
			CObjectMap::FreeSymbol(&pObj);
	}
}

/*
typedef std::list <GUID> GUID_LIST;
BOOL GetPropertyPageList(REFCLSID catid, REFCLSID clsid, GUID_LIST &guidList)
{
	ASSERT(guidList.size() == 0);

	BSTR bstrCATID = NULL;
	HRESULT hr = ::StringFromCLSID(catid, &bstrCATID);
	if (SUCCEEDED(hr))
	{
		USES_CONVERSION;

		CString strCATID = OLE2A(bstrCATID);
		::CoTaskMemFree(bstrCATID);

		BSTR bstrCLSID = NULL;
		HRESULT hr = ::StringFromCLSID(clsid, &bstrCLSID);
		if (SUCCEEDED(hr))
		{
			CString strCLSID = OLE2A(bstrCLSID);
			::CoTaskMemFree(bstrCLSID);

			CRegKey reg;

			CString strCATIDKey;
			strCATIDKey.Format("CLSID\\%s\\%s\\PropertyPages", strCATID, strCLSID);
			
			if (reg.Open(HKEY_CLASSES_ROOT, strCATIDKey, KEY_READ) == ERROR_SUCCESS)
			{
				TCHAR szName[255];				
				DWORD dwIndex = 0;
				while (true)
				{
					DWORD dwLen = 255; // must init for each call
					if (reg.EnumKey(dwIndex, szName, &dwLen, NULL) != ERROR_SUCCESS)
						break;

					dwIndex++;

					CLSID clsid;
					if (SUCCEEDED(CLSIDFromString(A2OLE(szName), &clsid)))
						guidList.push_back(clsid);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}
*/

#define CopyCommonParameter(szName)	\
{	\
	CComVariant vt_##szName;	\
	if(SUCCEEDED(pSrc->get_Item(CComBSTR(szName), &vt_##szName)))	\
		pDst->put_Item(CComBSTR(szName), vt_##szName);	\
}

void CopyCommonParameters(IGLD_Parameters *pSrc, IGLD_Parameters *pDst)
{
	ASSERT(pSrc != NULL && pDst != NULL);

	CopyCommonParameter(EP_BREAK_APART);

	CopyCommonParameter(EP_TEXT_BORDER);
	CopyCommonParameter(EP_BORDER_WIDTH);
	CopyCommonParameter(EP_BORDER_COLOR);
	CopyCommonParameter(EP_CUSTOM_FILL);
	CopyCommonParameter(EP_FILL_STYLE);
	CopyCommonParameter(EP_FILL_INDIVIDUALLY);
	CopyCommonParameter(EP_FILL_ANGLE);
	CopyCommonParameter(EP_ADD_SHADOW);
	CopyCommonParameter(EP_SHADOW_SIZE);
	CopyCommonParameter(EP_SHADOW_COLOR);

	CopyCommonParameter(EP_AUTO_PLAY);
	CopyCommonParameter(EP_LOOP);
}

#define RemoveCommonParameter(szName)	\
{	\
	CComVariant vt_##szName;	\
	pSrc->Remove(CComBSTR(szName));	\
}

void RemoveCommonParameters(IGLD_Parameters *pSrc)
{
	ASSERT(pSrc != NULL);

	RemoveCommonParameter(EP_BREAK_APART);

	RemoveCommonParameter(EP_TEXT_BORDER);
	RemoveCommonParameter(EP_BORDER_WIDTH);
	RemoveCommonParameter(EP_BORDER_COLOR);
	RemoveCommonParameter(EP_CUSTOM_FILL);
	RemoveCommonParameter(EP_FILL_STYLE);
	RemoveCommonParameter(EP_FILL_INDIVIDUALLY);
	RemoveCommonParameter(EP_FILL_ANGLE);
	RemoveCommonParameter(EP_ADD_SHADOW);
	RemoveCommonParameter(EP_SHADOW_SIZE);
	RemoveCommonParameter(EP_SHADOW_COLOR);

	RemoveCommonParameter(EP_AUTO_PLAY);
	RemoveCommonParameter(EP_LOOP);
}

BOOL gldInstance::ConfigEffect(gldInstance *pInstance, gldEffect *pEffect)
{
	#define CheckResult() if (FAILED(hr)) return FALSE;

	HRESULT hr;

	GUID *pPages = NULL;
	long nPages = 0;

	if (pEffect && pEffect->m_pIEffect == NULL)
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, 
			"Failed to configure this effect. The effect does not exist on your system or it is not registered correctly.");
		return FALSE;
	}

	if (pEffect)
	{
		hr = pEffect->m_pIEffect->GetPropertyPages(&pPages, &nPages);
		CheckResult();
	}

#ifdef _DEBUG
	if (pEffect && nPages == 0)
	{
		AfxMessageBox("No property pages associated with this effect.");
	}
#endif

	CComPtr<IGLD_Parameters> pIParasAll;
	hr = pIParasAll.CoCreateInstance(__uuidof(GLD_Parameters));
	CheckResult();

	if (pEffect)
		pIParasAll->Copy(pEffect->m_pIParas);

	CopyCommonParameters(m_pIParas, pIParasAll);

	GUID *guids = new GUID[nPages + 1];
	guids[0] = CLSID_CommonPage;

	if (pEffect)
	{
		memcpy(guids + 1, pPages, sizeof(GUID) * (nPages));
		::CoTaskMemFree(pPages);
	}

	BOOL ret = CreatePropertySheet(AfxGetMainWnd()->m_hWnd, CW_USEDEFAULT, CW_USEDEFAULT, "Effect Settings", pIParasAll, nPages + 1, guids, pEffect ? 1 : 0, TRUE);

	delete guids;

	if (ret)
	{
		CComPtr<IGLD_Parameters> pIParasCommon;
		hr = pIParasCommon.CoCreateInstance(__uuidof(GLD_Parameters));
		CheckResult();

		CComPtr<IGLD_Parameters> pIParasEffect;
		if (pEffect)
		{
			hr = pIParasEffect.CoCreateInstance(__uuidof(GLD_Parameters));
			CheckResult();
		}

		CopyCommonParameters(pIParasAll, pIParasCommon);

		if (pEffect)
		{
			RemoveCommonParameters(pIParasAll);
			pIParasEffect->Copy(pIParasAll);
		}

		TCommandGroup *pCmd = new TCommandGroup(IDS_CMD_CHANGEEFFECTS);

		pCmd->Do(new CCmdChangeEffectParameters(m_pIParas, pIParasCommon));

		if (pEffect)
			pCmd->Do(new CCmdChangeEffectParameters(pEffect->m_pIParas, pIParasEffect));

		return my_app.Commands().Do(pCmd) ? TRUE: FALSE;
	}

	return FALSE;
}

BOOL gldInstance::BuildEffects(EnumErrorHandleMethod method /*= IS_PROMPT*/)
{
	if (m_mc)
	{
		ReleaseMovieClipUseObjects(m_mc);
		delete m_mc;
		m_mc = NULL;
	}

	if (m_effectList.size() == 0)
	{
		return TRUE;
	}

	CAutoPtr<gldMovieClip> pMovieClip(new gldMovieClip());

	gldLayer *pLayer = new gldLayer("", 0);
	pMovieClip->AddLayer(pLayer);

	int nMinTime = GetMinTime();
	int nMaxTime = GetMaxTime();
	ASSERT(nMaxTime > 0);

	// add a blank frame clip if the first effect start time > 0.
	if (nMinTime > 0)
	{
		gldFrameClip *pFrameClipBlank = new gldFrameClip(0, nMinTime);
		pLayer->m_frameClipList.push_back(pFrameClipBlank);
	}
	gldFrameClip *pFrameClipEffect = new gldFrameClip(nMinTime, 1);
	pLayer->m_frameClipList.push_back(pFrameClipEffect);

	// push effect character key
	VARIANT_BOOL bBreakApart = VARIANT_TRUE;
	GetParameter(m_pIParas, EP_BREAK_APART, &bBreakApart);
	switch (m_obj->GetGObjType())
	{
		case gobjText:
		{
			if (bBreakApart == VARIANT_TRUE)
			{
				OBJINSTANCE_LIST oiList;
				CSWFProxy::BreakApartTextToShapes((gldText *)m_obj, gldMatrix(), m_cxform, m_pIParas, oiList);

				for (OBJINSTANCE_LIST::iterator it = oiList.begin(); it != oiList.end(); it++)
				{
					ObjInstance oi = *it;
					pFrameClipEffect->AddCharacter(oi.m_obj, oi.m_mx, oi.m_cx);
				}

				break;
			}

			// continue to default
		}

		case gobjSprite:
		{
			if (bBreakApart == VARIANT_TRUE)
			{
				gldSprite *pSprite = (gldSprite *)m_obj;
				if (CSWFProxy::GObjIsValidGroup(pSprite))
				{
					gldLayer *pLayer = pSprite->m_mc->m_layerList[0];

					if (pLayer->m_frameClipList.size() > 0)
					{
						GCHARACTERKEY_LIST &ckList = (*pLayer->m_frameClipList.begin())->m_characterKeyList;
						for (GCHARACTERKEY_LIST::iterator it = ckList.begin(); it != ckList.end(); it++)
						{
							gldCharacterKey *ck = *it;
							gldCharacter *c = ck->GetCharacter(0);
							pFrameClipEffect->AddCharacter(ck->GetObj(), c->m_matrix, c->m_cxform * m_cxform, 0);
						}

						break;
					}					
				}
			}
			// continue to default
		}

		default:
			pFrameClipEffect->AddCharacter(m_obj, gldMatrix(), m_cxform, 0);
			break;
	}

	HRESULT hr;

	CComPtr<IGLD_Sprite> pISprite;
	hr = pISprite.CoCreateInstance(__uuidof(GLD_Sprite));
	if (FAILED(hr))
		return FALSE;

	CComPtr<IGLD_MovieClip> pIMovieClip;
	hr = pISprite->get_MovieClip(&pIMovieClip);
	if (FAILED(hr))
		return FALSE;

	CC2IMap C2IMap;
	CC2IMap *oldC2IMap = CC2I::m_pC2IMap;
	CC2I::m_pC2IMap = &C2IMap;
	hr = CC2I::Convert(pMovieClip, pIMovieClip);
	CC2I::m_pC2IMap = oldC2IMap;
	
	if (FAILED(hr))
		return FALSE;

	// Initialize extra parameters
	CComPtr<IGLD_Parameters> pIParasAll;
	pIParasAll.CoCreateInstance(__uuidof(GLD_Parameters));
	if (pIParasAll == NULL)
		return FALSE;

	PutParameter(pIParasAll, "Movie.Width", _GetMainMovie2()->m_width);
	PutParameter(pIParasAll, "Movie.Height", _GetMainMovie2()->m_height);

	CComPtr<IGLD_Matrix> pIInstMatrix;
	pIInstMatrix.CoCreateInstance(__uuidof(GLD_Matrix));
	if (pIInstMatrix == NULL)
		return FALSE;
	pIInstMatrix->SetData(m_matrix.m_e11, m_matrix.m_e12, m_matrix.m_e21, m_matrix.m_e22, m_matrix.m_x, m_matrix.m_y);
	PutParameter(pIParasAll, "MovieClip.Matrix", pIInstMatrix);

	CComPtr<IGLD_Cxform> pIInstCxform;
	pIInstCxform.CoCreateInstance(__uuidof(GLD_Cxform));
	if (pIInstCxform == NULL)
		return FALSE;
	pIInstCxform->SetData(m_cxform.ra, m_cxform.rb, m_cxform.ga, m_cxform.gb, m_cxform.ba, m_cxform.bb, m_cxform.aa, m_cxform.ab);
	PutParameter(pIParasAll, "MovieClip.Cxform", pIInstCxform);

	for (GEFFECT_LIST::iterator it = m_effectList.begin(); it != m_effectList.end(); it++)
	{
		gldEffect *pEffect = *it;

		if (pEffect->m_pIEffect != NULL)
		{
			// determine if padding frames is needed.

			long nMovieClipLength = 0;
			hr = pIMovieClip->get_MaxTime(&nMovieClipLength);
			CheckResult();

			ASSERT(nMovieClipLength > 0);	// must have frame clips
			int nPadFrame = pEffect->m_startTime - (nMovieClipLength - 1);
			if (nMovieClipLength > 0 && nPadFrame > 0)	// need to pad
			{
				CComPtr<IGLD_Layers> pILayers;
				hr = pIMovieClip->get_Layers(&pILayers);
				CheckResult();

				long count;
				pILayers->get_Count(&count);
				ASSERT(count > 0);
				if (count > 0)
				{
					CComPtr<IEnumVARIANT> pIEnumLayers;
					hr = pILayers->get__NewEnum(&pIEnumLayers);
					CheckResult();
					CComVariant varLayer;
					while (pIEnumLayers->Next(1, &varLayer, NULL) == S_OK)
					{
						CComQIPtr<IGLD_Layer> pILayer = varLayer.punkVal;
						if (pILayer == NULL)
						{
							hr = E_UNEXPECTED;
							CheckResult();
						}
						varLayer.Clear();

						CComPtr<IGLD_FrameClips> pIFrameClips;
						hr = pILayer->get_FrameClips(&pIFrameClips);
						CheckResult();

						CComPtr<IGLD_FrameClip> pIFrameClip;
						hr = pIFrameClips->get_Item(nMovieClipLength - 1, &pIFrameClip);
						if (SUCCEEDED(hr) && pIFrameClip)
						{
							CComPtr<IGLD_CharacterKeys> pICharacterKeys;
							hr = pIFrameClip->get_CharacterKeys(&pICharacterKeys);
							if (SUCCEEDED(hr))
							{
								for (int i = 0; i < nPadFrame; i++)
								{
									CComPtr<IGLD_FrameClip> pIFrameClipDup;
									hr = pIFrameClips->Append(1, gceStaticFrameClip, &pIFrameClipDup);

									CComPtr<IGLD_CharacterKeys> pICharacterKeysDup;
									hr = pIFrameClipDup->get_CharacterKeys(&pICharacterKeysDup);
									CheckResult();

									CComPtr<IEnumVARIANT> pIEnumCharacterKeys;
									hr = pICharacterKeys->get__NewEnum(&pIEnumCharacterKeys);
									if (SUCCEEDED(hr))
									{
										CComVariant varCharacterKey;
										while (pIEnumCharacterKeys->Next(1, &varCharacterKey, NULL) == S_OK)
										{
											CComQIPtr<IGLD_CharacterKey> pICharacterKey = varCharacterKey.punkVal;
											if (pICharacterKey == NULL)
												hr = E_UNEXPECTED;
											CheckResult();

											varCharacterKey.Clear();

											CComPtr<IGLD_Object> pIObject;
											hr = pICharacterKey->get_Object(&pIObject);
											CheckResult();

											CComPtr<IGLD_Matrix> pIMatrix;
											hr = pICharacterKey->get_Matrix(&pIMatrix);
											CheckResult();

											CComPtr<IGLD_Cxform> pICxform;
											hr = pICharacterKey->get_Cxform(&pICxform);
											CheckResult();

											hr = pICharacterKeysDup->Append(pIObject, pIMatrix, pICxform, NULL);
											CheckResult();
										}
									}
								}
							}
						}
					}
				}
			}

			hr = pEffect->m_pIEffect->Build(pIMovieClip, pEffect->m_startTime
				, pEffect->m_length, pEffect->m_pIParas, pIParasAll);

			if (FAILED(hr))
			{
				if (method == IS_PROMPT)
				{
					CComQIPtr<ISupportErrorInfo> pISEI = pEffect->m_pIEffect;
					CCoException e(pISEI, __uuidof(IGLD_Effect), hr);
					CString strErrorInfo;
					e.GetErrorMessage(strErrorInfo);
					if (AfxMessageBoxEx(MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2, 
						"Failed to build effect \"%s\" for instance \"%s\": %s\r\nIgnore this error and continue anyway?", 
						pEffect->GetName().c_str(), 
						m_name.c_str(), 
						strErrorInfo) != IDYES)
						return FALSE;
				}
				else if (method == IS_RETURN)
				{
					return FALSE;
				}

			}
		}
		else
		{
			if (method == IS_PROMPT)
			{
				if (AfxMessageBoxEx(MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2, 
					"Failed to build effect \"%s\" for instance \"%s\". The effect does not exist on your system or it is not registered correctly. Ignore this error and continue anyway?", 
					pEffect->GetName().c_str(), 
					m_name.c_str()) != IDYES)
					return FALSE;
			}
			else if (method == IS_RETURN)
			{
				return FALSE;
			}
		}
	}

	m_mc = new gldMovieClip();
	
	CI2CMap I2CMap;
	C2IMap.CopyTo(&I2CMap);
	CI2CMap *pOldI2CMap = CI2C::m_pI2CMap;
	CI2C::m_pI2CMap = &I2CMap;
	hr = CI2C::Convert(pIMovieClip, m_mc);
	CI2C::m_pI2CMap = pOldI2CMap;

	if (FAILED(hr))
		return FALSE;
	
	I2CMap.CopyTo(_GetObjectMap());

	BOOL bAutoPlay = TRUE;
	GetParameter(m_pIParas, EP_AUTO_PLAY, &bAutoPlay);
	if (!bAutoPlay)
	{
		gldLayer *_actionLayer = new gldLayer("Action", 0);
		gldFrameClip *_actionFC = new gldFrameClip(0, 1);
		_actionFC->m_action = new gldAction();
		_actionFC->m_action->m_type = actionSegFrame;
		_actionFC->m_action->SetSourceCode(
			"if(firstRun == undefined)"
			"{"
				"stop();"
				"firstRun = false;"
			"}"
			);
		_actionLayer->m_frameClipList.push_back(_actionFC);

		m_mc->AddLayer(_actionLayer);
	}

	BOOL bLoop = TRUE;
	GetParameter(m_pIParas, EP_LOOP, &bLoop);
	if (!bLoop)
	{
		gldLayer *_actionLayer = new gldLayer("Action", 0);
		if (nMaxTime > 1)
		{
			gldFrameClip *_emptyFC = new gldFrameClip(0, nMaxTime - 1);
			_actionLayer->m_frameClipList.push_back(_emptyFC);

		}
		gldFrameClip *_actionFC = new gldFrameClip(nMaxTime - 1, 1);
		_actionFC->m_action = new gldAction();
		_actionFC->m_action->m_type = actionSegFrame;
		_actionFC->m_action->SetSourceCode("stop();");
		_actionLayer->m_frameClipList.push_back(_actionFC);

		m_mc->AddLayer(_actionLayer);
	}

	return TRUE;
}

int gldInstance::GetMaxTime() const
{
	int nMaxTime = 1;
	for (GEFFECT_LIST::const_iterator it = m_effectList.begin(); it != m_effectList.end(); it++)
	{
		gldEffect *pEffect = *it;
		nMaxTime = max(nMaxTime, pEffect->m_startTime + pEffect->m_length);
	}
	return nMaxTime;
}

gldEffect *gldInstance::GetEffect(int index)
{
	ASSERT(index >= 0);
	if (index >= 0)
	{
		int count = 0;
		for (GEFFECT_LIST::iterator it = m_effectList.begin(); it != m_effectList.end(); it++, count++)
			if (count == index)
				return *it;
	}

	return NULL;
}

gldInstance *gldInstance::Clone() const
{
	gldInstance *pInst = new gldInstance(m_obj, m_matrix, m_cxform);

	pInst->Copy(this);

	return pInst;
}

void gldInstance::Copy(const gldInstance *pSrc)
{
	ASSERT(pSrc != NULL);
	if (pSrc == this)
		return;

	m_name = pSrc->m_name;
	m_obj = pSrc->m_obj;
	m_matrix = pSrc->m_matrix;
	m_cxform = pSrc->m_cxform;
	m_tx = pSrc->m_tx;
	m_ty = pSrc->m_ty;
	m_ptr;
	m_mc;
	GEFFECT_LIST::iterator it = m_effectList.begin();
	for (; it != m_effectList.end(); ++it)
	{
		delete *it;
	}
	m_effectList.clear();
	GEFFECT_LIST::const_iterator cit = pSrc->m_effectList.begin();
	for (; cit != pSrc->m_effectList.end(); ++cit)
	{
		m_effectList.push_back((*cit)->Clone());
	}
	m_pIParas.Release();
	VERIFY(SUCCEEDED(m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters))));
	CComPtr<IEnumVARIANT> enumVar;
	VERIFY(SUCCEEDED(pSrc->m_pIParas->get__NewEnum(&enumVar)));
	CComVariant varPara;
	while (enumVar->Next(1, &varPara, NULL) == S_OK)
	{
		CComQIPtr<IGLD_Parameter> pIPara = varPara.punkVal;
		ASSERT(pIPara != NULL);
		CComBSTR bstrName;
		CComVariant value;
		VERIFY(SUCCEEDED(pIPara->get_Name(&bstrName)));
		VERIFY(SUCCEEDED(pIPara->get_Value(&value)));
		VERIFY(SUCCEEDED(m_pIParas->put_Item(bstrName, value)));
	}
	m_action = pSrc->m_action;
	m_cxStyle = pSrc->m_cxStyle;
	m_tint = pSrc->m_tint;
	m_mask = pSrc->m_mask;
	m_expanded = pSrc->m_expanded;
}

HRESULT gldInstance::WriteToStream(IStream *pStm, void *pUnk)
{
	// swf library data
	oBinStream os;
	os << (WORD)1; // version
	os << m_name;
	os << m_obj->m_id;
	m_matrix.WriteToBinStream(os);
	m_cxform.WriteToBinStream(os);
	os << m_tx;
	os << m_ty;
	os << m_action;
	U8 cxStyle = m_cxStyle;
	os << cxStyle;
	os << m_tint;
	os << m_mask;
	os << m_expanded;
	HRESULT hr = ::WriteToStream(os, pStm);
	CHECKRESULT();
	
	// effect list
	IStreamWrapper stm(pStm);
	DWORD count = (DWORD)m_effectList.size();
	hr = stm.Write(count);
	CHECKRESULT();
	GEFFECT_LIST_IT ite = m_effectList.begin();
	for (; ite != m_effectList.end(); ++ite)
	{
		hr = (*ite)->WriteToStream(pStm, pUnk);
		CHECKRESULT();
	}

	// common parameters
	hr = stm.Write(m_pIParas.p, pUnk);
	CHECKRESULT();
	
	return S_OK;
}

HRESULT gldInstance::ReadFromStream(IStream *pStm, void *pUnk)
{	
	ASSERT(pStm != NULL && pUnk != NULL);
	ASSERT(m_ptr == NULL);

	HRESULT hr;

	CReplaceLibrary xRplLib((gldLibrary *)pUnk);

	// swf library data
	iBinStream is;
	hr = ::ReadFromStream(pStm, is);
	CHECKRESULT();
	WORD ver;
	is >> ver;
	if (ver == 1)
	{
		U16 idObj = (U16)-1;

		is >> m_name;	
		is >> idObj;
		m_matrix.ReadFromBinStream(is);
		m_cxform.ReadFromBinStream(is);
		is >> m_tx;
		is >> m_ty;
		is >> m_action;
		U8 cxStyle;
		is >> cxStyle;
		m_cxStyle = (COLOR_TRANS_STYLE)cxStyle;
		is >> m_tint;
		is >> m_mask;
		is >> m_expanded;
		
		// effect list
		IStreamWrapper stm(pStm);
		DWORD count = 0;
		hr = stm.Read(count);
		CHECKRESULT();		
		for (; count > 0; count--)
		{
			gldEffect *pEffect = new gldEffect;
			hr = pEffect->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pEffect;
				return hr;
			}
			else
			{
				m_effectList.push_back(pEffect);
			}
		}

		// common parameters
		m_pIParas.Release();
		hr = m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters));
		CHECKRESULT();
		hr = stm.Read(m_pIParas.p, pUnk);
		CHECKRESULT();

		// prepare for other auxilary data
		gldLibrary *pLib = (gldLibrary *)pUnk;
		gldObj *pObj = pLib->FindObjByID(idObj);
		if (pObj == NULL)
			return E_FAIL;
		m_obj = pObj;
	}
	else
	{
		hr = E_FAIL;
	}
	
	return hr;
}

HRESULT gldInstance::SaveToFile(LPCTSTR pszPath)
{
	ASSERT(pszPath != NULL);
	ASSERT(m_obj != NULL);

	const DWORD STGM_RWE = STGM_READ | STGM_WRITE | STGM_SHARE_EXCLUSIVE;

	CComPtr<IGLD_Object> pIObject;
	HRESULT hr;
	CC2IMap C2IMap;
	CC2IMap *pOldMap = CC2I::m_pC2IMap;
	CC2I::m_pC2IMap = &C2IMap;
	hr = CC2I::Create(m_obj, &pIObject);
	CC2I::m_pC2IMap = pOldMap;
	CHECKRESULT();
	CObjectAttachedInfo trans;
	trans.SetTransform(m_matrix, m_cxform);	
	hr = ::SaveObjectToFile(pIObject, pszPath, trans);
	CHECKRESULT();
	USES_CONVERSION;
	CComPtr<IStorage> pIFile;
	hr = StgOpenStorage(CT2OLE(pszPath), NULL, STGM_RWE, NULL, 0, &pIFile);
	CHECKRESULT();
	IStreamWrapper stmElm;
	hr = pIFile->CreateStream(L"Elm", STGM_RWE, 0, 0, &stmElm);
	CHECKRESULT();
	WORD ver = 1;
	hr = stmElm.Write(ver);
	CHECKRESULT();
	oBinStream os;
	os << m_name;
	os << m_tx;
	os << m_ty;
	os << m_action;
	U8 cxStyle = m_cxStyle;
	os << cxStyle;
	os << m_tint;
	os << m_mask;
	os << m_expanded;
	hr = ::WriteToStream(os, stmElm);
	CHECKRESULT();
	// effect list
	DWORD count = (DWORD)m_effectList.size();
	hr = stmElm.Write(count);
	CHECKRESULT();
	GEFFECT_LIST_IT ite = m_effectList.begin();
	for (; ite != m_effectList.end(); ++ite)
	{
		hr = (*ite)->WriteToStream(stmElm, NULL);
		CHECKRESULT();
	}
	// common parameters
	hr = stmElm.Write(m_pIParas.p, NULL);
	CHECKRESULT();

	return S_OK;
}

#define CHECK_THROW_HR(hr) if (FAILED(hr)) throw hr

HRESULT gldInstance::LoadFromFile(LPCTSTR pszPath, GOBJECT_LIST &lstObj)
{	
	ASSERT(pszPath != NULL);
	ASSERT(m_obj == NULL);

	const DWORD STGM_RE = STGM_READ | STGM_SHARE_EXCLUSIVE;

	CI2CMap I2CMap;

	try
	{
		HRESULT hr = S_OK;
		CComPtr<IGLD_Object> pIObject;
		CObjectAttachedInfo info;
		hr = LoadObjectFromFile(pszPath, &pIObject, info);
		CHECK_THROW_HR(hr);

		bool bInitOK = true;

		try
		{
			USES_CONVERSION;
			CComPtr<IStorage> pIFile;
			hr = StgOpenStorage(CT2OLE(pszPath), NULL, STGM_RE, NULL, 0, &pIFile);
			CHECK_THROW_HR(hr);
			CObjectAttachedInfo trans;
			trans.SetMask(CObjectAttachedInfo::IF_ALL);
			hr = trans.LoadFromStorage(pIFile);
			CHECK_THROW_HR(hr);
			m_matrix = trans.GetMatrix();
			m_cxform = trans.GetCxform();
			IStreamWrapper stmElm;
			hr = pIFile->OpenStream(L"Elm", NULL, STGM_RE, 0, &stmElm);
			CHECK_THROW_HR(hr);
			WORD ver = 1;
			hr = stmElm.Read(ver);
			CHECK_THROW_HR(hr);
			if (ver != 1) throw E_UNEXPECTED;
			iBinStream is;
			hr = ::ReadFromStream(stmElm, is);
			CHECK_THROW_HR(hr);
			is >> m_name;			
			is >> m_tx;
			is >> m_ty;
			is >> m_action;
			U8 cxStyle = m_cxStyle;
			is >> cxStyle;
			is >> m_tint;
			is >> m_mask;
			is >> m_expanded;
			// effect list
			DWORD count = 0;
			hr = stmElm.Read(count);
			CHECK_THROW_HR(hr);			
			for (DWORD i = 0; i < count; i++)
			{
				gldEffect *pEffect = new gldEffect;
				hr = pEffect->ReadFromStream(stmElm, NULL);
				CHECK_THROW_HR(hr);
				m_effectList.push_back(pEffect);
			}
			// common parameters
			hr = stmElm.Read(m_pIParas.p, NULL);
			CHECK_THROW_HR(hr);
		}
		catch (HRESULT hr)
		{
			UNUSED_ALWAYS(hr);

			bInitOK = false;
		}

		GLD_ObjectType type;
		hr = pIObject->get_Type(&type);
		CHECK_THROW_HR(hr);

		if (type != gceShape && type != gceButton && type != gceSprite)
			throw E_UNEXPECTED;

		// Adjust center point if the file saved element information must not
		if (!bInitOK)
		{
			if (type == gceButton)
			{
				CComQIPtr<IGLD_Button> pIButton = pIObject;
				if (pIButton == NULL)
					throw E_NOINTERFACE;

				hr = pIButton->Normalize();
				CHECK_THROW_HR(hr);
			}
			else if (type == gceSprite)
			{
				CComQIPtr<IGLD_Sprite> pISprite = pIObject;
				if (pISprite == NULL)
					throw E_NOINTERFACE;

				hr = pISprite->Normalize();
				CHECK_THROW_HR(hr);
			}
			else if (type == gceShape)
			{
				CComQIPtr<IGLD_Shape> pIShape = pIObject;
				if (pIShape == NULL)
					throw E_NOINTERFACE;

				hr = pIShape->Normalize();
				CHECK_THROW_HR(hr);
			}
		}

		CI2CMap *pTmp = CI2C::m_pI2CMap;
		CI2C::m_pI2CMap = &I2CMap;
		hr = CI2C::Create(pIObject, &m_obj);
		CI2C::m_pI2CMap = pTmp;
		CHECK_THROW_HR(hr);
	}
	catch (HRESULT hr)
	{		
		I2CMap.FreeAllSymbol();		
		return hr;
	}

	for (CI2CMap::_I2CMap::iterator i = I2CMap.m_Map.begin()
		; i != I2CMap.m_Map.end(); ++i)
	{
		lstObj.push_back((*i).second);
	}

	return S_OK;
}

int gldInstance::GetMinTime() const
{
	if (m_effectList.size() == 0)
	{
		return 0;
	}
	else
	{
		gldEffect *pEffect = *m_effectList.begin();
		int nMinTime = pEffect->m_startTime;
		for (GEFFECT_LIST::const_iterator it = ++m_effectList.begin()
			; it != m_effectList.end(); ++it)
		{
			pEffect = *it;
			nMinTime = min(nMinTime, pEffect->m_startTime);
		}
		return nMinTime;
	}	
}

int gldInstance::GetLength() const
{
	return GetMaxTime() - GetMinTime();
}

int gldInstance::GetEffectKeyCount()
{
	int n = 0;
	VARIANT_BOOL bBreakApart = VARIANT_TRUE;
	if (m_pIParas != NULL)
	{
		GetParameter(m_pIParas, EP_BREAK_APART, &bBreakApart);
	}

	if (bBreakApart)
	{
		gldObj *pObj = m_obj;
		ASSERT(pObj != NULL);
		if (pObj->IsGObjInstanceOf(gobjText)) // is text
		{
			gldText2 *pText = (gldText2 *)pObj;
			Paragraphs::iterator itp = pText->m_ParagraphList.begin();
			for (; itp != pText->m_ParagraphList.end(); ++itp)
			{
				CParagraph *pp = *itp;
				TextBlocks::iterator itt = pp->m_TextBlockList.begin();
				for (;itt != pp->m_TextBlockList.end(); ++itt)
				{
					CTextBlock *pt = *itt;
					const wchar_t *pc = pt->strText.c_str();
					while (*pc != L'\0')
					{
						if (*pc != L' ' 
							&& *pc != L'\t'
							&& *pc != L'\n'
							&& *pc != L'\r')
						{
							n++;
						}
						pc++;
					}
				}
			}
		}
		else if (CSWFProxy::GObjIsValidGroup(pObj)) // is group
		{			
			gldSprite *pGrp = (gldSprite *)pObj;
			for (GLAYER_LIST_IT itl = pGrp->m_mc->m_layerList.begin();
				itl != pGrp->m_mc->m_layerList.end(); ++itl)
			{
				gldLayer *pl = *itl;
				for (GFRAMECLIP_LIST_IT itf = pl->m_frameClipList.begin();
					itf != pl->m_frameClipList.end(); ++itf)
				{
					gldFrameClip *pfc = *itf;
					n += (int)pfc->m_characterKeyList.size();
				}
			}		
		}
	}

	return __max(n, 1);	
}

BOOL gldInstance::SaveAsTemplate() const
{
	return m_effectList.size() > 0 && GetLength() > 0;
}

const char gldInstance::s_TemplateFlag[] = "ComboEffect";

HRESULT gldInstance::SaveAsTemplate(LPCTSTR pszPath, LPCTSTR name)
{
	UNUSED_ALWAYS(name);

	const DWORD STGM_RWEC = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE;

	if (!SaveAsTemplate())
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	// Create the document file
	CComPtr<IStorage> pIFile;
	hr = StgCreateDocfile(CT2OLE(pszPath), STGM_RWEC, 0, &pIFile);
	CHECKRESULT();

	// Write information
	IStreamWrapper stmInfo;
	hr = pIFile->CreateStream(L"info", STGM_RWEC, 0, 0, &stmInfo);
	CHECKRESULT();
	hr = stmInfo.WriteBlock(s_TemplateFlag, sizeof(s_TemplateFlag)); // Flag
	CHECKRESULT();
	hr = stmInfo.Write((WORD)1); // version
	CHECKRESULT();	
	hr = stmInfo.Write((DWORD)GetLength()); // length
	CHECKRESULT();
	hr = stmInfo.Write((DWORD)GetEffectKeyCount()); // key count
	CHECKRESULT();
	hr = stmInfo.Write(m_pIParas.p, NULL);	// common parameters
	CHECKRESULT();
	hr = stmInfo.Write((DWORD)m_effectList.size()); // size of effect
	CHECKRESULT();
	gldEffect *pEffect = *m_effectList.begin();
	int minTime = GetMinTime();
	for (GEFFECT_LIST::const_iterator ite = m_effectList.begin()
		; ite != m_effectList.end(); ++ite) // effect list
	{
		pEffect = *ite;
		CLSID id = pEffect->m_clsid;
		hr = stmInfo.Write(id);
		CHECKRESULT();
		hr = stmInfo.Write((DWORD)(pEffect->m_startTime - minTime));
		CHECKRESULT();
		hr = stmInfo.Write((DWORD)pEffect->m_length);
		CHECKRESULT();
		hr = stmInfo.Write(pEffect->m_pIParas.p, NULL);
		CHECKRESULT();
	}

	// Write demo swf file
	IStreamWrapper stmDemo;
	hr = pIFile->CreateStream(L"demo", STGM_RWEC, 0, 0, &stmDemo);
	CHECKRESULT();
	if (!BuildEffects(IS_PROMPT))
		return E_FAIL;
	CMoviePreview movie;
	movie.SetMovieClip(m_mc, gldMatrix(), gldCxform(), FALSE, TRUE);
	CString strDemo = GetTempFile(NULL, "~Template", "swf");
	if (!movie.CreatePreview(strDemo, _GetMainMovie2()->m_color, FALSE))
		return E_FAIL;
	try
	{
		CStdioFile swfFile(strDemo, CFile::typeBinary | CFile::modeRead);
		UINT size = (UINT)swfFile.GetLength();
		CAutoPtr<U8> data(new U8[size]);
		swfFile.Read(data.m_p, size);
		swfFile.Close();
		DeleteFile(strDemo);
		stmDemo.Write((DWORD)size);
		stmDemo.WriteBlock(data.m_p, size);
	}
	catch (CFileException *e)
	{
		e->Delete();
		return E_FAIL;
	}

	return hr;
}

template <typename TCmd>
class CCmdAutoRollback
{
public:
	CCmdAutoRollback(TCmd *pCmd)
		: m_pCmd(pCmd)
	{
	}

	virtual ~CCmdAutoRollback()
	{
		if (m_pCmd != NULL)
		{
			if (m_pCmd->Done())
			{
				m_pCmd->Unexecute();
			}			
			delete m_pCmd;
		}
	}

	TCmd *operator->()
	{
		return m_pCmd;
	}

	TCmd *Detach()
	{
		TCmd *ret = m_pCmd;
		m_pCmd = NULL;
		return ret;
	}

private:
	TCmd *m_pCmd;
};

HRESULT gldInstance::ApplyTemplate(LPCTSTR pszPath)
{
	const DWORD STGM_RE = STGM_READ | STGM_SHARE_EXCLUSIVE;

	CCmdAutoRollback<TCommandGroup> cmd(new TCommandGroup(IDS_CMD_APPLYTEMPLATE));

	HRESULT hr = S_OK;
	// Open the document file
	CComPtr<IStorage> pIFile;
	hr = StgOpenStorage(CT2OLE(pszPath), NULL, STGM_RE, NULL, 0, &pIFile);
	CHECKRESULT();

	// Get information
	IStreamWrapper stmInfo;
	hr = pIFile->OpenStream(L"info", NULL, STGM_RE, 0, &stmInfo);
	CHECKRESULT();
	CAutoPtr<char> flag(new char[sizeof(s_TemplateFlag)]);
	hr = stmInfo.ReadBlock(flag.m_p, sizeof(s_TemplateFlag)); // Flag	
	CHECKRESULT();
	if (strncmp(s_TemplateFlag, flag.m_p, sizeof(s_TemplateFlag)) != 0)
	{
		return E_UNEXPECTED;
	}
	WORD ver = 0;
	hr = stmInfo.Read(ver); // version
	CHECKRESULT();
	if (ver != 1)
	{
		return E_UNEXPECTED;
	}	
	DWORD dwLen = 1;
	hr = stmInfo.Read(dwLen); // length
	CHECKRESULT();
	DWORD cKeys = 0;
	hr = stmInfo.Read(cKeys); // key count
	CHECKRESULT();
	CComPtr<IGLD_Parameters> pIParas;
	hr = pIParas.CoCreateInstance(__uuidof(GLD_Parameters));
	CHECKRESULT();
	hr = stmInfo.Read(pIParas.p, NULL);	// common parameters
	CHECKRESULT();
	cmd->Do(new CCmdChangeEffectParameters(m_pIParas, pIParas));
	DWORD dwCount = 0;
	hr = stmInfo.Read(dwCount); // size of effect
	CHECKRESULT();
	double ratio = (double)GetEffectKeyCount() / cKeys;
	int nTimeOffset = GetMaxTime();	
	if (m_effectList.size() == 0) // because empty effect will return 1, the detail please refrence GetMaxTime() function.
		nTimeOffset = 0;
	for (DWORD i = 0; i < dwCount; i++) // effect list
	{
		CLSID id;
		hr = stmInfo.Read(id);
		CHECKRESULT();
		CAutoPtr<gldEffect> pEffect(new gldEffect(id));
		if (pEffect->m_pIParas == NULL || pEffect->m_pIEffect == NULL)
		{
			return E_FAIL;
		}
		DWORD nStartTime = 0;
		hr = stmInfo.Read(nStartTime);
		CHECKRESULT();
		DWORD nEffectLen = 0;
		hr = stmInfo.Read(nEffectLen);
		CHECKRESULT();
		hr = stmInfo.Read(pEffect->m_pIParas.p, NULL);
		CHECKRESULT();
		pEffect->m_length = __max(1, (int)(ratio * nEffectLen));
		pEffect->m_startTime = (int)(ratio * nStartTime) + nTimeOffset;
		cmd->Do(new CCmdInsertEffect(this, pEffect.Detach(), NULL));
	}

	my_app.Commands().Do(cmd.Detach());

	return S_OK;
}

HRESULT gldInstance::GetTemplateName(LPCTSTR pszTemplPath, CString &strTemplName)
{
	strTemplName = FileStripPath(pszTemplPath);
	int nDot = strTemplName.ReverseFind(_T('.'));
	if (nDot != -1)
	{
		strTemplName = strTemplName.Left(nDot);
	}
	return S_OK;
	/*const DWORD STGM_RE = STGM_READ | STGM_SHARE_EXCLUSIVE;

	HRESULT hr = S_OK;
	// Open the document file
	CComPtr<IStorage> pIFile;
	hr = StgOpenStorage(CT2OLE(pszTemplPath), NULL, STGM_RE, NULL, 0, &pIFile);
	CHECKRESULT();

	// Get information
	IStreamWrapper stmInfo;
	hr = pIFile->OpenStream(L"info", NULL, STGM_RE, 0, &stmInfo);
	CHECKRESULT();
	CAutoPtr<char> flag(new char[sizeof(s_TemplateFlag)]);
	hr = stmInfo.ReadBlock(flag.m_p, sizeof(s_TemplateFlag)); // Flag	
	CHECKRESULT();
	if (strncmp(s_TemplateFlag, flag.m_p, sizeof(s_TemplateFlag)) != 0)
	{
		return E_UNEXPECTED;
	}
	WORD ver = 0;
	hr = stmInfo.Read(ver); // version
	CHECKRESULT();
	if (ver != 1)
	{
		return E_UNEXPECTED;
	}
	CComBSTR bstrName;	// name
	hr = bstrName.ReadFromStream(stmInfo);
	CHECKRESULT();
	
	strTemplName = bstrName;*/

	return S_OK;
}

HRESULT gldInstance::ExtractDemo(LPCTSTR pszTemplPath, LPCTSTR pszDemoPath)
{
	const DWORD STGM_RE = STGM_READ | STGM_SHARE_EXCLUSIVE;

	HRESULT hr = S_OK;
	// Open the document file
	CComPtr<IStorage> pIFile;
	hr = StgOpenStorage(CT2OLE(pszTemplPath), NULL, STGM_RE, NULL, 0, &pIFile);
	CHECKRESULT();

	// Get Demo
	IStreamWrapper stmDemo;
	hr = pIFile->OpenStream(L"demo", NULL, STGM_RE, 0, &stmDemo);
	CHECKRESULT();	
	DWORD dwLen = 0;
	hr = stmDemo.Read(dwLen);
	CHECKRESULT();
	CAutoPtr<U8> buf(new U8[dwLen]);
	hr = stmDemo.ReadBlock(buf.m_p, dwLen);
	CHECKRESULT();
	try
	{
		CStdioFile file(pszDemoPath, CFile::modeCreate | CFile::typeBinary | CFile::modeWrite);
		file.Write(buf.m_p, (UINT)dwLen);
		file.Close();
	}
	catch(...)
	{
		return E_FAIL;
	}

	return S_OK;
}