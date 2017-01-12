#include "StdAfx.h"
#include "EffectCommonParameters.h"

struct _ParameterEntry
{
	LPCTSTR name;
	int type;
};

static _ParameterEntry _PEntry[] = {
	{EP_BREAK_APART, PT_COMMON},
	{EP_TEXT_BORDER, PT_COMMON},
	{EP_BORDER_WIDTH, PT_COMMON},
	{EP_BORDER_COLOR, PT_COMMON},
	{EP_CUSTOM_FILL, PT_COMMON},
	{EP_FILL_STYLE, PT_COMMON},
	{EP_FILL_INDIVIDUALLY, PT_COMMON},
	{EP_FILL_ANGLE, PT_COMMON},
	{EP_ADD_SHADOW, PT_COMMON},
	{EP_SHADOW_SIZE, PT_COMMON},
	{EP_SHADOW_COLOR, PT_COMMON},
	{EP_AUTO_PLAY, PT_COMMON},
	{EP_LOOP, PT_COMMON}	
};

int GetParameterType(LPCTSTR name, LPCTSTR *paras /*= NULL*/, int nParas /*= 0*/)
{
	if (paras != NULL)
	{
		for (int i = 0; i < nParas; i++)
		{
			if (_tcsicmp(name, paras[i]) == 0)
				return PT_DEFINED;
		}
	}	
	for (int i = 0; i < sizeof(_PEntry) / sizeof(_ParameterEntry); i++)
	{
		if (_tcsicmp(name, _PEntry[i].name) == 0)
			return _PEntry[i].type;
	}
	return PT_CUSTOM;
}

void RemoveParameters(IGLD_Parameters *paras, ParameterType type, LPCTSTR *defined_paras /*= NULL*/, int n_defined_paras /*= 0*/)
{
	CComPtr<IEnumVARIANT> pIEnum;
	HRESULT hr = paras->get__NewEnum(&pIEnum);
	if (SUCCEEDED(hr))
	{
		USES_CONVERSION;
		CComVariant varP;
		while (pIEnum->Next(1, &varP, NULL) == S_OK)
		{
			CComQIPtr<IGLD_Parameter> pIP = varP.punkVal;
			varP.Clear();
			if (pIP != NULL)
			{
				CComBSTR name;
				CComVariant var;
				hr = pIP->get_Name(&name);
				if (SUCCEEDED(hr))
				{
					if (GetParameterType(OLE2A(name), defined_paras, n_defined_paras) == type)
						paras->Remove(name);
				}
			}
		}
	}
}

void CopyParameters(IGLD_Parameters *src_paras, ParameterType type, IGLD_Parameters *dst_paras, LPCTSTR *defined_paras /*= NULL*/, int n_defined_paras /*= 0*/)
{
	CComPtr<IEnumVARIANT> pIEnum;
	HRESULT hr = src_paras->get__NewEnum(&pIEnum);
	if (SUCCEEDED(hr))
	{
		USES_CONVERSION;
		CComVariant varP;
		while (pIEnum->Next(1, &varP, NULL) == S_OK)
		{
			CComQIPtr<IGLD_Parameter> pIP = varP.punkVal;
			varP.Clear();
			if (pIP != NULL)
			{
				CComBSTR name;
				CComVariant var;
				hr = pIP->get_Name(&name);
				if (SUCCEEDED(hr))
				{
					if (GetParameterType(OLE2A(name), defined_paras, n_defined_paras) == type)
					{
						CComVariant var;
						hr = pIP->get_Value(&var);
						if (SUCCEEDED(hr))
							dst_paras->put_Item(name, var);
					}					
				}
			}
		}
	}
}

void CopyParameters(IGLD_Parameters *src_paras, IGLD_Parameters *dst_paras)
{
	CComPtr<IEnumVARIANT> pIEnum;
	HRESULT hr = src_paras->get__NewEnum(&pIEnum);
	if (SUCCEEDED(hr))
	{
		USES_CONVERSION;
		CComVariant varP;
		while (pIEnum->Next(1, &varP, NULL) == S_OK)
		{
			CComQIPtr<IGLD_Parameter> pIP = varP.punkVal;
			varP.Clear();
			if (pIP != NULL)
			{
				CComBSTR name;
				CComVariant var;
				hr = pIP->get_Name(&name);
				if (SUCCEEDED(hr))
				{
					CComVariant var;
					hr = pIP->get_Value(&var);
					if (SUCCEEDED(hr))
						dst_paras->put_Item(name, var);					
				}
			}
		}
	}
}
