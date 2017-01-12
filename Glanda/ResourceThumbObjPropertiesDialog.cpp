// ResourceThumbObjPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ResourceThumbObjPropertiesDialog.h"
#include "ResourceThumbItem.h"
#include "CmdChangeSymbolName.h"
#include "my_app.h"
#include "Observer.h"
#include "filepath.h"


// CResourceThumbObjPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CResourceThumbObjPropertiesDialog, CPropertyPage)
CResourceThumbObjPropertiesDialog::CResourceThumbObjPropertiesDialog(CResourceThumbItem* pThumbItem, CWnd* pParent /*=NULL*/)
	: CPropertyPage(CResourceThumbObjPropertiesDialog::IDD)
	, m_pThumbItem(pThumbItem)
{
}

CResourceThumbObjPropertiesDialog::~CResourceThumbObjPropertiesDialog()
{
}

void CResourceThumbObjPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResourceThumbObjPropertiesDialog, CPropertyPage)
END_MESSAGE_MAP()


// CResourceThumbObjPropertiesDialog message handlers

BOOL CResourceThumbObjPropertiesDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CResourceThumbObjPropertiesDialog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	ASSERT(m_pThumbItem->m_bStockResource);

	SetDlgItemText(IDC_EDIT_NAME, m_pThumbItem->m_pObj->m_name.c_str());

	TCHAR *pszExt = NULL;
	if(m_pThumbItem->m_pObj->IsGObjInstanceOf(gobjImage))
	{
		CString strDimensions;
		strDimensions.Format("%d x %d", m_pThumbItem->GetDimensions().cx, m_pThumbItem->GetDimensions().cy);
		SetDlgItemText(IDC_EDIT_DIMENSIONS, strDimensions);
		GetDlgItem(IDC_EDIT_DIMENSIONS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DIMENSIONS)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_SAMPLE_CHANNEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SAMPLE_CHANNEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SAMPLE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SAMPLE_RATE)->ShowWindow(SW_HIDE);

		CString s;
		s.LoadString(IDS_RESOURCE_OBJ_IMAGE);
		SetDlgItemText(IDC_EDIT_TYPE, s);

		pszExt = ".bmp";
	}
	else if(m_pThumbItem->m_pObj->IsGObjInstanceOf(gobjShape))
	{
		CString strDimensions;
		strDimensions.Format("%d x %d", m_pThumbItem->GetDimensions().cx, m_pThumbItem->GetDimensions().cy);
		SetDlgItemText(IDC_EDIT_DIMENSIONS, strDimensions);
		GetDlgItem(IDC_EDIT_DIMENSIONS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DIMENSIONS)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_SAMPLE_CHANNEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SAMPLE_CHANNEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SAMPLE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SAMPLE_RATE)->ShowWindow(SW_HIDE);

		CString s;
		s.LoadString(IDS_RESOURCE_OBJ_SHAPE);
		SetDlgItemText(IDC_EDIT_TYPE, s);

		pszExt = ".gls";
	}
	else if(m_pThumbItem->m_pObj->IsGObjInstanceOf(gobjButton))
	{
		CString strDimensions;
		strDimensions.Format("%d x %d", m_pThumbItem->GetDimensions().cx, m_pThumbItem->GetDimensions().cy);
		SetDlgItemText(IDC_EDIT_DIMENSIONS, strDimensions);
		GetDlgItem(IDC_EDIT_DIMENSIONS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DIMENSIONS)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_SAMPLE_CHANNEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SAMPLE_CHANNEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SAMPLE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SAMPLE_RATE)->ShowWindow(SW_HIDE);

		CString s;
		s.LoadString(IDS_RESOURCE_OBJ_BUTTON);
		SetDlgItemText(IDC_EDIT_TYPE, s);

		pszExt = ".glb";
	}
	else if(m_pThumbItem->m_pObj->IsGObjInstanceOf(gobjSound))
	{
		double dDuration = m_pThumbItem->GetDuration();
		int hours	= int(dDuration) / 3600;
		int minutes = int(dDuration) % 3600 / 60;
		int seconds = int(dDuration) % 60;
		CString strDuration;
		strDuration.Format("%d:%02d:%02d", hours, minutes, seconds);
		SetDlgItemText(IDC_EDIT_DURATION, strDuration);
		GetDlgItem(IDC_EDIT_DURATION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DURATION)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_SAMPLE_CHANNEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_SAMPLE_CHANNEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SAMPLE_RATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_SAMPLE_RATE)->ShowWindow(SW_SHOW);

		if(m_pThumbItem->GetSampleChannel()==0)
		{
			CString str;
			str.LoadString(IDS_SAMPLE_CHANNEL_MONO);
			SetDlgItemText(IDC_EDIT_SAMPLE_CHANNEL, str);
		}
		else if(m_pThumbItem->GetSampleChannel()==1)
		{
			CString str;
			str.LoadString(IDS_SAMPLE_CHANNEL_STERO);
			SetDlgItemText(IDC_EDIT_SAMPLE_CHANNEL, str);
		}
		switch(m_pThumbItem->GetSampleRate())
		{
		case 0:
			SetDlgItemText(IDC_EDIT_SAMPLE_RATE, "8Kbps");
			break;
		case 1:
			SetDlgItemText(IDC_EDIT_SAMPLE_RATE, "11Kbps");
			break;
		case 2:
			SetDlgItemText(IDC_EDIT_SAMPLE_RATE, "22Kbps");
			break;
		case 3:
			SetDlgItemText(IDC_EDIT_SAMPLE_RATE, "44Kbps");
			break;
		default:
			break;
		}

		CString s;
		s.LoadString(IDS_RESOURCE_OBJ_SOUND);
		SetDlgItemText(IDC_EDIT_TYPE, s);

		pszExt = ".wav";
	}

	if (pszExt)
	{
		SHFILEINFO sfi;
		memset(&sfi, 0, sizeof(SHFILEINFO));
		SHGetFileInfo(pszExt, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON);

		CStatic* pStaticIcon = (CStatic*)GetDlgItem(IDC_STATIC_ICON);
		pStaticIcon->SetIcon(sfi.hIcon);
	}

	CString strTemp;
	if (m_pThumbItem->GetDataLen() > 0)
	{
		if (m_pThumbItem->GetDataLen() < 1024)
		{
			strTemp.Format(IDS_N_BYTES, m_pThumbItem->GetDataLen());
		}
		else
		{
			float fSize = (float)m_pThumbItem->GetDataLen() / 1024;
			strTemp.Format("%.1f KB", fSize);
		}
	}
	SetDlgItemText(IDC_EDIT_SIZE, strTemp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CResourceThumbObjPropertiesDialog::OnApply()
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
			return CPropertyPage::OnApply();
		}
		else
		{
			if(IsValidFileName(strNewName)==FALSE || strNewName[0] == _T('.'))
			{
				AfxMessageBox(IDS_ERROR_INVALID_FILE_NAME, MB_OK|MB_ICONERROR);
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
						return CPropertyPage::OnApply();
					}
					else
					{
						ReportLastError();
					}
				}
				else
				{
					AfxMessageBox(IDS_ERROR_CHANGE_FILE_NAME_EXTENSION, MB_OK|MB_ICONERROR);
				}
			}
		}
		return FALSE;
	}
	else
	{
		return CPropertyPage::OnApply();
	}
}
