// EditMCReplaceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCReplaceDialog.h"
#include "filepath.h"
#include "DeferWindowPos.h"
#include "filepath.h"

// CEditMCReplaceDialog dialog

//IMPLEMENT_DYNAMIC(CEditMCReplaceDialog, CResizableDialog)
CEditMCReplaceDialog::CEditMCReplaceDialog(int nResourceType, CWnd* pParent /*=NULL*/)
	: CResizableDialog(IDD, pParent)
	, m_ExplorerWnd(nResourceType, TRUE, TRUE, TRUE)
	, m_nResourceType(nResourceType)
	, m_pCurSelResource(NULL)
{
}

CEditMCReplaceDialog::~CEditMCReplaceDialog()
{
	if(m_pCurSelResource)
	{
		delete m_pCurSelResource;
		m_pCurSelResource = NULL;
	}
}

void CEditMCReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditMCReplaceDialog, CResizableDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEditMCReplaceDialog message handlers

BOOL CEditMCReplaceDialog::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	CString strCaption;
	switch(m_nResourceType)
	{
	case RESOURCE_IMAGES:
		strCaption.LoadString(IDS_EDIT_MC_REPLACE_IMAGE_DIALOG_CAPTION);
		SetWindowText(strCaption);
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultImagesDir);
		SetProfileSection("Resource\\ReplaceImage");
		break;
	case RESOURCE_SHAPES:
		strCaption.LoadString(IDS_EDIT_MC_REPLACE_SHAPE_DIALOG_CAPTION);
		SetWindowText(strCaption);
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultShapesDir);
		SetProfileSection("Resource\\ReplaceShape");
		break;
	case RESOURCE_BUTTONS:
	case RESOURCE_MOVIECLIP:
		strCaption.LoadString(IDS_EDIT_MC_REPLACE_MOVIECLIP_DIALOG_CAPTION);
		SetWindowText(strCaption);
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultButtonsDir);
		SetProfileSection("Resource\\ReplaceButton");
		break;
	case RESOURCE_SOUNDS:
		strCaption.LoadString(IDS_EDIT_MC_REPLACE_SOUND_DIALOG_CAPTION);
		SetWindowText(strCaption);
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultSoundsDir);
		SetProfileSection("Resource\\ReplaceSound");
		break;
	default:
		ASSERT(0);
		break;
	}

	CRect rc;
	GetDlgItem(IDC_STATIC_RECT)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ExplorerWnd.Create(m_ExplorerWnd.IDD, this);
	m_ExplorerWnd.MoveWindow(&rc);
	m_ExplorerWnd.SetActionObject(this);

	AddAnchor(IDC_STATIC_RECT, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDOK, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(m_ExplorerWnd.m_hWnd, TOP_LEFT, BOTTOM_RIGHT);

	::LoadWindowPlacement(this, m_strProfileSection);
	m_ExplorerWnd.EnsureSelectionVisible();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMCReplaceDialog::OnDestroy()
{
	CResizableDialog::OnDestroy();
	::SaveWindowPlacement(this, m_strProfileSection);
}

void CEditMCReplaceDialog::OnResourceThumbAction(int nIndex, CResourceThumbListCtrl* pThumbListCtrl)
{
	OnOK();
}

void CEditMCReplaceDialog::OnOK()
{
	CResourceThumbItem* pThumbItem = m_ExplorerWnd.GetSelectedResource();
	if(pThumbItem)
	{
		m_pCurSelResource = new CResourceThumbItem(pThumbItem);
		CResizableDialog::OnOK();
	}
	else
	{
		AfxMessageBox(IDS_PROMPT_NONE_RESOURCE_SELECTED, MB_OK|MB_ICONINFORMATION);
	}
}