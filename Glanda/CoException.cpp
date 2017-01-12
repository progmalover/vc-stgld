#include "StdAfx.h"
#include ".\coexception.h"
#include "Resource.h"
#include "ComUtils.h"

CCoException::CCoException(HRESULT hr)
: m_hResult(hr)
{
}

CCoException::CCoException(ISupportErrorInfo *pSupportErrorInfo, REFIID riid, HRESULT hr /*= S_OK*/)
: m_hResult(hr)
{
	if (pSupportErrorInfo != NULL && SUCCEEDED(pSupportErrorInfo->InterfaceSupportsErrorInfo(riid)))
	{
		GetErrorInfo(0, &m_pInfo);
	}
}

CCoException::CCoException(IErrorInfo *pInfo, HRESULT hr /*= S_OK*/)
: m_pInfo(pInfo)
, m_hResult(hr)
{
}

CCoException::~CCoException(void)
{
}

BOOL CCoException::GetErrorMessage(CString &strError, UINT *uHelpContext /*= NULL*/)
{
	ASSERT(uHelpContext == NULL);

	if (m_pInfo != NULL)
	{
		CComBSTR bstrDes;
		if (SUCCEEDED(m_pInfo->GetDescription(&bstrDes)))
		{
			strError = bstrDes;
			return TRUE;
		}
	}

	if (FAILED(m_hResult))
	{
		CoGetErrorMessage(m_hResult, strError);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CCoException::ReportError(UINT nType /*= MB_OK*/, UINT nMessageID /*= 0*/)
{
	CString strError;

	if (GetErrorMessage(strError))
	{
		return AfxMessageBox(strError, nType, 0);
	}
	else
	{
		if (nMessageID == 0)
		{
			return AfxMessageBox(IDS_NO_ERROR_MSG);
		}
		else
		{
			return AfxMessageBox(nMessageID, nType, 0);
		}
	}
}