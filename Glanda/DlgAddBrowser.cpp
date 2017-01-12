// DlgAddBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgAddBrowser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddBrowser dialog


CDlgAddBrowser::CDlgAddBrowser(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddBrowser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddBrowser)
	m_strName = _T("");
	m_strFile = _T("");
	//}}AFX_DATA_INIT
}


void CDlgAddBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddBrowser)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 255);
	DDX_Text(pDX, IDC_EDIT_FILE, m_strFile);
	DDV_MaxChars(pDX, m_strFile, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddBrowser, CDialog)
	//{{AFX_MSG_MAP(CDlgAddBrowser)
	ON_BN_CLICKED(IDC_BUTTON_LOCATE, OnLocate)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeNameFile)
	ON_EN_CHANGE(IDC_EDIT_FILE, OnChangeNameFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddBrowser message handlers

void CDlgAddBrowser::OnLocate() 
{
	// TODO: Add your control notification handler code here
	
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST, "Executable Files (*.exe)|*.exe||");
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_EDIT_FILE, dlg.GetPathName());
}

void CDlgAddBrowser::OnOK() 
{
	// TODO: Add extra validation here

	if (UpdateData(TRUE))
		CDialog::OnOK();
}

void CDlgAddBrowser::OnChangeNameFile() 
{
	CString strName, strFile;
	GetDlgItemText(IDC_EDIT_NAME, strName);
	GetDlgItemText(IDC_EDIT_FILE, strFile);

	GetDlgItem(IDOK)->EnableWindow(!strName.IsEmpty() && !strFile.IsEmpty());
}

BOOL CDlgAddBrowser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	OnChangeNameFile();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
