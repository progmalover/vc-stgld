#include "StdAfx.h"
#include "gldeffect.h"
#include "gldEffectManager.h"
#include "GlandaDoc.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "glandaclipboard.h"
#include "my_app.h"
#include "Resource.h"
#include "GLD_PropertySheet.h"
#include "EffectCommonParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

gldEffectInfo::gldEffectInfo()
{
	m_init = FALSE;
	m_type = typeEmphasize;
	m_iconIndex = -1;
}

BOOL gldEffectInfo::Init(REFIID clsid)
{
	ASSERT(!m_init);

	m_clsid = clsid;

	m_autoShowConfig = false;


	CComPtr<IGLD_Effect> pIEffect;
	if (FAILED(pIEffect.CoCreateInstance(m_clsid)))
		return FALSE;

	CComQIPtr<IGLD_EffectInfo> pIInfo(pIEffect);
	if (pIInfo != NULL)
	{
		USES_CONVERSION;

		// name
		CComBSTR bstrName;
		if (SUCCEEDED(pIInfo->get_Name(&bstrName)))
			m_name = OLE2A(bstrName);

		// description
		CComBSTR bstrDesciption;
		if (SUCCEEDED(pIInfo->get_Description(&bstrDesciption)))
			m_description = OLE2A(bstrDesciption);

		// auto show configure window
		long autoShowConfig;
		if (SUCCEEDED(pIInfo->get_AutoShowConfigWindow(&autoShowConfig)))
			m_autoShowConfig = (autoShowConfig != 0);

		// resource module
		CComBSTR bstrModule;
		if (SUCCEEDED(pIInfo->get_ResourceModuleFile(&bstrModule)))
		{
			m_resourceModule = OLE2A(bstrModule);

			// Bitmap ID
			long id;
			if (SUCCEEDED(pIInfo->get_Bitmap(&id)))
			{
				m_bitmapID = (UINT)id;
				m_bitmap.Attach(GetBitmap());
			}
		}
	}

	m_init = TRUE;
	return TRUE;
}

gldEffectInfo::~gldEffectInfo()
{
}

HBITMAP gldEffectInfo::GetBitmap()
{
	HBITMAP hBitmap = NULL;
	HMODULE hModule = ::LoadLibrary(m_resourceModule.c_str());
	if (hModule)
	{
		hBitmap = ::LoadBitmap(hModule, MAKEINTRESOURCE(m_bitmapID));
		::FreeLibrary(hModule);
	}
	return hBitmap;
}

UINT gldEffectInfo::BrowseForProperties(HWND parent, int x, int y, const CString &caption, int nPages, const CLSID *pPages, int nActivate, LPUNKNOWN props)
{
	long nMyPages = 0;
	GUID *pMyPages = NULL;
	CComPtr<IGLD_Effect> pIEffect;
	HRESULT hr = pIEffect.CoCreateInstance(m_clsid);
	if (SUCCEEDED(hr))
	{
		pIEffect->GetPropertyPages(&pMyPages, &nMyPages);
	}
	if (nPages + nMyPages == 0)
	{
		return IDOK;
	}
	CAutoPtr<GUID> guid(new GUID[nPages + nMyPages]);
	for (int i = 0; i < nPages; i++)
	{
		guid.m_p[i] = pPages[i];
	}
	if (pMyPages != NULL)
	{
		memcpy(guid.m_p + nPages, pMyPages, nMyPages * sizeof(GUID));
		CoTaskMemFree(pMyPages);
	}

	CGLD_PropertySheet dlg(CWnd::FromHandle(parent), x, y, caption, props, nPages + nMyPages, guid.m_p);
	dlg.m_nActiveInitial = (nActivate < 0 ? nPages : nActivate);
	return dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

gldEffect::gldEffect()
{
	memset(&m_clsid, 0, sizeof(m_clsid));
	m_startTime = 0;
	m_pEffectInfo = NULL;
	m_length = 0;
}

gldEffect::gldEffect(REFIID clsid)
{
	m_clsid = clsid;
	m_startTime = 0;
	m_length = 0;
	m_pEffectInfo = NULL;

	VERIFY(SUCCEEDED(m_pIEffect.CoCreateInstance(m_clsid)));
	VERIFY(SUCCEEDED(m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters))));

	m_pEffectInfo = gldEffectManager::Instance()->FindEffect(clsid);
	ASSERT(m_pEffectInfo);
}

void gldEffect::ChangeTo(REFIID clsid, IGLD_Parameters *paras)
{
	m_clsid = clsid;
	m_pEffectInfo = NULL;

	if (m_pIEffect != NULL)
		m_pIEffect.Release();	
	VERIFY(SUCCEEDED(m_pIEffect.CoCreateInstance(m_clsid)));
	if (m_pIParas != NULL)
		m_pIParas.Release();
	VERIFY(SUCCEEDED(m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters))));
	if (paras != NULL)
		CopyParameters(paras, m_pIParas);
	m_pEffectInfo = gldEffectManager::Instance()->FindEffect(clsid);
	ASSERT(m_pEffectInfo);
}

void gldEffect::CalcIdealLength(int nTotalCharacterKeys)
{
	m_length = 24;

	CComQIPtr<IGLD_EffectInfo> pIInfo(m_pIEffect);
	if (pIInfo)
	{
		long frames = 0;
		if (SUCCEEDED(pIInfo->GetIdealFrames((long)nTotalCharacterKeys, &frames)))
		{
			m_length = max(1, (int)frames);
			return;
		}
	}
	ASSERT(FALSE);
}

gldEffect::~gldEffect(void)
{
}

gldEffect *gldEffect::Clone() const
{
	gldEffect *pEffect = new gldEffect(m_clsid);

	pEffect->Copy(this);

	return pEffect;
}

void gldEffect::Copy(const gldEffect *pSrc)
{	
	m_pIEffect.Release();
	m_clsid = pSrc->m_clsid;
	m_pIEffect.CoCreateInstance(m_clsid);
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
	
	m_startTime = pSrc->m_startTime;
	m_length = pSrc->m_length;

	m_pEffectInfo = pSrc->m_pEffectInfo;
}

std::string gldEffect::GetName()
{
	return m_pEffectInfo ? m_pEffectInfo->m_name : m_name;
}

void gldEffect::GetToolTipText(CString &strText)
{
	float frameRate = _GetMainMovie2()->m_frameRate;
	strText.Format("%s\nStart: %d (%.1fs)\nDuration: %d (%.1fs)", 
		GetName().c_str(), 
		m_startTime + 1, 
		(float)m_startTime / frameRate, 
		m_length, 
		(float)m_length / frameRate);
}

HRESULT gldEffect::WriteToStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	HRESULT hr;

	// version
	WORD ver = 1;
	hr = stm.Write(ver);
	CHECKRESULT();
	// m_clsid
	hr = stm.Write(m_clsid);
	CHECKRESULT();
	// m_pIParas
	hr = stm.Write(m_pIParas.p, pUnk);
	CHECKRESULT();
	// m_startTime
	hr = stm.Write(m_startTime);
	CHECKRESULT();
	// m_length
	hr = stm.Write(m_length);
	CHECKRESULT();
	// m_name
	hr = stm.Write(GetName());
	CHECKRESULT();
	
	return S_OK;
}

HRESULT gldEffect::ReadFromStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	HRESULT hr;

	CReplaceLibrary xRplLib((gldLibrary *)pUnk);

	// version
	WORD ver = 1;
	hr = stm.Read(ver);
	CHECKRESULT();
	if (ver == 1)
	{
		// m_clsid
		hr = stm.Read(m_clsid);
		CHECKRESULT();
		// create effect coclass
		m_pIEffect.Release();
		m_pIEffect.CoCreateInstance(m_clsid); // ignore effect create error
		//hr = m_pIEffect.CoCreateInstance(m_clsid);
		//CHECKRESULT();
		// m_pIParas
		m_pIParas.Release();
		hr = m_pIParas.CoCreateInstance(__uuidof(GLD_Parameters));
		CHECKRESULT();
		hr = stm.Read(m_pIParas.p, pUnk);
		CHECKRESULT();
		// m_startTime
		hr = stm.Read(m_startTime);
		CHECKRESULT();
		// m_length
		hr = stm.Read(m_length);
		CHECKRESULT();
		// m_name
		hr = stm.Read(m_name);
		CHECKRESULT();

		// get effect information
		m_pEffectInfo = gldEffectManager::Instance()->FindEffect(m_clsid);
		//ASSERT(m_pEffectInfo); // m_pEffectInfo may be null if this effect is not installed
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

CCmdChangeEffectParameters::CCmdChangeEffectParameters(IGLD_Parameters *pTarget, IGLD_Parameters *pNew)
{
	_M_Desc_ID = IDS_CMD_CHANGEEFFECTSETTINGS;

	if (SUCCEEDED(m_pOld.CoCreateInstance(__uuidof(GLD_Parameters))))
	{
		m_pTarget = pTarget;

		m_pOld->Copy(m_pTarget);
		m_pNew = pNew;
	}
}

CCmdChangeEffectParameters::~CCmdChangeEffectParameters()
{
}

bool CCmdChangeEffectParameters::Execute()
{
	if (m_pOld == NULL)
		return FALSE;

	m_pTarget->Copy(m_pNew);

	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdChangeEffectParameters::Unexecute()
{
	if (m_pOld == NULL)
		return FALSE;

	m_pTarget->Copy(m_pOld);

	my_app.Repaint();

	return TCommand::Unexecute();
}
