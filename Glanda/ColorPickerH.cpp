// ColorPickerH.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ColorPickerH.h"
#include "ColorPicker.h"
#include ".\colorpickerh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPickerH

CColorPickerH::CColorPickerH()
{
	m_nHValue = 0;
	m_bTracking = FALSE;
	m_bHorz = TRUE;
}

CColorPickerH::~CColorPickerH()
{
}


BEGIN_MESSAGE_MAP(CColorPickerH, CStatic)
	//{{AFX_MSG_MAP(CColorPickerH)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_CAPTURECHANGED()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerH message handlers

void CColorPickerH::OnPaint() 
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

	CColorPicker::DrawHRect(&dcMem, rc, m_bHorz);

	CPoint point;
	GetPointFromHValue(point);
	CColorPicker::DrawMarker(&dcMem, point);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);

	// Do not call CStatic::OnPaint() for painting messages
}

int CColorPickerH::GetHValue()
{
	return m_nHValue;
}

void CColorPickerH::SetHValue(int nHValue)
{
	ASSERT(nHValue >= 0 && nHValue <= 360);
	m_nHValue = nHValue;
	Invalidate();
}

void CColorPickerH::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	StartTrack();

	GetHValueFromPoint(point);
	Invalidate();

	CStatic::OnLButtonDown(nFlags, point);
}

void CColorPickerH::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bTracking)
	{
		ReleaseCapture();
		Invalidate();
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void CColorPickerH::StartTrack()
{
	SetCapture();

	m_bTracking = TRUE;

	CRect rc;
	GetClientRect(&rc);
	rc.right++;
	rc.bottom++;
	ClientToScreen(&rc);
	ClipCursor(&rc);
}

void CColorPickerH::EndTrack()
{
	m_bTracking = FALSE;
	ClipCursor(NULL);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CPN_HCHANGE), (LPARAM)m_hWnd);
}

void CColorPickerH::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here

	if (m_bTracking)
		EndTrack();

	CStatic::OnCaptureChanged(pWnd);
}

void CColorPickerH::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bTracking)
	{
		GetHValueFromPoint(point);
		Invalidate();
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CColorPickerH::GetHValueFromPoint(const CPoint &point)
{
	CRect rc;
	GetClientRect(&rc);

	// point.x can be -1 from WM_LBUTTONDOWN. unknow reason.
	if (m_bHorz)
	{
		int x = max(0, min(rc.right, point.x));

		if (x == rc.Width())
			m_nHValue = MAX_H;
		else
			m_nHValue = (MAX_H + 1) * x / (rc.Width() + 1);

		TRACE("P2V: x=%d, m_nHValue=%d\n", x, m_nHValue);
	}
	else
	{
		int y = max(0, min(rc.bottom, point.y));

		if (y == rc.Height())
			m_nHValue = MAX_H;
		else
			m_nHValue = (MAX_H + 1) * y / (rc.Height() + 1);

		TRACE("P2V: y=%d, m_nHValue=%d\n", y, m_nHValue);
	}

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CPN_HCHANGE), (LPARAM)m_hWnd);
}

void CColorPickerH::GetPointFromHValue(CPoint &point)
{
	CRect rc;
	GetClientRect(&rc);

	if (m_bHorz)
	{
		if (m_nHValue == MAX_H)
			point.x = rc.Width();
		else
			point.x = (rc.Width() + 1) * m_nHValue / (MAX_H + 1);
		point.y = rc.Height() / 2;
	}
	else
	{
		if (m_nHValue == MAX_H)
			point.y = rc.Height();
		else
			point.y = (rc.Height() + 1) * m_nHValue / (MAX_H + 1);
		point.x = rc.Width() / 2;
	}

	//TRACE("V2P: m_nHValue=%d, point.x=%d\n", m_nHValue, point.x);
}

void CColorPickerH::SetStyle(BOOL bHorz)
{
	m_bHorz = bHorz;
}

void CColorPickerH::OnEnable(BOOL bEnable)
{
	// avoid repainting.
	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
