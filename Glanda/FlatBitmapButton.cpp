// FlatBitmapButton.cpp : implementation file
//

#include "stdafx.h"
#include "FlatBitmapButton.h"
#include "Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatBitmapButton

CFlatBitmapButton::CFlatBitmapButton()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBitmapID = 0;
	m_nImageCount = 0;
}

CFlatBitmapButton::~CFlatBitmapButton()
{
}


BEGIN_MESSAGE_MAP(CFlatBitmapButton, CFlatButton)
	//{{AFX_MSG_MAP(CFlatBitmapButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlatBitmapButton message handlers

BOOL CFlatBitmapButton::LoadBitmap(HINSTANCE hInst, UINT nBitmapID, int nImageCount, COLORREF crTrans)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(nImageCount >= 1 && nImageCount <= 3);
	m_nImageCount = nImageCount;

	UnloadBitmap(FALSE);

	HBITMAP hBmp = ::LoadBitmap(hInst, MAKEINTRESOURCE(nBitmapID));
	if (hBmp == NULL)
		return FALSE;

	m_Bmp.Attach(hBmp);

	m_nBitmapID = nBitmapID;

	BITMAP bm;
	m_Bmp.GetBitmap(&bm);

	m_nWidth = bm.bmWidth / m_nImageCount;
	m_nHeight = bm.bmHeight;
	m_crTrans = crTrans;

	Invalidate();

	return TRUE;
}

BOOL CFlatBitmapButton::LoadBitmap(UINT nBitmapID, int nImageCount, COLORREF crTrans)
{
	return LoadBitmap(AfxGetResourceHandle(), nBitmapID, nImageCount, crTrans);
}

BOOL CFlatBitmapButton::LoadBitmap(LPCTSTR lpszFile, int nImageCount, COLORREF crTrans)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(nImageCount >= 1 && nImageCount <= 3);
	m_nImageCount = nImageCount;

	UnloadBitmap(FALSE);

	HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), lpszFile, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (!hBmp)
		return FALSE;

	m_Bmp.Attach(hBmp);

	m_nBitmapID = -1;

	BITMAP bm;
	m_Bmp.GetBitmap(&bm);

	m_nWidth = bm.bmWidth / m_nImageCount;
	m_nHeight = bm.bmHeight;
	m_crTrans = crTrans;

	Invalidate();

	return TRUE;
}

void CFlatBitmapButton::UnloadBitmap(BOOL bRedraw)
{
	ASSERT(::IsWindow(m_hWnd));

	if (m_Bmp.m_hObject)
	{
		m_Bmp.DeleteObject();
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBitmapID = 0;

		if (bRedraw)
			Invalidate();
	}
}

BOOL CFlatBitmapButton::IsBitmapLoaded()
{
	return m_Bmp.m_hObject != NULL;
}

UINT CFlatBitmapButton::GetBitmapID()
{
	return m_nBitmapID;
}

void CFlatBitmapButton::DrawStuff(CDC *pDC, const CRect &rc)
{
	if (IsWindowEnabled())
	{
		int x = 0;
		if (m_dwStyle & (FBS_FLAT | FBS_TRACKBORDER))
		{
			switch (m_dwState)
			{
			case STATE_FLAT:
				x = 0;
				break;
			case STATE_UP:
				switch (m_nImageCount)
				{
				case 1:
					x = 0;
					break;
				case 2:
				case 3:
					x = m_nWidth;
					break;
				}
				break;
			case STATE_DOWN:
				switch (m_nImageCount)
				{
				case 1:
					x = 0;
					break;
				case 2:
					x = m_nWidth;
					break;
				case 3:
					x = m_nWidth * 2;
					break;
				}
				break;
			}
		}
		else
		{
			switch (m_dwState)
			{
			case STATE_FLAT:
			case STATE_UP:
				x = 0;
				break;
			case STATE_DOWN:
				switch (m_nImageCount)
				{
				case 1:
					x = 0;
					break;
				case 2:
					x = m_nWidth;
					break;
				case 3:
					x = m_nWidth * 2;
					break;
				}
				break;
			}
		}

		DrawTransparent(pDC, rc.left, rc.top, rc.Width(), rc.Height(), &m_Bmp, x, 0, m_crTrans);
	}
	else
	{
		DitherBlt(pDC->m_hDC, rc.left, rc.top, rc.Width(), rc.Height(), (HBITMAP)m_Bmp.m_hObject, m_nImageCount  == 1 ? 0 : m_nWidth, 0, FACE_COLOR);
	}
}

CSize CFlatBitmapButton::GetStuffSize()
{
	return CSize(m_nWidth, m_nHeight);
}
