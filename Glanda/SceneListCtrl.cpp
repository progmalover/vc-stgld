// SceneListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "scenelistctrl.h"
#include "my_app.h"

#include "GlandaDoc.h"
#include "gldMainMovie2.h"
#include "gldScene2.h"
#include "CmdInsertScene.h"
#include "CmdDeleteScene.h"
#include "CmdChangeCurrentScene.h"
#include "SWFProxy.h"
#include "TransAdaptor.h"
#include "CmdRenameScene.h"
#include <atlwin.h>
#include ".\scenelistctrl.h"
#include "GlandaClipboard.h"
#include "Background.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const UINT IDT_DELAY_EDIT_NAME	= 1001;
const UINT IDT_AUTO_SCROLL		= 1002;

#define IsControlDown() (GetKeyState(VK_CONTROL) & (1 << (sizeof(short int) * 8 - 1)))

// CSceneListCtrl
IMPLEMENT_SINGLETON(CSceneListCtrl)
CSceneListCtrl::CSceneListCtrl()
{
	m_dwStyle = FLCS_DISABLENOSCROLL;

	SetMargin(4, 5, 4, 5);
	SetItemSpace(0, 6);

	m_nLabelHeight = 0;
	m_nLButtonDownOnSelected = -1;
	m_nEditName = -1;
	m_nDelayEditName = -1;
	m_bInEndEditName = FALSE;

	m_bScrolled = FALSE;
	m_nAutoScrollDir = none;

	m_hAccel = NULL;
}

CSceneListCtrl::~CSceneListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSceneListCtrl, CFlexListCtrl)
	ON_COMMAND(ID_SCENE_ADD, OnSceneAdd)
	ON_COMMAND(ID_SCENE_INSERT, OnSceneInsert)
	ON_COMMAND(ID_SCENE_RENAME, OnSceneRename)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(1, OnNameEditKillFocus)
	ON_EN_UPDATE(1, OnNameEditChange)
	ON_CONTROL(EN_KEYRETURN, 1, OnNameEditKeyEnter)
	ON_CONTROL(EN_KEYESCAPE, 1, OnNameEditKeyEscape)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_COMMAND(ID__DUPLICATESCENE, OnSceneDupliate)
	ON_UPDATE_COMMAND_UI(ID__DUPLICATESCENE, OnUpdateSceneDupliate)
END_MESSAGE_MAP()



// CSceneListCtrl message handlers

int CSceneListCtrl::InsertScene(int index, gldScene2 *pScene)
{
	ASSERT(index >= 0 && index <= GetItemCount());
	ASSERT(pScene != NULL);

	_ItemData* pItemData = new _ItemData(pScene);
	return InsertItem(index, (DWORD_PTR)pItemData);
}

int CSceneListCtrl::FindScenePos(gldScene2* pScene)
{
	for(int i = 0, j= GetItemCount(); i<j; ++i)
	{
		if(GetScene(i)==pScene)
		{
			return i;
		}
	}
	return -1;
}

void CSceneListCtrl::OnSceneAdd()
{
	// 生成初始的场景名称
	CString strName;
	AutoGenerateSceneName(strName);

	int nIndex = GetItemCount();

	// 先添加场景,然后再选中此场景
	TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_ADDSCENE);
	pCmdGroup->Do(new CCmdInsertScene(strName, nIndex));
	pCmdGroup->Do(new CCmdChangeCurrentScene(CSceneListCtrl::Instance()->GetScene(nIndex)));
	if (my_app.Commands().Do(pCmdGroup))
		EnsureVisible(nIndex);
}

void CSceneListCtrl::OnSceneInsert()
{
	// 生成初始的场景名称
	CString strName;
	AutoGenerateSceneName(strName);

	int nIndex = GetCurSel();
	if(nIndex<0)
	{
		nIndex = GetItemCount();
	}

	// 先插入场景,然后再选中此场景
	TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_ADDSCENE);
	pCmdGroup->Do(new CCmdInsertScene(strName, nIndex));
	pCmdGroup->Do(new CCmdChangeCurrentScene(CSceneListCtrl::Instance()->GetScene(nIndex)));
	if (my_app.Commands().Do(pCmdGroup))
		EnsureVisible(nIndex);
}

void CSceneListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

	if (pWnd == this)
	{
		CRect rc;

		GetClientRect(rc);
		ClientToScreen(&rc);

		if (rc.PtInRect(point))
		{
			CMenu menu;
			if (menu.LoadMenu(IDR_SCENE))
			{
				CMenu *pPopup = menu.GetSubMenu(0);
				if (pPopup)
					pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
			}
		}
	}
}

void CSceneListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CFlexListCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	EndEditName(TRUE);

	RecalcItemSize();
}

void CSceneListCtrl::RecalcItemSize()
{
	if (::IsWindow(m_hWnd))
	{
		gldMainMovie2 *pMainMovie = _GetMainMovie2();
		if (pMainMovie)
		{
			CRect rc;
			GetClientRect(&rc);
			int cx = rc.Width();
			cx -= (m_rcMargin.left + m_rcMargin.right + 3 * 2);
			cx = max(0, cx);

			int w = max(1, cx);
			int h = max(1, cx * pMainMovie->m_height / pMainMovie->m_width);
			h = min(300, h);

			// add text height
			int nLabelHeight = GetLabelHeight();
			SetItemSize(w + 3 * 2, h + 3 * 2 + 3 /* name edit */ + nLabelHeight + 1 /* focus rect */);
		}
	}
}

IMPLEMENT_OBSERVER(CSceneListCtrl, ChangeMainMovie2)
{
	DeleteAllItems();

	gldMainMovie2 *pMainMovie = _GetMainMovie2();
	if (pMainMovie)
	{
		int count = 0;
		for (GSCENE2_LIST::iterator it = pMainMovie->m_sceneList.begin(); it != pMainMovie->m_sceneList.end(); it++, count++)
			InsertScene(count, *it);

		gldScene2 *pScene = _GetCurScene2();
		if (pScene)
			SelectItem(FindScenePos(pScene));

		RecalcItemSize();
	}
}

// modify scene size & background
IMPLEMENT_OBSERVER(CSceneListCtrl, ChangeMovieProperties)
{
	RecalcItemSize();
	for(int i = 0, j = GetItemCount(); i < j; ++i)
	{
		_ItemData* pItemData = (_ItemData*)GetItemData(i);
		pItemData->ClearBuffer();
	}
	Invalidate(FALSE);
}

// 场景选择改变时侦听到的事件
IMPLEMENT_OBSERVER(CSceneListCtrl, ChangeCurrentScene2)
{
	EndEditName(FALSE);
	CancelDelayEditName();

	// 查找新的场景的索引
	int nNewIndex = -1;
	gldScene2 *pScene = _GetCurScene2();
	if (pScene)
		nNewIndex = FindScenePos(_GetCurScene2());

	int nOldIndex = GetCurSel();

	// 设置新的场景为选中场景
	if(nOldIndex != nNewIndex)
	{
		if (nNewIndex >= 0)
			SelectItem(nNewIndex);
		else
			UnselectItem(nOldIndex);
	}
}

IMPLEMENT_OBSERVER(CSceneListCtrl, ModifyCurrentScene2)
{
	TRACE0("Notify: ModifyCurrentScene2\n");

	int index = GetCurSel();
	ASSERT(index >= 0);
	if (index >= 0)
	{
		_ItemData* pItemData = (_ItemData*)GetItemData(index);
		pItemData->ClearBuffer();
		RedrawItem(index);
	}
}

void CSceneListCtrl::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
	BOOL bSel = GetCurSel() == index;
	BOOL bFocused = GetFocus() == this;

	_ItemData* pItemData = (_ItemData*)GetItemData(index);
	gldScene2* pScene = pItemData->m_pScene;

	CBrush brWindow(::GetSysColor(COLOR_WINDOW));
	pDC->FillRect(pRect, &brWindow);

	CRect rcText;
	GetLabelRect(index, &rcText, TRUE);

	if (index != m_nEditName)
	{
		CFont* pOldFont = pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

		CBrush brText(::GetSysColor(bSel ? (bFocused ? COLOR_HIGHLIGHT : COLOR_BTNFACE) : COLOR_WINDOW));

		pDC->FillRect(&rcText, &brText);
		pDC->SetTextColor(GetSysColor((bSel && bFocused) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(pScene->m_name.c_str(), &rcText, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
		pDC->SelectObject(pOldFont);

		if (bSel && bFocused)
		{
			CRect rcFocus = rcText;
			rcFocus.InflateRect(1, 1);
			// important for DrawFocusRect to function correctly
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->DrawFocusRect(&rcFocus);
		}
	}

	CRect rcThumb(pRect);
	rcThumb.bottom = rcText.top - 3;
	if (rcThumb.Width() > 0)
	{
		if (bSel)
		{
			CBrush Brush(GetSysColor(bFocused ? COLOR_HIGHLIGHT : COLOR_3DSHADOW));
			pDC->FrameRect(&rcThumb, &Brush);
			rcThumb.DeflateRect(1, 1);
			pDC->FrameRect(&rcThumb, &Brush);
			rcThumb.DeflateRect(1, 1);
		}
		else
		{
			CBrush Brush(GetSysColor(COLOR_3DFACE));
			rcThumb.DeflateRect(1, 1);
			pDC->FrameRect(&rcThumb, &Brush);
			rcThumb.DeflateRect(1, 1);
		}

		rcThumb.DeflateRect(1, 1);
		if (rcThumb.Width() > 0 && rcThumb.Height() > 0)
		{
			if(pItemData->m_pBitmap)
			{
				BITMAP bm;
				pItemData->m_pBitmap->GetBitmap(&bm);
				if(rcThumb.Width() != bm.bmWidth || rcThumb.Height() != bm.bmHeight)
				{
					pItemData->ClearBuffer();
				}
			}

			CDC dc;
			dc.CreateCompatibleDC(pDC);
			if(!pItemData->m_pBitmap)
			{
				pItemData->m_pBitmap = new CBitmap();
				BITMAPINFO		bi;
				BYTE			*pBits = NULL;
				memset(&bi, 0, sizeof(bi));	
				bi.bmiHeader.biBitCount = 32;
				bi.bmiHeader.biCompression = BI_RGB;
				bi.bmiHeader.biHeight = rcThumb.Height();
				bi.bmiHeader.biWidth = rcThumb.Width();
				bi.bmiHeader.biPlanes = 1;
				bi.bmiHeader.biSize = sizeof(bi.bmiHeader);	
				pItemData->m_pBitmap->Attach(::CreateDIBSection(0, &bi, DIB_RGB_COLORS, (void **)&pBits, 0, 0));

				CBitmap* pOldBitmap = dc.SelectObject(pItemData->m_pBitmap);
				CRect rcBuffer(0, 0, rcThumb.Width(), rcThumb.Height());
				dc.FillRect(&rcBuffer, CBrush::FromHandle(::GetSysColorBrush(COLOR_WINDOW)));
				RenderThumb(dc, rcBuffer, pScene);

				// Render Thumb
				pDC->BitBlt(rcThumb.left, rcThumb.top, rcThumb.Width(), rcThumb.Height(), &dc, 0, 0, SRCCOPY);
				dc.SelectObject(pOldBitmap);
			}
			else
			{
				CBitmap* pOldBitmap = dc.SelectObject(pItemData->m_pBitmap);
				pDC->BitBlt(rcThumb.left, rcThumb.top, rcThumb.Width(), rcThumb.Height(), &dc, 0, 0, SRCCOPY);
				dc.SelectObject(pOldBitmap);
			}
		}
	}
}

void CSceneListCtrl::RenderThumb(CDC& dc, CRect& rc, gldScene2* pScene)
{
	CSize szCanvas = my_app.GetMovieSize();

	int xCenter = (rc.left + rc.right) / 2;
	int wHalf = max(1, rc.Height() *  szCanvas.cx / szCanvas.cy / 2);
	CRect rcAdjust;
	rcAdjust.top = rc.top;
	rcAdjust.bottom = rc.bottom;
	rcAdjust.left = xCenter - wHalf;
	rcAdjust.right = xCenter + wHalf;

	// Render Thumb
	TGraphicsDevice device(rcAdjust.Width(), rcAdjust.Height());
	TMatrix scaleMat((float)rcAdjust.Width() / szCanvas.cx,
		0.0f, 0.0f, (float)rcAdjust.Height() / szCanvas.cy, 0, 0);	
	device.SetMatrix(scaleMat);

	COLORREF rgb = _GetMainMovie2()->m_color;
	TColor color(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), 255);
	device.SetBkColor(color);
	device.ClearRect();


	if (pScene->m_backgrnd != NULL)
	{
		gld_shape backgrnd = pScene->m_backgrnd->GetEditShape(pScene != _GetCurScene2());
		TDraw::draw(backgrnd, device);
		backgrnd.release(true);
	}
	GINSTANCE_LIST::iterator iInst = pScene->m_instanceList.begin();
	for (; iInst != pScene->m_instanceList.end(); iInst++)
	{
		gldInstance *pInst = *iInst;
		gld_shape	shape;
		TMatrix mx;
		TCxform cx;
		CTransAdaptor::GM2TM(pInst->m_matrix, mx);
		CTransAdaptor::GCX2TCX(pInst->m_cxform, cx);
		shape.create((gld_shape)CTraitShape(pInst->m_obj), mx, cx, NULL);
		shape.trans_origin(pInst->m_tx, pInst->m_ty);		
		TDraw::draw(shape, device);
		shape.release(true);
	}
	device.Display(dc.m_hDC, rcAdjust.left, rcAdjust.top);
}

void CSceneListCtrl::AutoGenerateSceneName(CString& strName)
{
	CString strScene;
	strScene.LoadString(IDS_KEY_SCENE);
	GSCENE2_LIST& sceneList = _GetMainMovie2()->m_sceneList;
	for(int i=1; true ; ++i)
	{
		strName.Format("%s %d", (LPCTSTR)strScene, i);
		GSCENE2_LIST::iterator iter;
		for(iter = sceneList.begin(); iter!=sceneList.end(); ++iter)
		{
			gldScene2 *pScene = *iter;
			if(!strName.CompareNoCase(pScene->m_name.c_str()))
				break;
		}

		if(iter==sceneList.end())
		{
			return ;
		}
	}
}

void CSceneListCtrl::AutoGenerateSceneName(std::string& sName)
{
	CString strName;
	AutoGenerateSceneName(strName);
	sName = (LPCTSTR)strName;
}

gldScene2* CSceneListCtrl::GetScene(int nIndex)
{
	if(nIndex < 0)
		return NULL;

	_ItemData* pItemData = (_ItemData*)GetItemData(nIndex);
	return pItemData->m_pScene;
}

gldScene2* CSceneListCtrl::GetSelectedScene(void)
{
	return GetScene(GetCurSel());
}

void CSceneListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CFlexListCtrl::OnRButtonDown(nFlags, point);

	int index = ItemFromPoint(point);
	if (index >= 0 && index != GetCurSel())
		SelectItem(index, TRUE);
}

bool CSceneListCtrl::MoveScene(int nNewIndex, int nOldIndex)
{
	return true;
}

void CSceneListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CFlexListCtrl::OnSetFocus(pOldWnd);

	if(GetCurSel()>=0)
	{
		RedrawItem(GetCurSel());
	}
}

void CSceneListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CFlexListCtrl::OnKillFocus(pNewWnd);

	if(GetCurSel()>=0)
	{
		RedrawItem(GetCurSel());
	}
}

int CSceneListCtrl::GetLabelHeight()
{
	int nLabelHeight = 12;

	CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	if (pFont)
	{
		CClientDC dc(this);
		CFont *pFontOld = dc.SelectObject(pFont);
		TEXTMETRIC tm;
		memset(&tm, 0, sizeof(tm));
		dc.GetTextMetrics(&tm);
		dc.SelectObject(pFont);

		nLabelHeight = tm.tmHeight;
	}

	return nLabelHeight;
}

void CSceneListCtrl::GetLabelRect(int index, RECT *pRect, BOOL bExact)
{
	GetItemRect(index, pRect);

	// reserve 1 pixel for focus rect
	pRect->left++;
	pRect->right--;
	pRect->bottom--;
	pRect->top = pRect->bottom - GetLabelHeight();
	if (bExact)
	{
		gldScene2 *pScene = GetScene(index);
		if (pScene)
		{
			CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			if (pFont)
			{
				CClientDC dc(this);
				CFont *pFontOld = dc.SelectObject(pFont);
				CSize size = dc.GetTextExtent(pScene->m_name.c_str());
				dc.SelectObject(pFont);

				int x = (pRect->left + pRect->right) / 2;
				pRect->left = max(pRect->left, x - size.cx / 2 - 1);
				pRect->right = min(pRect->right, x + size.cx / 2 + 1);
			}
		}
	}
}

void CSceneListCtrl::BeginEditName(int index)
{
	ASSERT(index == GetCurSel());
	if (index != GetCurSel())
		return;

	gldScene2* pScene = GetScene(index);
	if (pScene)
	{
		if (!::IsWindow(m_edit.m_hWnd))
		{
			m_edit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(0, 0, 0, 0), this, 1);
			m_edit.SetFont(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)), FALSE);
			m_edit.SetMargins(3, 3);
			m_edit.LimitText(gldScene2::MAX_SCENE_NAME);
		}

		m_edit.SetWindowText(pScene->m_name.c_str());

		CRect rcItem, rcLabel;
		GetItemRect(index, &rcItem);
		GetLabelRect(index, &rcLabel, FALSE);
		rcLabel.InflateRect(0, 2, 0, 2);
		m_edit.SetWindowPos(NULL, rcLabel.left, rcLabel.top, rcLabel.Width(), rcLabel.Height(), SWP_NOZORDER);
		m_edit.SetMaxWidth(rcLabel.Width());
		m_edit.RecalcWindowWidth();
		m_edit.ShowWindow(SW_SHOW);
		m_edit.SetSel(0, -1);
		m_edit.SetFocus();

		m_nEditName = index;
	}
}

void CSceneListCtrl::EndEditName(BOOL bSubmit)
{
	// check reenter
	if (m_bInEndEditName)
		return;

	m_bInEndEditName = TRUE;

	if (m_nEditName >= 0)
	{
		if (bSubmit)
		{
			gldScene2 *pCurScene = GetScene(m_nEditName);
			ASSERT(pCurScene);
			if (pCurScene)
			{
				CString str;
				m_edit.GetWindowText(str);
				if (!str.IsEmpty())
				{
					gldMainMovie2 *pMainMovie = _GetMainMovie2();
					for (GSCENE2_LIST::iterator it = pMainMovie->m_sceneList.begin(); it != pMainMovie->m_sceneList.end(); it++)
					{
						gldScene2 *pScene = *it;
						if (pScene != pCurScene && str.CompareNoCase(pScene->m_name.c_str()) == 0)
						{
							AfxMessageBox(IDS_SCENE_NAME_EXISTS);
							m_edit.SetWindowText(pCurScene->m_name.c_str());
							m_edit.SetSel(0, -1);
							m_edit.SetFocus();
							
							m_bInEndEditName = FALSE;

							return;
						}
					}

					if (str.Compare(pCurScene->m_name.c_str()) != 0)
					{
						my_app.Commands().Do(new CCmdRenameScene(str));
					}
				}
			}
		}

		m_edit.ShowWindow(SW_HIDE);
		m_edit.SetWindowText(NULL);

		RedrawItem(m_nEditName);

		m_nEditName = -1;
	}

	m_bInEndEditName = FALSE;
}

void CSceneListCtrl::OnSceneRename()
{
	int index = GetCurSel();
	if (index >= 0)
		BeginEditName(index);
}

void CSceneListCtrl::OnNameEditKillFocus()
{
	EndEditName(TRUE);
}

void CSceneListCtrl::OnNameEditChange()
{
	m_edit.RecalcWindowWidth();
}

void CSceneListCtrl::OnDeleteItem(int index)
{
	EndEditName(FALSE);
	CancelDelayEditName();

	DWORD_PTR data = GetItemData(index);
	_ItemData* pItemData = (_ItemData*)data;
	delete pItemData;
}

void CSceneListCtrl::OnNameEditKeyEnter()
{
	EndEditName(TRUE);
}

void CSceneListCtrl::OnNameEditKeyEscape()
{
	EndEditName(FALSE);
}

void CSceneListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int nIndex = ItemFromPoint(point);
	if (nIndex >= 0 && IsItemSelected(nIndex) && GetFocus() == this)
		m_nLButtonDownOnSelected = nIndex;
	else
		m_nLButtonDownOnSelected = -1;

	CFlexListCtrl::OnLButtonDown(nFlags, point);

	if (nIndex >= 0)
	{
		int nIndexNew;
		BOOL bBefore;
		BOOL bCopy;
		if (TrackMoveScene(point, nIndexNew, bBefore, bCopy))
		{
			if (!bBefore)
				nIndexNew++;

			if (!bCopy)
			{
				if (nIndexNew == nIndex || nIndexNew == nIndex + 1)
				{
					MessageBeep(0);
					return;
				}

				if (nIndex < nIndexNew)
					nIndexNew--;
			}

			gldScene2 *pScene = GetScene(nIndex);
			ASSERT(pScene != NULL);
			if (!bCopy)
			{
				// change current scene to NULL, or the last CCmdChangeCurrentScene
				// can not be executed successfully (because pOldScene == pNewScene).
				TCommandGroup *pCmd = new TCommandGroup(IDS_CMD_MOVESCENE);
				pCmd->Do(new CCmdChangeCurrentScene(NULL));
				pCmd->Do(new CCmdDeleteScene(pScene, false));
				pCmd->Do(new CCmdInsertScene(pScene, nIndexNew, false));
				pCmd->Do(new CCmdChangeCurrentScene(pScene));
				my_app.Commands().Do(pCmd);
			}
			else
			{
				// Copy scene
				HGLOBAL hMem = NULL;
				WriteSceneToGlobal(pScene, &hMem);
				if (hMem != NULL)
				{
					AddSceneFromGlobal(hMem, nIndexNew);
					GlobalFree(hMem);
				}
			}
		}
	}
}

BOOL CSceneListCtrl::TrackMoveScene(const CPoint point, int &nIndex, BOOL &bBefore, BOOL &bCopy)
{
	UpdateWindow();

	SetCapture();
	SetFocus();

	CRect rc;
	GetClientRect(rc);

	CDC *pDC = GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);

	BOOL bInsertionMarkerDrawn = FALSE;
	int nOldIndex = GetCurSel();
	BOOL bOldBefore = FALSE;

	BOOL bMoved = FALSE;
	BOOL bFirstMove = TRUE;

	BOOL bRet = FALSE;

	int xDrag = GetSystemMetrics(SM_CXDRAG);
	int yDrag = GetSystemMetrics(SM_CYDRAG);

	m_bScrolled = FALSE;

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
			{
				int x = GET_X_LPARAM(msg.lParam);
				int y = GET_Y_LPARAM(msg.lParam);
				if (!bMoved)
				{
					if (abs(x - point.x) >= xDrag || abs(y - point.y) >= yDrag)
						bMoved = TRUE;
					else
						continue;
				}

				CancelDelayEnsureVisible();

				::SetCursor(AfxGetApp()->LoadCursor(IsControlDown() ? IDC_DRAG_SINGLE_COPY : IDC_DRAG_SINGLE));

				int nNewIndex;
				BOOL bNewBefore;
				VERIFY(GetNearestItem(y, nNewIndex, bNewBefore));

				TRACE("nOldIndex = %d, nNewIndex = %d\n", nOldIndex, nNewIndex);

				if (y < rc.top + 15)
					EnableAutoScroll(top);
				else if (y > rc.bottom - 15)
					EnableAutoScroll(bottom);
				else
					EnableAutoScroll(none);

				if (bFirstMove || nOldIndex != nNewIndex || bOldBefore != bNewBefore || m_bScrolled)
				{
					bFirstMove = FALSE;

					if(bInsertionMarkerDrawn && !m_bScrolled)
					{
						TRACE1("Erase insertion marker: %d\r\n", nOldIndex);
						DrawInsertionMarker(pDC, nOldIndex, bOldBefore);
						bInsertionMarkerDrawn = FALSE;
					}

					nOldIndex = nNewIndex;
					bOldBefore = bNewBefore;

					TRACE1("Draw insertion marker: %d\n", nOldIndex);
					DrawInsertionMarker(pDC, nOldIndex, bOldBefore);
					bInsertionMarkerDrawn = TRUE;
					m_bScrolled = FALSE;
				}

				continue;
			}

			case WM_LBUTTONUP:
				SetCursor(NULL);
				ReleaseCapture();

				if (bMoved)
				{
					if (bInsertionMarkerDrawn && !m_bScrolled)
					{
						DrawInsertionMarker(pDC, nOldIndex, bOldBefore);
						bInsertionMarkerDrawn = FALSE;
					}

					bRet = TRUE;

					goto _exit;
				}
				else
				{
					// continue to dispatch this message, so "rename" feature can work.
					break;
				}

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
				continue;

			case WM_SYSKEYDOWN:
				continue;

			case WM_KEYDOWN:
			case WM_KEYUP:
				if (msg.wParam == VK_ESCAPE)
				{
					ReleaseCapture();
					break;
				}
				else if (msg.wParam == VK_CONTROL)
				{
					::SetCursor(AfxGetApp()->LoadCursor(IsControlDown() ? IDC_DRAG_SINGLE_COPY : IDC_DRAG_SINGLE));
				}
				continue;

			default:
				break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (GetCapture() != this)
			break;
	}

	if(bInsertionMarkerDrawn && !m_bScrolled)
		DrawInsertionMarker(pDC, nOldIndex, bOldBefore);

_exit:
	EnableAutoScroll(none);
	
	ReleaseDC(pDC);

	if (bRet)
	{
		nIndex = nOldIndex;
		bBefore = bOldBefore;
		bCopy = IsControlDown();
	}

	return bRet;
}

void CSceneListCtrl::EnableAutoScroll(AUTOSCROLLDIR dir)
{
	if (m_nAutoScrollDir == none && dir != none)
		SetTimer(IDT_AUTO_SCROLL, 150, NULL);
	else if (m_nAutoScrollDir != none && dir == none)
		KillTimer(IDT_AUTO_SCROLL);

	m_nAutoScrollDir = dir;
}

void CSceneListCtrl::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	switch (nIDEvent)
	{
		case IDT_DELAY_EDIT_NAME:
		{
			if (m_nDelayEditName >= 0 && m_nDelayEditName == GetCurSel())
			{
				BeginEditName(m_nDelayEditName);
				CancelDelayEditName();
			}
			break;
		}

		case IDT_AUTO_SCROLL:
		{
			int nPos;
			switch (m_nAutoScrollDir)
			{
			case top:
				nPos = GetScrollPos(SB_VERT);
				if (nPos > 0)
				{
					SetScrollPos(SB_VERT, nPos - 1, TRUE);
					Invalidate(FALSE);
					UpdateWindow();

					m_bScrolled = TRUE;

					POINT point;
					GetCursorPos(&point);
					ScreenToClient(&point);
					PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
				}
				break;

			case bottom:
				nPos = GetScrollPos(SB_VERT);
				SetScrollPos(SB_VERT, nPos + 1, TRUE);
				if (GetScrollPos(SB_VERT) != nPos)
				{
					Invalidate(FALSE);
					UpdateWindow();

					m_bScrolled = TRUE;

					POINT point;
					GetCursorPos(&point);
					ScreenToClient(&point);
					PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
				}
				break;
			}
			break;
		}
	}

	CFlexListCtrl::OnTimer(nIDEvent);
}

BOOL CSceneListCtrl::GetNearestItem(int y, int &index, BOOL &bBefore)
{
	index = -1;

	CRect rc;
	GetClientRect(&rc);

	CRect rcItem;

	int nFirst = GetFirstVisibleRow();
	GetItemRect(nFirst, &rcItem);
	if (y < rcItem.top)
	{
		index = nFirst;
	}
	else
	{
		int nCount = GetItemCount();
		int nVisible = GetVisibleRows();
		int nLast = min(nCount - 1, nFirst + nVisible - 1);
		GetItemRect(nLast, &rcItem);
		if (y > rcItem.bottom)
		{
			index = nLast;
		}
		else
		{
			int nSpace = m_sizeSpace.cy / 2 + m_sizeSpace.cy % 2;
			for (int i = nFirst; i <= nLast; i++)
			{
				int nTop = (i == nFirst ? 0 : nSpace);
				int nBottom = (i == nLast ? 0 : nSpace);
				GetItemRect(i, &rcItem);
				if (y >= rcItem.top - nTop && y <= rcItem.bottom + nBottom)
				{
					index = i;
					break;
				}
			}
		}
	}

	ASSERT(index >= 0);
	if (index == -1)
		return FALSE;

	GetItemRect(index, &rcItem);
	int yCenter = (rcItem.top + rcItem.bottom) / 2;
	bBefore = (y < yCenter);

	return TRUE;
}

void CSceneListCtrl::DrawInsertionMarker(CDC *pDC, int index, BOOL bBefore)
{
	int nROP2 = pDC->SetROP2(R2_NOTXORPEN);

	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
	CPen *pPenOld = pDC->SelectObject(&pen);

	if (bBefore)
		index--;

	int y;
	CRect rc;
	if (index >= 0)
	{
		GetItemRect(index, &rc);
		y = rc.bottom + m_sizeSpace.cy / 2;
	}
	else
	{
		GetItemRect(0, &rc);
		y = rc.top - m_sizeSpace.cy / 2;
	}

	// line
	pDC->MoveTo(rc.left, y);
	pDC->LineTo(rc.right, y);
	pDC->MoveTo(rc.left, y + 1);
	pDC->LineTo(rc.right, y + 1);

	// left end
	pDC->MoveTo(rc.left - 1, y - 1);
	pDC->LineTo(rc.left - 1, y + 3);

	pDC->MoveTo(rc.left - 2, y - 2);
	pDC->LineTo(rc.left - 2, y + 4);

	// right end
	pDC->MoveTo(rc.right, y - 1);
	pDC->LineTo(rc.right, y + 3);

	pDC->MoveTo(rc.right + 1, y - 2);
	pDC->LineTo(rc.right + 1, y + 4);

	pDC->SelectObject(pPenOld);

	pDC->SetROP2(nROP2);
}

void CSceneListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CFlexListCtrl::OnLButtonUp(nFlags, point);

	if (m_nLButtonDownOnSelected >= 0)
	{
		CRect rc;
		GetLabelRect(m_nLButtonDownOnSelected, rc, TRUE);
		if (rc.PtInRect(point))
			BeginDelayEditName(m_nLButtonDownOnSelected);
	}
}

void CSceneListCtrl::BeginDelayEditName(int index)
{
	CancelDelayEditName();
	ASSERT(m_nDelayEditName == -1);
	if (index >= 0)
	{
		m_nDelayEditName = index;
		SetTimer(IDT_DELAY_EDIT_NAME, 200, NULL);
	}
}

void CSceneListCtrl::CancelDelayEditName()
{
	if (m_nDelayEditName >= 0)
	{
		KillTimer(IDT_DELAY_EDIT_NAME);
		m_nDelayEditName = -1;
	}
}

void CSceneListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	EndEditName(FALSE);

	CFlexListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSceneListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int index = ItemFromPoint(point);
	if (index >= 0 && index == GetCurSel())
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_EDIT_SCENE_PROPERTIES, 0);

	CFlexListCtrl::OnLButtonDblClk(nFlags, point);
}

int CSceneListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFlexListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_SCENE));

	return 0;
}

BOOL CSceneListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		if (m_nEditName == -1 && m_hAccel && ::TranslateAccelerator(AfxGetMainWnd()->m_hWnd, m_hAccel, pMsg))
			return TRUE;

	return CFlexListCtrl::PreTranslateMessage(pMsg);
}

BOOL CSceneListCtrl::GetToolTipText(int index, CString &strText)
{
	_ItemData* pItemData = (_ItemData*)GetItemData(index);
	if (pItemData)
	{
		gldMainMovie2 *pMovie = _GetMainMovie2();
		int nStart = pMovie->GetSceneStartTime(pItemData->m_pScene);
		int nLength = pItemData->m_pScene->GetMaxTime(FALSE);
		float fFrameRate = pMovie->m_frameRate;
		strText.Format(IDS_SCENE_SOUND_TOOLTIPS_s_d_f_d_f, 
				pItemData->m_pScene->m_name.c_str(), 
				nStart + 1, 
				(float)nStart / fFrameRate, 
				nLength, 
				(float)nLength / fFrameRate);
		return TRUE;
	}
	return FALSE;
}

void CSceneListCtrl::OnSceneDupliate()
{
	gldScene2 *scene = GetSelectedScene();
	if (scene != NULL)
	{
		// Copy scene
		HGLOBAL hMem = NULL;
		WriteSceneToGlobal(scene, &hMem);
		if (hMem != NULL)
		{
			AddSceneFromGlobal(hMem, _GetMainMovie2()->m_sceneList.size());
			GlobalFree(hMem);
		}
	}
}

void CSceneListCtrl::OnUpdateSceneDupliate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedScene() != NULL);
}
