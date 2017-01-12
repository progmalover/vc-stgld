// DlgSelectBackgrndPos.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgSelectBackgrndPos.h"
#include ".\dlgselectbackgrndpos.h"


// CDlgSelectBackgrndPos dialog

IMPLEMENT_DYNAMIC(CDlgSelectBackgrndPos, CDialog)
CDlgSelectBackgrndPos::CDlgSelectBackgrndPos(int selected /*= 0*/, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectBackgrndPos::IDD, pParent)
	, m_selected(selected)
{
}

CDlgSelectBackgrndPos::~CDlgSelectBackgrndPos()
{
}

void CDlgSelectBackgrndPos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_POS, m_cmbPos);
}


BEGIN_MESSAGE_MAP(CDlgSelectBackgrndPos, CDialog)
END_MESSAGE_MAP()


// CDlgSelectBackgrndPos message handlers

BOOL CDlgSelectBackgrndPos::OnInitDialog()
{
	CDialog::OnInitDialog();

	FillComboBox(&m_cmbPos, IDS_LIST_BACKGROUND_POS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectBackgrndPos::OnOK()
{
	m_selected = m_cmbPos.GetCurSel();

	CDialog::OnOK();
}
