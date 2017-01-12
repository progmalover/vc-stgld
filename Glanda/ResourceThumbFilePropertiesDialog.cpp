// ResourceThumbFilePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ResourceThumbFilePropertiesDialog.h"
#include "ResourceThumbListCtrl.h"
#include "filepath.h"
#include "ResourceThumbItem.h"
#include "my_app.h"
#include "CmdChangeSymbolName.h"
#include "DlgComment.h"
#include "SWFStream.h"
#include "ComUtils.h"

// CResourceThumbFilePropertiesDialog dialog

IMPLEMENT_DYNAMIC(CResourceThumbFilePropertiesDialog, CPropertyPage)
CResourceThumbFilePropertiesDialog::CResourceThumbFilePropertiesDialog(CResourceThumbItem* pThumbItem, CWnd* pParent /*=NULL*/)
	: CPropertyPage(CResourceThumbFilePropertiesDialog::IDD)
	, m_pThumbItem(pThumbItem)
{
}

CResourceThumbFilePropertiesDialog::~CResourceThumbFilePropertiesDialog()
{
}

void CResourceThumbFilePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResourceThumbFilePropertiesDialog, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_COMMENT, OnBnClickedButtonComment)
END_MESSAGE_MAP()


// CResourceThumbFilePropertiesDialog message handlers

BOOL CResourceThumbFilePropertiesDialog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_tooltip.Create(this);

	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(SHFILEINFO));
	SHGetFileInfo(m_pThumbItem->m_strPathName, 0, &sfi, sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_TYPENAME | SHGFI_LARGEICON | SHGFI_ICON);

	CStatic* pStaticIcon = (CStatic*)GetDlgItem(IDC_STATIC_ICON);
	pStaticIcon->SetIcon(sfi.hIcon);

	SetDlgItemText(IDC_EDIT_TYPE, sfi.szTypeName);

	CFileStatus fs;
	if(CFile::GetStatus(m_pThumbItem->m_strPathName, fs))
	{
		CString strTemp;
		if (fs.m_size > 0)
		{
			if (fs.m_size < 1024)
			{
				strTemp.Format(IDS_N_BYTES, fs.m_size);
			}
			else
			{
				float fSize = (float)fs.m_size / 1024;
				strTemp.Format("%.1f KB", fSize);
			}
		}
		SetDlgItemText(IDC_EDIT_SIZE, strTemp);

		CString strFileName = ::FileStripPath(m_pThumbItem->m_strPathName);
		CWnd* pWndFileName = GetDlgItem(IDC_EDIT_NAME);
		CRect rc;
		pWndFileName->GetClientRect(&rc);
		pWndFileName->SetWindowText(strFileName);
		CDC* pDCFileName = pWndFileName->GetDC();
		if(pDCFileName->GetTextExtent(strFileName).cx > rc.Width())
		{
			m_tooltip.AddTool(pWndFileName, strFileName);
		}
		pWndFileName->ReleaseDC(pDCFileName);

		CString strLocation = ::FileStripFileName(m_pThumbItem->m_strPathName);
		CWnd* pWndLocation = GetDlgItem(IDC_EDIT_LOCATION);
		pWndLocation->GetClientRect(&rc);
		pWndLocation->SetWindowText(strLocation);
		CDC* pDCLocation = pWndLocation->GetDC();
		if(pDCLocation->GetTextExtent(strLocation).cx > rc.Width())
		{
			m_tooltip.AddTool(pWndLocation, strLocation);
		}
		pWndLocation->ReleaseDC(pDCLocation);

		SetDlgItemText(IDC_EDIT_CREATED_TIME, fs.m_ctime.Format("%Y-%m-%d %H:%M"));
		SetDlgItemText(IDC_EDIT_MODIFIED_TIME, fs.m_mtime.Format("%Y-%m-%d %H:%M"));
		SetDlgItemText(IDC_EDIT_ACCESSED_TIME, fs.m_atime.Format("%Y-%m-%d %H:%M"));

		BOOL bShowComment = FALSE;
		switch(m_pThumbItem->GetResourceType())
		{
		case RESOURCE_BUTTONS:
		case RESOURCE_MOVIECLIP:
			bShowComment = TRUE;
		case RESOURCE_IMAGES:
		case RESOURCE_SHAPES:
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
			}
			break;
		case RESOURCE_SOUNDS:
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
			}
			break;
		}

		GetDlgItem(IDC_BUTTON_COMMENT)->ShowWindow(bShowComment ? SW_SHOW : SW_HIDE);
	}

	m_tooltip.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CResourceThumbFilePropertiesDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		m_tooltip.RelayEvent(pMsg);

	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CResourceThumbFilePropertiesDialog::OnApply()
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
					GotoDlgCtrl(pEditName);
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

void CResourceThumbFilePropertiesDialog::OnBnClickedButtonComment()
{
	// TODO: Add your control notification handler code here

	CDlgComment dlg;
	dlg.m_strComment = m_pThumbItem->GetDesc();
	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor wc;
		HRESULT hr = ChangeObjectFileDesc(m_pThumbItem->m_strPathName, dlg.m_strComment);
		if (SUCCEEDED(hr))
			m_pThumbItem->SetDesc(dlg.m_strComment);
		else
			::CoReportError(hr);
	}
}
