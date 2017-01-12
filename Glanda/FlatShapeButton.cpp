// FlatShapeButton.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "FlatShapeButton.h"
#include "gldShape.h"
#include "gld_graph.h"
#include "SWFProxy.h"
#include "gld_draw.h"
#include "MemDC.h"
#include "Graphics.h"
#include "TextStyleManager.h"
#include "Graphics.h"
#include "Dib.h"
#include "ResourceThumbItem.h"
#include "VisualStylesXP.h"

// CFlatShapeButton

//IMPLEMENT_DYNAMIC(CFlatShapeButton, CFlatButton)
CFlatShapeButton::CFlatShapeButton()
{
}

CFlatShapeButton::~CFlatShapeButton()
{
}


BEGIN_MESSAGE_MAP(CFlatShapeButton, CFlatButton)
END_MESSAGE_MAP()



// CFlatShapeButton message handlers

void CFlatShapeButton::DrawStuff(CDC *pDC, const CRect &rc)
{
	CRect rc2(rc);
	rc2.DeflateRect(1, 1);

	if(m_pBitmapBuffer != NULL && m_pBitmapBuffer->m_Size != rc2.Size())
	{
		m_pBitmapBuffer.Free();
	}

	if(m_pBitmapBuffer==NULL)
	{
		m_pBitmapBuffer.Attach(new CDib());
		m_pBitmapBuffer->Create(rc2.Width(), rc2.Height());

		CBitmap bmp;
		bmp.Attach(m_pBitmapBuffer->m_Bitmap);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap* bmpOld = dc.SelectObject(&bmp);

		RenderEx(dc, CRect(CPoint(0, 0), m_pBitmapBuffer->m_Size));

		dc.SelectObject(bmpOld);
		bmp.Detach();
	}

	COLORREF clrBackground = g_xpStyle.IsAppThemed() ? RGB(255, 255, 255) : GetSysColor(COLOR_3DFACE);
	DrawTransparent(pDC, rc2.left, rc2.top, rc2.Width(), rc2.Height(), CBitmap::FromHandle(m_pBitmapBuffer->m_Bitmap), 0, 0, clrBackground);

	rc2.InflateRect(1, 1);
}

CSize CFlatShapeButton::GetStuffSize()
{
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(SPACING, SPACING);
	return rc.Size();
}	

CTextStyleData* CFlatShapeButton::GetTextStyle()
{
	return m_pTextStyle;
}

void CFlatShapeButton::SetTextStyle(CTextStyleData* pTextStyle)
{
	m_pTextStyle.Free();
	if(pTextStyle)
	{
		m_pTextStyle.Attach(pTextStyle->Clone());
	}

	ReleaseBitmapBuffer();
	Invalidate();
}

void CFlatShapeButton::RenderEx(CDC& dc, CRect& rc)
{
	if(m_pTextStyle)
	{
		gldShape* pShape = m_pTextStyle->GeneratePreviewText();
		if(pShape)
		{
			COLORREF clrBackground = g_xpStyle.IsAppThemed() ? RGB(255, 255, 255) : GetSysColor(COLOR_3DFACE);
			CResourceThumbItem::RenderObj(dc, rc, pShape, clrBackground);
			CTransAdaptor::DestroyTShapePtr(pShape);
			delete pShape;
		}
	}
}

void CFlatShapeButton::ReleaseBitmapBuffer()
{
	m_pBitmapBuffer.Free();
}