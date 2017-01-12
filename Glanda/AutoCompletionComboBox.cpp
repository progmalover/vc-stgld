// AutoCompletionComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompletionComboBox.h"
#include ".\autocompletioncombobox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoCompletionComboBox

CAutoCompletionComboBox::CAutoCompletionComboBox()
{
	m_bEnableAutoCompletion = TRUE;
	m_bSelCanceled = FALSE;
}

CAutoCompletionComboBox::~CAutoCompletionComboBox()
{
}


BEGIN_MESSAGE_MAP(CAutoCompletionComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDCANCEL, OnSelEndCancel)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoCompletionComboBox message handlers

void CAutoCompletionComboBox::HandleCompletion()
{
	ASSERT(m_bEnableAutoCompletion);
	if (GetCount() > 0)
	{
		// Make sure we can 'talk' to the edit control
		CEdit *pEdit = (CEdit *)GetDlgItem(1001);
		if (!pEdit)
			return;

		CString strText;
		pEdit->GetWindowText(strText);
		DWORD dwSel = pEdit->GetSel();

		int index = FindStringExact(-1, strText);
		if (index == -1)
			index = FindString(-1, strText);

		pEdit->SetRedraw(FALSE);

		// Select the matching entry in the list
		if (!GetDroppedState())
		{
			// Important! or the cursor will be invisible under Win2K or WinXP
			::SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW))); 

			ShowDropDown(TRUE);
		}

		TRACE2("%d, %d\n", GetCurSel(), index);
		if (GetCurSel() != index)
			SetCurSel(index);

		// Restore the edit control
		pEdit->SetWindowText(strText);
		pEdit->SetSel(LOWORD(dwSel), HIWORD(dwSel));

		pEdit->SetRedraw(TRUE);

		pEdit->Invalidate();
	}
}

BOOL CAutoCompletionComboBox::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (m_bEnableAutoCompletion)
	{
		if (HIWORD(wParam) == EN_CHANGE)
		{
			HandleCompletion();
			return TRUE;
		}
	}

	return CComboBox::OnCommand(wParam, lParam);
}

void CAutoCompletionComboBox::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	m_edit.SubclassDlgItem(1001, this);

	CComboBox::PreSubclassWindow();
}

void CAutoCompletionComboBox::OnSelEndCancel()
{
	// TODO: Add your control notification handler code here

	if (m_bEnableAutoCompletion)
	{
		if(m_edit.m_hWnd == NULL)
		return ;
		m_bSelCanceled = TRUE;
		m_edit.GetWindowText(m_strText);
	}
}

void CAutoCompletionComboBox::OnDropDown()
{
	if (m_bEnableAutoCompletion)
	{
		m_bSelCanceled = FALSE;
		m_strText.Empty();
	}
}

void CAutoCompletionComboBox::OnCloseUp()
{
	if (m_bEnableAutoCompletion)
	{
		if (m_bSelCanceled)
		{
			SetCurSel(-1);
			SetWindowText(m_strText);
			SetEditSel(0, -1);
		}
	}
}

void CAutoCompletionComboBox::EnableAutoCompletion(BOOL bEnable)
{
	m_bEnableAutoCompletion = bEnable;
}
