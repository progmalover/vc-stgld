// FontComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include ".\FontComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////
// CFontComboBoxEdit

BEGIN_MESSAGE_MAP(CFontComboBoxEdit, CSmartEdit)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CFontComboBoxEdit::OnPaint()
{
	if (::GetFocus() != m_hWnd)
	{
		CString strText;
		GetWindowText(strText);
		ASSERT(m_pComboBox);
		if (m_pComboBox->FindStringExact(-1, strText) < 0)
		{
			CPaintDC dc(this);

			CRect rc;
			GetClientRect(&rc);

			dc.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(IsWindowEnabled() ? COLOR_WINDOW : COLOR_3DFACE)));

			CFont *pOldFont = NULL;
			CFont *pFont = GetFont();
			if (pFont)
				pOldFont = dc.SelectObject(pFont);

			COLORREF crOldText = dc.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
			int nBkMode = dc.SetBkMode(TRANSPARENT);

			dc.DrawText(strText, strText.GetLength(), &rc, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX);

			dc.SetBkMode(nBkMode);
			dc.SetTextColor(crOldText);

			if (pFont)
				dc.SelectObject(pOldFont);

			return;
		}
	}

	Default();
}

/////////////////////////////////////////////////////////////////////////////
// CFontComboBox

CFontComboBox::CFontComboBox()
{
	m_bDroppedDown = FALSE;
	m_nCurSelTrascking = FALSE;

	m_nSize = 18;
	m_bBold = FALSE;
	m_bItalic = FALSE;
}

CFontComboBox::~CFontComboBox()
{
}


BEGIN_MESSAGE_MAP(CFontComboBox, CSmartComboBox)
	ON_WM_TIMER()
	ON_CONTROL_REFLECT_EX(CBN_CLOSEUP, OnCbnCloseup)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnCbnDropdown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontComboBox message handlers

BOOL CFontComboBox::EnumFonts()
{
	HDC hDC;
	
	// Get screen fonts
	hDC = ::GetWindowDC(NULL);
	
	LOGFONT lf;
	
	ZeroMemory(&lf,sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;

	if (!EnumFontFamiliesEx(
			hDC,	// handle to device context
			&lf,	// pointer to logical font information
			(FONTENUMPROC)EnumFamScreenCallBackEx,	// pointer to callback function
			(LPARAM) this,	// application-supplied data
			(DWORD) 0))
		return FALSE;

	::ReleaseDC(NULL,hDC);
	return TRUE;
}

BOOL CALLBACK AFX_EXPORT CFontComboBox::EnumFamScreenCallBackEx(ENUMLOGFONTEX* pelf, 
	NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis)

{
	if (_tcslen(pelf->elfLogFont.lfFaceName) > 0)
		if (FontType == TRUETYPE_FONTTYPE)
			if (pelf->elfLogFont.lfFaceName[0] != '@')
				if(((CFontComboBox*)pThis)->FindStringExact(-1,pelf->elfLogFont.lfFaceName)<0)
					((CFontComboBox*)pThis)->AddString(pelf->elfLogFont.lfFaceName);

	return 1; // Call me back
}

void CFontComboBox::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!IsDropDownList())
	{
		m_edit.m_pComboBox = this;
		m_edit.SubclassDlgItem(1001, this);
	}

	// bypass CSmartComboBox subclass
	//CSmartComboBox::PreSubclassWindow();
	CComboBox::PreSubclassWindow();
}

void CFontComboBox::PreviewCurrentFont()
{
	int index = GetCurSel();
	if (index >= 0)
	{
		// if current is not changed, do nothing
		if (index == m_nCurSelTrascking)
			return;
		m_nCurSelTrascking = index;

		int nTop = GetTopIndex();
		if (nTop >= 0)
		{
			int nHeight = GetItemHeight(nTop);

			CRect rc;
			GetWindowRect(&rc);

			// please note the ComboLBox's rect returned from 
			// GetDroppedControlRect() is incorrect (under Window XP)
			CRect rcList;
			GetDroppedControlRect(&rcList);
			int x = rcList.right;
			int y = rc.top;

			CString str;
			GetLBText(index, str);

			m_dlgPreview.Preview(x, y, str, m_nSize, m_bBold, m_bItalic);

			return;
		}
	}

	if (m_nCurSelTrascking >= 0)
		m_dlgPreview.ShowWindow(SW_HIDE);
}

void CFontComboBox::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1000)
	{
		ASSERT(GetDroppedState());
		PreviewCurrentFont();
	}

	CSmartComboBox::OnTimer(nIDEvent);
}

void CFontComboBox::SetPreviewParameters(int nSize, BOOL bBold, BOOL bItalic)
{
	m_nSize = nSize;
	m_bBold = bBold;
	m_bItalic = bItalic;
}

BOOL CFontComboBox::OnCbnDropdown()
{
	// TODO: Add your control notification handler code here

	m_nCurSelTrascking = -1;
	m_dlgPreview.Create(IDD_FONT_PREVIEW, GetDesktopWindow());
	SetTimer(1000, 50, NULL);

	return FALSE;
}

BOOL CFontComboBox::OnCbnCloseup()
{
	// TODO: Add your control notification handler code here

	KillTimer(1000);
	m_dlgPreview.DestroyWindow();

	return CSmartComboBox::OnCbnCloseup();
}
