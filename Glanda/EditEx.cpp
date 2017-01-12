// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "EditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEx

CEditEx::CEditEx()
{
}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers

UINT CEditEx::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return DLGC_WANTALLKEYS;

	//return CEdit::OnGetDlgCode();
}

void CEditEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar)
	{
	case VK_RETURN:
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_KEYRETURN), (LPARAM)m_hWnd);
		return;

	case VK_ESCAPE:
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_KEYESCAPE), (LPARAM)m_hWnd);
		return;
	
	case VK_TAB:
		::SendMessage(::GetParent(m_hWnd), WM_NEXTDLGCTL, 0, 0);
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	// to disable the "beep"
	switch(nChar)
	{
	case VK_RETURN:
	case VK_ESCAPE:
		return;
	default:
		break;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
