// StaticTip.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "StaticTip.h"

#include "MemDC.h"
#include "Graphics.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int MARGIN = 3;

// CStaticTip

IMPLEMENT_DYNAMIC(CStaticTip, CStatic)
CStaticTip::CStaticTip() : 
	m_bLButtonDown(FALSE)
{
}

CStaticTip::~CStaticTip()
{
}


BEGIN_MESSAGE_MAP(CStaticTip, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CStaticTip message handlers


void CStaticTip::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	DWORD dwStyle = GetStyle();

	CRect rc;
	GetClientRect(&rc);

	if (rc.Height() < 2)
		return;

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	dcMem.FrameRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));

	rc.InflateRect(-1, -1);
	dcMem.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_INFOBK)));

	// load bitmap (icon)
	static CBitmap bmpInfo;
	static BITMAP bm;
	if (!bmpInfo.m_hObject)
	{
		bmpInfo.LoadBitmap(IDB_INFORMATION_SMALL);
		bmpInfo.GetBitmap(&bm);
	}

	// get text
	CString strText;
	GetWindowText(strText);

	// select font
	CFont *pFont = GetParent()->GetFont();
	if (!pFont)
		pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont *pFontOld = dcMem.SelectObject(pFont);

	UINT nFlags = DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS;

	// calculate rects
	CRect rcIcon = rc;
	CRect rcText = rc;

	rcIcon.left += MARGIN;
	rcText.left += MARGIN + bm.bmWidth + MARGIN;
	if (dwStyle & SS_LEFTNOWORDWRAP)	// Single line
	{
		nFlags |=  DT_SINGLELINE | DT_VCENTER;

		rcIcon.top += (rc.Height() - bm.bmHeight) / 2;
	}
	else
	{
		nFlags |= DT_WORDBREAK;

		dcMem.DrawText(strText, &rcText, nFlags | DT_CALCRECT);
		rcText.OffsetRect(0, (rc.Height() - rcText.Height()) / 2);

		rcIcon.top = rcText.top + 1;
	}

	::DrawTransparent(&dcMem, rcIcon.left, rcIcon.top, bm.bmWidth, bm.bmHeight, &bmpInfo, 0, 0, RGB(255, 0, 255));

	dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextColor(::GetSysColor(COLOR_INFOTEXT));

	dcMem.DrawText(strText, &rcText, nFlags);

	dcMem.SelectObject(pFontOld);

	GetClientRect(&rc);
	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);
}

void CStaticTip::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (CanClick())
	{
		m_bLButtonDown = TRUE;
		SetCapture();
	}

	//CStatic::OnLButtonDown(nFlags, point);
}

void CStaticTip::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (CanClick() && m_bLButtonDown)
	{
		CRect rc;
		GetClientRect(&rc);
		if (rc.PtInRect(point))
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), STN_CLICKED), (LPARAM)m_hWnd);
		ReleaseCapture();
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void CStaticTip::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here

	if (CanClick() && m_bLButtonDown)
		m_bLButtonDown = FALSE;

	CStatic::OnCaptureChanged(pWnd);
}

BOOL CStaticTip::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	if (CanClick())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_MY_HAND));
		return TRUE;
	}
	else
	{
		return CStatic::OnSetCursor(pWnd, nHitTest, message);
	}
}

BOOL CStaticTip::CanClick()
{
	return ((GetStyle() & SS_NOTIFY) == SS_NOTIFY);
}
