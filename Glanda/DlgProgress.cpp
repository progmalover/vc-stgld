// DlgProgress.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_REGISTERED_MESSAGE(WM_DLGPROGRESS)

// WPARAM for WM_DLGPROGRESS
enum
{
	MSG_SET_CAPTION	= 0, 
	MSG_SET_PROMPT, 
	MSG_SET_PROGRESS, 
	MSG_END_DIALOG
};

struct PROGRESS_RANGE
{
	int nLower;
	int nUpper;
};


// CDlgProgress dialog

IMPLEMENT_DYNAMIC(CDlgProgress, CDialog)
CDlgProgress::CDlgProgress(CWnd* pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt)
	: CDialog(CDlgProgress::IDD, pParent), 
	m_hEvent(NULL), 
	m_pbCanceled(NULL), 
	m_strCaption(lpszCaption), 
	m_strPrompt(lpszPrompt)
{
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CDlgProgress::~CDlgProgress()
{
	if (m_hEvent)
		CloseHandle(m_hEvent);
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_strPrompt);
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	ON_REGISTERED_MESSAGE(WM_DLGPROGRESS, OnProgressMessage)
END_MESSAGE_MAP()


// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	::SetEvent(m_hEvent);

	CMenu *pMenu = GetSystemMenu(FALSE);
	if (pMenu)
		pMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);

	SetWindowText(m_strCaption);

	m_progress.SetRange(0, 100);
	m_progress.SetStep(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProgress::WaitForInitialize()
{
	ASSERT(m_hEvent != NULL);
	WaitForSingleObject(m_hEvent, INFINITE);
}

void CDlgProgress::SetCancelFlagPtr(bool *pbCancel)
{
	m_pbCanceled = pbCancel;
	if (::IsWindow(m_hWnd))
		GetDlgItem(IDCANCEL)->EnableWindow(pbCancel != NULL);
}

void CDlgProgress::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_pbCanceled == NULL)	// not initialized yet. this's a small bug, ignored.
	{
		MessageBeep(0);
		return;
	}
	*m_pbCanceled = TRUE;
}

void CDlgProgress::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CDlgProgress::End(UINT nIDResult)
{
	PostMessage(WM_DLGPROGRESS, MSG_END_DIALOG, (LPARAM)nIDResult);
}

void CDlgProgress::SetCaption(LPCTSTR lpszText)
{
	SendMessage(WM_DLGPROGRESS, MSG_SET_CAPTION, (LPARAM)lpszText);
}

void CDlgProgress::SetPrompt(LPCTSTR lpszPrompt)
{
	SendMessage(WM_DLGPROGRESS, MSG_SET_PROMPT, (LPARAM)lpszPrompt);
}

void CDlgProgress::SetProgress(int nProgress)
{
	SendMessage(WM_DLGPROGRESS, MSG_SET_PROGRESS, (LPARAM)nProgress);
}

LRESULT CDlgProgress::OnProgressMessage(WPARAM wp, LPARAM lp)
{
	switch (wp)
	{
		case MSG_SET_CAPTION:
			m_strCaption = (LPCTSTR)lp;
			SetWindowText(m_strCaption);
			break;

		case MSG_SET_PROMPT:
			m_strPrompt = (LPCTSTR)lp;
			UpdateData(FALSE);
			break;

		case MSG_SET_PROGRESS:
		{
			int nPos = (int)lp;
			nPos = min(100, nPos);
			if (nPos != m_progress.GetPos())
				m_progress.SetPos(nPos);
			break;
		}

		case MSG_END_DIALOG:
			EndDialog((UINT)lp);
			break;
	}
	return 0;
}

void CDlgProgress::Update(void *pData)
{
	SetProgress((int)pData);
}
