// DlgSelectTextChanging.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgSelectTextChanging.h"
#include ".\dlgselecttextchanging.h"


// CDlgSelectTextChanging dialog

IMPLEMENT_DYNAMIC(CDlgSelectTextChanging, CDialog)
CDlgSelectTextChanging::CDlgSelectTextChanging(std::list<CString> *texts, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectTextChanging::IDD, pParent)
	, m_Texts(texts)
{
	ASSERT(m_Texts != NULL);
	ASSERT(m_Texts->size() > 1);
}

CDlgSelectTextChanging::~CDlgSelectTextChanging()
{
}

void CDlgSelectTextChanging::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_TEXTS, m_lstTexts);
}


BEGIN_MESSAGE_MAP(CDlgSelectTextChanging, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_TEXTS, OnLbnSelchangeListTexts)
END_MESSAGE_MAP()


// CDlgSelectTextChanging message handlers

BOOL CDlgSelectTextChanging::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::list<CString>::iterator itt = m_Texts->begin();
	for (; itt != m_Texts->end(); ++itt)
		m_lstTexts.AddString(*itt);

	m_lstTexts.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectTextChanging::OnOK()
{
	int sel = m_lstTexts.GetCurSel();
	if (sel < 0)
		return;
	std::list<CString>::iterator i = m_Texts->begin();
	for (; sel > 0; sel--)
		++i;
	m_strText = *i;
	CDialog::OnOK();
}

void CDlgSelectTextChanging::OnLbnSelchangeListTexts()
{
	GetDlgItem(IDOK)->EnableWindow(m_lstTexts.GetCurSel() > -1);
}
