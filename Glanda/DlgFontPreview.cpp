// DlgFontPreview.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "./DlgFontPreview.h"
#include ".\dlgfontpreview.h"


// CDlgFontPreview dialog

IMPLEMENT_DYNAMIC(CDlgFontPreview, CDialog)
CDlgFontPreview::CDlgFontPreview(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFontPreview::IDD, pParent)
{
}

CDlgFontPreview::~CDlgFontPreview()
{
}

void CDlgFontPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FONT, m_stcFont);
}


BEGIN_MESSAGE_MAP(CDlgFontPreview, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgFontPreview message handlers

BOOL CDlgFontPreview::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_stcFont.ModifySTStyle(0, STS_AUTOSIZE);
	m_stcFont.SetBkColor(::GetSysColor(COLOR_INFOBK));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFontPreview::Preview(int x, int y, LPCTSTR lpszFont, int nSize, BOOL bBold, BOOL bItalic)
{
	m_stcFont.SetRedraw(FALSE);

	m_stcFont.SetWindowText(lpszFont);
	m_stcFont.SetFontFaceName(lpszFont);
	m_stcFont.SetFontSize(nSize);
	m_stcFont.SetFontBold(bBold);
	m_stcFont.SetFontItalic(bItalic);

	CRect rc;
	m_stcFont.GetWindowRect(&rc);
	
	rc.InflateRect(5, 5);
	rc.OffsetRect(x - rc.left, y - rc.top);
	
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
	if (rc.bottom > cyScreen)
		rc.OffsetRect(0, cyScreen - rc.bottom);

	SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE);

	m_stcFont.SetWindowPos(NULL, 5, 5, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	m_stcFont.SetRedraw(TRUE);
	
	Invalidate(FALSE);
	UpdateWindow();
}

void CDlgFontPreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(&rc, CBrush::FromHandle(::GetSysColorBrush(COLOR_INFOBK)));
}
