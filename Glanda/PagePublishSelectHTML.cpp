// PagePublishSelectHTML.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishSelectHTML.h"
#include "filepath.h"
#include "FileDialogEx.h"
#include "SheetPublish.h"

// CPagePublishSelectHTML dialog

IMPLEMENT_DYNAMIC(CPagePublishSelectHTML, CPropertyPageFixed)
CPagePublishSelectHTML::CPagePublishSelectHTML()
	: CPropertyPageFixed(CPagePublishSelectHTML::IDD)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_SELECT_HTML, 0);

	m_nFormat = FORMAT_UNDEFINED;
}

CPagePublishSelectHTML::~CPagePublishSelectHTML()
{
}

void CPagePublishSelectHTML::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FILE, m_cmbFile);
}


BEGIN_MESSAGE_MAP(CPagePublishSelectHTML, CPropertyPageFixed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CPagePublishSelectHTML message handlers

void CPagePublishSelectHTML::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here

	CString strTitle, strFilter;
	strTitle.LoadString(IDS_HTML_TITLE);
	strFilter.LoadString(IDS_FILTER_HTML_ALL);
	CFileDialogEx dlg(strTitle, TRUE, NULL, strFilter);
	dlg.m_pOFN->Flags &= ~OFN_FILEMUSTEXIST;			// because we allow to create new file.
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_COMBO_FILE, dlg.GetPathName());
}

BOOL CPagePublishSelectHTML::OnInitDialog()
{
	CPropertyPageFixed::OnInitDialog();

	// TODO:  Add extra initialization here

	m_cmbFile.Init("Publish\\Insert", 16);
	if (m_cmbFile.Load() > 0)
		m_cmbFile.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPagePublishSelectHTML::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPageFixed::OnSetActive();
}

LRESULT CPagePublishSelectHTML::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	return IDD_PUBLISH_HTML_OPTIONS;
}

LRESULT CPagePublishSelectHTML::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	CString strFile;
	m_cmbFile.GetWindowText(strFile);

	strFile.Trim();
	if (strFile.IsEmpty())
	{
		AfxMessageBox(IDS_PROMPT_ENTER_HTML_FULL_PATH);
		return -1;
	}

	if (!::IsValidPath(strFile))
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_INVALID_FILE_PATH1, (LPCTSTR)strFile);
		return -1;
	}

	if (m_strFile.CompareNoCase(strFile) != 0)
	{
		UINT nFlags = CFile::modeRead;
		if (!::PathFileExists(strFile))
		{
			if (AfxMessageBoxEx(MB_ICONQUESTION | MB_YESNO, IDS_PROMPT_CREATE_FILE1, (LPCTSTR)strFile) != IDYES)
				return -1;
			nFlags = CFile::modeCreate;
		}
		try
		{
			CFile file(strFile, nFlags);
			int len = file.GetLength();
			if (len == 0)
			{
				m_strFileContent.Empty();
			}
			else
			{
				TCHAR *pBuf = m_strFileContent.GetBuffer(len + 1);
				file.Read(pBuf, len);
				pBuf[len] = 0;
				m_strFileContent.ReleaseBuffer();
			}
			m_strFile = strFile;
		}
		catch (CFileException *e)
		{
			e->ReportError();
			e->Delete();
			return -1;
		}

		m_nFormat = FORMAT_WIDNOWS;
		int nPos = m_strFileContent.Find("\n");
		if (nPos >= 0)
		{
			if (nPos == 0 || m_strFileContent[nPos - 1] != '\r')
				m_nFormat = FORMAT_UNIX;
		}
		else
		{
			nPos = m_strFileContent.Find("\r");
			if (nPos >= 0)
			{
				if (nPos == m_strFileContent.GetLength() - 1 || m_strFileContent[nPos + 1] != '\n')
					m_nFormat = FORMAT_MAC;
			}
		}	

		m_cmbFile.AddCurrentText();
		m_cmbFile.Save();
	}

	return IDD_PUBLISH_INSERT_CODE;
}
