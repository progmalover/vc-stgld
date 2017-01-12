// StaticShapePreview.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "StaticShapePreview.h"
#include "gldShape.h"
#include "gld_graph.h"
#include "SWFProxy.h"
#include "gld_draw.h"
#include "MemDC.h"
#include "EditShapeSheet.h"
#include "Dib.h"
#include "Graphics.h"
#include "ResourceThumbItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CStaticShapePreview

IMPLEMENT_DYNAMIC(CStaticShapePreview, CStatic)
CStaticShapePreview::CStaticShapePreview()
: m_clrBackground(RGB(0xff,0xff,0xff))
, m_pObj(NULL)
, m_nMargin(5)
, m_nBorderWidth(1)
{
}

CStaticShapePreview::~CStaticShapePreview()
{
}


BEGIN_MESSAGE_MAP(CStaticShapePreview, CStatic)
END_MESSAGE_MAP()



// CStaticShapePreview message handlers

void CStaticShapePreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc(lpDrawItemStruct->rcItem);

	if(m_pBitmapBuffer != NULL && m_pBitmapBuffer->m_Size != rc.Size())
	{
		m_pBitmapBuffer.Free();
	}

	if(m_pBitmapBuffer==NULL)
	{
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		m_pBitmapBuffer.Attach(new CDib());
		m_pBitmapBuffer->Create(rc.Width(), rc.Height());


		CDC dc2;
		dc2.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.Attach(m_pBitmapBuffer->m_Bitmap);
		CBitmap* bmpOld = dc2.SelectObject(&bmp);

		CRect rc2(CPoint(0, 0), m_pBitmapBuffer->m_Size);
		RenderEx(dc2, rc2);

		dc2.SelectObject(bmpOld);
		bmp.Detach();

		dc.Detach();
	}

	DrawBitmap(lpDrawItemStruct->hDC, rc.left, rc.top, rc.Width(), rc.Height(), m_pBitmapBuffer->m_Bitmap, 0, 0);
}

void CStaticShapePreview::RenderEx(CDC& dc, CRect& rc)
{
	CBrush brush(GetSysColor(COLOR_HIGHLIGHT));
	for(int i = 0; i < m_nBorderWidth; ++i)
	{
		dc.FrameRect(&rc, &brush);
		rc.DeflateRect(1,1);
	}
	dc.FillSolidRect(&rc, m_clrBackground);

	if(m_pObj)
	{
		rc.DeflateRect(m_nMargin, m_nMargin);
		CResourceThumbItem::RenderObj(dc, rc, m_pObj, m_clrBackground);
		rc.InflateRect(m_nMargin, m_nMargin);
	}
}

void CStaticShapePreview::ReleaseBitmapBuffer()
{
	m_pBitmapBuffer.Free();
}