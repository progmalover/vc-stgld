// PageEffectPreviewOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PageEffectPreviewOptions.h"
#include ".\pageeffectpreviewoptions.h"
#include "EffectCommonParameters.h"

#define EP_EFFECT_PREVIEW_TEXT		"Effect.Preview.Text"
#define EP_EFFECT_PREVIEW_HAS_SEL	"Effect.Preview.HasSel"
#define EP_EFFECT_PREVIEW_USE_SEL	"Effect.Preview.UseSel"

// CPageEffectPreviewOptions dialog

IMPLEMENT_DYNCREATE(CPageEffectPreviewOptions, CGLD_PropertyPage)

// Message map

BEGIN_MESSAGE_MAP(CPageEffectPreviewOptions, CGLD_PropertyPage)
	ON_BN_CLICKED(IDC_CHECK_USE_SELECTED, OnBnClickedCheckUseSelected)
END_MESSAGE_MAP()



// Initialize class factory and guid

// {E288EEE8-16D9-4E56-B9C2-3A8C4CCFFB0F}
IMPLEMENT_OLECREATE_EX(CPageEffectPreviewOptions, "Glanda.PageEffectPreviewOptions",
	0xe288eee8, 0x16d9, 0x4e56, 0xb9, 0xc2, 0x3a, 0x8c, 0x4c, 0xcf, 0xfb, 0xf)



// CPageEffectPreviewOptions::CPageEffectPreviewOptionsFactory::UpdateRegistry -
// Adds or removes system registry entries for CPageEffectPreviewOptions

BOOL CPageEffectPreviewOptions::CPageEffectPreviewOptionsFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, 0);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CPageEffectPreviewOptions::CPageEffectPreviewOptions - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CPageEffectPreviewOptions::CPageEffectPreviewOptions() :
	CGLD_PropertyPage(IDD, IDS_PAGE_EFFECT_PREVIEW_OPTIONS)		
{
	m_bHasSel = FALSE;
	m_bUseSel = FALSE;
}



// CPageEffectPreviewOptions::DoDataExchange - Moves data between page and properties

void CPageEffectPreviewOptions::DoDataExchange(CDataExchange* pDX)
{
	//DDP_PostProcessing(pDX);	
}


BOOL CPageEffectPreviewOptions::OnApply()
{
	CComQIPtr<IGLD_Parameters> pIParas(m_pIUnknown);
	if (pIParas != NULL)
	{
		CEdit *pEdtText = (CEdit *)GetDlgItem(IDC_EDIT_TEXT);
		ASSERT_VALID(pEdtText);
		CString strText;
		pEdtText->GetWindowText(strText);
		BSTR bstrText = strText.AllocSysString();
		PutParameter(pIParas, EP_EFFECT_PREVIEW_TEXT, bstrText);
		PutParameter(pIParas, EP_EFFECT_PREVIEW_USE_SEL, m_bUseSel);
		return TRUE;
	}
	return FALSE;
}
// CPageEffectPreviewOptions message handlers

BOOL CPageEffectPreviewOptions::OnInitDialog()
{
	CGLD_PropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here	
	CComQIPtr<IGLD_Parameters> pIParas(m_pIUnknown);
	if (pIParas != NULL)
	{
		BSTR bstrText = NULL;
		GetParameter(pIParas, EP_EFFECT_PREVIEW_TEXT, &bstrText);
		if (bstrText != NULL)
		{
			CEdit *pEdtText = (CEdit *)GetDlgItem(IDC_EDIT_TEXT);
			ASSERT_VALID(pEdtText);
			pEdtText->SetWindowText(CString(bstrText));
			SysFreeString(bstrText);
		}
		
		GetParameter(pIParas, EP_EFFECT_PREVIEW_HAS_SEL, &m_bHasSel);
		if (m_bHasSel)
		{
			GetParameter(pIParas, EP_EFFECT_PREVIEW_USE_SEL, &m_bUseSel);			
		}
	}

	CButton *pBtnUseSel = (CButton *)GetDlgItem(IDC_CHECK_USE_SELECTED);
	ASSERT_VALID(pBtnUseSel);
	pBtnUseSel->SetCheck(m_bUseSel ? BST_CHECKED : BST_UNCHECKED);
	pBtnUseSel->EnableWindow(m_bHasSel);
	if (m_bUseSel)
	{
		CWnd *pWnd = GetDlgItem(IDC_EDIT_TEXT);
		ASSERT_VALID(pWnd);
		pWnd->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageEffectPreviewOptions::OnBnClickedCheckUseSelected()
{
	// TODO: Add your control notification handler code here
	if (m_bHasSel)
	{
		CButton *pBtnThis = (CButton *)GetDlgItem(IDC_CHECK_USE_SELECTED);
		ASSERT_VALID(pBtnThis);
		m_bUseSel = (pBtnThis->GetCheck() == BST_CHECKED);
		CWnd *pWnd = GetDlgItem(IDC_EDIT_TEXT);
		ASSERT_VALID(pWnd);
		pWnd->EnableWindow(!m_bUseSel);
	}
}
