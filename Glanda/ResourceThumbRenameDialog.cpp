// ResourceThumbRenameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ResourceThumbRenameDialog.h"
#include "ResourceThumbItem.h"
#include "filepath.h"
#include "CmdChangeSymbolName.h"
#include "my_app.h"
#include "Observer.h"


// CResourceThumbRenameDialog dialog

IMPLEMENT_DYNAMIC(CResourceThumbRenameDialog, CDialog)
CResourceThumbRenameDialog::CResourceThumbRenameDialog(CResourceThumbItem* pThumbItem, CWnd* pParent /*=NULL*/)
	: CDialog(CResourceThumbRenameDialog::IDD, pParent)
	, m_pThumbItem(pThumbItem)
{
}

CResourceThumbRenameDialog::~CResourceThumbRenameDialog()
{
}

void CResourceThumbRenameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResourceThumbRenameDialog, CDialog)
END_MESSAGE_MAP()


// CResourceThumbRenameDialog message handlers

BOOL CResourceThumbRenameDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_pThumbItem->m_bStockResource)
	{
		SetDlgItemText(IDC_EDIT_NAME, m_pThumbItem->m_pObj->m_name.c_str());
	}
	else
	{
		SetDlgItemText(IDC_EDIT_NAME, ::FileStripPath(m_pThumbItem->m_strPathName));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CResourceThumbRenameDialog::OnOK()
{
	CEdit* pEditName = (CEdit*)GetDlgItem(IDC_EDIT_NAME);

	if(pEditName->GetModify())
	{
		CString strNewName;
		GetDlgItemText(IDC_EDIT_NAME, strNewName);
		strNewName.TrimLeft();
		strNewName.TrimRight();
		if(m_pThumbItem->m_bStockResource)
		{
			TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_CHANGE_SYMBOL_NAME);
			pCmdGroup->Do(new CCmdChangeSymbolName(m_pThumbItem->m_pObj, (LPCTSTR)strNewName));
			my_app.Commands().Do(pCmdGroup);
			CDialog::OnOK();
		}
		else
		{
			if(IsValidFileName(strNewName)==FALSE || strNewName[0] == _T('.'))
			{
				AfxMessageBox(IDS_ERROR_INVALID_FILE_NAME, MB_OK|MB_ICONERROR);
				GotoDlgCtrl(pEditName);
			}
			else
			{
				CString strExt = ::PathFindExtension(strNewName);
				if(strExt.CompareNoCase(::PathFindExtension(m_pThumbItem->m_strPathName))==0)
				{
					CString strNewPathName = ::FileStripFileName(m_pThumbItem->m_strPathName) + '\\' + strNewName;
					if(MoveFile(m_pThumbItem->m_strPathName, strNewPathName))
					{
						CString strMovePathName[2];
						strMovePathName[0] = m_pThumbItem->m_strPathName;
						strMovePathName[1] = strNewPathName;

						CSubjectManager::Instance()->GetSubject("MoveFile")->Notify((void*)strMovePathName);
						CDialog::OnOK();
					}
					else
					{
						ReportLastError();
					}
				}
				else
				{
					AfxMessageBox(IDS_ERROR_CHANGE_FILE_NAME_EXTENSION, MB_OK|MB_ICONERROR);
					GotoDlgCtrl(pEditName);
				}
			}
		}
	}
	else
	{
		CDialog::OnOK();
	}
}
