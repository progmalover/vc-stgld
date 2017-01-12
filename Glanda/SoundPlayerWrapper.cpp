// SoundPlayerWrapper.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "SoundPlayerWrapper.h"
#include "Global.h"
#include "gldswfengine.h"
#include "filepath.h"
#include "GlandaDoc.h"

// CSoundPlayerWrapper

IMPLEMENT_DYNAMIC(CSoundPlayerWrapper, CWnd)
CSoundPlayerWrapper::CSoundPlayerWrapper()
: m_nPlayingFrame(-1)
, m_pListener(NULL)
{
}

CSoundPlayerWrapper::~CSoundPlayerWrapper()
{
}


BEGIN_MESSAGE_MAP(CSoundPlayerWrapper, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CSoundPlayerWrapper, CWnd)
	ON_EVENT(CSoundPlayerWrapper, IDC_SOUND_PLAYER, 150, FSCommandPlayer, VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()

// CSoundPlayerWrapper message handlers

BOOL CSoundPlayerWrapper::BindTo(CWnd* pWnd, CSoundPlayerWrapperListener* pListener, UINT nID /* = 1001 */)
{
	if(Create(_afxWnd, "", WS_CHILD, CRect(0, 0, 0, 0), pWnd, nID))
	{
		m_pListener = pListener;
		return TRUE;
	}
	return FALSE;
}

void CSoundPlayerWrapper::Unbind()
{
	DestroyWindow();
}

int CSoundPlayerWrapper::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_player.Create(_afxWnd, WS_CHILD, CRect(0,0,0,0), this, IDC_SOUND_PLAYER);

	return 0;
}

void CSoundPlayerWrapper::FSCommandPlayer(LPCTSTR command, LPCTSTR args)
{
	if(_tcscmp(command, "OnFrameChanged")==0)
	{
		if(m_nPlayingFrame==-1 && m_player.GetTotalFrames() > 1)
		{
			m_nPlayingFrame = atoi(args) - 1;
		}
		else
		{
			m_nPlayingFrame = atoi(args) - 1;
			if(m_nPlayingFrame == 0)
			{
				m_player.Stop();
				m_nPlayingFrame = -1;
				if(m_pListener)
				{
					m_pListener->OnSoundStop();
				}
			}
		}
		if(m_pListener)
		{
			m_pListener->OnSoundFrame(m_nPlayingFrame);
		}
	}
}

BOOL CSoundPlayerWrapper::Play(gldSound* pSound)
{
	Stop();

	m_strPreviewPathName = GetTempFile(NULL, "~sound", "swf");
	{
		CWaitCursor wc;
		gldSWFEngine _engine;
		_engine.CreateStreamSoundPreview(m_strPreviewPathName, pSound, _GetMainMovie2()->m_frameRate);
		m_player.SetMovie(m_strPreviewPathName);
	}
	m_player.Play();

#ifdef _DEBUG
	{
		CString strMovie = m_player.GetMovie();
		ASSERT(strMovie == m_strPreviewPathName);
	}
#endif

	return TRUE;
}

void CSoundPlayerWrapper::Stop()
{
	m_player.Stop();
	m_player.SetMovie("1");
	m_nPlayingFrame = -1;

	if(m_strPreviewPathName.IsEmpty()==FALSE)
	{
		if(::PathFileExists(m_strPreviewPathName))
		{
			::DeleteFile(m_strPreviewPathName);
		}
		m_strPreviewPathName.Empty();
	}
}

BOOL CSoundPlayerWrapper::IsPlaying()
{
	return m_nPlayingFrame != -1;
}

void CSoundPlayerWrapper::OnDestroy()
{
	Stop();
	m_pListener = NULL;
	m_player.DestroyWindow();

	CWnd::OnDestroy();
}

void CSoundPlayerWrapper::Pause()
{
	m_player.StopPlay();
}

void CSoundPlayerWrapper::gotoAndPlay(int nFrame)
{
	m_player.GotoFrame(nFrame);
	m_player.Play();
}

long CSoundPlayerWrapper::GetTotalFrames()
{
	return m_player.GetTotalFrames();
}