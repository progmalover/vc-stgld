// RuleWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "RuleWnd.h"
#include "rulewnd.h"
#include <math.h>
#include "my_app.h"
#include ".\rulewnd.h"
#include "event_code.h"
#include "gldMovieClip.h"
#include "GlandaDoc.h"
#include "gldScene2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CRuleWnd

IMPLEMENT_DYNAMIC(CRuleWnd, CWnd)
IMPLEMENT_SINGLETON(CRuleWnd)

CRuleWnd::CRuleWnd()
{
	m_SiblingWnd = NULL;	
	m_ShowRule = (AfxGetApp()->GetProfileInt("Views\\Ruler", "Show", 0) != 0);

	m_HSmallFont.CreatePointFont(7 * 10, "Small Fonts", NULL);
	LOGFONT logFnt;
	m_HSmallFont.GetLogFont(&logFnt);
	logFnt.lfEscapement = 900;
	m_VSmallFont.CreateFontIndirect(&logFnt);

	HDC hDC = ::GetDC(NULL);
	ASSERT(hDC != NULL);
	HFONT oldFont = (HFONT)::SelectObject(hDC, m_HSmallFont.m_hObject);
	ASSERT(oldFont != NULL);
	TCHAR txt[] = "0123456789";
	SIZE sz;
	VERIFY(::GetTextExtentPoint(hDC, txt, (int)strlen(txt), &sz));
	::SelectObject(hDC, oldFont);
	::ReleaseDC(NULL, hDC);
	m_SGradHeight = 2;
	m_MGradHeight = 4;
	m_LGradHeight = m_MGradHeight + sz.cy + 2;		
	m_RuleWidth = m_LGradHeight + 2;
	
	BITMAPINFO		bi;
	BYTE			*pBits = NULL;

	memset(&bi, 0, sizeof(bi));	
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biHeight = 1;
	bi.bmiHeader.biWidth = 1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);	
	
	VERIFY(m_HRuleBmp.Attach(::CreateDIBSection(0, &bi, DIB_RGB_COLORS, (void **)&pBits, 0, 0)));
	VERIFY(m_VRuleBmp.Attach(::CreateDIBSection(0, &bi, DIB_RGB_COLORS, (void **)&pBits, 0, 0)));

	m_HStart = -1;
	m_HEnd = -1;
	m_VStart = -1;
	m_VEnd = -1;
}

CRuleWnd::~CRuleWnd()
{
	m_HRuleBmp.DeleteObject();
	m_VRuleBmp.DeleteObject();
}


BEGIN_MESSAGE_MAP(CRuleWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()	
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(UM_BEGIN_DRAG, OnBeginDrag)
	ON_MESSAGE(UM_CANCEL_DRAG, OnCancelDrag)
	ON_MESSAGE(UM_DRAG_OVER, OnDragOver)
	ON_MESSAGE(UM_DRAG_DROP, OnDragDrop)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CRuleWnd message handlers


void CRuleWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	if (m_ShowRule)
	{	
		CRect	clipBox;
		
		// Get clip box
		int ret = dc.GetClipBox(&clipBox);		
		if (ret == ERROR) return;
		if (ret == NULLREGION) GetClientRect(&clipBox);

		//TRACE("Invalidate Rect:(%d, %d, %d, %d)\n", clipBox.left, clipBox.top,
		//	clipBox.right, clipBox.bottom);

		DrawRule(&dc, clipBox);		
	}
}

BOOL CRuleWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}


void CRuleWnd::SetSibling(CWnd *sibling)
{
	ASSERT(sibling != NULL);

	m_SiblingWnd = sibling;
	
	m_HRuleTool.SetDesignWnd(sibling);
	m_VRuleTool.SetDesignWnd(sibling);	
}

BOOL CRuleWnd::ShowRule()
{
	return m_ShowRule;
}

void CRuleWnd::ShowRule(BOOL show)
{
	m_ShowRule = show;
}

int CRuleWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;	
	
	m_VRuleTool.Hwnd(m_hWnd);
	m_HRuleTool.Hwnd(m_hWnd);	

	return 0;
}


void CRuleWnd::DrawRule(CDC *pDC, const CRect &clip)
{
	TMatrix mx = my_app.ScreenToDataSpace();	
	ASSERT(mx.m_e11 == mx.m_e22);
	double	scale = mx.m_e11;
	CFont *oldFont = (CFont *)pDC->SelectObject(&m_HSmallFont);
	ASSERT(oldFont != NULL);
	CSize sz = pDC->GetTextExtent("5");
	pDC->SelectObject(oldFont);
	double		pixPerGrad = max(5, sz.cx + 1);
	ASSERT(scale > 0);
	int stepGrad = (int)(pixPerGrad * scale);
	// 将stepGrad转换为容易计算的刻度如(1, 2, 5, 10, 20, 30...)
	if (stepGrad < 10)
	{
		if (stepGrad > 5)
			stepGrad = 10;
		else if (stepGrad > 2)
			stepGrad = 5;
		else if (stepGrad > 1)
			stepGrad = 2;
		else
			stepGrad = 1;
	}
	else
	{
		stepGrad = (stepGrad + 9) / 10 * 10;
	}
	// 计算规范化后的pixPerGrad
	pixPerGrad = stepGrad / scale;
	// 计算开始刻度	
	int maxGrad = stepGrad * 10;
	int xGrad = 0, yGrad = 0;
	mx.Transform(xGrad, yGrad);
	xGrad = TWIPS_TO_PIXEL(xGrad);
	yGrad = TWIPS_TO_PIXEL(yGrad);
	xGrad = xGrad / stepGrad * stepGrad;
	yGrad = yGrad / stepGrad * stepGrad;
	// 计算开始坐标
	int x = PIXEL_TO_TWIPS(xGrad);
	int y = PIXEL_TO_TWIPS(yGrad);
	my_app.DataSpaceToScreen(x, y);
	x = TWIPS_TO_PIXEL(x);
	y = TWIPS_TO_PIXEL(y);
	// 画刻度
	CRect	rcClient;
	GetClientRect(&rcClient);
	CRect	rcHRule, rcVRule, rcInv;
	rcHRule.SetRect(m_RuleWidth, 0, rcClient.right, m_RuleWidth);
	rcVRule.SetRect(0, 0, m_RuleWidth, rcClient.bottom);
	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(pDC))
		return;	
	POINT	org = {0, 0};
	SiblingToClient(org);
	if (rcInv.IntersectRect(&rcHRule, &clip))
	{
		CBitmap *oldBmp = (CBitmap *)dcMem.SelectObject(&m_HRuleBmp);
		if (oldBmp == NULL) return;
		dcMem.FillSolidRect(&rcInv, GetSysColor(COLOR_BTNFACE));
		DrawHorzRule(&dcMem, x + org.x, xGrad, pixPerGrad, stepGrad, maxGrad);
		DrawHorzVernier(&dcMem);
		dcMem.DrawEdge(&rcHRule, BDR_SUNKENOUTER, BF_TOP | BF_RIGHT);
		pDC->BitBlt(rcInv.left, rcInv.top, rcInv.Width(), rcInv.Height(), &dcMem,
			rcInv.left, rcInv.top, SRCCOPY);
		dcMem.SelectObject(oldBmp);
	}
	if (rcInv.IntersectRect(&rcVRule, &clip))
	{
		CBitmap *oldBmp = (CBitmap *)dcMem.SelectObject(&m_VRuleBmp);
		if (oldBmp == NULL) return;
		dcMem.FillSolidRect(&rcInv, GetSysColor(COLOR_BTNFACE));
		DrawVertRule(&dcMem, y + org.y, yGrad, pixPerGrad, stepGrad, maxGrad);
		DrawVertVernier(&dcMem);
		dcMem.DrawEdge(&rcVRule, BDR_SUNKENOUTER, BF_LEFT | BF_BOTTOM);
		CRect rcSpace(0, 0, m_RuleWidth, m_RuleWidth);
		if (rcSpace.IntersectRect(&rcSpace, &clip))
		{
			dcMem.FillSolidRect(&rcSpace, GetSysColor(COLOR_BTNFACE));
			dcMem.DrawEdge(&rcSpace, BDR_SUNKENOUTER, BF_LEFT | BF_TOP);
			//dcMem.Draw3dRect(&rcSpace, GetSysColor(COLOR_3DSHADOW),
			//	GetSysColor(COLOR_3DHILIGHT));
		}
		pDC->BitBlt(rcInv.left, rcInv.top, rcInv.Width(), rcInv.Height(), &dcMem,
			rcInv.left, rcInv.top, SRCCOPY);
		dcMem.SelectObject(oldBmp);
	}	
}

void CRuleWnd::DrawHorzRule(CDC *pDC, int sttPix, int sttGrad, double pixPerGrad, int stepGrad, int maxGrad)
{
	CRect	rc;
	double	x = sttPix;
	int		grad = sttGrad;
	CString	label;

	GetClientRect(&rc);

	CFont	*oldFont = (CFont *)pDC->SelectObject(&m_HSmallFont);
	int		oldMode = pDC->SetBkMode(TRANSPARENT);

	while (x < rc.Width() + 1)
	{
		if (grad % maxGrad == 0)	// Large-graduation
		{
			pDC->MoveTo((int)x, 2);
			pDC->LineTo((int)x, m_RuleWidth - 1);

			label.Format("%d", abs(grad));
			pDC->TextOut((int)x + 2, 3, label);
		}
		else if (grad % 2 == 0)	// Medi-graduation
		{
			pDC->MoveTo((int)x, m_RuleWidth - 2 - m_MGradHeight);
			pDC->LineTo((int)x, m_RuleWidth - 1);
		}
		else	// Small-graduation
		{
			pDC->MoveTo((int)x, m_RuleWidth - 2 - m_SGradHeight);
			pDC->LineTo((int)x, m_RuleWidth - 1);
		}

		x += pixPerGrad;
		grad += stepGrad;
	}

	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldMode);
}

void CRuleWnd::DrawVertRule(CDC *pDC, int sttPix, int sttGrad, double pixPerGrad, int stepGrad, int maxGrad)
{
	CRect	rc;
	double	y = sttPix;
	int		grad = sttGrad;
	CString	label;

	GetClientRect(&rc);

	CFont	*oldFont = (CFont *)pDC->SelectObject(&m_VSmallFont);
	int		oldMode = pDC->SetBkMode(TRANSPARENT);
	CSize	szText;

	while (y < rc.Height() + 1)
	{
		if (grad % maxGrad == 0) // Large-graduation
		{
			pDC->MoveTo(2, (int)y);
			pDC->LineTo(m_RuleWidth - 1, (int)y);

			label.Format("%d", abs(grad));
			szText = pDC->GetTextExtent(label);
			pDC->TextOut(3, (int)(y + max(szText.cx, szText.cy) + 3), label);
		}
		else if (grad % 2 == 0) // Medi-graduation
		{
			pDC->MoveTo(m_RuleWidth - 2 - m_MGradHeight, (int)y);
			pDC->LineTo(m_RuleWidth - 1, (int)y);
		}
		else	// Small-graduation
		{
			pDC->MoveTo(m_RuleWidth - 2 - m_SGradHeight, (int)y);
			pDC->LineTo(m_RuleWidth - 1, (int)y);
		}		

		y += pixPerGrad;
		grad += stepGrad;
	}

	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldMode);
}

void CRuleWnd::DrawHorzVernier(CDC *pDC)
{
	bool pixel = true;

	for (int y = 2; y < m_RuleWidth - 1; y++)
	{
		if (pixel)
			pDC->SetPixel(m_HStart, y, RGB(0, 0, 0));
		else
			pDC->SetPixel(m_HStart, y, RGB(255, 255, 255));
		pixel = !pixel;
	}
	
	if (m_HEnd != m_HStart)
	{
		pixel = true;

		for (int y = 2; y < m_RuleWidth - 1; y++)
		{
			if (pixel)
				pDC->SetPixel(m_HEnd, y, RGB(0, 0, 0));
			else
				pDC->SetPixel(m_HEnd, y, RGB(255, 255, 255));
			pixel = !pixel;
		}
	}
}

void CRuleWnd::DrawVertVernier(CDC *pDC)
{
	bool pixel = true;

	for (int x = 2; x < m_RuleWidth - 1; x++)
	{
		if (pixel)
			pDC->SetPixel(x, m_VStart, RGB(0, 0, 0));
		else
			pDC->SetPixel(x, m_VStart, RGB(255, 255, 255));
		pixel = !pixel;
	}
	
	if (m_VEnd != m_VStart)
	{
		pixel = true;

		for (int x = 2; x < m_RuleWidth - 1; x++)
		{
			if (pixel)
				pDC->SetPixel(x, m_VEnd, RGB(0, 0, 0));
			else
				pDC->SetPixel(x, m_VEnd, RGB(255, 255, 255));
			pixel = !pixel;
		}
	}
}

IMPLEMENT_OBSERVER(CRuleWnd, ChangeMapMode)
{
	Invalidate(FALSE);

	UpdateWindow();
}


void CRuleWnd::SetVernierPos(int h, int v, int hLength /*= 0*/, int vLength /*= 0*/)
{	
	if (m_ShowRule)
	{
		int hs = m_HStart;
		int vs = m_VStart;
		int	he = m_HEnd;
		int ve = m_VEnd;

		m_HStart = h;
		m_VStart = v;
		SiblingToClient(m_HStart, m_VStart);

		m_HEnd = h + hLength;
		m_VEnd = v + vLength;
		SiblingToClient(m_HEnd, m_VEnd);

		CRect rcInv;

		rcInv.SetRect(hs, 0, he, m_RuleWidth);
		rcInv.NormalizeRect();
		rcInv.InflateRect(1, 0, 1, 0);
		InvalidateRect(&rcInv, FALSE);		

		rcInv.SetRect(0, vs, m_RuleWidth, ve);
		rcInv.NormalizeRect();
		rcInv.InflateRect(0, 1, 0, 1);
		InvalidateRect(&rcInv, FALSE);

		rcInv.SetRect(m_HStart, 0, m_HEnd, m_RuleWidth);
		rcInv.NormalizeRect();
		rcInv.InflateRect(1, 0, 1, 0);
		InvalidateRect(&rcInv, FALSE);		

		rcInv.SetRect(0, m_VStart, m_RuleWidth, m_VEnd);
		rcInv.NormalizeRect();
		rcInv.InflateRect(0, 1, 0, 1);
		InvalidateRect(&rcInv, FALSE);

		UpdateWindow();
	}
}

void CRuleWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect hBar, vBar, rc;
	
	GetClientRect(&rc);
	hBar.SetRect(m_RuleWidth, 0, rc.Width(), m_RuleWidth);
	vBar.SetRect(0, m_RuleWidth, m_RuleWidth, rc.Height());

	if (hBar.PtInRect(point))
	{
		m_HRuleTool.Track(point);
	}
	else if (vBar.PtInRect(point))
	{
		m_VRuleTool.Track(point);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

LRESULT CRuleWnd::OnBeginDrag(WPARAM wParam, LPARAM lParam)
{
	if (m_SiblingWnd != NULL)
	{
		RECT rc = *(RECT *)lParam;

		ClientToSibling(rc);

		return m_SiblingWnd->SendMessage(UM_BEGIN_DRAG, wParam, (LPARAM)&rc);
	}
	else
	{
		return 0;
	}
}

LRESULT CRuleWnd::OnCancelDrag(WPARAM wParam, LPARAM lParam)
{
	if (m_SiblingWnd != NULL)
	{
		RECT rc = *(RECT *)lParam;

		ClientToSibling(rc);

		return m_SiblingWnd->SendMessage(UM_CANCEL_DRAG, wParam, (LPARAM)&rc);
	}
	else
	{
		return 0;
	}
}

LRESULT CRuleWnd::OnDragOver(WPARAM wParam, LPARAM lParam)
{
	if (m_SiblingWnd != NULL)
	{
		RECT rc = *(RECT *)lParam;

		ClientToSibling(rc);

		return m_SiblingWnd->SendMessage(UM_DRAG_OVER, wParam, (LPARAM)&rc);
	}
	else
	{
		return 0;
	}
}

LRESULT CRuleWnd::OnDragDrop(WPARAM wParam, LPARAM lParam)
{
	if (m_SiblingWnd != NULL)
	{
		RECT rc = *(RECT *)lParam;

		ClientToSibling(rc);

		return m_SiblingWnd->SendMessage(UM_DRAG_DROP, wParam, (LPARAM)&rc);
	}
	else
	{
		return 0;
	}
}

void CRuleWnd::ClientToSibling(POINT &pt)
{	
	ClientToScreen(&pt);
	m_SiblingWnd->ScreenToClient(&pt);
}

void CRuleWnd::ClientToSibling(RECT &rc)
{
	ClientToScreen(&rc);
	m_SiblingWnd->ScreenToClient(&rc);
}

void CRuleWnd::SiblingToClient(POINT &pt)
{
	m_SiblingWnd->ClientToScreen(&pt);
	ScreenToClient(&pt);
}

void CRuleWnd::SiblingToClient(int &x, int &y)
{
	POINT pt = {x, y};

	SiblingToClient(pt);

	x = pt.x;
	y = pt.y;
}

int CRuleWnd::GetRuleSize()
{
	return m_RuleWidth;
}

// CRuleTool
CRuleTool::CRuleTool()
{
	m_pDC = NULL;
	m_OldR2 = 0;
	m_DragPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_OldPen = NULL;
	m_DesignWnd = NULL;
}

CRuleTool::~CRuleTool()
{
}

void CRuleTool::SetDesignWnd(CWnd *pWnd)
{
	ASSERT(pWnd != NULL);

	m_DesignWnd = pWnd;
}

void CRuleTool::PrepareDC()
{
	ASSERT(m_DesignWnd != NULL);

	m_pDC = m_DesignWnd->GetDC();	
	ASSERT(m_pDC != NULL);
	m_OldR2 = m_pDC->SetROP2(R2_NOTXORPEN);
	m_OldPen = (CPen *)m_pDC->SelectObject(&m_DragPen);
	ASSERT(m_OldPen != NULL);
}

void CRuleTool::UnprepareDC()
{
	ASSERT(m_DesignWnd != NULL);
	ASSERT(m_pDC != NULL);

	m_pDC->SetROP2(m_OldR2);
	m_pDC->SelectObject(m_OldPen);
	m_DesignWnd->ReleaseDC(m_pDC);
	m_pDC = NULL;
}

POINT CRuleTool::GetPoint(const POINT &p)
{
	POINT	result = p;

	ClientToScreen(_M_hwnd, &result);
	ScreenToClient(m_DesignWnd->m_hWnd, &result);

	return result;
}

// CVRuleTool
CVRuleTool::CVRuleTool()
{
	m_PrevX = 0;
}

void CVRuleTool::OnBeginDrag(UINT nFlags, const POINT &point)
{
	PrepareDC();

	CRect rc;
	
	m_DesignWnd->GetClientRect(&rc);
	m_pDC->MoveTo(point.x, 0);
	m_pDC->LineTo(point.x, rc.Height());

	m_PrevX = point.x;
}

void CVRuleTool::OnDragOver(UINT nFlags, const POINT &point)
{
	CRect rc;
	
	m_DesignWnd->GetClientRect(&rc);
	m_pDC->MoveTo(m_PrevX, 0);
	m_pDC->LineTo(m_PrevX, rc.Height());
	m_pDC->MoveTo(point.x, 0);
	m_pDC->LineTo(point.x, rc.Height());

	m_PrevX = point.x;
}

void CVRuleTool::OnCancelDrag(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	m_DesignWnd->Invalidate(FALSE);
}

void CVRuleTool::OnDragDrop(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	m_DesignWnd->Invalidate(FALSE);

	CRect	rc;
	m_DesignWnd->GetClientRect(&rc);
	if (point.x >= rc.left && point.x <= rc.right)
	{
		int x = PIXEL_TO_TWIPS(point.x), y = 0;
		my_app.ScreenToDataSpace(x, y);
		my_app.Commands().Do(new CCmdAddRule(x, false));
	}
}


// CHRuleTool
CHRuleTool::CHRuleTool()
{
	m_PrevY = 0;
}

void CHRuleTool::OnBeginDrag(UINT nFlags, const POINT &point)
{
	PrepareDC();

	CRect rc;
	
	m_DesignWnd->GetClientRect(&rc);
	m_pDC->MoveTo(0, point.y);
	m_pDC->LineTo(rc.Width(), point.y);

	m_PrevY = point.y;
}

void CHRuleTool::OnDragOver(UINT nFlags, const POINT &point)
{
	CRect rc;
	
	m_DesignWnd->GetClientRect(&rc);
	m_pDC->MoveTo(0, m_PrevY);
	m_pDC->LineTo(rc.Width(), m_PrevY);
	m_pDC->MoveTo(0, point.y);
	m_pDC->LineTo(rc.Width(), point.y);

	m_PrevY = point.y;
}

void CHRuleTool::OnCancelDrag(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	m_DesignWnd->Invalidate(FALSE);
}

void CHRuleTool::OnDragDrop(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	m_DesignWnd->Invalidate(FALSE);

	CRect	rc;
	m_DesignWnd->GetClientRect(&rc);
	if (point.y >= rc.top && point.y <= rc.bottom)
	{
		int x = 0, y = PIXEL_TO_TWIPS(point.y);
		my_app.ScreenToDataSpace(x, y);
		my_app.Commands().Do(new CCmdAddRule(y, true));
	}
}

// CCmdAddRule
CCmdAddRule::CCmdAddRule(int pos, bool horz)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
{
	_M_Desc_ID = IDS_CMD_ADDAUXILIARYLINE;

	if (horz)
	{
		m_Horz = true;
		m_Vert = false;		
		m_HPos = pos;
	}
	else
	{
		m_Horz = false;
		m_Vert = true;
		m_VPos = pos;
	}
}

CCmdAddRule::CCmdAddRule(int hPos, int vPos)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
{
	_M_Desc_ID = IDS_CMD_ADDAUXILIARYLINE;

	m_Vert = true;
	m_Horz = true;
	m_HPos = hPos;
	m_VPos = vPos;	
}

bool CCmdAddRule::Execute()
{	
	gldScene2 *scene = _GetCurScene2();	
	ASSERT(scene != NULL);

	if (m_Vert)
	{
		int vIndex = my_app.AddAuxLine(m_VPos, false);
		if (vIndex == -1)
			m_Vert = false;
		else
			scene->m_vLines.insert(scene->m_vLines.begin() + vIndex, m_VPos);
	}

	if (m_Horz)
	{
		int hIndex = my_app.AddAuxLine(m_HPos, true);
		if (hIndex == -1)
			m_Horz = false;
		else
			scene->m_hLines.insert(scene->m_hLines.begin() + hIndex, m_HPos);
	}

	if (!m_Vert && !m_Horz)
	{
		return false;
	}
	else
	{		
		UpdateUniId();

		my_app.Repaint();

		return TCommand::Execute();
	}
}

bool CCmdAddRule::Unexecute()
{
	gldScene2 *scene = _GetCurScene2();	
	ASSERT(scene != NULL);

	if (m_Vert)
	{
		int vIndex = my_app.RemoveAuxLine(m_VPos, false);

		ASSERT(vIndex != -1);
		ASSERT(scene->m_vLines[vIndex] == m_VPos);
		scene->m_vLines.erase(scene->m_vLines.begin() + vIndex);
	}

	if (m_Horz)
	{
		int hIndex = my_app.RemoveAuxLine(m_HPos, true);

		ASSERT(hIndex != -1);
		ASSERT(scene->m_hLines[hIndex] == m_HPos);
		scene->m_hLines.erase(scene->m_hLines.begin() + hIndex);
	}

	UpdateUniId();

	my_app.Repaint();

	return TCommand::Unexecute();
}

// CCmdRemoveRule
CCmdRemoveRule::CCmdRemoveRule(int pos, bool horz)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
{
	m_Horz = horz;
	m_Pos = pos;

	_M_Desc_ID = IDS_CMD_REMOVEAUXILIARYLINE;
}

bool CCmdRemoveRule::Execute()
{	
	int index = my_app.RemoveAuxLine(m_Pos, m_Horz);

	if (index != -1)
	{
		gldScene2 *scene = _GetCurScene2();	
		ASSERT(scene != NULL);
		if (m_Horz)
		{
			ASSERT(scene->m_hLines[index] == m_Pos);
			scene->m_hLines.erase(scene->m_hLines.begin() + index);
		}
		else
		{
			ASSERT(scene->m_vLines[index] == m_Pos);
			scene->m_vLines.erase(scene->m_vLines.begin() + index);
		}
		UpdateUniId();

		my_app.Repaint();

		return TCommand::Execute();
	}
	else
	{
		return false;
	}
}

bool CCmdRemoveRule::Unexecute()
{
	UpdateUniId();

	int index = my_app.AddAuxLine(m_Pos, m_Horz);
	if (index != -1)
	{
		gldScene2 *scene = _GetCurScene2();	
		ASSERT(scene != NULL);

		if (m_Horz)
		{
			scene->m_hLines.insert(scene->m_hLines.begin() + index, m_Pos);
		}
		else
		{
			scene->m_vLines.insert(scene->m_vLines.begin() + index, m_Pos);
		}
	}

	my_app.Repaint();

	return TCommand::Unexecute();
}

// CCmdMoveRule
CCmdMoveRule::CCmdMoveRule(int oldPos, int newPos, bool horz)
: CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
{
	m_OldPos = oldPos;
	m_NewPos = newPos;
	m_Horz = horz;
	m_Added = false;

	_M_Desc_ID = IDS_CMD_MOVEAUXILIARYLINE;
}

bool CCmdMoveRule::Execute()
{
	if (m_OldPos == m_NewPos)
		return false;

	gldScene2 *scene = _GetCurScene2();	
	ASSERT(scene != NULL);

	int rmvIndex = my_app.RemoveAuxLine(m_OldPos, m_Horz);
	ASSERT(rmvIndex != -1);
	if (m_Horz)
	{
		ASSERT(scene->m_hLines[rmvIndex] == m_OldPos);
		scene->m_hLines.erase(scene->m_hLines.begin() + rmvIndex);
	}
	else
	{
		ASSERT(scene->m_vLines[rmvIndex] == m_OldPos);
		scene->m_vLines.erase(scene->m_vLines.begin() + rmvIndex);
	}

	int addIndex = my_app.AddAuxLine(m_NewPos, m_Horz);
	m_Added = (addIndex != -1);
	if (m_Added)
	{
		if (m_Horz)
		{			
			scene->m_hLines.insert(scene->m_hLines.begin() + addIndex, m_NewPos);
		}
		else
		{
			scene->m_vLines.insert(scene->m_vLines.begin() + addIndex, m_NewPos);
		}
	}
	UpdateUniId();

	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdMoveRule::Unexecute()
{
	gldScene2 *scene = _GetCurScene2();	
	ASSERT(scene != NULL);

	if (m_Added)
	{
		int rmvIndex = my_app.RemoveAuxLine(m_NewPos, m_Horz);
		ASSERT(rmvIndex != -1);
		if (m_Horz)
		{
			ASSERT(scene->m_hLines[rmvIndex] == m_NewPos);
			scene->m_hLines.erase(scene->m_hLines.begin() + rmvIndex);
		}
		else
		{
			ASSERT(scene->m_vLines[rmvIndex] == m_NewPos);
			scene->m_vLines.erase(scene->m_vLines.begin() + rmvIndex);
		}
	}

	int addIndex = my_app.AddAuxLine(m_OldPos, m_Horz);
	ASSERT(addIndex != -1);
	if (m_Horz)
	{
		scene->m_hLines.insert(scene->m_hLines.begin() + addIndex, m_OldPos);
	}
	else
	{
		scene->m_vLines.insert(scene->m_vLines.begin() + addIndex, m_OldPos);
	}
	
	UpdateUniId();

	my_app.Repaint();

	return TCommand::Unexecute();
}

// CMoveRuleTool
CMoveRuleTool::CMoveRuleTool()
{
	m_PrevPos = 0;
	m_Horz = false;
}

void CMoveRuleTool::SetTarget(int pos, bool horz)
{
	m_Pos = pos;
	m_Horz = horz;
}

void CMoveRuleTool::OnBeginDrag(UINT nFlags, const POINT &point)
{	
	my_app.RemoveAuxLine(m_Pos, m_Horz);
	my_app.Repaint();

	PrepareDC();

	CRect rc;
	m_DesignWnd->GetClientRect(&rc);
	if (m_Horz)
	{
		int x = 0, y = m_Pos;
		my_app.DataSpaceToScreen(x, y);
		y = TWIPS_TO_PIXEL(y);
		m_pDC->MoveTo(0, y);
		m_pDC->LineTo(rc.Width(), y);
		
		m_PrevPos = y;
	}
	else
	{
		int x = m_Pos, y = 0;
		my_app.DataSpaceToScreen(x, y);
		x = TWIPS_TO_PIXEL(x);
		m_pDC->MoveTo(x, 0);
		m_pDC->LineTo(x, rc.Height());

		m_PrevPos = x;
	}
}

void CMoveRuleTool::OnDragOver(UINT nFlags, const POINT &point)
{
	CRect rc;
	m_DesignWnd->GetClientRect(&rc);

	if (m_Horz)
	{		
		m_pDC->MoveTo(0, m_PrevPos);
		m_pDC->LineTo(rc.Width(), m_PrevPos);

		m_pDC->MoveTo(0, point.y);
		m_pDC->LineTo(rc.Width(), point.y);

		m_PrevPos = point.y;
	}
	else
	{		
		m_pDC->MoveTo(m_PrevPos, 0);
		m_pDC->LineTo(m_PrevPos, rc.Height());
		
		m_pDC->MoveTo(point.x, 0);
		m_pDC->LineTo(point.x, rc.Height());

		m_PrevPos = point.x;
	}	
}

POINT CMoveRuleTool::GetPoint(const POINT &p)
{
	return p;
}

void CMoveRuleTool::OnCancelDrag(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	my_app.AddAuxLine(m_Pos, m_Horz);
	my_app.Repaint();
}

void CMoveRuleTool::OnDragDrop(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	my_app.AddAuxLine(m_Pos, m_Horz);

	CRect rc;
	m_DesignWnd->GetClientRect(&rc);
	if (rc.PtInRect(point))
	{
		int pos = 0;
		if (m_Horz)
		{
			int x = 0, y = PIXEL_TO_TWIPS(point.y);
			my_app.ScreenToDataSpace(x, y);		
			pos = y;
		}
		else
		{
			int x = PIXEL_TO_TWIPS(point.x), y = 0;
			my_app.ScreenToDataSpace(x, y);		
			pos = x;
		}
		my_app.Commands().Do(new CCmdMoveRule(m_Pos, pos, m_Horz));
	}
	else
	{
		my_app.Commands().Do(new CCmdRemoveRule(m_Pos, m_Horz));
	}
}

BOOL CRuleWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	CRect hBar, vBar, rc;
	
	GetClientRect(&rc);
	hBar.SetRect(m_RuleWidth, 0, rc.Width(), m_RuleWidth);
	vBar.SetRect(0, m_RuleWidth, m_RuleWidth, rc.Height());

	POINT point;
	VERIFY(GetCursorPos(&point));
	ScreenToClient(&point);

	if (hBar.PtInRect(point))
	{
		if (SetCursor(my_app.GetStockCursor(IDC_G_VRULER)) != NULL)
			return FALSE;
	}
	else if (vBar.PtInRect(point))
	{
		if (SetCursor(my_app.GetStockCursor(IDC_G_HRULER)) != NULL)
			return FALSE;
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CRuleWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0)
	{
		m_HRuleBmp.DeleteObject();
		m_VRuleBmp.DeleteObject();

		BITMAPINFO		bi;
		BYTE			*pBits = NULL;

		memset(&bi, 0, sizeof(bi));	
		bi.bmiHeader.biBitCount = 32;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biHeight = m_RuleWidth;
		bi.bmiHeader.biWidth = cx;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);

		VERIFY(m_HRuleBmp.Attach(::CreateDIBSection(0, &bi, DIB_RGB_COLORS, (void **)&pBits, 0, 0)));
	
		bi.bmiHeader.biHeight = cy;
		bi.bmiHeader.biWidth = m_RuleWidth;
		
		VERIFY(m_VRuleBmp.Attach(::CreateDIBSection(0, &bi, DIB_RGB_COLORS, (void **)&pBits, 0, 0)));
	}
}
