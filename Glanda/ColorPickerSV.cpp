// ColorPickerSV.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ColorPickerSV.h"
#include "ColorPicker.h"
#include ".\colorpickersv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPickerSV

CColorPickerSV::CColorPickerSV()
{
	m_nHValue = 0;
	m_nSValue = 255;
	m_nVValue = 255;

	m_bTracking = FALSE;
}

CColorPickerSV::~CColorPickerSV()
{
}


BEGIN_MESSAGE_MAP(CColorPickerSV, CStatic)
	//{{AFX_MSG_MAP(CColorPickerSV)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_CAPTURECHANGED()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerSV message handlers

void CColorPickerSV::OnPaint() 
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

	CColorPicker::DrawSVRect(&dcMem, rc, m_nHValue);

	CPoint point;
	GetPointFromSVValue(point);
	CColorPicker::DrawMarker(&dcMem, point);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);

	// Do not call CStatic::OnPaint() for painting messages
}

void CColorPickerSV::SetHValue(int nHValue, BOOL bRedraw)
{
	m_nHValue = nHValue;
	if (bRedraw)
		Invalidate();
}

void CColorPickerSV::SetSValue(int nSValue, BOOL bRedraw)
{
	m_nSValue = nSValue;
	if (bRedraw)
		Invalidate();
}

void CColorPickerSV::SetVValue(int nVValue, BOOL bRedraw)
{
	m_nVValue = nVValue;
	if (bRedraw)
		Invalidate();
}

void CColorPickerSV::StartTrack()
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

void CColorPickerSV::EndTrack()
{
	m_bTracking = FALSE;
	ClipCursor(NULL);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CPN_SVCHANGE), (LPARAM)m_hWnd);
}

void CColorPickerSV::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	StartTrack();

	GetSVValueFromPoint(point);
	Invalidate();

	CStatic::OnLButtonDown(nFlags, point);
}

void CColorPickerSV::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bTracking)
	{
		ReleaseCapture();
		Invalidate();
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void CColorPickerSV::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here

	if (m_bTracking)
		EndTrack();

	CStatic::OnCaptureChanged(pWnd);
}

void CColorPickerSV::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_bTracking)
	{
		GetSVValueFromPoint(point);
		Invalidate();
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CColorPickerSV::GetSVValueFromPoint(const CPoint &point)
{
	CRect rc;
	GetClientRect(&rc);

	// point.x, point.y can be -1 from WM_LBUTTONDOWN. unknow reason.
	int x = max(0, min(rc.right, point.x));
	int y = max(0, min(rc.bottom, point.y));
	int s;
	int v;
	if (x == rc.Width())
		s = MAX_S;
	else
		s = (MAX_S + 1) * x / (rc.Width() + 1);

	if (y == rc.Height())
		v = MAX_V;
	else
		v = (MAX_V + 1) * y / (rc.Height() + 1);

	m_nSValue = MAX_S - s;
	m_nVValue = MAX_V - v;

	TRACE("S=%d, V=%d\n", m_nSValue, m_nVValue);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CPN_SVCHANGE), (LPARAM)m_hWnd);
}

void CColorPickerSV::GetPointFromSVValue(CPoint &point)
{
	CRect rc;
	GetClientRect(&rc);

	int s = MAX_S - m_nSValue;
	int v = MAX_V - m_nVValue;
	if (s == MAX_S)
		point.x = rc.Width();
	else
		point.x = (rc.Width() + 1) * s / (MAX_S + 1);

	if (v == MAX_V)
		point.y = rc.Height();
	else
		point.y = (rc.Height() + 1) * v / (MAX_V + 1);
}

int CColorPickerSV::GetHValue()
{
	return m_nHValue;
}

int CColorPickerSV::GetSValue()
{
	return m_nSValue;
}

int CColorPickerSV::GetVValue()
{
	return m_nVValue;
}

void CColorPickerSV::OnEnable(BOOL bEnable)
{
	// avoid repainting.
	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
