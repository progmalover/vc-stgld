#include "StdAfx.h"
#include ".\effectinplaceeditor.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "GlandaDoc.h"
#include "gldEffect.h"
#include "gldInstance.h"
#include "my_app.h"
#include "EffectCommonUtils.h"
#include <math.h>
#include "gld_func.h"
#include "DrawHelper.h"
#include "SWFProxy.h"
#include "EffectWnd.h"

#define HALF_HANDLER_SIZE	60
#define ROTATE_HANDLE_LENGTH 400

#define H_NO 0
#define H_LT 1
#define H_LM 2
#define H_LB 3
#define H_MB 4
#define H_RB 5
#define H_RM 6
#define H_RT 7
#define H_MT 8
#define H_RO 9
#define H_ED 10

//**********************************************************************************
//
// Helper function
//
//**********************************************************************************
BOOL PathIsLine(IGLD_CurvedPath *pIPath)
{
	ASSERT(pIPath != NULL);

	ULONG iCount = 0;
	HRESULT hr = pIPath->get_Count(&iCount);
	if (iCount != 3)
		return FALSE;
	SAFEARRAY *psa = NULL;
	hr = pIPath->get_Data(&psa);
	if (FAILED(hr))
		return FALSE;
	DOUBLE *pData = (DOUBLE *)psa->pvData;
	double	A = pData[5] - pData[1];
	double	B = pData[0] - pData[4];
	double	C = - pData[1] * B - pData[0] * A;	
	double	AB = A * A + B * B;
	double	DT = A * pData[2] + B * pData[3] + C;
	SafeArrayDestroy(psa);

	return DT * DT / AB < 1600;
}

inline void DrawHandler(TDraftDC &dc, int x, int y)
{
	dc.Ellipse(x - HALF_HANDLER_SIZE, y - HALF_HANDLER_SIZE, 
			x + HALF_HANDLER_SIZE + 20, y + HALF_HANDLER_SIZE + 20,
			TDraftDC::_no_trans_tag());
}

inline BOOL PtOnHandler(const CPoint &hit, const CPoint &handler)
{
	return abs(hit.x - handler.x) <= HALF_HANDLER_SIZE &&
		abs(hit.y - handler.y) <= HALF_HANDLER_SIZE;
}

inline HRESULT GetEffectPath(gldEffect *pEffect, IGLD_CurvedPath **ppIPath)
{
	ASSERT(pEffect != NULL && ppIPath != NULL);

	*ppIPath = NULL;

	return GetParameter(pEffect->m_pIParas, "Path", ppIPath);
}

inline TMatrix GetPathMatrix(gldEffect *pEffect)
{
	ASSERT(pEffect != NULL);

	CComPtr<IGLD_Matrix> pIMx;
	TMatrix mx;
	HRESULT hr = GetParameter(pEffect->m_pIParas, "Path.Matrix", &pIMx);
	if (SUCCEEDED(hr))
	{
		double e11, e12, e21, e22, dx, dy;
		if (SUCCEEDED(pIMx->GetData(&e11, &e12, &e21, &e22, &dx, &dy)))
		{
			mx.m_e11 = (float)e11;
			mx.m_e12 = (float)e12;
			mx.m_e21 = (float)e21;
			mx.m_e22 = (float)e22;
			mx.m_dx = (int)dx;
			mx.m_dy = (int)dy;
		}
	}
	return mx;
}

void DrawPath(TDraftDC &dc, DOUBLE *pData, ULONG cb)
{
	dc.MoveTo((int)pData[0], (int)pData[1], TDraftDC::_trans_tag());
	pData += 2;
	for (ULONG i = 1; i < cb; i += 2)
	{
		dc.BezierTo((int)pData[0], (int)pData[1], (int)pData[2], 
			(int)pData[3], TDraftDC::_trans_tag());
		pData += 4;
	}
}

HRESULT Path2Screen(gldInstance *pInst, gldEffect *pEffect, IGLD_Matrix **ppIMx)
{
	CComPtr<IGLD_Matrix> pIPathMx;
	HRESULT hr = S_OK;
	CComPtr<IGLD_CurvedPath> pIPath;
	hr = GetEffectPath(pEffect, &pIPath);
	CHECKRESULT();
	GetParameter(pEffect->m_pIParas, "Path.Matrix", &pIPathMx);
	if (pIPathMx == NULL)
	{
		hr = pIPathMx.CoCreateInstance(__uuidof(GLD_Matrix));
		CHECKRESULT();
	}
	ASSERT(pInst != NULL);
	int cx = pInst->m_matrix.m_x;
	int cy = pInst->m_matrix.m_y;
	double sx = 0, sy = 0;
	pIPath->GetAt(1, &sx, &sy);
	CComPtr<IGLD_Matrix> pID2S;
	hr = pID2S.CoCreateInstance(__uuidof(GLD_Matrix));
	CHECKRESULT();
	TMatrix D2S = my_app.DataSpaceToScreen();
	pID2S->SetData(D2S.m_e11, D2S.m_e12, D2S.m_e21, D2S.m_e22, D2S.m_dx, D2S.m_dy);
	double dx = 0, dy = 0;
	pIPathMx->get_Element(3, 1, &dx);
	pIPathMx->get_Element(3, 2, &dy);
	pIPathMx->Transform(&sx, &sy);
	dx += cx - sx;
	dy += cy - sy;
	pIPathMx->put_Element(3, 1, dx);
	pIPathMx->put_Element(3, 2, dy);
	CComPtr<IGLD_Matrix> pIMx;
	return pIPathMx->Multiply(pID2S, ppIMx);	
}

inline double Cross(double x1, double y1, double x2, double y2)
{
	return x1 * y2 - x2 * y1;
}
//**********************************************************************************
//
// CResizePathTool
//
//**********************************************************************************
CResizePathTool::CResizePathTool()
: m_hDC(NULL), m_hOldPen(NULL), m_nHandler(H_NO)
, m_pArrData(NULL), m_nOldR2(0)
, m_cx(0), m_cy(0), m_nArrData(0)
, m_bDo(FALSE)
{	
}

CResizePathTool::~CResizePathTool()
{	
}

POINT CResizePathTool::GetPoint(const POINT &p)
{
	int x = p.x * 20, y = p.y * 20;
	my_app.ScreenToDataSpace(x, y);
	POINT tp = {x, y};
	return tp;
}

BOOL CResizePathTool::Start(gldInstance *pInst, gldEffect *pEffect, IGLD_CurvedPath *pIPath, int nHandler, const CPoint &point)
{
	ASSERT(pInst != NULL);
	ASSERT(pEffect != NULL);
	ASSERT(pIPath != NULL);

	// get transform center
	m_cx = pInst->m_matrix.m_x;
	m_cy = pInst->m_matrix.m_y;
	m_nHandler = nHandler;

	HRESULT hr = pIPath->get_Data(&m_pArrData);
	if (FAILED(hr)) return FALSE;
	m_nArrData = 0;
	pIPath->get_Count(&m_nArrData);
	if (m_nHandler == H_ED)
	{
		m_bx = 0;
		m_by = 0;
		SAFEARRAY *psa = NULL;
		pIPath->get_Data(&psa);
		if (psa != NULL)
		{
			DOUBLE *pData = (DOUBLE *)psa->pvData;
			m_bx = pData[4];
			m_by = pData[5];
			SafeArrayDestroy(psa);
		}
	}

	m_PathMatrix = GetPathMatrix(pEffect);
	m_BasePathMatrix = m_PathMatrix;

	POINT sp = GetPoint(point);
	m_ex = m_sx = sp.x;
	m_ey = m_sy = sp.y;

	m_bDo = FALSE;

	Track(sp);

	SafeArrayDestroy(m_pArrData);
	m_pArrData = NULL;

	if (m_bDo)
	{
		Do(pEffect, pIPath);
	}
	else
	{
		InvalidateRect(_M_hwnd, NULL, FALSE);
	}

	return TRUE;
}

void CResizePathTool::PrepareDC()
{
	ASSERT(_M_hwnd != NULL);

	m_hDC = ::GetDC(_M_hwnd);
	ASSERT(m_hDC != NULL);
	m_hEffectPen = my_app.GetStockPen(SEP_AUXILIARY);
	m_hOldPen = (HPEN)::SelectObject(m_hDC, m_hEffectPen);
	m_nOldR2 = ::SetROP2(m_hDC, R2_NOTXORPEN);
}

void CResizePathTool::UnprepareDC()
{
	if (m_hOldPen != NULL)
		::SelectObject(m_hDC, m_hOldPen);
	::SetROP2(m_hDC, m_nOldR2);
	ReleaseDC(_M_hwnd, m_hDC);
}

void CResizePathTool::OnBeginDrag(UINT nFlags, const POINT &point)
{
	PrepareDC();

	DrawEffect();
}

void CResizePathTool::OnDragOver(UINT nFlags, const POINT &point)
{
	DrawEffect();

	m_ex = point.x;
	m_ey = point.y;
	RecalcPathMatrix(nFlags);

	DrawEffect();
}

void CResizePathTool::OnCancelDrag(UINT nFlags, const POINT &point)
{
	UnprepareDC();
	m_bDo = FALSE;
}

void CResizePathTool::OnDragDrop(UINT nFlags, const POINT &point)
{
	UnprepareDC();
	m_bDo = TRUE;
}

void CResizePathTool::DrawEffect()
{
	if (m_nArrData > 0)
	{
		int x = (int)((DOUBLE *)(m_pArrData->pvData))[0];
		int y = (int)((DOUBLE *)(m_pArrData->pvData))[1];
		m_PathMatrix.Transform(x, y);
		TMatrix mx(m_PathMatrix);
		mx.m_dx += (int)(m_cx - x);
		mx.m_dy += (int)(m_cy - y);
		mx = mx * my_app.DataSpaceToScreen();
		TDraftDC dc(m_hDC);
		dc.Matrix(mx);
		::DrawPath(dc, (DOUBLE *)m_pArrData->pvData, m_nArrData);
	}
}

void CResizePathTool::Do(gldEffect *pEffect, IGLD_CurvedPath *pIPath)
{
	if (m_nHandler == H_ED)
	{
		my_app.Commands().Do(new CCmdRelinePath(pIPath, m_ex - m_sx, m_ey - m_sy));
	}
	else
	{
		my_app.Commands().Do(new CCmdTransformPath(pEffect, m_PathMatrix));
	}
}

void CResizePathTool::RecalcPathMatrix(UINT nFlags)
{	
	if (m_nArrData == 0) return;	
	
	int cx = (int)((DOUBLE *)(m_pArrData->pvData))[0];
	int cy = (int)((DOUBLE *)(m_pArrData->pvData))[1];
	int sx = (int)m_sx;
	int sy = (int)m_sy;
	int ex = (int)m_ex;
	int ey = (int)m_ey;	

	if (m_nHandler == H_RO) // rotate
	{
		m_BasePathMatrix.Transform(cx, cy);

		int dx1 = sx - cx;
		int dx2 = ex - cx;
		int dy1 = sy - cy;
		int dy2 = ey - cy;
		float r1 = sqrtf((float)dx1 * dx1 + (float)dy1 * dy1);
		float r2 = sqrtf((float)dx2 * dx2 + (float)dy2 * dy2);

		if (fabsf(r1) < 1E-4 || fabsf(r2) < 1E-4)
			return;

		float s1 = dy1 / r1;
		float c1 = dx1 / r1;
		float s2 = dy2 / r2;
		float c2 = dx2 / r2;
		float s = s2 * c1 - c2 * s1;
		float c = c2 * c1 + s2 * s1;

		TMatrix roMx(c, s, -s, c, 0, 0);
		m_PathMatrix = m_BasePathMatrix * roMx;
	}
	else if (m_nHandler == H_ED) // line end point
	{
		DOUBLE *pData = (DOUBLE *)m_pArrData->pvData;
		pData[4] = m_bx + ex - sx;
		pData[5] = m_by + ey - sy;
		pData[2] = (pData[0] + pData[4]) / 2;
		pData[3] = (pData[1] + pData[5]) / 2;		
	}
	else if (m_nHandler != H_NO)// scale
	{
		TMatrix rmx = m_BasePathMatrix.Invert();		
		rmx.Transform(sx, sy);
		rmx.Transform(ex, ey);

		double scalex = 1, scaley = 1;

		if (m_nHandler % 2 != 0) // scale at x and y
		{
			if (sx - cx != 0)
				scalex = (double)(ex - cx) / (sx - cx);
			if (sy - cy != 0)
				scaley = (double)(ey - cy) / (sy - cy);
			if ((nFlags & MK_CONTROL) != 0)
			{
				double scale = __min(fabs(scalex), fabs(scaley));
				if (scale * scalex < 0)
					scalex = - scale;
				else
					scalex = scale;
				if (scale * scaley < 0)
					scaley = - scale;
				else
					scaley = scale;
			}
		}
		else if (m_nHandler == 2 || m_nHandler == 6) // scale at x
		{
			if (sx - cx != 0)
				scalex = (double)(ex - cx) / (sx - cx);
		}
		else // scale at y
		{
			if (sy - cy != 0)
				scaley = (double)(ey - cy) / (sy - cy);
		}

		if (fabs(scalex) < 1e-4 || fabs(scaley) < 1e-4)
			return;

		TMatrix scaleMx;
		scaleMx.SetMatrix((float)scalex, 0, 0, (float)scaley, 0, 0);
		m_PathMatrix = scaleMx * m_BasePathMatrix;
	}
}


//***********************************************************************************
//
// CEffectInplaceEditor
//
//***********************************************************************************

CEffectInplaceEditor::CEffectInplaceEditor(void)
{	
}

CEffectInplaceEditor::~CEffectInplaceEditor(void)
{
}

IMPLEMENT_OBSERVER(CEffectInplaceEditor, ChangeTimelineSel)
{
	CGuardDrawOnce xDraw;

	my_app.CurTool()->Leave();

	int selType = CEffectWnd::Instance()->GetSelType();

	if (selType == CEffectWnd::SEL_INST ||
		selType == CEffectWnd::SEL_SOUND ||
		selType == CEffectWnd::SEL_SCAPT ||
		selType == CEffectWnd::SEL_EFFECT)
	{		
		gld_shape_sel sel;
		gldInstance *pInst = CEffectWnd::Instance()->GetSelInstance();
		if (pInst != NULL)
		{
			gld_shape shape = CTraitShape(pInst);
			ASSERT(shape.validate());		
			sel.select(shape);		
		}

		my_app.DoSelect(sel);
		my_app.Repaint();
	}
	else if (selType == CEffectWnd::SEL_VOID)
	{
		my_app.Repaint();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CEffectInplaceEditor::DrawPath(HDC hDC, IGLD_CurvedPath *pIPath, IGLD_Matrix *pIMx)
{
	ASSERT(hDC != NULL);
	ASSERT(pIPath != NULL);
	ASSERT(pIMx != NULL);	

	SAFEARRAY *psap = NULL;	

	HRESULT hr = pIPath->get_Data(&psap);
	if (FAILED(hr) || (psap == NULL))
		return;

	LONG lb = 0, ub = 0, cb = 0;
	COMVERIFY(SafeArrayGetLBound(psap, 1, &lb));
	COMVERIFY(SafeArrayGetUBound(psap, 1, &ub));
	cb = ub - lb + 1;
	if (cb < 1)
	{
		SafeArrayDestroy(psap);
		return;
	}	
	
	TDraftDC dc(hDC);	
	double e11 = 1, e12 = 0, e21 = 0, e22 = 1, dx = 0, dy = 0;
	pIMx->GetData(&e11, &e12, &e21, &e22, &dx, &dy);
	TMatrix mx((float)e11, (float)e12, (float)e21, (float)e22, (long)dx, (long)dy);
	dc.Matrix(mx);
	
	// draw arrow
	int adx = 0, ady = 0;	
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	double *pData = (double *)psap->pvData;
	double *pd = pData;
	while (pd < pData + 2 * cb)
	{
		x1 = (int)pd[0];
		y1 = (int)pd[1];
		x2 = (int)pd[2];		
		y2 = (int)pd[3];
		mx.Transform(x1, y1);
		mx.Transform(x2, y2);
		adx = x2 - x1;
		ady = y2 - y1;
		if (abs(adx) > 0 || abs(ady) > 0)
			break;
		x2 = (int)pd[4];
		y2 = (int)pd[5];
		mx.Transform(x2, y2);
		adx = x2 - x1;
		ady = y2 - y1;
		if (abs(adx) > 0 || abs(ady) > 0)
			break;
		pd += 4;
	}
	if (abs(adx) > 0 || abs(ady) > 0)
	{
		double r = sqrt((double)(adx * adx + ady * ady));
		double s = ady / r;
		double c = adx / r;
		POINT arrow[3];
		arrow[0].x = (x1 + (int)(200 * c)) / 20;
		arrow[0].y = (y1 + (int)(200 * s)) / 20;
		arrow[1].x = (x1 - (int)(120 * s)) / 20;
		arrow[1].y = (y1 + (int)(120 * c)) / 20;
		arrow[2].x = 2 * x1 / 20 - arrow[1].x;
		arrow[2].y = 2 * y1 / 20 - arrow[1].y;
		HPEN oldPen = (HPEN)::SelectObject(hDC, my_app.GetStockPen(SEP_HANDLER));
		HBRUSH oldBsh = (HBRUSH)::SelectObject(hDC, my_app.GetStockBrush(SEB_SOLID_HANDLER));
		::Polygon(hDC, arrow, 3);
		::SelectObject(hDC, oldPen);
		::SelectObject(hDC, oldBsh);
	}

	HPEN oldPen = dc.SelectPen(my_app.GetStockPen(SEP_GUIDLINE));
	HBRUSH oldBsh = dc.SelectBrush(my_app.GetStockBrush(SEB_SOLID_HANDLER));

	::DrawPath(dc, pData, cb);	
	
	dc.SelectPen(oldPen);
	dc.SelectBrush(oldBsh);

	SafeArrayDestroy(psap);
}

void CEffectInplaceEditor::GetHandler(IGLD_Rect *pIRect, IGLD_Matrix *pIMx, POINT *arrHandler)
{
	double pp[8] = {0};
	pIRect->GetData(&pp[0], &pp[1], &pp[4], &pp[5]);
	pp[2] = pp[0];
	pp[3] = pp[5];
	pp[6] = pp[4];
	pp[7] = pp[1];

	pIMx->Transform(&pp[0], &pp[1]);
	pIMx->Transform(&pp[2], &pp[3]);
	pIMx->Transform(&pp[4], &pp[5]);
	pIMx->Transform(&pp[6], &pp[7]);
	
	arrHandler[0].x = (int)pp[0];
	arrHandler[0].y = (int)pp[1];
	arrHandler[1].x = (int)((pp[0] + pp[2]) / 2);
	arrHandler[1].y = (int)((pp[1] + pp[3]) / 2);
	arrHandler[2].x = (int)pp[2];
	arrHandler[2].y = (int)pp[3];
	arrHandler[3].x = (int)((pp[2] + pp[4]) / 2);
	arrHandler[3].y = (int)((pp[3] + pp[5]) / 2);
	arrHandler[4].x = (int)pp[4];
	arrHandler[4].y = (int)pp[5];
	arrHandler[5].x = (int)((pp[4] + pp[6]) / 2);
	arrHandler[5].y = (int)((pp[5] + pp[7]) / 2);
	arrHandler[6].x = (int)pp[6];
	arrHandler[6].y = (int)pp[7];
	arrHandler[7].x = (int)((pp[6] + pp[0]) / 2);
	arrHandler[7].y = (int)((pp[7] + pp[1]) / 2);

	double dx = arrHandler[6].x - arrHandler[0].x;
	double dy = arrHandler[6].y - arrHandler[0].y;
	double r = sqrt(dx * dx + dy * dy);
	arrHandler[8].x = (int)(arrHandler[7].x + ROTATE_HANDLE_LENGTH * dy / r);
	arrHandler[8].y = (int)(arrHandler[7].y - ROTATE_HANDLE_LENGTH * dx / r);
	double c1 = Cross(arrHandler[2].x - arrHandler[0].x, 
		arrHandler[2].y - arrHandler[0].y, dx, dy);
	double c2 = Cross(arrHandler[8].x - arrHandler[0].x, 
		arrHandler[8].y - arrHandler[0].y, dx, dy);		
	if (c1 * c2 > 0)
	{
		arrHandler[8].x = 2 * arrHandler[7].x - arrHandler[8].x;
		arrHandler[8].y = 2 * arrHandler[7].y - arrHandler[8].y;
	}
}

void CEffectInplaceEditor::DrawHandler(HDC hDC, IGLD_Rect *pIRect, IGLD_Matrix *pIMx)
{
	POINT ptHandler[9];

	GetHandler(pIRect, pIMx, ptHandler);

	TDraftDC dc(hDC);

	HBRUSH hOldBrush = dc.SelectBrush(my_app.GetStockBrush(SEB_HOLLOW_HANDLER));
	HPEN hOldPen = dc.SelectPen(my_app.GetStockPen(SEP_HANDLER));
	
	for (int i = 0; i < 7; i++)
	{
		::DrawHandler(dc, ptHandler[i].x, ptHandler[i].y);
	}

	dc.MoveTo(ptHandler[8].x, ptHandler[8].y, TDraftDC::_no_trans_tag());
	dc.LineTo(ptHandler[7].x, ptHandler[7].y, TDraftDC::_no_trans_tag());

	::DrawHandler(dc, ptHandler[7].x, ptHandler[7].y);
	
	dc.SelectBrush(my_app.GetStockBrush(SEB_SOLID_HANDLER));
	::DrawHandler(dc, ptHandler[8].x, ptHandler[8].y);

	dc.SelectBrush(hOldBrush);
	dc.SelectPen(hOldPen);
}

void CEffectInplaceEditor::DrawEffect(HDC hDC)
{
	gldEffect *pEffect = _GetCurEffect();
	gldInstance *pInst = _GetCurInstance();

	if (pEffect == NULL || pInst == NULL) return;
	
	// draw path
	CComPtr<IGLD_CurvedPath> pIPath;
	GetEffectPath(pEffect, &pIPath);
	if (pIPath == NULL) return;

	ULONG count = 0;
	pIPath->get_Count(&count);
	if (count == 0) return;

	CComPtr<IGLD_Matrix> pIMx;
	Path2Screen(_GetCurInstance(), pEffect, &pIMx);
	if (pIMx == NULL) return;			
	
	DrawPath(hDC, pIPath, pIMx);
	
	// draw transform handler
	if (PathIsLine(pIPath))
	{
		ASSERT(count == 3);
		double ex = 0, ey = 0;
		pIPath->GetAt(count, &ex, &ey);
		pIMx->Transform(&ex, &ey);
		TDraftDC dc(hDC);
		HBRUSH hOldBrush = dc.SelectBrush((HBRUSH)GetStockObject(WHITE_BRUSH));
		HPEN hOldPen = dc.SelectPen((HPEN)GetStockObject(BLACK_PEN));
		::DrawHandler(dc, (int)ex, (int)ey);
		dc.SelectBrush(hOldBrush);
		dc.SelectPen(hOldPen);
	}
	else
	{
		CComPtr<IGLD_Rect> pIBnd;
		if (SUCCEEDED(pIPath->get_Bound(&pIBnd)))
		{
			ASSERT(pIBnd != NULL);
			DrawHandler(hDC, pIBnd, pIMx);
		}
	}		
}

int CEffectInplaceEditor::HitTest(const CPoint &point)
{
	int nHit = H_NO;

	gldEffect *pEffect = _GetCurEffect();
	gldInstance *pInst = _GetCurInstance();

	if (pEffect == NULL || pInst == NULL) return H_NO;
	
	CComPtr<IGLD_CurvedPath> pIPath;
	GetEffectPath(pEffect, &pIPath);
	if (pIPath == NULL) return H_NO;

	ULONG count = 0;
	pIPath->get_Count(&count);
	if (count == 0) return H_NO;

	CComPtr<IGLD_Matrix> pIMx;
	Path2Screen(_GetCurInstance(), pEffect, &pIMx);
	if (pIMx == NULL) return H_NO;	
	
	CPoint ptTest(point.x * 20, point.y * 20);
	if (PathIsLine(pIPath))
	{
		ASSERT(count == 3);
		double ex = 0, ey = 0;
		pIPath->GetAt(count, &ex, &ey);
		pIMx->Transform(&ex, &ey);
		if (PtOnHandler(ptTest, CPoint((int)ex, (int)ey)))
		{
			nHit = H_ED;
		}
	}
	else
	{
		CComPtr<IGLD_Rect> pIBnd;
		if (SUCCEEDED(pIPath->get_Bound(&pIBnd)))
		{
			ASSERT(pIBnd != NULL);
			POINT arrHandler[9];
			GetHandler(pIBnd, pIMx, arrHandler);
			for (int i = 0; i < 9; i++)
			{
				if (PtOnHandler(ptTest, arrHandler[i]))							
				{
					nHit = i + 1;
					break;
				}
			}
		}
	}

	return nHit;
}

void CEffectInplaceEditor::Track(const CPoint &point)
{
	int nHit = H_NO;

	gldEffect *pEffect = _GetCurEffect();
	gldInstance *pInst = _GetCurInstance();

	if (pEffect == NULL || pInst == NULL) return;
	
	CComPtr<IGLD_CurvedPath> pIPath;
	GetEffectPath(pEffect, &pIPath);
	if (pIPath == NULL) return;

	ULONG count = 0;
	pIPath->get_Count(&count);
	if (count == 0) return;

	CComPtr<IGLD_Matrix> pIMx;
	Path2Screen(_GetCurInstance(), pEffect, &pIMx);
	if (pIMx == NULL) return;	
	
	CPoint ptTest(point.x * 20, point.y * 20);
	if (PathIsLine(pIPath))
	{
		ASSERT(count == 3);
		double ex = 0, ey = 0;
		pIPath->GetAt(count, &ex, &ey);
		pIMx->Transform(&ex, &ey);
		if (PtOnHandler(ptTest, CPoint((int)ex, (int)ey)))
		{
			nHit = H_ED;
		}
	}
	else
	{
		CComPtr<IGLD_Rect> pIBnd;
		if (SUCCEEDED(pIPath->get_Bound(&pIBnd)))
		{
			ASSERT(pIBnd != NULL);
			POINT arrHandler[9];
			GetHandler(pIBnd, pIMx, arrHandler);
			for (int i = 0; i < 9; i++)
			{
				if (PtOnHandler(ptTest, arrHandler[i]))							
				{
					nHit = i + 1;
					break;
				}
			}
		}
	}

	if (nHit != H_NO)
	{
		m_PathTool.Hwnd(my_app.GetDesignWnd());
		m_PathTool.Start(pInst, pEffect, pIPath, nHit, point);
	}
}

bool CEffectInplaceEditor::PtOnPath(const CPoint &point)
{
	gldInstance *pInst = _GetCurInstance();
	if (pInst == NULL) return FALSE;
	gldEffect *pEffect = _GetCurEffect();
	if (pEffect == NULL) return FALSE;
	if (pEffect->m_pIParas == NULL) return FALSE;
	CComPtr<IGLD_CurvedPath> pIPath;
	HRESULT hr = GetParameter(pEffect->m_pIParas, "Path", &pIPath);
	if (FAILED(hr)) return FALSE;
	ULONG nPoint = 0;
	hr = pIPath->get_Count(&nPoint);
	if (FAILED(hr)) return FALSE;
	if (nPoint == 0) return FALSE;
	SAFEARRAY *psa = NULL;
	hr = pIPath->get_Data(&psa);
	if (FAILED(hr)) return FALSE;
	CComPtr<IGLD_Matrix> pIMx;
	Path2Screen(pInst, pEffect, &pIMx);
	TMatrix mx;
	if (pIMx != NULL)
	{
		double e11 = 1, e12 = 0, e21 = 0, e22 = 1, dx = 0, dy = 0;
		pIMx->GetData(&e11, &e12, &e21, &e22, &dx, &dy);
		mx.SetMatrix((float)e11, (float)e12, (float)e21, (float)e22, (long)dx, (long)dy);
	}
	tPoint query = {point.x, point.y};	
	double *pData = (double *)psa->pvData;
	bool bOn = false;
	double maxErr = 8000.0 / my_app.GetZoomPercent();
	for (; pData < (double *)psa->pvData + 2 * nPoint; pData += 4)
	{
		tQBezier bez;
		bez.P0.x = (int)pData[0];
		bez.P0.y = (int)pData[1];
		bez.P1.x = (int)pData[2];
		bez.P1.y = (int)pData[3];
		bez.P2.x = (int)pData[4];
		bez.P2.y = (int)pData[5];
		mx.Transform(bez.P0.x, bez.P0.y);
		mx.Transform(bez.P1.x, bez.P1.y);
		mx.Transform(bez.P2.x, bez.P2.y);
		double u;
		double dt = tPointDistance(&query, &bez, &u);
		if (fabs(dt) < maxErr)
		{
			bOn = true;
			break;
		}
	}
	SafeArrayDestroy(psa);
	return bOn;
}

BOOL CEffectInplaceEditor::SetCursor(const CPoint &point)
{
	gldEffect *pEffect = _GetCurEffect();
	gldInstance *pInst = _GetCurInstance();

	if (pEffect == NULL || pInst == NULL) return FALSE;
	
	CComPtr<IGLD_CurvedPath> pIPath;
	GetEffectPath(pEffect, &pIPath);
	if (pIPath == NULL) return FALSE;

	ULONG count = 0;
	pIPath->get_Count(&count);
	if (count == 0) return FALSE;

	CComPtr<IGLD_Matrix> pIMx;
	Path2Screen(_GetCurInstance(), pEffect, &pIMx);
	if (pIMx == NULL) return FALSE;	
	
	CPoint ptTest(point.x * 20, point.y * 20);
	HCURSOR hCursor = NULL;
	if (PathIsLine(pIPath))
	{
		ASSERT(count == 3);
		double ex = 0, ey = 0;
		pIPath->GetAt(count, &ex, &ey);
		pIMx->Transform(&ex, &ey);
		if (PtOnHandler(ptTest, CPoint((int)ex, (int)ey)))		
			hCursor = my_app.GetStockCursor(IDC_G_RESHAPE_DRAG);		
	}
	else
	{
		CComPtr<IGLD_Rect> pIBnd;
		if (SUCCEEDED(pIPath->get_Bound(&pIBnd)))
		{
			ASSERT(pIBnd != NULL);
			POINT arrHandler[9];
			GetHandler(pIBnd, pIMx, arrHandler);
			int i = 0;
			for (; i < 9; i++)
			{
				if (PtOnHandler(ptTest, arrHandler[i]))
					break;			
			}
			if (i < 9)
			{
				if (i + 1 == H_RO)
				{
					hCursor = my_app.GetStockCursor(IDC_G_ROTATE);
				}
				else
				{
					int ip = (7 + i) % 8;
					int in = (9 + i) % 8;
					int iCursor = TSECursor::GetStretchCursor(
						arrHandler[i].x, arrHandler[i].y,
						arrHandler[ip].x, arrHandler[ip].y,
						arrHandler[in].x, arrHandler[in].y);
					hCursor = my_app.GetStockCursor(iCursor);
				}
			}
		}
	}

	return (hCursor != NULL) ? (::SetCursor(hCursor) != NULL) : FALSE;	
}

//******************************************************************************
//
// Path commands
//
//******************************************************************************
CCmdTransformPath::CCmdTransformPath(gldEffect *pEffect, TMatrix &newMatrix)
: m_pEffect(pEffect)
, m_Matrix(newMatrix)
{
	_M_Desc_ID = IDS_CMD_TRANSFORMPATH;
}

bool CCmdTransformPath::Execute()
{
	ASSERT(m_pEffect != NULL);

	TMatrix mx = GetPathMatrix(m_pEffect);
	CComPtr<IGLD_Matrix> pIMx;
	pIMx.CoCreateInstance(__uuidof(GLD_Matrix));
	if (pIMx != NULL)
	{
		pIMx->SetData(m_Matrix.m_e11, m_Matrix.m_e12, 
			m_Matrix.m_e21, m_Matrix.m_e22,
			m_Matrix.m_dx, m_Matrix.m_dy);
		PutParameter(m_pEffect->m_pIParas, "Path.Matrix", pIMx);
		m_Matrix = mx;
		my_app.Repaint();
		return TCommand::Execute();
	}
	
	return false;	
}

bool CCmdTransformPath::Unexecute()
{
	ASSERT(m_pEffect != NULL);

	TMatrix mx = GetPathMatrix(m_pEffect);
	CComPtr<IGLD_Matrix> pIMx;
	pIMx.CoCreateInstance(__uuidof(GLD_Matrix));
	if (pIMx != NULL)
	{
		pIMx->SetData(m_Matrix.m_e11, m_Matrix.m_e12, 
			m_Matrix.m_e21, m_Matrix.m_e22,
			m_Matrix.m_dx, m_Matrix.m_dy);
		PutParameter(m_pEffect->m_pIParas, "Path.Matrix", pIMx);
		m_Matrix = mx;
		my_app.Repaint();
	}
	
	return TCommand::Unexecute();
}


CCmdRelinePath::CCmdRelinePath(IGLD_CurvedPath *pIPath, double offsetX, double offsetY)
: m_pIPath(pIPath)
, m_OffsetX(offsetX)
, m_OffsetY(offsetY)
{
	_M_Desc_ID = IDS_CMD_RELINEPATH;
}

bool CCmdRelinePath::Execute()
{
	ASSERT(m_pIPath != NULL);
#ifdef _DEBUG
	ULONG cb = 0;
	m_pIPath->get_Count(&cb);
	ASSERT(cb == 3);
#endif
	SAFEARRAY *pArrData = NULL;
	m_pIPath->get_Data(&pArrData);
	if (pArrData != NULL)
	{
		DOUBLE *pData = (DOUBLE *)pArrData->pvData;		
		pData[4] += m_OffsetX;
		pData[5] += m_OffsetY;
		pData[2] = (pData[0] + pData[4]) / 2;
		pData[3] = (pData[1] + pData[5]) / 2;
		HRESULT hr = m_pIPath->put_Data(pArrData);
		SafeArrayDestroy(pArrData);
		if (SUCCEEDED(hr))
		{
			my_app.Repaint();
			return TCommand::Execute();
		}
	}

	return false;
}

bool CCmdRelinePath::Unexecute()
{
	ASSERT(m_pIPath != NULL);
#ifdef _DEBUG
	ULONG cb = 0;
	m_pIPath->get_Count(&cb);
	ASSERT(cb == 3);
#endif
	SAFEARRAY *pArrData = NULL;
	m_pIPath->get_Data(&pArrData);
	if (pArrData != NULL)
	{
		DOUBLE *pData = (DOUBLE *)pArrData->pvData;
		pData[4] -= m_OffsetX;
		pData[5] -= m_OffsetY;
		pData[2] = (pData[0] + pData[4]) / 2;
		pData[3] = (pData[1] + pData[5]) / 2;
		m_pIPath->put_Data(pArrData);
		SafeArrayDestroy(pArrData);
		my_app.Repaint();
	}

	return TCommand::Unexecute();
}
