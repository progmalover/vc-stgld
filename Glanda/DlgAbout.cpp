// DlgAbout.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgAbout.h"
#include ".\dlgabout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout dialog


CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAbout)
	//}}AFX_DATA_INIT
}


void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAbout)
	DDX_Control(pDX, IDC_STATIC_PRODUCT, m_stcProduct);
	DDX_Control(pDX, IDC_STATIC_WEBSITE, m_stcWebsite);
	DDX_Control(pDX, IDC_STATIC_SUPPORT, m_stcSupport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	//{{AFX_MSG_MAP(CDlgAbout)
	ON_BN_CLICKED(IDC_STATIC_SUPPORT, OnSupport)
	ON_BN_CLICKED(IDC_STATIC_WEBSITE, OnWebsite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout message handlers

BOOL CDlgAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_stcProduct.SetFontSize(12);
	m_stcProduct.SetFontBold(TRUE);

	HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MY_HAND);
	m_stcWebsite.SetHoverCursor(hCursor);
	m_stcSupport.SetHoverCursor(hCursor);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAbout::OnSupport() 
{
	// TODO: Add your control notification handler code here

	CString strSupport;
	strSupport.LoadString(IDS_SUPPORT_URL);
	ShellExecute(m_hWnd, "open", strSupport, NULL, NULL, SW_SHOWNORMAL);
}

void CDlgAbout::OnWebsite() 
{
	// TODO: Add your control notification handler code here
	
	CString strWebsite;
	strWebsite.LoadString(IDS_URL);
	ShellExecute(m_hWnd, "open", strWebsite, NULL, NULL, SW_SHOWNORMAL);
}
