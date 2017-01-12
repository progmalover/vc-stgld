// DlgToolOption.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgToolOption.h"
#include "VisualStylesXP.h"
#include ".\dlgtooloption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgToolOption dialog

CDlgToolOption::CDlgToolOption(UINT nIDD, CWnd* pParent)
	: CResizableToolTipDialog(nIDD, pParent)
{
}

CDlgToolOption::~CDlgToolOption()
{
}

void CDlgToolOption::DoDataExchange(CDataExchange* pDX)
{
	CResizableToolTipDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgToolOption, CResizableToolTipDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CDlgToolOption message handlers

void CDlgToolOption::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CResizableToolTipDialog::OnLButtonDown(nFlags, point);

	CWnd *pParent = GetParent();

	ClientToScreen(&point);
	pParent->ScreenToClient(&point);
	pParent->SendMessage(WM_LBUTTONDOWN, (WPARAM)MK_LBUTTON, MAKELPARAM(point.x, point.y));
}

void CDlgToolOption::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CResizableToolTipDialog::OnLButtonDblClk(nFlags, point);

	CWnd *pParent = this;
	while (pParent->GetStyle() & WS_CHILD)
	{
		pParent = pParent->GetParent();
		if (pParent->IsKindOf(RUNTIME_CLASS(CControlBar)))
		{
			ClientToScreen(&point);
			pParent->ScreenToClient(&point);
			pParent->SendMessage(WM_LBUTTONDBLCLK, (WPARAM)MK_LBUTTON, MAKELPARAM(point.x, point.y));
			break;
		}
	}
}

BOOL CDlgToolOption::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	//return CResizableToolTipDialog::PreTranslateMessage(pMsg);

	VERIFY(!CWnd::PreTranslateMessage(pMsg));
	
	// return FALSE to let parent handle it. At last 
	// CSizingControlBar(CControlBar) will handle it.
	return FALSE;	
}

void CDlgToolOption::OnOK()
{
}

void CDlgToolOption::OnCancel()
{
}

