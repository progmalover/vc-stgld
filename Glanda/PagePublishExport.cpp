// PagePublishExport.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishExport.h"
#include "DlgExport.h"
#include "Global.h"
#include "GlandaDoc.h"
#include "filepath.h"
#include ".\pagepublishexport.h"

// CPagePublishExport dialog

IMPLEMENT_DYNAMIC(CPagePublishExport, CPropertyPageFixed)
CPagePublishExport::CPagePublishExport()
	: CPropertyPageFixed(CPagePublishExport::IDD)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_EXPORT_MOVIE, 0);
}

CPagePublishExport::~CPagePublishExport()
{
}

void CPagePublishExport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPagePublishExport, CPropertyPageFixed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CPagePublishExport message handlers

BOOL CPagePublishExport::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPageFixed::OnSetActive();
}

void CPagePublishExport::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here

	CString strName = _GetCurDocument()->GetExportMovieName();

	CString strTitle, strFilter;
	strTitle.LoadString(IDS_EXPORT_MOVIE_TITLE);
	strFilter.LoadString(IDS_FILTER_SWF);
	CDlgExport dlg(strTitle, FALSE, "swf", strFilter, strName);
	dlg.m_pOFN->lpstrTitle = strTitle;
	if (dlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_PATH, dlg.GetPathName());
		CheckDlgButton(IDC_CHECK_COMPRESS, dlg.m_bCompress ? BST_CHECKED : BST_UNCHECKED);
	}
}

LRESULT CPagePublishExport::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	return IDD_PUBLISH_SELECT;
}

LRESULT CPagePublishExport::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	CString strMovie;
	GetDlgItemText(IDC_EDIT_PATH, strMovie);

	strMovie.Trim();
	if (strMovie.IsEmpty())
	{
		AfxMessageBox(IDS_PROMPT_ENTER_MOVIE_PATH);
		return -1;
	}

	if (!::IsValidPath(strMovie))
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_INVALID_FILE_PATH1, (LPCTSTR)strMovie);
		return -1;
	}

	BOOL bCompress = (IsDlgButtonChecked(IDC_CHECK_COMPRESS) == BST_CHECKED ? TRUE : FALSE);
	AfxGetApp()->WriteProfileInt("Export", "Compress", bCompress);
    if (!ExportMovie(this->GetParent(), strMovie, bCompress, FALSE))
		return -1;

	_GetCurDocument()->m_strMovieExport = strMovie;

	return IDD_PUBLISH_FINISHED;
}

BOOL CPagePublishExport::OnInitDialog()
{
	CPropertyPageFixed::OnInitDialog();

	// TODO:  Add extra initialization here

	SetDlgItemText(IDC_EDIT_PATH, _GetCurDocument()->m_strMovieExport);
	
	BOOL bCompress = AfxGetApp()->GetProfileInt("Export", "Compress", TRUE);
	CheckDlgButton(IDC_CHECK_COMPRESS, bCompress ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
