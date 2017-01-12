// ImageListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ImageListCtrl.h"
#include "gldImage.h"
#include "GGraphics.h"
#include "Image.h"
#include "jpeg.h"
#include "bitmap.h"
#include "Graphics.h"
#include "my_app.h"
#include "GlandaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CImageListCtrl

CImageListCtrl::CImageListCtrl()
{
	m_dwStyle |= FLCS_GRID;
	SetMargin(0, 0, 0, 0);
	SetItemSpace(1, 1);
}

CImageListCtrl::~CImageListCtrl()
{
}


BEGIN_MESSAGE_MAP(CImageListCtrl, CFlexListCtrl)
	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CImageListCtrl message handlers

void CImageListCtrl::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
	CItemData *pData = (CItemData *)GetItemData(index);

	CRect rc = *pRect;
	rc.InflateRect(-2, -2);

	gldImage *_gimg = pData->pImage;
	TImage *_timg = (TImage *)_gimg->m_ptr;
	ASSERT(_gimg->m_ptr);

	ASSERT(_timg->Width() > 0 && _timg->Height() > 0);

	float scalex = (float)PIXEL_TO_TWIPS(rc.Width()) / _timg->Width();
	float scaley = (float)PIXEL_TO_TWIPS(rc.Height()) / _timg->Height();
	float scale = min(scalex, scaley);
	int dx = 0;
	int dy = 0;
	if (scalex > scaley)
	{
		int w = (float)rc.Width() * scaley / scalex;
		w = max(1, w);
		int left = rc.left;
		rc.left = (rc.right + rc.left - w) / 2;
		rc.right = rc.left + w;
		dx = rc.left - left;

	}
	else if(scalex < scaley)
	{
		int h = (float)rc.Height() * scalex / scaley;
		h = max(1, h);
		int top = rc.top;
		rc.top = (rc.bottom + rc.top - h) / 2;
		rc.bottom = rc.top + h;
		dy = rc.top - top;
	}

	if (pData->hBitmap == NULL)
	{
		TMatrix mat(scale, 0.0f, 0.0f, scale, 0.0, 0.0);
		TClippedBitmapFillStyle *pFillStyle = new TClippedBitmapFillStyle(_timg);
		pFillStyle->SetMatrix(mat);
		TFillStyleTable fstab;
		int nFillIndex = fstab.Add(pFillStyle);
		TGraphicsDevice device(rc.Width(), rc.Height());
		device.SetFillStyle0(nFillIndex);
		device.MoveTo(PIXEL_TO_TWIPS(0), PIXEL_TO_TWIPS(0));
		device.LineTo(PIXEL_TO_TWIPS(rc.Width()), PIXEL_TO_TWIPS(0));
		device.LineTo(PIXEL_TO_TWIPS(rc.Width()), PIXEL_TO_TWIPS(rc.Height()));
		device.LineTo(PIXEL_TO_TWIPS(0), PIXEL_TO_TWIPS(rc.Height()));
		device.LineTo(PIXEL_TO_TWIPS(0), PIXEL_TO_TWIPS(0));
		device.SetFillStyleTable(&fstab);
		device.Render();

		pData->hBitmap = device.Detach();
	}

	ASSERT(pData->hBitmap);
	
	::DrawBitmap(pDC->m_hDC, rc.left, rc.top, rc.Width(), rc.Height(), pData->hBitmap, 0, 0);

	if (IsItemSelected(index))
	{
		//int nROP2 = pDC->SetROP2(R2_NOTXORPEN);

		//CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
		//CPen *pPenOld = pDC->SelectObject(&pen);

		//pDC->MoveTo(rc.left, rc.top);
		//pDC->LineTo(rc.right - 1, rc.top);
		//pDC->LineTo(rc.right - 1, rc.bottom - 1);
		//pDC->LineTo(rc.left, rc.bottom - 1);
		//pDC->LineTo(rc.left, rc.top);

		//pDC->SelectObject(pPenOld);
	
		//pDC->SetROP2(nROP2);

		CBrush br;
		br.CreateSolidBrush(RGB(255, 0, 0));

		pDC->FrameRect(pRect, &br);
	}
}

int CImageListCtrl::InsertImage(int index, gldImage * pImage)
{
	CItemData *pData = new CItemData();
	pData->pImage = pImage;
	InsertItem(index, (DWORD_PTR)pData);

	return 0;
}

int CImageListCtrl::AddImage(gldImage * pImage)
{
	return InsertImage(GetItemCount(), pImage);
}

int CImageListCtrl::FindImage(gldImage *pImage)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		CItemData *pData = (CItemData *)GetItemData(i);
		if (pData->pImage == pImage)
			return i;
	}
	return -1;
}

void CImageListCtrl::OnDeleteItem(int index)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	delete pData;

	CFlexListCtrl::OnDeleteItem(index);
}

gldImage *CImageListCtrl::GetImage(int index)
{
	if (index >= 0 && index < GetItemCount())
	{
		CItemData *pData = (CItemData *)GetItemData(index);
		return pData->pImage;
	}
	return NULL;
}

BOOL CImageListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	
	if (nHitTest == HTCLIENT)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND_POINT));
		return TRUE;
	}
	return CFlexListCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void CImageListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

	//CRect rc;

	//GetClientRect(rc);
	//ClientToScreen(&rc);

	//if (rc.PtInRect(point))
	//{
	//	CMenu menu;
	//	if (menu.LoadMenu(IDR_POPUP_IMAGE_LIST))
	//	{
	//		CMenu *pPopup = menu.GetSubMenu(0);
	//		if (pPopup)
	//			pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, GetParent());
	//	}
	//}
}

BOOL CImageListCtrl::GetToolTipText(int index, CString &strText)
{
	CItemData *pData = (CItemData *)GetItemData(index);

	gldImage *_gimg = pData->pImage;
	strText.Format(IDS_TIP_IMAGE_LIST4, 
		_gimg->m_name.c_str(), 
		_gimg->GetWidth(), 
		_gimg->GetHeight(), 
		round((float)_gimg->GetRawDataLen() / 1024));

	return TRUE;
}
