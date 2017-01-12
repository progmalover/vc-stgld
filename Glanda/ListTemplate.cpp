// ListTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ListTemplate.h"
#include "filepath.h"
#include "gldInstance.h"
#include ".\listtemplate.h"
#include "Utils.h"
#include "filepath.h"
#include "InputBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CListTemplate

CListTemplate::CListTemplate()
{
}

CListTemplate::~CListTemplate()
{
}


void CListTemplate::Fill(FilterTemplate filter)
{
	LoadImages(IDB_COMBO_EFFECT, ILC_MASK | ILC_COLOR32, 16, 16, RGB(255, 0, 255));

	LPCTSTR pszModulePath = GetModuleFilePath();
	
	if (filter == FILTER_ELEMENT)
	{
		CString strFilterMask;
		CString strTempPath;
		strFilterMask.Format("%s\\Templates\\Element\\*.*", pszModulePath);
		strTempPath.Format("%s\\Templates\\Element", pszModulePath);
		WIN32_FIND_DATA wfd;
		memset(&wfd, 0, sizeof(wfd));
		HANDLE hFind = FindFirstFile(strFilterMask, &wfd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0
					&& (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					CString strTempFile;
					strTempFile.Format("%s\\%s", (LPCTSTR)strTempPath, wfd.cFileName);
					CString strTempName;
					if (SUCCEEDED(gldInstance::GetTemplateName(strTempFile, strTempName)))
					{
						int nItem = AddString(strTempName);
						SetItemUserData(nItem, (DWORD_PTR)(new CString(strTempFile)));
						SetItemImage(nItem, 0);
						SetItemBold(nItem, FALSE);
					}
				}
			} while(FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}
	}
}

void CListTemplate::SetCurSelNotify(int nCur)
{
	SetCurSel(nCur);
	CWnd *parent = GetParent();
	if (parent != NULL)
	{
		parent->SendMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd);
	}	
}

LPCTSTR CListTemplate::GetCurTemplate()
{
	int nSel = GetCurSel();
	if (nSel > -1)
	{
		return (LPCTSTR)(*(CString *)GetItemUserData(nSel));
	}
	return NULL;
}

void CListTemplate::DeleteCurSel()
{
	LPCTSTR pszFile = GetCurTemplate();
	if (pszFile == NULL)
	{
		return;
	}
	CString strName = FileStripPath(pszFile);
	int nDot = strName.ReverseFind(_T('.'));
	if (nDot != -1)
	{
		strName = strName.Left(nDot);
	}
	if (AfxMessageBoxEx(MB_YESNO, IDS_SURE_DEL_TEMPLATE_1, (LPCTSTR)strName) == IDNO)
	{
		return;
	}
	if (!DeleteFile(pszFile, FALSE))
	{
		ReportLastError(IDS_FAILED_DEL_TEMPLATE_1);
	}
	else
	{
		int nCur = GetCurSel();
		DeleteString(nCur);
		if (nCur > GetCount())
		{
			nCur = -1;
		}
		SetCurSelNotify(nCur);
	}
}

bool RenameElementTemplateNameIsValid(void *pData, LPCTSTR lpszText)
{
	CString fileName(lpszText);
	fileName.Trim();
	if (fileName.GetLength() == 0)
	{
		AfxMessageBox(IDS_NEED_TEMPLATE_NAME);
		return FALSE;
	}
	if (!IsValidFileName(fileName))
	{
		AfxMessageBox(IDS_ERROR_INVALID_FILE_NAME);
		return FALSE;
	}	
	return TRUE;
}

void CListTemplate::RenameCurSel()
{
	LPCTSTR pszFile = GetCurTemplate();
	if (pszFile == NULL)
	{
		return;
	}
	CString strName = FileStripPath(pszFile);
	int nDot = strName.ReverseFind(_T('.'));
	if (nDot != -1)
	{
		strName = strName.Left(nDot);
	}
	CString strPath = FileStripFileName(pszFile);
	while (1)
	{
		CInputBox inputName(this, IDS_RENAME_EFFECT_SETTINGS, IDS_INPUT_NEW_SETTINGS_NAME
			, strName, RenameElementTemplateNameIsValid, NULL);
		if (inputName.DoModal() == IDOK)
		{
			strName = inputName.m_strText;
			CString strNewFile;
			strNewFile.Format("%s\\%s.gmd", (LPCTSTR)strPath, (LPCTSTR)strName);
			if (_tcsicmp(pszFile, strNewFile) == 0)
			{
				break;
			}
			try
			{
				CFile::Rename(pszFile, strNewFile);
				DeleteString(GetCurSel());
				int nItem = AddString(strName);
				SetItemImage(nItem, 0);
				SetItemUserData(nItem, (DWORD_PTR)(new CString(strNewFile)));
				SetCurSel(nItem);
				break;
			}
			catch (CFileException *e)
			{
				if (e->m_lOsError == ERROR_ALREADY_EXISTS)
				{
					AfxMessageBoxEx(MB_OK, IDS_TEMPLATE_EXISTS_1, (LPCTSTR)strName);
				}
				else
				{
					ReportLastError(IDS_FAILED_RENAME_TEMPLATE_1);
				}
				e->Delete();
			}
		}
		else
		{
			break;
		}
	}	
}

BEGIN_MESSAGE_MAP(CListTemplate, COwnerDrawListBox)		
END_MESSAGE_MAP()


// CListTemplate message handlers
void CListTemplate::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	if (lpDeleteItemStruct->itemID != -1)
	{
		CString *str = (CString *)GetItemUserData(lpDeleteItemStruct->itemID);
		if (str != NULL)
		{
			delete str;
		}
	}
	COwnerDrawListBox::DeleteItem(lpDeleteItemStruct);
}
