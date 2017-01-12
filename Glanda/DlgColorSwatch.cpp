// DlgColorSwatch.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgColorSwatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DEFINE_REGISTERED_MESSAGE(WM_COLORCHANGE)
DEFINE_REGISTERED_MESSAGE(WM_COLORADVANCED)

const UINT IDT_SCREEN_CAPTURER	= 1000;

/////////////////////////////////////////////////////////////////////////////
// CDlgColorSwatch dialog

CDlgColorSwatch *CDlgColorSwatch::m_pInstance = NULL;

CDlgColorSwatch::CDlgColorSwatch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColorSwatch::IDD, pParent)
	, m_ptScreenCapturer(INT_MAX, INT_MAX)
	, m_bSendColorChanged(FALSE)
{
	//{{AFX_DATA_INIT(CDlgColorSwatch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgColorSwatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorSwatch)
	DDX_Control(pDX, IDC_STATIC_ADVANCED, m_btnAdvanced);
	DDX_Control(pDX, IDC_STATIC_COLOR_NEW, m_stcColor);
	DDX_Control(pDX, IDC_STATIC_SWATCH, m_stcSwatch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorSwatch, CDialog)
	ON_BN_CLICKED(IDC_STATIC_ADVANCED, OnAdvanced)
	ON_CONTROL(SWN_CHANGE, IDC_STATIC_SWATCH, OnSwatchChange)
	ON_WM_MOUSEACTIVATE()
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorSwatch message handlers

int CDlgColorSwatch::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	return MA_NOACTIVATE;

	//return CDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

BOOL CDlgColorSwatch::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_stcSwatch.AdjustSize();
	m_stcSwatch.SetAlwaysSelect(FALSE);

	m_btnAdvanced.LoadBitmap(IDB_COLOR_PALETTE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgColorSwatch::SetColor(COLORREF color, int alpha)
{
	m_stcSwatch.SetColor(color);
	m_stcColor.SetColor(color);
	m_stcColor.SetAlpha(alpha);

	if (color != CLR_INVALID)
	{
		SetDlgItemInt(IDC_STATIC_R, GetRValue(color), FALSE);
		SetDlgItemInt(IDC_STATIC_G, GetGValue(color), FALSE);
		SetDlgItemInt(IDC_STATIC_B, GetBValue(color), FALSE);

		int alpha2 = alpha * 100 / 255;
		if (alpha * 100 % 255 >= 128)
			alpha2++;

		SetDlgItemInt(IDC_STATIC_A, alpha2, FALSE);

		GetDlgItem(IDC_STATIC_R)->UpdateWindow();
		GetDlgItem(IDC_STATIC_G)->UpdateWindow();
		GetDlgItem(IDC_STATIC_B)->UpdateWindow();
	}
	else
	{
		SetDlgItemText(IDC_STATIC_R, NULL);
		SetDlgItemText(IDC_STATIC_G, NULL);
		SetDlgItemText(IDC_STATIC_B, NULL);

		SetDlgItemText(IDC_STATIC_A, NULL);
	}
}

void CDlgColorSwatch::OnSwatchChange()
{
	COLORREF color = m_stcSwatch.GetColor();
	SetColor(color, m_stcColor.GetAlpha());
	//GetOwner()->SendMessage(WM_COLORCHANGE, 0, 0L);
	//m_bSendColorChanged = FALSE;
	m_bSendColorChanged = TRUE;
}

void CDlgColorSwatch::RelayEvent(MSG *pMsg)
{
	ASSERT (::IsWindow(m_hWnd) && IsWindowVisible());

	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			CRect rc;
			GetWindowRect(&rc);
			if (!rc.PtInRect(pMsg->pt))
				ShowWindow(SW_HIDE);
		}
		break;

	case WM_LBUTTONUP:
		{
			CRect rc;
			GetOwner()->GetWindowRect(&rc);
			if (!rc.PtInRect(pMsg->pt))
				ShowWindow(SW_HIDE);
		}
		break;

	case WM_NCLBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCRBUTTONDOWN:

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		m_bSendColorChanged = FALSE;
		ShowWindow(SW_HIDE);
	}
}

COLORREF CDlgColorSwatch::GetColor()
{
	return m_stcColor.GetColor();
}

void CDlgColorSwatch::OnAdvanced() 
{
	// TODO: Add your control notification handler code here

	GetOwner()->SendMessage(WM_COLORADVANCED, 0, 0L);
}

CDlgColorSwatch * CDlgColorSwatch::Instance(void)
{
	if (!m_pInstance)
		m_pInstance = new CDlgColorSwatch;
	return m_pInstance;
}

void CDlgColorSwatch::ReleaseInstance(void)
{
	delete m_pInstance;
	m_pInstance = NULL;
}

void CDlgColorSwatch::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == IDT_SCREEN_CAPTURER)
	{
		if ((GetKeyState(VK_LBUTTON) >> (sizeof(SHORT) - 1)))
		{
			CPoint point;
			GetCursorPos(&point);
			if (point != m_ptScreenCapturer)
			{
				m_ptScreenCapturer = point;

				CRect rc;
				GetWindowRect(&rc);
				if (!rc.PtInRect(point))
				{
					CRect rcOwner;
					GetOwner()->GetWindowRect(&rcOwner);
					if (!rcOwner.PtInRect(point))
					{
						SetCapture();
						::SetCursor(AfxGetApp()->LoadCursor(IDC_PICKER));

						HDC hDC = ::GetDC(NULL);
						if (hDC)
						{
							COLORREF color = GetPixel(hDC, point.x, point.y);
							::ReleaseDC(NULL, hDC);

							TRACE1("%#06x\n", color);
							SetColor(color, m_stcColor.GetAlpha());

							m_bSendColorChanged = TRUE;
						}
					}
				}
				else
				{
					if (GetCapture() == this)
						ReleaseCapture();
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgColorSwatch::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here

	if (lpwndpos->flags & SWP_SHOWWINDOW)
	{
		// windows was disabled if we're in a modal dialog.
		EnableWindow(TRUE);

		m_ptScreenCapturer.x = INT_MAX;
		m_ptScreenCapturer.y = INT_MAX;

		m_bSendColorChanged = FALSE;

		SetTimer(IDT_SCREEN_CAPTURER, 1, NULL);
	}

	if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		KillTimer(IDT_SCREEN_CAPTURER);

		if (GetCapture() == this)
			ReleaseCapture();

		if (m_bSendColorChanged)
			GetOwner()->SendMessage(WM_COLORCHANGE, 0, 0L);
	}
}
