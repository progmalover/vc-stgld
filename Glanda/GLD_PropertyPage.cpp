// ComPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include ".\GLD_PropertyPage.h"
#include "VisualStylesXP.h"

#include <afxpriv.h>
#include <..\src\mfc\afximpl.h>

/////////////////////////////////////////////////////////////////////////////
// CGLD_PropertyPage::CGLD_PropertyPage

IMPLEMENT_DYNAMIC(CGLD_PropertyPage, CDialog)

BEGIN_INTERFACE_MAP(CGLD_PropertyPage, CDialog)
	INTERFACE_PART(CGLD_PropertyPage, __uuidof(IGLD_PropertyPage), PropertyPage)
END_INTERFACE_MAP()

CGLD_PropertyPage::CGLD_PropertyPage(UINT nIDD, UINT nIDCaption) :
	m_nIDDialog(nIDD),
	m_nIDCaption(nIDCaption),
	m_hDialog(NULL)
{
	// m_lpDialogTemplate is needed later by CWnd::ExecuteDlgInit
	m_lpszTemplateName = MAKEINTRESOURCE(m_nIDDialog);

	Init();

	// Keep this DLL loaded at least until this object is deleted.
	AfxOleLockApp();
}

CGLD_PropertyPage::~CGLD_PropertyPage()
{
	// Remember to free the resource we loaded!
	if (m_hDialog != NULL)
		GlobalFree(m_hDialog);

	m_pIUnknown.Release();
	
	AfxOleUnlockApp();
}

void CGLD_PropertyPage::OnFinalRelease()
{
	if (m_hWnd != NULL)
		DestroyWindow();

	delete this;
}

BOOL CGLD_PropertyPage::Init()
{
	// Load title text
	if (!m_strCaption.LoadString(m_nIDCaption))
		m_strCaption.LoadString(AFX_IDS_PROPPAGE_UNKNOWN);

	// Try to load the dialog resource template into memory and get its size
	m_sizePage.cx = 0;
	m_sizePage.cy = 0;

	CDialogTemplate dt;
	if (!dt.Load(MAKEINTRESOURCE(m_nIDDialog)))
		return FALSE;

	// If no font specified, set the system font.
	BOOL bSetSysFont = !dt.HasFont();
	CString strFace;
	WORD wSize;

	// On DBCS systems, also change "MS Sans Serif" or "Helv" to system font.
	if ((!bSetSysFont) && GetSystemMetrics(SM_DBCSENABLED))
	{
		CString strFace;
		dt.GetFont(strFace, wSize);
		bSetSysFont = (strFace == _T("MS Shell Dlg") ||
			strFace == _T("MS Sans Serif") || strFace == _T("Helv"));
	}

	// Here is where we actually set the font.
	if (bSetSysFont && AfxGetPropSheetFont(strFace, wSize, FALSE))
		dt.SetFont(strFace, wSize);

	dt.GetSizeInPixels(&m_sizePage);

	m_hDialog = dt.Detach();

	return TRUE;
}

STDMETHODIMP_(ULONG) CGLD_PropertyPage::XPropertyPage::AddRef()
{
	METHOD_PROLOGUE_EX_(CGLD_PropertyPage, PropertyPage)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CGLD_PropertyPage::XPropertyPage::Release()
{
	METHOD_PROLOGUE_EX_(CGLD_PropertyPage, PropertyPage)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(CGLD_PropertyPage, PropertyPage)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::GetTitle(LPOLESTR *ppszTitle)
{
	METHOD_PROLOGUE_EX_(CGLD_PropertyPage, PropertyPage)
	LPOLESTR pszTitle = AtlAllocTaskOleString(pThis->m_strCaption);
	if (pszTitle == NULL)
		return E_OUTOFMEMORY;
	*ppszTitle = pszTitle;
	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::GetSize(SIZE *pSize)
{
	METHOD_PROLOGUE_EX_(CGLD_PropertyPage, PropertyPage)
	*pSize = pThis->m_sizePage;
	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::SetObject(IUnknown *pUnk)
{
	METHOD_PROLOGUE_EX_(CGLD_PropertyPage, PropertyPage)
	pThis->m_pIUnknown = pUnk;

	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::Create(HWND hWndParent, LPCRECT pRect)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)
	ASSERT_VALID(pThis);
	ASSERT_NULL_OR_POINTER(pRect, RECT);

	BOOL bSuccess = FALSE;  // Did we successfully create the dialog box

	if (pThis->m_hDialog != NULL)
	{
		// We've already loaded the dialog template into memory so just
		// create it!

		void* lpDialogTemplate = LockResource(pThis->m_hDialog);
		if (lpDialogTemplate != NULL)
		{
			bSuccess = pThis->CreateIndirect(lpDialogTemplate, CWnd::FromHandle(hWndParent));
			UnlockResource(pThis->m_hDialog);
		}
		else
		{
			bSuccess = pThis->Create(pThis->m_nIDDialog, CWnd::FromHandle(hWndParent));
		}
	}
	else
	{
		bSuccess = pThis->Create(pThis->m_nIDDialog, CWnd::FromHandle(hWndParent));
	}

	// Were we successful in creating the dialog box!
	if (bSuccess)
	{
		pThis->ModifyStyle(WS_CAPTION | WS_DLGFRAME | WS_BORDER, WS_CHILD);
		pThis->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		pThis->MoveWindow(pRect);       // Force page to fill area given by frame *
		pThis->UpdateData(FALSE);

		return S_OK;
	}

	return E_FAIL;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::Enable(BOOL bEnable)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)
	pThis->EnableWindow(bEnable);
	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::Show(BOOL bShow)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)
	pThis->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	if (bShow)
		pThis->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::SetFocus(void)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)
	pThis->SetFocus();

	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::Move(LPCRECT pRect)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)
	pThis->MoveWindow(pRect);
	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::Activate(BOOL bActivate)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)

	if (bActivate)
	{
		if (!pThis->UpdateData(FALSE) || !pThis->OnSetActive())
			return E_FAIL;
	}
	else
	{
		if (!pThis->UpdateData(TRUE) || !pThis->OnKillActive())
			return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CGLD_PropertyPage::XPropertyPage::Apply(void)
{
	METHOD_PROLOGUE_EX(CGLD_PropertyPage, PropertyPage)
	if (!pThis->UpdateData(TRUE) || !pThis->OnApply())
		return E_FAIL;

	return S_OK;
}

BOOL CGLD_PropertyPage::OnKillActive()
{
	return TRUE;
}

BOOL CGLD_PropertyPage::OnSetActive()
{
	return TRUE;
}

BOOL CGLD_PropertyPage::OnApply()
{
	return TRUE;
}

BOOL CGLD_PropertyPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	if (g_xpStyle.IsAppThemed())
		g_xpStyle.EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
