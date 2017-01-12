// StaticColorAlpha.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticColorAlpha.h"
#include "Dib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticColorAlpha

CStaticColorAlpha::CStaticColorAlpha()
{
	m_Alpha = 255;
}

CStaticColorAlpha::~CStaticColorAlpha()
{
}


BEGIN_MESSAGE_MAP(CStaticColorAlpha, CStaticColor)
	//{{AFX_MSG_MAP(CStaticColorAlpha)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticColorAlpha message handlers

void CStaticColorAlpha::SetAlpha(int alpha)
{
	ASSERT(alpha >= 0 && alpha <= 255);
	m_Alpha = alpha;
	if (::IsWindow(m_hWnd))
		Invalidate();
}

int CStaticColorAlpha::GetAlpha()
{
	return m_Alpha;
}

void CStaticColorAlpha::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rc;
	GetClientRect(&rc);

	CDib dib;
	dib.Create(rc.Width(), rc.Height());

	// background
	dib.Fill(255, 255, 255);

	// grid
	#define GRID_SIZE	6

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
	if (m_Alpha != 255)
		dib.FillGlass(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color), m_Alpha);
	else if (m_Color == CLR_INVALID)
		dib.FillGlass(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color), 0);
	else
		dib.Fill(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color));

	dib.Render(&dc, rc.Width(), rc.Height());

	// Do not call CStaticColor::OnPaint() for painting messages
}
