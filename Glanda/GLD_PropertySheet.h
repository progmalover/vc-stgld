#pragma once

#include "..\CommonDef\CommonDef.h"
#include <vector>

// CGLD_PropertySheet dialog

class CGLD_PropertySheet : public CDialog
{
	DECLARE_DYNAMIC(CGLD_PropertySheet)

	class CPageData
	{
	public:
		CPageData(REFIID clsidPage)
		{
			clsid = clsidPage;
			pIPage.CoCreateInstance(clsid);
			if (pIPage)
			{
				LPOLESTR pszTitle = NULL;
				if (SUCCEEDED(pIPage->GetTitle(&pszTitle)))
				{
					USES_CONVERSION;
					strTitle = OLE2A(pszTitle);
					::CoTaskMemFree(pszTitle);
				}
				pIPage->GetSize(&size);
			}

			bCreated = FALSE;
		}

	public:
		GUID clsid;
		CComPtr<IGLD_PropertyPage> pIPage;
		CString strTitle;
		CSize size;
		BOOL bCreated;
	};

	typedef std::vector <CPageData *> PAGEDATA_LIST;

public:
	CGLD_PropertySheet(CWnd* pParent, int x, int y, LPCTSTR lpszCaption, IUnknown *pUnknown, UINT nPages, CLSID *pPageCLSID);   // standard constructor
	virtual ~CGLD_PropertySheet();

// Dialog Data
	enum { IDD = IDD_GLD_PROPERTYSHEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CTabCtrl m_tab;

	CString m_strCaption;
	CPoint m_point;
	CComPtr<IUnknown> m_pIUnknown;	// Object
	PAGEDATA_LIST m_pageList;		// Page IDs
	CSize m_sizeMax;

	int AddPage(REFIID clsid);
	BOOL SetActive(int index);

	int m_nActive;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	BOOL ApplyPage(int index);

	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

public:
	int m_nActiveInitial;
};

BOOL CreatePropertySheet(HWND hwndParent, int x, int y, LPCTSTR lpszCaption, IUnknown *pUnknown, UINT nPages, CLSID *pPageCLSID, int nActivePage, BOOL bModal);
