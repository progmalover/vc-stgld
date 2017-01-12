// ImportImageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ImportImageDialog.h"
#include ".\importimagedialog.h"


// CImportImageDialog dialog

IMPLEMENT_DYNAMIC(CImportImageDialog, CDialog)
CImportImageDialog::CImportImageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImportImageDialog::IDD, pParent)
{
	m_bShow = AfxGetApp()->GetProfileInt("Tips\\Import Image", "Show", TRUE);
	m_bBackground = AfxGetApp()->GetProfileInt("Tips\\Import Image", "Import As Background", FALSE);
	m_bTiled = AfxGetApp()->GetProfileInt("Tips\\Import Image", "Tiled", FALSE);
}

CImportImageDialog::~CImportImageDialog()
{
}

void CImportImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImportImageDialog, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_NORMAL, OnBnClickedRadioNormal)
	ON_BN_CLICKED(IDC_RADIO_BACKGROUND, OnBnClickedRadioBackground)
END_MESSAGE_MAP()


// CImportImageDialog message handlers

BOOL CImportImageDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateControls(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImportImageDialog::OnDestroy()
{
	UpdateControls(TRUE);

	AfxGetApp()->WriteProfileInt("Tips\\Import Image", "Show", m_bShow);
	AfxGetApp()->WriteProfileInt("Tips\\Import Image", "Import As Background", m_bBackground);
	AfxGetApp()->WriteProfileInt("Tips\\Import Image", "Tiled", m_bTiled);

	CDialog::OnDestroy();
}

void CImportImageDialog::OnBnClickedRadioNormal()
{
	UpdateControls(TRUE);
}

void CImportImageDialog::OnBnClickedRadioBackground()
{
	UpdateControls(TRUE);
}

void CImportImageDialog::UpdateControls(BOOL bSave)
{
	if (bSave)
	{
		m_bShow = (IsDlgButtonChecked(IDC_CHECK_DONNOT_SHOW_AGAIN) != BST_CHECKED);
		m_bBackground = (IsDlgButtonChecked(IDC_RADIO_BACKGROUND) == BST_CHECKED);
		m_bTiled = (IsDlgButtonChecked(IDC_CHECK_TILED) == BST_CHECKED);
	}
	else
	{
		CheckDlgButton(IDC_RADIO_NORMAL, m_bBackground ? BST_UNCHECKED : BST_CHECKED);
		CheckDlgButton(IDC_RADIO_BACKGROUND, m_bBackground ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK_TILED, m_bTiled? BST_CHECKED : BST_UNCHECKED);
	}
	GetDlgItem(IDC_CHECK_TILED)->EnableWindow(m_bBackground);
}

void CImportImageDialog::OnOK()
{
	UpdateControls(TRUE);
	CDialog::OnOK();
}
