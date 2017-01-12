// TipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "TipWnd.h"
#include "MemDC.h"

#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define X_MARGIN		5
#define Y_MARGIN		3

/////////////////////////////////////////////////////////////////////////////
// CTipWnd

IMPLEMENT_SINGLETON(CTipWnd)
CTipWnd::CTipWnd()
{
	m_nMaxWidth = 600;
	m_nCurrent = 0;
	m_pFuncInfo = NULL;
}

CTipWnd::~CTipWnd()
{
}


BEGIN_MESSAGE_MAP(CTipWnd, CWnd)
	//{{AFX_MSG_MAP(CTipWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTipWnd message handlers

BOOL CTipWnd::Create()
{
	if (!CWnd::Create(
		NULL, 
		NULL,
		WS_CHILD | WS_BORDER, 
		CRect(0, 0, 0, 0), 
		GetDesktopWindow(), 
		0))
		return FALSE;

	ModifyStyleEx(0, WS_EX_TOOLWINDOW);

	return TRUE;
}

void CTipWnd::SetParameterInfo(CFuncInfo *pFunc, int nCurrent)
{
	m_pFuncInfo = pFunc;
	
	BOOL bNameChanged = (m_pFuncInfo == NULL || m_pFuncInfo->m_strName.Compare(m_strName) != 0);

	m_strName = m_pFuncInfo->m_strName;
	m_strName += (m_pFuncInfo->m_aParams.size() > 0 ? "(" : "()");

	// Set current
	BOOL bCurrentChanged = (m_nCurrent != nCurrent);
	m_nCurrent = nCurrent;
	if ((RecalcLayout() || bNameChanged || bCurrentChanged) && IsWindowVisible())
	{
		Invalidate();
		UpdateWindow();
	}
}

// return TRUE if WindowRect is changed
BOOL CTipWnd::RecalcLayout()
{
	m_aParamRect.clear();

	CDC *pDC = GetDC();

	CFont *pFontOld = pDC->SelectObject(&m_fntNormal);

	int cParameters = m_pFuncInfo->m_aParams.size();

	CSize sizeName = pDC->GetTextExtent(m_strName);
	m_rcName.SetRect(X_MARGIN, Y_MARGIN, X_MARGIN + sizeName.cx, Y_MARGIN + sizeName.cy);
	
	pDC->SelectObject(pFontOld);


	BOOL bMulLine = FALSE;

	int xStart = m_rcName.right;
	int yStart = Y_MARGIN;

	int nMaxRight = xStart;

	for (int i = 0; i < cParameters; i++)
	{
		if (i != m_nCurrent)
			pFontOld = pDC->SelectObject(&m_fntNormal);
		else
			pFontOld = pDC->SelectObject(&m_fntBold);

		CSize sizeParameter = pDC->GetTextExtent(m_pFuncInfo->m_aParams[i]->m_strName + (i < cParameters - 1 ? ", " : ")"));

		if (xStart + sizeParameter.cx + X_MARGIN > m_nMaxWidth)
		{
			xStart = m_rcName.right;
			yStart += sizeName.cy;

			bMulLine = TRUE;
		}
		CRect rcParameter(xStart, yStart, xStart + sizeParameter.cx, yStart + sizeParameter.cy);
		m_aParamRect.push_back(rcParameter);
		xStart += sizeParameter.cx;
		nMaxRight = max(nMaxRight, xStart);

		pDC->SelectObject(pFontOld);
	}

	if (!m_pFuncInfo->m_strDesc.IsEmpty())
	{
		pDC->SelectObject(&m_fntNormal);
		CSize sizeDesc = pDC->GetTextExtent(m_pFuncInfo->m_strDesc);
		pDC->SelectObject(pFontOld);
		if (X_MARGIN + sizeDesc.cx > nMaxRight)
			nMaxRight = min(m_nMaxWidth - X_MARGIN, X_MARGIN + sizeDesc.cx);
	}

	CRect rc;
	GetWindowRect(&rc);
	CRect rcOld = rc;


	rc.right = rc.left + nMaxRight + X_MARGIN;
	rc.bottom = rc.top + yStart + sizeName.cy + Y_MARGIN;

	if (!m_pFuncInfo->m_strDesc.IsEmpty())
	{
		pDC->SelectObject(&m_fntNormal);

		m_rcDesc.left = X_MARGIN;
		m_rcDesc.top = rc.Height() + Y_MARGIN;
		m_rcDesc.right = rc.Width() - X_MARGIN;
		m_rcDesc.bottom = m_rcDesc.top + 1;
		pDC->DrawText(m_pFuncInfo->m_strDesc, &m_rcDesc, DT_CALCRECT | DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
		rc.bottom += Y_MARGIN + m_rcDesc.Height() + Y_MARGIN;

		pDC->SelectObject(pFontOld);
	}

	rc.bottom += 2;

	ReleaseDC(pDC);

	EnsureWholeRectVisible(rc);
	SetWindowPos(&wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), 0);
	
	return (rc != rcOld);
}

void CTipWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here	

	CGlandaMemDC dcMem(&dc);

	CRect rc;
	GetClientRect(&rc);

	dcMem.FillSolidRect(&rc, ::GetSysColor(COLOR_INFOBK));

	rc.InflateRect(-X_MARGIN, -Y_MARGIN);

	CFont *pFontOld = dcMem.SelectObject(&m_fntNormal);

	int cParameters = m_pFuncInfo->m_aParams.size();
	dcMem.DrawText(m_strName, -1, &m_rcName, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
	
	dcMem.SelectObject(pFontOld);


	for (int i = 0; i < cParameters; i++)
	{
		if (i != m_nCurrent)
			pFontOld = dcMem.SelectObject(&m_fntNormal);
		else
			pFontOld = dcMem.SelectObject(&m_fntBold);

		dcMem.DrawText(m_pFuncInfo->m_aParams[i]->m_strName, -1, &m_aParamRect[i], DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);

		if (i == m_nCurrent)
			dcMem.SelectObject(&m_fntNormal);

		CString strExt = (i < cParameters - 1 ? ", " : ")");
		CSize sizeExt = dcMem.GetTextExtent(strExt);
		CRect rcExt = m_aParamRect[i];
		rcExt.left = rcExt.right - sizeExt.cx;
		dcMem.DrawText(strExt, -1, &rcExt, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);

		dcMem.SelectObject(pFontOld);
	}

	if (!m_pFuncInfo->m_strDesc.IsEmpty())
	{
		pFontOld = dcMem.SelectObject(&m_fntNormal);
		dcMem.DrawText(m_pFuncInfo->m_strDesc, &m_rcDesc, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
		dcMem.SelectObject(pFontOld);
	}

	dcMem.SelectObject(pFontOld);

	// Do not call CWnd::OnPaint() for painting messages
}

void CTipWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonDown(nFlags, point);
}

int CTipWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default

	return MA_NOACTIVATE;
	
	//return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CTipWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
}

LRESULT CTipWnd::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	if (m_fntNormal.m_hObject)
		m_fntNormal.DeleteObject();
	if (m_fntBold.m_hObject)
		m_fntBold.DeleteObject();

	ASSERT(wParam);

	if (wParam)
	{
		LOGFONT lf;
		::GetObject((HFONT)wParam, sizeof(lf), &lf);

		m_fntNormal.CreateFontIndirect(&lf);
		lf.lfWeight = FW_BOLD;
		m_fntBold.CreateFontIndirect(&lf);

	}

	return TRUE;
}

BOOL CTipWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void CTipWnd::RelayEvent(MSG *pMsg)
{
	if (!::IsWindow(m_hWnd) || !IsWindowVisible())
		return;

	if (pMsg->message >= WM_LBUTTONDOWN && pMsg->message <= WM_MBUTTONDBLCLK ||
		pMsg->message >= WM_NCLBUTTONDOWN && pMsg->message <= WM_NCMBUTTONDBLCLK)
	{
		ShowWindow(SW_HIDE);
	}
	else
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			ShowWindow(SW_HIDE);
	}
}

BOOL CTipWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}
