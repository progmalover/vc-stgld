// DlgNewFolder.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgNewFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewFolder dialog


CDlgNewFolder::CDlgNewFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewFolder)
	m_strCurrentFolder = _T("");
	m_strNewFolder = _T("");
	//}}AFX_DATA_INIT
}


void CDlgNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewFolder)
	DDX_Text(pDX, IDC_EDIT_CURENT_FOLDER, m_strCurrentFolder);
	DDV_MaxChars(pDX, m_strCurrentFolder, 255);
	DDX_Text(pDX, IDC_EDIT_NEW_FOLDER, m_strNewFolder);
	DDV_MaxChars(pDX, m_strNewFolder, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewFolder, CDialog)
	//{{AFX_MSG_MAP(CDlgNewFolder)
	ON_EN_CHANGE(IDC_EDIT_NEW_FOLDER, OnChangeEditNewFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewFolder message handlers

void CDlgNewFolder::OnOK() 
{
	// TODO: Add extra validation here
	
	if (!UpdateData(TRUE))
		return;

	m_strNewFolder.TrimLeft();
	m_strNewFolder.TrimRight();

	// Check if there're invalid characters
	static const TCHAR szTok[] = "\"*?/:><|";
	int ret = _tcscspn(m_strNewFolder, szTok);
	if (ret < m_strNewFolder.GetLength())
	{
		AfxMessageBox(IDS_INVALID_FOLDER_NAME);
		return;
	}

	EndDialog(IDOK);
}

void CDlgNewFolder::OnChangeEditNewFolder() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	CString strFolder;
	GetDlgItemText(IDC_EDIT_NEW_FOLDER, strFolder);
	strFolder.TrimLeft();
	strFolder.TrimRight();

	GetDlgItem(IDOK)->EnableWindow(!strFolder.IsEmpty());
}

BOOL CDlgNewFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
