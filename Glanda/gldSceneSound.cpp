#include "StdAfx.h"
#include "gldscenesound.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "gldSound.h"
#include "SWFStream.h"
#include "GlandaDoc.h"
#include "Resource.h"

gldSceneSound::gldSceneSound(void)
{
	m_sound = new gldSoundInstance();

	m_sound->m_soundType = soundEvent;
	m_sound->m_loopFlag = false;
	m_sound->m_loopTimes = 0;

	m_sound->m_info.m_flag = 0;
	m_sound->m_info.m_loopCount = 0;

	m_startTime = 0;
	m_length = -1;
}

gldSceneSound::~gldSceneSound(void)
{
	delete m_sound;
}

HRESULT gldSceneSound::WriteToStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	
	HRESULT hr;
	hr = stm.Write((WORD)1); //version
	CHECKRESULT();
	hr = stm.Write(m_name);
	CHECKRESULT();
	hr = stm.Write(m_startTime);
	CHECKRESULT();
	hr = stm.Write(m_length);
	CHECKRESULT();
	oBinStream os;
	m_sound->WriteToBinStream(os);
	hr = ::WriteToStream(os, pStm);
	CHECKRESULT();

	return S_OK;
}

HRESULT gldSceneSound::ReadFromStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	
	HRESULT hr;
	WORD ver = 0;
	hr = stm.Read(ver); //version
	CHECKRESULT();
	
	if (ver == 1)
	{
		hr = stm.Read(m_name);
		CHECKRESULT();
		hr = stm.Read(m_startTime);
		CHECKRESULT();
		hr = stm.Read(m_length);
		CHECKRESULT();
		iBinStream is;
		hr = ::ReadFromStream(pStm, is);
		CHECKRESULT();
		m_sound->ReadFromBinStream(is);
	}

	return S_OK;
}

void gldSceneSound::GetToolTipText(CString& strText)
{
	float frameRate = _GetMainMovie2()->m_frameRate;
	strText.Format(IDS_SCENE_SOUND_TOOLTIPS_s_d_f_d_f,
		m_sound->m_soundObj->m_name.c_str(),
		m_startTime + 1, 
		(float)m_startTime / frameRate, 
		m_length, 
		(float)m_length / frameRate);
}
