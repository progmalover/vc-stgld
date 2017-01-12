// Player.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FlashPlayer.h"
#include ".\flashplayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlashPlayer

MapHwndToProc CFlashPlayer::m_mapHandle;
BOOL CFlashPlayer::m_bTracking = FALSE;

CFlashPlayer::CFlashPlayer()
{
	m_zoom = 0;
}

CFlashPlayer::~CFlashPlayer()
{
}


BEGIN_MESSAGE_MAP(CFlashPlayer, CShockwaveFlash)
	//{{AFX_MSG_MAP(CFlashPlayer)
	//}}AFX_MSG_MAP
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFlashPlayer message handlers

LRESULT CALLBACK CFlashPlayer::PlayerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_RBUTTONUP:
			TRACE0("WM_RBUTTONUP\n");
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			return ::DefWindowProc(hwnd, uMsg, wParam, lParam);

		case WM_RBUTTONDOWN:
		{
			TRACE0("WM_RBUTTONDWON\n");
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		
		case WM_CONTEXTMENU:
		{
			TRACE0("WM_CONTEXTMENU\n");
	
			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			if (point.x == -1 && point.y == -1)
			{
				point.x = 0;
				point.y = 0;
			}
			ShowContextMenu(point.x, point.y);
			
			return 0;
		}

		case WM_TIMER:
		{
			if (m_bTracking)
				return 0;
			break;
		}

		case WM_DESTROY:
		{
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG)m_mapHandle[hwnd]);
			break;
		}
	}

	return m_mapHandle[hwnd](hwnd, uMsg, wParam, lParam);
}

BOOL CFlashPlayer::Subclass()
{
#ifdef CUSTOMIZE_MENU
	MapHwndToProc::iterator it = m_mapHandle.find(this->m_hWnd);
	ASSERT(it == m_mapHandle.end());
	if (it != m_mapHandle.end())
		return FALSE;

	LONG lProc = SetWindowLong(this->m_hWnd, GWL_WNDPROC, (LONG)PlayerWndProc);
	if (!lProc)
		return FALSE;

	m_mapHandle[this->m_hWnd] = (WNDPROC)lProc;
#endif

	return TRUE;
}

void CFlashPlayer::Zoomin()
{
	Zoom(50);
	m_zoom++;
}

void CFlashPlayer::Zoomout()
{
	ASSERT(m_zoom > 0);
	if (m_zoom > 0)
	{
		Zoom(200);
		m_zoom--;
	}
}

void CFlashPlayer::ShowAll()
{
	ASSERT(m_zoom != 0);
	Zoom(0);
	m_zoom = 0;
}

int CFlashPlayer::GetZoom()
{
	return m_zoom;
}

void CFlashPlayer::SetMovie(LPCTSTR lpszNewValue)
{
	CShockwaveFlash::SetMovie(lpszNewValue);
//	ShowAll();
}

void CFlashPlayer::ShowContextMenu(int x, int y)
{
	CMenu menu;
	if (menu.LoadMenu(IDR_PLAYER))
	{
		CMenu *pMenu = menu.GetSubMenu(0);
		if (pMenu)
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			m_bTracking = TRUE;
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, AfxGetMainWnd());
			m_bTracking = FALSE;
		}
	}
}

UINT CFlashPlayer::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	return DLGC_WANTALLKEYS;
	//return CShockwaveFlash::OnGetDlgCode();
}
