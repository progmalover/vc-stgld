// OptionsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OptionsSheet.h"
#include ".\optionssheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COptionsSheet

const UINT IDC_BUTTON_RESET			= 1000;

int COptionsSheet::m_nActivePage	= 0;

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheet)
COptionsSheet::COptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, m_nActivePage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage(&m_pageGeneral);
	AddPage(&m_pageTimeLine);
	AddPage(&m_pageDesign);
	AddPage(&m_pageAS);
	AddPage(&m_pagePreview);
}

COptionsSheet::~COptionsSheet()
{
}


BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheet)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnReset)
END_MESSAGE_MAP()


// COptionsSheet message handlers

BOOL COptionsSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// TODO:  Add your specialized code here

	//m_btnReset.Create("Reset", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_BUTTON_RESET);
	//m_btnReset.SetFont(GetFont());

	//RecalcLayout();

	return bResult;
}

void COptionsSheet::OnDestroy()
{
	CPropertySheet::OnDestroy();

	// TODO: Add your message handler code here

	m_nActivePage = GetActiveIndex();
}

void COptionsSheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	//if (::IsWindow(m_btnReset.m_hWnd))
	//	RecalcLayout();
}

void COptionsSheet::RecalcLayout()
{
	CRect rc;
	GetClientRect(&rc);

	CWnd *pOK = GetDlgItem(IDOK);
	CWnd *pHelp = GetDlgItem(IDHELP);
	ASSERT(pOK && pHelp);
	if (pOK && pHelp)
	{
		CRect rcOK, rcHelp;
		pOK->GetWindowRect(&rcOK);
		ScreenToClient(&rcOK);
		pHelp->GetWindowRect(&rcHelp);
		ScreenToClient(&rcHelp);
		
		CWnd *pPrev = pOK->GetNextWindow(GW_HWNDPREV);
		ASSERT(pPrev);
		m_btnReset.SetWindowPos(pPrev, rc.right - rcHelp.right, rcOK.top, rcOK.Width(), rcOK.Height(), 0);
	}
}

void COptionsSheet::OnReset()
{
	//CMenu menu;
	//if (menu.LoadMenu(IDR_RESET_OPTIONS))
	//{
	//	CMenu *pPopup = menu.GetSubMenu(0);
	//	if (pPopup)
	//	{
	//		CRect rc;
	//		m_btnReset.GetWindowRect(&rc);
	//		UINT nID = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, rc.left, rc.bottom, this);
	//		switch (nID)
	//		{
	//		case ID_RESET_CURRENT_PAGE:
	//			break;

	//		case ID_RESET_ALL_PAGES:
	//			break;

	//		default:
	//			break;
	//		}
	//	}
	//}
}
