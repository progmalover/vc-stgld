#pragma once

#define CHECKRESULT_HRESULT(hr) \
if (FAILED(hr)) \
{	\
	TRACE("%s(%d): COM calling failure (HRESULT=0x%08x)\n", __FILE__, __LINE__, hr);	\
	return hr;	\
}

#define CHECKRESULT_BOOL(hr) \
if (FAILED(hr)) \
{	\
	TRACE("%s(%d): COM calling failure (HRESULT=0x%08x)\n", __FILE__, __LINE__, hr);	\
	return FALSE;	\
}

#define CHECKRESULT_VOID(hr) \
if (FAILED(hr)) \
{	\
	TRACE("%s(%d): COM calling failure (HRESULT=0x%08x)\n", __FILE__, __LINE__, hr);	\
	return;	\
}

#define CHECKRESULT_CUSTOM(hr, ret) \
if (FAILED(hr)) \
{	\
	TRACE("%s(%d): COM calling failure (HRESULT=0x%08x)\n", __FILE__, __LINE__, hr);	\
	return ret;	\
}


void CoGetErrorMessage(HRESULT hr, TCHAR *pszBuffer, int nBufferSize);
void CoGetErrorMessage(HRESULT hr, CString &strBuffer);
void CoReportError(HRESULT hr);
