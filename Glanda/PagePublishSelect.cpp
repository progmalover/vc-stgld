// PagePublishSelect.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishSelect.h"
#include ".\pagepublishselect.h"


// CPagePublishSelect dialog

IMPLEMENT_DYNAMIC(CPagePublishSelect, CPropertyPageFixed)
CPagePublishSelect::CPagePublishSelect()
	: CPropertyPageFixed(CPagePublishSelect::IDD)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_SELECT, 0);
}

CPagePublishSelect::~CPagePublishSelect()
{
}

void CPagePublishSelect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPagePublishSelect, CPropertyPageFixed)
END_MESSAGE_MAP()


// CPagePublishSelect message handlers

BOOL CPagePublishSelect::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPageFixed::OnSetActive();
}

LRESULT CPagePublishSelect::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	if (IsDlgButtonChecked(IDC_RADIO_EXPORT_MOVIE))
		m_nChoice = PUBLISH_EXPORT_MOVIE;
	else if(IsDlgButtonChecked(IDC_RADIO_INSERT_CODE))
		m_nChoice = PUBLISH_INSERT_CODE;
	else
		m_nChoice = PUBLISH_VIEW_CODE;

	AfxGetApp()->WriteProfileInt("Publish", "Choice", m_nChoice);

	switch (m_nChoice)
	{
		case PUBLISH_EXPORT_MOVIE:
			return IDD_PUBLISH_EXPORT;

		case PUBLISH_INSERT_CODE:
		case PUBLISH_VIEW_CODE:
			return IDD_PUBLISH_HTML_OPTIONS;
	}
	return -1;
}

BOOL CPagePublishSelect::OnInitDialog()
{
	CPropertyPageFixed::OnInitDialog();

	// TODO:  Add extra initialization here

	m_nChoice = AfxGetApp()->GetProfileInt("Publish", "Choice", 0);

	switch (m_nChoice)
	{
		case PUBLISH_EXPORT_MOVIE:
			CheckDlgButton(IDC_RADIO_EXPORT_MOVIE, BST_CHECKED);
			break;

		case PUBLISH_INSERT_CODE:
			CheckDlgButton(IDC_RADIO_INSERT_CODE, BST_CHECKED);
			break;

		case PUBLISH_VIEW_CODE:
			CheckDlgButton(IDC_RADIO_VIEW_CODE, BST_CHECKED);
			break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
