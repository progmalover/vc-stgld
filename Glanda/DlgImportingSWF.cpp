// DlgImportingSWF.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgImportingSWF.h"
#include "SWFImportCache.h"
#include "Options.h"

#define IMPORT_SWF_PROGRESS WM_USER + 303

// CDlgImportingSWF dialog

IMPLEMENT_DYNAMIC(CDlgImportingSWF, CDialog)
CDlgImportingSWF::CDlgImportingSWF(const CString &swfFileName, CWnd* pParent /*=NULL*/)
: CDialog(CDlgImportingSWF::IDD, pParent)
, m_sSWFPath(swfFileName)
, m_bCanceled(FALSE)
{
}

CDlgImportingSWF::~CDlgImportingSWF()
{
}

void CDlgImportingSWF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgImportingSWF, CDialog)
	ON_MESSAGE(IMPORT_SWF_PROGRESS, OnProgressMessage)
END_MESSAGE_MAP()


// CDlgImportingSWF message handlers
LRESULT CDlgImportingSWF::OnProgressMessage(WPARAM wParam, LPARAM lParam)
{
	if (lParam == (LPARAM)SWFIMPORT_START)
	{
		((CProgressCtrl *)GetDlgItem(IDC_PRO_BAR))->SetRange(0, CSWFImportCache::MAX_STEP);
	}
	else if (lParam == (LPARAM)SWFIMPORT_END)
	{
		BOOL bResult = CSWFImportCache::Instance()->GetImportResult(m_sPath);		
		if (!bResult)
		{
			if (!m_bCanceled)
			{
				CString strError = CSWFImportCache::Instance()->GetErrorMessage();
				if (strError.GetLength() > 0)
					AfxMessageBox(strError);
			}			
		}
		EndDialog(bResult ? IDOK : IDABORT);
	}
	else
	{
		((CProgressCtrl *)GetDlgItem(IDC_PRO_BAR))->SetPos((int)lParam);
	}

	return 0;
}

BOOL WINAPI CDlgImportingSWF::ImportSWFCallback(int rate, LPARAM lParam)
{
	CDlgImportingSWF *me = (CDlgImportingSWF *)lParam;
	
	::PostMessage(me->m_hWnd, IMPORT_SWF_PROGRESS, 0, (LPARAM)rate);
	
	return !me->m_bCanceled;
}

BOOL CDlgImportingSWF::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString sTips = m_sSWFPath;
	CWnd *pStatic = GetDlgItem(IDC_STATIC_TIPS);
	CClientDC dc(this);
	CFont *pFont = GetFont();
	if (pFont)
	{
		CFont *pOldFont = dc.SelectObject(pFont);

		CRect rc;
		pStatic->GetClientRect(&rc);

		::PathCompactPath(dc.m_hDC, sTips.GetBuffer(), rc.Width());
		sTips.ReleaseBuffer();

		dc.SelectObject(pOldFont);

	}
	pStatic->SetWindowText(sTips);

	CSWFImportCache::Instance()->SetCachable((COptions::Instance()->m_bCacheSWFResources != FALSE));
	CSWFImportCache::Instance()->SetCallback(ImportSWFCallback, (LPARAM)this);

	BOOL bRet = CSWFImportCache::Instance()->Import(m_sSWFPath);
	if (!bRet)
	{
		CSWFImportCache::Instance()->SetCallback(NULL, 0);
		CString strError = CSWFImportCache::Instance()->GetErrorMessage();
		if (strError.GetLength() > 0)
			AfxMessageBox(strError);
		EndDialog(IDABORT);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImportingSWF::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CDlgImportingSWF::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	m_bCanceled = TRUE;
	//CDialog::OnCancel();
}
