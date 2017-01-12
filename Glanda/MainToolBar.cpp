// MainToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MainToolBar.h"

#include "DlgUndoRedo.h"

#include "VisualStylesXP.h"
#include ".\maintoolbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CMainToolBar

CMainToolBar::CMainToolBar()
{
}

CMainToolBar::~CMainToolBar()
{
}


BEGIN_MESSAGE_MAP(CMainToolBar, CToolBarEx)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnDropDown)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMainToolBar message handlers

void CMainToolBar::OnDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR pNMTB = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);
	
	// TODO: Add your control notification handler code here

	BOOL bUndo = TRUE;

	switch (pNMTB->iItem)
	{
	case ID_EDIT_UNDO:
		bUndo = TRUE;
		break;	
	
	case ID_EDIT_REDO:
		bUndo = FALSE;
		break;	
	
	default:
		CToolBarEx::OnDropDown(pNMHDR, pResult);
		return;	
	}

	CDlgUndo *pDlg = CDlgUndo::Instance();
	ASSERT(::IsWindow(pDlg->m_hWnd));

	pDlg->LoadList(bUndo);

	CToolBarCtrl &tb = GetToolBarCtrl();
	int index = tb.CommandToIndex(pNMTB->iItem);
	CRect rc;
	tb.GetItemRect(index, &rc);
	tb.ClientToScreen(&rc);

	CRect rc2;
	pDlg->GetWindowRect(&rc2);

	rc2.OffsetRect(rc.left - rc2.left - 1, rc.bottom - rc2.top);
	EnsureWholeRectVisible(rc2);
	pDlg->SetWindowPos(&wndTopMost, rc2.left, rc2.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	pDlg->TrackMessage();

	*pResult = 0;
}

CSize CMainToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (g_xpStyle.IsAppThemed())
	{
		CSize size;
		GetToolBarCtrl().GetMaxSize(&size);
		size.cx = 32767;
		size.cy += 6;

		return size;
	}
	else
	{
		return CToolBarEx::CalcFixedLayout(bStretch, bHorz);
	}
}

BOOL CMainToolBar::CreateZoomComboBox()
{
	BOOL ret = m_cmbZoom.Create(CBS_DROPDOWN | WS_VSCROLL | WS_VISIBLE | WS_TABSTOP, CRect(0, 0, 0, 0), this, 1);
	if (ret)
	{
		m_cmbZoom.SetFont(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

		m_cmbZoom.AddString("10%");
		m_cmbZoom.AddString("25%");
		m_cmbZoom.AddString("50%");
		m_cmbZoom.AddString("75%");
		m_cmbZoom.AddString("100%");
		m_cmbZoom.AddString("150%");
		m_cmbZoom.AddString("200%");
		m_cmbZoom.AddString("300%");
		m_cmbZoom.AddString("400%");
		m_cmbZoom.AddString("500%");
		m_cmbZoom.AddString("1000%");

		return TRUE;
	}
	return FALSE;
}

void CMainToolBar::RecalcLayout()
{
	CToolBarEx::RecalcLayout();

	RecalcZoomComboBoxPos();
}

void CMainToolBar::RecalcZoomComboBoxPos()
{
	if (::IsWindow(m_hWnd) && ::IsWindow(m_cmbZoom.m_hWnd))
	{
		CRect rc;
		GetClientRect(&rc);

		CRect rcCombo;
		m_cmbZoom.GetWindowRect(&rcCombo);

		rc.right -= 10;
		rc.left = rc.right - 60;

		int index = GetToolBarCtrl().GetButtonCount() - 1;
		if (index >= 0)
		{
			CRect rcButton;
			GetItemRect(index, &rcButton);
			if (rc.left <= rcButton.right)
			{
				m_cmbZoom.ShowWindow(SW_HIDE);
				return;
			}
		}

		rc.top = (rc.Height() - rcCombo.Height()) / 2;
		rc.bottom = 200;

		m_cmbZoom.SetWindowPos(&wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}
}

void CMainToolBar::OnSize(UINT nType, int cx, int cy)
{
	CToolBarEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RecalcZoomComboBoxPos();
}
