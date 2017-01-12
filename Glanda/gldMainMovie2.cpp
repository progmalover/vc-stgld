#include "StdAfx.h"
#include "gldmainmovie2.h"
#include "observer.h"

#include "my_app.h"
#include "GlandaDoc.h"
#include "SWFStream.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "gldMovieSound.h"
#include "gldSound.h"
#include "GlandaClipboard.h"
#include "ProgressStatusBar.h"
#include "Selection.h"
#include "TransAdaptor.h"
#include "Background.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

gldMainMovie2::gldMainMovie2(void)
{
	m_width = 400;
	m_height = 300;
	m_color = 0xffffffff;
	m_frameRate = 12;

	m_pCurScene = NULL;

	m_pMovieSound = NULL;
}

gldMainMovie2::~gldMainMovie2(void)
{
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
		delete *it;
	m_sceneList.clear();

	if(m_pMovieSound)
	{
		delete m_pMovieSound;
		m_pMovieSound = NULL;
	}
}

void gldMainMovie2::SetCurScene(gldScene2 *pScene, BOOL bNotify)
{
	if(m_pCurScene!=pScene)
	{
		m_pCurScene = pScene;
		if(bNotify)
		{
			CSubjectManager::Instance()->GetSubject("ChangeCurrentScene2")->Notify(0);
		}
	}
}

gldScene2 *gldMainMovie2::GetScene(int index)
{
	int count = 0;
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
	{
		if (count == index)
			return *it;
		count++;
	}

	return NULL;
}

int gldMainMovie2::GetSceneIndex(gldScene2 *pScene)
{
	int count = 0;
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
	{
		if (*it == pScene)
			return count;
		count++;
	}

	return -1;
}

BOOL gldMainMovie2::SetCurScene(int index, BOOL bNotify)
{
	gldScene2 *pScene = GetScene(index);
	if (pScene)
	{
		SetCurScene(pScene, bNotify);
		return TRUE;
	}

	return FALSE;
}

gldScene2 *gldMainMovie2::FindSceneByName(const CString &name)
{
	for (GSCENE2_LIST::iterator its = m_sceneList.begin(); its != m_sceneList.end(); ++its)
	{
		gldScene2 *pScene = *its;
		if (name.CompareNoCase(pScene->m_name.c_str()) == 0)
		{
			return pScene;
		}
	}
	return NULL;
}

int gldMainMovie2::FindSceneIndexByName(const CString &name)
{
	int i = 0;
	for (GSCENE2_LIST::iterator its = m_sceneList.begin(); its != m_sceneList.end(); ++its, i++)
	{
		gldScene2 *pScene = *its;
		if (name.CompareNoCase(pScene->m_name.c_str()) == 0)
		{
			return i;
		}
	}
	return -1;
}

int gldMainMovie2::GetMaxTime()
{
	int nFrames = 0;
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
	{
		gldScene2 *pScene = *it;
		nFrames += pScene->GetMaxTime(FALSE);
	}
	return nFrames;
}

int gldMainMovie2::GetSceneStartTime(gldScene2 *pScene)
{
	int nFrames = 0;
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
	{
		gldScene2 *pTempScene = *it;
		if (pTempScene == pScene)
			return nFrames;

		nFrames += pTempScene->GetMaxTime(FALSE);
	}
	ASSERT(FALSE);
	return -1;
}

gldScene2 *gldMainMovie2::GetSceneByFrameNumner(int nFrame)
{
	int nFrames = 0;
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
	{
		gldScene2 *pScene = *it;
		int nSceneTime = pScene->GetMaxTime(FALSE);
		if (nFrame >= nFrames && nFrame < nFrames + nSceneTime)
			return pScene;

		nFrames += nSceneTime;
	}
	ASSERT(FALSE);
	return NULL;
}

int gldMainMovie2::GetEffectCount()
{
	int count = 0;
	for (GSCENE2_LIST::iterator it = m_sceneList.begin(); it != m_sceneList.end(); it++)
	{
		gldScene2 *pScene = *it;
		count += pScene->GetEffectCount();
	}
	return count;
}

#define STGM_RWEC (STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)

HRESULT gldMainMovie2::Save(IStorage *pStg, void *pUnk)
{	
	HRESULT hr;

	// save scene list to "Scenes" storage
	CComPtr<IStorage> pStgScenes;
	hr = pStg->CreateStorage(L"Scenes", STGM_RWEC, 0, 0, &pStgScenes);
	CHECKRESULT();
	GSCENE2_LIST::iterator its = m_sceneList.begin();
	int index = 1;
	for (; its != m_sceneList.end(); ++its, ++index)
	{
		OLECHAR stmName[10];
		swprintf(stmName, L"S%d", index);
		CComPtr<IStream> pStmScene;
		hr = pStgScenes->CreateStream(stmName, STGM_RWEC, 0, 0, &pStmScene);
		CHECKRESULT();
		hr = (*its)->WriteToStream(pStmScene, pUnk);
		CHECKRESULT();

		CProgressStatusBar::Instance()->SetPos(70 + index * 30 / m_sceneList.size());
	}
	pStgScenes.Release();


	// save movie sound to MovieSound storage
	if(m_pMovieSound)
	{
		CComPtr<IStorage> pStgMovieSound;
		hr = pStg->CreateStorage(L"MovieSound", STGM_RWEC, 0, 0, &pStgMovieSound);
		CHECKRESULT();

		CComPtr<IStream> pStmMovieSound;
		hr = pStgMovieSound->CreateStream(L"Sound", STGM_RWEC, 0, 0, &pStmMovieSound);
		CHECKRESULT();
		hr = m_pMovieSound->WriteToStream(pStmMovieSound, pUnk);
		CHECKRESULT();

		pStgMovieSound.Release();
	}

	// save main movie properties to "Prop" stream
	CComPtr<IStream> pStmProp;
	hr = pStg->CreateStream(L"Prop", STGM_RWEC, 0, 0, &pStmProp);
	CHECKRESULT();
	IStreamWrapper stmProp(pStmProp);
	hr = stmProp.Write((WORD)1); // version
	CHECKRESULT();
	hr = stmProp.Write(m_width);
	CHECKRESULT();
	hr = stmProp.Write(m_height);
	CHECKRESULT();
	hr = stmProp.Write(m_frameRate);
	CHECKRESULT();
	hr = stmProp.Write(m_color);
	CHECKRESULT();

	hr = stmProp.Write((DWORD)m_sceneList.size());
	CHECKRESULT();
	DWORD curScene = 0;	
	for (its = m_sceneList.begin(); its != m_sceneList.end(); ++its, ++curScene)
	{
		if (*its == m_pCurScene)
			break;
	}
	hr = stmProp.Write(curScene);
	CHECKRESULT();

	return S_OK;
}

void gldMainMovie2::GetUsedObjs(GOBJECT_LIST &lstObj)
{
	if (m_pMovieSound != NULL && m_pMovieSound->m_sound != NULL)
	{
		ASSERT(m_pMovieSound->m_sound->m_soundObj != NULL);
		AddObjAndRefs(m_pMovieSound->m_sound->m_soundObj, lstObj);
	}

	GSCENE2_LIST::iterator its = m_sceneList.begin();
	for (; its != m_sceneList.end(); ++its)
	{
		gldScene2 *scene2 = *its;
		GSCENESOUND_LIST::iterator itu = scene2->m_soundList.begin();
		for (; itu != scene2->m_soundList.end(); ++itu)
		{
			AddObjAndRefs((*itu)->m_sound->m_soundObj, lstObj);
		}
		GINSTANCE_LIST::iterator iti = scene2->m_instanceList.begin();
		for (; iti != scene2->m_instanceList.end(); ++iti)
		{
			AddObjAndRefs((*iti)->m_obj, lstObj);
		}
		if (scene2->m_backgrnd != NULL)
		{
			AddObjAndRefs(scene2->m_backgrnd->GetObj(), lstObj);
		}
	}	
}

HRESULT gldMainMovie2::Load(IStorage *pStg, void *pUnk)
{
	HRESULT hr;

	// save main movie properties to "Prop" stream
	CComPtr<IStream> pStmProp;
	hr = pStg->OpenStream(L"Prop", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStmProp);
	CHECKRESULT();
	IStreamWrapper stmProp(pStmProp);
	WORD ver = 0;
	hr = stmProp.Read(ver); // version
	CHECKRESULT();
	DWORD sceneCount = 0;
	DWORD curScene = 0;

	if (ver == 1)
	{
		hr = stmProp.Read(m_width);
		CHECKRESULT();
		hr = stmProp.Read(m_height);
		CHECKRESULT();
		hr = stmProp.Read(m_frameRate);
		CHECKRESULT();
		hr = stmProp.Read(m_color);
		CHECKRESULT();

		hr = stmProp.Read(sceneCount);
		CHECKRESULT();		
		hr = stmProp.Read(curScene);
		CHECKRESULT();
	}
	else
	{
		hr = E_FAIL;
	}
	CHECKRESULT();
	pStmProp.Release();


	// load scene list from "Scenes" storage
	CComPtr<IStorage> pStgScenes;
	hr = pStg->OpenStorage(L"Scenes", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgScenes);
	CHECKRESULT();
	for (DWORD i = 0; i < sceneCount; i++)
	{
		OLECHAR stmName[10];
		swprintf(stmName, L"S%d", i + 1);
		CComPtr<IStream> pStmScene;
		hr = pStgScenes->OpenStream(stmName, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStmScene);
		CHECKRESULT();
		CAutoPtr<gldScene2> scene(new gldScene2);
		hr = scene->ReadFromStream(pStmScene, pUnk);
		CHECKRESULT();
		m_sceneList.push_back(scene.Detach());

		CProgressStatusBar::Instance()->SetPos(70 + (i + 1)* 30 / sceneCount);
	}
	pStgScenes.Release();


	// load movie sound from "MovieSound" storage
	CComPtr<IStorage> pStgMovieSound;
	hr = pStg->OpenStorage(L"MovieSound", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgMovieSound);
	if(SUCCEEDED(hr))
	{
		CComPtr<IStream> pStmMovieSound;
		hr = pStgMovieSound->OpenStream(L"Sound", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStmMovieSound);
		CHECKRESULT();

		CAutoPtr<gldMovieSound> movieSound(new gldMovieSound);
		hr = movieSound->ReadFromStream(pStmMovieSound, pUnk);
		CHECKRESULT();
		m_pMovieSound = movieSound.Detach();
	}
	else if(hr != STG_E_FILENOTFOUND)
	{
		CHECKRESULT();
	}

	if (curScene >= 0 && curScene < sceneCount)
	{
		GSCENE2_LIST::iterator its = m_sceneList.begin();
		for (; curScene > 0; curScene--)
			++its;
		m_pCurScene = *its;
	}
	else
	{
		m_pCurScene = NULL;
	}

	return S_OK;
}

CCmdChangeMovieProperties::CCmdChangeMovieProperties(float fFrameRate, int nWidth, int nHeight, COLORREF color)
{
	_M_Desc_ID = IDS_CMD_CHANGEMOVIEPROPERTIES;

	m_fFrameRate = fFrameRate;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_color = color;
}

CCmdChangeMovieProperties::~CCmdChangeMovieProperties()
{
}

bool CCmdChangeMovieProperties::Execute()
{
	CGuardSelKeeper xSel;
	
	gldMainMovie2 *pMovie = _GetMainMovie2();
	ASSERT(pMovie);

	m_fFrameRateOld = pMovie->m_frameRate;
	m_nWidthOld = pMovie->m_width;
	m_nHeightOld = pMovie->m_height;
	m_colorOld = pMovie->m_color;

	pMovie->m_frameRate = m_fFrameRate;

	pMovie->m_width = m_nWidth;
	pMovie->m_height = m_nHeight;
	CSize size(m_nWidth, m_nHeight);
	my_app.SetMovieSize(size, false);

	pMovie->m_color = m_color;
	my_app.SetMovieBackground(m_color, false);

	CTransAdaptor::RebuildCurrentScene(true);

	my_app.Redraw();
	my_app.Repaint();

	CSubjectManager::Instance()->GetSubject("ChangeMovieProperties")->Notify(0);

	return TCommand::Execute();
}

bool CCmdChangeMovieProperties::Unexecute()
{
	CGuardSelKeeper xSel;

	gldMainMovie2 *pMovie = _GetMainMovie2();
	ASSERT(pMovie);

	pMovie->m_frameRate = m_fFrameRateOld;

	pMovie->m_width = m_nWidthOld;
	pMovie->m_height = m_nHeightOld;
	CSize size(m_nWidthOld, m_nHeightOld);
	my_app.SetMovieSize(size, false);

	pMovie->m_color = m_colorOld;
	my_app.SetMovieBackground(m_colorOld, true);

	CTransAdaptor::RebuildCurrentScene(true);

	my_app.Redraw();
	my_app.Repaint();

	CSubjectManager::Instance()->GetSubject("ChangeMovieProperties")->Notify(0);

	return TCommand::Unexecute();
}
