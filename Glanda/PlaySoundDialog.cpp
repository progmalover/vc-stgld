// PlaySoundDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PlaySoundDialog.h"
#include "gldSound.h"
#include "gldSWFEngine.h"
#include "filepath.h"
#include "GlandaDoc.h"

// CPlaySoundDialog dialog

IMPLEMENT_DYNAMIC(CPlaySoundDialog, CDialog)
CPlaySoundDialog::CPlaySoundDialog(gldSound* pSound, CWnd* pParent /*=NULL*/)
	: CDialog(CPlaySoundDialog::IDD, pParent)
	, m_pSound(pSound)
	, m_nPlayingFrame(-1)
{
}

CPlaySoundDialog::~CPlaySoundDialog()
{
}

void CPlaySoundDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlaySoundDialog, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_STOP_SOUND, OnBnClickedButtonStopSound)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPlaySoundDialog, CDialog)
	ON_EVENT(CPlaySoundDialog, IDC_SOUND_PLAYER, 150, FSCommandPlayer, VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()


// CPlaySoundDialog message handlers

BOOL CPlaySoundDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_pSound->m_name.c_str());

	m_player.Create(_afxWnd, WS_CHILD, CRect(0,0,0,0), this, IDC_SOUND_PLAYER);
	m_player.SetLoop(FALSE);

	m_strPreviewPathName = GetTempFile(NULL, "~sound", "swf");
	{
		CWaitCursor wc;
		gldSWFEngine _engine;
		_engine.CreateStreamSoundPreview(m_strPreviewPathName, m_pSound, _GetMainMovie2()->m_frameRate);
		m_player.SetMovie(m_strPreviewPathName);
	}
	m_player.Play();

	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SOUND_SLIDER);
	int nFrames = m_player.GetTotalFrames();
	pSliderCtrl->SetRange(0, nFrames - 1);

	return TRUE;
}

void CPlaySoundDialog::OnSoundStop()
{
	OnOK();
}

void CPlaySoundDialog::OnDestroy()
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
	m_player.DestroyWindow();

	CDialog::OnDestroy();
}

void CPlaySoundDialog::OnBnClickedButtonStopSound()
{
	OnOK();
}

void CPlaySoundDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SOUND_SLIDER);

	if((CScrollBar*)pSliderCtrl==pScrollBar)
	{
		switch(nSBCode)
		{
		case TB_BOTTOM:
			TRACE("Sound SliderCtrl:\tTB_BOTTOM\r\n");
		case TB_TOP:
			TRACE("Sound SliderCtrl:\tTB_TOP\r\n");
		case TB_PAGEDOWN:
			TRACE("Sound SliderCtrl:\tTB_PAGEDOWN\r\n");
		case TB_PAGEUP:
			TRACE("Sound SliderCtrl:\tTB_PAGEUP\r\n");
		case TB_LINEDOWN:
			TRACE("Sound SliderCtrl:\tTB_LINEDOWN\r\n");
		case TB_LINEUP:
			TRACE("Sound SliderCtrl:\tTB_LINEUP\r\n");
		case TB_THUMBTRACK:
			TRACE("Sound SliderCtrl:\tTB_THUMBTRACK\r\n");
		case TB_THUMBPOSITION:
			TRACE("Sound SliderCtrl:\tTB_THUMBPOSITION\r\n");
			m_player.StopPlay();
			break;

		case TB_ENDTRACK:
			TRACE("Sound SliderCtrl:\tTB_ENDTRACK\r\n");
			{
				int nFrame = pSliderCtrl->GetPos() + 1;
				m_player.GotoFrame(nFrame);
				m_player.Play();
			}
			break;

		default:
			TRACE("Sound SliderCtrl:\tdefault\r\n");
			break;
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlaySoundDialog::OnSoundFrame(int nFrame)
{
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SOUND_SLIDER);
	pSliderCtrl->SetPos(nFrame);
}

void CPlaySoundDialog::FSCommandPlayer(LPCTSTR command, LPCTSTR args)
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
				OnSoundStop();
				return ;
			}
		}
		OnSoundFrame(m_nPlayingFrame);
	}
}