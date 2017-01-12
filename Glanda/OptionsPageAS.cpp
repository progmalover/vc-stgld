// OptionsPageAS.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OptionsPageAS.h"
#include "Options.h"
#include "optionspageas.h"

#include "ASView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// COptionsPageAS dialog

IMPLEMENT_DYNAMIC(COptionsPageAS, CPropertyPage)
COptionsPageAS::COptionsPageAS()
	: CPropertyPage(COptionsPageAS::IDD)
{
}

COptionsPageAS::~COptionsPageAS()
{
}

void COptionsPageAS::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_AUTO_LIST_MEMBERS, COptions::Instance()->m_bAutoListMembers);
	DDX_Check(pDX, IDC_CHECK_AUTO_SHOW_PAREMETER_INFO, COptions::Instance()->m_bAutoShowParameterInfo);
	DDX_Check(pDX, IDC_CHECK_AUTO_INDENT, COptions::Instance()->m_bAutoIndent);
	DDX_Check(pDX, IDC_CHECK_SHOW_SELECTION_MARGIN, COptions::Instance()->m_bShowSelMargin);
	DDX_Check(pDX, IDC_CHECK_SHOW_LINE_NUMBERS, COptions::Instance()->m_bShowLineNumbers);
}


BEGIN_MESSAGE_MAP(COptionsPageAS, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_SHOW_SELECTION_MARGIN, OnBnClickedCheckShowSelectionMargin)
END_MESSAGE_MAP()


// COptionsPageAS message handlers

void COptionsPageAS::OnBnClickedCheckShowSelectionMargin()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}

void COptionsPageAS::UpdateControls()
{
	GetDlgItem(IDC_CHECK_SHOW_LINE_NUMBERS)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_SHOW_SELECTION_MARGIN) == BST_CHECKED);
}

BOOL COptionsPageAS::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL COptionsPageAS::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	//CPropAction2::Instance()->m_pViewNormalAction->m_bSelMargin = COptions::Instance()->m_bShowSelMargin;
	//CPropAction2::Instance()->m_pViewNormalAction->m_bShowLineNumber = COptions::Instance()->m_bShowLineNumbers;
	//CPropAction2::Instance()->m_pViewNormalAction->m_bAutoIndent = COptions::Instance()->m_bAutoIndent;
	//CPropAction2::Instance()->m_pViewNormalAction->Invalidate(TRUE);

	//CPropAction2::Instance()->m_pViewInitAction->m_bSelMargin = COptions::Instance()->m_bShowSelMargin;
	//CPropAction2::Instance()->m_pViewInitAction->m_bShowLineNumber = COptions::Instance()->m_bShowLineNumbers;
	//CPropAction2::Instance()->m_pViewInitAction->m_bAutoIndent = COptions::Instance()->m_bAutoIndent;
	//CPropAction2::Instance()->m_pViewInitAction->Invalidate(TRUE);

	return CPropertyPage::OnApply();
}
