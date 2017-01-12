// ASViewTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ASViewTabCtrl.h"

#include "ASView.h"
#include "ASDoc.h"
#include "gldAction.h"

// CASViewTabCtrl

CASViewTabCtrl::CASViewTabCtrl()
{
	m_dwExStyle = TCXS_EX_NOBORDER | TCXS_EX_TRAPEZIA | TCXS_EX_AUTOSIZE;

	SetMargin(5, 5, 3);
	SetPadding(9, 9);
}

CASViewTabCtrl::~CASViewTabCtrl()
{
	for (int i = m_arItems.GetCount() - 1; i >= 0; i--)
		delete m_arItems[i];
	m_arItems.RemoveAll();
}


BEGIN_MESSAGE_MAP(CASViewTabCtrl, CCuteTabCtrl)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CASViewTabCtrl message handlers

int CASViewTabCtrl::AddView(CASView *pView, LPCTSTR lpszCaption)
{
	CString strCaption;
	if (lpszCaption != NULL)
	{
		strCaption = lpszCaption;
	}
	else
	{
		strCaption = pView->GetDocument()->GetTitle();
	}

	pView->SetParent(this);

	return AddItem(strCaption, (LPARAM)pView);
}

CASView *CASViewTabCtrl::GetView(int index)
{
	if (index >= 0 && index < GetCount())
		return (CASView *)m_arItems[index]->m_lParam;
	return NULL;
}

void CASViewTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CCuteTabCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	for (int i = 0; i < GetCount(); i++)
	{
		CASView *pView = GetView(i);
		if (pView)
		{
			CRect rc;
			GetClientRect(&rc);
			rc.bottom -= GetItemHeight();
			pView->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
			pView->Invalidate(FALSE);	// avoid repaint problem when disabled.
		}
	}
}

BOOL CASViewTabCtrl::OnActivatePage(int nItemOld, int nItemNew)
{
	if (nItemOld != -1)
	{
		CASView *pView = GetView(nItemOld);
		if (pView)
		{
			pView->ShowWindow(FALSE);
			pView->EnableWindow(FALSE);
		}
	}

	if (nItemNew != -1)
	{
		CASView *pView = GetView(nItemNew);
		if (pView)
		{
			pView->EnableWindow(pView->m_nActionType != actionSegNone);
			pView->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
			CWnd *pWnd = GetFocus();
			if (pWnd == NULL || pWnd == this)
				if (pView->IsWindowEnabled())
					pView->SetFocus();
		}
	}

	return TRUE;
}

CASView *CASViewTabCtrl::GetActiveView()
{
	int index = GetActiveItem();
	if (index >= 0)
	{
		CASView *pView = GetView(index);
		return pView;
	}
	return NULL;
}

int CASViewTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCuteTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	ModifyStyleEx(0, WS_EX_CONTROLPARENT, SWP_FRAMECHANGED);

	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(GetVersion())));
	if (dwWindowsMajorVersion >= 5)
	{
		m_font.CreatePointFont(85, "MS Shell Dlg 2");
	}
	else
	{
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONT lf;
		::GetObject(hFont, sizeof(lf), &lf);
		m_font.CreateFontIndirect(&lf);
	}

	return 0;
}

void CASViewTabCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CCuteTabCtrl::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	CASView *pView = GetActiveView();
	if (pView)
		pView->SetFocus();
}
