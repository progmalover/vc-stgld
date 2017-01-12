#include "stdafx.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "SWFStream.h"
#include "I2C.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_FILE_DESCRIPTION	1024

HRESULT WriteToStream(oBinStream &os, IStream *pStm)
{
	HRESULT hr;
	U32 size = os.Size();
	ULONG cbWritten;
	hr = pStm->Write(&size, (ULONG)sizeof(size), &cbWritten);
	CHECKRESULT();
	if (cbWritten != sizeof(size))
		return E_FAIL;
	CAutoPtr<U8> buf(new U8[size]);
	os.WriteToMemory(buf);
	hr = pStm->Write(buf, size, &cbWritten);
	CHECKRESULT();
	if (cbWritten != size)
		return E_FAIL;
	return S_OK;
}

HRESULT ReadFromStream(IStream *pStm, iBinStream &is)
{
	HRESULT hr;
	U32 size = 0;
	ULONG cbRead;
	hr = pStm->Read(&size, (ULONG)sizeof(size), &cbRead);
	CHECKRESULT();

	TRACE("Read The Obj Size From File :%d\n",size);

	if (cbRead != sizeof(size))
		return E_FAIL;
	CAutoPtr<U8> buf(new U8[size]);
	hr = pStm->Read(buf.m_p, size, &cbRead);
	CHECKRESULT();
	if (cbRead != size)
		return E_FAIL;
	is.ReadFromMemory(size, buf.m_p);
	return S_OK;
}

// ------------------------------------------CObjectAttachedInfo----------------------------------
#define CHECK_THROW_HR(hr) if (FAILED(hr)) throw hr

CObjectAttachedInfo::CObjectAttachedInfo()
: m_mask(0)
{
}

HRESULT CObjectAttachedInfo::LoadFromStorage(IStorage *pIStorage)
{
	ASSERT(pIStorage != NULL);

	HRESULT hr = S_OK;

	if ((m_mask & IF_DESC) != 0)
	{
		IStreamWrapper stmDesc;
		hr = pIStorage->OpenStream(L"Desc", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &stmDesc);
		if (SUCCEEDED(hr))
		{
			ULONG len = 0;
			hr = stmDesc.Read(len);
			if (SUCCEEDED(hr))
			{
				len = __min(len, MAX_FILE_DESCRIPTION);
				CAutoPtr<char> szDesc(new char[len + 1]);
				ULONG cbRead = 0;
				hr = stmDesc->Read(szDesc.m_p, len, &cbRead);
				if (SUCCEEDED(hr))
				{
					szDesc[cbRead] = '\0';
					m_strDesc = szDesc;					
				}			
			}
		}
	}

	if ((m_mask & IF_TRANS) != 0)
	{
		IStreamWrapper stmTrans;
		try
		{
			hr = pIStorage->OpenStream(L"Trans", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &stmTrans);
			CHECK_THROW_HR(hr);
			WORD ver = 0;
			hr = stmTrans.Read(ver);
			CHECKRESULT();
			if (ver != 1)
			{
				CHECK_THROW_HR(hr);
			}
			CComPtr<IGLD_Matrix> pIMx;
			hr = pIMx.CoCreateInstance(__uuidof(GLD_Matrix));
			CHECK_THROW_HR(hr);
			CComPtr<IGLD_Cxform> pICx;
			hr = pICx.CoCreateInstance(__uuidof(GLD_Cxform));
			CHECK_THROW_HR(hr);
			hr = stmTrans.Read(pIMx.p, NULL);
			CHECK_THROW_HR(hr);
			hr = stmTrans.Read(pICx.p, NULL);
			CHECK_THROW_HR(hr);
			hr = CI2C::Convert(pIMx, &m_matrix);
			CHECK_THROW_HR(hr);
			hr = CI2C::Convert(pICx, &m_cxform);
			CHECK_THROW_HR(hr);			
		}
		catch (HRESULT hr)
		{
			hr;
		}
	}

	return hr;
}

HRESULT CObjectAttachedInfo::SaveToStorage(IStorage *pIStorage)
{
	ASSERT(pIStorage != NULL);

	HRESULT hr = S_OK;

	if ((m_mask & IF_DESC) != 0)
	{
		IStreamWrapper stmDesc;
		hr = pIStorage->CreateStream(L"Desc", STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &stmDesc);
		CHECKRESULT();
		ULONG len = strlen((const char *)(LPCTSTR)m_strDesc);
		hr = stmDesc.Write(len);
		CHECKRESULT();
		hr = stmDesc.WriteBlock((LPCTSTR)m_strDesc, len);
		CHECKRESULT();		
	}

	if ((m_mask & IF_TRANS) != 0)
	{
		IStreamWrapper stmTrans;
		hr = pIStorage->CreateStream(L"Trans", STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &stmTrans);
		CHECKRESULT();
		hr = stmTrans.Write((WORD)1);
		CHECKRESULT();
		CComPtr<IGLD_Matrix> pIMx;
		hr = pIMx.CoCreateInstance(__uuidof(GLD_Matrix));
		CHECKRESULT();
		hr = pIMx->SetData(m_matrix.m_e11, m_matrix.m_e12, m_matrix.m_e21, m_matrix.m_e22, m_matrix.m_x, m_matrix.m_y);
		CHECKRESULT();
		CComPtr<IGLD_Cxform> pICx;
		hr = pICx.CoCreateInstance(__uuidof(GLD_Cxform));
		CHECKRESULT();
		hr = pICx->SetData(m_cxform.ra, m_cxform.rb, m_cxform.ga, m_cxform.gb, m_cxform.ba, m_cxform.bb, m_cxform.aa, m_cxform.ab);
		CHECKRESULT();
		hr = stmTrans.Write(pIMx.p, NULL);
		CHECKRESULT();
		hr = stmTrans.Write(pICx.p, NULL);
		CHECKRESULT();
	}

	return S_OK;
}

void CObjectAttachedInfo::SetDescription(const CString &strDesc)
{
	m_strDesc = strDesc;
	m_mask |= IF_DESC;
}

void CObjectAttachedInfo::SetTransform(const gldMatrix &matrix, const gldCxform &cxform)
{
	m_matrix = matrix;
	m_cxform = cxform;
	m_mask |= IF_TRANS;
}
//-----------------------------------------------------------------------------------------------
// ------------------------------------------LoadObjectFromFile----------------------------------
HRESULT LoadObjectFromFile(const CString &strFileName, IGLD_Object **ppObject, CObjectAttachedInfo &info)
{
	IF_ARG_NULL_RETURN(ppObject);

	HRESULT hr;

	// check if old gls format
	CComPtr<IGLD_FileStream> pIFileStm;
	hr = pIFileStm.CoCreateInstance(__uuidof(GLD_FileStream));
	if (SUCCEEDED(hr))
	{
		hr = pIFileStm->Open(CComBSTR(strFileName), CComBSTR(_T("rb")));
		if (SUCCEEDED(hr))
		{
			CComPtr<IGLD_Shape> pIShape;
			hr = pIShape.CoCreateInstance(__uuidof(GLD_Shape));
			if (SUCCEEDED(hr))
			{
				hr = pIShape->Load(pIFileStm);
				if (SUCCEEDED(hr))
				{
					*ppObject = pIShape.Detach();
					info.SetMask(0);
					return S_OK;
				}
			}
		}
	}

	if (pIFileStm != NULL)	// close file
		pIFileStm.Release();

	CComPtr<IStorage> pStg;
	USES_CONVERSION;
	hr = StgOpenStorage(A2OLE((LPCTSTR)strFileName), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);
	CHECKRESULT();

	CLSID clsid;
	hr = ReadClassStg(pStg, &clsid);
	CHECKRESULT();

	CComPtr<IGLD_Object> pIObject;
	hr = pIObject.CoCreateInstance(clsid);
	CHECKRESULT();

	CComQIPtr<IPersistStorage> pPStg = pIObject;
	if (pPStg == NULL)
		return E_FAIL;
	hr = pPStg->Load(pStg);
	CHECKRESULT();

	hr = pIObject.CopyTo(ppObject);
	CHECKRESULT();

	hr = info.LoadFromStorage(pStg);	

	return hr;
}

HRESULT LoadObjectFromFile(const CString &strFileName, IGLD_Object **ppObject, CString &strDesc)
{
	CObjectAttachedInfo info;
	info.SetMask(CObjectAttachedInfo::IF_DESC);
	HRESULT hr = LoadObjectFromFile(strFileName, ppObject, info);
	CHECKRESULT();
	strDesc = info.GetDescription();
	return S_OK;
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------SaveObjectToFile--------------------------------------------
HRESULT SaveObjectToFile(IGLD_Object *pIObject, const CString &strFileName, CObjectAttachedInfo &info)
{
	IF_ARG_NULL_RETURN(pIObject);

	CComPtr<IStorage> pStg;

	HRESULT hr = S_OK;
	USES_CONVERSION;
	hr = StgCreateDocfile(A2OLE(LPCTSTR(strFileName)), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &pStg);
	CHECKRESULT();

	CComQIPtr<IPersistStorage> pPStg = pIObject;
	if (pPStg == NULL)
		hr = E_NOINTERFACE;
	CHECKRESULT();

	hr = OleSave(pPStg, pStg, FALSE);
	CHECKRESULT();

	hr = info.SaveToStorage(pStg);
	CHECKRESULT();

	return S_OK;
}

HRESULT SaveObjectToFile(IGLD_Object *pIObject, const CString &strFileName, CString &strDesc)
{	
	CObjectAttachedInfo info;
	info.SetDescription(strDesc);
	return SaveObjectToFile(pIObject, strFileName, info);
}
//----------------------------------------------------------------------------------------------------
//------------------------------------ChangeObjectFileDesc--------------------------------------------
HRESULT ChangeObjectFileDesc(const CString &strFileName, const CString &strDesc)
{
	CComPtr<IStorage> pStg;
	
	HRESULT hr = S_OK;
	USES_CONVERSION;
	hr = StgOpenStorage(A2OLE((LPCTSTR)strFileName), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);
	CHECKRESULT();	
	
	CComPtr<IStream> pIDesc;
	hr = pStg->CreateStream(L"Desc", STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &pIDesc);
	CHECKRESULT();
	ULONG len = strlen((const char *)(LPCTSTR)strDesc);
	IStreamWrapper stm(pIDesc);
	hr = stm.Write(len);
	CHECKRESULT();
	ULONG cbWritten = 0;
	hr = pIDesc->Write((LPCTSTR)strDesc, len, &cbWritten);
	CHECKRESULT();
	if (cbWritten != len)
		return E_FAIL;

	return S_OK;
}
//----------------------------------------------------------------------------------------------------