#pragma once

class CCoException
{
public:	
	~CCoException(void);
	CCoException(HRESULT hr);
	CCoException(ISupportErrorInfo *pSupportErrorInfo, REFIID riid, HRESULT hr = S_OK);	
	CCoException(IErrorInfo *pInfo, HRESULT hr = S_OK);

	virtual BOOL GetErrorMessage(CString &strError, UINT *uHelpContext = NULL);
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);

private:
	HRESULT m_hResult;
	CComPtr<IErrorInfo> m_pInfo;
};