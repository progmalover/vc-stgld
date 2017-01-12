// StaticBitmap.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticBitmap.h"

#include "Graphics.h"

#include "VisualStylesXP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CStaticBitmap

IMPLEMENT_DYNAMIC(CStaticBitmap, CStatic)
CStaticBitmap::CStaticBitmap()
{
}

CStaticBitmap::~CStaticBitmap()
{
}


BEGIN_MESSAGE_MAP(CStaticBitmap, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CStaticBitmap message handlers


BOOL CStaticBitmap::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CStaticBitmap::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CRect rc;
	GetClientRect(&rc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	if (g_xpStyle.IsAppThemed())
		g_xpStyle.DrawThemeParentBackground(m_hWnd, dcMem, &rc);
	else
		dcMem.FillSolidRect(&rc, FACE_COLOR);

	HBITMAP hBmp = GetBitmap();
	if (hBmp)
	{
		CBitmap *pBmp = CBitmap::FromHandle(hBmp);
		BITMAP bm;
		pBmp->GetBitmap(&bm);
		DrawTransparent(&dcMem, rc.left + (rc.Width() - bm.bmWidth) / 2, rc.top + (rc.Height() - bm.bmHeight) / 2, bm.bmWidth, bm.bmHeight, pBmp, 0, 0, RGB(255, 0, 255));
	}

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pBmpOld);
}

void CStaticBitmap::OnEnable(BOOL bEnable)
{
	// avoid repaint

	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
