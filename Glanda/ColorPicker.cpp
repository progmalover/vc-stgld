// ColorPicker.cpp: implementation of the CColorPicker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "ColorPicker.h"
#include "Graphics.h"
#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TCHAR szColorSwatch[] = "Color Swatch";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLORREF CColorPicker::Swatch[VBLOCKS][HBLOCKS] = 
{
	{0x000000, 0x000000, 0x003300, 0x006600, 0x009900, 0x00cc00, 0x00ff00, 0x000033, 0x003333, 0x006633, 0x009933, 0x00cc33, 0x00ff33, 0x000066, 0x003366, 0x006666, 0x009966, 0x00cc66, 0x00ff66}, 
	{0x333333, 0x330000, 0x333300, 0x336600, 0x339900, 0x33cc00, 0x33ff00, 0x330033, 0x333333, 0x336633, 0x339933, 0x33cc33, 0x33ff33, 0x330066, 0x333366, 0x336666, 0x339966, 0x33cc66, 0x33ff66}, 
	{0x666666, 0x660000, 0x663300, 0x666600, 0x669900, 0x66cc00, 0x66ff00, 0x660033, 0x663333, 0x666633, 0x669933, 0x66cc33, 0x66ff33, 0x660066, 0x663366, 0x666666, 0x669966, 0x66cc66, 0x66ff66}, 
	{0x999999, 0x990000, 0x993300, 0x996600, 0x999900, 0x99cc00, 0x99ff00, 0x990033, 0x993333, 0x996633, 0x999933, 0x99cc33, 0x99ff33, 0x990066, 0x993366, 0x996666, 0x999966, 0x99cc66, 0x99ff66}, 
	{0xcccccc, 0xcc0000, 0xcc3300, 0xcc6600, 0xcc9900, 0xcccc00, 0xccff00, 0xcc0033, 0xcc3333, 0xcc6633, 0xcc9933, 0xcccc33, 0xccff33, 0xcc0066, 0xcc3366, 0xcc6666, 0xcc9966, 0xcccc66, 0xccff66}, 
	{0xffffff, 0xff0000, 0xff3300, 0xff6600, 0xff9900, 0xffcc00, 0xffff00, 0xff0033, 0xff3333, 0xff6633, 0xff9933, 0xffcc33, 0xffff33, 0xff0066, 0xff3366, 0xff6666, 0xff9966, 0xffcc66, 0xffff66}, 
	{0x0000ff, 0x000099, 0x003399, 0x006699, 0x009999, 0x00cc99, 0x00ff99, 0x0000cc, 0x0033cc, 0x0066cc, 0x0099cc, 0x00cccc, 0x00ffcc, 0x0000ff, 0x0033ff, 0x0066ff, 0x0099ff, 0x00ccff, 0x00ffff}, 
	{0x00ff00, 0x330099, 0x333399, 0x336699, 0x339999, 0x33cc99, 0x33ff99, 0x3300cc, 0x3333cc, 0x3366cc, 0x3399cc, 0x33cccc, 0x33ffcc, 0x3300ff, 0x3333ff, 0x3366ff, 0x3399ff, 0x33ccff, 0x33ffff}, 
	{0xff0000, 0x660099, 0x663399, 0x666699, 0x669999, 0x66cc99, 0x66ff99, 0x6600cc, 0x6633cc, 0x6666cc, 0x6699cc, 0x66cccc, 0x66ffcc, 0x6600ff, 0x6633ff, 0x6666ff, 0x6699ff, 0x66ccff, 0x66ffff}, 
	{0x00ffff, 0x990099, 0x993399, 0x996699, 0x999999, 0x99cc99, 0x99ff99, 0x9900cc, 0x9933cc, 0x9966cc, 0x9999cc, 0x99cccc, 0x99ffcc, 0x9900ff, 0x9933ff, 0x9966ff, 0x9999ff, 0x99ccff, 0x99ffff}, 
	{0xffff00, 0xcc0099, 0xcc3399, 0xcc6699, 0xcc9999, 0xcccc99, 0xccff99, 0xcc00cc, 0xcc33cc, 0xcc66cc, 0xcc99cc, 0xcccccc, 0xccffcc, 0xcc00ff, 0xcc33ff, 0xcc66ff, 0xcc99ff, 0xccccff, 0xccffff}, 
	{0xff00ff, 0xff0099, 0xff3399, 0xff6699, 0xff9999, 0xffcc99, 0xffff99, 0xff00cc, 0xff33cc, 0xff66cc, 0xff99cc, 0xffcccc, 0xffffcc, 0xff00ff, 0xff33ff, 0xff66ff, 0xff99ff, 0xffccff, 0xffffff},

	{-1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},

	{0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff}, 
	{0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff}, 
};


CColorPicker::CColorPicker()
{
}

CColorPicker::~CColorPicker()
{

}

// S & V rect
void CColorPicker::DrawSVRect(CDC *pDC, const CRect &rc, int nHValue)
{
	int left = rc.left;
	int top = rc.top;
	int nWidth = rc.Width();
	int nHeight = rc.Height();

	CDib dib;
	dib.Create(nWidth, nHeight);
	
	COLORREF *pBits = dib.m_Bits;


	double fds = (float)MAX_S / nWidth;
	double fdv = (float)MAX_V / nHeight;
	double cur_v = MAX_V;

	HSVType hsv = {nHValue, MAX_S, MAX_V};

	for (register int i = 0; i < nHeight; i++)
	{
		cur_v -= fdv;
		hsv.v = (int)cur_v;

		double cur_s = MAX_S;
		for (register int j = 0; j < nWidth; j++)
		{
			cur_s -= fds;
			hsv.s = (int)cur_s;

			COLORREF rgb = hsv.toRGB().toRGB();
			BYTE *p = (BYTE *)(pBits + (nHeight - 1 - i) * nWidth + j);
			p[0] = GetBValue(rgb);
			p[1] = GetGValue(rgb);
			p[2] = GetRValue(rgb);
		}
	}


	// Create a logical palette for the bitmap
	int nColors = dib.m_Info.bmiHeader.biClrUsed ? dib.m_Info.bmiHeader.biClrUsed : 1 << dib.m_Info.bmiHeader.biBitCount;

#ifdef _HALFTONE

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap *pBmpOld = dcMem.SelectObject(CBitmap::FromHandle(dib.m_Bitmap));

	// Create a halftone palette if colors > 256.
	CPalette pal;
	pal.CreateHalftonePalette(pDC);
	CPalette *pPalOld = pDC->SelectPalette(&pal, FALSE);
	pDC->RealizePalette();
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(left, top, nWidth, nHeight, &dcMem, 0, 0, nWidth, nHeight, SRCCOPY);
	pDC->SelectPalette(pPalOld, FALSE);
	pDC->RealizePalette();

	dcMem.SelectObject(pBmpOld);

#else

	dib.Render(pDC, left, top, nWidth, nHeight);

#endif
}

// Hue rest
void CColorPicker::DrawHRect(CDC *pDC, const CRect &rc, BOOL bHorz)
{
	CPen pen;
	CPen *pPenOld = NULL;

	HSVType hsv = {0, MAX_S, MAX_V};

	int nTotal = bHorz ? rc.Width() : rc.Height();
	double fds = (float)MAX_H / nTotal;
	double cur_h = 0;
	for (int i = 0; i < nTotal; i++)
	{
		cur_h += fds;
		hsv.h = (int)cur_h;

		pen.CreatePen(PS_SOLID, 1, hsv.toRGB().toRGB());

		if (i == 0)
			pPenOld = pDC->SelectObject(&pen);
		else
			pPenOld = pDC->SelectObject(&pen);

		if (bHorz)
		{
			pDC->MoveTo(rc.left + i, rc.top);
			pDC->LineTo(rc.left + i, rc.bottom);
		}
		else
		{
			pDC->MoveTo(rc.left, rc.top + i);
			pDC->LineTo(rc.right, rc.top + i);
		}

		pDC->SelectObject(pPenOld);

		pen.DeleteObject();
	}
}

void CColorPicker::DrawMarker(CDC *pDC, const CPoint &point)
{
	//pDC->FillSolidRect(point.x - 7, point.y - 1, 4, 3, 0);
	//pDC->FillSolidRect(point.x + 4, point.y - 1, 4, 3, 0);
	//pDC->FillSolidRect(point.x - 1, point.y - 7, 3, 4, 0);
	//pDC->FillSolidRect(point.x - 1, point.y + 4, 3, 4, 0);

	const COLORREF crOuter = RGB(255, 255, 255);
	const COLORREF crInner = RGB(0, 0, 0);

	pDC->FillSolidRect(point.x - 8, point.y - 1, 5, 3, crOuter);
	pDC->FillSolidRect(point.x + 4, point.y - 1, 5, 3, crOuter);
	pDC->FillSolidRect(point.x - 1, point.y - 8, 3, 5, crOuter);
	pDC->FillSolidRect(point.x - 1, point.y + 4, 3, 5, crOuter);

	pDC->SetPixel(point.x - 7, point.y, crInner);
	pDC->SetPixel(point.x - 6, point.y, crInner);
	pDC->SetPixel(point.x - 5, point.y, crInner);

	pDC->SetPixel(point.x + 7, point.y, crInner);
	pDC->SetPixel(point.x + 6, point.y, crInner);
	pDC->SetPixel(point.x + 5, point.y, crInner);

	pDC->SetPixel(point.x, point.y - 7, crInner);
	pDC->SetPixel(point.x, point.y - 6, crInner);
	pDC->SetPixel(point.x, point.y - 5, crInner);

	pDC->SetPixel(point.x, point.y + 7, crInner);
	pDC->SetPixel(point.x, point.y + 6, crInner);
	pDC->SetPixel(point.x, point.y + 5, crInner);
}

void CColorPicker::DrawSwatch(CDC *pDC, const CRect &rc)
{
	int cx = (rc.Width()) / HBLOCKS;
	int cy = (rc.Height()) / VBLOCKS;

	CRect rc1(rc.left, rc.top, rc.left + cx * HBLOCKS + 1, rc.top + cy * (VBLOCKS - 3) + 1);
	pDC->FillSolidRect(&rc1, RGB(0, 0, 0));

	CRect rc2(rc.left, rc.top + cy * (VBLOCKS - 3) + 1, rc.left + cx * HBLOCKS + 1, rc.top + cy * (VBLOCKS - 2) + 1);
	pDC->FillSolidRect(&rc2, ::GetSysColor(COLOR_3DFACE));

	CRect rc3(rc.left, rc.top + cy * (VBLOCKS - 2), rc.left + cx * HBLOCKS + 1, rc.top + cy * VBLOCKS + 1);
	pDC->FillSolidRect(&rc3, RGB(0, 0, 0));

	for (int i = 0; i < VBLOCKS; i++)
	{
		if (i != VBLOCKS - 3)
		{
			for (int j = 0; j < HBLOCKS; j++)
				pDC->FillSolidRect(1 + cx * j, 1 + cy * i, cx - 1, cy - 1, Swatch[i][j]);
		}
	}
}

void CColorPicker::AdjustSwatchRect(CRect &rc)
{
	int cx = (rc.Width()) / HBLOCKS;
	int cy = (rc.Height()) / VBLOCKS;

	rc.right = rc.left + cx * HBLOCKS + 1;
	rc.bottom = rc.top + cy * VBLOCKS + 1;
}

void CColorPicker::DrawSwatchMarker(CDC *pDC, const CRect &rc, int x, int y)
{
	int cx = (rc.Width()) / HBLOCKS;
	int cy = (rc.Height()) / VBLOCKS;

	if (x >= 0 && x < HBLOCKS && y >= 0 && y < VBLOCKS)
	{
		ASSERT(y != VBLOCKS - 3);
		pDC->Draw3dRect(cx * x, cy * y, cx + 1, cy + 1, RGB(255, 255, 255), RGB(255, 255, 255));
	}
}

void CColorPicker::LoadUserColor(void)
{
	CString strEntry;
	for (int i = 0; i < HBLOCKS * 2; i++)
	{
		strEntry.Format("Color #%d", i);
		COLORREF color = AfxGetApp()->GetProfileInt(szColorSwatch, strEntry, -1);
		{
			Swatch[VBLOCKS - 2 + i / HBLOCKS][i % HBLOCKS] = (color == -1 ? RGB(255, 255, 255) : color);
		}
	}
}
