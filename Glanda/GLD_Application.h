// GLD_Application.h : Declaration of the CGLD_Application

#pragma once
#include "resource.h"       // main symbols
#include "Glanda_i.h"


// CGLD_Application
class gldObj;

class ATL_NO_VTABLE CGLD_Application : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGLD_Application, &CLSID_GLD_Application>,
	public IDispatchImpl<IGLD_Application, &IID_IGLD_Application, &LIBID_GlandaLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CGLD_Application()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_GLD_APPLICATION)


BEGIN_COM_MAP(CGLD_Application)
	COM_INTERFACE_ENTRY(IGLD_Application)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:
	STDMETHOD(AddScene)(long nPos, BSTR bstrName);
	STDMETHOD(DeleteScene)();
	STDMETHOD(SetCurScene)(long nPos);
	STDMETHOD(CreateInstance)(IGLD_Object *pIObject, IGLD_Matrix *pIMatrix, IGLD_Cxform *pICxform, long *pPos);
	STDMETHOD(Select)(long nPos, VARIANT_BOOL bAppend);
	STDMETHOD(GetMovieSize)(long *nWidth, long *nHeight);
	STDMETHOD(SetMovieSize)(long nWidth, long nHeight);
	STDMETHOD(GetViewSize)(long *nWidth, long *nHeight);
	STDMETHOD(ViewToCanvas)(long *x, long *y);
	STDMETHOD(CenterMovie)();
	STDMETHOD(AddEffect)(long nPos, BSTR bstrProgID, long nStart, long nLength, IGLD_Parameters *pParameters);
	STDMETHOD(LoadObjectFromFile)(BSTR bstrFileName, IGLD_Object **ppObject);
	STDMETHOD(Draw)(IGLD_Object *pIObject, HDC hDC, long x, long y, long cx, long cy, long nTime);	
	STDMETHOD(Group)();
	STDMETHOD(Ungroup)();
	STDMETHOD(BringForward)();
	STDMETHOD(BringToFront)();
	STDMETHOD(SendBackward)();
	STDMETHOD(SendToBack)();
	STDMETHOD(ExportMovie)(BSTR bstrPathName, VARIANT_BOOL bCompressed);
	STDMETHOD(AddSceneSound)(IGLD_Sound *pSound, long nStartTime, long *pPos);
	STDMETHOD(DeleteSceneSound)(long nPos);
	STDMETHOD(SetMovieSound)(IGLD_Sound *pSound, BSTR bstrStartScenebool, VARIANT_BOOL bLoopFlag, long nLoopTimes);

	STDMETHOD(get_hWnd)(long *pWnd);
	STDMETHOD(get_BgColor)(long *Color);
	STDMETHOD(put_BgColor)(long Color);
	STDMETHOD(get_FrameRate)(double *fFrameRate);
	STDMETHOD(put_FrameRate)(double fFrameRate);
	STDMETHOD(get_TotalScenes)(long *nCount);
	STDMETHOD(get_EnterSceneActionScript)(BSTR *bstrAction);
	STDMETHOD(put_EnterSceneActionScript)(BSTR bstrAction);
	STDMETHOD(get_ExitSceneActionScript)(BSTR *bstrAction);
	STDMETHOD(put_ExitSceneActionScript)(BSTR bstrAction);
	STDMETHOD(get_InstanceActionScript)(BSTR *bstrAction);
	STDMETHOD(put_InstanceActionScript)(BSTR bstrAction);
	STDMETHOD(get_InstanceName)(BSTR *bstrName);
	STDMETHOD(put_InstanceName)(BSTR bstrName);
	STDMETHOD(get_SceneName)(BSTR *bstrName);
	STDMETHOD(put_SceneName)(BSTR bstrName);
	STDMETHOD(get_EffectCommonParameters)(IGLD_Parameters **ppParameters);
	STDMETHOD(put_EffectCommonParameters)(IGLD_Parameters *pParameters);
	STDMETHOD(get_InstanceMask)(VARIANT_BOOL *bMask);
	STDMETHOD(put_InstanceMask)(VARIANT_BOOL bMask);
};

OBJECT_ENTRY_AUTO(__uuidof(GLD_Application), CGLD_Application)
