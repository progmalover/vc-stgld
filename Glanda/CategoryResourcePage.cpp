// CategoryResourcePage.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "CategoryResourcePage.h"
#include "filepath.h"
#include "Symbols.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CCategoryResourcePage dialog

//IMPLEMENT_DYNAMIC(CCategoryResourcePage, CCategoryPage)
CCategoryResourcePage::CCategoryResourcePage(CWnd* pParent /*=NULL*/)
	: CCategoryPage(CCategoryResourcePage::IDD, pParent)
	, m_ExplorerShapePage(RESOURCE_SHAPES)	
	, m_ExplorerSoundPage(RESOURCE_SOUNDS)
	, m_ExplorerButtonPage(RESOURCE_BUTTONS)
	, m_ExplorerGreetingCardPage(RESOURCE_GREETINGCARD)
	, m_ExplorerPreloadPage(RESOURCE_PRELOAD)
	, m_ExplorerBackgroundPage(RESOURCE_BACKGROUND)
	, m_ShapePanel(&m_ExplorerShapePage, INDEX_SHAPE, IDS_PANEL_CAPTION_SHAPE)	
	, m_SoundPanel(&m_ExplorerSoundPage, INDEX_SOUND, IDS_PANEL_CAPTION_SOUND)
	, m_ButtonPanel(&m_ExplorerButtonPage, INDEX_BUTTON, IDS_PANEL_CAPTION_BUTTON)
	, m_GreetingCardPanel(&m_ExplorerGreetingCardPage, INDEX_SPRITE, IDS_PANEL_CAPTION_GREETINGCARD)
	, m_PreloadPanel(&m_ExplorerPreloadPage, INDEX_SPRITE, IDS_PANEL_CAPTION_PRELOAD)
	, m_BackgroundPanel(&m_ExplorerBackgroundPage, INDEX_SPRITE, IDS_PANEL_CAPTION_BACKGROUND)
{
}

CCategoryResourcePage::~CCategoryResourcePage()
{
}

void CCategoryResourcePage::DoDataExchange(CDataExchange* pDX)
{
	CCategoryPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCategoryResourcePage, CCategoryPage)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CCategoryResourcePage message handlers

void CCategoryResourcePage::OnDestroy()
{
	CCategoryPage::OnDestroy();

	if(m_PanelGroup.GetActivePanel() >= 0)
	{
		m_PanelGroup.SaveActivePanel("Resource");
	}
	RemoveAnchor(m_PanelGroup.m_hWnd);
	m_PanelGroup.DestroyWindow();
}

BOOL CCategoryResourcePage::OnInitDialog()
{
	CCategoryPage::OnInitDialog();

	CRect rc;
	GetDlgItem(IDC_STATIC_SLIDING_RECT)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_PanelGroup.Create(WS_CHILD | WS_VISIBLE, rc, this, 1);


	if (m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		if (bmp.LoadBitmap(IDB_SYMBOLS))
		{
			m_imgList.Add(&bmp, RGB(255, 0, 255));
			m_PanelGroup.SetImageList(&m_imgList);
		}
	}

	m_PanelGroup.AddPanel(&m_ShapePanel);	
	m_PanelGroup.AddPanel(&m_SoundPanel);
	m_PanelGroup.AddPanel(&m_ButtonPanel);
	m_PanelGroup.AddPanel(&m_GreetingCardPanel);
	m_PanelGroup.AddPanel(&m_PreloadPanel);
	m_PanelGroup.AddPanel(&m_BackgroundPanel);

	AddAnchor(m_PanelGroup.m_hWnd, TOP_LEFT, BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCategoryResourcePage::OnSetActive()
{
	if(m_bFirstActivePage)
	{
		m_PanelGroup.LoadActivePanel("Resource");
	}

	CCategoryPage::OnSetActive();
}