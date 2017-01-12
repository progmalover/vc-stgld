#pragma once
#include "CategoryPage.h"
#include "SlidingPanelGroup.h"
#include "SlidingPanel.h"
#include "CategoryExplorerPage.h"


// CCategoryResourcePage dialog

class CCategoryResourcePage : public CCategoryPage
{
//	DECLARE_DYNAMIC(CCategoryResourcePage)

public:
	CImageList m_imgList;
	CSlidingPanelGroup m_PanelGroup;
	CCategoryExplorerPage m_ExplorerShapePage;
	CCategoryExplorerPage m_ExplorerSoundPage;
	CCategoryExplorerPage m_ExplorerButtonPage;
	CCategoryExplorerPage m_ExplorerGreetingCardPage;
	CCategoryExplorerPage m_ExplorerPreloadPage;
	CCategoryExplorerPage m_ExplorerBackgroundPage;
	CSlidingPanel m_ShapePanel;	
	CSlidingPanel m_SoundPanel;
	CSlidingPanel m_ButtonPanel;
	CSlidingPanel m_GreetingCardPanel;
	CSlidingPanel m_PreloadPanel;
	CSlidingPanel m_BackgroundPanel;

	CCategoryResourcePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCategoryResourcePage();

// Dialog Data
	enum { IDD = IDD_CATEGORY_RESOURCE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnSetActive();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
};
