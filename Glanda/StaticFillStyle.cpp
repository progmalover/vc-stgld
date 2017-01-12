// StaticFillStyle.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticFillStyle.h"
#include "SWFProxy.h"
#include ".\staticfillstyle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CStaticFillStyle

IMPLEMENT_DYNAMIC(CStaticFillStyle, CStatic)
CStaticFillStyle::CStaticFillStyle()
{
	m_style = NULL;
}

CStaticFillStyle::~CStaticFillStyle()
{
	RELEASE(m_style);
}


BEGIN_MESSAGE_MAP(CStaticFillStyle, CStatic)
	ON_WM_PAINT()
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CStaticFillStyle message handlers

void CStaticFillStyle::SetFillStyle(TFillStyle *pFillStyle)
{
	REFRENCE(pFillStyle);
	RELEASE(m_style);

	m_style = pFillStyle;	

	if (::IsWindow(m_hWnd))
		Invalidate(FALSE);
}

void CStaticFillStyle::OnPaint()
{
	CPaintDC dc(this); // _device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages


	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	dcMem.FillSolidRect(&rc, RGB(255, 255, 255));

	#define GRID_SIZE	6

	for (int i = 0; i < rc.right; i += GRID_SIZE)
		for (int j = 0; j < rc.bottom; j += GRID_SIZE)
			if ((((i / GRID_SIZE) % 2 == 0 && (j / GRID_SIZE) % 2 == 0) || ((i / GRID_SIZE) % 2 != 0 && (j / GRID_SIZE) % 2 != 0)))
				dcMem.FillSolidRect(i, j, GRID_SIZE, GRID_SIZE, RGB(192, 192, 192));


	TGraphicsDevice _device;

	_device.ClearRect();
	_device.SetSize(rc.Width() + 1, rc.Height() + 1);

	_device.BitBlt(dcMem, 0, 0, rc.Width(), rc.Height());	

	int _xmin = 0;
	int _ymin = 0;
	int _xmax = PIXEL_TO_TWIPS(rc.Width());
	int _ymax = PIXEL_TO_TWIPS(rc.Height());

	TSETrivial::SetFillStyleMatrix(m_style, gld_rect(_xmin, _ymin, _xmax, _ymax));
	
	TFillStyleTable _fstab;	

	int _fill_id = _fstab.Add(m_style);	

	_device.SetFillStyle0(_fill_id);
	_device.MoveTo(_xmin, _ymin);
	_device.LineTo(_xmax, _ymin);
	_device.LineTo(_xmax, _ymax);
	_device.LineTo(_xmin, _ymax);
	_device.LineTo(_xmin, _ymin);

	_device.SetFillStyleTable(&_fstab);
	_device.Render();
	_device.Display(dcMem, 0, 0);



	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);
}

void CStaticFillStyle::OnEnable(BOOL bEnable)
{
	// avoid repainting.
	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
