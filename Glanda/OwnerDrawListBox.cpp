// OwnerDrawListBox.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OwnerDrawListBox.h"
#include ".\ownerdrawcombobox.h"

// COwnerDrawListBox

COwnerDrawListBox::COwnerDrawListBox()
{
}

COwnerDrawListBox::~COwnerDrawListBox()
{
}


BEGIN_MESSAGE_MAP(COwnerDrawListBox, CListBox)
END_MESSAGE_MAP()



// COwnerDrawListBox message handlers

BOOL COwnerDrawListBox::LoadImages(UINT nIDBitmap, int cx, COLORREF crMask)
{
	return m_imgList.Create(nIDBitmap, cx, 1, crMask);
}

BOOL COwnerDrawListBox::LoadImages(UINT nIDBitmap, int nFlags, int cx, int cy, COLORREF crMask)
{
	if (m_imgList.Create(cx, cy, nFlags, 0, 1))
	{
		CBitmap bmp;
		if (bmp.LoadBitmap(nIDBitmap))
		{
			return m_imgList.Add(&bmp, crMask) != -1;
		}
	}
	return FALSE;
}

void COwnerDrawListBox::DrawItem(LPDRAWITEMSTRUCT lp)
{
	// TODO:  Add your code to draw the specified item

	int index = lp->itemID;
	if (index >= 0 && lp->itemData != (DWORD)-1)
	{
		CItemData *pData = (CItemData *)lp->itemData;
		ASSERT(pData);

		CDC *pDC = CDC::FromHandle(lp->hDC);
		CRect rc = lp->rcItem;

		int nBkModeOld = pDC->SetBkMode(TRANSPARENT);
		CFont *pFont = GetParent()->GetFont();
		if (!pFont)
			return;

		LOGFONT lf;
		CFont font;
		
		if (pData->m_bBold)
		{
			pFont->GetLogFont(&lf);
			lf.lfWeight = FW_BOLD;
			font.CreateFontIndirect(&lf);
			pFont = &font;
		}

		CFont *pFontOld = pDC->SelectObject(pFont);

		COLORREF crBack;
		COLORREF crTextOld;

		if (lp->itemState & ODS_SELECTED)
		{
			crBack = GetSysColor(COLOR_HIGHLIGHT);
			crTextOld = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{
			crBack = GetSysColor(COLOR_WINDOW);
			crTextOld = pDC->SetTextColor(pData->GetTextColor());
		}

		if (m_imgList.m_hImageList == NULL)
		{
			pDC->FillSolidRect(&rc, crBack);
		}
		else
		{
			CRect rc1 = rc;
			rc1.right = rc1.left + 2 + 16 + 2;
			pDC->FillSolidRect(&rc1, GetSysColor(COLOR_WINDOW));

			CRect rc2 = rc;
			rc2.left += 2 + 16 + 2;
			pDC->FillSolidRect(&rc2, crBack);
		}

		if (m_imgList.m_hImageList != NULL)
		{
			if (pData->m_nImage >= 0)
			{
				IMAGEINFO ii;
				m_imgList.GetImageInfo(pData->m_nImage, &ii);

				POINT point;
				point.x = 2;
				point.y = rc.top + (rc.Height() - (ii.rcImage.bottom - ii.rcImage.top)) / 2;
				m_imgList.Draw(pDC, pData->m_nImage, point, ILD_TRANSPARENT);
			}
			rc.left += 16;
		}

		CString strText;
		GetText(index, strText);
		rc.left += 6;
		pDC->DrawText(strText, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		pDC->SelectObject(pFontOld);
		pDC->SetBkMode(nBkModeOld);
		pDC->SetTextColor(crTextOld);
	}
}

void COwnerDrawListBox::SetItemBold(int index, BOOL bBold)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	pData->m_bBold = bBold;
}

void COwnerDrawListBox::SetItemColor(int index, BOOL bSysColor, COLORREF color)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	pData->m_bSysColor = bSysColor;
	pData->m_crText = color;
}

LRESULT COwnerDrawListBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	LRESULT lResult = CListBox::WindowProc(message, wParam, lParam);
	if (message == LB_ADDSTRING || message == LB_INSERTSTRING)
	{
		if ((int)lResult >= 0)
		{
			CItemData *pData = new CItemData();
			SetItemData((int)lResult, (DWORD_PTR)pData);
		}
	}

	return lResult;
}

void COwnerDrawListBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	// TODO: Add your specialized code here and/or call the base class

	CItemData *pData = (CItemData *)GetItemData((int)lpDeleteItemStruct->itemID);
	if (pData)
		delete pData;

	CListBox::DeleteItem(lpDeleteItemStruct);
}

void COwnerDrawListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO:  Add your code to determine the size of specified item

	CFont *pFont = GetParent()->GetFont();
	if (pFont)
	{
		CClientDC dc(this);
		CFont *pOldFont = dc.SelectObject(pFont);
		lpMeasureItemStruct->itemHeight = dc.GetTextExtent("X").cy + 2;
		dc.SelectObject(pOldFont);
	}
}

DWORD_PTR COwnerDrawListBox::SetItemUserData(int index, DWORD_PTR dwData)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT (pData);
	DWORD_PTR dwOldData = pData->m_data;
	pData->m_data = dwData;
	return dwOldData;
}

DWORD_PTR COwnerDrawListBox::GetItemUserData(int index)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT (pData);
	return pData->m_data;
}

void COwnerDrawListBox::SetItemImage(int index, int nImage)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	if (pData->m_nImage != nImage)
	{
		pData->m_nImage = nImage;
		RedrawItem(index);
	}
}

int COwnerDrawListBox::GetItemImage(int index)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	return pData->m_nImage;
}

void COwnerDrawListBox::RedrawItem(int index)
{
	CRect rc;
	GetItemRect(index, &rc);
	InvalidateRect(&rc);
}
