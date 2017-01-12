// CategoryExplorerPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryExplorerPage.h"
#include "DeferWindowPos.h"
#include "filepath.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CCategoryExplorerPage dialog

//IMPLEMENT_DYNAMIC(CCategoryExplorerPage, CCategoryPage)
CCategoryExplorerPage::CCategoryExplorerPage(int nResourceType,CWnd* pParent /*=NULL*/)
	: CCategoryPage(CCategoryExplorerPage::IDD, pParent)
	, m_ExplorerWnd(nResourceType, FALSE, FALSE)
{
	switch(nResourceType)
	{	
	case RESOURCE_SHAPES:
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultShapesDir);
		m_ExplorerWnd.SetProfileSection("Resource\\Shapes");
		break;
	case RESOURCE_BUTTONS:
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultButtonsDir);
		m_ExplorerWnd.SetProfileSection("Resource\\Buttons");
		break;	
	case RESOURCE_SOUNDS:
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultSoundsDir);
		m_ExplorerWnd.SetProfileSection("Resource\\Sounds");
		break;
	case RESOURCE_GREETINGCARD:
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultGreetingCardsDir);
		m_ExplorerWnd.SetProfileSection("Resource\\Sounds");
		break;
	case RESOURCE_PRELOAD:
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultPreloadsDir);
		m_ExplorerWnd.SetProfileSection("Resource\\Sounds");
		break;
	case RESOURCE_BACKGROUND:
		m_ExplorerWnd.SetResourceRootDir(GetModuleFilePath() + CString("\\") + szDefaultBackgroundsDir);
		m_ExplorerWnd.SetProfileSection("Resource\\Sounds");
		break;
	default:
		ASSERT(0);
		break;
	}
}

CCategoryExplorerPage::~CCategoryExplorerPage()
{
}

void CCategoryExplorerPage::DoDataExchange(CDataExchange* pDX)
{
	CCategoryPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCategoryExplorerPage, CCategoryPage)
END_MESSAGE_MAP()


// CCategoryExplorerPage message handlers

BOOL CCategoryExplorerPage::OnInitDialog()
{
	CCategoryPage::OnInitDialog();

	CRect rc;
	GetClientRect(&rc);

	m_ExplorerWnd.Create(m_ExplorerWnd.IDD, this);
	m_ExplorerWnd.MoveWindow(&rc);
	m_ExplorerWnd.SetActionObject(this);
	m_ExplorerWnd.EnableDragResource(TRUE);

	AddAnchor(m_ExplorerWnd.m_hWnd, TOP_LEFT, BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCategoryExplorerPage::OnSetActive()
{
	if(m_bFirstActivePage)
	{
		m_ExplorerWnd.EnsureSelectionVisible();
	}

	CCategoryPage::OnSetActive();
}