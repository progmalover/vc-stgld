// DlgToolOption.cpp : implementation file
//

#include "stdafx.h"
#include "ResizableToolTipDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CResizableToolTipDialog dialog

CResizableToolTipDialog::CResizableToolTipDialog(UINT nIDD, CWnd* pParent)
	: CResizableDialog(nIDD, pParent)
{
}

CResizableToolTipDialog::~CResizableToolTipDialog()
{
}

void CResizableToolTipDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResizableToolTipDialog, CResizableDialog)
	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipText)
END_MESSAGE_MAP()


// CResizableToolTipDialog message handlers

BOOL CResizableToolTipDialog::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	EnableToolTips(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CResizableToolTipDialog::AddToolTip(UINT nIDCtrl, UINT nIDString)
{
	CWnd *pWnd = GetDlgItem(nIDCtrl);
	if (pWnd)
		AddToolTip(pWnd, nIDString);
}

void CResizableToolTipDialog::AddToolTip(UINT nIDCtrl, LPCTSTR lpszString)
{
	CWnd *pWnd = GetDlgItem(nIDCtrl);
	if (pWnd)
		AddToolTip(pWnd, lpszString);
}

void CResizableToolTipDialog::AddToolTip(CWnd *pWnd, LPCTSTR lpszString)
{
	m_mapTips[pWnd->m_hWnd] = std::string(lpszString);
}

void CResizableToolTipDialog::AddToolTip(CWnd *pWnd, UINT nIDString)
{
	CString strTip;
	if (nIDString != 0)
		strTip.LoadString(nIDString);
	else
		pWnd->GetWindowText(strTip);

	if (strTip.GetLength() > 0)
		AddToolTip(pWnd, strTip);
}

BOOL CResizableToolTipDialog::GetToolTipText(HWND hWnd, CString &strTip)
{
	if (m_mapTips.find(hWnd) != m_mapTips.end())
	{
		strTip = m_mapTips[hWnd].c_str();
		return TRUE;
	}
	return FALSE;
}

BOOL CResizableToolTipDialog::OnToolTipText(UINT nID, NMHDR * pNMHDR, LRESULT * pResult)
{
	*pResult = 0;

	CString strTipText;
	if (pNMHDR->code == TTN_NEEDTEXTA)
	{
		TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
		if (pTTTA->uFlags & TTF_IDISHWND)
		{
			if (GetToolTipText((HWND)pNMHDR->idFrom, strTipText))
			{
				_tcsncpy(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
				return TRUE;
			}
		}
	}
	else if (pNMHDR->code == TTN_NEEDTEXTW)
	{
		TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
		if (pTTTW->uFlags & TTF_IDISHWND)
		{
			if (GetToolTipText((HWND)pNMHDR->idFrom, strTipText))
			{
				_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
				return TRUE;
			}
		}
	}

	return FALSE;
}
