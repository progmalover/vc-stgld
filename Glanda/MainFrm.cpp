// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Glanda.h"
#include "MainFrm.h"
#include "Global.h"

#include "GlandaDoc.h"
#include "GlandaView.h"
#include "PaneBar.h"

#include "my_app.h"
#include "DrawHelper.h"

#include "gldDataKeeper.h"
#include "gldMovieClip.h"
#include "gldCharacter.h"
#include "TransAdaptor.h"

#include "DlgUndoRedo.h"
#include "DlgColorSwatch.h"

#include "DlgWebBrowser.h"

#include "ColorPicker.h"

#include "Help.h"
#include "filepath.h"

#include "DesignWnd.h"
#include "Observer.h"

#include "Options.h"
#include "OptionsSheet.h"

#include "DockBarEx.h"
#include "MiniDockFrameWndEx.h"

#include "SceneListBar.h"
#include "SceneListCtrl.h"

#include "..\updater\updater.h"
#include "EffectBar.h"
#include "EffectWnd.h"

#include "CategoryBar.h"
#include "CategoryWnd.h"
#include "OutputBar.h"

#include "ProgressStatusBar.h"
#include "Splash.h"

#include "DlgPreview.h"

#include "SysParas.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ACTIVATEAPP()
	ON_WM_ACTIVATE()
	ON_WM_NCPAINT()
	ON_COMMAND(ID_PREVIEW_IN_DEFAULT_BROWSER, OnPreviewInDefaultBrowser)
	ON_WM_SYSCOMMAND()

	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipText)
	ON_COMMAND(ID_PREVIEW_EDIT_BROWSER_LIST, OnPreviewEditBrowserList)
	ON_UPDATE_COMMAND_UI(ID_PREVIEW_BROWSER1, OnUpdatePreviewBrowser1)
	ON_COMMAND_RANGE(ID_PREVIEW_BROWSER1, ID_PREVIEW_BROWSER99, OnPreviewBrowsersRange)

	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_HELPTOPIC, OnHelpHelptopic)
	ON_COMMAND(ID_HELP_CHECKFORUPDATES, OnHelpCheckforupdates)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_HELP_TUTORIAL, OnHelpTutorial)
	ON_COMMAND(ID_HELP_VIEW_SAMPLES, OnHelpViewSamples)
	ON_COMMAND(ID_VIEW_TIMELINE, OnViewTimeline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TIMELINE, OnUpdateViewTimeline)

	ON_COMMAND(ID_VIEW_RESET_PANE_LOCATIONS, OnViewResetPaneLocations)

	ON_COMMAND(ID_VIEW_CUSTOMIZETOOLBAR, OnViewCustomizetoolbar)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateViewOutput)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SWITCH_WINDOW, OnSwitchWindow)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)

	ON_COMMAND(ID_VIEW_RESOURCE, OnViewResource)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESOURCE, OnUpdateViewResource)
	ON_COMMAND(ID_VIEW_FILES, OnViewFiles)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILES, OnUpdateViewFiles)
	ON_COMMAND(ID_VIEW_TOOLS, OnViewTools)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS, OnUpdateViewTools)

	ON_COMMAND(ID_VIEW_SCENES, OnViewScenes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCENES, OnUpdateViewScenes)

	ON_COMMAND(ID_SCENE_ADD, OnAddScene)
	ON_COMMAND(ID_EFFECT_ADD, OnAddEffect)
	ON_UPDATE_COMMAND_UI(ID_EFFECT_ADD, OnUpdateAddEffect)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame() : 
	m_hWndFocused(NULL), 
	m_hAccelUndo(NULL)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	for (STRING_LIST::iterator it = m_aPreviweFiles.begin(); it != m_aPreviweFiles.end(); it++)
	{
		CString strFile = *it;
		if (::PathFileExists(strFile))
			::DeleteFile(strFile);
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CBRS_SIZE_DYNAMIC will cause flicker when using non-docking tool bar
	DWORD dwStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
	if (!m_wndToolBar.CreateEx(this, dwStyle, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		/*| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_GRIPPER | CBRS_SIZE_DYNAMIC*/, CRect(2, 0, 2, 0)) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}


	m_wndToolBar.CreateZoomComboBox();

#ifdef _DEBUG
	HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), "c:\\bitmap1.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hBitmap)
	{
		CBitmap bmp;
		bmp.Attach(hBitmap);
		m_wndToolBar.LoadImage(&bmp, 16, 16);
	}
	else
	{
		m_wndToolBar.LoadImage(IDB_TOOLBAR_TRUECOLOR, 16, 16);
	}
#else
	m_wndToolBar.LoadImage(IDB_TOOLBAR_TRUECOLOR, 16, 16);
#endif

	
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM);

	m_wndToolBar.SetDropDownButton(ID_FILE_NEW, IDR_POPUP_NEW);
	m_wndToolBar.SetDropDownButton(ID_FILE_OPEN, IDR_POPUP_MRU);
	m_wndToolBar.SetDropDownButton(ID_EDIT_UNDO, 0);
	m_wndToolBar.SetDropDownButton(ID_EDIT_REDO, 0);
	m_wndToolBar.SetDropDownButton(ID_PREVIEW_IN_DEFAULT_BROWSER, IDR_POPUP_PREVIEW);
	m_wndToolBar.SetDropDownButton(ID_FILE_EXPORT_MOVIE, IDR_POPUP_EXPORT_MOVIE);

	m_wndToolBar.SetButtonText(ID_FILE_IMPORT, 0);
	m_wndToolBar.SetButtonText(ID_FILE_EXPORT_MOVIE, 0);
	m_wndToolBar.SetButtonText(ID_FILE_PUBLISH, 0);

	m_wndToolBar.Init("Main ToolBar\\Customize", "Standard");

	// seems all tool bar share a single tool tip control
	CToolTipCtrl *pToolTip = m_wndToolBar.GetToolBarCtrl().GetToolTips();
	if (pToolTip)
	{
		pToolTip->ModifyStyle(0, TTS_ALWAYSTIP);
		//pToolTip->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if (!CPaneBar::Instance()->Create(this, "", AFX_IDW_CONTROLBAR_FIRST + 40, WS_CHILD | WS_VISIBLE))
	{
		TRACE0("Failed to create panebar\n");
		return -1;
	}
	CPaneBar::Instance()->SetBarStyle(CPaneBar::Instance()->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_BORDER_3D | CBRS_ALIGN_TOP);

	if (!CProgressStatusBar::Instance()->Create(this) ||
		!CProgressStatusBar::Instance()->SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	CProgressStatusBar::Instance()->SetPaneStyle(0, SBPS_STRETCH); 


	// EnbaleDocking() must be called AFTER fixed bars were created.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableDocking(CBRS_ALIGN_RIGHT);
	EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_BOTTOM);

	GetControlBar(AFX_IDW_DOCKBAR_LEFT)->SetBarStyle(GetControlBar(AFX_IDW_DOCKBAR_LEFT)->GetBarStyle() & ~CBRS_BORDER_ANY);
	GetControlBar(AFX_IDW_DOCKBAR_TOP)->SetBarStyle(GetControlBar(AFX_IDW_DOCKBAR_TOP)->GetBarStyle() & ~CBRS_BORDER_ANY);
	GetControlBar(AFX_IDW_DOCKBAR_BOTTOM)->SetBarStyle(GetControlBar(AFX_IDW_DOCKBAR_BOTTOM)->GetBarStyle() & ~CBRS_BORDER_ANY);
	GetControlBar(AFX_IDW_DOCKBAR_RIGHT)->SetBarStyle(GetControlBar(AFX_IDW_DOCKBAR_RIGHT)->GetBarStyle() & ~CBRS_BORDER_ANY);

	// Scene list bar
	CSceneListBar::Instance()->Create("Scenes", this, CSize(110, 200), FALSE, AFX_IDW_CONTROLBAR_FIRST + 41);
	CSceneListBar::Instance()->SetBarStyle(CSceneListBar::Instance()->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	CSceneListBar::Instance()->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    CSceneListBar::Instance()->SetOwner(this);
	DockControlBar(CSceneListBar::Instance(), AFX_IDW_DOCKBAR_LEFT);

	// Create time line
	CEffectBar::Instance()->Create("TimeLine", this, CSize(400, 160), FALSE, AFX_IDW_CONTROLBAR_FIRST + 42);
	CEffectBar::Instance()->SetBarStyle(CEffectBar::Instance()->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	CEffectBar::Instance()->EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	CEffectBar::Instance()->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	DockControlBar(CEffectBar::Instance(), AFX_IDW_DOCKBAR_TOP);

	// Category bar
	CCategoryBar::Instance()->Create("Category", this, CSize(240,600), FALSE, AFX_IDW_CONTROLBAR_FIRST + 43);
	CCategoryBar::Instance()->SetBarStyle(CCategoryBar::Instance()->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	CCategoryBar::Instance()->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	CCategoryBar::Instance()->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	DockControlBar(CCategoryBar::Instance(), AFX_IDW_DOCKBAR_RIGHT);

	// Output bar
	COutputBar::Instance()->Create("Output", this, CSize(400, 160), FALSE, AFX_IDW_CONTROLBAR_FIRST + 44);
	COutputBar::Instance()->SetBarStyle(COutputBar::Instance()->GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	COutputBar::Instance()->EnableDocking(CBRS_ALIGN_ANY);
	COutputBar::Instance()->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	DockControlBar(COutputBar::Instance(), AFX_IDW_DOCKBAR_BOTTOM);
	ShowControlBar(COutputBar::Instance(), FALSE, FALSE);

	if (!CDlgUndo::Instance()->Create(IDD_UNDO, this))
		return - 1;

	if (!CDlgColorSwatch::Instance()->Create(IDD_COLOR_SWATCH, GetDesktopWindow()))
		return -1;
	CColorPicker::LoadUserColor();

	VERIFY(m_hAccelUndo = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_UNDO)));

	CSizingControlBar::GlobalSaveState("ToolBars\\DockBar-Default");
	SaveBarState("ToolBars\\DockBar-Default");

	if (!LoadBarStateEx("ToolBars\\DockBar"))
		OnViewResetPaneLocations();

	CSizingControlBar::GlobalLoadState("ToolBars\\DockBar");

	// do not display Register and Check for Updates menu
#if defined ELEMENT5
	CMenu *pMenu = GetMenu();
	UINT nCount = pMenu->GetMenuItemCount();
	CMenu *pHelpMenu = pMenu->GetSubMenu(nCount - 1);
	ASSERT(pHelpMenu);
	pHelpMenu->DeleteMenu(ID_HELP_REGISTER, MF_BYCOMMAND);
	pHelpMenu->DeleteMenu(ID_HELP_CHECKFORUPDATES, MF_BYCOMMAND);
	::DeleteRedundantSeparator(pHelpMenu);
#endif

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
	CCreateContext* pContext)
{
	if (!CFrameWnd::OnCreateClient(lpcs, pContext))
		return FALSE;

	CSplashWnd::Instance()->ShowSplashScreen(this);

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	static TCHAR strClassName[] = "SothinkFrameWnd";
	WNDCLASS wndclass;
	wndclass.style = CS_DBLCLKS;
	wndclass.lpfnWndProc = ::DefWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = AfxGetInstanceHandle();
	wndclass.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = strClassName;

	if (AfxRegisterClass(&wndclass))
		cs.lpszClass = strClassName;

	cs.style |= WS_CLIPCHILDREN;
	cs.style |= WS_POPUP;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	CWinApp *pApp = AfxGetApp();

	CString strTemp;
	strTemp.Format("Windows\\%s", "Main");

	UINT showCmd = pApp->GetProfileInt(strTemp, "showCmd", SW_SHOWNORMAL);

	int cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);

	CRect rc;

	rc.left = pApp->GetProfileInt(strTemp, "Left", 50);
	rc.top = pApp->GetProfileInt(strTemp, "Top", 50);
	rc.right = pApp->GetProfileInt(strTemp, "Right", cx - 100);
	rc.bottom = pApp->GetProfileInt(strTemp, "Bottom", cy - 100);

	::EnsureWholeRectVisible(rc);

	cs.x = rc.left;
	cs.y = rc.top;
	cs.cx = rc.Width();
	cs.cy = rc.Height();

	if (showCmd == SW_MAXIMIZE)
		cs.style |= WS_MAXIMIZE;
	else
		showCmd = SW_SHOWNORMAL;

	// avoid flicker
	pApp->m_nCmdShow = showCmd;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


//void CMainFrame::PostNcDestroy()
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	CFrameWnd::PostNcDestroy();
//
//	PostQuitMessage(0);
//}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	if (!IsIconic())
		::SaveWindowPlacement(this, "Main");

	CFrameWnd::OnClose();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{

	// TODO: Add your message handler code here

	if (m_hWndFocused == NULL)
	{
		if (nState == WA_INACTIVE)
		{
			m_hWndFocused = ::GetFocus();
		}
	}

	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
	
	if (!IsIconic())
	{
		if (nState != WA_INACTIVE)
		{
			if (m_hWndFocused)
			{
				if (::IsChild(m_hWnd, m_hWndFocused))
					::SetFocus(m_hWndFocused);

				m_hWndFocused = NULL;
			}
		}
	}
}

void CMainFrame::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CFrameWnd::OnNcPaint() for painting messages

	CFrameWnd::OnNcPaint();

/*
	CRect rc;
	GetWindowRect(&rc);
	rc.OffsetRect(-rc.left, -rc.top);

	int cxBorder = ::GetSystemMetrics(SM_CXFRAME);
	int cyBorder = ::GetSystemMetrics(SM_CYFRAME);
	int cyCaption = ::GetSystemMetrics(SM_CYCAPTION);
	int cyMenu = ::GetSystemMetrics(SM_CYMENU);

	rc.InflateRect(-cxBorder, -cyBorder);
	rc.top += cyCaption;
	rc.bottom = rc.top + cyMenu;

	CWindowDC dc(this);
	dc.DrawEdge(&rc, EDGE_ETCHED, BF_RECT);
*/
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	HWND hWndFocus = ::GetFocus();

	if (hWndFocus == CSceneListCtrl::Instance()->m_hWnd)
		if (CSceneListCtrl::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;

	if (hWndFocus == CEffectWnd::Instance()->m_hWnd)
		if (CEffectWnd::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;

	if (hWndFocus == CDesignWnd::Instance()->m_hWnd)
		if (CDesignWnd::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;

	if (CMruFileManager::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnViewTimeline()
{
	// TODO: Add your command handler code here

	ShowControlBar(CEffectBar::Instance(), !CEffectBar::Instance()->IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewTimeline(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(CEffectBar::Instance()->IsWindowVisible() ? 1 : 0);
}

void CMainFrame::OnPreviewInDefaultBrowser()
{
	// TODO: Add your command handler code her

	ExternalPreview(NULL);
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	if (nID == SC_MINIMIZE)
	{
		if (m_hWndFocused == NULL)
			m_hWndFocused = ::GetFocus();

		// if the main windows is going to be minimized, all the floaters
		// will be hide first (os' default behave?). so we must save the 
		// windows' status here
		::SaveWindowPlacement(this, "Main");
	}

	CFrameWnd::OnSysCommand(nID, lParam);
}

BOOL CMainFrame::LoadBarStateEx(LPCTSTR lpszProfileName)
{
	// Note: LocadBarState will generate application error if CControlBar's ID has been changed
	CDockState state;
	state.LoadState(lpszProfileName);

	// Verify dock state. Written by Cristi Posea (the author of CSIzingCOntrolBar).
	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		int nDockedCount = pInfo->m_arrBarID.GetSize();
		if (nDockedCount > 0)
		{
			// dockbar
			for (int j = 0; j < nDockedCount; j++)
			{
				UINT nID = (UINT) pInfo->m_arrBarID[j];
				if (nID == 0) continue; // row separator
				if (nID > 0xFFFF)
					nID &= 0xFFFF; // placeholder - get the ID
				if (GetControlBar(nID) == NULL)
					return FALSE;
			}
		}
	    
		if (!pInfo->m_bFloating) // floating dockbars can be created later
			if (GetControlBar(pInfo->m_nBarID) == NULL)
				return FALSE; // invalid bar ID
	}

	LoadBarState(lpszProfileName);

	return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		BOOL bTanslateUndoAccel = TRUE;

		HWND hWnd = ::GetFocus();
		if (hWnd)
		{
			static TCHAR szClass[256];
			if (::GetClassName(hWnd, szClass, 256) > 0)
			{
				if (_tcsicmp(szClass, _T("EDIT")) == 0)
					if (::SendMessage(hWnd, EM_CANUNDO, 0, 0) || ::SendMessage(hWnd, EM_CANREDO, 0, 0))
						bTanslateUndoAccel = FALSE;
			}
		}

		if (bTanslateUndoAccel)
			if (m_hAccelUndo && ::TranslateAccelerator(m_hWnd, m_hAccelUndo, pMsg))
				return TRUE;
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnToolTipText(UINT nID, NMHDR * pNMHDR, LRESULT * pResult)
{
	*pResult = 0;

	CString strTipText;
	if (GetToolTipText((int)pNMHDR->idFrom, strTipText))
	{
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
			//if (pTTTA->uFlags & TTF_IDISHWND)
 				_tcsncpy(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else if (pNMHDR->code == TTN_NEEDTEXTW)
		{
			TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
			//if (pTTTW->uFlags & TTF_IDISHWND)
				_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
		return TRUE;
	}
	return CFrameWnd::OnToolTipText(nID, pNMHDR, pResult);
}

BOOL CMainFrame::GetToolTipText(int nID, CString &strTip)
{
	if ((nID == ID_EDIT_UNDO && my_app.Commands().CanUndo()) || (nID == ID_EDIT_REDO && my_app.Commands().CanRedo()))
	{
		UINT nIDDesc;

		TCommands::CmdIterator it = my_app.Commands().Last();
		if (nID == ID_EDIT_UNDO)
			nIDDesc = my_app.Commands().GetCommandDescID(it);
		else
			nIDDesc = my_app.Commands().GetCommandDescID(++it);
		
		CString strDesc;
		if (GetCommandDesc(nIDDesc, strDesc))
		{
			if (nID == ID_EDIT_UNDO)
				strTip.Format(IDS_UNDO_ACTION1, (LPCTSTR)strDesc);
			else
				strTip.Format(IDS_REDO_ACTION1, (LPCTSTR)strDesc);

			return TRUE;
		}
	}
	return FALSE;
}

CMainToolBar &CMainFrame::GetToolBar()
{
	return m_wndToolBar;
}

void CMainFrame::ExternalPreview(LPCTSTR lpszBrowser)
{
	CString strSWFFile = GetTempFile(COptions::Instance()->GetPreviewPath(), "~preview", "swf");
	CString strHTMFile;
	if (ExportMovie(AfxGetMainWnd(), strSWFFile, FALSE, FALSE))
	{
		m_aPreviweFiles.push_back(strSWFFile);
		if (lpszBrowser != NULL)
		{
			strHTMFile= GetTempFile(COptions::Instance()->GetPreviewPath(), "~preview", "htm");
			const char szHTMTemplate[] = 
				"<html>\r\n"
				"<head>\r\n"
				"<title>Preview</title>\r\n"
				"</head>\r\n"
				"<body bgcolor=\"#e0e0e0\">\r\n"
				"<object classid=\"clsid:d27cdb6e-ae6d-11cf-96b8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" width=\"%d\" height=\"%d\" id=\"Untitled.swf\" align=\"middle\">\r\n"
				"	<param name=\"allowScriptAccess\" value=\"sameDomain\">\r\n"
				"	<param name=\"movie\" value=\"%s\">\r\n"
				"	<param name=\"quality\" value=\"high\">\r\n"
				"	<param name=\"bgcolor\" value=\"#ffffff\">\r\n"
				"	<embed src=\"%s\" quality=\"high\" bgcolor=\"#ffffff\" width=\"%d\" height=\"%d\" name=\"%s\" align=\"middle\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\">\r\n"
				"</object>\r\n"
				"</body>\r\n"
				"</html>";
			CString strHTMContent;
			CString strSWFFileName = ::FileStripPath(strSWFFile);
			gldMainMovie2 *pMovie = _GetMainMovie2();
			strHTMContent.Format(szHTMTemplate, pMovie->m_width, pMovie->m_height, (LPCTSTR)strSWFFileName, (LPCTSTR)strSWFFileName, pMovie->m_width, pMovie->m_height, (LPCTSTR)strSWFFileName);

			try
			{
				CStdioFile file(strHTMFile, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
				file.Write(strHTMContent, strHTMContent.GetLength());
				file.Flush();
				file.Close();
			}
			catch (CFileException *e)
			{
				e->ReportError();
				e->Delete();
				return;
			}
			m_aPreviweFiles.push_back(strHTMFile);
			::ShellExecute(NULL, "open", lpszBrowser, CString(_T("\"")) + strHTMFile + CString(_T("\"")), NULL, SW_SHOWNORMAL);
		}
		else
		{
			long lResult = (long)::ShellExecute(NULL, "open", strSWFFile, NULL, NULL, SW_SHOWNORMAL);
			if (lResult <= 32)
			{
				if (lResult == SE_ERR_NOASSOC)
				{
					if (AfxMessageBox(IDS_PROMPT_NO_DEFAULT_PLAYER, MB_ICONQUESTION | MB_YESNO) != IDYES)
						return;
					ShellExecute(m_hWnd, "open", "rundll32.exe", "shell32.dll,OpenAs_RunDLL " + strSWFFile, NULL, SW_SHOWNORMAL);
				}
				else
				{
					AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_SHELLEXECUTE2, strSWFFile, GetLastErrorMessage());
				}
			}
		}
	}
}

void CMainFrame::OnPreviewEditBrowserList()
{
	// TODO: Add your command handler code here

	CDlgWebBrowser dlg;
	dlg.DoModal();
}

void CMainFrame::OnUpdatePreviewBrowser1(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!pCmdUI->m_pMenu)
		return;

	CBrowsers *pBrowsers = CBrowsers::Instance();
	UINT i = ID_PREVIEW_BROWSER1;
	for (; i <= ID_PREVIEW_BROWSER99; i++)
		if (!pCmdUI->m_pMenu->DeleteMenu(i, MF_BYCOMMAND))
			break;

	UINT count1 = pBrowsers->GetBrowserNum();
	UINT count2 = pBrowsers->GetSpecifyBrowserNum();

	for (i = 0; i < count1; i++)
		pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex + i, MF_STRING | MF_BYPOSITION, ID_PREVIEW_BROWSER1 + i, pBrowsers->GetBrowserName(i));

    if (count2 > 0)
	{
		//pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex + (i + 2), MF_SEPARATOR | MF_BYPOSITION);
		//i++;
		for (UINT j = 0; j < count2; i++, j++)
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex + i, MF_STRING | MF_BYPOSITION, ID_PREVIEW_BROWSER1 + i, pBrowsers->GetSpecifyBrowserName(j));
	}

	pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
}

void CMainFrame::OnPreviewBrowsersRange(UINT nID)
{
	int nIndex = nID - ID_PREVIEW_BROWSER1;
	ASSERT(nIndex >= 0 );

	CBrowsers *pBrowsers = CBrowsers::Instance();

	UINT nBrowserNum = pBrowsers->GetBrowserNum();
	CString strBrowser;
	if (nIndex < (int)nBrowserNum)
		strBrowser = pBrowsers->GetBrowserExeFile(nIndex);
	else
		strBrowser = pBrowsers->GetSpecifyBrowserExeFile(nIndex-nBrowserNum);

	ExternalPreview(strBrowser);
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: Add your specialized code here and/or call the base class

	CDocument *pDoc = _GetCurDocument();
	if (pDoc)
	{
		CString strTitle = pDoc->GetPathName();
		if (strTitle.IsEmpty())
			strTitle = pDoc->GetTitle();

		if (pDoc->IsModified())
			strTitle += "*";

		CString strFullTitle;
		strFullTitle.Format("%s - [%s]", (LPCTSTR)m_strTitle, (LPCTSTR)strTitle);
		AfxSetWindowText(m_hWnd, strFullTitle);
	}

	//CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}

class CDialogHelpDummy : public CDialog
{
public:
	UINT GetHelpID()
	{
		return m_nIDHelp;
	}
};

void CMainFrame::OnHelp()
{
	HWND hWnd = ::GetFocus();
	while (hWnd)
	{
		TCHAR szClass[256];
		::GetClassName(hWnd, szClass, 256);
		if (_tcscmp(szClass, "#32770") == 0)
			break;
		hWnd = ::GetParent(hWnd);
	}

	if (hWnd)
	{
		// dirty cast, but safe.
		CWnd *pWnd = (CDialogHelpDummy *)CWnd::FromHandlePermanent(hWnd);
		if (pWnd)
		{
			UINT nID = 0;
			if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
			{
				CDialogHelpDummy *pDlg = (CDialogHelpDummy *)pWnd;
				nID = pDlg->GetHelpID();
			}
			else if(pWnd->IsKindOf(RUNTIME_CLASS(CPropertySheet)))
			{
				CPropertySheet *pSheet = (CPropertySheet *)pWnd;
				CPropertyPage *pPage = pSheet->GetActivePage();
				if (pPage)
				{
					// cause CPropertyPage is derived from CDialog
					CDialogHelpDummy *pDlg = (CDialogHelpDummy *)pPage;
					nID = pDlg->GetHelpID();
				}
			}

			ShowHelpFile(GetHelpFileOfID(nID));
		}
	}
	else
	{
		hWnd = ::GetFocus();
		if (hWnd)
		{
			CWnd *pWnd = CWnd::FromHandlePermanent(hWnd);
			if (pWnd == CSceneListCtrl::Instance())
			{
				ShowHelpFile("interface/scene.htm");
				return;
			}
			else if (pWnd == CEffectWnd::Instance())
			{
				ShowHelpFile("interface/timeline.htm");
				return;
			}
			else if (pWnd == CDesignWnd::Instance())
			{
				ShowHelpFile("interface/design.htm");
				return;
			}
		}

		ShowHelpFile(GetHelpFileOfID(0));
	}
}

void CMainFrame::OnHelpHelptopic()
{
	// TODO: Add your command handler code here

	ShowHelpFile(GetHelpFileOfID(0));
}

void CMainFrame::OnHelpCheckforupdates()
{
	// TODO: Add your command handler code here

	RunUpdater(g_szAppName);
}

void CMainFrame::OnToolsOptions()
{
	// TODO: Add your command handler code here

	COptionsSheet dlg(IDS_OPTIONS, this);
	if (dlg.DoModal() == IDOK)
	{
		RedrawWindow(0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
		CEffectWnd::Instance()->SetShowAllInstances(COptions::Instance()->m_bShowAllElementsInTimeline);
	}
}

void CMainFrame::OnHelpTutorial()
{
	// TODO: Add your command handler code here

	ShowHelpFile("tutorial/overview.htm");
}

void CMainFrame::OnHelpViewSamples()
{
	// TODO: Add your command handler code here

	CString strURL;
	strURL.LoadString(IDS_URL_SAMPLES);
	ShellExecute(NULL, "open", strURL, NULL, NULL, SW_SHOWNORMAL);	
}

void CMainFrame::OnViewResetPaneLocations()
{
	// TODO: Add your command handler code here

	//SetRedraw(FALSE);

	CSizingControlBar::GlobalLoadState("ToolBars\\DockBar-Default");
	LoadBarStateEx("ToolBars\\DockBar-Default");

	//FloatControlBar(CSceneListBar::Instance(), CPoint(32767, 32767));
	//ShowControlBar(CSceneListBar::Instance(), TRUE, TRUE);
	//DockControlBar(CSceneListBar::Instance(), AFX_IDW_DOCKBAR_LEFT);

	//FloatControlBar(CEffectBar::Instance(), CPoint(32767, 32767));
	//ShowControlBar(CEffectBar::Instance(), TRUE, TRUE);
	//DockControlBar(CEffectBar::Instance(), AFX_IDW_DOCKBAR_TOP);

	RecalcLayout(TRUE);

	// GetActiveView() will return NULL if this function is 
	// called by CMainFrame::OnCreate()
	CView *pView = GetActiveView();
	if (pView)
		pView->SetFocus();

	//SetRedraw(TRUE);
}

BOOL CMainFrame::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CString strKey;
	strKey.Format("Software\\%s\\%s\\ToolBars", AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszProfileName);
	AfxGetApp()->DelRegTree(HKEY_CURRENT_USER, strKey);

	CSizingControlBar::GlobalSaveState("ToolBars\\DockBar");
	SaveBarState("ToolBars\\DockBar");


	CDlgPreview *pDlg = CDlgPreview::Instance();
	if (::IsWindow(pDlg->m_hWnd))
		pDlg->DestroyWindow();
	// must call this after DestroyWindow. because SaveWindowPos is called in 
	// CDlgPreview::OnDestroyWindow().
	pDlg->ResetWindowPos();

	CDlgPreview::ReleaseInstance();

	CProgressStatusBar::ReleaseInstance();

	COutputBar::Instance()->DestroyWindow();
	COutputBar::ReleaseInstance();

	CCategoryBar::Instance()->DestroyWindow();
	CCategoryBar::ReleaseInstance();

	CEffectBar::Instance()->DestroyWindow();
	CEffectBar::ReleaseInstance();

	CSceneListBar::Instance()->DestroyWindow();
	CSceneListBar::ReleaseInstance();

	CDesignWnd::ReleaseInstance();

	CPaneBar::ReleaseInstance();

	CDlgUndo::ReleaseInstance();

	CBrowsers::ReleaseInstance();

	//_CrtDumpMemoryLeaks();
		
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnViewCustomizetoolbar()
{
	// TODO: Add your command handler code here

	m_wndToolBar.GetToolBarCtrl().Customize();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	CFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	//CFrameWnd::OnSetFocus(pOldWnd);
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
}

void CMainFrame::OnSwitchWindow()
{
	// TODO: Add your command handler code here

	CWnd *pWnd = GetFocus();
	if (pWnd == CDesignWnd::Instance())
		CEffectWnd::Instance()->SetFocus();
	else
		CDesignWnd::Instance()->SetFocus();
}

void CMainFrame::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	m_pFloatingFrameClass = RUNTIME_CLASS(CMiniDockFrameWndEx);
	for (int i = 0; i < 4; i++)
	{
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CDockBar* pDock = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
			if (pDock == NULL)
			{
				pDock = new CDockBarEx;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwDockBarMap[i][1], dwDockBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
			}
		}
	}
}

void CMainFrame::OnViewZoomin()
{
	// TODO: Add your command handler code here

	int zoom = my_app.GetZoomPercent();
	my_app.ZoomTo(zoom <= 50 ? zoom * 2 : zoom < 100 ? 100 : zoom + 50);
}

void CMainFrame::OnViewZoomout()
{
	// TODO: Add your command handler code here

	int zoom = my_app.GetZoomPercent();
	my_app.ZoomTo(zoom <= 50 ? zoom / 2 : zoom > 100 && zoom < 150 ? 100 : zoom - 50);
}

void CMainFrame::OnViewOutput()
{
	ShowControlBar(COutputBar::Instance(), !COutputBar::Instance()->IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewOutput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(COutputBar::Instance()->IsWindowVisible() ? 1 : 0);
}

void CMainFrame::OnViewResource()
{
	// TODO: Add your command handler code here
	
	ShowControlBar(CCategoryBar::Instance(), TRUE, FALSE);
	CCategoryWnd::Instance()->SetActivePage(INDEX_RESOURCE_PAGE);
}

void CMainFrame::OnUpdateViewResource(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetRadio(CCategoryBar::Instance()->IsWindowVisible() && CCategoryWnd::Instance()->GetActivePage() == INDEX_RESOURCE_PAGE);
}

void CMainFrame::OnViewFiles()
{
	// TODO: Add your command handler code here

	ShowControlBar(CCategoryBar::Instance(), TRUE, FALSE);
	CCategoryWnd::Instance()->SetActivePage(INDEX_FILES_PAGE);
}

void CMainFrame::OnUpdateViewFiles(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetRadio(CCategoryBar::Instance()->IsWindowVisible() && CCategoryWnd::Instance()->GetActivePage() == INDEX_FILES_PAGE);
}

void CMainFrame::OnViewTools()
{
	// TODO: Add your command handler code here

	ShowControlBar(CCategoryBar::Instance(), TRUE, FALSE);
	CCategoryWnd::Instance()->SetActivePage(INDEX_TOOLS_PAGE);
}

void CMainFrame::OnUpdateViewTools(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetRadio(CCategoryBar::Instance()->IsWindowVisible() && CCategoryWnd::Instance()->GetActivePage() == INDEX_TOOLS_PAGE);
}

void CMainFrame::OnViewScenes()
{
	ShowControlBar(CSceneListBar::Instance(), !CSceneListBar::Instance()->IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewScenes(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(CSceneListBar::Instance()->IsWindowVisible() ? 1 : 0);
}

void CMainFrame::OnAddScene()
{
	CSceneListCtrl::Instance()->OnSceneAdd();
}

void CMainFrame::OnAddEffect()
{
	CEffectBar::Instance()->m_dlgEffect.OnSelectEffect();
}

void CMainFrame::OnUpdateAddEffect(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 0);
}

void CMainFrame::OnSysColorChange()
{
	SysParas.Refresh();

	CFrameWnd::OnSysColorChange();	
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	SysParas.Refresh();

	CFrameWnd::OnSettingChange(uFlags, lpszSection);	
}
