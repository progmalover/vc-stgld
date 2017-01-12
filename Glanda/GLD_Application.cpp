// GLD_Application.cpp : Implementation of CGLD_Application

#include "stdafx.h"
#include "GLD_Application.h"
#include ".\gld_application.h"

#include "GlandaDoc.h"
#include "gldLibrary.h"

#include "my_app.h"

#include "CmdInsertScene.h"
#include "CmdDeleteScene.h"
#include "CmdChangeCurrentScene.h"
#include "CmdAddShape.h"
#include "CmdInsertEffect.h"
#include "CmdGroup.h"
#include "CmdBreakApart.h"
#include "CmdBringToFront.h"
#include "CmdSendToBack.h"
#include "GlandaCommand.h"

#include "gldImage.h"

#include "TransAdaptor.h"
#include "SWFProxy.h"

#include "I2C.h"
#include "gldButton.h"

#include "gldEffect.h"
#include "gldSprite.h"
#include "CmdAddSound.h"
#include "SWFStream.h"

#include "gldSound.h"
#include "gldSceneSound.h"
#include "gldMovieSound.h"

#include "CmdModifyMovieSound.h"
#include "CmdRemoveSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CGLD_Application

// {E23DBDE5-A854-4a37-8A92-FA43F524F1D3}
DEFINE_GUID(CATID_GLD_Template, 0xe23dbde5, 0xa854, 0x4a37, 0x8a, 0x92, 0xfa, 0x43, 0xf5, 0x24, 0xf1, 0xd3);

STDMETHODIMP CGLD_Application::get_hWnd(long *pWnd)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here

	*pWnd = (long)AfxGetMainWnd()->GetSafeHwnd();
	return S_OK;
}
//此处应为模板COM对象调用接口，传递一个场景名？ 
STDMETHODIMP CGLD_Application::AddScene(long nPos, BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here

	if (nPos < 1 || nPos > _GetMainMovie2()->m_sceneList.size() + 1)
		return E_FAIL;

	CCmdInsertScene *pCmd = new CCmdInsertScene(CString(bstrName), (int)nPos - 1);
	my_app.Commands().Do(pCmd);

	return S_OK;
}

STDMETHODIMP CGLD_Application::DeleteScene()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here

	gldMainMovie2 *pMovie = _GetMainMovie2();
	gldScene2 *pScene = pMovie->GetCurScene();
	if (pScene == NULL)
		return E_INVALIDARG;

	int index = pMovie->GetSceneIndex(pScene);

	int count = _GetMainMovie2()->m_sceneList.size();
	gldScene2 *pSceneNew = pMovie->GetScene(index < count - 1 ? index + 1 : index - 1);

	TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_DELETESCENE);
	pCmdGroup->Do(new CCmdChangeCurrentScene(pSceneNew));
	pCmdGroup->Do(new CCmdDeleteScene(pScene, true));
	my_app.Commands().Do(pCmdGroup);

	return S_OK;
}

STDMETHODIMP CGLD_Application::SetCurScene(long nPos)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here

	gldScene2 *pScene = _GetMainMovie2()->GetScene((int)nPos - 1);
	ASSERT(pScene);
	if (pScene)
	{
		CCmdChangeCurrentScene *pCmd = new CCmdChangeCurrentScene(pScene);
		return my_app.Commands().Do(pCmd) ? S_OK : E_FAIL;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::CreateInstance(IGLD_Object *pIObject, IGLD_Matrix *pIMatrix, IGLD_Cxform *pICxform, long *pPos)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here

	gldObj *pObj;
	HRESULT hr = CI2C::Create(pIObject, &pObj);
	if (FAILED(hr))
		return E_UNEXPECTED;

	gldMatrix _matrix;
	//pIMatrix  to  _matrix
	CI2C::Convert(pIMatrix, &_matrix);

	gldCxform _cxform;
	CI2C::Convert(pICxform, &_cxform);

	//ASSERT(_GetLibrary()->FindObj(_obj) != NULL);

	gldScene2 *pScene = _GetCurScene2();

	TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_CREATEINSTANCE);
	pCmdGroup->Do(new CCmdAddObj(pObj));
	pCmdGroup->Do(new CCmdAddInstance(pScene, pObj, _matrix, _cxform));
	if (!my_app.Commands().Do(pCmdGroup))
		return E_FAIL;

	*pPos = pScene->m_instanceList.size();

	return S_OK;
}

STDMETHODIMP CGLD_Application::Select(long nPos, VARIANT_BOOL bAppend)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here
	
	gldScene2 *pScene = _GetCurScene2();
	ASSERT(pScene);

	gldInstance *pInstance = pScene->GetInstance((int)nPos - 1);

	if (pInstance == NULL)
		return E_FAIL;

	gld_shape_sel sel;
	if (bAppend != VARIANT_FALSE)
		sel = my_app.CurSel();

	sel.select(CTraitShape(pInstance));

	my_app.DoSelect(sel);

	return S_OK;
}

STDMETHODIMP CGLD_Application::GetMovieSize(long *nWidth, long *nHeight)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CSize size = my_app.GetMovieSize();
	*nWidth = size.cx;
	*nHeight = size.cy;

	return S_OK;
}

STDMETHODIMP CGLD_Application::SetMovieSize(long nWidth, long nHeight)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldMainMovie2 *pMainMovie = _GetMainMovie2();
	return my_app.Commands().Do(new CCmdChangeMovieProperties(pMainMovie->m_frameRate, nWidth, nHeight, pMainMovie->m_color)) ? S_OK : E_FAIL;
}

STDMETHODIMP CGLD_Application::GetViewSize(long *nWidth, long *nHeight)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CSize size = my_app.GetViewSize();
	*nWidth = size.cx;
	*nHeight = size.cy;

	return S_OK;
}

STDMETHODIMP CGLD_Application::ViewToCanvas(long *x, long *y)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	int _x = *x;
	int _y = *y;
	my_app.ScreenToDataSpace(_x, _y);
	*x = _x;
	*y = _y;

	return S_OK;
}

STDMETHODIMP CGLD_Application::CenterMovie()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	my_app.CenterMovie();
	return S_OK;
}

STDMETHODIMP CGLD_Application::get_TotalScenes(long *nCount)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*nCount = _GetMainMovie2()->m_sceneList.size();
	return S_OK;
}

STDMETHODIMP CGLD_Application::get_EnterSceneActionScript(BSTR *bstrAction)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldScene2 *pScene = _GetCurScene2();
	if (pScene)
	{
		USES_CONVERSION;
		*bstrAction = ::SysAllocString(A2OLE(pScene->m_actionEnter.c_str()));
		if (*bstrAction != NULL)
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_EnterSceneActionScript(BSTR bstrAction)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldScene2 *pScene = _GetCurScene2();
	if (pScene)
	{
		if (my_app.Commands().Do(new CCmdSetSceneAction(pScene, CString(bstrAction), true)))
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::get_ExitSceneActionScript(BSTR *bstrAction)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldScene2 *pScene = _GetCurScene2();
	if (pScene)
	{
		USES_CONVERSION;
		*bstrAction = ::SysAllocString(A2OLE(pScene->m_actionExit.c_str()));
		if (*bstrAction != NULL)
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_ExitSceneActionScript(BSTR bstrAction)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldScene2 *pScene = _GetCurScene2();
	if (pScene)
	{
		if (my_app.Commands().Do(new CCmdSetSceneAction(pScene, CString(bstrAction), false)))
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::get_InstanceActionScript(BSTR *bstrAction)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		USES_CONVERSION;
		*bstrAction = ::SysAllocString(A2OLE(pInstance->m_action.c_str()));
		if (*bstrAction != NULL)
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_InstanceActionScript(BSTR bstrAction)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		if (my_app.Commands().Do(new CCmdSetAction(pInstance, CString(bstrAction))))
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::get_BgColor(long *Color)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*Color = _GetMainMovie2()->m_color;
	return S_OK;
}

STDMETHODIMP CGLD_Application::put_BgColor(long Color)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldMainMovie2 *pMainMovie =  _GetMainMovie2();
	return my_app.Commands().Do(new CCmdChangeMovieProperties(pMainMovie->m_frameRate, pMainMovie->m_width, pMainMovie->m_height, Color)) ? S_OK : E_FAIL;
}

STDMETHODIMP CGLD_Application::get_FrameRate(double *fFrameRate)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	*fFrameRate = _GetMainMovie2()->m_frameRate;
	return S_OK;
}

STDMETHODIMP CGLD_Application::put_FrameRate(double fFrameRate)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldMainMovie2 *pMainMovie =  _GetMainMovie2();
	return my_app.Commands().Do(new CCmdChangeMovieProperties((float)fFrameRate, pMainMovie->m_width, pMainMovie->m_height, pMainMovie->m_color)) ? S_OK : E_FAIL;
}

STDMETHODIMP CGLD_Application::get_InstanceName(BSTR *bstrName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		USES_CONVERSION;
		*bstrName = ::SysAllocString(A2OLE(pInstance->m_name.c_str()));
		if (*bstrName != NULL)
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_InstanceName(BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		if (my_app.Commands().Do(new CCmdChangeInstanceName(pInstance, CString(bstrName))))
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::get_InstanceMask(VARIANT_BOOL *bMask)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		*bMask = pInstance->m_mask ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_InstanceMask(VARIANT_BOOL bMask)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		if (my_app.Commands().Do(new CCmdChangeInstanceMask(pInstance, bMask != VARIANT_FALSE)))
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::get_SceneName(BSTR *bstrName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldScene2 *pScene = _GetCurScene2();
		USES_CONVERSION;
		*bstrName = ::SysAllocString(A2OLE(pScene->m_name.c_str()));
		if (*bstrName != NULL)
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_SceneName(BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldScene2 *pScene = _GetCurScene2();
		if (my_app.Commands().Do(new CCmdChangeSceneName(pScene, CString(bstrName))))
			return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::AddEffect(long nPos, BSTR bstrProgID, long nStart, long nLength, IGLD_Parameters *pParameters)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	ASSERT(nStart > 0 && nLength > 0 && pParameters != NULL);
	if (nStart > 0 && nLength > 0 && pParameters != NULL)
	{
		gld_shape_sel sel = my_app.CurSel();
		if (sel.count() == 1)
		{
			CLSID clsid;
			if (SUCCEEDED(::CLSIDFromProgID(bstrProgID, &clsid)))
			{
				USES_CONVERSION;

				LPTSTR lpszProgID = OLE2A(bstrProgID);

				gldEffect*pEffect = new gldEffect(clsid);
				if (pEffect->m_pIEffect != NULL && pEffect->m_pIParas != NULL)
				{
					pEffect->m_startTime = nStart - 1;
					pEffect->m_length = nLength;
					pEffect->m_pIParas = pParameters;

					gldInstance*pInstance = CTraitInstance(*sel.begin());
					gldEffect *pEffectInsetrBefore = pInstance->GetEffect(nPos - 1);
						
					my_app.Commands().Do(new CCmdInsertEffect(pInstance, pEffect, pEffectInsetrBefore));

					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}


STDMETHODIMP CGLD_Application::LoadObjectFromFile(BSTR bstrFileName, IGLD_Object **ppObject)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CString strDesc;

	USES_CONVERSION;

	return ::LoadObjectFromFile(CString(OLE2A(bstrFileName)), ppObject, strDesc);
}

STDMETHODIMP CGLD_Application::Draw(IGLD_Object *pIObject, HDC hDC, long x, long y, long cx, long cy, long nTime)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	IF_ARG_NULL_RETURN(pIObject);

	if (cx < 1 || cy < 1)
		return S_OK;

	HRESULT hr;

	GLD_ObjectType type;
	hr = pIObject->get_Type(&type);
	CHECKRESULT();

	if (type != gceShape && type != gceButton 
		&& type != gceSprite)
		return E_FAIL;
	
	CI2CMap I2CMap;
	CI2CMap *pTmpMap = CI2C::m_pI2CMap;	
	gldObj *pObj = NULL;
	if (type == gceShape)
	{
		CI2C::m_pI2CMap = &I2CMap;

		hr = CI2C::Create(pIObject, &pObj);
	}
	else 
	{
		CComPtr<IGLD_MovieClip> pIMovie;
		if (type == gceButton)
		{
			CComQIPtr<IGLD_Button> pIButton = pIObject;
			if (pIButton == NULL)
				hr = E_FAIL;
			CHECKRESULT();
			hr = pIButton->get_MovieClip(&pIMovie);
		}
		else
		{
			ASSERT(type == gceSprite);
			CComQIPtr<IGLD_Sprite> pISprite = pIObject;
			if (pISprite == NULL)
				hr = E_FAIL;
			CHECKRESULT();
			hr = pISprite->get_MovieClip(&pIMovie);
		}
		CHECKRESULT();
		ASSERT(pIMovie != NULL);
		CAutoPtr<gldSprite> pSprite(new gldSprite);
		ASSERT(pSprite->m_mc != NULL);
		CI2C::m_pI2CMap = &I2CMap;
		hr = CI2C::GetShowList(pIMovie, nTime, pSprite->m_mc);
		pObj = pSprite.Detach();
		I2CMap.Insert(pIObject, pObj);
	}

	CI2C::m_pI2CMap = pTmpMap;
	if (FAILED(hr))
	{
		I2CMap.FreeAllSymbol();
		return hr;
	}

	ASSERT(pObj != NULL);
	CSWFProxy::PrepareTShapeForGObj(pObj);

	gld_shape shape = CTraitShape(pObj);
	gld_rect bnd = shape.bound();
	if (bnd.width() > 0 && bnd.height() > 0)
	{
		float sx = (float)cx *20 / bnd.width();
		float sy = (float)cy *20 / bnd.height();
		float s = __min(sx, sy);
		TMatrix mx(s, 0, 0, s, 0, 0);
		mx.m_dx = (long)(10 *cx - (bnd.left + bnd.right) *s / 2);
		mx.m_dy = (long)(10 *cy - (bnd.top + bnd.bottom) *s / 2);
		TGraphicsDevice device(cx, cy);
		device.SetMatrix(mx);

		TDraw::draw(shape, device);
		device.Display(hDC, x, y);
	}

	I2CMap.FreeAllSymbol();

	return hr;
}

STDMETHODIMP CGLD_Application::Group()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() >= 2)
	{
		return my_app.Commands().Do(new CCmdGroupInstance(sel)) ? S_OK : E_FAIL;
	}
	return E_FAIL;
}

STDMETHODIMP CGLD_Application::Ungroup()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gld_shape shape = *sel.begin();
		gldInstance *pInstance = CTraitInstance(shape);
		if (pInstance->m_obj->IsGObjInstanceOf(gobjSprite))
		{
			gldSprite *pSprite = (gldSprite *)pInstance->m_obj;
			if (pSprite->m_isGroup)
				return my_app.Commands().Do(new CCmdBreakApart(shape)) ? S_OK : E_FAIL;
		}
	}
	return E_FAIL;
}

STDMETHODIMP CGLD_Application::BringForward()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() > 0)
	{
		return my_app.Commands().Do(new CCmdBringToFront(sel, 0)) ? S_OK : E_FAIL;
	}
	return E_FAIL;
}

STDMETHODIMP CGLD_Application::BringToFront()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() > 0)
	{
		return my_app.Commands().Do(new CCmdBringToFront(sel, 1)) ? S_OK : E_FAIL;
	}
	return E_FAIL;
}

STDMETHODIMP CGLD_Application::SendBackward()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() > 0)
	{
		return my_app.Commands().Do(new CCmdSendToBack(sel, 0)) ? S_OK : E_FAIL;
	}
	return E_FAIL;
}

STDMETHODIMP CGLD_Application::SendToBack()
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() > 0)
	{
		return my_app.Commands().Do(new CCmdSendToBack(sel, 1)) ? S_OK : E_FAIL;
	}
	return E_FAIL;
}

STDMETHODIMP CGLD_Application::ExportMovie(BSTR bstrPathName, VARIANT_BOOL bCompressed)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	USES_CONVERSION;
	LPTSTR lpszMovie = OLE2A(bstrPathName);
	_GetCurDocument()->m_strMovieExport = lpszMovie;
	return ::ExportMovie(AfxGetMainWnd()->GetLastActivePopup(), lpszMovie, bCompressed != VARIANT_FALSE, FALSE) ? S_OK : E_FAIL;
}

STDMETHODIMP CGLD_Application::AddSceneSound(IGLD_Sound *pSound, long nStartTime, long *pPos)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldScene2 *pScene = _GetCurScene2();
	if (pScene == NULL)
		return E_FAIL;

	gldObj *pObj;
	HRESULT hr = CI2C::Create(pSound, &pObj);
	if (FAILED(hr))
		return E_UNEXPECTED;

	gldSound *pSoundObj = (gldSound *)pObj;

	gldSceneSound *pSceneSound = new gldSceneSound();
	pSceneSound->m_sound->m_soundObj = pSoundObj;
	pSceneSound->m_startTime = nStartTime;
	pSceneSound->m_length = max(1, pSoundObj->GetSoundTime() * _GetMainMovie2()->m_frameRate);
	pSceneSound->m_name = pSoundObj->m_name;

	*pPos = pScene->m_soundList.size() + 1;

	TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_IMPORTSOUND);
	pCmdGroup->Do(new CCmdAddObj(pObj));
	pCmdGroup->Do(new CCmdAddSound(pSceneSound));
	return my_app.Commands().Do(pCmdGroup) ? S_OK : E_FAIL;
}

STDMETHODIMP CGLD_Application::DeleteSceneSound(long nPos)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldScene2 *pScene = _GetCurScene2();
	if (pScene == NULL)
		return E_FAIL;

	nPos--;
	if (nPos >= 0 && nPos < pScene->m_soundList.size())
	{
		gldSceneSound *pSound = pScene->GetSound(nPos);
		return my_app.Commands().Do(new CCmdRemoveSound(pSound)) ? S_OK : E_FAIL;
	}

	return E_INVALIDARG;
}

STDMETHODIMP CGLD_Application::SetMovieSound(IGLD_Sound *pSound, BSTR bstrStartScene, VARIANT_BOOL bLoopFlag, long nLoopTimes)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gldObj *pObj = NULL;
	gldSound *pSoundObj = NULL;

	if (pSound)
	{
		HRESULT hr = CI2C::Create(pSound, &pObj);
		if (FAILED(hr))
			return E_UNEXPECTED;

		gldSound *pSoundObj = (gldSound *)pObj;
	}

	USES_CONVERSION;

	gldMovieSound *pMovieSound = NULL;
	
	if (pObj)
	{
		pMovieSound = new gldMovieSound();
		pMovieSound->m_sound->m_soundObj = pSoundObj;
		pMovieSound->m_startScene = OLE2A(bstrStartScene);
		pMovieSound->m_sound->m_loopFlag = (bLoopFlag == VARIANT_TRUE);
		pMovieSound->m_sound->m_loopTimes = nLoopTimes;
	}

	return my_app.Commands().Do(new CCmdModifyMovieSound(pMovieSound)) ? S_OK : E_FAIL;
}

STDMETHODIMP CGLD_Application::get_EffectCommonParameters(IGLD_Parameters **ppParameters)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		*ppParameters = pInstance->m_pIParas;
		(*ppParameters)->AddRef();
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_Application::put_EffectCommonParameters(IGLD_Parameters *pParameters)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gldInstance *pInstance = CTraitInstance(*sel.begin());
		CCmdChangeEffectParameters *pCmd = new CCmdChangeEffectParameters(pInstance->m_pIParas, pParameters);
		return my_app.Commands().Do(pCmd) ? S_OK : E_FAIL;
	}

	return E_FAIL;
}
