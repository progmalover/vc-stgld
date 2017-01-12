// DlgExport.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgExport.h"
#include "dlgs.h"
#include ".\dlgexport.h"


// CDlgExport

IMPLEMENT_DYNAMIC(CDlgExport, CFileDialogEx)
CDlgExport::CDlgExport(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialogEx(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

CDlgExport::CDlgExport(LPCTSTR lpszSection, BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR lpszFileName, CWnd *pParentWnd) : 
	CFileDialogEx(lpszSection, bOpenFileDialog, lpszDefExt, lpszFilter, lpszFileName, pParentWnd)
{
	m_pOFN->Flags |= OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
	m_pOFN->lpTemplateName = MAKEINTRESOURCE(IDD_EXPORT);

	m_bCompress = TRUE;
}

CDlgExport::~CDlgExport()
{
}


BEGIN_MESSAGE_MAP(CDlgExport, CFileDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_COMPRESS, OnCheckCompress)
END_MESSAGE_MAP()



// CDlgExport message handlers


void CDlgExport::OnSize(UINT nType, int cx, int cy)
{
	CFileDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RecalcLayout();
}

void CDlgExport::RecalcLayout()
{
	if (::IsWindow(m_hWnd))
	{
		CWnd *pFileType = GetParent()->GetDlgItem(cmb1);
		if (pFileType)
		{
			CRect rc1;
			pFileType->GetWindowRect(&rc1);
			ScreenToClient(&rc1);

			CWnd *pCheckCompress = GetDlgItem(IDC_CHECK_COMPRESS);
			CRect rc2;
			pCheckCompress->GetWindowRect(&rc2);
			ScreenToClient(&rc2);

			pCheckCompress->SetWindowPos(0, rc1.left, rc2.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
	}
}

void CDlgExport::OnCheckCompress()
{
	m_bCompress = (IsDlgButtonChecked(IDC_CHECK_COMPRESS) == BST_CHECKED);
}

BOOL CDlgExport::OnInitDialog()
{
	CFileDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	m_bCompress = AfxGetApp()->GetProfileInt("Export", "Compress", TRUE);
	if (m_bCompress)
		CheckDlgButton(IDC_CHECK_COMPRESS, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgExport::OnFileNameOK()
{
	// TODO: Add your specialized code here and/or call the base class

	AfxGetApp()->WriteProfileInt("Export", "Compress", m_bCompress);

	return CFileDialogEx::OnFileNameOK();
}
