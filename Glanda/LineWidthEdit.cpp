// LineWidthEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "LineWidthEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern UINT WM_SLIDERPOSINIT;
extern UINT WM_SLIDERPOSCHANGED;

// CLineWidthEdit

IMPLEMENT_DYNAMIC(CLineWidthEdit, CNumberEdit)
CLineWidthEdit::CLineWidthEdit()
{
}

CLineWidthEdit::~CLineWidthEdit()
{
}


BEGIN_MESSAGE_MAP(CLineWidthEdit, CNumberEdit)
	ON_REGISTERED_MESSAGE(WM_SLIDERPOSCHANGED, OnSliderPosChanged)
	ON_REGISTERED_MESSAGE(WM_SLIDERPOSINIT, OnSliderPosInit)
END_MESSAGE_MAP()



// CLineWidthEdit message handlers

LRESULT CLineWidthEdit::OnSliderPosChanged(WPARAM wp, LPARAM lp)
{
	int nPos = (int)wp;

	CString strNew;
	strNew.Format("%.2f", (float)nPos * 0.25);
	RemoveFloatTrailZero(strNew);

	CString strOld;
	GetWindowText(strOld);
	if (strNew.Compare(strOld) != 0)
	{
		SetWindowText(strNew);
		UpdateWindow();
		m_bChanged = TRUE;		// OnSetText() will set this to false
	}

	return 1;
}

LRESULT CLineWidthEdit::OnSliderPosInit(WPARAM wp, LPARAM lp)
{
	int *pPos = (int *)lp;

	BOOL bTrans;
	float fValue = (float)GetDlgItemFloat(::GetParent(m_hWnd), GetDlgCtrlID(), &bTrans);
	*pPos = (int)(bTrans ? max(0, fValue / 0.25) : 0);

	return 1;
}
