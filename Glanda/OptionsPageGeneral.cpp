// OptionsPageGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OptionsPageGeneral.h"

#include "Options.h"
#include "ResourceExplorerTree.h"
#include "SWFImportCache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COptionsPageGeneral dialog

COptionsPageGeneral::COptionsPageGeneral()
	: CPropertyPage(COptionsPageGeneral::IDD)
{
}

COptionsPageGeneral::~COptionsPageGeneral()
{
}

void COptionsPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_RELOAD_LAST_DOCUMENT, COptions::Instance()->m_bReloadLastDocument);
	DDX_Check(pDX, IDC_CHECK_SHOW_BALLOON_TIPS, COptions::Instance()->m_bShowContextSensitiveTips);
	DDX_Check(pDX, IDC_CHECK_CACHE_RESOURCES, COptions::Instance()->m_bCacheSWFResources);
}


BEGIN_MESSAGE_MAP(COptionsPageGeneral, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ALL_TIPS, OnBnClickedButtonResetAllTips)
	ON_BN_CLICKED(IDC_BUTTON_EMPTY_CACHE, OnBnClickedButtonEmptyCache)
END_MESSAGE_MAP()


// COptionsPageGeneral message handlers

extern BOOL AFXAPI _AfxDeleteRegKey(LPCTSTR lpszKey);
void COptionsPageGeneral::OnBnClickedButtonResetAllTips()
{
	// TODO: Add your control notification handler code here

	BOOL bOK = FALSE;

	CWaitCursor wc;
	CString strRegKey;
	strRegKey.Format("Software\\%s\\%s\\Tips", AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszProfileName);

	HKEY hKey = NULL;
	if (::RegOpenKey(HKEY_CURRENT_USER, strRegKey, &hKey) == ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		
		LONG ret = AfxGetApp()->DelRegTree(HKEY_CURRENT_USER, strRegKey);
		if (ret == ERROR_SUCCESS)
		{
			bOK = TRUE;
		}
		else
		{
			SetLastError(ret);
			ReportLastError(IDS_ERROR_DELETE_REG_KEY1);
		}
	}
	else	// the key is not created yet.
	{
		bOK = TRUE;
	}

	if (bOK)
	{
		Sleep(100);
		GetDlgItem(IDC_BUTTON_RESET_ALL_TIPS)->EnableWindow(FALSE);
	}
}

extern BOOL RemoveDirectoryRecursively(const CString &path);
void COptionsPageGeneral::OnBnClickedButtonEmptyCache()
{
	// TODO: Add your control notification handler code here

	CWaitCursor wc;
	CString strPath = CSWFImportCache::GetCachePath();
	if (!::PathFileExists(strPath) || RemoveDirectoryRecursively(strPath))
	{
		Sleep(100);
		GetDlgItem(IDC_BUTTON_EMPTY_CACHE)->EnableWindow(FALSE);
	}
}
