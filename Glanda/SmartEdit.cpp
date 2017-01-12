// NumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SmartEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CSmartEdit

IMPLEMENT_DYNAMIC(CSmartEdit, CEdit)
CSmartEdit::CSmartEdit()
{
	m_bChanged = FALSE;
	m_bPrompt = FALSE;
}

CSmartEdit::~CSmartEdit()
{
}


BEGIN_MESSAGE_MAP(CSmartEdit, CEdit)
	ON_CONTROL_REFLECT(EN_UPDATE, OnEnUpdate)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()



// CSmartEdit message handlers


void CSmartEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	// TODO: Add your control notification handler code here
}

void CSmartEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	HandleModified();
}

BOOL CSmartEdit::HandleModified()
{
	if (!m_bChanged)
		return TRUE;

	BOOL ret = TRUE;
	if (!ValidateText())
	{
		SetSel(0, -1);
		if (m_bPrompt)
		{
			BOOL bSetFocus = TRUE;
			HWND hWnd = ::GetFocus();
			if (hWnd)
			{
				DWORD dwID = ::GetCurrentThreadId();
				DWORD dwIDFocus = ::GetWindowThreadProcessId(hWnd, NULL);
				if (dwID != dwIDFocus)
					bSetFocus = FALSE;
			}
			if (bSetFocus)
				SetFocus();
		}

		ret = FALSE;
	}

	SaveDefaultValue();

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_MODIFY), (LPARAM)m_hWnd);

	return ret;
}

void CSmartEdit::OnEnUpdate()
{
	m_bChanged = TRUE;
	TRACE0("m_bChanged = TRUE\n");
}

BOOL CSmartEdit::ValidateText()
{
	return TRUE;
}

void CSmartEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar)
	{
		case VK_RETURN:
		{
			if (HandleModified())
			{
				// following code will not work if the parent is PropertyPage
				CWnd *pParent = GetParent();
				LRESULT ret = pParent->SendMessage(DM_GETDEFID, 0, 0L);
				if (HIWORD(ret) == DC_HASDEFID)
				{
					UINT nIDControl = LOWORD(ret);
					HWND hWndControl = NULL;
					while (pParent)
					{
						hWndControl = ::GetDlgItem(pParent->m_hWnd, nIDControl);
						if (hWndControl)
							break;
						pParent = pParent->GetParent();
					}
					if (pParent && hWndControl)
						pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nIDControl, BN_CLICKED), (LPARAM)hWndControl);
				}
			}
			break;
		}	
		case VK_TAB:
		{
			// the edit may be used as the child window of Combo-Box etc.
			CWnd *pWnd = GetParent();
			while (pWnd && (pWnd->GetStyle() & WS_TABSTOP))
				pWnd = pWnd->GetParent();
			if (pWnd)
				pWnd->SendMessage(WM_NEXTDLGCTL, (GetKeyState(VK_SHIFT) & (1 << (sizeof(short int) * 8 - 1))) ? 1 : 0, 0);
			break;
		}
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CSmartEdit::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_HASSETSEL;
	//return CEdit::OnGetDlgCode();
}

void CSmartEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	// disable beep
	if (nChar == VK_RETURN || nChar == VK_TAB)
		return;

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

LRESULT CSmartEdit::OnSetText(WPARAM wp, LPARAM lp)
{
	LRESULT lResult = 0;

	LPCTSTR lpszNew = (LPCTSTR)lp == NULL ? "" : (LPCTSTR)lp;
	int nNewLen = lstrlen(lpszNew);
	TCHAR szOld[256];
	// fast check to see if text really changes (reduces flash in controls)
	if (nNewLen > _countof(szOld) || ::GetWindowText(m_hWnd, szOld, _countof(szOld)) != nNewLen || lstrcmp(szOld, lpszNew) != 0)
		lResult = Default();

	SaveDefaultValue();
	
	return lResult;
}

void CSmartEdit::SaveDefaultValue()
{
	m_bChanged = FALSE;
}
