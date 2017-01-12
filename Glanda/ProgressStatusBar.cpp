// ProgressStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressStatusBar.h"
#include ".\progressstatusbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_PROGRESS	1

/////////////////////////////////////////////////////////////////////////////
// CProgressStatusBar

IMPLEMENT_SINGLETON(CProgressStatusBar)
CProgressStatusBar::CProgressStatusBar()
{
}

CProgressStatusBar::~CProgressStatusBar()
{
}


BEGIN_MESSAGE_MAP(CProgressStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CProgressStatusBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProgressStatusBar message handlers

int CProgressStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_wndProgress.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_PROGRESS);

	return 0;
}

void CProgressStatusBar::BeginProgress()
{
	m_wndProgress.SetRange(0, 100);
	m_wndProgress.SetStep(1);
	m_wndProgress.SetPos(0);

	CRect rc;
	GetItemRect(0, &rc);

	m_wndProgress.SetWindowPos(0, rc.left + 2, rc.top + 2, min(200, rc.Width() - 4), rc.Height() - 4, SWP_NOZORDER | SWP_SHOWWINDOW);
}

void CProgressStatusBar::EndProgress()
{
	m_wndProgress.ShowWindow(SW_HIDE);
	GetParentFrame()->SetMessageText(AFX_IDS_IDLEMESSAGE);
}

void CProgressStatusBar::StepIt()
{
	int nLower, nUpper;
	m_wndProgress.GetRange(nLower, nUpper);
	if (m_wndProgress.GetPos() < nUpper)
		m_wndProgress.StepIt();
}

void CProgressStatusBar::SetPos(int nPos)
{
	m_wndProgress.SetPos(max(0, min(100, nPos)));
}
