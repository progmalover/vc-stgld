// PagePublishViewCode.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishViewCode.h"
#include "SheetPublish.h"
#include ".\pagepublishviewcode.h"


// CPagePublishViewCode dialog

IMPLEMENT_DYNAMIC(CPagePublishViewCode, CPropertyPageFixed)
CPagePublishViewCode::CPagePublishViewCode()
	: CPropertyPageFixed(CPagePublishViewCode::IDD)
	, m_nFormat(0)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_VIEW_CODE, 0);

	m_nFormat = max(FORMAT_WIDNOWS, min(FORMAT_MAC, AfxGetApp()->GetProfileInt("Publish\\Code", "Clipboard Format", FORMAT_WIDNOWS)));
}

CPagePublishViewCode::~CPagePublishViewCode()
{
	AfxGetApp()->WriteProfileInt("Publish\\Code", "Clipboard Format", m_nFormat);
}

void CPagePublishViewCode::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_CODE, m_edtCode);
	DDX_Radio(pDX, IDC_RADIO_WINDOWS, m_nFormat);
}


BEGIN_MESSAGE_MAP(CPagePublishViewCode, CPropertyPageFixed)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
END_MESSAGE_MAP()


// CPagePublishViewCode message handlers

void CPagePublishViewCode::OnBnClickedButtonCopy()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	BOOL bOK = FALSE;

	if (::OpenClipboard(NULL))
	{
		CString strCode = m_strCode;
		switch (m_nFormat)
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
		
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (strCode.GetLength() + 1) * sizeof(TCHAR));
		if (hGlobal)
		{
			TCHAR *pBuf = (TCHAR *)::GlobalLock(hGlobal);
			if (pBuf)
			{
				::EmptyClipboard();
				strcpy(pBuf, strCode);
				::GlobalUnfix(hGlobal);
				bOK = (::SetClipboardData(CF_TEXT, hGlobal) != NULL);
			}
		}
		::CloseClipboard();
	}
	if (!bOK)
		ReportLastError();
}

LRESULT CPagePublishViewCode::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	return IDD_PUBLISH_HTML_OPTIONS;
}

LRESULT CPagePublishViewCode::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	return IDD_PUBLISH_FINISHED;
}

BOOL CPagePublishViewCode::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	if (m_strCode.Compare(pSheet->m_pageHTMLOptions.m_strHTML) != 0)
	{
		m_strCode = pSheet->m_pageHTMLOptions.m_strHTML;
		m_edtCode.SetWindowText(m_strCode);
	}

	return CPropertyPageFixed::OnSetActive();
}

BOOL CPagePublishViewCode::OnInitDialog()
{
	CPropertyPageFixed::OnInitDialog();

	// TODO:  Add extra initialization here

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
