#pragma once
#include "gldSound.h"
#include "shockwaveflash.h"

// CSoundPlayerWrapper


class CSoundPlayerWrapperListener
{
public:
	virtual void OnSoundStop() = 0;
	virtual void OnSoundFrame(int nFrame) = 0;
};

class CSoundPlayerWrapper : public CWnd
{
	DECLARE_DYNAMIC(CSoundPlayerWrapper)

	enum { IDC_SOUND_PLAYER = 1001};
public:
	CSoundPlayerWrapper();
	virtual ~CSoundPlayerWrapper();

	BOOL BindTo(CWnd* pWnd, CSoundPlayerWrapperListener* pListener, UINT nID = 1001);
	void Unbind();
	void Stop();
	BOOL Play(gldSound* pSound);
	BOOL IsPlaying();
	void Pause();
	void gotoAndPlay(int nFrame);
	long GetTotalFrames();
protected:
	DECLARE_MESSAGE_MAP()

	CShockwaveFlash	m_player;
	gldSound*		m_pSoundPlaying;
	CString			m_strPreviewPathName;
	int				m_nPlayingFrame;
	CSoundPlayerWrapperListener* m_pListener;
	void FSCommandPlayer(LPCTSTR command, LPCTSTR args);

	DECLARE_EVENTSINK_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
};
