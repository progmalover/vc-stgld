// GLD_PropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "GLD_PropertySheet.h"

BOOL CreatePropertySheet(HWND hwndParent, int x, int y, LPCTSTR lpszCaption, IUnknown *pUnknown, UINT nPages, CLSID *pPageCLSID, int nActivePage, BOOL bModal)
{
	CGLD_PropertySheet dlg(CWnd::FromHandle(hwndParent), x, y, lpszCaption, pUnknown, nPages, pPageCLSID);
	dlg.m_nActiveInitial = nActivePage;	

	if (bModal)
	{
		return (dlg.DoModal() == IDOK);
	}
	else
	{
		if (!dlg.Create(IDD_GLD_PROPERTYSHEET, CWnd::FromHandle(hwndParent)))
			return FALSE;
		dlg.ShowWindow(SW_SHOW);
		return TRUE;
	}
}

// CGLD_PropertySheet dialog

IMPLEMENT_DYNAMIC(CGLD_PropertySheet, CDialog)
CGLD_PropertySheet::CGLD_PropertySheet(CWnd* pParent, int x, int y, LPCTSTR lpszCaption, IUnknown *pUnknown, UINT nPages, CLSID *pPageCLSID)
	: CDialog(CGLD_PropertySheet::IDD, pParent)
{
	m_sizeMax.cx = 0;
	m_sizeMax.cy = 0;

	m_point.x = x;
	m_point.y = y;
	m_strCaption = lpszCaption;
	m_pIUnknown = pUnknown;

	for (int i = 0; i < nPages; i++)
		AddPage(pPageCLSID[i]);

	m_nActiveInitial = 0;
	m_nActive = -1;
}

CGLD_PropertySheet::~CGLD_PropertySheet()
{
	for (int i = 0; i < m_pageList.size(); i++)
		delete m_pageList[i];
}

void CGLD_PropertySheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tab);
}


BEGIN_MESSAGE_MAP(CGLD_PropertySheet, CDialog)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, OnTcnSelchangingTab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CGLD_PropertySheet message handlers

int CGLD_PropertySheet::AddPage(REFIID clsid)
{
	CPageData *pData = new CPageData(clsid);

	if (pData->pIPage)
	{
		m_sizeMax.cx = max(m_sizeMax.cx, pData->size.cx);
		m_sizeMax.cy = max(m_sizeMax.cy, pData->size.cy);

		m_pageList.push_back(pData);

		return (int)m_pageList.size() - 1;
	}

	LPOLESTR psz = NULL;
	if (SUCCEEDED(::StringFromCLSID(clsid, &psz)))
	{
		USES_CONVERSION;
		CString str = OLE2A(psz);
		::CoTaskMemFree(psz);
		AfxMessageBoxEx(MB_OK, IDS_FAILED_INIT_PAGE_1, str);
	}

	return -1;
}

BOOL CGLD_PropertySheet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowText(m_strCaption);

	for (int i = 0; i < m_pageList.size(); i++)
	{
		CPageData *pData = m_pageList[i];
		m_tab.InsertItem(i, pData->strTitle, -1);
	}

	CRect rcClient, rcTab, rcOK, rcCancel;
	GetClientRect(&rcClient);

	m_tab.GetWindowRect(&rcTab);
	ScreenToClient(&rcTab);

	// Get initial margins of tab
	int lTab = rcTab.left - rcClient.left;
	int tTab = rcTab.top - rcClient.top;
	int rTab = rcClient.right - rcTab.right;
	int bTab = rcClient.bottom - rcTab.bottom;

	CWnd *pWndOK = GetDlgItem(IDOK);
	pWndOK->GetWindowRect(&rcOK);
	ScreenToClient(&rcOK);
	int rOK = rcClient.right - rcOK.right;
	int bOK = rcClient.bottom - rcOK.bottom;

	CWnd *pWndCancel = GetDlgItem(IDCANCEL);
	pWndCancel->GetWindowRect(&rcCancel);
	ScreenToClient(&rcCancel);
	int rCancel = rcClient.right - rcCancel.right;
	int bCancel = rcClient.bottom - rcCancel.bottom;


	rcTab.left = 0;
	rcTab.top = 0;
	rcTab.right = m_sizeMax.cx;
	rcTab.bottom = m_sizeMax.cy;

	m_tab.AdjustRect(TRUE, &rcTab);

	CRect rcWin = rcTab;
	rcWin.InflateRect(lTab, tTab, rTab, bTab);

	AdjustWindowRect(&rcWin, GetStyle(), FALSE);

	CWnd *pParent = GetParent();
	if (pParent == NULL)
		pParent = GetDesktopWindow();

	CRect rcParent;
	pParent->GetWindowRect(&rcParent);

	CPoint point = rcParent.CenterPoint();
	point.x -= rcWin.Width() / 2;
	point.y -= rcWin.Height() / 2;

	int x = (m_point.x == CW_USEDEFAULT ? point.x : m_point.x);
	int y = (m_point.y == CW_USEDEFAULT ? point.y : m_point.y);

	SetWindowPos(NULL, x, y, rcWin.Width(), rcWin.Height(), SWP_NOZORDER);
	
	GetClientRect(&rcClient);

	m_tab.SetWindowPos(NULL, lTab, tTab, rcTab.Width(), rcTab.Height(), SWP_NOZORDER);

	pWndOK->SetWindowPos(NULL, rcClient.Width() - rOK - rcOK.Width(), rcClient.Height() - bOK - rcOK.Height(), rcOK.Width(), rcOK.Height(), SWP_NOZORDER);

	pWndCancel->SetWindowPos(NULL, rcClient.Width() - rCancel - rcCancel.Width(), rcClient.Height() - bCancel - rcCancel.Height(), rcCancel.Width(), rcCancel.Height(), SWP_NOZORDER);

	if (m_pageList.size() > 0)
	{
		if (m_nActiveInitial >= m_pageList.size())
			m_nActiveInitial = 0;

		m_tab.SetCurSel(m_nActiveInitial);
		SetActive(m_nActiveInitial);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGLD_PropertySheet::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	int index = m_tab.GetCurSel();
	if (index >= 0 && index < m_pageList.size())
	{
		CPageData *pDataOld = m_pageList[index];
		ASSERT (pDataOld->pIPage != NULL && pDataOld->bCreated);

		if (SUCCEEDED(pDataOld->pIPage->Activate(FALSE)))
		{
			if (SUCCEEDED(pDataOld->pIPage->Enable(FALSE)))
			{
				if (SUCCEEDED(pDataOld->pIPage->Show(FALSE)))
				{
					*pResult = 0;
					return;
				}
			}
		}
	}

	*pResult = 1;
}

void CGLD_PropertySheet::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	int index = m_tab.GetCurSel();
	if (index >= 0 && index < m_pageList.size())
	{
		if (!SetActive(index))
		{
			SetActive(m_nActive);
			*pResult = 1;
			return;
		}
	}

	*pResult = 0;
}

BOOL CGLD_PropertySheet::SetActive(int index)
{
	#ifdef CheckResult
		#undef CheckResult
	#endif

	#define CheckResult() if (FAILED(hr)) return FALSE;

	ASSERT(index >= 0 && index < m_pageList.size());
	if (index >= 0 && index < m_pageList.size())
	{
		HRESULT hr = E_FAIL;

		//if (m_nActive >= 0 && m_nActive < m_pageList.size())
		//{
		//	CPageData *pDataOld = m_pageList[m_nActive];
		//	ASSERT (pDataOld->pIPage != NULL && pDataOld->bCreated);

		//	hr = pDataOld->pIPage->Activate(FALSE);
		//	CheckResult();

		//	hr = pDataOld->pIPage->Enable(FALSE);
		//	CheckResult();

		//	hr = pDataOld->pIPage->Show(FALSE);
		//	CheckResult();
		//}

		CPageData *pDataNew = m_pageList[index];
		
		if (!pDataNew->bCreated)
		{
			ASSERT (pDataNew->pIPage != NULL);

			hr = pDataNew->pIPage->SetObject(m_pIUnknown);
			CheckResult();

			CRect rc;
			m_tab.GetWindowRect(&rc);
			m_tab.AdjustRect(FALSE, &rc);

			ScreenToClient(&rc);

			hr = pDataNew->pIPage->Create(m_hWnd, &rc);
			CheckResult();

			pDataNew->bCreated = TRUE;
		}

		hr = pDataNew->pIPage->Activate(TRUE);
		CheckResult();

		hr = pDataNew->pIPage->Enable(TRUE);
		CheckResult();

		hr = pDataNew->pIPage->Show(TRUE);
		CheckResult();

		if (::GetFocus() != m_tab.m_hWnd)
			pDataNew->pIPage->SetFocus();

		m_nActive = index;

		if (m_tab.GetCurSel() != index)
			m_tab.SetCurSel(index);

		return TRUE;
	}

	return FALSE;
}

BOOL CGLD_PropertySheet::ApplyPage(int index)
{
	ASSERT(index >= 0);

	CPageData *pData = m_pageList[index];
	if (pData->bCreated)
	{
		ASSERT(pData->pIPage);
		return (SUCCEEDED(pData->pIPage->Apply()));
	}

	// Page not create, just return TRUE
	return TRUE;
}

void CGLD_PropertySheet::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	// Apply active page
	int index = m_tab.GetCurSel();
	ASSERT(index >= 0);
	if (index >= 0)
	{
		if (!ApplyPage(index))
			return;
	}

	// Apply other pages
	for (int i = 0; i < m_pageList.size(); i++)
	{
		if (i != index)
		{
			if (!ApplyPage(i))
				return;
		}	
	}

	CDialog::OnOK();
}
