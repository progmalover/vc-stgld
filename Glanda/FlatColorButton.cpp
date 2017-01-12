// FlatColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "FlatColorButton.h"
#include "Graphics.h"
#include "DlgColorAdvanced.h"
#include "Utils.h"

#include "VisualStylesXP.h"

#include "Tips.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatColorButton

CFlatColorButton::CFlatColorButton()
{
	m_bEnableAlpha = TRUE;
	m_dwStyle = FBS_CLIENTEDGE;

	m_nColor = CLR_INVALID;
	m_nAValue = 255;
}

CFlatColorButton::~CFlatColorButton()
{
}


BEGIN_MESSAGE_MAP(CFlatColorButton, CFlatButton)
	//{{AFX_MSG_MAP(CFlatColorButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_COLORCHANGE, OnColorChange)
	ON_REGISTERED_MESSAGE(WM_COLORADVANCED, OnColorAdvanced)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlatColorButton message handlers

void CFlatColorButton::SetColor(COLORREF color, int alpha)
{
	m_nColor = color;
	m_nAValue = alpha;

	if (::IsWindow(this->m_hWnd))
		Invalidate();
}

COLORREF CFlatColorButton::GetColor()
{
	return m_nColor;
}

CSize CFlatColorButton::GetStuffSize()
{
	CRect rc;
	GetClientRect(&rc);
	if (g_xpStyle.IsAppThemed())
		return CSize(rc.Width() - 6, rc.Height() - 6);
	else
		return CSize(rc.Width() - 4, rc.Height() - 4);
}

void CFlatColorButton::DrawStuff(CDC *pDC, const CRect &rc)
{
	CDib dib;
	dib.Create(rc.Width(), rc.Height());

	// background
	dib.Fill(255, 255, 255);

	// grid
	#define GRID_SIZE	3

	for (int i = 0; i < rc.right; i += GRID_SIZE)
	{
		for (int j = 0; j < rc.bottom; j += GRID_SIZE)
		{
			if ((((i / GRID_SIZE) % 2 == 0 && (j / GRID_SIZE) % 2 == 0) ||
				((i / GRID_SIZE) % 2 != 0 && (j / GRID_SIZE) % 2 != 0)))
			{
				dib.FillRect(i, j, GRID_SIZE, GRID_SIZE, 192, 192, 192);
			}
		}
	}
	// alpha
	if (m_nAValue != 255)
		dib.FillGlass(GetRValue(m_nColor), GetGValue(m_nColor), GetBValue(m_nColor), m_nAValue);
	else if (m_nColor == CLR_INVALID)
		dib.FillGlass(GetRValue(m_nColor), GetGValue(m_nColor), GetBValue(m_nColor), 0);
	else
		dib.Fill(GetRValue(m_nColor), GetGValue(m_nColor), GetBValue(m_nColor));

	dib.Render(pDC, rc.left, rc.top, rc.Width(), rc.Height());

	pDC->Draw3dRect(&rc, SHADOW_COLOR, HILIGHT_COLOR);

	#define _MARGIN		0
	#define _SIZE		8

	POINT points[3] = 
	{
		{rc.right - (_SIZE + _MARGIN), rc.bottom - _MARGIN}, 
		{rc.right - _MARGIN, rc.bottom - (_SIZE + _MARGIN)}, 
		{rc.right - _MARGIN, rc.bottom - _MARGIN}
	};

	CRgn rgn;
	rgn.CreatePolygonRgn(points, 3, ALTERNATE);

	pDC->FillRgn(&rgn, CBrush::FromHandle(IsWindowEnabled() ? (HBRUSH)GetStockObject(BLACK_BRUSH) : (HBRUSH)::GetSysColorBrush(COLOR_3DSHADOW)));
	pDC->FrameRgn(&rgn, CBrush::FromHandle((HBRUSH)::GetSysColorBrush(COLOR_3DFACE)), 1, 1);
	CPen pen(PS_SOLID, 1, HILIGHT_COLOR);
	CPen *pPenOld = pDC->SelectObject(&pen);
	pDC->MoveTo(rc.right - (_SIZE + _MARGIN), rc.bottom - _MARGIN - 1);
	pDC->LineTo(rc.right - _MARGIN, rc.bottom - (_SIZE + _MARGIN) - 1);
	pDC->SelectObject(pPenOld);
}

void CFlatColorButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFlatButton::OnLButtonDown(nFlags, point);

	CDlgColorSwatch *pDlg = CDlgColorSwatch::Instance();

	pDlg->SetOwner(this);

	SetFocus();

	// if a CSmartEdit validation failed, we will lost focus
	if (GetFocus() != this)
		return;

	pDlg->SetColor(m_nColor, m_nAValue);

	CRect rc;
	GetWindowRect(&rc);

	CRect rc2;
	pDlg->GetWindowRect(&rc2);

	rc2.OffsetRect(rc.left - rc2.left - 1, rc.bottom - rc2.top);

	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	if (cyScreen - rc.bottom < rc2.Height())
		rc2.OffsetRect(0, -(rc.Height() + rc2.Height()));

	EnsureWholeRectVisible(rc2);


	pDlg->SetWindowPos(&wndTopMost, rc2.left, rc2.top, rc2.Width(), rc2.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_COLORSWATCHDROPDOWN), (LPARAM)m_hWnd);

	ShowColorButtonTip(rc.CenterPoint());

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		BOOL bVisible = pDlg->IsWindowVisible();

		if (bVisible)
			pDlg->RelayEvent(&msg);

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (!bVisible)
			break;
	}
}

void CFlatColorButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFlatButton::OnMouseMove(nFlags, point);

	CRect rc;
	GetWindowRect(&rc);
	ClientToScreen(&point);
	if (!PtInRect(&rc, point))
		ReleaseCapture();
}

void CFlatColorButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CFlatButton::PreSubclassWindow();
}

LRESULT CFlatColorButton::OnColorChange(WPARAM wp, LPARAM lp)
{
	SetColor(CDlgColorSwatch::Instance()->GetColor(), m_nAValue);
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_COLORCHANGE), (LPARAM)m_hWnd);

	return TRUE;
}

LRESULT  CFlatColorButton::OnColorAdvanced(WPARAM wp, LPARAM lp)
{
	CDlgColorAdvanced dlg;

	dlg.EnableAlpha(m_bEnableAlpha);
	dlg.SetColor(GetColor(), m_bEnableAlpha ? m_nAValue : 255);

	if (dlg.DoModal() == IDOK)
	{
		SetColor(dlg.GetColor(), dlg.GetAlpha());
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_COLORCHANGE), (LPARAM)m_hWnd);
	}

	return TRUE;
}

void CFlatColorButton::OnKillFocus(CWnd* pNewWnd) 
{
	CFlatButton::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
	if (::IsWindow(CDlgColorSwatch::Instance()->m_hWnd))
		CDlgColorSwatch::Instance()->ShowWindow(SW_HIDE);
}

int CFlatColorButton::GetAlpha()
{
	return m_nAValue;
}

void CFlatColorButton::EnableAlpha(BOOL bEnable)
{
	m_bEnableAlpha = bEnable;
}

void CFlatColorButton::OnDestroy()
{
	CFlatButton::OnDestroy();

	// TODO: Add your message handler code here
}
