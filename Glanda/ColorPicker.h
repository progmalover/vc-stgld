// ColorPicker.h: interface for the CColorPicker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORPICKER_H__8B889662_69D5_11D5_9A19_0080C82BC2DE__INCLUDED_)
#define AFX_COLORPICKER_H__8B889662_69D5_11D5_9A19_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Graphics.h"	// Added by ClassView
#include "math.h"

#define MAX_H			360
#define MAX_S			255
#define MAX_V			255

#define MAX_R			255
#define MAX_G			255
#define MAX_B			255

#define MAX_A			255

#define HBLOCKS			19
#define VBLOCKS			(12 + 1 + 2)

class CColorPicker  
{
public:
	static void DrawSwatchMarker(CDC *pDC, const CRect &rc, int x, int y);
	static COLORREF Swatch[VBLOCKS][HBLOCKS];
	static void AdjustSwatchRect(CRect &rc);
	static void DrawSwatch(CDC *pDC, const CRect &rc);
	static void DrawMarker(CDC *pDC, const CPoint &point);
	static void DrawHRect(CDC *pDC, const CRect &rc, BOOL bHorz);
	static void DrawSVRect(CDC *pDC, const CRect &rc, int nHValue);
	CColorPicker();
	virtual ~CColorPicker();

	static void LoadUserColor(void);
};

#endif // !defined(AFX_COLORPICKER_H__8B889662_69D5_11D5_9A19_0080C82BC2DE__INCLUDED_)
