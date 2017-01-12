// PagePublishHTMLOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishHTMLOptions.h"
#include "SheetPublish.h"
#include "gldDataKeeper.h"
#include "GlandaDoc.h"
#include "Graphics.h"

// CPagePublishHTMLOptions dialog

IMPLEMENT_DYNAMIC(CPagePublishHTMLOptions, CPropertyPageFixed)
CPagePublishHTMLOptions::CPagePublishHTMLOptions()
	: CPropertyPageFixed(CPagePublishHTMLOptions::IDD)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_HTML_OPTIONS, 0);
}

CPagePublishHTMLOptions::~CPagePublishHTMLOptions()
{
}

void CPagePublishHTMLOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_cmbQuality);
	DDX_Control(pDX, IDC_COMBO_WMODE, m_cmbWmode);
	DDX_Control(pDX, IDC_COMBO_SCALE, m_cmbScale);
	DDX_Control(pDX, IDC_COMBO_FLASH_ALIGN, m_cmbFlashAlign);
	DDX_Control(pDX, IDC_COMBO_HTML_ALIGN, m_cmbHTMLAlign);
}


BEGIN_MESSAGE_MAP(CPagePublishHTMLOptions, CPropertyPageFixed)
END_MESSAGE_MAP()


// CPagePublishHTMLOptions message handlers

BOOL CPagePublishHTMLOptions::OnInitDialog()
{
	CPropertyPageFixed::OnInitDialog();

	// TODO:  Add extra initialization here

	struct INIT_COMBOBOX
	{
		UINT nID;
		UINT nIDList;
		TCHAR *szSection;
		int nDefSel;
	}ic[] = 
	{
		{IDC_COMBO_QUALITY, IDS_LIST_FLASH_QUALITY, "quality", 4}, 
		{IDC_COMBO_WMODE, IDS_LIST_FLASH_WMODE, "wmode", 0}, 
		{IDC_COMBO_SCALE, IDS_LIST_FLASH_SCALE, "scale", 0}, 
		{IDC_COMBO_FLASH_ALIGN, IDS_LIST_FLASH_FLASH_ALIGN, "salign", 4}, 
		{IDC_COMBO_HTML_ALIGN, IDS_LIST_FLASH_HTML_ALIGN, "align", 0}, 
	};

	for (int i = 0; i < sizeof(ic) / sizeof(INIT_COMBOBOX); i++)
	{
		CComboBox *pCombo = (CComboBox *)GetDlgItem(ic[i].nID);
		FillComboBox(pCombo, ic[i].nIDList);
		int nSel = AfxGetApp()->GetProfileInt("Publish\\HTML", ic[i].szSection, ic[i].nDefSel);
		if (nSel < 0 || nSel > pCombo->GetCount() - 1)
			nSel = ic[i].nDefSel;
		pCombo->SetCurSel(nSel);
	}

	BOOL bPause = AfxGetApp()->GetProfileInt("Publish\\HTML", "pause", FALSE);
	BOOL bLoop = AfxGetApp()->GetProfileInt("Publish\\HTML", "loop", TRUE);
	BOOL bDeviceFont = AfxGetApp()->GetProfileInt("Publish\\HTML", "devicefont", FALSE);
	BOOL bMenu = AfxGetApp()->GetProfileInt("Publish\\HTML", "menu", TRUE);

	CheckDlgButton(IDC_CHECK_PAUSE, bPause ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_LOOP, bLoop ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_DEVICEFONT, bDeviceFont ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHECK_MENU, bMenu ? BST_CHECKED : BST_UNCHECKED);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPagePublishHTMLOptions::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return CPropertyPageFixed::OnSetActive();
}

LRESULT CPagePublishHTMLOptions::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	return IDD_PUBLISH_SELECT;
}

LRESULT CPagePublishHTMLOptions::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	GenerateHTML();

	CSheetPublish *pSheet = (CSheetPublish *)GetParent();
	switch (pSheet->m_pageSelect.m_nChoice)
	{
	case PUBLISH_INSERT_CODE:
		return IDD_PUBLISH_SELECT_HTML;
	case PUBLISH_VIEW_CODE:
		return IDD_PUBLISH_VIEW_CODE;
	}

	return -1;
}

void CPagePublishHTMLOptions::GenerateHTML()
{
	int nQuality = ((CComboBox *)GetDlgItem(IDC_COMBO_QUALITY))->GetCurSel();
	int nWmode = ((CComboBox *)GetDlgItem(IDC_COMBO_WMODE))->GetCurSel();
	int nScale = ((CComboBox *)GetDlgItem(IDC_COMBO_SCALE))->GetCurSel();
	int nFlashAlign = ((CComboBox *)GetDlgItem(IDC_COMBO_FLASH_ALIGN))->GetCurSel();
	int nHTMLAlign = ((CComboBox *)GetDlgItem(IDC_COMBO_HTML_ALIGN))->GetCurSel();

	BOOL bPause = IsDlgButtonChecked(IDC_CHECK_PAUSE) == BST_CHECKED;
	BOOL bLoop = IsDlgButtonChecked(IDC_CHECK_LOOP) == BST_CHECKED;
	BOOL bDeviceFont = IsDlgButtonChecked(IDC_CHECK_DEVICEFONT) == BST_CHECKED;
	BOOL bMenu = IsDlgButtonChecked(IDC_CHECK_MENU) == BST_CHECKED;

	AfxGetApp()->WriteProfileInt("Publish\\HTML", "quality", nQuality);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "wmode", nWmode);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "scale", nScale);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "salign", nFlashAlign);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "align", nHTMLAlign);

	AfxGetApp()->WriteProfileInt("Publish\\HTML", "pause", bPause);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "loop", bLoop);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "devicefont", bDeviceFont);
	AfxGetApp()->WriteProfileInt("Publish\\HTML", "menu", bMenu);

	const TCHAR *szQuality[] = 
	{
		"low",
		"autolow",
		"autohigh",
		"medium",
		"high",
		"best"
	};

	const TCHAR *szWmode[] = 
	{
		"", 
		"opaque", 
		"transparent"
	};

	const TCHAR *szScale[] = 
	{
		"",
		"noborder",
		"exactfit",
		"noscale"
	};

	const TCHAR *szHTMLAlign[] = 
	{
		"middle",
		"left",
		"top",
		"right",
		"bottom"
	};

	const TCHAR *szFlashAlign[] = 
	{
		"lt",
		"l",
		"lb",
		"t",
		"",
		"b",
		"rt",
		"r",
		"rb"
	};


	// code template
	const TCHAR szHTML[] =
		"<object classid=\"clsid:d27cdb6e-ae6d-11cf-96b8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0\" width=\"%d\" height=\"%d\" id=\"%s\" align=\"%s\">\r\n"
		"	<param name=\"allowScriptAccess\" value=\"sameDomain\">\r\n"
		"	<param name=\"movie\" value=\"%s\">\r\n"
		"	<param name=\"quality\" value=\"%s\">\r\n"
		"	<param name=\"bgcolor\" value=\"%s\">\r\n"
		"%s"
		"	<embed src=\"%s\" quality=\"%s\" bgcolor=\"%s\" width=\"%d\" height=\"%d\" name=\"%s\" align=\"%s\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\">\r\n"
		"</object>\r\n";

	gldMainMovie2 *pMovie = _GetMainMovie2();
	CString strMovie = _GetCurDocument()->GetExportMovieName();
	CString strColor = RGBToString(pMovie->m_color);

	CString strExtra;
	CString strParam;

	// wmode
	if (_tcslen(szWmode[nWmode]) > 0)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "wmode", szWmode[nWmode]);
		strExtra += strParam;
	}

	// scale
	if (_tcslen(szScale[nScale]) > 0)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "scale", szScale[nScale]);
		strExtra += strParam;
	}

	// FlashAlign
	if (_tcslen(szFlashAlign[nFlashAlign]) > 0)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "salign", szFlashAlign[nFlashAlign]);
		strExtra += strParam;
	}


	// pause at start
	if (bPause)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "play", "false");
		strExtra += strParam;
	}

	// display menu
	if (!bMenu)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "menu", "false");
		strExtra += strParam;
	}

	// loop
	if (!bLoop)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "loop", "false");
		strExtra += strParam;
	}

	// device font
	if (bDeviceFont)
	{
		strParam.Format("	<param name=\"%s\" value=\"%s\">\r\n", "devicefont", "true");
		strExtra += strParam;
	}

	m_strHTML.Format(szHTML, 
		pMovie->m_width, pMovie->m_height, (LPCTSTR)strMovie, szHTMLAlign[nHTMLAlign], 
		(LPCTSTR)strMovie,
		szQuality[nQuality], 
		(LPCTSTR)strColor,
		(LPCTSTR)strExtra,
		(LPCTSTR)strMovie, szQuality[nQuality], (LPCTSTR)strColor, pMovie->m_width, pMovie->m_height, (LPCTSTR)strMovie, szHTMLAlign[nHTMLAlign]);
}
