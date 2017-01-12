// CategoryWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryWnd.h"
#include "DeferWindowPos.h"
#include "CategoryTabCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CCategoryWnd

IMPLEMENT_DYNAMIC(CCategoryWnd, CWnd)
CCategoryWnd::CCategoryWnd()
{
}

CCategoryWnd::~CCategoryWnd()
{
}


BEGIN_MESSAGE_MAP(CCategoryWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CCategoryWnd message handlers


int CCategoryWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	// 先创建,然后再添加页面
    m_CategoryTabCtrl.Create(WS_CHILD|WS_VISIBLE|TCS_MULTILINE|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 1001);
	m_CategoryTabCtrl.AddPage(IDS_CATEGORY_TAB_RESOURCE_CAPTION, &m_CategoryResourcePage);
	m_CategoryTabCtrl.AddPage(IDS_CATEGORY_TAB_IMPORT_CAPTION, &m_CategoryImportPage);
	m_CategoryTabCtrl.AddPage(IDS_CATEGORY_TAB_TOOLS_CAPTION, &m_CategoryToolsPage);

	return 0;
}

void CCategoryWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CCategoryWnd::RecalcLayout()
{
	if (::IsWindow(m_hWnd))
	{
		CRect rc;
		GetClientRect(&rc);

		rc.InflateRect(-2, -2, -2, -2);
		m_CategoryTabCtrl.MoveWindow(&rc);
	}
}

void CCategoryWnd::OnDestroy()
{
	CWnd::OnDestroy();

	SaveActivePage();
	m_CategoryTabCtrl.DestroyWindow();
}

void CCategoryWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);
	
	dc.FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));

}

void CCategoryWnd::LoadActivePage()
{
	m_CategoryTabCtrl.SetActivePage(min(2, max(0, AfxGetApp()->GetProfileInt("Windows\\Category", "ActiveTab", 0))));
}

void CCategoryWnd::SaveActivePage()
{
	AfxGetApp()->WriteProfileInt("Windows\\Category", "ActiveTab", m_CategoryTabCtrl.GetActivePage());
}