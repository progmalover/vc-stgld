#pragma once
#include "shockwaveflash.h"

class gldSound;


// CPlaySoundDialog dialog

class CPlaySoundDialog : public CDialog
{
	DECLARE_DYNAMIC(CPlaySoundDialog)

public:
	CPlaySoundDialog(gldSound* pSound, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlaySoundDialog();

// Dialog Data
	enum { IDD = IDD_PLAY_SOUND_DIALOG };
	enum { IDC_SOUND_PLAYER = 1001};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CShockwaveFlash		m_player;
	gldSound*			m_pSound;
	CString				m_strPreviewPathName;
	int					m_nPlayingFrame;

	virtual void OnSoundStop();
	virtual void OnSoundFrame(int nFrame);
	void FSCommandPlayer(LPCTSTR command, LPCTSTR args);

	DECLARE_EVENTSINK_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonStopSound();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
