// DlgPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgPreview.h"
#include "Global.h"

#include "gldDataKeeper.h"
#include "GlandaDoc.h"

#include "OutputBar.h"
#include "OutputView.h"
#include "Options.h"
#include "BalloonToolTip.h"
#include "armadillo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgPreview dialog
IMPLEMENT_SINGLETON(CDlgPreview)
CDlgPreview::CDlgPreview(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgPreview::IDD, pParent), 
	m_hIcon(NULL)
{
	m_hAccel = NULL;

	ResetWindowPos();
}

CDlgPreview::~CDlgPreview()
{
	if (m_hIcon)
		::DestroyIcon(m_hIcon);
}

void CDlgPreview::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYER, m_player);
}


BEGIN_MESSAGE_MAP(CDlgPreview, CResizableDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_PREVIEW_ESCAPE, OnPreviewEscape)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CDlgPreview message handlers

void CDlgPreview::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (::IsWindow(m_player.m_hWnd))
	{
		m_player.MoveWindow(0, 0, cx, cy);
	}
}

void CDlgPreview::ResetWindowPos()
{	
	m_ptPreviewPos.x = SHRT_MAX;
	m_ptPreviewPos.y = SHRT_MAX;
}

void CDlgPreview::SaveWindowPos()
{
	WINDOWPLACEMENT wp;
	memset(&wp, 0, sizeof(wp));
	GetWindowPlacement(&wp);

	CRect rc = wp.rcNormalPosition;
	if ((GetStyle() & WS_CAPTION) == 0)
		::AdjustWindowRectEx(&rc, WS_CAPTION | WS_THICKFRAME, FALSE, 0);

	m_ptPreviewPos.x = rc.left;
	m_ptPreviewPos.y = rc.top;
}

void CDlgPreview::LoadWindowPos()
{
	CRect rc;
	GetWindowRect(&rc);

	if (m_ptPreviewPos.x != SHRT_MAX || m_ptPreviewPos.y != SHRT_MAX)
	{
		rc.OffsetRect(m_ptPreviewPos.x - rc.left, m_ptPreviewPos.y - rc.top);
		EnsureWholeRectVisible(rc);
		SetWindowPos(0, rc.left, rc.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}

BOOL CDlgPreview::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_hIcon = ::AfxGetApp()->LoadIcon(IDI_PREVIEW);
	SetIcon(m_hIcon, FALSE);

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_PREVIEW));

	// display progress
	gldMainMovie2 *_mainMovie = _GetMainMovie2();

	CRect rc(0, 0, _mainMovie->m_width, _mainMovie->m_height);
	CalcWindowRect(&rc, TRUE);
	SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOMOVE);
	CenterWindow(AfxGetMainWnd());

	LoadWindowPos();

	// show player
	m_player.Zoom(0);
	m_player.SetMovie("1");
	m_player.SetMovie(m_strFile);

	m_bFullScreen = FALSE;
	m_bOutputShown = FALSE;

#ifndef _USB_VERSION_
	if (IsTrialKey())
	{
		GetClientRect(&rc);
		ClientToScreen(&rc);
		CBalloonToolTip::Instance()->Show(rc.left + 30, rc.top + 8, IDS_TIP_TRIAL_WATERMARK);
	}
#endif

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPreview::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

BEGIN_EVENTSINK_MAP(CDlgPreview, CResizableDialog)
	ON_EVENT(CDlgPreview, IDC_PLAYER, DISPID_READYSTATECHANGE, OnReadyStateChangePlayer, VTS_I4)
	ON_EVENT(CDlgPreview, IDC_PLAYER, 150, FSCommandPlayer, VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()

void CDlgPreview::OnReadyStateChangePlayer(long newState)
{
	// TODO: Add your message handler code here
}

void CDlgPreview::ShowCaptionAndBorder(BOOL bShow)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	ClientToScreen(&rcClient);

	CRect rcWin = rcClient;
	::AdjustWindowRectEx(&rcWin, WS_CAPTION | WS_THICKFRAME, FALSE, 0);

	if (GetStyle() & WS_CAPTION)
	{
		if (!bShow)
		{
			ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0, 0);
			SetWindowPos(0, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);

			KillTimer(1);
			SetTimer(1, 100, NULL);
		}
	}
	else
	{
		if (bShow)
		{
			ModifyStyle(0, WS_CAPTION | WS_THICKFRAME, 0);
			SetWindowPos(0, rcWin.left, rcWin.top, rcWin.Width(), rcWin.Height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);

			KillTimer(1);
			SetTimer(1, 1000, NULL);
		}
	}
}

void CDlgPreview::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1)
	{
		if (COptions::Instance()->m_bAutoHideCaption)
		{
			if ((GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)) == 0 && !m_bFullScreen)
			{
				CRect rcClient;
				GetClientRect(&rcClient);
				ClientToScreen(&rcClient);

				CRect rcWin = rcClient;
				::AdjustWindowRectEx(&rcWin, WS_CAPTION | WS_THICKFRAME, FALSE, 0);

				CPoint point;
				GetCursorPos(&point);

				BOOL bInClient = (rcClient.PtInRect(point));
				BOOL bInWin = (rcWin.PtInRect(point));

				ShowCaptionAndBorder(bInWin && !bInClient);
			}
		}
		else if (!m_bFullScreen)
		{
			ShowCaptionAndBorder(TRUE);
		}
	}

	CResizableDialog::OnTimer(nIDEvent);
}

void CDlgPreview::FSCommandPlayer(LPCTSTR command, LPCTSTR args)
{
	// TODO: Add your message handler code here

	if (_tcsicmp(command, "fullscreen") == 0)
	{
		int nArgs = atoi(args);
		if (_tcsicmp(args, "true") == 0 || nArgs != 0)
		{
			if (!m_bFullScreen)
			{
				GetWindowRect(m_rcNormal);
				ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0, SWP_FRAMECHANGED);
				SetWindowPos(0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);

				m_bFullScreen = TRUE;

#ifndef _USB_VERSION_
				if (IsTrialKey())
					CBalloonToolTip::Instance()->ShowWindow(SW_HIDE);
#endif
			}
		}
		else
		if (_tcsicmp(args, "false") == 0 || nArgs == 0)
		{
			if (m_bFullScreen)
			{
				ModifyStyle(0, WS_CAPTION | WS_THICKFRAME, SWP_FRAMECHANGED);
				SetWindowPos(0, m_rcNormal.left, m_rcNormal.top, m_rcNormal.Width(), m_rcNormal.Height(), SWP_NOZORDER);

				m_bFullScreen = FALSE;
			}
		}
		return;
	}
	else if (_tcsicmp(command, "allowscale") == 0)
	{
		int nArgs = atoi(args);
		if (_tcsicmp(args, "true") == 0 || nArgs != 0)
		{
			m_player.SetScaleMode(0);
		}
		else
		if (_tcsicmp(args, "false") == 0 || nArgs == 0)
		{
			m_player.SetScaleMode(3);	//
		}
		return;
	}
	else if (_tcsicmp(command, "showmenu") == 0)
	{
		int nArgs = atoi(args);
		if (_tcsicmp(args, "true") == 0 || nArgs != 0)
		{
			m_player.SetMenu(TRUE);
		}
		else
		if (_tcsicmp(args, "false") == 0 || nArgs == 0)
		{
			m_player.SetMenu(FALSE);
		}
		return;
	}

	COutputBar *pBar = COutputBar::Instance();
	if (!m_bOutputShown)
	{
		pBar->Show(TRUE);
		m_bOutputShown = TRUE;
	}

	if (_tcsicmp(command, "trace") == 0)
		pBar->m_pView->AppendLog("%s\r\n", args);
	else
		pBar->m_pView->AppendLog("fscommand: %s, %s\r\n", command, args);
}

void CDlgPreview::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CResizableDialog::OnCancel();

	DestroyWindow();
}

void CDlgPreview::OnDestroy()
{
	CResizableDialog::OnDestroy();

	// TODO: Add your message handler code here

	SaveWindowPos();

	::DeleteFile(m_strFile);
}

BOOL CDlgPreview::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;

	return CResizableDialog::PreTranslateMessage(pMsg);
}

void CDlgPreview::OnPreviewEscape()
{
	if (m_bFullScreen)
	{
		if (COptions::Instance()->m_bQuitFullscreenOnEsc)
		{
			// do not quit preview mode if it's fullscreen mode
			FSCommandPlayer("fullscreen", "false");
			return;
		}
	}

	if (COptions::Instance()->m_bClosePreviewOnEsc)
		OnCancel();
}

void CDlgPreview::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	if (nID == SC_MAXIMIZE)
		ShowCaptionAndBorder(TRUE);

	CResizableDialog::OnSysCommand(nID, lParam);
}
