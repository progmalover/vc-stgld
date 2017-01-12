// HexColorEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "HexColorEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CHexColorEdit

IMPLEMENT_DYNAMIC(CHexColorEdit, CEdit)
CHexColorEdit::CHexColorEdit()
{
	m_bInternalSetWindowText = FALSE;
}

CHexColorEdit::~CHexColorEdit()
{
}


BEGIN_MESSAGE_MAP(CHexColorEdit, CEdit)
	ON_CONTROL_REFLECT(EN_UPDATE, OnEnUpdate)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(EM_SETSEL, OnSetSel)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CHexColorEdit message handlers

LRESULT CHexColorEdit::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (!m_bInternalSetWindowText)
	{
		TCHAR *pText = (TCHAR *)lParam;
		if (pText)
		{
			m_strText = pText;
			m_dwSel = GetSel();
		}
	}

	return DefWindowProc(WM_SETTEXT, wParam, lParam);
}

LRESULT CHexColorEdit::OnSetSel(WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(EM_SETSEL, wParam, lParam);
}

void CHexColorEdit::OnEnUpdate()
{
	if (m_bInternalSetWindowText)
		return;

	CString strText;
	GetWindowText(strText);
	int len = strText.GetLength();
	if (len > 0)
	{
		int nStart, nEnd;	
		GetSel(nStart, nEnd);

		int nNewLen = 0;
		if (strText[0] == '#' && len > 7)
			nNewLen = 7;
		else 
		if (strText[0] != '#' && len > 6)
			nNewLen = 6;

		if (nNewLen != 0)
			strText = strText.Left(nNewLen);

		BOOL bValid = ValidateText(strText);
		if (!bValid)
		{
			// restore previous valid text
			MessageBeep(0);

			m_bInternalSetWindowText = TRUE;
			SetWindowText(m_strText);
			m_bInternalSetWindowText = FALSE;

			SetSel(m_dwSel);
			return;
		}

		if (nNewLen != 0)
		{
			// text is truncated, may need to re-set cursor pos
			nStart = min(strText.GetLength(), nStart);
			nEnd = max(nStart, nEnd);

			m_bInternalSetWindowText = TRUE;
			SetWindowText(strText);
			m_bInternalSetWindowText = FALSE;

			SetSel(nStart, nEnd);
		}
	}

	m_strText = strText;
	m_dwSel = GetSel();
}

BOOL CHexColorEdit::ValidateText(LPCTSTR lpszText)
{
	int len = _tcslen(lpszText);

	if (len > 0)
	{
		if ((lpszText[0] == '#' && len > 7) ||  (lpszText[0] != '#' && len > 6))
			return FALSE;

		for (int i = 0; i < len; i++)
		{
			BYTE nChar = lpszText[i];
			if ((i == 0 && nChar == '#') ||
				nChar >= '0' && nChar <= '9' || 
				nChar >= 'A' && nChar <= 'F' ||
				nChar >= 'a' && nChar <= 'f')
			{
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void CHexColorEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

	m_dwSel = GetSel();
}

void CHexColorEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CEdit::OnLButtonDown(nFlags, point);

	m_dwSel = GetSel();
}
