// DlgTemplates.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgTemplates.h"
#include ".\dlgtemplates.h"

#include "../Glanda/TemplateCategory.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IDC_TEMPLATE_LIST 1000

// CDlgTemplates dialog

CDlgTemplates::CDlgTemplates(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgTemplates::IDD, pParent)
{
}

CDlgTemplates::~CDlgTemplates()
{
}

void CDlgTemplates::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTemplates, CResizableDialog)
	ON_WM_DESTROY()
	ON_CONTROL(FLCN_DBLCLK, IDC_TEMPLATE_LIST, OnDblclkList)
END_MESSAGE_MAP()


// CDlgTemplates message handlers

BOOL CDlgTemplates::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CRect rc;
	GetDlgItem(IDC_STATIC_LIST)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_list.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rc, this, IDC_TEMPLATE_LIST);
	m_list.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

	m_list.CalcItemSize();

	AddAnchor(IDC_TEMPLATE_LIST, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_CHECK_SHOW_AT_STARTUP, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(IDOK, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT, BOTTOM_RIGHT);

	if (!LoadTemplates())
		OnCancel();

	m_list.SetFocus();

	BOOL bShow = AfxGetApp()->GetProfileInt("Templates", "Show at Startup", TRUE);
	CheckDlgButton(IDC_CHECK_SHOW_AT_STARTUP, bShow ? BST_CHECKED : BST_UNCHECKED);

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CompareTempaltes(gldTemplateInfo *info1, gldTemplateInfo *info2)
{
	if (info1->dwPriority == info2->dwPriority)
	{
		return info1->strName.CompareNoCase(info2->strName) < 0;
	}
	else
	{
		return info1->dwPriority < info2->dwPriority;
	}
}

BOOL CDlgTemplates::LoadTemplates()
{
	HKEY hCat;
	DWORD dwRet = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, "CLSID\\{E23DBDE5-A854-4a37-8A92-FA43F524F1D3}", 0, KEY_READ, &hCat);
	if (dwRet != ERROR_SUCCESS)
	{
		::SetLastError(dwRet);
		::ReportLastError();
		return FALSE;
	}

	CString strClsid = AfxGetApp()->GetProfileString("Templates", "Selected", "");

	GTEMPLATEINFO_LIST infoList;

	BOOL bSelected = FALSE;
	DWORD dwIndex = 0;
	TCHAR szKey[255]; // 255 is the limit of Windows 9x
	while (::RegEnumKey(hCat, dwIndex, szKey, 255) == ERROR_SUCCESS)
	{
		dwIndex++;

		CComBSTR bstrKey(szKey);
		CLSID clsid;
		if (SUCCEEDED(CLSIDFromString(bstrKey, &clsid)))
		{
			gldTemplateInfo *pInfo = new gldTemplateInfo(clsid);
			if (pInfo)
			{
				if (pInfo->LoadTemplateInfo())
				{
					pInfo->bSelected = (strClsid.CompareNoCase(szKey) == 0);	
					if (pInfo->bSelected)
						bSelected = pInfo->bSelected;
					infoList.push_back(pInfo);
				}
				else
				{
					// no memory leak if failed
					delete pInfo;
				}
			}
		}
	}

	CLSID clsid;
	memset(&clsid, 0, sizeof(clsid));
	gldTemplateInfo *pInfo = new gldTemplateInfo(clsid);
	if (pInfo)
	{
		pInfo->strName.LoadString(IDS_BLANK_DOCUMENT);
		pInfo->strDescription.LoadString(IDS_CREATE_BLANK_DOCUMENT);
		pInfo->hBitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BLANK_DOC));
		pInfo->dwPriority = 0;			// absolute priority
		pInfo->bSelected = !bSelected;	// if no template was selected, then selected the blank document
		infoList.push_back(pInfo);
	}

	::RegCloseKey(hCat);

	if (infoList.size() > 0)
	{
		std::sort(infoList.begin(), infoList.end(), CompareTempaltes);

		for (GTEMPLATEINFO_LIST::iterator it = infoList.begin(); it != infoList.end(); it++)
		{
			gldTemplateInfo *pInfo = *it;
			int index = m_list.AddTemplate(pInfo);
			if (index >= 0 && pInfo->bSelected)
				m_list.SelectItem(index);
		}

		if (m_list.GetItemCount() > 0 && m_list.GetCurSel() == -1)
			m_list.SelectItem(0, TRUE);
	}

	return TRUE;
}

void CDlgTemplates::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	int index = m_list.GetCurSel();
	if (index < 0)
	{
		AfxMessageBox(IDS_NEED_SELECT_TEMPLATE);
		return;
	}

	REFCLSID clsid = m_list.GetTemplate(index);

	m_clsid = clsid;

	BSTR bstrClsid = NULL;
	if (SUCCEEDED(::StringFromCLSID(clsid, &bstrClsid)))
	{
		USES_CONVERSION;
		AfxGetApp()->WriteProfileString("Templates", "Selected", OLE2A(bstrClsid));
		::CoTaskMemFree(bstrClsid);
	}

	CResizableDialog::OnOK();
}

void CDlgTemplates::OnDestroy()
{
	CResizableDialog::OnDestroy();

	// TODO: Add your message handler code here

	AfxGetApp()->WriteProfileInt("Templates", "Show at Startup", IsDlgButtonChecked(IDC_CHECK_SHOW_AT_STARTUP) == BST_CHECKED);
}

void CDlgTemplates::OnDblclkList()
{
	int index = m_list.GetCurSel();
	if (index >= 0)
	{
		CPoint point;
		GetCursorPos(&point);
		m_list.ScreenToClient(&point);
		if (m_list.ItemFromPoint(point) == index)
			OnOK();
	}
}
