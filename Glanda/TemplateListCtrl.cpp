// TemplateListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "TemplateListCtrl.h"
#include "Graphics.h"
#include ".\templatelistctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// gldTemplateInfo
BOOL gldTemplateInfo::LoadTemplateInfo()
{
	LPOLESTR lpszCLSID = NULL;
	if (FAILED(::StringFromCLSID(clsid, &lpszCLSID)))
		return FALSE;

	CRegKey reg;

	CString strSubKey;
	USES_CONVERSION;
	strSubKey.Format("CLSID\\{E23DBDE5-A854-4a37-8A92-FA43F524F1D3}\\%s", OLE2A(lpszCLSID)); 
	::CoTaskMemFree(lpszCLSID);

	LONG ret = reg.Open(HKEY_CLASSES_ROOT, strSubKey, KEY_READ);
	if (ret != ERROR_SUCCESS)
		return FALSE;

	TCHAR szBuffer[_MAX_PATH];
	ULONG nCount;
	
	nCount = _MAX_PATH;
	if (reg.QueryStringValue("", szBuffer, &nCount) != ERROR_SUCCESS)
		return FALSE;
	CString strModule = szBuffer;

	nCount = _MAX_PATH;
	if (reg.QueryStringValue("Name", szBuffer, &nCount) != ERROR_SUCCESS)
		return FALSE;
	strName = szBuffer;

	nCount = _MAX_PATH;
	if (reg.QueryStringValue("Description", szBuffer, &nCount) != ERROR_SUCCESS)
		return FALSE;
	strDescription = szBuffer;

	if (reg.QueryDWORDValue("Priority", dwPriority) != ERROR_SUCCESS)
		dwPriority = (DWORD)-1;


	nCount = _MAX_PATH;
	if (reg.QueryStringValue("Bitmap", szBuffer, &nCount) != ERROR_SUCCESS)
		return FALSE;
	CString strBitmap = szBuffer;

	::PathUnquoteSpaces(strModule.GetBuffer());
	strModule.ReleaseBuffer();
	::PathRemoveFileSpec(strModule.GetBuffer());
	strModule.ReleaseBuffer();

	CString strFile;
	strFile.Format("%s\\%s", (LPCTSTR)strModule, (LPCTSTR)strBitmap);
	hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), strFile, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
#ifdef _DEBUG
	if (hBitmap == NULL)
		ReportLastError();
#endif

	reg.Close();

	return TRUE;
}

// CTemplateListCtrl

CTemplateListCtrl::CTemplateListCtrl()
{
	SetMargin(15, 15, 15, 15);
	SetItemSpace(15, 15);
	SetBkColor(COLOR_WINDOW, TRUE);
}

CTemplateListCtrl::~CTemplateListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTemplateListCtrl, CFlexListCtrl)
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CTemplateListCtrl message handlers

void CTemplateListCtrl::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
	gldTemplateInfo *pInfo = (gldTemplateInfo *)GetItemData(index);

	CRect rcImage = *pRect;
	rcImage.bottom = rcImage.top + IMAGE_BORDER + IMAGE_HEIGHT + IMAGE_BORDER;

	CPoint point = rcImage.CenterPoint();

	BITMAP bm;
	::GetObject(pInfo->hBitmap, sizeof(BITMAP), &bm);

	int x = point.x - bm.bmWidth / 2;
	int y = point.y - bm.bmHeight / 2;

	::DrawBitmap(pDC->m_hDC, x, y, bm.bmWidth, bm.bmHeight, pInfo->hBitmap, 0, 0);

	BOOL bSel = IsItemSelected(index);
	BOOL bFocused = (::GetFocus() == m_hWnd);

	CBrush *pbrBorder = CBrush::FromHandle(::GetSysColorBrush(bSel ? COLOR_HIGHLIGHT : COLOR_3DFACE));
	pDC->FrameRect(rcImage, pbrBorder);

	CRect rcText = *pRect;
	rcText.top = rcImage.bottom + IMAGE_TEXT_SPACING;
	rcText.InflateRect(-TEXT_PADDING, -TEXT_PADDING);

	CFont* pOldFont = pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

	CSize size = pDC->GetTextExtent(pInfo->strName);
	if (size.cx < rcText.Width())
	{
		int delta = (rcText.Width() - size.cx) / 2;
		rcText.left += delta;
		rcText.right -= delta;
	}

	if (bSel)
	{
		CBrush *pbrText = CBrush::FromHandle(GetSysColorBrush(bSel ? (bFocused ? COLOR_HIGHLIGHT : COLOR_BTNFACE) : COLOR_WINDOW));
		pDC->FillRect(&rcText, pbrText);
	}

	pDC->SetTextColor(GetSysColor((bSel && bFocused) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(pInfo->strName, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	pDC->SelectObject(pOldFont);

	if (bSel && bFocused)
	{
		CRect rcFocus = rcText;
		rcFocus.InflateRect(TEXT_PADDING, TEXT_PADDING);
		// important for DrawFocusRect to function correctly
		pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->DrawFocusRect(&rcFocus);
	}
}

void CTemplateListCtrl::CalcItemSize()
{
	CClientDC dc(this);
	int nTextHeight = 12;
	CFont *pFont = GetFont();
	if (pFont == NULL)
		pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	if (pFont)
	{
		CClientDC dc(this);
		CFont *pFontOld = dc.SelectObject(pFont);
		TEXTMETRIC tm;
		memset(&tm, 0, sizeof(tm));
		dc.GetTextMetrics(&tm);
		dc.SelectObject(pFont);

		nTextHeight = tm.tmHeight;
	}
	SetItemSize(IMAGE_WIDTH + IMAGE_BORDER * 2, 
		IMAGE_HEIGHT  + IMAGE_BORDER * 2 + IMAGE_TEXT_SPACING + nTextHeight + TEXT_PADDING * 2);
}

int CTemplateListCtrl::AddTemplate(gldTemplateInfo *pInfo)
{
	return InsertItem(GetItemCount(), (DWORD_PTR)pInfo);
}

REFCLSID CTemplateListCtrl::GetTemplate(int index)
{
	gldTemplateInfo *pInfo = (gldTemplateInfo *)GetItemData(index);
	return pInfo->clsid;
}

void CTemplateListCtrl::OnDeleteItem(int index)
{
	gldTemplateInfo *pInfo = (gldTemplateInfo *)GetItemData(index);
	delete pInfo;

	CFlexListCtrl::OnDeleteItem(index);
}

BOOL CTemplateListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	
	if (ItemFromPoint(point) >= 0)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_MY_HAND));
		return TRUE;
	}

	return CFlexListCtrl::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CTemplateListCtrl::GetToolTipText(int index, CString &strText)
{
	gldTemplateInfo *pInfo = (gldTemplateInfo *)GetItemData(index);
	strText.Format("%s\r\n%s", (LPCTSTR)pInfo->strName, (LPCTSTR)pInfo->strDescription);
	return TRUE;
}

void CTemplateListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CFlexListCtrl::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	int index = GetCurSel();
	if (index >= 0)
		RedrawItem(index);
}

void CTemplateListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CFlexListCtrl::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	int index = GetCurSel();
	if (index >= 0)
		RedrawItem(index);
}
