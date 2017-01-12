// DlgWebBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgWebBrowser.h"
#include "Browsers.h"
#include "DlgAddBrowser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWebBrowser dialog


CDlgWebBrowser::CDlgWebBrowser(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWebBrowser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWebBrowser)
	//}}AFX_DATA_INIT
}


void CDlgWebBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWebBrowser)
	DDX_Control(pDX, IDC_LIST_WELLKNOWN, m_lstWellknown);
	DDX_Control(pDX, IDC_LIST_CUSTOMIZED, m_lstCustomized);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWebBrowser, CDialog)
	//{{AFX_MSG_MAP(CDlgWebBrowser)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnAdd)
	ON_LBN_SELCHANGE(IDC_LIST_CUSTOMIZED, OnSelChangeListCustomized)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWebBrowser message handlers

BOOL CDlgWebBrowser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int nSpeBrowser = CBrowsers::Instance()->GetSpecifyBrowserNum();
	int i = 0;
	for ( ; i < nSpeBrowser; i++){
		m_lstCustomized.AddString(CBrowsers::Instance()->GetSpecifyBrowserName(i));
	}

	int nInstalledBrowser = CBrowsers::Instance()->GetBrowserNum();
	for ( i = 0; i < nInstalledBrowser; i++){
		m_lstWellknown.AddString(CBrowsers::Instance()->GetBrowserName(i));
	}

	OnSelChangeListCustomized();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWebBrowser::OnDelete() 
{
	// TODO: Add your control notification handler code here

	int nSelected = m_lstCustomized.GetCurSel();
	ASSERT(nSelected >= 0);

	CString strName;
	m_lstCustomized.GetText(nSelected, strName);

	if (CBrowsers::Instance()->DelSpecifyBrowser(strName))
	{
		m_lstCustomized.DeleteString(nSelected);
		OnSelChangeListCustomized();
	}
}

void CDlgWebBrowser::OnAdd() 
{
	// TODO: Add your control notification handler code here

	CDlgAddBrowser dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (CBrowsers::Instance()->AddSpecifyBrowser(dlg.m_strFile, dlg.m_strName))
			m_lstCustomized.AddString(dlg.m_strName);
	}
}

void CDlgWebBrowser::OnSelChangeListCustomized() 
{
	// TODO: Add your control notification handler code here
	

	int nSelected = m_lstCustomized.GetCurSel();
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(nSelected >= 0);
}
