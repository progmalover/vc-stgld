#include "StdAfx.h"
#include ".\comutils.h"

void CoGetErrorMessage(HRESULT hr, TCHAR *pszBuffer, int nBufferSize)
{
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, 
				hr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				pszBuffer, 
				nBufferSize, 
				NULL) == 0)
	{
		sprintf(pszBuffer, "Unknown error (0x%08x)", hr);
	}
}

void CoGetErrorMessage(HRESULT hr, CString &strBuffer)
{
	CoGetErrorMessage(hr, strBuffer.GetBuffer(256), 256);
	strBuffer.ReleaseBuffer();
}

void CoReportError(HRESULT hr)
{
	TCHAR szError[256];
	CoGetErrorMessage(hr, szError, 256);
	AfxMessageBox(szError, MB_ICONERROR | MB_OK);
}
