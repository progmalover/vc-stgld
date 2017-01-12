// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "stdafx.h"  // e. g. stdafx.h
#include "resource.h"  // e.g. resource.h
#include "Splash.h"  // e.g. splash.h
#include "Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

IMPLEMENT_SINGLETON(CSplashWnd)
CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()

typedef BOOL (WINAPI* ANIMATEWINDOWPROC)(HWND hwnd, DWORD dwTime, DWORD dwFlags);

#define AW_HOR_POSITIVE             0x00000001
#define AW_HOR_NEGATIVE             0x00000002
#define AW_VER_POSITIVE             0x00000004
#define AW_VER_NEGATIVE             0x00000008
#define AW_CENTER                   0x00000010
#define AW_HIDE                     0x00010000
#define AW_ACTIVATE                 0x00020000
#define AW_SLIDE                    0x00040000
#define AW_BLEND                    0x00080000

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd)
{
	if (Create(pParentWnd))
		UpdateWindow();
}

BOOL CSplashWnd::Create(CWnd* pParentWnd)
{
	if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	return CreateEx(WS_EX_TOOLWINDOW, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.

	HMODULE hLib = LoadLibrary("USER32");
	if (hLib)
	{
		ANIMATEWINDOWPROC lp = (ANIMATEWINDOWPROC)GetProcAddress(hLib, "AnimateWindow");
		if (lp)
			lp(m_hWnd, 300, AW_HIDE | AW_BLEND);
		
		FreeLibrary(hLib);
	}

	DestroyWindow();

	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	return 0;
}

void CSplashWnd::Paint(CDC *pDC)
{
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(pDC))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);
	Paint(&dc);
}

LRESULT CSplashWnd::OnPrintClient(WPARAM wp, LPARAM lp)
{
	HDC hDC = (HDC)wp;
	if (hDC)
	{
		CDC *pDC = CDC::FromHandle(hDC);
		if (pDC)
			Paint(pDC);
	}
	return 0;
}
