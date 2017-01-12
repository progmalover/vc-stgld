// TextStyleListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "TextStyleListCtrl.h"
#include "ResourceThumbItem.h"
#include "Dib.h"
#include "Graphics.h"
#include "ResourceThumbItem.h"


// CTextStyleListCtrl

//IMPLEMENT_DYNAMIC(CTextStyleListCtrl, CFlexListCtrl)
CTextStyleListCtrl::CTextStyleListCtrl()
{
	SetMargin(3, 3, 3, 3);
	SetItemSpace(3, 3);
	SetItemSize(36, 36);
	SetBkColor(COLOR_WINDOW, TRUE);
}

CTextStyleListCtrl::~CTextStyleListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTextStyleListCtrl, CFlexListCtrl)
END_MESSAGE_MAP()

void CTextStyleListCtrl::OnDeleteItem(int index)
{
	_ItemData* pItemData = (_ItemData*)GetItemData(index);
	delete pItemData;
}

void CTextStyleListCtrl::InsertTextStyle(int index, CTextStyleData* pTextStyle)
{
	_ItemData* pItemData = new _ItemData(pTextStyle);
	InsertItem(index, (DWORD_PTR)pItemData);
}

CTextStyleData* CTextStyleListCtrl::GetTextStyle(int index)
{
	_ItemData* pItemData = (_ItemData*)GetItemData(index);
	return pItemData->m_pTextStyle;
}

// CTextStyleListCtrl message handlers
void CTextStyleListCtrl::DrawItem(CDC* pDC, int index, const CRect* pRect)
{
	COLORREF clrBackground = GetSysColor(GetCurSel()==index ? COLOR_HIGHLIGHT : COLOR_3DFACE);
	CBrush Brush(clrBackground);
	pDC->FrameRect(pRect, &Brush);

	CRect rc2(*pRect);
	rc2.DeflateRect(1,1);



	_ItemData* pItemData = (_ItemData*)GetItemData(index);
	if(pItemData->m_pBitmapBuffer != NULL && pItemData->m_pBitmapBuffer->m_Size != rc2.Size())
	{
		pItemData->m_pBitmapBuffer.Free();
	}

	if(pItemData->m_pBitmapBuffer==NULL)
	{
		pItemData->m_pBitmapBuffer.Attach(new CDib());
		pItemData->m_pBitmapBuffer->Create(rc2.Width(), rc2.Height());

		CDC dc;
		dc.CreateCompatibleDC(pDC);

		CBitmap bmp;
		bmp.Attach(pItemData->m_pBitmapBuffer->m_Bitmap);
		CBitmap* pOldBitmap = dc.SelectObject(&bmp);

		CRect rc(CPoint(0, 0), pItemData->m_pBitmapBuffer->m_Size);
		RenderEx(dc, rc, pItemData->m_pTextStyle);

		dc.SelectObject(pOldBitmap);
		bmp.Detach();
	}

	DrawBitmap(pDC->m_hDC, rc2.left, rc2.top, rc2.Width(), rc2.Height(),
		pItemData->m_pBitmapBuffer->m_Bitmap, 0, 0);
}

void CTextStyleListCtrl::RenderEx(CDC& dc, CRect& rc, CTextStyleData* pTextStyle)
{
	if(pTextStyle)
	{
		gldShape* pShape = pTextStyle->GeneratePreviewText();
		if(pShape)
		{
			COLORREF clrBackground = GetSysColor(COLOR_WINDOW);
			dc.FillSolidRect(&rc, clrBackground);
			rc.DeflateRect(SPACING, SPACING);
			CResourceThumbItem::RenderObj(dc, rc, pShape, clrBackground);
			rc.InflateRect(SPACING, SPACING);
			CTransAdaptor::DestroyTShapePtr(pShape);
			delete pShape;
		}
	}
}