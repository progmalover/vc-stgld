// PagePublishInsertCode.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishInsertCode.h"
#include "SheetPublish.h"
#include "GlandaDoc.h"


// CPagePublishInsertCode dialog

IMPLEMENT_DYNAMIC(CPagePublishInsertCode, CPropertyPageFixed)
CPagePublishInsertCode::CPagePublishInsertCode()
	: CPropertyPageFixed(CPagePublishInsertCode::IDD)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_INSERT_CODE, 0);
}

CPagePublishInsertCode::~CPagePublishInsertCode()
{
}

void CPagePublishInsertCode::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_FILE, m_edtFile);
}


BEGIN_MESSAGE_MAP(CPagePublishInsertCode, CPropertyPageFixed)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_EN_CHANGE(IDC_RICHEDIT_FILE, OnEnChangeRicheditFile)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, OnBnClickedButtonPreview)
END_MESSAGE_MAP()


// CPagePublishInsertCode message handlers

void CPagePublishInsertCode::OnBnClickedButtonInsert()
{
	// TODO: Add your control notification handler code here

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();
	CString strCode = pSheet->m_pageHTMLOptions.m_strHTML;

	LONG lStart, lEnd;
	m_edtFile.GetSel(lStart, lEnd);
	m_edtFile.ReplaceSel(strCode, TRUE);
	strCode.Replace("\r\n", "\n");
	m_edtFile.SetSel(lStart, lStart + strCode.GetLength());

	m_bInserted = TRUE;
	m_bSaved = FALSE;
}

void CPagePublishInsertCode::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here

	DoSave();
}

void CPagePublishInsertCode::OnBnClickedButtonPreview()
{
	// TODO: Add your control notification handler code here

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();
	
	if (m_bChanged && !m_bSaved)
	{
		int ret = AfxMessageBox(IDS_PROMPT_SAVE_MODIFIED_FILE, MB_ICONWARNING | MB_YESNOCANCEL);
		if (ret == IDCANCEL)
			return;

		if (ret == IDYES)
		{
			if (!DoSave())
				return;
		}
	}

	ShellExecute(NULL, "open", pSheet->m_pageSelectHTML.m_strFile, NULL, NULL, SW_SHOWNORMAL);
}

BOOL CPagePublishInsertCode::DoSave()
{
	CWaitCursor wc;

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();
	CString strFile = pSheet->m_pageSelectHTML.m_strFile;

	CString strMovie = pSheet->m_pageSelectHTML.m_strFile;
	::PathRemoveFileSpec(strMovie.GetBuffer(0));
	strMovie.ReleaseBuffer();
	strMovie += "\\";
	strMovie += _GetCurDocument()->GetExportMovieName();

	if (!::ExportMovie(pSheet, strMovie, AfxGetApp()->GetProfileInt("Export", "Compress", TRUE), FALSE))
		return FALSE;

	CString strCode;
	m_edtFile.GetWindowText(strCode);

	switch (pSheet->m_pageSelectHTML.m_nFormat)
	{
	case FORMAT_WIDNOWS:
		break;
	case FORMAT_UNIX:
		strCode.Replace("\r", "");
		break;
	case FORMAT_MAC:
		strCode.Replace("\n", "");
		break;
	}

	try
	{
		CFile file(strFile, CFile::modeCreate | CFile::modeWrite);
		file.SeekToBegin();
		int len = strCode.GetLength();
		if (len > 0)
			file.Write(strCode, len);
		file.Close();

		pSheet->m_pageSelectHTML.m_strFileContent = strCode;
		m_bSaved = TRUE;

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

		return TRUE;
	}
	catch (CFileException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
}

BOOL CPagePublishInsertCode::OnInitDialog()
{
	CPropertyPageFixed::OnInitDialog();

	// TODO:  Add extra initialization here

	m_edtFile.SetEventMask(ENM_CHANGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPagePublishInsertCode::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();
	
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	m_edtFile.SetWindowText(pSheet->m_pageSelectHTML.m_strFileContent);

	m_bChanged = FALSE;
	m_bInserted = FALSE;
	m_bSaved = FALSE;

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	return CPropertyPageFixed::OnSetActive();
}

LRESULT CPagePublishInsertCode::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_bChanged && !m_bSaved)
	{
		int ret = AfxMessageBox(IDS_PROMPT_SAVE_MODIFIED_FILE, MB_ICONWARNING | MB_YESNOCANCEL);
		if (ret == IDCANCEL)
			return -1;

		if (ret == IDYES)
		{
			if (!DoSave())
				return -1;
		}
	}

	return IDD_PUBLISH_SELECT_HTML;
}

LRESULT CPagePublishInsertCode::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!m_bInserted)
	{
		if (AfxMessageBox(IDS_PROMPT_CODE_NOT_INSERTED, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
			return -1;
	}
	else
	{
		if (m_bChanged && !m_bSaved)
		{
			int ret = AfxMessageBox(IDS_PROMPT_SAVE_MODIFIED_FILE, MB_ICONWARNING | MB_YESNOCANCEL);
			if (ret == IDCANCEL)
				return -1;

			if (ret == IDYES)
			{
				if (!DoSave())
					return -1;
			}
		}
	}

	return IDD_PUBLISH_FINISHED;
}

void CPagePublishInsertCode::OnEnChangeRicheditFile()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPageFixed::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	m_bChanged = TRUE;
	m_bSaved = FALSE;

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}
