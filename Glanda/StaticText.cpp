// StaticText.cpp : implementation file
//

#include "stdafx.h"
#include "StaticText.h"
#include "MemDC.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticText

CStaticText::CStaticText()
{
	m_dwStyle = STS_AUTOSIZE;

	m_crText = GetSysColor(COLOR_WINDOWTEXT);
	m_crBack = CLR_INVALID;

	m_bDynamicCreate = FALSE;
	m_bInit = FALSE;
}

CStaticText::~CStaticText()
{
}

BEGIN_MESSAGE_MAP(CStaticText, CStatic)
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticText message handlers

void CStaticText::CreateFont(CFont &font)
{
	if (font.CreateFont(m_nSize, 0, 0, 0, 
		m_bBold ? FW_BOLD : FW_NORMAL, (BYTE)m_bItalic, (BYTE)m_bUnderline, 
		0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, FF_DONTCARE | DEFAULT_PITCH, m_strFaceName))
		return;

	ASSERT(FALSE);
	font.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
}

void CStaticText::AutoSize()
{
	ASSERT(m_dwStyle & STS_AUTOSIZE);


	CString str;
	GetWindowText(str);

	CFont font;
	CreateFont(font);

	CClientDC dc(this);
	CFont *pOldFont = (CFont *)dc.SelectObject(&font);
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(pOldFont);
	size.cx += 2;
	size.cy += 2;

	SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	Invalidate(FALSE);
}

BOOL CStaticText::SetFontDirect(CFont *pFont)
{
	ASSERT_VALID(pFont);

	LOGFONT lf;
	pFont->GetLogFont(&lf);

	m_strFaceName = lf.lfFaceName;
	m_nSize = lf.lfHeight;
	m_bBold = (lf.lfWeight >= FW_BOLD);
	m_bItalic = lf.lfItalic;
	m_bUnderline = lf.lfUnderline;

	if(m_dwStyle & STS_AUTOSIZE)
		AutoSize();
	else
		Invalidate(FALSE);

	return TRUE;
}

BOOL CStaticText::SetFontSize(int nSize)
{
	CClientDC dc(this);
	m_nSize = -MulDiv(nSize, GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);

	if(m_dwStyle & STS_AUTOSIZE)
		AutoSize();
	else
		Invalidate(FALSE);

	return TRUE;
}

BOOL CStaticText::SetFontBold(BOOL bBold)
{
	return ModifySTStyle(bBold ? 0 : STS_BOLD, bBold ? STS_BOLD : 0);
}

BOOL CStaticText::SetFontItalic(BOOL bItalic)
{
	return ModifySTStyle(bItalic ? 0 : STS_ITALIC, bItalic ? STS_ITALIC : 0);
}

BOOL CStaticText::SetFontUnderline(BOOL bUnderline)
{
	return ModifySTStyle(bUnderline ? 0 : STS_UNDERLINE, bUnderline ? STS_UNDERLINE : 0);
}

BOOL CStaticText::SetFontFaceName(LPCTSTR lpszFaceName)
{
	ASSERT(strlen(lpszFaceName) < LF_FACESIZE);	// length<=32, include NULL
	if(!(strlen(lpszFaceName) < LF_FACESIZE))
		return FALSE;

	m_strFaceName = lpszFaceName;

	if(m_dwStyle & STS_AUTOSIZE)
		AutoSize();
	else
		Invalidate(FALSE);

	return TRUE;
}

BOOL CStaticText::SetTextColor(COLORREF crText)
{
	m_crText = crText;
	InvalidateRect(NULL);
	return TRUE;
}

BOOL CStaticText::SetBkColor(COLORREF rgb)
{	
	m_crBack = rgb;
	Invalidate(FALSE);
	return TRUE;
}

LRESULT CStaticText::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = Default();
	if (m_dwStyle & STS_AUTOSIZE)
		AutoSize();
	else
		Invalidate();

	return TRUE;
}

void CStaticText::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	DWORD dwStyle = GetStyle();

	CRect rc;
	GetClientRect(&rc);

	if (rc.Width() == 0 || rc.Height() == 0)
		return;

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	if (m_crBack != CLR_INVALID)
	{
		dcMem.FillSolidRect(&rc, GetBkColor());
	}
	else
	{
		if (g_xpStyle.IsAppThemed())
			g_xpStyle.DrawThemeParentBackground(m_hWnd, dcMem, &rc);
		else
			dcMem.FillSolidRect(&rc, FACE_COLOR);
	}

	// get text
	CString strText;
	GetWindowText(strText);

	CFont font;
	CreateFont(font);

	// select font
	CFont *pFontOld = dcMem.SelectObject(&font);

	UINT nFlags = 0;

	if (dwStyle & SS_CENTER)
		nFlags |= DT_CENTER;
	else if (dwStyle & SS_RIGHT)
		nFlags |= DT_RIGHT;

	if (dwStyle & SS_CENTERIMAGE)
		nFlags |= DT_SINGLELINE | DT_VCENTER;

	if (dwStyle & SS_LEFTNOWORDWRAP)
		nFlags |=  DT_SINGLELINE | DT_VCENTER;
	else
		nFlags |= DT_WORDBREAK;

	if (dwStyle & SS_NOPREFIX)
		nFlags |= DT_NOPREFIX;

	if (dwStyle & SS_ENDELLIPSIS)
		nFlags |= DT_END_ELLIPSIS;

	if (dwStyle & SS_PATHELLIPSIS)
		nFlags |= SS_PATHELLIPSIS;

	dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextColor(IsWindowEnabled() ? m_crText : ::GetSysColor(COLOR_GRAYTEXT));

	dcMem.DrawText(strText, &rc, nFlags);

	dcMem.SelectObject(pFontOld);

	GetClientRect(&rc);
	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);
}

BOOL CStaticText::Init()
{
	CWnd *pWnd = GetParent();
	ASSERT(pWnd);
	CFont *pFont = pWnd->GetFont();
	ASSERT(pFont);
	if (!pFont)
		pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));

	SetFontDirect(pFont);

	m_bInit = TRUE;

	return TRUE;
}

BOOL CStaticText::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
	m_bDynamicCreate = TRUE;
	BOOL ret = CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
	return ret ? Init() : FALSE;
}

void CStaticText::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	CStatic::PreSubclassWindow();

	if (!m_bDynamicCreate && !m_bInit)
		Init();
}

COLORREF CStaticText::GetBkColor()
{
	return m_crBack;
}

COLORREF CStaticText::GetTextColor()
{
	return m_crText;
}

BOOL CStaticText::ModifySTStyle(DWORD dwRemove, DWORD dwAdd)
{
	m_dwStyle &= ~dwRemove;
	m_dwStyle |= dwAdd;

	if ((dwRemove | dwAdd) & (STS_BOLD | STS_ITALIC | STS_UNDERLINE))
	{
		m_bBold = (m_dwStyle & STS_BOLD) != 0;
		m_bItalic = (m_dwStyle & STS_ITALIC) != 0;
		m_bUnderline = (m_dwStyle & STS_UNDERLINE) != 0;

		if(m_dwStyle & STS_AUTOSIZE)
			AutoSize();
		else
			Invalidate(FALSE);
	}

	return TRUE;
}

void CStaticText::OnEnable(BOOL bEnable)
{
	Invalidate();
}
