// Glanda.h : main header file for the GMovie DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// CGlandaApp
// See GMovie.cpp for the implementation of this class
//

class CGlandaApp : public CWinApp
{
public:
	CGlandaApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
	virtual int ExitInstance();
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	afx_msg void OnDebugTestimport();
	//afx_msg void OnHelpRegister();
	//afx_msg void OnUpdateHelpRegister(CCmdUI *pCmdUI);
	afx_msg void OnHelpAboutLame();
};

extern CGlandaApp theApp;
