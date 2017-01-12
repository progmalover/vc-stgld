// CheckMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "CheckMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_WIDTH		300
#define MAX_WIDTH		640

/////////////////////////////////////////////////////////////////////////////
// CCheckMessageBox dialog


CCheckMessageBox::CCheckMessageBox(LPCTSTR lpszCaption, LPCTSTR lpszText, LPCTSTR lpszCheckText, BOOL bChecked, UINT nType, CWnd* pParent)
	: CDialog(CCheckMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckMessageBox)
	m_strText = _T("");
	m_bChecked = FALSE;
	//}}AFX_DATA_INIT

	m_strCaption = lpszCaption;
	m_strText = lpszText;
	m_strCheckText = lpszCheckText;
	m_bChecked = bChecked;
	m_nType = nType;
}


void CCheckMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckMessageBox)
	DDX_Control(pDX, IDC_STATIC_ICON, m_stcIcon);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_strText);
	DDX_Check(pDX, IDC_CHECK, m_bChecked);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckMessageBox, CDialog)
	//{{AFX_MSG_MAP(CCheckMessageBox)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDOK, IDNO, OnButtonClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckMessageBox message handlers

BOOL CCheckMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	SetWindowText(m_strCaption);

	SetDlgItemText(IDC_STATIC_TEXT, m_strText);
	SetDlgItemText(IDC_CHECK, m_strCheckText);

	RecalcLayout();

	/*
	#define MB_ICONHAND                 0x00000010L
	#define MB_ICONQUESTION             0x00000020L
	#define MB_ICONEXCLAMATION          0x00000030L
	#define MB_ICONASTERISK             0x00000040L
	*/
	UINT nIcon = m_nType & MB_ICONMASK;
	LPCTSTR lpszIcon;
	switch (nIcon)
	{
	case MB_ICONHAND:
		lpszIcon = IDI_HAND;
		break;
	case MB_ICONQUESTION:
		lpszIcon = IDI_QUESTION;
		break;
	case MB_ICONEXCLAMATION:
		lpszIcon = IDI_EXCLAMATION;
		break;
	case MB_ICONASTERISK:
		lpszIcon = IDI_ASTERISK;
		break;
	default:
		lpszIcon = NULL;
		break;
	}

	m_stcIcon.SetIcon(AfxGetApp()->LoadStandardIcon(lpszIcon));

	/*
	#define MB_OK                       0x00000000L
	#define MB_OKCANCEL                 0x00000001L
	#define MB_ABORTRETRYIGNORE         0x00000002L
	#define MB_YESNOCANCEL              0x00000003L
	#define MB_YESNO                    0x00000004L
	#define MB_RETRYCANCEL              0x00000005L
	*/
	UINT nType = m_nType & MB_TYPEMASK;
	switch (nType)
	{
	case MB_OK:
		m_cButtons = 1;
		m_buttons[0].nID = IDOK;
		break;

	case MB_OKCANCEL:
		m_cButtons = 2;
		m_buttons[0].nID = IDOK;
		m_buttons[1].nID = IDCANCEL;
		break;

	case MB_YESNOCANCEL:
		m_cButtons = 3;
		m_buttons[0].nID = IDYES;
		m_buttons[1].nID = IDNO;
		m_buttons[2].nID = IDCANCEL;
		break;

	case MB_YESNO:
		m_cButtons = 2;
		m_buttons[0].nID = IDYES;
		m_buttons[1].nID = IDNO;
		break;

	default:
		ASSERT(FALSE);
		m_cButtons = 1;
		m_buttons[0].nID = IDOK;
		break;
	}

	/*
	#define IDOK                1
	#define IDCANCEL            2
	#define IDABORT             3
	#define IDRETRY             4
	#define IDIGNORE            5
	#define IDYES               6
	#define IDNO                7
	*/
	const UINT nCaptionIDs[] = 
	{
		IDS_MB_OK, 
		IDS_MB_CANCEL, 
		IDS_MB_ABORT, 
		IDS_MB_RETRY, 
		IDS_MB_IGNORE, 
		IDS_MB_YES, 
		IDS_MB_NO
	};

	CString strCaptions[7];
	for (int i = 0; i < sizeof(nCaptionIDs) / sizeof(UINT); i++)
		strCaptions[i].LoadString(nCaptionIDs[i]);

	CFont *pFont = GetFont();
	for (int i = 0; i < m_cButtons; i++)
	{
		UINT nStyle = WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP| BS_TEXT | BS_PUSHBUTTON;
		if (i == 0)
			nStyle |= WS_GROUP;

		m_buttons[i].button.Create(strCaptions[m_buttons[i].nID - 1], nStyle, CRect(0, 0, 0, 0), this, m_buttons[i].nID);
		m_buttons[i].button.ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY);
		m_buttons[i].button.SetFont(pFont);
	}


	/*
	#define MB_DEFBUTTON1               0x00000000L
	#define MB_DEFBUTTON2               0x00000100L
	#define MB_DEFBUTTON3               0x00000200L
	*/
	UINT nDef = m_nType & MB_DEFMASK;
	nDef >>= 8;
	if (!((int)nDef >= 0 && (int)nDef < m_cButtons))
		nDef = 0;
	SendMessage(DM_SETDEFID, m_buttons[nDef].nID, 0L);
	GotoDlgCtrl(&m_buttons[nDef].button);

	
	CRect rc;
	GetClientRect(&rc);

	CRect rcCenter(0, 0, 50, 14);
	MapDialogRect(&rcCenter);
	rcCenter.OffsetRect((rc.Width() - rcCenter.Width()) / 2, rc.Height() - 12 - rcCenter.Height());

	CRect rcButton;
	switch (m_cButtons)
	{
	case 1:
		rcButton = rcCenter;
		m_buttons[0].button.MoveWindow(&rcButton);
		break;

	case 2:
		rcButton = rcCenter;
		rcButton.OffsetRect(-(rcButton.Width() / 2 + 6), 0);
		m_buttons[0].button.MoveWindow(&rcButton);

		rcButton = rcCenter;
		rcButton.OffsetRect((rcButton.Width() / 2 + 6), 0);
		m_buttons[1].button.MoveWindow(&rcButton);
		break;

	case 3:
		rcButton = rcCenter;
		m_buttons[1].button.MoveWindow(&rcButton);

		rcButton = rcCenter;
		rcButton.OffsetRect(-(rcButton.Width() + 12), 0);
		m_buttons[0].button.MoveWindow(&rcButton);

		rcButton = rcCenter;
		rcButton.OffsetRect((rcButton.Width() + 12), 0);
		m_buttons[2].button.MoveWindow(&rcButton);
		break;
	}

	CenterWindow();

	MessageBeep(nIcon);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckMessageBox::RecalcLayout()
{
	CClientDC dc(this);
	CFont *pFont = GetFont();
	CFont *pOldFont = dc.SelectObject(pFont);

	CString strText = m_strText;
	
	/*
	strText.Replace("\r\n", "\n");
	int nMax = 0;
	CString strLine;
	for (int i = 0; ;i++)
	{
		if (!AfxExtractSubString(strLine, strText, i, '\n'))
			break;

		CSize size = dc.GetTextExtent(strLine);
		if (size.cx > nMax)
			nMax = size.cx;
	}
	
	TRACE1("nMax: %d\n", nMax);
	*/


	CRect rcIcon;
	GetDlgItem(IDC_STATIC_ICON)->GetWindowRect(&rcIcon);
	ScreenToClient(&rcIcon);

	CRect rcText;
	GetDlgItem(IDC_STATIC_TEXT)->GetWindowRect(&rcText);
	ScreenToClient(&rcText);

	rcText.right = rcText.left + ::GetSystemMetrics(SM_CXSCREEN) / 3;

	/*
	rcText.right = rcText.left + nMax;
	if (rcIcon.left * 4 + rcIcon.Width() + rcText.Width() < MIN_WIDTH)
		rcText.right = rcText.left + (MIN_WIDTH - rcIcon.left * 4 + rcIcon.Width());
	if (rcIcon.left * 4 + rcIcon.Width() + rcText.Width() > MAX_WIDTH)
		rcText.right = rcText.left + (MAX_WIDTH - rcIcon.left * 4 + rcIcon.Width());
	*/

	dc.DrawText(m_strText, &rcText, DT_CALCRECT | DT_WORDBREAK);

	dc.SelectObject(pOldFont);

	//rcText.right += 4;
	//rcText.bottom += 4;

	if (rcIcon.left * 4 + rcIcon.Width() + rcText.Width() < MIN_WIDTH)
		rcText.right = rcText.left + (MIN_WIDTH - rcIcon.left * 4 + rcIcon.Width());
	if (rcIcon.left * 4 + rcIcon.Width() + rcText.Width() > MAX_WIDTH)
		rcText.right = rcText.left + (MAX_WIDTH - rcIcon.left * 4 + rcIcon.Width());

	GetDlgItem(IDC_STATIC_TEXT)->MoveWindow(&rcText);

	CRect rcCheck;
	GetDlgItem(IDC_CHECK)->GetWindowRect(&rcCheck);
	ScreenToClient(&rcCheck);
	rcCheck.OffsetRect(0, rcText.bottom - rcCheck.top + 24);
	GetDlgItem(IDC_CHECK)->MoveWindow(&rcCheck);


	CRect rc;
	GetWindowRect(&rc);
	rc.right = rc.left + rcIcon.left * 6 + rcIcon.Width() + rcText.Width();
	rc.bottom = GetSystemMetrics(SM_CYCAPTION) + rc.top + rcIcon.top * 2 + rcText.Height() + 90;

	MoveWindow(&rc);
}

void CCheckMessageBox::OnButtonClick(UINT nID)
{
	// OnOK will be trigger always in CDialog::PreTranslateMesage
	
	BOOL bFound = FALSE;
	for (int i = 0; i < m_cButtons; i++)
	{	
		if (m_buttons[i].nID == nID)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		UpdateData(TRUE);
		EndDialog(nID);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CCheckMessageBox::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	BOOL bFound = FALSE;
	for (int i = 0; i < m_cButtons; i++)
	{
		if (m_buttons[i].nID == IDCANCEL)
		{
			bFound = TRUE;
			break;
		}
		if (m_buttons[i].nID == IDNO)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
		EndDialog(IDCANCEL);
	else
		EndDialog(IDOK);

	CDialog::OnClose();
}

BOOL CCheckMessageBox::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		OnClose();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

UINT CheckMessageBox(UINT nIDText, UINT nIDCheckText, BOOL &bChecked, UINT nType, UINT nIDCaption, CWnd* pParent)
{
	CString strText, strCheckText, strCaption;
	strText.LoadString(nIDText);
	strCheckText.LoadString(nIDCheckText);
	if (nIDCaption > 0)
		strCaption.LoadString(nIDCaption);
	return CheckMessageBox(strText, strCheckText, bChecked, nType, nIDCaption > 0 ? (LPCTSTR)strCaption : NULL, pParent);
}

UINT CheckMessageBox(LPCTSTR lpszText, LPCTSTR lpszCheckText, BOOL &bChecked, UINT nType, LPCTSTR lpszCaption, CWnd* pParent)
{
	if (pParent == NULL)
		pParent = AfxGetMainWnd();

	if (lpszCaption == NULL)
		lpszCaption = AfxGetApp()->m_pszAppName;

	CCheckMessageBox dlg(
		lpszCaption, 
		lpszText, 
		lpszCheckText, 
		bChecked, 
		nType);

	UINT ret = dlg.DoModal();
	bChecked = dlg.m_bChecked;
	
	return ret;
}
