#include "StdAfx.h"
#include ".\gldscene2.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "gldSound.h"
#include "glandaclipboard.h"
#include <algorithm>
#include "Background.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

gldScene2::gldScene2(void)
{
	m_specifiedFrames = -1;
	m_soundExpanded = TRUE;
	m_backgrnd = NULL;
}

gldScene2::~gldScene2(void)
{
	for (GSCENESOUND_LIST::iterator it = m_soundList.begin(); it != m_soundList.end(); it++)
		delete *it;
	m_soundList.clear();

	for (GINSTANCE_LIST::iterator it = m_instanceList.begin(); it != m_instanceList.end(); it++)
		delete *it;
	m_instanceList.clear();

	if (m_backgrnd != NULL)
		delete m_backgrnd;
}

gldInstance *gldScene2::GetInstance(int index)
{
	int count = 0;
	for (GINSTANCE_LIST::iterator it = m_instanceList.begin(); it != m_instanceList.end(); it++)
	{
		if (count == index)
			return *it;
		count++;
	}

	return NULL;
}

gldSceneSound *gldScene2::GetSound(int index)
{
	int count = 0;
	for (GSCENESOUND_LIST::iterator it = m_soundList.begin(); it != m_soundList.end(); it++)
	{
		if (count == index)
			return *it;
		count++;
	}

	return NULL;
}

int gldScene2::GetEffectCount()
{
	int count = 0;
	for (GINSTANCE_LIST::iterator it = m_instanceList.begin(); it != m_instanceList.end(); it++)
	{
		gldInstance *pInstance = *it;
		if (pInstance->m_effectList.size() > 0)
			count++;
	}

	return count;
}

int gldScene2::GetMaxTime(BOOL bIgnoreSpecifiedTimed)
{
	if (!bIgnoreSpecifiedTimed && m_specifiedFrames >= 0)
		return m_specifiedFrames;

	int nMaxTime = 0;
	for (GINSTANCE_LIST::iterator it = m_instanceList.begin(); it != m_instanceList.end(); it++)
	{
		gldInstance *pInst = *it;
		nMaxTime = max(nMaxTime , pInst->GetMaxTime());
	}

	if (nMaxTime == 0)
	{
		// if action or sound exists, the max time should be 1
		if (!m_actionEnter.empty() || !m_actionExit.empty() || m_soundList.size() > 0 || m_backgrnd != NULL)
			nMaxTime = 1;
	}

	return nMaxTime;
}

void gldScene2::GetUsedObjs(GOBJECT_LIST &lstObj)
{
	// instances
	GINSTANCE_LIST::iterator iti = m_instanceList.begin();
	for (; iti != m_instanceList.end(); ++iti)
	{				
		gldInstance *pInst = *iti;
		gldObj *pObj = pInst->m_obj;
		ASSERT(pObj != NULL);
		if (find(lstObj.begin(), lstObj.end(), pObj) == lstObj.end())
			lstObj.push_back(pObj);
	}
	// sounds
	GSCENESOUND_LIST::iterator its = m_soundList.begin();
	for (; its != m_soundList.end(); ++its)
	{
		gldSound *pSound = (*its)->m_sound->m_soundObj;
		ASSERT(pSound != NULL);
		if (find(lstObj.begin(), lstObj.end(), pSound) == lstObj.end())
			lstObj.push_back(pSound);
	}
	// background
	if (m_backgrnd != NULL)
	{
		gldObj *pObj = m_backgrnd->GetObj();
		if (find(lstObj.begin(), lstObj.end(), pObj) == lstObj.end())
			lstObj.push_back(pObj);
	}
}

HRESULT gldScene2::WriteToStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	
	HRESULT hr = stm.Write((WORD)3); // version
	CHECKRESULT();

	// auxiliary line
	hr = stm.Write((int)m_hLines.size());
	CHECKRESULT();
	vector<int>::iterator iti = m_hLines.begin();
	for (; iti != m_hLines.end(); ++iti)
	{
		hr = stm.Write(*iti);
		CHECKRESULT();
	}

	hr = stm.Write((int)m_vLines.size());
	CHECKRESULT();
	for (iti = m_vLines.begin(); iti != m_vLines.end(); ++iti)
	{
		hr = stm.Write(*iti);
		CHECKRESULT();
	}

	hr = stm.Write(m_name);
	CHECKRESULT();
	hr = stm.Write(m_actionEnter);
	CHECKRESULT();
	hr = stm.Write(m_actionExit);
	CHECKRESULT();

	hr = stm.Write((DWORD)m_instanceList.size());
	CHECKRESULT();
	for (GINSTANCE_LIST::iterator iti = m_instanceList.begin();
		iti != m_instanceList.end(); ++iti)
	{
		hr = (*iti)->WriteToStream(pStm, pUnk);
		CHECKRESULT();
	}

	hr = stm.Write((DWORD)m_soundList.size());
	CHECKRESULT();
	for (GSCENESOUND_LIST::iterator its = m_soundList.begin();
		its != m_soundList.end(); ++its)
	{
		hr = (*its)->WriteToStream(pStm, pUnk);
		CHECKRESULT();
	}
	
	hr = stm.Write(m_soundExpanded);
	CHECKRESULT();

	hr = stm.Write(m_specifiedFrames);
	CHECKRESULT();

	if (m_backgrnd != NULL)
	{
		char fb = 'B';
		hr = stm.Write(fb);
		CHECKRESULT();
		hr = m_backgrnd->WriteToStream(pStm, pUnk);
		CHECKRESULT();
	}
	else
	{
		char fb = 'U';
		hr = stm.Write(fb);
		CHECKRESULT();
	}

	return S_OK;
}

HRESULT gldScene2::ReadFromStream(IStream *pStm, void *pUnk)
{
	CReplaceLibrary xRplLib((gldLibrary *)pUnk);
	IStreamWrapper stm(pStm);

	WORD ver = 0;
	HRESULT hr = stm.Read(ver); // version
	CHECKRESULT();
	
	if (ver == 1)
	{
		hr = stm.Read(m_name);
		CHECKRESULT();
		hr = stm.Read(m_actionEnter);
		CHECKRESULT();
		hr = stm.Read(m_actionExit);
		CHECKRESULT();

		DWORD count = 0;
		hr = stm.Read(count);
		CHECKRESULT();
		for (; count > 0; count--)
		{
			gldInstance *pInst = new gldInstance;
			hr = pInst->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pInst;
				return hr;
			}
			m_instanceList.push_back(pInst);
		}

		hr = stm.Read(count);
		CHECKRESULT();
		for (; count > 0; count--)
		{		
			gldSceneSound *pSound = new gldSceneSound;
			hr = pSound->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pSound;
				return hr;
			}
			m_soundList.push_back(pSound);
		}
		
		hr = stm.Read(m_soundExpanded);
		CHECKRESULT();

		hr = stm.Read(m_specifiedFrames);
		CHECKRESULT();
	}
	else if (ver == 2)
	{
		// read auxilary
		int nAux = 0;
		hr = stm.Read(nAux);
		CHECKRESULT();
		m_hLines.clear();
		for (; nAux > 0; nAux--)
		{
			int v = 0;
			hr = stm.Read(v);
			CHECKRESULT();
			m_hLines.push_back(v);
		}
		hr = stm.Read(nAux);
		CHECKRESULT();
		m_vLines.clear();
		for (; nAux > 0; nAux--)
		{
			int v = 0;
			hr = stm.Read(v);
			CHECKRESULT();
			m_vLines.push_back(v);
		}

		hr = stm.Read(m_name);
		CHECKRESULT();
		hr = stm.Read(m_actionEnter);
		CHECKRESULT();
		hr = stm.Read(m_actionExit);
		CHECKRESULT();

		DWORD count = 0;
		hr = stm.Read(count);
		CHECKRESULT();
		for (; count > 0; count--)
		{
			gldInstance *pInst = new gldInstance;
			hr = pInst->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pInst;
				return hr;
			}
			m_instanceList.push_back(pInst);
		}

		hr = stm.Read(count);
		CHECKRESULT();
		for (; count > 0; count--)
		{		
			gldSceneSound *pSound = new gldSceneSound;
			hr = pSound->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pSound;
				return hr;
			}
			m_soundList.push_back(pSound);
		}

		hr = stm.Read(m_soundExpanded);
		CHECKRESULT();

		hr = stm.Read(m_specifiedFrames);
		CHECKRESULT();
	}
	else if (ver == 3)
	{
		// read auxilary
		int nAux = 0;
		hr = stm.Read(nAux);
		CHECKRESULT();
		m_hLines.clear();
		for (; nAux > 0; nAux--)
		{
			int v = 0;
			hr = stm.Read(v);
			CHECKRESULT();
			m_hLines.push_back(v);
		}
		hr = stm.Read(nAux);
		CHECKRESULT();
		m_vLines.clear();
		for (; nAux > 0; nAux--)
		{
			int v = 0;
			hr = stm.Read(v);
			CHECKRESULT();
			m_vLines.push_back(v);
		}

		hr = stm.Read(m_name);
		CHECKRESULT();
		hr = stm.Read(m_actionEnter);
		CHECKRESULT();
		hr = stm.Read(m_actionExit);
		CHECKRESULT();

		DWORD count = 0;
		hr = stm.Read(count);
		CHECKRESULT();
		for (; count > 0; count--)
		{
			gldInstance *pInst = new gldInstance;
			hr = pInst->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pInst;
				return hr;
			}
			m_instanceList.push_back(pInst);
		}

		hr = stm.Read(count);
		CHECKRESULT();
		for (; count > 0; count--)
		{		
			gldSceneSound *pSound = new gldSceneSound;
			hr = pSound->ReadFromStream(pStm, pUnk);
			if (FAILED(hr))
			{
				delete pSound;
				return hr;
			}
			m_soundList.push_back(pSound);
		}

		hr = stm.Read(m_soundExpanded);
		CHECKRESULT();

		hr = stm.Read(m_specifiedFrames);
		CHECKRESULT();

		char fb;
		hr = stm.Read(fb);
		CHECKRESULT();
		if (fb == 'B')
		{
			if (m_backgrnd == NULL)
			{	
				m_backgrnd = CBackground::FromStream(pStm, pUnk);
				if (m_backgrnd == NULL)
					return E_FAIL;
			}
			else
			{
				hr = m_backgrnd->ReadFromStream(pStm, pUnk);
				CHECKRESULT();
			}
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}