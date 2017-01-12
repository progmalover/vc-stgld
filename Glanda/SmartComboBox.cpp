// SmartComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartComboBox.h"
#include "afxpriv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSmartComboBox

void AFXAPI DDX_CBString2(CDataExchange* pDX, int nIDC, CString& value)
{
	pDX->PrepareCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	if (pDX->m_bSaveAndValidate)
	{
		// just get current edit item text (or drop list static)
		int nLen = ::GetWindowTextLength(hWndCtrl);
		if (nLen > 0)
		{
			// get known length
			::GetWindowText(hWndCtrl, value.GetBufferSetLength(nLen), nLen+1);
		}
		else
		{
			// for drop lists GetWindowTextLength does not work - assume
			//  max of 255 characters
			::GetWindowText(hWndCtrl, value.GetBuffer(255), 255+1);
		}
		value.ReleaseBuffer();
	}
	else
	{
		// set current selection based on model string
		// just set the edit text (will be ignored if DROPDOWNLIST)
		AfxSetWindowText(hWndCtrl, value);
	}
}

IMPLEMENT_DYNAMIC(CSmartComboBox, CComboBox)
CSmartComboBox::CSmartComboBox()
: m_strText(_T(""))
{
}

CSmartComboBox::~CSmartComboBox()
{
}


BEGIN_MESSAGE_MAP(CSmartComboBox, CComboBox)
	ON_CONTROL_REFLECT_EX(CBN_SETFOCUS, OnCbnSetfocus)
	ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, OnCbnSelchange)
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnCbnCloseup)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()



// CSmartComboBox message handlers


BOOL CSmartComboBox::IsDropDownList()
{
	return (GetStyle() & 0x03) == CBS_DROPDOWNLIST;
}

BOOL CSmartComboBox::OnCbnSetfocus()
{
	// TODO: Add your control notification handler code here

	if (!IsDropDownList())
	{
	#ifdef _DEBUG
		CString strText;
		GetWindowText(strText);	
		ASSERT(m_strText == strText);
	#endif

		GetWindowText(m_strText);
	}

	return FALSE;
}

BOOL CSmartComboBox::OnCbnSelchange()
{
	// TODO: Add your control notification handler code here

	if (!IsDropDownList())
	{
		int index = GetCurSel();
		if (index >= 0)
		{
			CString strText;
			GetLBText(index, strText);
			if (m_strText.Compare(strText) != 0)
			{
				m_strText = strText;
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_MODIFY), (LPARAM)m_hWnd);
			}
		}
	}
	return FALSE;
}

BOOL CSmartComboBox::OnCbnCloseup()
{
	// TODO: Add your control notification handler code here
	
	if (!IsDropDownList())
	{
		CString strText;
		GetWindowText(strText);	
		if (m_strText.Compare(strText) != 0)
		{
			m_strText = strText;
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_MODIFY), (LPARAM)m_hWnd);
		}
	}
	return FALSE;
}

BOOL CSmartComboBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if (!IsDropDownList())
	{
		switch (HIWORD(wParam))
		{
			case EN_CHANGE:
				GetWindowText(m_strText);
				return 0;
		
			case EN_MODIFY:
				GetWindowText(m_strText);
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_MODIFY), (LPARAM)m_hWnd);
				return 0;
		}
	}

	return CComboBox::OnCommand(wParam, lParam);
}

void CSmartComboBox::GetText(CString & strText)
{
	strText = m_strText;
}

LRESULT CSmartComboBox::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (IsDropDownList())
		return Default();

	LRESULT lResult = 0;

	LPCTSTR lpszNew = (LPCTSTR)lParam;
	int nNewLen = lstrlen(lpszNew);
	TCHAR szOld[256];
	// fast check to see if text really changes (reduces flash in controls)
	if (nNewLen > _countof(szOld) || ::GetWindowText(m_hWnd, szOld, _countof(szOld)) != nNewLen || lstrcmp(szOld, lpszNew) != 0)
		lResult = Default();

	GetWindowText(m_strText);
	return lResult;
}

void CSmartComboBox::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!IsDropDownList())
	{
		HWND hWnd = ::GetDlgItem(m_hWnd, 1001);
		if (!FromHandlePermanent(hWnd))
			VERIFY(m_edit.SubclassWindow(hWnd));

		m_strText.Empty();
	}

	CComboBox::PreSubclassWindow();
}

int GetComboBoxMaxItemWidth(CComboBox *pCB)
{
	CClientDC dc(pCB);

	CFont *pFont = pCB->GetFont();
	CFont *pOldFont = NULL;
	if (pFont)
		pOldFont = dc.SelectObject(pFont);

	// should not be narrower than the default width
	CRect rc;
	pCB->GetWindowRect(&rc);
	int nWidthMax = rc.Width();
	for (int i = 0; i < pCB->GetCount(); i++)
	{
		CString strText;
		pCB->GetLBText(i, strText);
		nWidthMax = max(nWidthMax, dc.GetTextExtent(strText).cx + ::GetSystemMetrics(SM_CXVSCROLL) + 5);
	}

	if (pOldFont)
		dc.SelectObject(pOldFont);

	return nWidthMax;
}
