// SlidingPanelGroup.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "SlidingPanelGroup.h"
#include "SlidingPanel.h"
#include "DeferWindowPos.h"
#include <algorithm>
#include "Schemadef.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSlidingPanelGroup

IMPLEMENT_DYNAMIC(CSlidingPanelGroup, CWnd)
CSlidingPanelGroup::CSlidingPanelGroup()
: m_nActivePanel(-1)
, m_pImageList(NULL)
{
}

CSlidingPanelGroup::~CSlidingPanelGroup()
{
}


BEGIN_MESSAGE_MAP(CSlidingPanelGroup, CWnd)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CSlidingPanelGroup message handlers
void CSlidingPanelGroup::AddPanel(CSlidingPanel* pPanel)
{
	ASSERT(::IsWindow(m_hWnd));
	if(::IsWindow(m_hWnd))
	{
		m_Panels.push_back(pPanel);
		if(m_nActivePanel >= 0)
		{
			CDeferWindowPos dw;
			dw.BeginDeferWindowPos();
			for(int i = m_nActivePanel, j = m_Panels.size() -1; i < j; ++i)
			{
				CSlidingPanel* pPanel = GetPanel(i);
				CRect rc;
				GetPanelRect(rc, i);
				dw.DeferWindowPos(pPanel->m_hWnd, NULL, rc, SWP_NOZORDER);
			}
			dw.EndDeferWindowPos();
		}
		CRect rc;
		GetPanelRect(rc, m_Panels.size() -1);
		pPanel->Create(_afxWnd, "", WS_CHILD | WS_VISIBLE, rc, this, m_Panels.size());
	}
}

CSlidingPanel* CSlidingPanelGroup::GetPanel(int nPanel)
{
	return m_Panels[nPanel];
}

void CSlidingPanelGroup::SetActivePanel(int nActivePanel)
{
	ASSERT(::IsWindow(m_hWnd));

	if(m_nActivePanel!=nActivePanel)
	{
		int nOldIndex = m_nActivePanel>=0 ? m_nActivePanel : (m_Panels.size()-1);
		int nNewIndex = nActivePanel>=0 ? nActivePanel : (m_Panels.size()-1);
		int nCollapsePanel = m_nActivePanel;
		int nExpandPanel = nActivePanel;
		m_nActivePanel = nActivePanel;

		CDeferWindowPos dw;
		dw.BeginDeferWindowPos();

		if(nExpandPanel>=0)
		{
			// 先改变大小, 再展开
			CSlidingPanel* pPanel = GetPanel(nExpandPanel);
			CRect rc;
			GetPanelRect(rc, nExpandPanel);
			dw.DeferWindowPos(pPanel->m_hWnd, NULL, rc, SWP_NOZORDER);
		}

		if(nCollapsePanel>=0)
		{
			// 先收起, 再改变大小
			CSlidingPanel* pPanel = GetPanel(nCollapsePanel);
			pPanel->Collapse();
			CRect rc;
			GetPanelRect(rc, nCollapsePanel);
			dw.DeferWindowPos(pPanel->m_hWnd, NULL, rc, SWP_NOZORDER);
		}

		for(int i = min(nOldIndex, nNewIndex), j = max(nOldIndex, nNewIndex); i<=j; ++i)
		{
			CSlidingPanel* pPanel = GetPanel(i);

			CRect rc;
			GetPanelRect(rc, i);

			if(i!=nCollapsePanel && i!=nExpandPanel)
			{
				// 只作移动处理, 不改变大小
				dw.DeferWindowPos(pPanel->m_hWnd, NULL, rc, SWP_NOZORDER | SWP_NOSIZE);
			}
		}
		dw.EndDeferWindowPos();
		GetPanel(nExpandPanel)->Expand();
	}
}

void CSlidingPanelGroup::SetActivePanel(CSlidingPanel* pPanel)
{
	for(int i = 0, j = m_Panels.size(); i < j; ++i)
	{
		if(m_Panels[i] == pPanel)
		{
			SetActivePanel(i);
			break;
		}
	}
}

void CSlidingPanelGroup::OnDestroy()
{
	CWnd::OnDestroy();

	for(int i = 0, j = m_Panels.size(); i < j; ++i)
	{
		CSlidingPanel* pPanel = GetPanel(i);
		if(::IsWindow(pPanel->m_hWnd))
		{
			pPanel->DestroyWindow();
		}
	}
	m_nActivePanel = -1;
}

void CSlidingPanelGroup::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CSlidingPanelGroup::RecalcLayout()
{
	CDeferWindowPos dw;
	dw.BeginDeferWindowPos();
	for(int i = 0, j = m_Panels.size(); i < j; ++i)
	{
		CSlidingPanel* pPanel = GetPanel(i);
		CRect rc;
		GetPanelRect(rc, i);
		dw.DeferWindowPos(pPanel->m_hWnd, NULL, rc, SWP_NOZORDER);
	}
	dw.EndDeferWindowPos();
}

void CSlidingPanelGroup::GetPanelRect(CRect& rect, int nPanel)
{
	GetClientRect(&rect);
	CSlidingPanel* pPanel = GetPanel(nPanel);
	rect.DeflateRect(PANEL_SPACING, PANEL_SPACING);

	int j = m_Panels.size();
	if(nPanel<m_nActivePanel || m_nActivePanel < 0)
	{
		rect.top += (CSlidingPanel::GetTitleHeight() + PANEL_SPACING) * nPanel;
		rect.bottom = rect.top + CSlidingPanel::GetTitleHeight();
	}
	else if(nPanel>m_nActivePanel)
	{
		rect.bottom -= (CSlidingPanel::GetTitleHeight() + PANEL_SPACING) * (j - 1 - nPanel);
		rect.top = rect.bottom - CSlidingPanel::GetTitleHeight();
	}
	else
	{
		rect.top += (CSlidingPanel::GetTitleHeight() + PANEL_SPACING) * nPanel;
		rect.bottom -= (CSlidingPanel::GetTitleHeight() + PANEL_SPACING) * (j - 1 - nPanel);
	}
}

BOOL CSlidingPanelGroup::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(_afxWnd, "", dwStyle, rect, pParentWnd, nID);
}

BEGIN_TM_CLASS_PARTS(GLOBALS)
    TM_PART(1, GP, BORDER)
    TM_PART(2, GP, LINEHORZ)
    TM_PART(3, GP, LINEVERT)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(GP_BORDER)
    TM_STATE(1, BSS, FLAT)
    TM_STATE(2, BSS, RAISED)
    TM_STATE(3, BSS, SUNKEN)
END_TM_PART_STATES()

void CSlidingPanelGroup::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);

	COLORREF crBorder;
	if (g_xpStyle.IsAppThemed())
	{
		HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"TREEVIEW");
		if (hTheme)
		{
			HRESULT hr = g_xpStyle.GetThemeColor(hTheme, 0, 0, TMT_BORDERCOLOR, &crBorder);
			g_xpStyle.CloseThemeData(hTheme);
		}
	}
	else
	{
		crBorder = ::GetSysColor(COLOR_3DSHADOW);
	}
	
	CBrush br(crBorder);
	dc.FillRect(&rc, &br);
}

int CSlidingPanelGroup::GetMinHeight(int nSize)
{
	return nSize * (PANEL_SPACING + CSlidingPanel::GetTitleHeight()) + PANEL_SPACING;
}

void CSlidingPanelGroup::LoadActivePanel(LPCTSTR lpszSection)
{
	int nActivePanel = AfxGetApp()->GetProfileInt(lpszSection, "ActivePanel", 0);
	SetActivePanel(min(m_Panels.size()-1, max(0, nActivePanel)));
}

void CSlidingPanelGroup::SaveActivePanel(LPCTSTR lpszSection)
{
	AfxGetApp()->WriteProfileInt(lpszSection, "ActivePanel", GetActivePanel());
}

int CSlidingPanelGroup::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(0, WS_CLIPCHILDREN);

	return 0;
}

CImageList *CSlidingPanelGroup::SetImageList(CImageList *pImageList)
{
	CImageList *pOldImageList = m_pImageList;
	m_pImageList = pImageList;
	return pOldImageList;
}

CImageList *CSlidingPanelGroup::GetImageList()
{
	return m_pImageList;
}
