// CategoryImportPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryImportPage.h"
#include "filepath.h"
#include "DeferWindowPos.h"
#pragma comment( lib, "Rpcrt4")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CCategoryImportPage dialog

//IMPLEMENT_DYNAMIC(CCategoryImportPage, CCategoryPage)
CCategoryImportPage::CCategoryImportPage(CWnd* pParent /*=NULL*/)
	: CCategoryPage(CCategoryImportPage::IDD, pParent)
	, m_ExplorerWnd(RESOURCE_ALL, FALSE, TRUE)
{
}

CCategoryImportPage::~CCategoryImportPage()
{
}

void CCategoryImportPage::DoDataExchange(CDataExchange* pDX)
{
	CCategoryPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCategoryImportPage, CCategoryPage)
END_MESSAGE_MAP()


// CCategoryImportPage message handlers

BOOL CCategoryImportPage::OnInitDialog()
{
	CCategoryPage::OnInitDialog();

	CRect rc;
	GetDlgItem(IDC_STATIC_RECT)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultResourceDir);
	m_ExplorerWnd.SetProfileSection("Files");
	m_ExplorerWnd.EnableDragResource(TRUE);
	m_ExplorerWnd.SetActionObject(this);
	m_ExplorerWnd.Create(m_ExplorerWnd.IDD, this);
	m_ExplorerWnd.MoveWindow(&rc);

	AddAnchor(m_ExplorerWnd.m_hWnd, TOP_LEFT, BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCategoryImportPage::OnSetActive()
{
	if(m_bFirstActivePage)
	{
		m_ExplorerWnd.EnsureSelectionVisible();
	}

	CCategoryPage::OnSetActive();
}