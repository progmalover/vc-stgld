// DlgApplyTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgApplyTemplate.h"
#include ".\dlgapplytemplate.h"
#include "gldInstance.h"
#include "filepath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgApplyTemplate dialog

CDlgApplyTemplate::CDlgApplyTemplate(CListTemplate::FilterTemplate filter, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgApplyTemplate::IDD, pParent)
	, m_Filter(filter)
{
}

CDlgApplyTemplate::~CDlgApplyTemplate()
{
	if (m_strPreviewFile.GetLength() > 0)
	{
		DeleteFile(m_strPreviewFile);
	}
}

void CDlgApplyTemplate::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_TEMPLATE, m_lstTemplate);
	DDX_Control(pDX, IDC_PLAYER, m_Player);
}


BEGIN_MESSAGE_MAP(CDlgApplyTemplate, CResizableDialog)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_LIST_TEMPLATE, OnLbnSelchangeListTemplate)
	ON_BN_CLICKED(IDC_BTN_RENAME, OnBnClickedBtnRename)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
END_MESSAGE_MAP()


// CDlgApplyTemplate message handlers

BOOL CDlgApplyTemplate::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	m_Player.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	AddAnchor(IDC_LIST_TEMPLATE, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_STATIC_PLAYER, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_RENAME, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_BTN_DELETE, BOTTOM_LEFT, BOTTOM_LEFT);
	Reposition();
	m_lstTemplate.Fill(m_Filter);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgApplyTemplate::Reposition()
{
	if (m_Player.m_hWnd != NULL)
	{
		CRect rcWnd;
		GetDlgItem(IDC_STATIC_PLAYER)->GetWindowRect(&rcWnd);
		ScreenToClient(&rcWnd);
		m_Player.MoveWindow(&rcWnd, TRUE);
	}	
}

void CDlgApplyTemplate::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	if (m_hWnd != NULL)
	{
		Reposition();
	}	
}

void CDlgApplyTemplate::OnLbnSelchangeListTemplate()
{
	m_strTemplatePath = m_lstTemplate.GetCurTemplate();
	UpdateControls();
	if (m_strTemplatePath.GetLength() > 0)
	{
		if (m_strPreviewFile.GetLength() > 0)
		{
			DeleteFile(m_strPreviewFile);
		}
		m_strPreviewFile = GetTempFile(NULL, "~combeffect", "swf");
		if (SUCCEEDED(gldInstance::ExtractDemo(m_strTemplatePath, m_strPreviewFile)))
		{
			m_Player.Zoom(0);
			m_Player.SetMovie("1");
			m_Player.SetMovie(m_strPreviewFile);
			return;
		}		
	}	
	m_Player.SetMovie("1");
	if (m_strTemplatePath.GetLength() > 0)
	{
		AfxMessageBox(IDS_FAILED_CREATE_PREVIEW); // that's some error
	}
}

void CDlgApplyTemplate::UpdateControls()
{
	if (m_strTemplatePath.GetLength() > 0)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_RENAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_DELETE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_RENAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_DELETE)->EnableWindow(FALSE);
	}
}

void CDlgApplyTemplate::OnOK()
{
	if (m_strTemplatePath.GetLength() == 0)
	{
		return;
	}
	CResizableDialog::OnOK();
}

void CDlgApplyTemplate::OnBnClickedBtnRename()
{
	m_lstTemplate.RenameCurSel();
}

void CDlgApplyTemplate::OnBnClickedBtnDelete()
{
	m_lstTemplate.DeleteCurSel();
}
