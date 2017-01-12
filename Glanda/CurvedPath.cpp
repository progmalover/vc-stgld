#include "StdAfx.h"
#include ".\curvedpath.h"
#include "GGraphics.h"
#include "gld_draft_dc.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "my_app.h"

CCurvedPath::CCurvedPath(IGLD_CurvedPath *pIPath /*= NULL*/)
: m_Path(pIPath)
{
}

CCurvedPath::~CCurvedPath(void)
{
}

void CCurvedPath::Draw(HDC hDC, TMatrix &mx)
{
	ASSERT(m_Path != NULL);
	ASSERT(hDC != NULL);
	if (m_Path == NULL)
		return;

	SAFEARRAY *psap = NULL;	

	HRESULT hr = m_Path->get_Data(&psap);
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
	
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush brush(RGB(255, 255, 255));	
	TDraftDC dc(hDC);
	dc.Matrix(mx);
	HPEN oldPen = dc.SelectPen(pen);
	HBRUSH oldBsh = dc.SelectBrush(brush);

	DOUBLE *pvData = (DOUBLE *)psap->pvData;	
	dc.MoveTo((int)pvData[0], (int)pvData[1], TDraftDC::_trans_tag());
	pvData += 2;
	for (LONG i = 1; i < cb; i += 2)
	{
		dc.BezierTo((int)pvData[0], (int)pvData[1], (int)pvData[2], 
			(int)pvData[3], TDraftDC::_trans_tag());
		pvData += 4;
	}
	pvData -= 2; // end point
	int x = (int)pvData[0];
	int y = (int)pvData[1];
	mx.Transform(x, y);
	dc.Ellipse(x - 80, y - 80, x + 80, y + 80, TDraftDC::_no_trans_tag());

	dc.SelectPen(oldPen);
	dc.SelectBrush(oldBsh);

	SafeArrayDestroy(psap);
}
