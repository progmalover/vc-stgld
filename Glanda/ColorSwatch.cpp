// ColorSwatch.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ColorSwatch.h"
#include "ColorPicker.h"
#include ".\colorswatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSwatch

CColorSwatch::CColorSwatch()
{
	m_bAlwaysSelect = TRUE;

	m_ptSwatch.x = -1;
	m_ptSwatch.y = -1;

	m_bTracking = FALSE;
}

CColorSwatch::~CColorSwatch()
{
}


BEGIN_MESSAGE_MAP(CColorSwatch, CStatic)
	//{{AFX_MSG_MAP(CColorSwatch)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_ENABLE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSwatch message handlers

void CColorSwatch::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	CColorPicker::DrawSwatch(&dcMem, rc);
	CColorPicker::DrawSwatchMarker(&dcMem, rc, m_ptSwatch.x, m_ptSwatch.y);


	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pBmpOld);

	// Do not call CStatic::OnPaint() for painting messages
}

void CColorSwatch::AdjustSize()
{
	CRect rc;
	GetWindowRect(&rc);

	CColorPicker::AdjustSwatchRect(rc);

	GetParent()->ScreenToClient(&rc);

	MoveWindow(&rc);
}

COLORREF CColorSwatch::GetColor()
{
	if (m_ptSwatch.x >= 0 && m_ptSwatch.x < HBLOCKS && m_ptSwatch.y >= 0 && m_ptSwatch.y < VBLOCKS)
	{
		return CColorPicker::Swatch[m_ptSwatch.y][m_ptSwatch.x];
	}
	else
	{
		return CLR_INVALID;
	}
}

void CColorSwatch::SetColor(COLORREF color)
{
	if (GetColor() == color)
		return;

	BOOL bFound = FALSE;
	if (color != CLR_INVALID)
	{
		for (int  i = 0; i < VBLOCKS; i++)
		{
			if (i != VBLOCKS - 3)
			{
				for (int j = 0; j < HBLOCKS; j++)
				{
					if (CColorPicker::Swatch[i][j] == color)
					{
						m_ptSwatch.y = i;
						m_ptSwatch.x = j;
						bFound = TRUE;
						break;
					}
				}
			}
		}
	}

	if (!bFound)
	{
		m_ptSwatch.y = -1;
		m_ptSwatch.x = -1;
	}

	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CColorSwatch::SwatchChange(const CPoint &point)
{
	CRect rc;
	GetClientRect(&rc);

	int cx = (rc.Width()) / HBLOCKS;
	int cy = (rc.Height()) / VBLOCKS;

	int x = point.x / cx;
	int y = point.y / cy;

	x = max(0, min(HBLOCKS - 1, x));
	y = max(0, min(VBLOCKS - 1, y));

	if (y != VBLOCKS - 3 && (m_ptSwatch.x != x || m_ptSwatch.y != y))
	{
		m_ptSwatch.x = x;
		m_ptSwatch.y = y;

		Invalidate();

		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SWN_CHANGE), (LPARAM)m_hWnd);
	}
}

void CColorSwatch::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//StartTrack();

	SwatchChange(point);

	CStatic::OnLButtonDown(nFlags, point);
}

void CColorSwatch::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//if (m_bTracking)
	//	SwatchChange(point);

	if (m_bAlwaysSelect || (GetKeyState(VK_LBUTTON) >> (sizeof(SHORT) - 1)))
		SwatchChange(point);

	CStatic::OnMouseMove(nFlags, point);
}

void CColorSwatch::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//if (m_bTracking)
	//{
	//	EndTrack();
	//	SwatchChange(point);
	//}

	CStatic::OnLButtonUp(nFlags, point);
}

void CColorSwatch::StartTrack()
{
	m_bTracking = TRUE;

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	ClipCursor(&rc);
}

void CColorSwatch::EndTrack()
{
	m_bTracking = FALSE;
	ClipCursor(NULL);
}

BOOL CColorSwatch::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PICKER));

	return TRUE;

	//return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CColorSwatch::SetAlwaysSelect(BOOL bAlways)
{
	m_bAlwaysSelect = bAlways;
}

void CColorSwatch::OnEnable(BOOL bEnable)
{
	// avoid repainting.
	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
