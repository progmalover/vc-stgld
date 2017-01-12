#pragma once

#include "..\GlandaCOM\GlandaCOM.h"

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, long *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_I4)))
		return hr;
	*value = vt.lVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, int *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_INT)))
		return hr;
	*value = vt.intVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, UINT *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_UINT)))
		return hr;
	*value = vt.uintVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, double *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_R8)))
		return hr;
	*value = vt.dblVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, float *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_R4)))
		return hr;
	*value = vt.fltVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, VARIANT_BOOL *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_BOOL)))
		return hr;
	*value = vt.boolVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, DWORD *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_UI4)))
		return hr;
	*value = vt.ulVal;
	return S_OK;
}

inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, BSTR *value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_BSTR)))
		return hr;
	*value = ::SysAllocString(vt.bstrVal);
	return *value != NULL ? S_OK : E_OUTOFMEMORY;
}

template <typename QI>
inline HRESULT GetParameter(IGLD_Parameters *pParameters, LPCTSTR lpszName, QI **value)
{
	CComVariant vt;
	HRESULT hr = pParameters->get_Item(CComBSTR(lpszName), &vt);
	if (FAILED(hr))
		return hr;
	if (FAILED(vt.ChangeType(VT_UNKNOWN)))
		return hr;
	vt.punkVal->QueryInterface(__uuidof(QI), (void **)value);
	return S_OK;
}

#define PutParameter(pParameters, lpszName, value)	pParameters->put_Item(CComBSTR(lpszName), CComVariant(value))
