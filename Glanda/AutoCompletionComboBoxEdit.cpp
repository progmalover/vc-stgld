// AutoCompletionComboBoxEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompletionComboBoxEdit.h"
#include ".\autocompletioncomboboxedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CAutoCompletionComboBoxEdit

IMPLEMENT_DYNAMIC(CAutoCompletionComboBoxEdit, CEdit)
CAutoCompletionComboBoxEdit::CAutoCompletionComboBoxEdit()
{
}

CAutoCompletionComboBoxEdit::~CAutoCompletionComboBoxEdit()
{
}


BEGIN_MESSAGE_MAP(CAutoCompletionComboBoxEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CAutoCompletionComboBoxEdit message handlers


void CAutoCompletionComboBoxEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar)
	{
		case VK_RETURN:
		{
			CComboBox *pCombo = (CComboBox *)GetParent();
			ASSERT(pCombo);
			if (pCombo->GetDroppedState())
			{
				CString strText;
				GetWindowText(strText);			
				DWORD dwSel = GetSel();

				SetRedraw(FALSE);

				pCombo->ShowDropDown(FALSE);

				SetWindowText(strText);
				SetSel(LOWORD(dwSel), HIWORD(dwSel));

				SetRedraw(TRUE);

				return;
			}
		}

		case VK_UP:
		case VK_DOWN:
		{
			CComboBox *pCombo = (CComboBox *)GetParent();
			ASSERT(pCombo);
			if (pCombo->GetDroppedState())
			{	
				if (pCombo->GetCount() == 1 && pCombo->GetCurSel() == 0)
				{
					CString strText;
					pCombo->GetLBText(0, strText);			
					SetWindowText(strText);
					SetSel(0, -1);

					return;
				}
			}
		}

		default:
			break;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAutoCompletionComboBoxEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	// disable beep
	if (nChar == VK_RETURN || nChar == VK_TAB)
		return;

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
