// DlgChangeButtonText.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgChangeButtonText.h"
#include ".\dlgchangebuttontext.h"


// CDlgChangeButtonText dialog

IMPLEMENT_DYNAMIC(CDlgChangeButtonText, CDialog)
CDlgChangeButtonText::CDlgChangeButtonText(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangeButtonText::IDD, pParent)
	, m_Method(0)
{
}

CDlgChangeButtonText::~CDlgChangeButtonText()
{
}

void CDlgChangeButtonText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_TEXT, m_strText);	
}


BEGIN_MESSAGE_MAP(CDlgChangeButtonText, CDialog)
END_MESSAGE_MAP()


// CDlgChangeButtonText message handlers

BOOL CDlgChangeButtonText::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_Method == 0)
		CheckDlgButton(IDC_RADIO_KEEP_BOUND, BST_CHECKED);
	else
		CheckDlgButton(IDC_RADIO_KEEP_SIZE, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChangeButtonText::OnOK()
{
	m_Method = (((CButton *)GetDlgItem(IDC_RADIO_KEEP_BOUND))->GetCheck() == BST_CHECKED) ? 0 : 1;

	CDialog::OnOK();
}
