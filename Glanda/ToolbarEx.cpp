// ToolbarEx.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ToolbarEx.h"

#include "VisualStylesXP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern AUX_DATA afxData;
#define CX_BORDER 1
#define CY_BORDER 1
/////////////////////////////////////////////////////////////////////////////
// CToolBarEx

CToolBarEx::CToolBarEx() : 
	m_pButtons(NULL), 
	m_nButtons(0), 
	m_hKey(NULL), 
	m_pImageList(NULL)
{
}

CToolBarEx::~CToolBarEx()
{
	if (m_pButtons)
		delete []m_pButtons;
	if (m_pImageList)
		delete m_pImageList;
}


BEGIN_MESSAGE_MAP(CToolBarEx, CToolBar)
	//{{AFX_MSG_MAP(CToolBarEx)
	//}}AFX_MSG_MAP
	ON_COMMAND(_ID_CUSTOMIZE, OnToolBarCustomize)
	ON_UPDATE_COMMAND_UI(_ID_CUSTOMIZE, OnUpdateToolBarCustomize)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnDropDown)
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(TB_SETBUTTONSIZE, OnSetButtonSize)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolBarEx message handlers

BOOL CToolBarEx::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	if (message == WM_NOTIFY)
	{
		NMHDR *pNMHDR = (NMHDR *) lParam;
		switch (pNMHDR->code)
		{
		case (TBN_FIRST - 23) /*TBN_INITCUSTOMIZE*/:	// needs IE 5 or later
			*pResult = 0x00000001 /*TBNRF_HIDEHELP*/;
			return TRUE;

		case TBN_BEGINADJUST :
			TRACE0("TBN_BEGINADJUST\n");
			*pResult = FALSE;
			return FALSE;
		
		case TBN_BEGINDRAG:
			TRACE0("TBN_BEGINDRAG\n");
			*pResult = FALSE;
			return FALSE;

		case TBN_GETBUTTONINFO:
			return GetButtonInfo(wParam, lParam, pResult);
		
		case TBN_QUERYINSERT:
			TRACE2("TBN_QUERYINSERT: %d:%d\n", GetToolBarCtrl().GetButtonCount(), ((NMTOOLBAR *)lParam)->iItem);
			if (::IsWindow(m_wndButton.m_hWnd))
				*pResult = ((NMTOOLBAR *)lParam)->iItem == GetToolBarCtrl().GetButtonCount() - 1 ? FALSE : TRUE;
			else
				*pResult = TRUE;
			return TRUE;
		
		case TBN_QUERYDELETE:
			TRACE0("TBN_QUERYDELETE\n");
			if (::IsWindow(m_wndButton.m_hWnd))
				*pResult = ((NMTOOLBAR *)lParam)->iItem == GetToolBarCtrl().GetButtonCount() - 1 ? FALSE : TRUE;
			else
				*pResult = TRUE;
			return TRUE;

		case TBN_TOOLBARCHANGE:
			TRACE1("TBN_TOOLBARCHANGE: %d\n", ((NMTOOLBARA *)lParam)->iItem);
			RecalcLayout();
			*pResult = TRUE;
			return TRUE;

		case TBN_RESET:
			RestoreInitialState();
			*pResult = TRUE;
			return TRUE;

		case TBN_ENDADJUST:
			SaveState();
			*pResult = TRUE;
			return TRUE;
		}
	}

	return CToolBar::OnChildNotify(message, wParam, lParam, pResult);
}

void CToolBarEx::SetAdjustable(BOOL bAdjustable)
{
	DWORD dwStyle = GetToolBarCtrl().GetStyle();
	if (bAdjustable)
		dwStyle |= CCS_ADJUSTABLE;
	else
		dwStyle &= ~CCS_ADJUSTABLE;
	GetToolBarCtrl().SetStyle(dwStyle);
}

void CToolBarEx::SaveState()
{
	GetToolBarCtrl().SaveState(m_hKey, m_strSubKey, m_strName);
}

void CToolBarEx::RestoreState()
{
	GetToolBarCtrl().RestoreState(m_hKey, m_strSubKey, m_strName);
	RecalcLayout();
}

void CToolBarEx::SaveInitialState()
{
	m_nButtons = GetToolBarCtrl().GetButtonCount();
	if (m_nButtons > 0)
	{
		m_pButtons = new TBBUTTON[m_nButtons];
		ASSERT(m_pButtons);

		for (int i = 0; i < m_nButtons; i++)
			GetToolBarCtrl().GetButton(i, &m_pButtons[i]);
	}

	CString strName = m_strName + "-org";
	GetToolBarCtrl().SaveState(m_hKey, m_strSubKey, strName);
}

void CToolBarEx::RestoreInitialState()
{
	CString strName = m_strName + "-org";
	GetToolBarCtrl().RestoreState(m_hKey, m_strSubKey, strName);
	RecalcLayout();
}

BOOL CToolBarEx::GetButtonInfo(WPARAM wParam, LPARAM lParam, LRESULT *pLResult)
{
	TBNOTIFY *pTBN = (TBNOTIFY *) lParam;

	TRACE3("Count:%d, GetCount:%d, GetButtonCount:%d\n", m_nButtons, GetCount(), GetToolBarCtrl().GetButtonCount());
	if (pTBN->iItem >= m_nButtons)
	{
		* pLResult = FALSE;
	}
	else
	{
		CString strText;
		strText.LoadString(m_pButtons[pTBN->iItem].idCommand);
		CString strTip;
		AfxExtractSubString(strTip, strText, 1, '\n');
		strncpy(pTBN->pszText, strTip, pTBN->cchText - 1);
		pTBN->pszText[pTBN->cchText - 1] = 0;

		pTBN->tbButton = m_pButtons[pTBN->iItem];

		*pLResult = TRUE;
	}

	return TRUE;
}

BOOL CToolBarEx::CreateCustomizeButton()
{
	m_wndButton.Create("", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, _ID_CUSTOMIZE);
	m_wndButton.LoadBitmap(IDB_TOOLBAR_BUTTON);

	RecalcLayout();

	return TRUE;
}

void CToolBarEx::OnUpdateToolBarCustomize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CToolBarEx::OnToolBarCustomize()
{
	CRect rc;
	int count = GetToolBarCtrl().GetButtonCount();
	GetItemRect(count - 1, rc);
	ClientToScreen(&rc);

	ShowCustomizeMenu(CPoint(rc.left, rc.bottom));
}

void CToolBarEx::RecalcLayout()
{
	if (::IsWindow(m_wndButton.m_hWnd))
	{
		int index = CommandToIndex(_ID_CUSTOMIZE);
		SetButtonInfo(index, _ID_CUSTOMIZE, TBBS_SEPARATOR, 11);
		CRect rc;
		GetItemRect(index, rc);
		m_wndButton.MoveWindow(&rc);
	}

	CFrameWnd *pFrame;
	if (!IsFloating())
	{
		pFrame = (CFrameWnd *)GetTopLevelFrame();
		pFrame->RecalcLayout(TRUE);
	}
	else
	{
		pFrame = (CFrameWnd *)GetParent()->GetParent();
		pFrame->RecalcLayout();
	}
}

void CToolBarEx::Init(LPCTSTR lpszSection, LPCTSTR lpszName)
{
	m_hKey = HKEY_CURRENT_USER;
	m_strSubKey.Format("Software\\%s\\%s\\%s", AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszProfileName, lpszSection);
	m_strName = lpszName;

	SetAdjustable(TRUE);
	SaveInitialState();
	RestoreState();

	//CreateCustomizeButton();
}

void CToolBarEx::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTBCUSTOMDRAW *pcd = (NMTBCUSTOMDRAW *)pNMHDR;

	*pResult = CDRF_DODEFAULT;

	switch (pcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		}

		case CDDS_ITEMPREPAINT:
		{
			if (!g_xpStyle.IsAppThemed())
			{
				if (pcd->nmcd.uItemState & CDIS_CHECKED)
				{
					pcd->clrHighlightHotTrack = pcd->clrBtnFace;
					*pResult = TBCDRF_HILITEHOTTRACK;
				}
			}
			break;
		}
	}
}

BOOL CToolBarEx::SetButtonText(UINT nCommand, UINT nStringID)
{
	if (!SupportMixedButtons())
		return FALSE;

	CToolBarCtrl &tb = GetToolBarCtrl();

	//tb.SetMaxTextRows(1);

	int dwStyle = tb.GetStyle();
	if ((dwStyle & TBSTYLE_LIST) == 0)
		tb.ModifyStyle(0, TBSTYLE_LIST);

	int dwExStyle = tb.GetExtendedStyle();
	if ((dwExStyle & 0x00000008 /*TBSTYLE_EX_MIXEDBUTTONS*/) == 0)
		tb.SetExtendedStyle(dwExStyle | 0x00000008);

	int index = tb.CommandToIndex(nCommand);
	if (index == -1)
		return FALSE;

	CString strTip;
	if (nStringID == 0)
	{
		CString strText;
		if (!strText.LoadString(nCommand))
			return FALSE;
		if (!AfxExtractSubString(strTip, strText, 1, '\n'))
			return FALSE;
	}
	else
	{
		strTip.LoadString(nStringID);
	}
	if (strTip.IsEmpty())
		return FALSE;

	TBBUTTON button;
	_GetButton(index, &button);
	button.fsStyle |= 0x0040 /*BTNS_SHOWTEXT*/| TBSTYLE_AUTOSIZE;
	_SetButton(index, &button);

	CToolBar::SetButtonText(index, strTip);

	return TRUE;
}

void CToolBarEx::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here

	if (m_hKey && !m_strSubKey.IsEmpty() && !m_strName.IsEmpty())
		ShowCustomizeMenu(point);
}

void CToolBarEx::ShowCustomizeMenu(CPoint point)
{
	CMenu menu;
	if (menu.LoadMenu(IDR_TOOLBAR_CUSTOMIZE))
	{
		CMenu *pMenu = menu.GetSubMenu(0);
		if (pMenu)
		{
			UINT ret = pMenu->TrackPopupMenu(
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, 
				point.x, 
				point.y, 
				this, 
				NULL);

			switch (ret)
			{
			//case _ID_HIDE:
			//	GetTopLevelFrame()->ShowControlBar(this, FALSE, FALSE);
			//	break;
			case _ID_CUSTOMIZE:
				GetToolBarCtrl().Customize();
				break;
			case _ID_RESET:
				RestoreInitialState();
				break;
			}
		}
	}
}

void CToolBarEx::SetDropDownButton(UINT nCommand, UINT nIDMenu)
{
	if (!SupportDropDownButton())
		return;

	CToolBarCtrl &tb = GetToolBarCtrl();

	int dwExStyle = tb.GetExtendedStyle();
	if ((dwExStyle & TBSTYLE_EX_DRAWDDARROWS) == 0)
		tb.SetExtendedStyle(dwExStyle | TBSTYLE_EX_DRAWDDARROWS);

	TBBUTTONINFO tbi;

	tbi.dwMask= TBIF_STYLE | TBIF_LPARAM;
	tbi.cbSize= sizeof(TBBUTTONINFO);

	tb.GetButtonInfo(nCommand, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN;
	tbi.lParam = (DWORD)nIDMenu;
	tb.SetButtonInfo(nCommand, &tbi);

	//int index = CommandToIndex(nCommand);

	//TBBUTTON button;
	//_GetButton(index, &button);
	//button.fsStyle |= TBSTYLE_DROPDOWN;
	//_SetButton(index, &button);
}

void CToolBarEx::ShowPopupMenu(UINT nCommand, UINT nIDMenu)
{
	CMenu menu;
	if (menu.LoadMenu(nIDMenu))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		if (pPopup)
		{
			CToolBarCtrl &tb = GetToolBarCtrl();
			int index = tb.CommandToIndex(nCommand);
			if (index >= 0)
			{
				RECT rc;
				tb.GetItemRect(index, &rc);
				tb.ClientToScreen(&rc);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom, GetParentFrame());
			}
		}
	}
}

void CToolBarEx::OnDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR pNMTB = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);

	// TODO: Add your control notification handler code here

	*pResult = TBDDRET_DEFAULT;

	CToolBarCtrl &tb = GetToolBarCtrl();

	TBBUTTONINFO tbi;
	tbi.dwMask= TBIF_LPARAM;
	tbi.cbSize= sizeof(TBBUTTONINFO);
	tb.GetButtonInfo(pNMTB->iItem, &tbi);

	if (tbi.lParam == 0)
		return;

	ShowPopupMenu(pNMTB->iItem, (UINT)tbi.lParam);
}

LRESULT CToolBarEx::OnSetButtonSize(WPARAM wParam, LPARAM lParam)
{
	return CToolBar::OnSetButtonSize(wParam, lParam);
}

BOOL CToolBarEx::LoadImage(UINT nBitmapID, int cxButton, int cyButton)
{
	CBitmap bmp;
	if (!bmp.LoadBitmap(nBitmapID))
		return FALSE;

	return LoadImage(&bmp, cxButton, cyButton);
}

BOOL CToolBarEx::LoadImage(CBitmap *pBitmap, int cxButton, int cyButton)
{
	ASSERT(!m_pImageList);
	if (m_pImageList)
		delete m_pImageList;
	m_pImageList = new CImageList();
	if (!m_pImageList)
		return FALSE;

	if (!m_pImageList->Create(cxButton, cyButton, ILC_MASK | ILC_COLOR32, 0, 0))
		return FALSE;
	m_pImageList->Add(pBitmap, RGB(255, 0, 255));
	GetToolBarCtrl().SetImageList(m_pImageList);

	return TRUE;
}

void CToolBarEx::DrawBorders(CDC* pDC, CRect& rect)
{
	// TODO: Add your message handler code here
	// Do not call CToolBar::OnNcPaint() for painting messages

	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	DWORD dwStyle = m_dwStyle;
	if (!(dwStyle & CBRS_BORDER_ANY))
		return;

/*
	// prepare for dark lines
	ASSERT(rect.top == 0 && rect.left == 0);
	CRect rect1, rect2;
	rect1 = rect;
	rect2 = rect;
	COLORREF clr = afxData.clrBtnShadow;


	// draw dark line one pixel back/up
	if (dwStyle & CBRS_BORDER_3D)
	{
		rect1.right -= CX_BORDER;
		rect1.bottom -= CY_BORDER;
	}
	if (dwStyle & CBRS_BORDER_TOP)
		rect2.top += afxData.cyBorder2;
	if (dwStyle & CBRS_BORDER_BOTTOM)
		rect2.bottom -= afxData.cyBorder2;

	if (IsKindOf(RUNTIME_CLASS(CToolBar)) && (dwStyle & TBSTYLE_FLAT) && (dwStyle & CBRS_BORDER_ANY))
	{
		CRect rctBk;
		rctBk.left = 0;
		rctBk.right = rect.right;
		rctBk.top = rect.top;
		rctBk.bottom = rect.bottom;

		pDC->FillSolidRect((LPRECT)rctBk, pDC->GetBkColor());
	}

	// draw left and top
	if (dwStyle & CBRS_BORDER_LEFT)
		pDC->FillSolidRect(0, rect2.top, CX_BORDER, rect2.Height(), clr);
	if (dwStyle & CBRS_BORDER_TOP)
		pDC->FillSolidRect(0, 0, rect.right, CY_BORDER, clr);

	// draw right and bottom
	if (dwStyle & CBRS_BORDER_RIGHT)
		pDC->FillSolidRect(rect1.right, rect2.top, -CX_BORDER, rect2.Height(), clr);
	if (dwStyle & CBRS_BORDER_BOTTOM)
		pDC->FillSolidRect(0, rect1.bottom, rect.right, -CY_BORDER, clr);

	if (dwStyle & CBRS_BORDER_3D)
	{
		// prepare for hilite lines
		clr = afxData.clrBtnHilite;

		// draw left and top
		if (dwStyle & CBRS_BORDER_LEFT)
			pDC->FillSolidRect(1, rect2.top, CX_BORDER, rect2.Height(), clr);
		if (dwStyle & CBRS_BORDER_TOP)
			pDC->FillSolidRect(0, 1, rect.right, CY_BORDER, clr);

		// draw right and bottom
		if (dwStyle & CBRS_BORDER_RIGHT)
			pDC->FillSolidRect(rect.right, rect2.top, -CX_BORDER, rect2.Height(), clr);
		if (dwStyle & CBRS_BORDER_BOTTOM)
			pDC->FillSolidRect(0, rect.bottom, rect.right, -CY_BORDER, clr);
	}
*/

	if (dwStyle & TBSTYLE_FLAT)
	{
		CRect rctBk;
		rctBk.left = 0;
		rctBk.right = rect.right;
		rctBk.top = rect.top;
		rctBk.bottom = rect.bottom;

		pDC->FillSolidRect((LPRECT)rctBk, afxData.clrBtnFace);
	}

	COLORREF clr = afxData.clrBtnShadow;
	if (dwStyle & CBRS_BORDER_LEFT)
		pDC->FillSolidRect(rect.left, rect.top, CX_BORDER, rect.Height(), clr);
	if (dwStyle & CBRS_BORDER_TOP)
		if (!g_xpStyle.IsAppThemed())
			pDC->FillSolidRect(rect.left, rect.top, rect.right, CY_BORDER, clr);
	if (dwStyle & CBRS_BORDER_RIGHT)
		pDC->FillSolidRect(rect.right, rect.top, -CX_BORDER, rect.Height(), clr);
	if (dwStyle & CBRS_BORDER_BOTTOM)
		pDC->FillSolidRect(0, rect.bottom, rect.right, -CY_BORDER, clr);


	if (dwStyle & CBRS_BORDER_LEFT)
		rect.left += CX_BORDER;
	if (dwStyle & CBRS_BORDER_TOP)
		rect.top += CY_BORDER;
	if (dwStyle & CBRS_BORDER_RIGHT)
		rect.right -= CX_BORDER;
	if (dwStyle & CBRS_BORDER_BOTTOM)
		rect.bottom -= CY_BORDER;
}

BOOL CToolBarEx::SupportDropDownButton(void)
{
	return (_AfxGetComCtlVersion() >= MAKELONG(72, 4));
}

BOOL CToolBarEx::SupportMixedButtons(void)
{
	return (_AfxGetComCtlVersion() >= MAKELONG(81, 5));
}
