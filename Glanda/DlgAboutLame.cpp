// DlgAboutLame.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgAboutLame.h"
#include "filepath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAboutLame dialog


CDlgAboutLame::CDlgAboutLame(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAboutLame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAboutLame)
	//}}AFX_DATA_INIT

	try
	{
		CString strFile = GetModuleFilePath();
		strFile += "\\LGPL.txt";
		CStdioFile file(strFile, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
		int nLen = file.GetLength();
		if (nLen > 0)
		{
			LPTSTR p = m_strLicense.GetBufferSetLength(nLen);
			file.Read(p, nLen);
			m_strLicense.ReleaseBuffer();
			file.Close();
		}
	}
	catch (CFileException *e)
	{
#ifdef _DEBUG
		e->ReportError();
#endif
		char szError[256];
		e->GetErrorMessage(szError, 255);
		e->Delete();
		m_strLicense.Format(IDS_FAILED_OPEN_LICENSE_FILE_s, szError);
	}
}


void CDlgAboutLame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAboutLame)
	DDX_Control(pDX, IDC_STATIC_URL, m_stcLink);
	DDX_Text(pDX, IDC_EDIT_LICENSE, m_strLicense);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAboutLame, CDialog)
	//{{AFX_MSG_MAP(CDlgAboutLame)
	ON_BN_CLICKED(IDC_STATIC_URL, OnUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAboutLame message handlers

void CDlgAboutLame::OnUrl() 
{
	// TODO: Add your control notification handler code here
	
	ShellExecute(m_hWnd, "open", "http://www.sulaco.org/mp3/", NULL, NULL, SW_SHOWNORMAL);
}

BOOL CDlgAboutLame::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_stcLink.SetHoverCursor(AfxGetApp()->LoadCursor(IDC_MY_HAND));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
