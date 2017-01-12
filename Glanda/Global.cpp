#include "stdafx.h"
#include "Glanda.h"
#include "Global.h"
#include "filepath.h"

#include "gldDataKeeper.h"
#include "gldMovieClip.h"
#include "gldSWFEngine.h"
#include "gldLibrary.h"
#include "gldSprite.h"
#include "gldLayer.h"
#include "gldFrameClip.h"

#include "TransAdaptor.h"
#include "SWFActionSegment.h"

#include "DlgProgress.h"

#include "ToolsDef.h"
#include "ITextTool.h"
#include "TextToolEx.h"

#include "Options.h"

#include "acompiler.h"

#include "armadillo.h"

#include "GlandaDoc.h"
#include "SWFProxy.h"

#include "Regexx.h"
using namespace regexx;

#include "EffectCommonParameters.h"
#include "gldSound.h"
#include "gldMovieSound.h"
#include "I2C.h"

#include "OutputBar.h"
#include "OutputView.h"
#include "SWFStream.h"
#include "ComUtils.h"
#include "Background.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const char *g_szAppName			= "Sothink Glanda";

HMODULE g_hCompiler						= NULL;
PtrCompile g_lpfnCompile				= NULL;
PtrGetBinCode g_lpfnGetBinCode			= NULL;
PtrGetOutputMsg g_lpfnPtrGetOutputMsg	= NULL;
PtrFreeAllBuffers g_lpfnFreeAllBuffers	= NULL;

void MsgWaitForSingleObject(HANDLE hHandle)
{
	while (true)
	{
		DWORD dwRet = MsgWaitForMultipleObjects(1, &hHandle, FALSE, INFINITE, QS_ALLEVENTS);
		if (dwRet == WAIT_OBJECT_0)
		{
			break;
		}
		else if (dwRet == WAIT_OBJECT_0 + 1)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!::GetMessage(&msg, NULL, 0, 0))
					return;

				if (!::IsDialogMessage(NULL, &msg))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
		}
	}
}

struct CREATE_MOVIE_THREAD_PARAMETER
{
	CDlgProgress *pDlg;
	LPCTSTR lpszFilePath;
    BOOL bCompressed;
	BOOL bDebug;
};

BOOL BuildEffects(CDlgProgress *pDlg)
{
	BOOL ret = TRUE;

	gldMainMovie2 *_mainMovie2 = _GetMainMovie2();

	pDlg->SetPrompt("Generating effects...");
	pDlg->SetProgress(0);

	bool bCancel = false;
	pDlg->SetCancelFlagPtr(&bCancel);

	int nEffectCount = _mainMovie2->GetEffectCount();

	CSubject sub;
	CSubjectManager::Instance()->Register("BuildEffects", &sub);

	CSubjectManager::Instance()->GetSubject("BuildEffects")->Attach(pDlg);

	int nEffect = 0;
	GSCENE2_LIST::iterator iScene = _mainMovie2->m_sceneList.begin();
	for (; iScene != _mainMovie2->m_sceneList.end(); iScene++)
	{
		gldScene2 *_scene2 = *iScene;
		if (_scene2->m_backgrnd != NULL)
		{
			if (!_scene2->m_backgrnd->BuildEffects())
			{
				ret = FALSE;
				AfxMessageBox(IDS_FAILED_EXPORT_MOVIE);
				goto _exit;
			}
		}

		GINSTANCE_LIST::iterator iInst = _scene2->m_instanceList.begin(); 
		for (; iInst != _scene2->m_instanceList.end(); iInst++)
		{
			if (bCancel)
			{
				ret = FALSE;
				goto _exit;
			}

			gldInstance *_inst = *iInst;
			//ASSERT(_inst->m_mc == NULL);
			if (_inst->m_effectList.size() > 0)
			{
				if (!_inst->BuildEffects(gldInstance::IS_PROMPT))
				{
					ret = FALSE;
					AfxMessageBox(IDS_FAILED_EXPORT_MOVIE);
					goto _exit;
				}

				CSubjectManager::Instance()->GetSubject("BuildEffects")->Notify((void *)(++nEffect * 100 / nEffectCount));
			}
		}
	}

_exit:
	CSubjectManager::Instance()->UnRegister("BuildEffects");

	return ret;
}

void ClearEffects()
{
	gldMainMovie2 *_mainMovie2 = _GetMainMovie2();

	GSCENE2_LIST::iterator iScene = _mainMovie2->m_sceneList.begin();
	for (; iScene != _mainMovie2->m_sceneList.end(); iScene++)
	{
		gldScene2 *_scene2 = *iScene;
		GINSTANCE_LIST::iterator iInst = _scene2->m_instanceList.begin(); 
		for (; iInst != _scene2->m_instanceList.end(); iInst++)
		{
			gldInstance *_inst = *iInst;
			if (_inst->m_mc != NULL)
			{
				delete _inst->m_mc;
				_inst->m_mc = NULL;
			}
		}
	}
}

HRESULT LoadObjectFromResource(LPCTSTR lpszResType, LPCTSTR lpszResName, IGLD_Object **ppObject)
{
	HMODULE hMod = AfxGetInstanceHandle();

	HRSRC hRes = FindResource(hMod, lpszResName, lpszResType);
	if(hRes == NULL)
		return E_UNEXPECTED;

	HGLOBAL hResData = LoadResource(hMod, hRes);			
	if (hResData == NULL)
	{
		return E_OUTOFMEMORY;
	}
	
	DWORD dwSize = SizeofResource(hMod, hRes);
	BYTE *pBuff = (BYTE *)LockResource(hResData);
	if (pBuff == NULL) 
	{ 
		return E_OUTOFMEMORY; 
	}

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (hGlobal == NULL) 
	{ 
		return E_OUTOFMEMORY; 
	}
	memcpy(GlobalLock(hGlobal), pBuff, dwSize);
	FreeResource(hResData);

	LPLOCKBYTES pLockBytes;
	HRESULT hr = CreateILockBytesOnHGlobal(hGlobal, TRUE, &pLockBytes);
	CHECKRESULT_HRESULT(hr);

	CComPtr<IStorage> pIStg;
	hr = StgOpenStorageOnILockBytes(pLockBytes, NULL, STGM_READ | STGM_SHARE_DENY_WRITE, NULL ,0, &pIStg);
	CHECKRESULT_HRESULT(hr);

	CLSID clsid;
	hr = ReadClassStg(pIStg, &clsid);
	CHECKRESULT_HRESULT(hr);

	CComPtr<IGLD_Object> pIObject;
	pIObject.CoCreateInstance(clsid);
	CHECKRESULT_HRESULT(hr);

	CComQIPtr<IPersistStorage> pIPerStg = pIObject;
	if (pIPerStg == NULL)
		hr = E_NOINTERFACE;
	CHECKRESULT_HRESULT(hr);

	hr = pIPerStg->Load(pIStg);
	CHECKRESULT_HRESULT(hr);

	*ppObject = pIObject.Detach();

	return *ppObject ? S_OK : E_FAIL;
}

DWORD WINAPI CreateMovieThread(LPVOID lpParameter)
{
	UINT nIDResult = IDOK;

	CREATE_MOVIE_THREAD_PARAMETER *pParam = (CREATE_MOVIE_THREAD_PARAMETER *)lpParameter;

	CDlgProgress *pDlg = pParam->pDlg;
	pDlg->WaitForInitialize();

	::CoInitialize(0);

	// Build all the effects in all scenes
	if (!BuildEffects(pDlg))
		nIDResult = IDCANCEL;

	if (nIDResult != IDCANCEL)
	{
		gldMainMovie2 *_mainMovie2 = _GetMainMovie2();
		gldMovieClip *_mainMovie = new gldMovieClip();		

		gldDataKeeper::Instance()->m_curMovieClip = _mainMovie;

		gldLibrary *_objLibOld = gldDataKeeper::Instance()->m_objLib;
		//gldLibrary *_objLib = _GetLibrary();
		gldLibrary *_objLib = new gldLibrary;
		_GetObjectMap()->CopyTo(_objLib);
		// bug!!!!
		if (CI2C::m_pI2CMap != NULL)
		{
			CI2CMap::_I2CMap::iterator ito = CI2C::m_pI2CMap->m_Map.begin();
			for (; ito != CI2C::m_pI2CMap->m_Map.end(); ++ito)
				_objLib->AddObj((*ito).second);
		}
		_objLib->ClearAllObjID();

		vector<gldSprite *> lstTmpSpriteNoAutoDelete;
		vector<gldSprite *> lstTmpSpriteAutoDelete;

		gldDataKeeper::Instance()->m_objLib = _objLib;

		gldMovieClip *_scene = _mainMovie;

		gldMovieSound *_movieSound = _mainMovie2->m_pMovieSound;
		if (_movieSound != NULL)
		{
			gldLayer *_soundLayer = new gldLayer("", 0);

			ASSERT(_soundLayer->m_frameClipList.size() == 0);

			// get sound start time
			int _startTime;
			gldScene2 *_soundStartScene = _mainMovie2->FindSceneByName(_movieSound->m_startScene.c_str());
			if (_soundStartScene != NULL)
				_startTime = _mainMovie2->GetSceneStartTime(_soundStartScene);
			else
				_startTime = 0;

			// get sound length, including loop
			int _frameLength = (int)(_movieSound->m_sound->m_soundObj->GetSoundTime() * _mainMovie2->m_frameRate);
			if (_movieSound->m_sound->m_loopFlag)
			{
				ASSERT(_movieSound->m_sound->m_loopTimes >= 1);
				_frameLength *= _frameLength;
			}
			_frameLength = max(1, min(_mainMovie2->GetMaxTime() - _startTime, _frameLength));

			// create the sound frame clip
			gldFrameClip *_fc = new gldFrameClip(_startTime, _frameLength);
			if (_fc->m_sound)
				delete _fc->m_sound;
			_fc->m_sound = _movieSound->m_sound->Clone();

			_soundLayer->m_frameClipList.push_back(_fc);

			_scene->AddLayer(_soundLayer);
		}

		int nCurFrame = 0;

		GSCENE2_LIST::iterator iScene = _mainMovie2->m_sceneList.begin();
		for (; iScene != _mainMovie2->m_sceneList.end(); iScene++)
		{
			gldScene2 *_scene2 = *iScene;

			int nSceneTime = _scene2->GetMaxTime(FALSE);
			if (nSceneTime == 0)	// empty scene
				continue;

			ostringstream osPrevScene;
			osPrevScene << "gotoAndPlay(" << max(1, nCurFrame) << ")";
			std::string sPrevScene = osPrevScene.str();

			ostringstream osNextScene;
			osNextScene << "gotoAndPlay(" << nCurFrame + nSceneTime + 1 << ")";
			std::string sNextScene = osNextScene.str();			

			// create a frame clip to hold the frame label (inherit from scene name)
			if (_scene2->m_name.size() > 0)
			{
				gldLayer *_layer = new gldLayer("", 0);
				gldFrameClip *_fc = new gldFrameClip(nCurFrame, 1);
				_fc->m_label = _scene2->m_name;

				_layer->m_frameClipList.push_back(_fc);
				_scene->AddLayer(_layer);
			}			

			bool bEnterAction = !_scene2->m_actionEnter.empty();
			bool bExitAction = !_scene2->m_actionExit.empty();

			if (bEnterAction || bExitAction)
			{
				gldLayer *_layer = new gldLayer("", 0);

				int _fc1_len = max(1, nSceneTime - 1);
				gldFrameClip *_fc1 = new gldFrameClip(nCurFrame, _fc1_len);
				_layer->m_frameClipList.push_back(_fc1);

				gldFrameClip *_fc2 = _fc1;
				// if nSceneTime == 1, then share one frame clip
				if (bExitAction && nSceneTime >= 2)
				{
					_fc2 = new gldFrameClip(nCurFrame + _fc1_len, 1);
					_layer->m_frameClipList.push_back(_fc2);
				}

				Regexx re;
				std::string sAction1;
				if (bEnterAction)
				{
					sAction1 = re.replace(_scene2->m_actionEnter, "(prevScene[ \t\r\n]*\\([ \t]*\\))", sPrevScene, Regexx::nocase | Regexx::dotall);					
					sAction1 = re.replace(sAction1, "(nextScene[ \t\r\n]*\\([ \t]*\\))", sNextScene, Regexx::nocase | Regexx::dotall);					
				}

				std::string sAction2;
				if (bExitAction)
				{
					sAction2 = re.replace(_scene2->m_actionExit, "(prevScene[ \t\r\n]*\\([ \t]*\\))", sPrevScene, Regexx::nocase | Regexx::dotall);					
					sAction2 = re.replace(sAction2, "(nextScene[ \t\r\n]*\\([ \t]*\\))", sNextScene, Regexx::nocase | Regexx::dotall);
				}

				if (bEnterAction)
				{
					_fc1->m_action = new gldAction();
					_fc1->m_action->m_type = actionSegFrame;
				}

				if (bExitAction)
				{
					if (_fc2 != _fc1)	// not share one frame clip
					{
						_fc2->m_action = new gldAction();
						_fc2->m_action->m_type = actionSegFrame;
					}
					else
					{
						if (_fc1->m_action == NULL)
						{
							_fc1->m_action = new gldAction();
							_fc1->m_action->m_type = actionSegFrame;
						}
					}
				}

				if (bEnterAction && bExitAction && _fc2 == _fc1)	// share one frame clip
				{
					sAction1 += "\r\n" + sAction2;
					_fc1->m_action->SetSourceCode(sAction1.c_str());
				}
				else
				{
					if (bEnterAction)
						_fc1->m_action->SetSourceCode(sAction1.c_str());

					if (bExitAction)
						_fc2->m_action->SetSourceCode(sAction2.c_str());
				}

				_scene->AddLayer(_layer);
			}

			//int nMaxTime = 1;
			BOOL bMask = FALSE;
			GINSTANCE_LIST::reverse_iterator iInst = _scene2->m_instanceList.rbegin(); 
			for (; iInst != _scene2->m_instanceList.rend(); iInst++)
			{
				gldInstance *_inst = *iInst;				

				gldLayer *_layer = new gldLayer("", 0);

				if (_inst->m_mask)
				{
					_layer->m_mask = true;
					_layer->m_masked = false;

					bMask = TRUE;
				}
				else
				{
					if (bMask)
					{
						_layer->m_mask = false;
						_layer->m_masked = true;
					}
				}


				///////////////////////////////////////////////////////////////////////////
				// create effect
				gldFrameClip *_fc1 = NULL;
				gldFrameClip *_fc2 = NULL;		// need an extra frame clip if it is the 
												// longest effect and bLoop == false

				gldCharacterKey *_ck1 = NULL;
				gldCharacterKey *_ck2 = NULL;	// need an extra effect movie clip if it 
												// is the longest effect and bLoop == false

				if (_inst->m_mc != NULL)
				{
					gldMovieClip *_mc = _inst->m_mc;

					// get effect length
					int nEffectTime = _mc->GetMaxTime();

					BOOL bLoop = FALSE;
					GetParameter(_inst->m_pIParas, EP_LOOP, &bLoop);

					if (bLoop)
					{
						// if loop flag is set, should allocate a frame clip whose length 
						// equals to the scene.
						_fc1 = new gldFrameClip(nCurFrame, nSceneTime);
					}
					else
					{
						// if the effect's last frame is the scene's last frame, 
						// must create an extra key frame and place the same effect 
						// movie clip in it. the second movie clip must have the 
						// same instance name and action as the first one.
						// why? (yanghui)
						//if (/*nEffectTime == nSceneTime &&*/ nEffectTime > 1)
						//{
						//	_fc1 = new gldFrameClip(nCurFrame, nEffectTime - 1);
						//	_fc2 = new gldFrameClip(nCurFrame + nEffectTime - 1, 1);
						//}
						//else
						{
							_fc1 = new gldFrameClip(nCurFrame, nEffectTime);
						}
					}

					gldSprite *_sprite = new gldSprite();
					//_sprite->UpdateUniId();
					_GetObjectMap()->UpdateUniId(_sprite);
					delete _sprite->m_mc;
					_sprite->m_mc = _mc;
					_mc->m_parentObj = _sprite;
					_objLib->AddObj(_sprite);

					//Add to temporary sprite list
					lstTmpSpriteNoAutoDelete.push_back(_sprite);

					_ck1 = _fc1->AddCharacter(_sprite, _inst->m_matrix, gldCxform()/*_inst->m_cxform*/);
					_ck1->m_name = _inst->m_name;

					if (_fc2)
					{
						_ck2 = _fc2->AddCharacter(_sprite, _inst->m_matrix, gldCxform()/*_inst->m_cxform*/);
						_ck2->m_name = _inst->m_name;
					}
				}
				else
				{
					_fc1 = new gldFrameClip(nCurFrame, nSceneTime);

					if ((_inst->m_action.size() > 0 || !_inst->m_name.empty()) &&	// Has AS or has instance name
						!(_inst->m_obj->IsGObjInstanceOf(gobjButton) || _inst->m_obj->IsGObjInstanceOf(gobjSprite)))
					{
						gldSprite *_sprite = new gldSprite(_inst->m_obj, gldMatrix(), gldCxform());

						_objLib->AddObj(_sprite);

						//Add to temporary sprite list
						lstTmpSpriteAutoDelete.push_back(_sprite);

						_ck1 = _fc1->AddCharacter(_sprite, _inst->m_matrix, _inst->m_cxform);
					}
					else
					{
						_ck1 = _fc1->AddCharacter(_inst->m_obj, _inst->m_matrix, _inst->m_cxform);
					}

					_ck1->m_name = _inst->m_name;
				}

				if (_inst->m_action.size() > 0)
				{
					_ck1->m_action = new gldAction();
					if (_inst->m_mc == NULL && _inst->m_obj->IsGObjInstanceOf(gobjButton))
					{
						_ck1->m_action->m_type = actionSegButton;
					}
					else
					{
						_ck1->m_action->m_type = actionSegSprite;
						if (_ck2)
						{
							_ck2->m_action = new gldAction();
							_ck2->m_action->m_type = actionSegSprite;
						}
					}

					Regexx re;
					std::string sAction = re.replace(_inst->m_action, "(prevScene[ \t\r\n]*\\([ \t]*\\))", sPrevScene, Regexx::nocase | Regexx::dotall);					
					sAction = re.replace(sAction, "(nextScene[ \t\r\n]*\\([ \t]*\\))", sNextScene, Regexx::nocase | Regexx::dotall);					

					_ck1->m_action->SetSourceCode(sAction.c_str());
					if (_ck2)
						_ck2->m_action->SetSourceCode(sAction.c_str());
				}

				_layer->m_frameClipList.push_back(_fc1);
				if (_fc2)
					_layer->m_frameClipList.push_back(_fc2);

				_scene->AddLayer(_layer);
			}

			// process scene sounds
			GSCENESOUND_LIST::iterator sInst = _scene2->m_soundList.begin(); 
			for (; sInst != _scene2->m_soundList.end(); sInst++)
			{
				gldSceneSound *_sound = *sInst;

				if (_sound->m_startTime < nSceneTime)
				{
					gldLayer *_layer = new gldLayer("", 0);

					if (_sound->m_startTime > 0)
					{
						gldFrameClip *_fcEmpty = new gldFrameClip(0, nCurFrame + _sound->m_startTime);
						_layer->m_frameClipList.push_back(_fcEmpty);
					}

					gldFrameClip *_fc = new gldFrameClip(nCurFrame + _sound->m_startTime, min(nSceneTime - _sound->m_startTime, _sound->m_length));
					if (_fc->m_sound)
						delete _fc->m_sound;
					_fc->m_sound = _sound->m_sound->Clone();
					_layer->m_frameClipList.push_back(_fc);

					_scene->AddLayer(_layer);
				}
			}

			// background
			CBackground *backgrnd = _scene2->m_backgrnd;
			if (backgrnd != NULL)
			{
				gldLayer *layer = new gldLayer("", 0);
				gldFrameClip *fc = new gldFrameClip(nCurFrame, nSceneTime);
				fc->m_parentLayer = layer;
				layer->m_frameClipList.push_back(fc);
				_scene->AddLayer(layer);
				gldMovieClip *mc = backgrnd->GetMovieClip();
				gldMatrix mx = backgrnd->GetMatrix();
				gldCxform cx = backgrnd->GetCxform();
				gldObj *obj = backgrnd->GetObj();
				gldCharacterKey *key = NULL;			
				if (mc == NULL)
				{
					if (!(obj->IsGObjInstanceOf(gobjButton) || obj->IsGObjInstanceOf(gobjSprite))
						&& (backgrnd->GetName().size() > 0 || backgrnd->GetAction().size() > 0))
					{
						gldSprite *sprite = new gldSprite(obj, gldMatrix(), gldCxform());
						_objLib->AddObj(sprite);
						lstTmpSpriteAutoDelete.push_back(sprite);
						key = fc->AddCharacter(sprite, mx, cx);						
					}
					else
					{
						key = fc->AddCharacter(obj, mx, cx);
					}
				}
				else
				{
					gldSprite *sprite = new gldSprite;
					delete sprite->m_mc;
					sprite->m_mc = mc;
					mc->m_parentObj = sprite;
					_objLib->AddObj(sprite);
					lstTmpSpriteNoAutoDelete.push_back(sprite);
					key = fc->AddCharacter(sprite, mx, cx);
				}
				key->m_name = backgrnd->GetName();
				std::string &action = backgrnd->GetAction();
				if (action.size() > 0)
				{
					key->m_action = new gldAction();
					if (mc == NULL && obj->IsGObjInstanceOf(gobjButton))
					{
						key->m_action->m_type = actionSegButton;
					}
					else
					{
						key->m_action->m_type = actionSegSprite;						
					}
					Regexx re;
					std::string sAction = re.replace(action, "(prevScene[ \t\r\n]*\\([ \t]*\\))", sPrevScene, Regexx::nocase | Regexx::dotall);
					sAction = re.replace(sAction, "(nextScene[ \t\r\n]*\\([ \t]*\\))", sNextScene, Regexx::nocase | Regexx::dotall);
					key->m_action->SetSourceCode(sAction.c_str());					
				}
			}
			//nCurFrame += nMaxTime;
			nCurFrame += nSceneTime;
		}
		

		gldLayer *_layer = NULL;
		gldSprite *_gsprite = NULL;
		CI2CMap _newMap;
		CI2CMap *_oldMap;
		gldLibrary auxLib;

#ifndef _USB_VERSION_
		if (GetKeyType() == 0)
		{
			U16 _len = _scene->GetMaxTime();
			if (_len == 0)
				_len = 1;
			_layer = new gldLayer("Logo", _len);
			_scene->m_layerList.insert(_scene->m_layerList.begin(), _layer);
			gldFrameClip *_fc = _layer->GetFrameClip(0);

			// Load logo from .glm			
			CComPtr<IGLD_Object> pIObject;
			if (SUCCEEDED(::LoadObjectFromResource("GLM", MAKEINTRESOURCE(IDR_GLM_TRIAL), &pIObject)))
			{
				CComQIPtr<IGLD_Sprite> pISprite = pIObject;
				if (pISprite)
				{
					_oldMap = CI2C::m_pI2CMap;
					CI2C::m_pI2CMap = &_newMap;
					gldDataKeeper::Instance()->m_objLib = &auxLib;
					if (SUCCEEDED(CI2C::Create(pISprite, &_gsprite)))
					{
						CI2CMap::_I2CMap::iterator it = _newMap.m_Map.begin();
						for (; it != _newMap.m_Map.end(); ++it)
						{
							gldObj *pObj = (*it).second;
							_objLib->AddObj(pObj, false);
						}

						for (GOBJECT_LIST::const_iterator ito = auxLib.GetObjList().begin()
							; ito != auxLib.GetObjList().end(); ++ito)
						{
							gldObj *obj = *ito;
							_objLib->AddObj(obj);
						}

						CComPtr<IGLD_Rect> pIRect;
						pISprite->get_Bound(&pIRect);

						double _w = 0;
						double _h = 0;
						pIRect->get_Width(&_w);
						pIRect->get_Height(&_h);

						gldMatrix _mx;
						_mx.m_x = (S32)(10 * 20 + _w / 2);
						_mx.m_y = (S32)(5 * 20 + _h / 2);
						_fc->AddCharacter(_gsprite, _mx, gldCxform());
					}
					gldDataKeeper::Instance()->m_objLib = _objLib;
				}
			}
		}
#endif

		if (!CompileActionScript(NULL, pParam->bDebug))
		{
			COutputBar::Instance()->Show(TRUE);
			CString strError;
			strError.LoadString(IDS_ERROR_EXPORT_COMPILE_AS);
			if (pDlg->MessageBox(strError, 0, MB_ICONWARNING | MB_YESNO) != IDYES)
				nIDResult = IDCANCEL;
		}

		if (nIDResult != IDCANCEL)
		{
			CString strPrompt;
			strPrompt.LoadString(IDS_EXPORT_MOVIE_PROMPT);
			pDlg->SetPrompt(strPrompt);
			pDlg->SetProgress(0);

			const GOBJECT_LIST &_objList = _objLib->GetObjList();
			for(GOBJECT_LIST_CIT it = _objList.begin(); it != _objList.end(); it++)
			{
				CTransAdaptor::ConverTShapePtr(*it);
			}

			CSubject sub;
			CSubjectManager::Instance()->Register("CreateMovie", &sub);

			gldSWFEngine _engine;
			CSubjectManager::Instance()->GetSubject("CreateMovie")->Attach(pDlg);
			pDlg->SetCancelFlagPtr(_engine.GetCanceledFlagPtr());

			if (!_engine.CreateMovie(pParam->lpszFilePath, _scene
				, _mainMovie2->m_frameRate, _mainMovie2->m_color
				, _mainMovie2->m_width, _mainMovie2->m_height
				, pParam->bCompressed ? true : false))
			{
				if (!pDlg->IsCanceled())
					ReportLastError("Failed to create preview file: %s");
				nIDResult = IDCANCEL;
			}

			CSubjectManager::Instance()->UnRegister("CreateMovie");
		}

#ifndef _USB_VERSION_
		if (GetKeyType() == 0)
		{
			if (CI2C::m_pI2CMap == &_newMap)
				CI2C::m_pI2CMap = _oldMap;
			_newMap.FreeAllSymbol();

			_scene->m_layerList.erase(_scene->m_layerList.begin());
			delete _layer;
			//delete _gsprite;
		}
#endif

		delete _mainMovie;

		gldDataKeeper::Instance()->m_curMovieClip = NULL;
		gldDataKeeper::Instance()->m_objLib = _objLibOld;

		//_objLib->ClearAllObjID();
		// clear temporary library avoid destroy the objects
		_objLib->ClearList();
		delete _objLib;

		// destroy temporary sprites
		for (vector<gldSprite *>::iterator i = lstTmpSpriteNoAutoDelete.begin();
			i != lstTmpSpriteNoAutoDelete.end(); ++i)
		{
			(*i)->m_mc->m_parentObj = NULL;
			(*i)->m_mc = NULL;
			delete *i;
		}

		for (vector<gldSprite *>::iterator i = lstTmpSpriteAutoDelete.begin();
			i != lstTmpSpriteAutoDelete.end(); ++i)
		{
			delete *i;
		}

		//gldDataKeeper::ReleaseInstance();
	}

	ClearEffects();

	::CoUninitialize();

	pDlg->End(nIDResult);

	return 0;
}

BOOL ExportMovie(CWnd *pParentWnd, LPCTSTR lpszFilePath, BOOL bCompressed, BOOL bDebug)
{
	COutputBar::Instance()->m_pView->ClearLog();

	gldDataKeeper::Instance()->m_objLib->ClearAllObjID();

	// test file
	try
	{
		CFile file(lpszFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
		file.Close();
	}
	catch (CFileException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	// create movie
	CString strTitle, strPrompt;
	strTitle.LoadString(IDS_EXPORT_MOVIE_TITLE);
	strPrompt.LoadString(IDS_EXPORT_MOVIE_PROMPT);
	CDlgProgress dlg(pParentWnd, strTitle, strPrompt);

	CREATE_MOVIE_THREAD_PARAMETER param;
	param.pDlg = &dlg;
	param.lpszFilePath = lpszFilePath;
    param.bCompressed = bCompressed;
	param.bDebug = bDebug;

	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateMovieThread, (LPVOID)&param, 0, &dwThreadId);

	CWaitCursor wc;

	BOOL bResult = (dlg.DoModal() == IDOK);

	if (!bResult)
		::DeleteFile(lpszFilePath);

	return bResult;
}

BOOL GetCommandDesc(UINT nID, CString &strDesc)
{
	static HMODULE hInst = NULL;
	if (!hInst)
		hInst = ::LoadLibrary("CommandRes.dll");
	if (hInst)
	{
		if (!strDesc.LoadString(hInst, nID))
			strDesc.LoadString(hInst, IDS_CMD_NODESC);
		return TRUE;
	}
	return FALSE;
}

CString FormatCompilingError(int nFrame, gldCharacterKey *pCharacterKey, LPCTSTR lpszError)
{
	CString strError;
	if (_tcslen(lpszError) > 0)
	{
		gldScene2 *pScene = _GetMainMovie2()->GetSceneByFrameNumner(nFrame);
		if (pScene)
		{
			if (pCharacterKey)
			{
				CString strName = pCharacterKey->m_name.c_str();
				if (strName.IsEmpty())
					strName.LoadString(IDS_KEY_UNNAMED);
				CString strScene;
				strScene.LoadString(IDS_KEY_SCENE);
				CString strElement;
				strElement.LoadString(IDS_KEY_ELEMENT);
				strError.Format("%s \'%s\', %s \'%s\', %s", (LPCTSTR)strScene, pScene->m_name.c_str(), (LPCTSTR)strElement, (LPCTSTR)strName, lpszError);
			}
			else
			{
				CString strScene;
				strScene.LoadString(IDS_KEY_SCENE);
				strError.Format("%s \'%s\', %s", (LPCTSTR)strScene, pScene->m_name.c_str(), lpszError);
			}
		}
	}
	return strError;
}

bool CompileActionScript(int actionType, BOOL bDebug, const char *sourceCode, unsigned char **byteCode, int *byteCodeSize, CString &strError)
{
	if (g_hCompiler == NULL)
	{
		g_hCompiler = ::LoadLibrary("ascompiler.dll");
		if (g_hCompiler)
		{
			g_lpfnCompile = (PtrCompile)::GetProcAddress(g_hCompiler, "Compile");
			g_lpfnGetBinCode = (PtrGetBinCode)::GetProcAddress(g_hCompiler, "GetBinCode");
			g_lpfnPtrGetOutputMsg = (PtrGetOutputMsg)::GetProcAddress(g_hCompiler, "GetOutputMsg");
			g_lpfnFreeAllBuffers = (PtrFreeAllBuffers)::GetProcAddress(g_hCompiler, "FreeAllBuffers");
		}
	}
	if (g_hCompiler == NULL || 
		g_lpfnCompile == NULL || 
		g_lpfnGetBinCode == NULL || 
		g_lpfnPtrGetOutputMsg == NULL || 
		g_lpfnFreeAllBuffers == NULL)
	{
		strError.LoadString(IDS_ERROR_LOAD_COMPILER);
		return false;
	}

	int type;
	switch (actionType)
	{
		case actionSegInitSprite:
		case actionSegFrame:
			type = codeFrame;
			break;
		case actionSegSprite:
			type = codeMC;
			break;
		case actionSegButton:
			type = codeButton;
			break;
		default:
			ASSERT(FALSE);
			return false;
	}

	bool ret = (g_lpfnCompile(bDebug ? CF_DEBUG : 0, type, sourceCode) == 0);
	unsigned int nSize = 0;
	if (!ret)
	{
		LPCTSTR lpszMsg = g_lpfnPtrGetOutputMsg(&nSize);
		strError = lpszMsg;
	}
	else
	{
		const unsigned char *pBinCode = g_lpfnGetBinCode(&nSize);
		if (pBinCode && nSize > 0)
		{
			*byteCode = new unsigned char[nSize];
			if (byteCode == NULL)
			{
				ret = false;
			}
			else
			{
				memcpy(*byteCode, pBinCode, nSize);
				*byteCodeSize = nSize;
			}
		}
		else
		{
			*byteCode = NULL;
			*byteCodeSize = 0;
		}
	}

	g_lpfnFreeAllBuffers();

	return ret;
}

bool ShouldCompileActionScript(gldAction *pAction)
{
	if (pAction == NULL)
		return false;

	if (pAction->m_useCompiledActionBin)
	{
		TRACE1("*** Compiling action ***\n%s\n\n", pAction->GetSourceCode().c_str());
		return true;
	}
	else
	{
		return false;
	}
}

bool CompileActionScript(gldObj *pObj, BOOL bDebug)
{
	BOOL bErrorFound = FALSE;
	CString strError;

	gldMovieClip *pMC = NULL;
	BOOL bButton = FALSE;
	if (pObj == NULL)	// main movie
	{
		pMC = gldDataKeeper::Instance()->GetCurMovieClip();
	}
	else
	{
		GObjType type = pObj->GetGObjType();
		switch (type)
		{
			case gobjSprite:
			case gobjESprite:
				pMC = ((gldSprite *)pObj)->m_mc;
				break;

			case gobjButton:
				pMC = ((gldButton *)pObj)->m_bmc;
				bButton = TRUE;
				break;

			default:
				ASSERT(FALSE);
				break;
		}
	}

	if (pMC)
	{
		unsigned char *pBin;
		int nBinSize = 0;

		// #initclip action
		if (ShouldCompileActionScript(pMC->m_initAction))
		{
			if (!CompileActionScript(pMC->m_initAction->m_type, bDebug, pMC->m_initAction->GetSourceCode().c_str(), &pBin, &nBinSize, strError))
				bErrorFound = TRUE;
			else
				pMC->m_initAction->SetBinCode(pBin, nBinSize);
		}

		int nLayer = 0;
		for (GLAYER_LIST_IT itLayer = pMC->m_layerList.begin(); itLayer != pMC->m_layerList.end(); itLayer++, nLayer++)
		{
			gldLayer *pLayer = *itLayer;

			for (GFRAMECLIP_LIST_IT itFC = pLayer->m_frameClipList.begin(); itFC != pLayer->m_frameClipList.end(); itFC++)
			{
				gldFrameClip *pFC = *itFC;

				// frame action
				if (!bButton)
				{
					if (ShouldCompileActionScript(pFC->m_action))
					{
						if (!CompileActionScript(pFC->m_action->m_type, bDebug, pFC->m_action->GetSourceCode().c_str(), &pBin, &nBinSize, strError))
						{
							if (pObj == NULL)
							{
								CString strFullError = FormatCompilingError(pFC->m_startTime, NULL, strError);
								COutputBar::Instance()->m_pView->AppendLog(strFullError + "\r\n");
							}
							bErrorFound = TRUE;
						}
						else
							pFC->m_action->SetBinCode(pBin, nBinSize);
					}
				}

				// event action (on/onClipEvent)
				for (GCHARACTERKEY_LIST_IT itCK = pFC->m_characterKeyList.begin(); itCK != pFC->m_characterKeyList.end(); itCK++)
				{
					gldCharacterKey *pCK = *itCK;
					gldObj *pCKObj = pCK->GetObj();

					GObjType type = pCKObj->GetGObjType();
					if (type == gobjSprite || type == gobjButton)
					{
						if (ShouldCompileActionScript(pCK->m_action))
						{
							if (!CompileActionScript(pCK->m_action->m_type, bDebug, pCK->m_action->GetSourceCode().c_str(), &pBin, &nBinSize, strError))
							{
								if (pObj == NULL)
								{
									CString strFullError = FormatCompilingError(pFC->m_startTime, pCK, strError);
									COutputBar::Instance()->m_pView->AppendLog(strFullError + "\r\n");
								}
								bErrorFound = TRUE;
							}
							else
								pCK->m_action->SetBinCode(pBin, nBinSize);
						}

						// recursively compile actions in the symbol
						if (type != gobjESprite && pCKObj->m_id == 0)
						{
							pCKObj->m_id = 1;
							if (!CompileActionScript(pCKObj, bDebug))
								bErrorFound = TRUE;
						}
					}
				}
			}
		}
	}

	if (pObj == NULL)	// main movie
	{
		// compile export/import assets
		const GOBJECT_LIST &objList = gldDataKeeper::Instance()->m_objLib->GetObjList();

		for (GOBJECT_LIST_CIT it = objList.begin(); it != objList.end(); it++)
		{
			gldObj *pLibObj = *it;
			GObjType type = pLibObj->GetGObjType();
			if (type == gobjSprite || type == gobjESprite || type == gobjButton)
			{
				if (pLibObj->m_objFlag != gobjNormalFlag)
				{
					if (pLibObj->m_id == 0)
					{
						pLibObj->m_id = 1;
						if (!CompileActionScript(pLibObj, bDebug))
							bErrorFound = TRUE;
					}
				}
			}
		}
	}

	return !bErrorFound;
}
