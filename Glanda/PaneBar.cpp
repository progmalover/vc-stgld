// BoringBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PaneBar.h"
#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MARGIN = 3;

/////////////////////////////////////////////////////////////////////////////
// CPaneBar

IMPLEMENT_SINGLETON(CPaneBar)

CPaneBar::CPaneBar()
	: m_strText("")
{
}

CPaneBar::~CPaneBar()
{
}


BEGIN_MESSAGE_MAP(CPaneBar, CControlBar)
	//{{AFX_MSG_MAP(CPaneBar)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPaneBar message handlers

BOOL CPaneBar::Create(CWnd *pParentWnd, LPCTSTR pszTitle, UINT nID, DWORD dwStyle)
{
	ASSERT_VALID(pParentWnd);   // must have a parent
	ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));
	
    // save the style -- AMENDED by Holger Thiele - Thankyou
	m_dwStyle = dwStyle & CBRS_ALL;
	dwStyle |= CCS_NOPARENTALIGN | CCS_NOMOVEY | CCS_NODIVIDER | CCS_NORESIZE;
	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	//return CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), NULL),
						//NULL,
						//dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						//CRect(0,0,0,0), 
						//pParentWnd, 
						//nID);

	CString wndclass = ::AfxRegisterWndClass(CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW),
		::GetSysColorBrush(COLOR_BTNFACE), 
		0);


	return  CWnd::Create(wndclass, pszTitle, dwStyle, CRect(0, 0, 0, 0), pParentWnd, nID);
}

CSize CPaneBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (m_strText.IsEmpty())
	{
		return CSize(32767, 2);
	}
	else
	{
		return CSize(32767, ::GetSystemMetrics(SM_CYCAPTION) + MARGIN * 2);
	}
}

void CPaneBar::OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler)
{

}

void CPaneBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CControlBar::OnPaint() for painting messages

	CGlandaMemDC dcMem(&dc);

	CRect rc;
	GetClientRect(&rc);

	dcMem.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DFACE)));

	if (!m_strText.IsEmpty())
	{
		rc.InflateRect(-MARGIN, -MARGIN);
		dcMem.FrameRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));

		rc.InflateRect(-1, -1);
		dcMem.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_INFOBK)));

		CFont *pFont  = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
		CFont *pFontOld = dcMem.SelectObject(pFont);
		int nBkModeOld = dcMem.SetBkMode(TRANSPARENT);
		int nTextColorOld = dcMem.SetTextColor(::GetSysColor(COLOR_INFOTEXT));

		dcMem.DrawText(m_strText, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

		dcMem.SetTextColor(nTextColorOld);
		dcMem.SetBkMode(nBkModeOld);
		dcMem.SelectObject(pFontOld);
	}
}

void CPaneBar::OnSize(UINT nType, int cx, int cy)
{
	CControlBar::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	
	Invalidate();
}
