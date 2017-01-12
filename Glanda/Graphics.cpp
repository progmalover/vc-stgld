#include "stdafx.h"
#include "Graphics.h"
#include "DIB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void DrawBitmap(HDC hDC, int xDC, int yDC, int cx, int cy, HBITMAP hBitmap, int xBmp, int yBmp)
{
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	::BitBlt(hDC, xDC, yDC, cx, cy, hMemDC, xBmp, yBmp, SRCCOPY);
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
}

void DrawTransparent(CDC *pDC, int xDC, int yDC, int cx, int cy, CBitmap *pBmp, int xBmp, int yBmp, COLORREF crTrans)
{
	const COLORREF m_crWhite = RGB(255, 255, 255);
	const COLORREF m_crBlack = RGB(0, 0, 0);

	COLORREF crOldBack = pDC->SetBkColor(m_crWhite);
	COLORREF crOldText = pDC->SetTextColor(m_crBlack);

	CDC dcImage, dcTrans;

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage=dcImage.SelectObject(pBmp);
	if(!pOldBitmapImage)
		return;

	// Create the mask bitmap
	CBitmap bitmapTrans;
	BITMAP bmp;
	pBmp->GetBitmap(&bmp);
	int nWidth=bmp.bmWidth;
	int nHeight=bmp.bmHeight;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans=dcTrans.SelectObject(&bitmapTrans);
	if(!pOldBitmapTrans)
		return;

	// Build mask based on transparent colour
	dcImage.SetBkColor(crTrans);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	// Do the work - True Mask method - cool if not actual display
	pDC->BitBlt(xDC, yDC, cx, cy, &dcImage, xBmp, yBmp, SRCINVERT);
	pDC->BitBlt(xDC, yDC, cx, cy, &dcTrans, xBmp, yBmp, SRCAND);
	pDC->BitBlt(xDC, yDC, cx, cy, &dcImage, xBmp, yBmp, SRCINVERT);

	// Restore settings
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);
}

void DrawDisabled(CDC *pDC, int xDC, int yDC, int cx, int cy, CBitmap *pBmp, int xBmp, int yBmp)
{
	pDC->DrawState(CPoint(xDC, yDC), CSize(cx, cy), pBmp, DSS_DISABLED, NULL);
}

void DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HBITMAP hbm, int nXSrc, int nYSrc, COLORREF crBack)
{
  ASSERT(hdcDest && hbm);
  ASSERT(nWidth > 0 && nHeight > 0);
  
  // Create a generic DC for all BitBlts
  HDC hDC = CreateCompatibleDC(hdcDest);
  ASSERT(hDC);
  
  if (hDC)
  {
    // Create a DC for the monochrome DIB section
    HDC bwDC = CreateCompatibleDC(hDC);
    ASSERT(bwDC);
    
    if (bwDC)
    {
      // Create the monochrome DIB section with a black and white palette
      struct {
        BITMAPINFOHEADER bmiHeader; 
        RGBQUAD      bmiColors[2]; 
      } RGBBWBITMAPINFO = {
        
        {    // a BITMAPINFOHEADER
          sizeof(BITMAPINFOHEADER),  // biSize 
            nWidth,         // biWidth; 
            nHeight,        // biHeight; 
            1,            // biPlanes; 
            1,            // biBitCount 
            BI_RGB,         // biCompression; 
            0,            // biSizeImage; 
            0,            // biXPelsPerMeter; 
            0,            // biYPelsPerMeter; 
            0,            // biClrUsed; 
            0            // biClrImportant; 
        },    
        {
          { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
        } 
      };
      VOID *pbitsBW;
      HBITMAP hbmBW = CreateDIBSection(bwDC,
        (LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
      ASSERT(hbmBW);
      
      if (hbmBW)
      {
        // Attach the monochrome DIB section and the bitmap to the DCs
        HBITMAP olddib = (HBITMAP)SelectObject(bwDC, hbmBW);
        SelectObject(hDC, hbm);
        
        // BitBlt the bitmap into the monochrome DIB section
        BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);
        
        // Paint the destination rectangle in gray

		//HBRUSH hb = CreateSolidBrush(crBack);
        //FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
        //  nHeight), hb);
		//DeleteObject(hb);
        
        // BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT
        // bits in the destination DC
        // The magic ROP comes from the Charles Petzold's book
        HBRUSH hb = CreateSolidBrush(Hilighten(crBack));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
        BitBlt(hdcDest,nXDest+1,nYDest+1,nWidth,nHeight,bwDC,0,0,0xB8074A);
        
        // BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW
        // bits in the destination DC
        hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
        DeleteObject(SelectObject(hdcDest, hb));
        BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight,bwDC,0,0,0xB8074A);
        DeleteObject(SelectObject(hdcDest, oldBrush));
        VERIFY(DeleteObject(SelectObject(bwDC, olddib)));
      }
      
      VERIFY(DeleteDC(bwDC));
    }
    
    VERIFY(DeleteDC(hDC));
  }

}

void BlendWindow(CWnd *pWnd, DWORD dwTime)
{
	ASSERT(pWnd);

	CRect rc;
	pWnd->GetWindowRect(&rc);

	CDC dcDisp;
	dcDisp.CreateDC("DISPLAY", NULL, NULL, NULL);
	
	CDC dcDispMem;
	dcDispMem.CreateCompatibleDC(&dcDisp);

	CBitmap bmpDisp;
	bmpDisp.CreateCompatibleBitmap(&dcDisp, rc.Width(), rc.Height());
	CBitmap *pOldBmpDisp = dcDispMem.SelectObject(&bmpDisp);

	dcDispMem.BitBlt(0, 0, rc.Width(), rc.Height(), &dcDisp, rc.left, rc.top, SRCCOPY);
	
	dcDispMem.SelectObject(pOldBmpDisp);

	// The following does work, but the window can not draw itself 
	// correctly in the taskbar (if it has a icn in the task bar). 
	// The solve is using SetWindowPos() and an unknown extended window 
	// style as the code below.
	//pWnd->ModifyStyle(0,WS_VISIBLE);

	// I found that Windows 2000's AnimateWindow() adds the extended style
	// 0x00080000L during the animation.
	pWnd->ModifyStyleEx(0, 0x00080000);
	pWnd->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOCOPYBITS | SWP_NOREDRAW);
	pWnd->ModifyStyleEx(0x00080000, 0);

	CWindowDC dcWin(pWnd);

	CDC dcWinMem;
	dcWinMem.CreateCompatibleDC(&dcWin);

	CBitmap bmpWin;
	bmpWin.CreateCompatibleBitmap(&dcWin, rc.Width(), rc.Height());
	CBitmap *pOldBmpWin = dcWinMem.SelectObject(&bmpWin);

	dcWinMem.FillSolidRect(0, 0, rc.Width(), rc.Height(), FACE_COLOR);
	pWnd->SendMessage(WM_PRINT, (WPARAM)dcWinMem.m_hDC, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_ERASEBKGND | PRF_CHILDREN | PRF_OWNED);
	pWnd->SendMessage(WM_PRINT, (WPARAM)dcWinMem.m_hDC, (LPARAM)PRF_NONCLIENT | PRF_CHILDREN | PRF_OWNED);

	dcWinMem.SelectObject(pOldBmpWin);

	CDib dibDisp, dibWin;
	dibDisp.Create(rc.Width(), rc.Height());
	dibWin.Create(rc.Width(), rc.Height());

	dibDisp.SetBitmap(&dcDisp, &bmpDisp);

	#define CON_S		200
	#define CON_E		0

	DWORD dwTick = GetTickCount();
	for (int i = CON_S; i >= CON_E && GetTickCount() - dwTick < dwTime; i -= 4)
	{
		dibWin.SetBitmap(&dcWinMem, &bmpWin);
		dibWin.Blend(&dibDisp, i);
		dibWin.Render(&dcWin, rc.Width(), rc.Height());
	}

	pWnd->RedrawWindow(NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);


	//
	//	Test speed
	//
//	DWORD dwTick = GetTickCount();
//
//	for (int i = 255; i >= 0; i--)
//	{
//		dibWin.SetBitmap(&dcWinMem, &bmpWin);
//		dibWin.Blend(&dibDisp, i);
//		dibWin.Render(&dcWin, rc.Width(), rc.Height());
//	}

//	char sz[256];
//	itoa(GetTickCount() - dwTick, sz,10);
//	pWnd->//MessageBox(sz);

}

#define COLOR_SCALE_1	1.08
#define COLOR_SCALE_2	(1 / 0.8)

COLORREF Lighten(COLORREF cr)
{
	return RGB(min(GetRValue(cr) * COLOR_SCALE_1, 255), 
		min(GetGValue(cr) * COLOR_SCALE_1, 255), 
		min(GetBValue(cr) * COLOR_SCALE_1, 255));
}

COLORREF Hilighten(COLORREF cr)
{
	return RGB(min(GetRValue(cr) * COLOR_SCALE_2, 255), 
		min(GetGValue(cr) * COLOR_SCALE_2, 255), 
		min(GetBValue(cr) * COLOR_SCALE_2, 255));
}

COLORREF Darken(COLORREF cr)
{
	return RGB(max(GetRValue(cr) / COLOR_SCALE_2, 0), 
		max(GetGValue(cr) / COLOR_SCALE_2, 0), 
		max(GetBValue(cr) / COLOR_SCALE_2, 0));
}

RGBType HSVType::toRGB()
{
RGBType rgb;
	if(!h  && !s)
	{
		rgb.r = rgb.g = rgb.b = v;
	}
double min,max,delta,hue;
	
	max = v;
	delta = (max * s)/255.0;
	min = max - delta;

	hue = h;
	if(h > 300 || h <= 60)
	{
		rgb.r = (int)max;
		if(h > 300)
		{
			rgb.g = (int)min;
			hue = (hue - 360.0)/60.0;
			rgb.b = (int)((hue * delta - min) * -1);
		}
		else
		{
			rgb.b = (int)min;
			hue = hue / 60.0;
			rgb.g = (int)(hue * delta + min);
		}
	}
	else
	if(h > 60 && h < 180)
	{
		rgb.g = (int)max;
		if(h < 120)
		{
			rgb.b = (int)min;
			hue = (hue/60.0 - 2.0 ) * delta;
			rgb.r = (int)(min - hue);
		}
		else
		{
			rgb.r = (int)min;
			hue = (hue/60 - 2.0) * delta;
			rgb.b = (int)(min + hue);
		}
	}
	else
	{
		rgb.b = (int)max;
		if(h < 240)
		{
			rgb.r = (int)min;
			hue = (hue/60.0 - 4.0 ) * delta;
			rgb.g = (int)(min - hue);
		}
		else
		{
			rgb.g = (int)min;
			hue = (hue/60 - 4.0) * delta;
			rgb.r = (int)(min + hue);
		}
	}
	return rgb;
}


HSVType RGBType::toHSV()
{
HSVType hsv;

double min,max,delta,temp;
	min = __min(r,__min(g,b));	
	max = __max(r,__max(g,b));
	delta = max - min;
	
	hsv.v = (int)max;
	if(!delta)
	{
		hsv.h = hsv.s = 0;
	}
	else
	{
		temp = delta/max;
		hsv.s = (int)(temp*255);

		if(r == (int)max)
		{
			temp = (double)(g-b)/delta;
		}
		else
		if(g == (int)max)
		{
			temp = 2.0 + ((double)(b-r)/delta);
		}
		else
		{
			temp = 4.0 + ((double)(r-g)/delta);
		}
		temp *= 60;
		if(temp < 0)
		{
			temp+=360;
		}
		if(temp == 360)
		{
			temp = 0;
		}
		hsv.h = (int)temp;
	}
	return hsv;

}

// RGB(255, 255, 0) => #ffff00
CString RGBToString(COLORREF rgb)
{
	CString strColor;
	strColor.Format("#%02x%02x%02x", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
	return strColor;
}

// #ffff00 => RGB(255, 255, 0)
COLORREF StringToRGB(LPCTSTR lpszColor)
{
	int r = 0;
	int g = 0;
	int b = 0;

	CString strColor = lpszColor;
	int len = strColor.GetLength();
	if (len > 0)
	{
		int nNewLen = 6;
		int nStart = 0;
		if (strColor[0] == '#')
		{
			nStart++;
			nNewLen++;
		}

		for (int i = 0; i < nNewLen - len; i++)
			strColor += '0';

		sscanf(strColor.Mid(nStart, 2), "%x", &r);
		sscanf(strColor.Mid(nStart + 2, 2), "%x", &g);
		sscanf(strColor.Mid(nStart + 4, 2), "%x", &b);
	}

	return RGB((BYTE)r, (BYTE)g, (BYTE)b);
}

void DrawGradientRect(CDC* pDC, const CRect& Rect, COLORREF StartColor,COLORREF EndColor, BOOL bHorizontal)
{
	typedef BOOL (WINAPI* GRADIENTFILLPROC)(IN HDC, IN PTRIVERTEX, IN ULONG, IN PVOID, IN ULONG, IN ULONG);

	static HMODULE hLib = NULL;
	static GRADIENTFILLPROC lpfnGradientFill = NULL;
	static BOOL bAlreadyFailed = FALSE;

	if (lpfnGradientFill == NULL && !bAlreadyFailed)
	{
		if (hLib == NULL)
		{
			OSVERSIONINFO osvi;
			memset(&osvi, 0, sizeof(osvi));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if (GetVersionEx(&osvi) && osvi.dwMajorVersion >= 5L)
			{
				hLib = LoadLibrary("Msimg32.dll");
				if (hLib)
					lpfnGradientFill = (GRADIENTFILLPROC)GetProcAddress(hLib,"GradientFill");
			}
		}

		if (lpfnGradientFill == NULL)
			bAlreadyFailed = TRUE;
	}

	BOOL bUseSolid = (pDC->GetDeviceCaps(NUMCOLORS) == -1);

	// for running under win95 and WinNt 4.0 without loading Msimg32.dll
	if (!bUseSolid && lpfnGradientFill)
	{
		TRIVERTEX vert[2] ;
		GRADIENT_RECT gRect;

		vert [0].y = Rect.top;
		vert [0].x = Rect.left;

		vert [0].Red    = COLOR16(COLOR16(GetRValue(StartColor)) << 8);
		vert [0].Green  = COLOR16(COLOR16(GetGValue(StartColor)) << 8);
		vert [0].Blue   = COLOR16(COLOR16(GetBValue(StartColor)) << 8);
		vert [0].Alpha  = 0x0000;

		vert [1].y = Rect.bottom;
		vert [1].x = Rect.right;

		vert [1].Red    = COLOR16(COLOR16(GetRValue(EndColor)) << 8);
		vert [1].Green  = COLOR16(COLOR16(GetGValue(EndColor)) << 8);
		vert [1].Blue   = COLOR16(COLOR16(GetBValue(EndColor)) << 8);
		vert [1].Alpha  = 0x0000;

		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;

		if(bHorizontal)
			lpfnGradientFill(pDC->m_hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
		else
			lpfnGradientFill(pDC->m_hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
	}
	else
	{
		BYTE StartRed   = GetRValue(StartColor);
		BYTE StartGreen = GetGValue(StartColor);
		BYTE StartBlue  = GetBValue(StartColor);

		BYTE EndRed    = GetRValue(EndColor);
		BYTE EndGreen  = GetGValue(EndColor);
		BYTE EndBlue   = GetBValue(EndColor);

		int n = (bHorizontal) ? Rect.Width() : Rect.Height();

		// only need for the rest, can be optimized
		{
			if(bUseSolid)
			{
				// We need a solid brush (can not be doted)
				pDC->FillSolidRect(Rect, pDC->GetNearestColor(EndColor));
			}
			else
			{
				// We need a brush (can be doted)
				CBrush TempBrush(EndColor);
				pDC->FillRect(Rect, &TempBrush);
			}
		}
		int dy = 2;
		n-=dy;
		for(int dn=0;dn<=n;dn+=dy)
		{
			BYTE ActRed = (BYTE)(MulDiv(int(EndRed) - StartRed, dn, n) + StartRed);
			BYTE ActGreen = (BYTE)(MulDiv(int(EndGreen) - StartGreen, dn, n) + StartGreen);
			BYTE ActBlue = (BYTE)(MulDiv(int(EndBlue) - StartBlue, dn, n) + StartBlue);

			CRect TempRect; 
			if(bHorizontal)
				TempRect = CRect(CPoint(Rect.left + dn, Rect.top), CSize(dy, Rect.Height()));
			else
				TempRect = CRect(CPoint(Rect.left, Rect.top + dn), CSize(Rect.Width(), dy));

			if(bUseSolid)
			{
				pDC->FillSolidRect(TempRect, pDC->GetNearestColor(RGB(ActRed, ActGreen, ActBlue)));
			}
			else
			{
				CBrush TempBrush(RGB(ActRed, ActGreen, ActBlue));
				pDC->FillRect(TempRect, &TempBrush);
			}
		}
	}
}
