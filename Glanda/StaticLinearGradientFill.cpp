// StaticLinearGradientFill.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticLinearGradientFill.h"

#include "Graphics.h"
#include "gld_draw.h"
#include "SWFProxy.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CStaticLinearGradientFill

const int MARGIN = 5;
const int BORDER_SIZE = 1;

const int HANDLE_WIDTH = 11;
const int HANDLE_HEIGHT = 6;

IMPLEMENT_DYNAMIC(CStaticLinearGradientFill, CStatic)
CStaticLinearGradientFill::CStaticLinearGradientFill()
{
	
}

CStaticLinearGradientFill::~CStaticLinearGradientFill()
{
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CGradientColor *pgc = m_list[i];
		delete pgc;
	}
	m_list.clear();
}


BEGIN_MESSAGE_MAP(CStaticLinearGradientFill, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CStaticLinearGradientFill message handlers

void CStaticLinearGradientFill::SetFillStyle(TGradientFillStyle *pFillStyle)
{
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CGradientColor *pgc = m_list[i];
		delete pgc;
	}
	m_list.clear();

	LPGGRADIENTRECORD pRecord =  pFillStyle->GetFirstRecord();
	while (pRecord)
	{
		CGradientColor *pgc = new CGradientColor();
		pgc->color = pRecord->color;
		pgc->ratio = pRecord->ratio;

		m_list.push_back(pgc);

		pRecord = pRecord->next;
	}

	if (m_list.size() > 0)
		SetCurrentHandle(0);

	Invalidate(FALSE);
}

void CStaticLinearGradientFill::GetFillStyle(TGradientFillStyle *pFillStyle)
{
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CGradientColor *pgc = m_list[i];
		pFillStyle->AddRecord(pgc->color, pgc->ratio);
	}
}

void CStaticLinearGradientFill::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	if (g_xpStyle.IsAppThemed())
		g_xpStyle.DrawThemeParentBackground(m_hWnd, dcMem, &rc);
	else
		dcMem.FillSolidRect(&rc, FACE_COLOR);

	CRect rcSample = GetSampleRect(rc);

	m_device.ClearRect();
	m_device.SetSize(rcSample.Width() + 1, rcSample.Height() + 1);

	TFillStyleTable _fstab;
	TLinearGradientFillStyle *_fill;

	_fill = new TLinearGradientFillStyle();	
	GetFillStyle(_fill);	

	int _xmin = 0;
	int _ymin = 0;
	int _xmax = PIXEL_TO_TWIPS(rcSample.Width());
	int _ymax = PIXEL_TO_TWIPS(rcSample.Height());

	TSETrivial::SetFillStyleMatrix(_fill, gld_rect(_xmin, _ymin, _xmax, _ymax));

	int _fill_id = _fstab.Add(_fill);

	m_device.SetFillStyle0(_fill_id);
	m_device.MoveTo(_xmin, _ymin);
	m_device.LineTo(_xmax, _ymin);
	m_device.LineTo(_xmax, _ymax);
	m_device.LineTo(_xmin, _ymax);
	m_device.LineTo(_xmin, _ymin);

	m_device.SetFillStyleTable(&_fstab);
	m_device.Render();
	m_device.Display(dcMem, MARGIN + BORDER_SIZE, BORDER_SIZE);

	int nCurHandle = GetCurrentHandle();
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		if (i != nCurHandle)
		{
			CGradientColor *pgc = m_list[i];
			DrawHandle(dcMem, pgc);
		}
	}
	// draw current handle at last
	if (nCurHandle >= 0)
	{
		CGradientColor *pgc = m_list[nCurHandle];
		DrawHandle(dcMem, pgc);
	}

	rcSample.InflateRect(BORDER_SIZE, BORDER_SIZE);
	dcMem.Draw3dRect(&rcSample, SHADOW_COLOR, HILIGHT_COLOR);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);


	KillTimer(1);
	SetTimer(1, 500, NULL);
}

CRect CStaticLinearGradientFill::GetSampleRect(CRect &rcClient)
{
	CRect rcSample = rcClient;
	rcSample.left += MARGIN;
	rcSample.right -= MARGIN;
	rcSample.bottom -= HANDLE_HEIGHT;

	rcSample.InflateRect(-BORDER_SIZE, -BORDER_SIZE);

	return rcSample;
}

// nPos is relative to the sample rect
BYTE CStaticLinearGradientFill::PosToRatio(CRect &rcClient, int nPos)
{
	CRect rcSample = GetSampleRect(rcClient);
	int ratio = nPos * 255 / (rcSample.Width());
	return (BYTE)ratio;
}

// return pos is relative to the sample rect
int CStaticLinearGradientFill::RatioToPos(CRect &rcClient, BYTE ratio)
{
	CRect rcSample = GetSampleRect(rcClient);
	int pos = ratio * (rcSample.Width()) / 255;
	return pos;
}

void CStaticLinearGradientFill::DrawHandle(CDC &dc, CGradientColor *pgc)
{
	if (!pgc->visible)
		return;

	CRect rc;
	GetClientRect(&rc);

	int nPos = RatioToPos(rc, pgc->ratio);
	nPos = SampleToClient(rc, nPos);
	POINT points[] = 
	{
		{nPos - HANDLE_WIDTH / 2, rc.bottom - 1}, 
		{nPos, rc.bottom - HANDLE_HEIGHT}, 
		{nPos + HANDLE_WIDTH / 2, rc.bottom - 1}, 
	};

	CRgn rgn;
	rgn.CreatePolygonRgn(points, 3, ALTERNATE);
	CBrush br(RGB(pgc->color.m_r, pgc->color.m_g, pgc->color.m_b));
	dc.FillRgn(&rgn, &br);

	dc.MoveTo(points[0]);
	dc.LineTo(points[1]);
	dc.LineTo(points[2]);
	dc.LineTo(points[0]);
}

void CStaticLinearGradientFill::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	GetClientRect(&rc);

	int index = HitTest(point);
	if (index == -1)
	{
		if (m_list.size() < 8)
		{
			index = AddHandle(point.x);
		}
		else
		{
			MessageBeep(0);
			return;
		}
	}

	SetCurrentHandle(index);
	Invalidate(FALSE);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LGF_CURRENTHANDLECHANGED), (LPARAM)m_hWnd);

	Track(index, point);

	//CStatic::OnLButtonDown(nFlags, point);
}

void CStaticLinearGradientFill::Track(int index, CPoint point)
{
	SetCapture();

	CRect rc;
	GetClientRect(&rc);

	CRect rcSample = GetSampleRect(rc);

	int nPosOld = PosToRatio(rc, ClientToSample(rc, point.x));

	CGradientColor *pgc = m_list[index];

	BOOL bMoved = FALSE;

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
			{
				CPoint ptNew;
				ptNew.x = (int)(short)LOWORD(msg.lParam);
				ptNew.y = (int)(short)HIWORD(msg.lParam);

				if (ptNew.y < rc.top || ptNew.y > rc.bottom + 20)
				{
					if (m_list.size() > 2)
					{
						pgc->visible = false;
						Invalidate(FALSE);
					}
				}
				else
				{
					pgc->visible = true;
					pgc->ratio  = PosToRatio(rc, ClientToSample(rc, ptNew.x));
					Invalidate(FALSE);

					bMoved = TRUE;					
				}

				continue;
			}

			case WM_LBUTTONUP:
			{
				ReleaseCapture();
				break;
			}

			case WM_RBUTTONDOWN:
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				ReleaseCapture();
				break;

			default:
				break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (GetCapture() != this)
		{
			if (!pgc->visible)
			{
				RemoveHandle(index);
				Invalidate(FALSE);
			}
			break;
		}
	}

	if (bMoved)
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LGF_HANDLEPOSCHANGED), (LPARAM)m_hWnd);
}

int CStaticLinearGradientFill::ClientToSample(CRect &rcClient, int nPos)
{
	CRect rcSample = GetSampleRect(rcClient);
	int nPosSample = nPos - MARGIN - BORDER_SIZE;
	nPosSample = max(0, min(rcSample.Width(), nPosSample));
	return nPosSample;
}

int CStaticLinearGradientFill::SampleToClient(CRect &rcClient, int nPos)
{
	return nPos + MARGIN + BORDER_SIZE;
}

void CStaticLinearGradientFill::RemoveHandle(int index)
{
	CGradientColor *pgc = m_list[index];
	delete pgc;

	int i = 0;
	for (gradient_color_list::iterator it = m_list.begin(); it != m_list.end(); it++, i++)
	{
		if (i == index)
		{
			m_list.erase(it);
			break;
		}
	}

	if (--index < 0)
		index = 0;
	SetCurrentHandle(index);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LGF_HANDLEPOSCHANGED), (LPARAM)m_hWnd);
}

int CStaticLinearGradientFill::AddHandle(int nPos)
{
	CRect rc;
	GetClientRect(&rc);

	nPos = ClientToSample(rc, nPos);

	CGradientColor *pgc = new CGradientColor;
	if (m_list.size() == 0)
		pgc->color = TColor(0, 0, 0, 255);
	else if (m_list.size() == 1)
		pgc->color = TColor(255, 255, 255, 255);
	else
	{
		TLinearGradientFillStyle _fill;
		GetFillStyle(&_fill);
		//****************************************
		CRect rc;
		GetClientRect(&rc);
		CRect rcSample = GetSampleRect(rc);
		int _xmin = 0;
		int _ymin = 0;
		int _xmax = PIXEL_TO_TWIPS(rcSample.Width());
		int _ymax = PIXEL_TO_TWIPS(rcSample.Height());

		TMatrix _mat;
		_mat.m_e11 = (float)(_xmax - _xmin) / 0x8000;
		_mat.m_e12 = 0.0f;
		_mat.m_e21 = 0.0f;
		_mat.m_e22 = (float)(_ymax - _ymin) / 0x8000;
		_mat.m_dx = (_xmax + _xmin) / 2;
		_mat.m_dy = (_ymax + _ymin) / 2;		
		_fill.SetMatrix(_mat);
		//****************************************
		_fill.Prepare(TCxform(), TMatrix());
		pgc->color = _fill.GetPixel(PIXEL_TO_TWIPS(nPos), PIXEL_TO_TWIPS((GetSampleRect(rc).top + GetSampleRect(rc).bottom) / 2));
		_fill.Unprepare();
	}
	pgc->ratio = PosToRatio(rc, nPos);

	m_list.push_back(pgc);

	return (int)m_list.size() - 1;
}

int CStaticLinearGradientFill::GetCurrentHandle()
{
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CGradientColor *pgc = m_list[i];
		if (pgc->current)
			return i;
	}
	return -1;
}

void CStaticLinearGradientFill::SetCurrentHandle(int index)
{
	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CGradientColor *pgc = m_list[i];
		if (pgc->current)
		{
			if (i == index)
				return;

			pgc->current = false;
		}
	}

	m_list[index]->current = true;
}

void CStaticLinearGradientFill::SetCurrentColor(TColor &color)
{
	m_list[GetCurrentHandle()]->color = color;
	Invalidate(FALSE);
}

TColor CStaticLinearGradientFill::GetCurrentColor()
{
	return m_list[GetCurrentHandle()]->color;
}

int CStaticLinearGradientFill::HitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);

	for (int i = 0; i < (int)m_list.size(); i++)
	{
		CGradientColor *pgc = m_list[i];
		int nPos = RatioToPos(rc, pgc->ratio);
		nPos = SampleToClient(rc, nPos);
		if (point.x >= nPos - HANDLE_WIDTH / 2 && point.x <= nPos + HANDLE_WIDTH / 2)
			return i;
	}
		
	return -1;
}

void CStaticLinearGradientFill::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	ASSERT(GetStyle() & SS_NOTIFY);

	AddHandle(0);
	AddHandle(32767);

	SetCurrentHandle(0);

	CStatic::PreSubclassWindow();
}

int CStaticLinearGradientFill::GetMargin()
{
	return MARGIN;
}

void CStaticLinearGradientFill::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (!IsWindowVisible() || !IsWindowEnabled())
		return;

	if(GetCapture() == this)
		return;

	int index = GetCurrentHandle();
	if (index >= 0)
	{
		CGradientColor *pgc = m_list[index];
		if (!pgc->visible)
			return;

		CDC dc;
		dc.Attach(::GetDC(m_hWnd));

		int nROP2 = dc.SetROP2(R2_NOTXORPEN);

		CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen *pPenOld = dc.SelectObject(&pen);

		CRect rc;
		GetClientRect(&rc);

		int nPos = RatioToPos(rc, pgc->ratio);
		nPos = SampleToClient(rc, nPos);
		POINT points[] = 
		{
			{nPos - HANDLE_WIDTH / 2, rc.bottom - 1}, 
			{nPos, rc.bottom - HANDLE_HEIGHT}, 
			{nPos + HANDLE_WIDTH / 2, rc.bottom - 1}, 
		};

		dc.MoveTo(points[0]);
		dc.LineTo(points[1]);
		dc.LineTo(points[2]);
		dc.LineTo(points[0]);

		dc.SelectObject(pPenOld);

		dc.SetROP2(nROP2);
	}

	CStatic::OnTimer(nIDEvent);
}

void CStaticLinearGradientFill::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CStaticLinearGradientFill::OnEnable(BOOL bEnable)
{
	// avoid repainting.
	//CStatic::OnEnable(bEnable);

	// TODO: Add your message handler code here
}
