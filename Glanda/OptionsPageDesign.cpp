// OptionsPageDesign.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OptionsPageDesign.h"

#include "Options.h"
#include "my_app.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COptionsPageDesign dialog

IMPLEMENT_DYNAMIC(COptionsPageDesign, CPropertyPage)
COptionsPageDesign::COptionsPageDesign()
	: CPropertyPage(COptionsPageDesign::IDD)
{
}

COptionsPageDesign::~COptionsPageDesign()
{
}

void COptionsPageDesign::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_SHOW_CANVAS_BORDER, COptions::Instance()->m_bShowCanvasBorder);
	DDX_Check(pDX, IDC_CHECK_SHOW_DESIGN_TOOLTIP, COptions::Instance()->m_bShowDesignToolTip);
	DDX_Check(pDX, IDC_CHECK_CENTER_DESIGN_TOOLTIP, COptions::Instance()->m_bCenterDesignToolTip);
}


BEGIN_MESSAGE_MAP(COptionsPageDesign, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_SHOW_DESIGN_TOOLTIP, OnBnClickedCheckShowCanvasTip)
END_MESSAGE_MAP()


// COptionsPageDesign message handlers

BOOL COptionsPageDesign::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPageDesign::OnBnClickedCheckShowCanvasTip()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}

void COptionsPageDesign::UpdateControls()
{
	GetDlgItem(IDC_CHECK_CENTER_DESIGN_TOOLTIP)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_SHOW_DESIGN_TOOLTIP) == BST_CHECKED);
}

BOOL COptionsPageDesign::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	my_app.ShowCanvasBorder(COptions::Instance()->m_bShowCanvasBorder ? true: false);

	return CPropertyPage::OnApply();
}
