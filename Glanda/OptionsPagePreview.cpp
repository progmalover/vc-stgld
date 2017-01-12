// OptionPagePreview.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OptionsPagePreview.h"
#include "Options.h"
#include ".\optionspagepreview.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// COptionsPagePreview dialog

IMPLEMENT_DYNAMIC(COptionsPagePreview, CPropertyPage)
COptionsPagePreview::COptionsPagePreview()
	: CPropertyPage(COptionsPagePreview::IDD)
{
}

COptionsPagePreview::~COptionsPagePreview()
{
}

void COptionsPagePreview::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Check(pDX, IDC_CHECK_QUIT_FULLSCREEN_ON_ESC, COptions::Instance()->m_bQuitFullscreenOnEsc);
	DDX_Check(pDX, IDC_CHECK_CLOSE_PREVIEW_ON_ESC, COptions::Instance()->m_bClosePreviewOnEsc);
	DDX_Check(pDX, IDC_CHECK_AUTO_HIDE_CAPTION, COptions::Instance()->m_bAutoHideCaption);
}


BEGIN_MESSAGE_MAP(COptionsPagePreview, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_LBN_SELCHANGE(IDC_LIST, OnLbnSelchangeList)
END_MESSAGE_MAP()

// COptionsPagePreview message handlers

BOOL COptionsPagePreview::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	COptions *pOptions = COptions::Instance();

	CString strAll;
	int count = 0;
	if (strAll.LoadString(IDS_LIST_PREVIEW_DIR))
	{
		for (int i = 0; i < 3; i++)
		{
			CString strDir;
			if (!AfxExtractSubString(strDir, strAll, pOptions->m_nPreviewDir[i], '\n'))
				break;
			count++;

			m_list.AddString(strDir);
			m_list.SetItemData(i, pOptions->m_nPreviewDir[i]);
		}
	}

	if (count != 3)
		AfxThrowResourceException();

	m_list.SetCurSel(0);

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPagePreview::OnBnClickedButtonUp()
{
	// TODO: Add your control notification handler code here

	int index = m_list.GetCurSel();
	if (index >= 1)
	{
		CString strDir;
		m_list.GetText(index, strDir);
		DWORD_PTR dwData = m_list.GetItemData(index);
		m_list.DeleteString(index);
		index = m_list.InsertString(index - 1, strDir);
		m_list.SetItemData(index, dwData);
		m_list.SetCurSel(index);
	}

	UpdateControls();
}

void COptionsPagePreview::OnBnClickedButtonDown()
{
	// TODO: Add your control notification handler code here

	int index = m_list.GetCurSel();
	if (index >= 0 && index <= 1)
	{
		CString strDir;
		m_list.GetText(index, strDir);
		DWORD_PTR dwData = m_list.GetItemData(index);
		m_list.DeleteString(index);
		index = m_list.InsertString(index + 1, strDir);
		m_list.SetItemData(index, dwData);
		m_list.SetCurSel(index);
	}

	UpdateControls();
}

void COptionsPagePreview::OnLbnSelchangeList()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}

void COptionsPagePreview::UpdateControls()
{
	int index = m_list.GetCurSel();
	GetDlgItem(IDC_BUTTON_UP)->EnableWindow(index >= 1);
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(index >= 0 && index <= 1);
}

void COptionsPagePreview::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	COptions *pOptions = COptions::Instance();
	for (int i = 0; i < 3; i++)
		pOptions->m_nPreviewDir[i] = m_list.GetItemData(i);

	CPropertyPage::OnOK();
}
