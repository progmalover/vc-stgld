// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "MainToolBar.h"
#include "MruMgr.h"
#include <list>

typedef std::list <CString> STRING_LIST;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	DECLARE_DYNCREATE(CMainFrame)
	CMainFrame();

// Attributes
protected:

public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMainToolBar	m_wndToolBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
//	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcPaint();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
public:
	afx_msg void OnPreviewInDefaultBrowser();
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPreviewBrowsersRange(UINT nID);
protected:
	HWND m_hWndFocused;
	BOOL LoadBarStateEx(LPCTSTR lpszProfileName);
	BOOL GetToolTipText(int nID, CString &strTip);
	void ExternalPreview(LPCTSTR lpszBrowser);
	void LoadDefaultFloaterState();

	STRING_LIST m_aPreviweFiles;
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CMainToolBar &GetToolBar();
	afx_msg void OnPreviewEditBrowserList();
	afx_msg void OnUpdatePreviewBrowser1(CCmdUI *pCmdUI);
protected:
	HACCEL m_hAccelUndo;
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	void EnableDocking(DWORD dwDockStyle);
public:
    afx_msg LRESULT OnTimeLineMessage(WPARAM wp, LPARAM lp);
	afx_msg void OnHelp();
	afx_msg void OnHelpHelptopic();
	afx_msg void OnHelpCheckforupdates();
	afx_msg void OnToolsOptions();
	afx_msg void OnHelpTutorial();
	afx_msg void OnHelpViewSamples();

	afx_msg void OnViewTimeline();
	afx_msg void OnUpdateViewTimeline(CCmdUI *pCmdUI);

	afx_msg void OnViewScenes();
	afx_msg void OnUpdateViewScenes(CCmdUI *pCmdUI);

	afx_msg void OnViewResetPaneLocations();
	afx_msg void OnToolSmallButtons();
	afx_msg void OnUpdateToolSmallButtons(CCmdUI *pCmdUI);
	afx_msg void OnToolLargeButtons();
	afx_msg void OnUpdateToolLargeButtons(CCmdUI *pCmdUI);
	virtual BOOL DestroyWindow();
	afx_msg void OnViewCustomizetoolbar();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI *pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnSwitchWindow();

	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	
	afx_msg void OnViewResource();
	afx_msg void OnUpdateViewResource(CCmdUI *pCmdUI);
	afx_msg void OnViewFiles();
	afx_msg void OnUpdateViewFiles(CCmdUI *pCmdUI);
	afx_msg void OnViewTools();
	afx_msg void OnUpdateViewTools(CCmdUI *pCmdUI);

	afx_msg void OnAddScene();

	afx_msg void OnAddEffect();
	afx_msg void OnUpdateAddEffect(CCmdUI *pCmdUI);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
};
