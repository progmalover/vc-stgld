#include "StdAfx.h"
#include "gldmoviesound.h"
#include "gldSound.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "SWFStream.h"

gldMovieSound::gldMovieSound()
{
	m_sound = new gldSoundInstance();

	m_sound->m_soundType = soundStream;
	m_sound->m_loopFlag = false;
	m_sound->m_loopTimes = 0;

	m_sound->m_info.m_flag = 0;
	m_sound->m_info.m_loopCount = 0;
}

gldMovieSound::~gldMovieSound(void)
{
	delete m_sound;
}

HRESULT gldMovieSound::WriteToStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);

	HRESULT hr;
	hr = stm.Write((WORD)1); //version
	CHECKRESULT();
	hr = stm.Write(m_name);
	CHECKRESULT();
	hr = stm.Write(m_pathname);
	CHECKRESULT();
	hr = stm.Write(m_startScene);
	CHECKRESULT();

	oBinStream os;
	m_sound->WriteToBinStream(os);
	hr = ::WriteToStream(os, pStm);
	CHECKRESULT();

	return S_OK;
}

HRESULT gldMovieSound::ReadFromStream(IStream *pStm, void *pUnk)
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
		hr = stm.Read(m_pathname);
		CHECKRESULT();
		hr = stm.Read(m_startScene);
		CHECKRESULT();
		iBinStream is;
		hr = ::ReadFromStream(pStm, is);
		CHECKRESULT();
		m_sound->ReadFromBinStream(is);
	}

	return S_OK;
}
