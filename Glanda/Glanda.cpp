// GMovie.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Glanda.h"

#include "Global.h"

#include "gldDataKeeper.h"

#include "MainFrm.h"
#include "GlandaDoc.h"
#include "GlandaView.h"
#include "DlgAbout.h"

#include "DlgColorSwatch.h"
#include "BalloonToolTip.h"

#include "my_app.h"

#include "Options.h"
#include "Clipboard.h"


#include "InputBox.h"
#include "testImportDlg.h"

#include "CodeTip.h"
#include "TipWnd.h"
#include "MemberListBox.h"
#include "VersionInfo.h"

#include "filepath.h"

#include "IniFile.h"

#include "CheckMessageBox.h"

#include "armadillo.h"

#include "..\updater\updater.h"
#include ".\Glanda.h"
#include <initguid.h>

#include "Glanda_i.h"
#include "Glanda_i.c"
#include "GlandaClipboard.h"

#include "gldEffectManager.h"
#include "SWFImportCache.h"

#include "VisualStylesXP.h"
#include "NewMenu.h"
#include "EffectInplaceEditor.h"
#include "CategoryWnd.h"
#include "Splash.h"
#include "DlgAboutLame.h"

#include <GdiPlus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CGlandaModule :
	public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_GlandaLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_GLANDA, "{5FCF967D-A297-43D8-A10A-944D62989103}");};

CGlandaModule _AtlModule;


#pragma comment(lib, "delayimp")

// CGlandaDocManager
class CGlandaDocManager : public CDocManager
{
public:
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
	{
		return CDocManager::DoPromptFileName(fileName, nIDSTitle, lFlags | OFN_ENABLESIZING, bOpenFileDialog, pTemplate);
	}
};


// CGlandaApp

BEGIN_MESSAGE_MAP(CGlandaApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_DEBUG_TESTIMPORT, OnDebugTestimport)
	//ON_COMMAND(ID_HELP_REGISTER, OnHelpRegister)
	//ON_UPDATE_COMMAND_UI(ID_HELP_REGISTER, OnUpdateHelpRegister)
	ON_COMMAND(ID_HELP_ABOUT_LAME, OnHelpAboutLame)
END_MESSAGE_MAP()


BOOL CALLBACK IsMyFile(LPCTSTR lpszFile)
{
	return TRUE;
}

// CGlandaApp construction

CGlandaApp::CGlandaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CGlandaApp object

CGlandaApp theApp;

char *ext_res_layerName = NULL;                 // "Layer"
char *ext_res_maskLayerName = NULL;             // "Mask Layer"
char *ext_res_maskedLayerName = NULL;           // "Masked Layer"
char *ext_res_dummyLayerName = NULL;            // "Dummy Layer"
char *ext_res_actionLayerName = NULL;           // "Action Layer"
char *ext_res_streamSoundLayerName = NULL;      // "Stream Sound Layer"
char *ext_res_eventSoundLayerName = NULL;       // "Event Sound Layer"
char *ext_res_labelLayerName = NULL;            // "Label Layer"

char *ext_res_symbolName;						// "Symbol"
char *ext_res_effectName;						// "Effect"

void LoadStringHelper(char **ppszBuf, UINT nID)
{
	CString str;
	VERIFY(str.LoadString(nID) > 0);
	ASSERT(*ppszBuf == NULL);
	*ppszBuf = new TCHAR[str.GetLength() + 1];
	_tcscpy(*ppszBuf, str);
}

void LoadCSWFLibStrings()
{
	LoadStringHelper(&ext_res_layerName, IDS_CSWFLIB_LAYER);
	LoadStringHelper(&ext_res_maskLayerName, IDS_CSWFLIB_MASK_LAYER);
	LoadStringHelper(&ext_res_maskedLayerName, IDS_CSWFLIB_MASKED_LAYER);
	LoadStringHelper(&ext_res_dummyLayerName, IDS_CSWFLIB_DUMMY_LAYER);
	LoadStringHelper(&ext_res_actionLayerName, IDS_CSWFLIB_ACTION_LAYER);
	LoadStringHelper(&ext_res_streamSoundLayerName, IDS_CSWFLIB_STREAM_SOUND_LAYER);
	LoadStringHelper(&ext_res_eventSoundLayerName, IDS_CSWFLIB_EVENT_SOUND_LAYER);
	LoadStringHelper(&ext_res_labelLayerName, IDS_CSWFLIB_LABEL_LAYER);

	LoadStringHelper(&ext_res_symbolName, IDS_SYMBOL);
	LoadStringHelper(&ext_res_effectName, IDS_SYMBOL_EFFECT);
}

void FreeCSWFLibStrings()
{
	delete ext_res_layerName;
	delete ext_res_maskLayerName;
	delete ext_res_maskedLayerName;
	delete ext_res_dummyLayerName;
	delete ext_res_actionLayerName;
	delete ext_res_streamSoundLayerName;
	delete ext_res_eventSoundLayerName;
	delete ext_res_labelLayerName;

	delete ext_res_symbolName;
	delete ext_res_effectName;
}

//#define BETA_VERSION

// CGlandaApp initialization

ULONG_PTR g_gdiplusToken = NULL;
HANDLE g_hMutex;

#ifdef _USB_VERSION_
BOOL RegDll(LPCTSTR lpszDllFileName)
{
	HMODULE hLibrary = LoadLibraryEx(lpszDllFileName,NULL,LOAD_WITH_ALTERED_SEARCH_PATH);
	if(hLibrary != NULL) 
	{
		FARPROC lpFunc = GetProcAddress(hLibrary,"DllRegisterServer");
		if(lpFunc != NULL)
			lpFunc();
		FreeLibrary(hLibrary);

		return TRUE;
	}

	return FALSE;
}
#endif

BOOL CGlandaApp::InitInstance()
{
	CWinApp::InitInstance();

//给韩国代理使用的版本，程序不安装，只放在U盘上
#ifdef _USB_VERSION_

	//自动注册一些COM组件
	CString strFilePath,strFileFullName;
	::GetModuleFileName(NULL,strFileFullName.GetBuffer(1000),1000);
	strFileFullName.ReleaseBuffer();

	strFilePath = strFileFullName.Left(strFileFullName.ReverseFind('\\')+1);

	WinExec(strFilePath+"regfix.exe",0);
	Sleep(200);

	//写注册表信息
	BOOL bQuickerInstalled = FALSE;
	HKEY hKey;

	//检测是否安装了Quicker
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\SourceTec\\Sothink SWF Quicker",0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		bQuickerInstalled = TRUE;
		RegCloseKey(hKey);
	}

	RegDll(strFilePath + "Templates\\Album\\AlbumWizard.dll");
	RegDll(strFilePath + "Templates\\Banner\\BannerWizard.dll");
	RegDll(strFilePath + "Templates\\Navigation\\NavigationWizard.dll");

	if(!bQuickerInstalled)	//公用组件，如果安装了Quicker就不能重复注册了。
	{
		RegDll(strFilePath + "Shared\\GlandaCOM.dll");
		RegDll(strFilePath + "Shared\\GeneralEffect.dll");
		RegDll(strFilePath + "Shared\\EffectPack1.dll");
		RegDll(strFilePath + "Shared\\MotionPath.dll");
		RegDll(strFilePath + "sys\\msxml4.dll");
	}

	DWORD dwFlag;
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\SourceTec\\Sothink Glanda",0,
		NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwFlag) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey,"Install Path",0,REG_SZ,(const BYTE*)LPCTSTR(strFilePath),strlen(strFilePath)+1);
		RegSetValueEx(hKey,"Binary Path",0,REG_SZ,(const BYTE*)LPCTSTR(strFileFullName),strlen(strFileFullName)+1);
		RegSetValueEx(hKey,"Version",0,REG_SZ,(const BYTE*)"4.1",4);
		RegSetValueEx(hKey,"Build",0,REG_SZ,(const BYTE*)"70209",6);
		RegSetValueEx(hKey,"Language",0,REG_SZ,(const BYTE*)"English",8);
		RegSetValueEx(hKey,"Product ID",0,REG_SZ,(const BYTE*)"28",3);

		RegCloseKey(hKey);
	}

#endif


	g_hMutex = CreateMutex(NULL, FALSE, "C8F4800F-52F4-4115-BE64-FF1C23604E86");

	// Init OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Init COM libraries
	CoInitialize(0);

	// Enable ActiveX support
	AfxEnableControlContainer();

	// Set root registry key
	SetRegistryKey(_T("SourceTec"));

	// Set default sub registry key.
	// If i do not do this, AFX_IDS_APP_TITLE will be used. but AfxMessageBox will use
	// AFX_IDS_APP_TITLE too and AFX_IDS_APP_TITLE should be localized..
	if (m_pszProfileName)
		free((void *)m_pszProfileName);
	m_pszProfileName = _tcsdup(g_szAppName);

	// Check if Flash Player ActiveX is installed.
	const CLSID clsid = { 0xd27cdb6e, 0xae6d, 0x11cf, { 0x96, 0xb8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };
	CComPtr<IUnknown> pIUnknown;
	HRESULT hr = pIUnknown.CoCreateInstance(clsid);
	if (FAILED(hr))
	{
		if (AfxMessageBox(IDS_INSTALL_FLASH_PLAYER, MB_ICONQUESTION | MB_YESNO) == IDYES)
			::ShellExecute(NULL, "open", "http://www.macromedia.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash", NULL, NULL, SW_SHOWNORMAL);
		return FALSE;
	}

	// ignore the error if failed to initialize the cache manager.
	CSWFImportCache::Instance()->Initialize();

	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();
	
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

	if (!Clipboard.Initialize())
		return FALSE;

#if !defined ELEMENT5
	RunUpdaterScheduled(g_szAppName);
#endif

#ifdef BETA_VERSION
	if (IsExpired() || IsClockBack())
	{
		if (AfxMessageBox(IDS_PRERELEASE_EXPIRED, MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, "open", "http://www.sothink.com/webtools/glanda/", NULL, NULL, SW_SHOWNORMAL);
		return FALSE;
	}

	if (AfxGetApp()->GetProfileInt("Tips", "Beta", TRUE))
	{
		BOOL bChecked = FALSE;
		CheckMessageBox(IDS_PROMPT_PRERELEASE, 
			IDS_NO_PROMPT_AGAIN, 
			bChecked, 
			MB_ICONINFORMATION | MB_OK);
		AfxGetApp()->WriteProfileInt("Tips", "Beta", !bChecked);
	}
#else
	#ifndef _DEBUG
		#ifndef _USB_VERSION_
			if (!CheckKeyState())
				return FALSE;
		#endif
	#endif
#endif

	_AFX_RICHEDIT_STATE* pState = _afxRichEditState;
	ASSERT(pState->m_hInstRichEdit == NULL);
	pState->m_hInstRichEdit = LoadLibraryA("RICHED20.DLL");
	if (!pState->m_hInstRichEdit)
		if (!AfxInitRichEdit())
			AfxMessageBox(IDS_ERROR_INIT_RICHEDIT20);

	// Init CSWFLib constants
	LoadCSWFLibStrings();

	srand((unsigned)time(NULL));


	// load all effects
	gldEffectManager::Instance();

    // Set Proc id
    gldDataKeeper::m_procId = GetCurrentProcessId();

	CSubjectManager::Instance()->Register("Select", new CSubject());
    CSubjectManager::Instance()->Register("Transform", new CSubject());	
	CSubjectManager::Instance()->Register("Zoom", new CSubject());
	
	CSubjectManager::Instance()->Register("ModifyDocument", new CSubject());

	CSubjectManager::Instance()->Register("TextInplaceEdit", new CSubject());
	CSubjectManager::Instance()->Register("TextSelChange", new CSubject());

	CSubjectManager::Instance()->Register("ChangeMovieProperties", new CSubject());

	CSubjectManager::Instance()->Register("ChangeMapMode", new CSubject());

	// Add for Glanda
	CSubjectManager::Instance()->Register("ChangeMainMovie2", new CSubject());

	// 注册切换场景时向CSceneListCtrl发送选择改变的事件
	CSubjectManager::Instance()->Register("ChangeCurrentScene2", new CSubject());

	// 注册特效时间修改的事件
	CSubjectManager::Instance()->Register("ChangeTimelineSel", new CSubject());
	CSubjectManager::Instance()->Register("ChangeEffect", new CSubject());
	CSubjectManager::Instance()->Register("ChangeSceneSound", new CSubject());

	CSubjectManager::Instance()->Register("DeleteFile", new CSubject());
	CSubjectManager::Instance()->Register("MoveFile", new CSubject());

	// Modify scene content
	CSubjectManager::Instance()->Register("ModifyCurrentScene2", new CSubject());

	CSubjectManager::Instance()->Register("RenameSymbol", new CSubject());
	CSubjectManager::Instance()->Register("RenameInstance", new CSubject());

	EffectInplaceEditor::Instance();

	CFileFind ff;
	BOOL ret = ff.FindFile(CString(GetModuleFilePath()) + "\\classes\\*.as", 0);
	while (ret)
	{
		ret = ff.FindNextFile();
		CClassInfo *pClass = new CClassInfo();
		if (pClass)
		{
			if (pClass->ParseFile(ff.GetFilePath()))
				g_ASParser.AddClass(pClass);
			else
				delete pClass;
		}
	}

	// initialize ini file
	//CIniFile::Instance();

	// init document/view
	m_pDocManager = new CGlandaDocManager();
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGlandaDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGlandaView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Set the default menu style
	if (g_xpStyle.IsAppThemed())
		CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_XP_2003_NOBORDER);
	else
		CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_ORIGINAL_NOBORDER);

	CNewMenu::SetXpBlending(FALSE);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Add recent file category for other files
	CMruFileManager::Instance()->Add(ID_MY_RECENT_FILES,	// base command ID
		_T("Recent Files"),									// szSection = registry key name 
		_T("File%d"),										// szFileEntry = registry value name
		IsMyFile,											// test for file
		10);												// max number list/menu entries

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Register class factories via CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}
	// App was launched with /Unregserver or /Unregister switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		return FALSE;
	}
	// App was launched with /Register or /Regserver switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
	{
		_AtlModule.UpdateRegistryAppId(TRUE);
		_AtlModule.RegisterServer(TRUE);
		return FALSE;
	}

	if (__argc == 1)
	{
		if ((GetKeyState(VK_SHIFT) >> (sizeof(SHORT) - 1) == 0))
		{
			if (COptions::Instance()->m_bReloadLastDocument)
			{
				CString strFile = AfxGetApp()->GetProfileString("Last File", "Last File", NULL);
				if (!strFile.IsEmpty() && ::PathFileExists(strFile))
				{
					// avoid repeated crash
					AfxGetApp()->WriteProfileString("Last File", "Last File", NULL);
					cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
					cmdInfo.m_strFileName = strFile;
				}
			}
		}
	}

	// Create a blank document first, because ProcessShellCommand() may fail
	OnFileNew();

	// The one and only window has been initialized, so show and update it

	// NOTE: CSingleDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible) already make it visible
	if (!m_pMainWnd->IsWindowVisible())
		m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.

	// NOTE: if failed to open the last document, do not quit
	if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
	{
		ProcessShellCommand(cmdInfo);
	}
	else
	{
		if (AfxGetApp()->GetProfileInt("Templates", "Show at Startup", TRUE))
			m_pMainWnd->PostMessage(WM_COMMAND, ID_FILE_NEW_FROM_TEMPLATE, 0L);
	}

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	// We use CDesignnd instead
	//m_pMainWnd->DragAcceptFiles();
	CCategoryWnd::Instance()->LoadActivePage();

	CSplashWnd::Instance()->HideSplashScreen();
	CSplashWnd::ReleaseInstance();

	return TRUE;
}

void CGlandaApp::OnAppAbout()
{
	// TODO: Add your command handler code here

	CDlgAbout aboutDlg;
	aboutDlg.DoModal();
}

int CGlandaApp::ExitInstance()
{
	_AtlModule.RevokeClassObjects();
	// TODO: Add your specialized code here and/or call the base class

	EffectInplaceEditor::ReleaseInstance();

	CMruFileManager::Instance()->Remove(ID_MY_RECENT_FILES);
	CMruFileManager::ReleaseInstance();

	CSubjectManager::ReleaseInstance();
	COptions::ReleaseInstance();

	CDlgColorSwatch::ReleaseInstance();
	CBalloonToolTip::ReleaseInstance();
	CTipWnd::ReleaseInstance();
	CMemberListBox::ReleaseInstance();

	CVersionInfo::ReleaseInstance();

	CDlgTestImport::ReleaseInstance();

	// free ini file
	//CIniFile::Release();

	// free effects
	gldEffectManager::ReleaseInstance();

	// free CSWFLib constants
	FreeCSWFLibStrings();

	//_CrtDumpMemoryLeaks();

	GdiplusShutdown(g_gdiplusToken);

	CSWFImportCache::Instance()->Uninitialize();
	CSWFImportCache::ReleaseInstance();

	if (g_hMutex != NULL)
	{
		CloseHandle(g_hMutex);
	}

	return CWinApp::ExitInstance();
}

void CGlandaApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class

	//CWinApp::AddToRecentFileList(lpszPathName);
	CMainFrame *pFrame = (CMainFrame *)m_pMainWnd;
	CMruFileManager::Instance()->AddToRecentFileList(lpszPathName);
}

BOOL CGlandaApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	CBalloonToolTip::Instance()->RelayEvent(pMsg);
	CTipWnd::Instance()->RelayEvent(pMsg);
	CMemberListBox::Instance()->RelayEvent(pMsg);

	if (pMsg->message == WM_MOUSEWHEEL)
	{
		CWnd *pWnd = CWnd::WindowFromPoint(pMsg->pt);
		if (pWnd && pWnd->m_hWnd != pMsg->hwnd)
		{
			if (pWnd->IsWindowEnabled())
			{
				pWnd->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

CDocument* CGlandaApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	// TODO: Add your specialized code here and/or call the base class

	CString _ext = PathFindExtension(lpszFileName);
	if (_ext.CompareNoCase(".swf") == 0)
	{
		CGlandaDoc *pDoc = _GetCurDocument();
		return pDoc->ImportSWF(lpszFileName) ? pDoc : NULL;
	}
	else
	{
		return CWinApp::OpenDocumentFile(lpszFileName);
	}
}

void CGlandaApp::OnDebugTestimport()
{
	// TODO: Add your command handler code here

	CString strPassword;
	if (!InputBox("Password", "Enter password:", "", strPassword))
		return;
	if (strPassword != "sothink")
		return;

	CDlgTestImport *pDlg = CDlgTestImport::Instance();
	if (!::IsWindow(pDlg->m_hWnd))
		pDlg->Create(IDD_TESTIMPORT_DIALOG, CWnd::GetDesktopWindow());
	pDlg->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

/*
void CGlandaApp::OnHelpRegister()
{
	// TODO: Add your command handler code here

#if !defined ELEMENT5
	DoRegister();
#endif
}

void CGlandaApp::OnUpdateHelpRegister(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

#if !defined ELEMENT5
	DoUpdateRegisterMenu(pCmdUI);
#endif
}*/

void CGlandaApp::OnHelpAboutLame()
{
	// TODO: Add your command handler code here

	CDlgAboutLame dlg;
	dlg.DoModal();
}
