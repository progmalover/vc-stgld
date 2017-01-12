// DlgColorAdvanced.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgColorAdvanced.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TCHAR szColorSwatch[];

/////////////////////////////////////////////////////////////////////////////
// CDlgColorAdvanced dialog


CDlgColorAdvanced::CDlgColorAdvanced(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColorAdvanced::IDD, pParent)
	, m_strHEXValue(_T(""))
{
	//{{AFX_DATA_INIT(CDlgColorAdvanced)
	m_nHValue = 0;
	m_nSValue = 0;
	m_nVValue = 0;
	m_nRValue = 0;
	m_nGValue = 0;
	m_nBValue = 0;
	m_nAValue = 100;
	//}}AFX_DATA_INIT
	
	m_bEnableAlpha = TRUE;

	m_crColorOld = RGB(m_nRValue, m_nGValue, m_nBValue);
	m_nAlphaOld = m_nAValue;

	m_bInit = FALSE;
}


void CDlgColorAdvanced::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorAdvanced)
	DDX_Control(pDX, IDC_STATIC_SLIDER_A, m_btnA);
	DDX_Control(pDX, IDC_STATIC_COLOR_NEW, m_stcColorNew);
	DDX_Control(pDX, IDC_STATIC_COLOR_OLD, m_stcColorOld);
	DDX_Control(pDX, IDC_STATIC_SLIDER_B, m_btnB);
	DDX_Control(pDX, IDC_STATIC_SLIDER_G, m_btnG);
	DDX_Control(pDX, IDC_STATIC_SLIDER_R, m_btnR);
	DDX_Control(pDX, IDC_STATIC_SLIDER_V, m_btnV);
	DDX_Control(pDX, IDC_STATIC_SLIDER_S, m_btnS);
	DDX_Control(pDX, IDC_STATIC_SLIDER_H, m_btnH);
	DDX_Control(pDX, IDC_STATIC_SV, m_stcSV);
	DDX_Control(pDX, IDC_STATIC_H, m_stcH);
	DDX_Text(pDX, IDC_EDIT_H, m_nHValue);
	DDV_MinMaxUInt(pDX, m_nHValue, 0, 360);
	DDX_Text(pDX, IDC_EDIT_S, m_nSValue);
	DDV_MinMaxUInt(pDX, m_nSValue, 0, 100);
	DDX_Text(pDX, IDC_EDIT_V, m_nVValue);
	DDV_MinMaxUInt(pDX, m_nVValue, 0, 100);
	DDX_Text(pDX, IDC_EDIT_R, m_nRValue);
	DDV_MinMaxUInt(pDX, m_nRValue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_G, m_nGValue);
	DDV_MinMaxUInt(pDX, m_nGValue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_B, m_nBValue);
	DDV_MinMaxUInt(pDX, m_nBValue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_A, m_nAValue);
	DDV_MinMaxUInt(pDX, m_nAValue, 0, 100);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_HEX, m_edtHex);
	DDX_Text(pDX, IDC_EDIT_HEX, m_strHEXValue);
}


BEGIN_MESSAGE_MAP(CDlgColorAdvanced, CDialog)
	//{{AFX_MSG_MAP(CDlgColorAdvanced)
	ON_EN_CHANGE(IDC_EDIT_H, OnEditChangeH)
	ON_EN_CHANGE(IDC_EDIT_R, OnEditChangeRGB)
	ON_EN_CHANGE(IDC_EDIT_A, OnEditChangeA)
	ON_BN_CLICKED(IDC_STATIC_COLOR_OLD, OnColorOld)
	ON_EN_CHANGE(IDC_EDIT_S, OnEditChangeSV)
	ON_EN_CHANGE(IDC_EDIT_V, OnEditChangeSV)
	ON_EN_CHANGE(IDC_EDIT_G, OnEditChangeRGB)
	ON_EN_CHANGE(IDC_EDIT_B, OnEditChangeRGB)
	//}}AFX_MSG_MAP
	ON_CONTROL(CPN_HCHANGE, IDC_STATIC_H, OnPaletteChangeH)
	ON_CONTROL(CPN_SVCHANGE, IDC_STATIC_SV, OnPaletteChangeSV)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TO_SWATCH, OnAddToSwatch)
	ON_EN_CHANGE(IDC_EDIT_HEX, OnEditChangeHex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorAdvanced message handlers

BOOL CDlgColorAdvanced::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	LoadOldColor();

	m_btnH.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_H));
	m_btnS.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_S));
	m_btnV.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_V));

	m_btnR.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_R));
	m_btnG.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_G));
	m_btnB.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_B));

	m_btnA.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_A));

	m_btnH.SetRange(0, MAX_H);
	m_btnS.SetRange(0, 100);
	m_btnV.SetRange(0, 100);

	m_btnR.SetRange(0, 255);
	m_btnG.SetRange(0, 255);
	m_btnB.SetRange(0, 255);

	m_btnA.SetRange(0, 100);

	m_bInit = TRUE;

	EnableAlpha(m_bEnableAlpha);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgColorAdvanced::OnPaletteChangeH() 
{
	// TODO: Add your control notification handler code here
	
	m_stcSV.SetHValue(m_stcH.GetHValue());

	// sim a SV change event
	OnPaletteChangeSV();
}

void CDlgColorAdvanced::OnPaletteChangeSV() 
{
	// TODO: Add your control notification handler code here
	
	m_nHValue = m_stcSV.GetHValue();
	m_nSValue = round((float)m_stcSV.GetSValue() * 100 / MAX_S);
	m_nVValue = round((float)m_stcSV.GetVValue() * 100 / MAX_V);

	SyncRGBValue();
	SyncHEXValue();

	UpdateEdit();
}

///////////////////////////////////////////////////////////
// change edit field after palette changed
void CDlgColorAdvanced::UpdateEdit()
{
	UpdateData(FALSE);
	UpdateColorSample();
	UpdateWindow();
}

///////////////////////////////////////////////////////////
// change palette after edit field changed
void CDlgColorAdvanced::UpdateH()
{
	m_stcH.SetHValue(m_nHValue);
	m_stcSV.SetHValue(m_nHValue);

	UpdateColorSample();
}

void CDlgColorAdvanced::UpdateSV()
{
	m_stcSV.SetSValue(m_nSValue * MAX_S / 100);
	m_stcSV.SetVValue(m_nVValue * MAX_S / 100);

	UpdateColorSample();
}

void CDlgColorAdvanced::OnEditChangeH() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	// if the SpinButtonCtrl has "set buddy integer" style, 
	// it will call SetWindowText to change buddy's text and 
	// trigger EN_CHANGE event. If this occurs befor OnInitDialog(), 
	// a debug ASSERT will be triggered
	if (!m_bInit)
		return;

	if (!UpdateData(TRUE))
		return;

	SyncRGBValue();
	SyncHEXValue();

	UpdateData(FALSE);

	UpdateH();
}

void CDlgColorAdvanced::OnEditChangeSV()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	// if the SpinButtonCtrl has "set buddy integer" style, 
	// it will call SetWindowText to change buddy's text and 
	// trigger EN_CHANGE event. If this occurs befor OnInitDialog(), 
	// a debug ASSERT will be triggered
	if (!m_bInit)
		return;

	if (!UpdateData(TRUE))
		return;

	SyncRGBValue();
	SyncHEXValue();

	UpdateData(FALSE);

	UpdateSV();
}

void CDlgColorAdvanced::OnEditChangeRGB() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	if (!m_bInit)
		return;

	if (!UpdateData(TRUE))
		return;

	SyncHSVValue();
	SyncHEXValue();

	UpdateData(FALSE);

	UpdateH();
	UpdateSV();
}

void CDlgColorAdvanced::OnEditChangeHex()
{
	// TODO:  Add your control notification handler code here

	if (!m_bInit)
		return;

	if (!UpdateData(TRUE))
		return;

	int len = m_strHEXValue.GetLength();
	if (len > 0)
	{
		COLORREF color = StringToRGB(m_strHEXValue);
		m_nRValue = GetRValue(color);
		m_nGValue = GetGValue(color);
		m_nBValue = GetBValue(color);

		SyncHSVValue();
		
		UpdateData(FALSE);

		UpdateH();
		UpdateSV();
	}
}

void CDlgColorAdvanced::SyncRGBValue()
{
	HSVType hsv = {m_nHValue, m_nSValue * MAX_S / 100, m_nVValue * MAX_V / 100};
	RGBType rgb = hsv.toRGB();

	m_nRValue = rgb.r;
	m_nGValue = rgb.g;
	m_nBValue = rgb.b;
}

void CDlgColorAdvanced::SyncHSVValue()
{
	RGBType rgb = {m_nRValue, m_nGValue, m_nBValue};
	HSVType hsv = rgb.toHSV();

	m_nHValue = hsv.h;
	m_nSValue = hsv.s * 100 / MAX_S;
	m_nVValue = hsv.v * 100 / MAX_V;
}

void CDlgColorAdvanced::SyncHEXValue()
{
	COLORREF rgb = RGB(m_nRValue, m_nGValue, m_nBValue);
	m_strHEXValue = RGBToString(rgb);
}

void CDlgColorAdvanced::UpdateColorSample()
{
	m_stcColorNew.SetColor(RGB(m_nRValue, m_nGValue, m_nBValue));
	m_stcColorNew.SetAlpha(m_nAValue * 255 / 100);

	GetParent()->SendMessage(WM_COLORCHANGE, 0, 0L);
}

void CDlgColorAdvanced::OnEditChangeA() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	if (!m_bInit)
		return;

	if (!UpdateData(TRUE))
		return;

	m_stcColorNew.SetAlpha(m_nAValue * 255 / 100);
}

void CDlgColorAdvanced::LoadOldColor()
{
	m_nRValue = GetRValue(m_crColorOld);
	m_nGValue = GetGValue(m_crColorOld);
	m_nBValue = GetBValue(m_crColorOld);

	m_nAValue = m_nAlphaOld;

	SyncHSVValue();
	SyncHEXValue();

	UpdateData(FALSE);

	UpdateH();
	UpdateSV();

	m_stcColorOld.SetColor(m_crColorOld);
	m_stcColorOld.SetAlpha(m_nAlphaOld * 255 / 100);

	UpdateColorSample();
}

void CDlgColorAdvanced::OnColorOld() 
{
	// TODO: Add your control notification handler code here
	
	LoadOldColor();
}

void CDlgColorAdvanced::SetColor(COLORREF color, int alpha)
{
	m_crColorOld = color;
	m_nAlphaOld = alpha * 100 / 255;
	if (alpha * 100 % 255 >= 128)
		m_nAlphaOld++;

	if (::IsWindow(this->m_hWnd))
		LoadOldColor();
}

COLORREF CDlgColorAdvanced::GetColor()
{
	return RGB(m_nRValue, m_nGValue, m_nBValue);
}

int CDlgColorAdvanced::GetAlpha()
{
	return m_nAValue * 255 / 100;
}

void CDlgColorAdvanced::EnableAlpha(BOOL bEnable)
{
	m_bEnableAlpha = bEnable;

	if (::IsWindow(m_hWnd))
	{
		GetDlgItem(IDC_EDIT_A)->EnableWindow(m_bEnableAlpha);
		m_btnA.EnableWindow(m_bEnableAlpha);
	}
}

void CDlgColorAdvanced::OnAddToSwatch()
{
	// TODO: Add your control notification handler code here

	CWaitCursor wc;

	BOOL bFreeSpace = FALSE;

	CString strEntry;
	for (int i = 0; i < HBLOCKS * 2; i++)
	{
		strEntry.Format("Color #%d", i);
		if (AfxGetApp()->GetProfileInt(szColorSwatch, strEntry, -1) == -1)
		{
			AfxGetApp()->WriteProfileInt(szColorSwatch, strEntry, GetColor());
			bFreeSpace = TRUE;
			break;
		}
	}

	if (!bFreeSpace)
	{
		for (int i = 1; i < HBLOCKS * 2; i++)
		{
			strEntry.Format("Color #%d", i + 1);
			COLORREF color = AfxGetApp()->GetProfileInt(szColorSwatch, strEntry, -1);

			strEntry.Format("Color #%d", i);
			AfxGetApp()->WriteProfileInt(szColorSwatch, strEntry, color);
		}
		strEntry.Format("Color #%d", HBLOCKS * 2 - 1);
		AfxGetApp()->WriteProfileInt(szColorSwatch, strEntry, GetColor());
	}

	CColorPicker::LoadUserColor();
}
