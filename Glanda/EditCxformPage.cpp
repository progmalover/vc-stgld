// EditCxformPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditCxformPage.h"
#include "CmdModifyCxform.h"
#include "DrawHelper.h"

// CEditCxformPage dialog

IMPLEMENT_DYNAMIC(CEditCxformPage, CPropertyPage)
CEditCxformPage::CEditCxformPage(gldInstance* pInstance)
	: CPropertyPage(CEditCxformPage::IDD)
	, m_pInstance(pInstance)
	, m_pCmd(NULL)
{
	m_cxstyle = m_pInstance->m_cxStyle;
	m_cxcolor = m_pInstance->m_tint;
	CTransAdaptor::GCX2TCX(m_pInstance->m_cxform, m_cxform);
}

CEditCxformPage::~CEditCxformPage()
{
}

void CEditCxformPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_COLOR, m_btnColor);
	DDX_Control(pDX, IDC_STATIC_SPIN_COLOR_FACTOR, m_btnColorFactor);
	DDX_Control(pDX, IDC_STATIC_COLOR_PERCENT, m_stcColorPercent);
	DDX_Control(pDX, IDC_STATIC_SPIN_ALPHA_FACTOR, m_btnAlphaFactor);
	DDX_Control(pDX, IDC_STATIC_ALPHA_PERCENT, m_stcAlphaPercent);

	DDX_Control(pDX, IDC_EDIT_COLOR_FACTOR, m_edtColorFactor);
	DDX_Control(pDX, IDC_EDIT_ALPHA_FACTOR, m_edtAlphaFactor);

	DDX_Control(pDX, IDC_EDIT_R_FACTOR, m_edtRFactor);
	DDX_Control(pDX, IDC_EDIT_G_FACTOR, m_edtGFactor);
	DDX_Control(pDX, IDC_EDIT_B_FACTOR, m_edtBFactor);
	DDX_Control(pDX, IDC_EDIT_A_FACTOR, m_edtAFactor);

	DDX_Control(pDX, IDC_EDIT_R, m_edtR);
	DDX_Control(pDX, IDC_EDIT_G, m_edtG);
	DDX_Control(pDX, IDC_EDIT_B, m_edtB);
	DDX_Control(pDX, IDC_EDIT_A, m_edtA);

	DDX_Control(pDX, IDC_STATIC_SPIN_R_FACTOR, m_btnRFactor);
	DDX_Control(pDX, IDC_STATIC_SPIN_G_FACTOR, m_btnGFactor);
	DDX_Control(pDX, IDC_STATIC_SPIN_B_FACTOR, m_btnBFactor);
	DDX_Control(pDX, IDC_STATIC_SPIN_A_FACTOR, m_btnAFactor);

	DDX_Control(pDX, IDC_STATIC_SPIN_R, m_btnR);
	DDX_Control(pDX, IDC_STATIC_SPIN_G, m_btnG);
	DDX_Control(pDX, IDC_STATIC_SPIN_B, m_btnB);
	DDX_Control(pDX, IDC_STATIC_SPIN_A, m_btnA);
}


BEGIN_MESSAGE_MAP(CEditCxformPage, CPropertyPage)
	ON_BN_CLICKED(IDC_COLOR_TRANS_NONE, OnBnClickedColorTransNone)
	ON_BN_CLICKED(IDC_COLOR_TRANS_FADE_TO_ALPHA, OnBnClickedColorTransFadeToAlpha)
	ON_BN_CLICKED(IDC_COLOR_TRANS_FADE_TO_COLOR, OnBnClickedColorTransFadeToColor)
	ON_BN_CLICKED(IDC_COLOR_TRANS_ADVANCED, OnBnClickedColorTransAdvanced)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_ALPHA_FACTOR, OnModifyAlphaFactor)

	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_COLOR, OnModifyColor)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_COLOR_FACTOR, OnModifyColor)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_R_FACTOR, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_G_FACTOR, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_B_FACTOR, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_A_FACTOR, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_R, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_G, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_B, OnModifyAdvanced)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_A, OnModifyAdvanced)
END_MESSAGE_MAP()


// CEditCxformPage message handlers

BOOL CEditCxformPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_edtAlphaFactor.SetRange(0, 100, 0, FALSE, TRUE);
	m_btnAlphaFactor.SetRange(0, 100);
	m_btnAlphaFactor.SetBuddyWindow(m_edtAlphaFactor.m_hWnd);

	m_edtColorFactor.SetRange(0, 100, 100, FALSE, TRUE);
	m_btnColorFactor.SetRange(0, 100);
	m_btnColorFactor.SetBuddyWindow(m_edtColorFactor.m_hWnd);


	m_edtRFactor.SetRange(-100, 100, 100, FALSE, TRUE);
	m_btnRFactor.SetRange(-100, 100);
	m_btnRFactor.SetBuddyWindow(m_edtRFactor.m_hWnd);

	m_edtGFactor.SetRange(-100, 100, 100, FALSE, TRUE);
	m_btnGFactor.SetRange(-100, 100);
	m_btnGFactor.SetBuddyWindow(m_edtGFactor.m_hWnd);

	m_edtBFactor.SetRange(-100, 100, 100, FALSE, TRUE);
	m_btnBFactor.SetRange(-100, 100);
	m_btnBFactor.SetBuddyWindow(m_edtBFactor.m_hWnd);

	m_edtAFactor.SetRange(-100, 100, 100, FALSE, TRUE);
	m_btnAFactor.SetRange(-100, 100);
	m_btnAFactor.SetBuddyWindow(m_edtAFactor.m_hWnd);


	m_edtR.SetRange(-255, 255, 255, FALSE, TRUE);
	m_btnR.SetRange(-255, 255);
	m_btnR.SetBuddyWindow(m_edtR.m_hWnd);

	m_edtG.SetRange(-255, 255, 255, FALSE, TRUE);
	m_btnG.SetRange(-255, 255);
	m_btnG.SetBuddyWindow(m_edtG.m_hWnd);

	m_edtB.SetRange(-255, 255, 255, FALSE, TRUE);
	m_btnB.SetRange(-255, 255);
	m_btnB.SetBuddyWindow(m_edtB.m_hWnd);

	m_edtA.SetRange(-255, 255, 255, FALSE, TRUE);
	m_btnA.SetRange(-255, 255);
	m_btnA.SetBuddyWindow(m_edtA.m_hWnd);

	/*
	AddToolTip(IDC_BUTTON_CONSTRAIN_ANGLE);
	AddToolTip(IDC_BUTTON_CONSTRAIN_SIZE);
	*/


	switch(m_cxstyle)
	{
	case INST_COLOR_TRANS_STYLE_NONE:
		CheckRadioButton(IDC_COLOR_TRANS_NONE, IDC_COLOR_TRANS_ADVANCED, IDC_COLOR_TRANS_NONE);
		break;
	case INST_COLOR_TRANS_STYLE_ALPHA:
		CheckRadioButton(IDC_COLOR_TRANS_NONE, IDC_COLOR_TRANS_ADVANCED, IDC_COLOR_TRANS_FADE_TO_ALPHA);
		break;
	case INST_COLOR_TRANS_STYLE_COLOR:
		CheckRadioButton(IDC_COLOR_TRANS_NONE, IDC_COLOR_TRANS_ADVANCED, IDC_COLOR_TRANS_FADE_TO_COLOR);
		break;
	case INST_COLOR_TRANS_STYLE_ADVANCED:
		CheckRadioButton(IDC_COLOR_TRANS_NONE, IDC_COLOR_TRANS_ADVANCED, IDC_COLOR_TRANS_ADVANCED);
		break;
	default:
		break;
	}
	UpdateControls(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditCxformPage::OnBnClickedColorTransNone()
{
	m_cxstyle = INST_COLOR_TRANS_STYLE_NONE;
	UpdateControls();
	OnModifyNone();
}

void CEditCxformPage::OnBnClickedColorTransFadeToAlpha()
{
	m_cxstyle = INST_COLOR_TRANS_STYLE_ALPHA;
	UpdateControls();
	OnModifyAlphaFactor();
}

void CEditCxformPage::OnBnClickedColorTransFadeToColor()
{
	m_cxstyle = INST_COLOR_TRANS_STYLE_COLOR;
	UpdateControls();
	OnModifyColor();
}

void CEditCxformPage::OnBnClickedColorTransAdvanced()
{
	m_cxstyle = INST_COLOR_TRANS_STYLE_ADVANCED;
	UpdateControls();
	OnModifyAdvanced();
}

void CEditCxformPage::OnModifyNone()
{
	m_cxform = TCxform();

	CGuardDrawOnce xDraw;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
	}
	m_pCmd = new CCmdModifyCxform(m_pInstance, m_cxstyle, m_cxcolor, m_cxform);
	m_pCmd->Execute();
}

void CEditCxformPage::OnModifyAlphaFactor()
{
	float aa = GetDlgItemInt(IDC_EDIT_ALPHA_FACTOR, NULL, FALSE) / (float)100;
	m_cxform = TCxform();
	m_cxform.aa = FLOATTOFIXED(aa);
	m_cxform.ab = INTTOFIXED(0);

	CGuardDrawOnce xDraw;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
	}
	m_pCmd = new CCmdModifyCxform(m_pInstance, m_cxstyle, m_cxcolor, m_cxform);
	m_pCmd->Execute();
}

void CEditCxformPage::OnModifyColor()
{
	float f = (float)GetDlgItemInt(IDC_EDIT_COLOR_FACTOR, NULL, FALSE) / 100;
	m_cxform = TCxform();
	m_cxform.ra = FLOATTOFIXED(1 - f);
	m_cxform.ga = FLOATTOFIXED(1 - f);
	m_cxform.ba = FLOATTOFIXED(1 - f);

	COLORREF clr = m_btnColor.GetColor();
	m_cxcolor = RGBA(GetRValue(clr), GetGValue(clr), GetBValue(clr), round(f * 255));

	m_cxform.rb = FLOATTOFIXED(GetRValue(m_cxcolor) * f);
	m_cxform.gb = FLOATTOFIXED(GetGValue(m_cxcolor) * f);
	m_cxform.bb = FLOATTOFIXED(GetBValue(m_cxcolor) * f);

	CGuardDrawOnce xDraw;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
	}
	m_pCmd = new CCmdModifyCxform(m_pInstance, m_cxstyle, m_cxcolor, m_cxform);
	m_pCmd->Execute();
}

void CEditCxformPage::OnModifyAdvanced()
{
	m_cxform.ra = FLOATTOFIXED((float)(int)GetDlgItemInt(IDC_EDIT_R_FACTOR, NULL, TRUE) / 100);
	m_cxform.ga = FLOATTOFIXED((float)(int)GetDlgItemInt(IDC_EDIT_G_FACTOR, NULL, TRUE) / 100);
	m_cxform.ba = FLOATTOFIXED((float)(int)GetDlgItemInt(IDC_EDIT_B_FACTOR, NULL, TRUE) / 100);
	m_cxform.aa = FLOATTOFIXED((float)(int)GetDlgItemInt(IDC_EDIT_A_FACTOR, NULL, TRUE) / 100);

	m_cxform.rb = INTTOFIXED(GetDlgItemInt(IDC_EDIT_R, NULL, TRUE));
	m_cxform.gb = INTTOFIXED(GetDlgItemInt(IDC_EDIT_G, NULL, TRUE));
	m_cxform.bb = INTTOFIXED(GetDlgItemInt(IDC_EDIT_B, NULL, TRUE));
	m_cxform.ab = INTTOFIXED(GetDlgItemInt(IDC_EDIT_A, NULL, TRUE));

	CGuardDrawOnce xDraw;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
	}
	m_pCmd = new CCmdModifyCxform(m_pInstance, m_cxstyle, m_cxcolor, m_cxform);
	m_pCmd->Execute();
}

void CEditCxformPage::UpdateControls(BOOL bUpdateAll)
{
	switch (m_cxstyle)
	{
	case COLOR_TRANS_STYLE_NONE:
		ShowColorStyleAlphaControls(FALSE);
		ShowColorTransStyleColorControls(FALSE);
		ShowColorStyleAdvancedControls(FALSE);
		break;

	case COLOR_TRANS_STYLE_ALPHA:
		ShowColorStyleAlphaControls(TRUE);
		ShowColorTransStyleColorControls(FALSE);
		ShowColorStyleAdvancedControls(FALSE);
		break;

	case COLOR_TRANS_STYLE_COLOR:
		ShowColorStyleAlphaControls(FALSE);
		ShowColorTransStyleColorControls(TRUE);
		ShowColorStyleAdvancedControls(FALSE);
		break;

	case COLOR_TRANS_STYLE_ADVANCED:
		ShowColorStyleAlphaControls(FALSE);
		ShowColorTransStyleColorControls(FALSE);
		ShowColorStyleAdvancedControls(TRUE);
		break;
	}

	if(m_cxstyle == COLOR_TRANS_STYLE_ALPHA || bUpdateAll)
	{
		SetDlgItemInt(IDC_EDIT_ALPHA_FACTOR, max(0, round(FIXEDTOFLOAT(m_cxform.aa) * 100)));
	}

	if(m_cxstyle == COLOR_TRANS_STYLE_COLOR || bUpdateAll)
	{
		m_btnColor.SetColor(m_cxcolor & 0x00ffffff, 255);
		SetDlgItemInt(IDC_EDIT_COLOR_FACTOR, round((float)GetAValue(m_cxcolor) * 100 / 255), TRUE);
	}

	if(m_cxstyle == COLOR_TRANS_STYLE_ADVANCED || bUpdateAll)
	{
		SetDlgItemInt(IDC_EDIT_R_FACTOR, round(FIXEDTOFLOAT(m_cxform.ra) * 100));
		SetDlgItemInt(IDC_EDIT_G_FACTOR, round(FIXEDTOFLOAT(m_cxform.ga) * 100));
		SetDlgItemInt(IDC_EDIT_B_FACTOR, round(FIXEDTOFLOAT(m_cxform.ba) * 100));
		SetDlgItemInt(IDC_EDIT_A_FACTOR, round(FIXEDTOFLOAT(m_cxform.aa) * 100));

		SetDlgItemInt(IDC_EDIT_R, round(FIXEDTOFLOAT(m_cxform.rb)));
		SetDlgItemInt(IDC_EDIT_G, round(FIXEDTOFLOAT(m_cxform.gb)));
		SetDlgItemInt(IDC_EDIT_B, round(FIXEDTOFLOAT(m_cxform.bb)));
		SetDlgItemInt(IDC_EDIT_A, round(FIXEDTOFLOAT(m_cxform.ab)));
	}
}


void CEditCxformPage::ShowColorStyleAlphaControls(BOOL bShow)
{
	m_edtAlphaFactor.EnableWindow(bShow);
	m_btnAlphaFactor.EnableWindow(bShow);
	m_stcAlphaPercent.EnableWindow(bShow);
}

void CEditCxformPage::ShowColorTransStyleColorControls(BOOL bShow)
{
	m_btnColor.EnableWindow(bShow);
	m_edtColorFactor.EnableWindow(bShow);
	m_btnColorFactor.EnableWindow(bShow);
	m_stcColorPercent.EnableWindow(bShow);
}

void CEditCxformPage::ShowColorStyleAdvancedControls(BOOL bShow)
{
	GetDlgItem(IDC_STATIC_R1)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_R2)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_R3)->EnableWindow(bShow);

	GetDlgItem(IDC_STATIC_G1)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_G2)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_G3)->EnableWindow(bShow);

	GetDlgItem(IDC_STATIC_B1)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_B2)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_B3)->EnableWindow(bShow);

	GetDlgItem(IDC_STATIC_A1)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_A2)->EnableWindow(bShow);
	GetDlgItem(IDC_STATIC_A3)->EnableWindow(bShow);

	m_edtRFactor.EnableWindow(bShow); m_btnRFactor.EnableWindow(bShow);
	m_edtR.EnableWindow(bShow); m_btnR.EnableWindow(bShow);

	m_edtGFactor.EnableWindow(bShow); m_btnGFactor.EnableWindow(bShow);
	m_edtG.EnableWindow(bShow); m_btnG.EnableWindow(bShow);

	m_edtBFactor.EnableWindow(bShow); m_btnBFactor.EnableWindow(bShow);
	m_edtB.EnableWindow(bShow); m_btnB.EnableWindow(bShow);

	m_edtAFactor.EnableWindow(bShow); m_btnAFactor.EnableWindow(bShow);
	m_edtA.EnableWindow(bShow); m_btnA.EnableWindow(bShow);
}