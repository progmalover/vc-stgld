// CategoryTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryTabCtrl.h"
#include "CategoryResourcePage.h"
#include "CategoryImportPage.h"
#include "CategoryToolsPage.h"
#include "CategoryPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CCategoryTabCtrl

IMPLEMENT_DYNAMIC(CCategoryTabCtrl, CTabCtrl)
CCategoryTabCtrl::CCategoryTabCtrl()
: m_nActivePage(-1)
{
}

CCategoryTabCtrl::~CCategoryTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CCategoryTabCtrl, CTabCtrl)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCategoryTabCtrl message handlers
void CCategoryTabCtrl::AddPage(LPCTSTR lpszItem, CCategoryPage* pPage)
{
	ASSERT(::IsWindow(m_hWnd));
	if(::IsWindow(m_hWnd))
	{
		ASSERT(!::IsWindow(pPage->m_hWnd));
		pPage->InitPage(this);

		int nLength = _tcslen(lpszItem);
		TCHAR* szBuffer = new TCHAR[nLength+1];
		_tcscpy(szBuffer, lpszItem);
		szBuffer[nLength] = _T('\0');

		TCITEM tcItem;
		memset(&tcItem, 0, sizeof(tcItem));
		tcItem.mask = TCIF_TEXT | TCIF_PARAM;
		tcItem.pszText = szBuffer;
		tcItem.lParam = (LPARAM)pPage;

		InsertItem(GetItemCount(), &tcItem);

		delete szBuffer;
	}
}

void CCategoryTabCtrl::AddPage(UINT nIDItem, CCategoryPage* pPage)
{
	CString str;
	str.LoadString(nIDItem);
	AddPage(str, pPage);
}

void CCategoryTabCtrl::OnDestroy()
{
	CTabCtrl::OnDestroy();
}

void CCategoryTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CCategoryTabCtrl::RecalcLayout()
{
	if (::IsWindow(m_hWnd) && m_nActivePage>=0)
	{
		CRect rc;
		GetWindowRect(&rc);
		AdjustRect(FALSE, &rc);

		ScreenToClient(&rc);

		CCategoryPage* pPage = GetPage(m_nActivePage);
		pPage->MoveWindow(&rc);
	}
}

void CCategoryTabCtrl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateSelectionChange();
	*pResult = 0;
}

void CCategoryTabCtrl::UpdateSelectionChange()
{
	if(m_nActivePage != GetCurSel())
	{
		if(m_nActivePage>=0)
		{
			CCategoryPage* pPage = (CCategoryPage*)GetPage(m_nActivePage);
			pPage->ShowWindow(SW_HIDE);
			pPage->OnKillActive();
		}
		m_nActivePage = GetCurSel();
		if(m_nActivePage>=0)
		{
			CCategoryPage* pPage = (CCategoryPage*)GetPage(m_nActivePage);
			ASSERT(::IsWindow(pPage->m_hWnd));
			RecalcLayout();
			pPage->ShowWindow(SW_SHOW);
			pPage->OnSetActive();

			TCHAR szBuff[256];
			TCITEM tci;
			memset(&tci, 0, sizeof(tci));
			tci.mask = TCIF_TEXT;
			tci.pszText = szBuff;
			tci.cchTextMax = 256;
			GetItem(m_nActivePage, &tci);

			CControlBar *pBar = (CControlBar *)GetParent()->GetParent();
			ASSERT_VALID(pBar);
			if (pBar)
			{
				pBar->SetWindowText(tci.pszText);
				if (pBar->IsFloating())
					pBar->GetParent()->SetWindowText(tci.pszText);
			}
		}
	}
}

void CCategoryTabCtrl::SetActivePage(int nActivePage)
{
	if(m_nActivePage != nActivePage)
	{
		SetCurSel(nActivePage);
		UpdateSelectionChange();
	}
}

CCategoryPage* CCategoryTabCtrl::GetPage(int nPage)
{
	TCITEM tcItem;
	memset(&tcItem, 0, sizeof(tcItem));
	tcItem.mask = TCIF_PARAM;
	GetItem(nPage, &tcItem);
	return (CCategoryPage*)tcItem.lParam;
}

int CCategoryTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	::GetObject(hFont, sizeof(lf), &lf);
	m_fontNormal.CreateFontIndirect(&lf);
	SetFont(&m_fontNormal);

	return 0;
}
