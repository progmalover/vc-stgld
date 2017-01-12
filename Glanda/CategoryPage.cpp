// CategoryPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryPage.h"
#include "VisualStylesXP.h"
#include ".\categorypage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CCategoryPage

//IMPLEMENT_DYNAMIC(CCategoryPage, CResizableDialog)
CCategoryPage::CCategoryPage()
: CResizableDialog()
{
	m_bFirstActivePage = TRUE;
}

CCategoryPage::CCategoryPage(UINT nIDTemplate, CWnd* pParentWnd /* = NULL */)
: CResizableDialog(nIDTemplate, pParentWnd)
{
	m_bFirstActivePage = TRUE;
}

CCategoryPage::CCategoryPage(LPCTSTR lpszTemplateName, CWnd* pParentWnd /* = NULL */)
: CResizableDialog(lpszTemplateName, pParentWnd)
{
	m_bFirstActivePage = TRUE;
}

CCategoryPage::~CCategoryPage()
{
}

void CCategoryPage::OnSetActive()
{
	m_bFirstActivePage = FALSE;
}

void CCategoryPage::OnKillActive()
{
	CWnd *pWnd = GetFocus();
	if (pWnd && IsChild(pWnd))
		::SetFocus(NULL);
}

BEGIN_MESSAGE_MAP(CCategoryPage, CResizableDialog)
END_MESSAGE_MAP()

BOOL CCategoryPage::InitPage(CWnd* pParentWnd)
{
	if (Create(m_lpszTemplateName, pParentWnd))
	{
		if (g_xpStyle.IsAppThemed())
			g_xpStyle.EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);
		return TRUE;
	}
	return FALSE;
}

// CCategoryPage message handlers


void CCategoryPage::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

//	CResizableDialog::OnOK();
}

void CCategoryPage::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

//	CResizableDialog::OnCancel();
}

BOOL CCategoryPage::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	//return CResizableDialog::PreTranslateMessage(pMsg);
	VERIFY(!CWnd::PreTranslateMessage(pMsg));
	
	// return FALSE to let parent handle it. At last 
	// CSizingControlBar(CControlBar) will handle it.
	return FALSE;	
}
