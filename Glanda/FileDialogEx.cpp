// FileDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "FileDialogEx.h"
#include ".\filedialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)
CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

CFileDialogEx::CFileDialogEx(LPCTSTR lpszSection, BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR lpszFileName, CWnd *pParentWnd) : 
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, bOpenFileDialog ? OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | 0x02000000 /*OFN_DONTADDTORECENT*/ : OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, lpszFilter, pParentWnd)
{
	m_strSection.Format("File Dialogs\\%s", lpszSection);

	int nFilterIndex = AfxGetApp()->GetProfileInt(m_strSection, "Filter Index", 0);
	m_pOFN->nFilterIndex = nFilterIndex;

	m_strPath = AfxGetApp()->GetProfileString(m_strSection, "Last Path", NULL);
	if (!m_strPath.IsEmpty() && ::PathIsDirectory(m_strPath))
		m_pOFN->lpstrInitialDir = (LPCTSTR)m_strPath;
}

CFileDialogEx::~CFileDialogEx()
{
}


BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
END_MESSAGE_MAP()



// CFileDialogEx message handlers

BOOL CFileDialogEx::OnFileNameOK()
{
	// TODO: Add your specialized code here and/or call the base class

	AfxGetApp()->WriteProfileInt(m_strSection, "Filter Index", m_pOFN->nFilterIndex);
	ASSERT (m_pOFN->lpstrFile != NULL);	// see MFC source

	if (_tcslen(m_pOFN->lpstrFile) > 0)
	{
		TCHAR szPath[_MAX_PATH + 1];
		memset(szPath, 0,  sizeof(szPath));
		_tcsncpy(szPath, m_pOFN->lpstrFile, _MAX_PATH);

		if (!PathIsDirectory(szPath)) PathRemoveFileSpec(szPath);
		//modify by ÍõÏþ·É 2004-7-10 13:16,fix multiselect bug.

		ASSERT(::PathIsDirectory(szPath));
		AfxGetApp()->WriteProfileString(m_strSection, "Last Path", szPath);
	}

	return CFileDialog::OnFileNameOK();
}
