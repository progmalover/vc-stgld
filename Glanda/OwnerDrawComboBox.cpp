// OwnerDrawComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OwnerDrawComboBox.h"
#include ".\ownerdrawcombobox.h"

// COwnerDrawComboBox

COwnerDrawComboBox::COwnerDrawComboBox()
{
}

COwnerDrawComboBox::~COwnerDrawComboBox()
{
}


BEGIN_MESSAGE_MAP(COwnerDrawComboBox, CComboBox)
END_MESSAGE_MAP()



// COwnerDrawComboBox message handlers


void COwnerDrawComboBox::DrawItem(LPDRAWITEMSTRUCT lp)
{
	// TODO:  Add your code to draw the specified item

	int index = lp->itemID;
	if (index >= 0 || lp->itemData != (DWORD)-1)
	{
		CItemData *pData = (CItemData *)GetItemData(index);
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

		pDC->FillSolidRect(&rc, crBack);

		CString strText;
		GetLBText(index, strText);
		rc.left += 5;
		pDC->DrawText(strText, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		pDC->SelectObject(pFontOld);
		pDC->SetBkMode(nBkModeOld);
		pDC->SetTextColor(crTextOld);
	}
}

void COwnerDrawComboBox::SetItemBold(int index, BOOL bBold)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	pData->m_bBold = bBold;
}

void COwnerDrawComboBox::SetItemColor(int index, BOOL bSysColor, COLORREF color)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	pData->m_bSysColor = bSysColor;
	pData->m_crText = color;
}

LRESULT COwnerDrawComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	LRESULT lResult = CComboBox::WindowProc(message, wParam, lParam);
	if (message == CB_ADDSTRING || message == CB_INSERTSTRING)
	{
		if ((int)lResult >= 0)
		{
			CItemData *pData = new CItemData();
			SetItemData((int)lResult, (DWORD_PTR)pData);
		}
	}

	return lResult;
}

void COwnerDrawComboBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	// TODO: Add your specialized code here and/or call the base class

	CItemData *pData = (CItemData *)GetItemData((int)lpDeleteItemStruct->itemID);
	if (pData)
		delete pData;

	CComboBox::DeleteItem(lpDeleteItemStruct);
}

void COwnerDrawComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
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

void COwnerDrawComboBox::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CComboBox::PreSubclassWindow();
}
