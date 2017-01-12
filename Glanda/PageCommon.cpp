// PageCommon.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include <afxctl.h>
#include "PageCommon.h"
#include ".\pagecommon.h"

#include "TransAdaptor.h"
#include "C2I.h"
#include "i2C.h"

#include "EffectCommonParameters.h"
#include "TextToolEx.h"
#include "ITextTool.h"
#include "toolsdef.h"
#include "TransAdaptor.h"
#include "TextStyleManager.h"
#include "Dib.h"
#include "Graphics.h"

#define MAX_BORDER_WIDTH	10
#define MAX_SHADOW_SIZE		10
#define MIN_BORDER_WIDTH	0
#define MIN_SHADOW_SIZE		0

const TFillStyle::FillStyleType TFillStyleTypes[]	= 
{
	TFillStyle::no_fill,
		TFillStyle::solid_fill,
		TFillStyle::linear_gradient_fill,
		TFillStyle::radial_gradient_fill,
		TFillStyle::clipped_bitmap_fill,
		TFillStyle::tiled_bitmap_fill
};

// CPageCommon dialog

IMPLEMENT_DYNCREATE(CPageCommon, CGLD_PropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CPageCommon, CGLD_PropertyPage)
	ON_BN_CLICKED(IDC_CHECK_BREAK_APART, OnBnClickedCheckBreakApart)
	ON_BN_CLICKED(IDC_CHECK_TEXT_BORDER, OnBnClickedCheckTextBorder)
	ON_BN_CLICKED(IDC_CHECK_ADD_SHADOW, OnBnClickedCheckAddShadow)
	ON_BN_CLICKED(IDC_CHECK_CUSTOM_FILL, OnBnClickedCheckCustomFill)
	ON_CBN_SELENDOK(IDC_COMBO_FILL_TYPE, OnCbnSelendokComboFillType)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_FILL_COLOR, OnFillcolorChanged)
	ON_CONTROL(LGF_CURRENTHANDLECHANGED, IDC_STATIC_LINEAR_GRADIENT_FILL, OnGradientFillCurrentHandleChanged)
	ON_CONTROL(LGF_HANDLEPOSCHANGED, IDC_STATIC_LINEAR_GRADIENT_FILL, OnGradientFillHandlePosChanged)
	ON_CONTROL(CPN_HCHANGE, IDC_STATIC_H, OnPaletteChangeH)
	ON_CONTROL(CPN_SVCHANGE, IDC_STATIC_SV, OnPaletteChangeSV)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_BORDER_WIDTH, OnBorderWidth)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SHADOW_SIZE, OnShadowSize)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_BORDER_COLOR, OnBorderColor)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_SHADOW_COLOR, OnShadowColor)
	ON_BN_CLICKED(IDC_CHECK_FILL_INDIVIDUALLY, OnBnClickedCheckFillIndividually)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_ANGLE, OnAngle)
	ON_BN_CLICKED(IDC_CHECK_AUTO_PLAY, OnBnClickedCheckAutoPlay)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheckLoop)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_TEXT_STYLE1, OnBnClickedButtonTextStyle1)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_STYLE2, OnBnClickedButtonTextStyle2)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_STYLE3, OnBnClickedButtonTextStyle3)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_STYLE4, OnBnClickedButtonTextStyle4)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_STYLE5, OnBnClickedButtonTextStyle5)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_STYLE6, OnBnClickedButtonTextStyle6)
	ON_COMMAND(ID_TEXT_STYLE_APPLY_STYLE, OnTextStyleApplyStyle)
	ON_COMMAND(ID_TEXT_STYLE_SAVE_STYLE, OnTextStyleSaveStyle)
	ON_UPDATE_COMMAND_UI(ID_TEXT_STYLE_SAVE_STYLE, OnUpdateTextStyleSaveStyle)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// Initialize class factory and guid

// {4427A761-1E28-47E5-A841-FEEB556AAF7F}
IMPLEMENT_OLECREATE_EX(CPageCommon, "Glanda.Effect.Page.Common",
	0x4427a761, 0x1e28, 0x47e5, 0xa8, 0x41, 0xfe, 0xeb, 0x55, 0x6a, 0xaf, 0x7f)



// CPageCommon::CPageCommonFactory::UpdateRegistry -
// Adds or removes system registry entries for CPageCommon

BOOL CPageCommon::CPageCommonFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Define string resource for page type; replace '0' below with ID.

	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PAGE_COMMON);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CPageCommon::CPageCommon - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

CPageCommon::CPageCommon() :
	CGLD_PropertyPage(IDD, IDS_PAGE_COMMON)
{
	m_bBreakApart = TRUE;

	m_bTextBorder = FALSE;
	m_fBorderWidth = 0;

	m_bCustomFill = FALSE;
	m_bFillIndividually = TRUE;
	m_nAngle = 90;

	m_bAutoPlay = TRUE;
	m_bLoop = FALSE;
}



// CPageCommon::DoDataExchange - Moves data between page and properties

void CPageCommon::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT_BORDER_WIDTH, m_edtBorderWidth);
	DDX_Control(pDX, IDC_BUTTON_BORDER_COLOR, m_btnBorderColor);
	DDX_Control(pDX, IDC_SPIN_BORDER_WIDTH, m_btnBorderWidth);
	
	DDX_Control(pDX, IDC_BUTTON_SHADOW_COLOR, m_btnShadowColor);
	DDX_Control(pDX, IDC_EDIT_SHADOW_SIZE, m_edtShadowSize);
	DDX_Control(pDX, IDC_SPIN_SHADOW_SIZE, m_btnShadowSize);

	DDX_Control(pDX, IDC_STATIC_DEFREE_ANGLE, m_stcDefreeAngle);

	DDX_Control(pDX, IDC_COMBO_FILL_TYPE, m_cmbFillStyleType);
	DDX_Control(pDX, IDC_BUTTON_FILL_COLOR, m_btnFillColor);
	DDX_Control(pDX, IDC_STATIC_LINEAR_GRADIENT_FILL, m_stcLinearGradientFill);
	DDX_Control(pDX, IDC_STATIC_SV, m_stcSV);
	DDX_Control(pDX, IDC_STATIC_H, m_stcH);
	DDX_Control(pDX, IDC_EDIT_ANGLE, m_edtFillAngle);
	DDX_Control(pDX, IDC_STATIC_SPIN_ANGLE, m_btnFillAngle);

	DDX_Check(pDX, IDC_CHECK_BREAK_APART, m_bBreakApart);
	DDX_Check(pDX, IDC_CHECK_TEXT_BORDER, m_bTextBorder);
	DDX_Text(pDX, IDC_EDIT_BORDER_WIDTH, m_fBorderWidth);

	DDX_Check(pDX, IDC_CHECK_ADD_SHADOW, m_bAddShadow);
	DDX_Text(pDX, IDC_EDIT_SHADOW_SIZE, m_fShadowSize);

	DDX_Check(pDX, IDC_CHECK_CUSTOM_FILL, m_bCustomFill);

	DDX_Check(pDX, IDC_CHECK_FILL_INDIVIDUALLY, m_bFillIndividually);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_nAngle);
	DDV_MinMaxUInt(pDX, m_nAngle, 0, 359);

	DDX_Check(pDX, IDC_CHECK_AUTO_PLAY, m_bAutoPlay);
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bLoop);

	DDX_Control(pDX, IDC_STATIC_TEXT_PREVIEW, m_stcTextPreview);
	DDX_Control(pDX, IDC_BUTTON_TEXT_STYLE1, m_btnTextStyles[0]);
	DDX_Control(pDX, IDC_BUTTON_TEXT_STYLE2, m_btnTextStyles[1]);
	DDX_Control(pDX, IDC_BUTTON_TEXT_STYLE3, m_btnTextStyles[2]);
	DDX_Control(pDX, IDC_BUTTON_TEXT_STYLE4, m_btnTextStyles[3]);
	DDX_Control(pDX, IDC_BUTTON_TEXT_STYLE5, m_btnTextStyles[4]);
	DDX_Control(pDX, IDC_BUTTON_TEXT_STYLE6, m_btnTextStyles[5]);
}

// CPageCommon message handlers

BOOL CPageCommon::OnInitDialog()
{
	CGLD_PropertyPage::OnInitDialog();

	SetFillColor(RGB(0xff, 0x00, 0x00), 255);

	m_stcH.SetStyle(FALSE);

	m_edtBorderWidth.SetRange(2, MIN_BORDER_WIDTH, MAX_BORDER_WIDTH, 1, FALSE, TRUE);
	m_btnBorderWidth.SetBuddyWindow(m_edtBorderWidth.m_hWnd);
	m_btnBorderWidth.SetRange(MIN_BORDER_WIDTH, MAX_BORDER_WIDTH*4);
	m_btnShadowSize.SetBuddyWindow(m_edtShadowSize.m_hWnd);
	m_btnShadowSize.SetRange(MIN_SHADOW_SIZE, MAX_SHADOW_SIZE*4);
	m_edtShadowSize.SetRange(2, MIN_SHADOW_SIZE, MAX_SHADOW_SIZE, 1, FALSE, TRUE);
	m_btnFillAngle.SetBuddyWindow(m_edtFillAngle.m_hWnd);
	m_btnFillAngle.SetRange(0, 359);
	m_edtFillAngle.SetRange(0, 359, 90, FALSE, TRUE);

	// init members from parameters
	m_bBreakApart = TRUE;
	m_bTextBorder = FALSE;
	m_fBorderWidth = 1;
	m_clrBorderColor = 0xff000000;
	m_bAddShadow = FALSE;
	m_clrShadowColor = 0xff7f7f7f;
	m_fShadowSize = 1;
	m_bCustomFill = FALSE;
	m_bFillIndividually = TRUE;
	m_nAngle = 90;
	m_clrFillColor = 0xff000000;
	m_nFillAlpha = 100;
	m_bAutoPlay = TRUE;
	m_bLoop = FALSE;

	m_stcTextPreview.SetMargin(3);
	m_stcTextPreview.SetBorderWidth(0);

	CComQIPtr<IGLD_Parameters> pIParameters(m_pIUnknown);
	if (pIParameters != NULL)
	{
		// 处理BreakApart
		GetParameter(pIParameters, EP_BREAK_APART, &m_bBreakApart);

		// 处理TextBorder
		GetParameter(pIParameters, EP_TEXT_BORDER, &m_bTextBorder);
		GetParameter(pIParameters, EP_BORDER_WIDTH, &m_fBorderWidth);
		GetParameter(pIParameters, EP_BORDER_COLOR, &m_clrBorderColor);
		m_btnBorderColor.SetColor(m_clrBorderColor, m_clrBorderColor>>24);

		// 处理Shadow
		GetParameter(pIParameters, EP_ADD_SHADOW, &m_bAddShadow);
		GetParameter(pIParameters, EP_SHADOW_SIZE, &m_fShadowSize);
		GetParameter(pIParameters, EP_SHADOW_COLOR, &m_clrShadowColor);
		m_btnShadowColor.SetColor(m_clrShadowColor, m_clrShadowColor>>24);

		// 处理填充方式
		GetParameter(pIParameters, EP_CUSTOM_FILL, &m_bCustomFill);
		GetParameter(pIParameters, EP_FILL_INDIVIDUALLY, &m_bFillIndividually);
		GetParameter(pIParameters, EP_FILL_ANGLE, &m_nAngle);

		GetParameter(pIParameters, EP_AUTO_PLAY, &m_bAutoPlay);
		GetParameter(pIParameters, EP_LOOP, &m_bLoop);

		TFillStyle* ptFillStyle = NULL;
		if(m_bCustomFill)
		{
			CComPtr<IGLD_FillStyle> pIFillStyle = NULL;
			if (SUCCEEDED(GetParameter(pIParameters, EP_FILL_STYLE, &pIFillStyle)))
			{
				gldFillStyle* pgFillStyle;
				if(SUCCEEDED(CI2C::Create(pIFillStyle, &pgFillStyle)))
				{
					CTransAdaptor::GFS2TFS(*pgFillStyle, &ptFillStyle);
					delete pgFillStyle;
				}
			}
		}
		if(ptFillStyle==NULL)
		{
			ptFillStyle = new TSolidFillStyle(TColor(GetRValue(m_clrFillColor), GetGValue(m_clrFillColor), GetBValue(m_clrFillColor), 0));
		}

		m_ptFillStyle.Free();
		m_ptFillStyle.Attach(ptFillStyle);

		LoadFillStyle(ptFillStyle);
		UpdateData(FALSE);
		UpdateControlsUI();
		UpdatePreviewText();
	}

	for(int i = 0; i < 6; ++i)
	{
		m_btnTextStyles[i].ModifyFBStyle(FBS_FLAT, 0);
		CTextStyleData textStyle;
		CTextStyleManager::LoadStyle(textStyle, i);
		m_btnTextStyles[i].SetTextStyle(&textStyle);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageCommon::LoadFillStyle(TFillStyle* pFillStyle)
{
	// 若为无填充或者ALPHA=0的SOLID填充,则认为是无填充
	if(pFillStyle==NULL || pFillStyle->GetType()==TFillStyle::solid_fill && ((TSolidFillStyle*)pFillStyle)->GetColor().m_a==0)
	{
		SetFillType(TFillStyle::no_fill);
		if(pFillStyle)
		{
			TSolidFillStyle* ptStyle = (TSolidFillStyle*)pFillStyle;
			TColor color = ptStyle->GetColor();
			SetFillColor(RGB(color.m_r, color.m_g, color.m_b), GetFillAlpha());
			m_btnFillColor.SetColor(RGB(color.m_r, color.m_g, color.m_b), GetFillAlpha());				
		}
	}
	else
	{
		int type = pFillStyle->GetType();
		SetFillType(type);

		switch (type)
		{
		case TFillStyle::no_fill:
			{
				SetFillColor(GetFillColor(), GetFillAlpha());
				m_btnFillColor.SetColor(GetFillColor(), GetFillAlpha());
			}
			break;
		case TFillStyle::solid_fill:
			{
				TSolidFillStyle* ptStyle = (TSolidFillStyle*)pFillStyle;
				TColor color = ptStyle->GetColor();
				SetFillColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
				m_btnFillColor.SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);				
			}
			break;
		case TFillStyle::linear_gradient_fill:
		case TFillStyle::radial_gradient_fill:
			{
				TGradientFillStyle* ptStyle = (TGradientFillStyle*)pFillStyle;
				m_stcLinearGradientFill.SetFillStyle(ptStyle);
				if(m_stcLinearGradientFill.GetCurrentHandle()>=0)
				{
					TColor color = m_stcLinearGradientFill.GetCurrentColor();
					SetFillColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
					m_btnFillColor.SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
				}				
				break;
			}
		}
	}
}

void CPageCommon::SetFillType(int type)
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FILL_TYPE);
	switch(type)
	{
	case TFillStyle::no_fill:
		m_nOldFillType = 0;
		pComboBox->SetCurSel(0);
		break;
	case TFillStyle::solid_fill:
		m_nOldFillType = 1;
		pComboBox->SetCurSel(1);
		break;
	case TFillStyle::linear_gradient_fill:
		m_nOldFillType = 2;
		pComboBox->SetCurSel(2);
		break;
	case TFillStyle::radial_gradient_fill:
		m_nOldFillType = 3;
		pComboBox->SetCurSel(3);
		break;
	default:
		ASSERT(0);
		break;
	}

	UpdateFillControlsUI();
}

void CPageCommon::UpdateFillControlsUI()
{
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_FILL_TYPE))->GetCurSel();

	int nShowColor = (nIndex==1||nIndex==2||nIndex==3) ? SW_SHOW : SW_HIDE;
	int nShowImage = (nIndex == 4 || nIndex == 5) ? SW_SHOW : SW_HIDE;
	int nShowGradient = (nIndex == 2 || nIndex == 3) ? SW_SHOW : SW_HIDE;
	int nShowAngle = (nIndex == 2) ? SW_SHOW : SW_HIDE;

	m_btnFillColor.ShowWindow(nShowColor);
	m_stcH.ShowWindow(nShowColor);
	m_stcSV.ShowWindow(nShowColor);

	m_stcLinearGradientFill.ShowWindow(nShowGradient);
	GetDlgItem(IDC_CHECK_FILL_INDIVIDUALLY)->ShowWindow(nShowGradient);

	GetDlgItem(IDC_STATIC_ANGLE_LABEL)->ShowWindow(nShowAngle);
	GetDlgItem(IDC_EDIT_ANGLE)->ShowWindow(nShowAngle);
	GetDlgItem(IDC_STATIC_DEFREE_ANGLE)->ShowWindow(nShowAngle);
	GetDlgItem(IDC_STATIC_SPIN_ANGLE)->ShowWindow(nShowAngle);
}

BOOL CPageCommon::OnApply()
{
	TFillStyle* ptFillStyle = m_ptFillStyle;

	CComQIPtr<IGLD_Parameters> pIParameters(m_pIUnknown);
	if (pIParameters != NULL)
	{
		gldFillStyle gfs;
		CTransAdaptor::TFS2GFS(*ptFillStyle, gfs);

		CComPtr<IGLD_FillStyle> pIFillStyle;
		HRESULT hr = CC2I::Create(&gfs, &pIFillStyle);
		if (FAILED(hr))
			return FALSE;

		PutParameter(pIParameters, EP_BREAK_APART, m_bBreakApart);

		PutParameter(pIParameters, EP_TEXT_BORDER, m_bTextBorder);
		PutParameter(pIParameters, EP_BORDER_WIDTH, m_fBorderWidth);
		PutParameter(pIParameters, EP_BORDER_COLOR, m_clrBorderColor);

		PutParameter(pIParameters, EP_ADD_SHADOW, m_bAddShadow);
		PutParameter(pIParameters, EP_SHADOW_SIZE, m_fShadowSize);
		PutParameter(pIParameters, EP_SHADOW_COLOR, m_clrShadowColor);

		PutParameter(pIParameters, EP_FILL_INDIVIDUALLY, m_bFillIndividually);
		PutParameter(pIParameters, EP_FILL_ANGLE, m_nAngle);
		PutParameter(pIParameters, EP_FILL_STYLE, pIFillStyle);
		PutParameter(pIParameters, EP_CUSTOM_FILL, m_bCustomFill);

		PutParameter(pIParameters, EP_AUTO_PLAY, m_bAutoPlay);
		PutParameter(pIParameters, EP_LOOP, m_bLoop);

		return TRUE;
	}

	return FALSE;
}

void CPageCommon::OnBnClickedCheckBreakApart()
{
	m_bBreakApart = IsDlgButtonChecked(IDC_CHECK_BREAK_APART)==BST_CHECKED;
	UpdateControlsUI();
	UpdatePreviewText();
}

void CPageCommon::OnBnClickedCheckTextBorder()
{
	m_bTextBorder = IsDlgButtonChecked(IDC_CHECK_TEXT_BORDER)==BST_CHECKED;
	UpdateControlsUI();
	UpdatePreviewText();
}

void CPageCommon::OnBnClickedCheckAddShadow()
{
	m_bAddShadow = IsDlgButtonChecked(IDC_CHECK_ADD_SHADOW)==BST_CHECKED;
	UpdateControlsUI();
	UpdatePreviewText();
}

void CPageCommon::OnBnClickedCheckCustomFill()
{
	m_bCustomFill = IsDlgButtonChecked(IDC_CHECK_CUSTOM_FILL)==BST_CHECKED;
	UpdateControlsUI();
	UpdatePreviewText();
}

void CPageCommon::SetFillColor(COLORREF color, int alpha, BOOL bUpdatePalette)
{
	m_clrFillColor = color;
	m_nFillAlpha = alpha * 100 / 255;
	if (alpha * 100 % 255 >= 128)
		m_nFillAlpha++;

	if(::IsWindow(m_hWnd))
	{
		UpdateAllControls(bUpdatePalette);
	}
}

COLORREF CPageCommon::GetFillColor()
{
	return m_clrFillColor;
}

int CPageCommon::GetFillAlpha()
{
	return m_nFillAlpha * 255 / 100;
}

void CPageCommon::OnCbnSelendokComboFillType()
{
	// 更新控件的显示
	UpdateFillControlsUI();
	HandleFillStyleChange();
}

void CPageCommon::OnFillcolorChanged()
{
	SetFillColor(m_btnFillColor.GetColor(), m_btnFillColor.GetAlpha());
	HandleFillStyleChange();
}

void CPageCommon::OnGradientFillCurrentHandleChanged()
{
	TColor color = m_stcLinearGradientFill.GetCurrentColor();
	SetFillColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
	HandleFillStyleChange();
}

void CPageCommon::OnGradientFillHandlePosChanged()
{
	TColor color = m_stcLinearGradientFill.GetCurrentColor();
	SetFillColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
	HandleFillStyleChange();
}

void CPageCommon::OnPaletteChangeH()
{
	m_stcSV.SetHValue(m_stcH.GetHValue());
	OnPaletteChangeSV();
}

void CPageCommon::OnPaletteChangeSV()
{
	HSVType hsv = {m_stcSV.GetHValue(), m_stcSV.GetSValue(), m_stcSV.GetVValue()};
	RGBType rgb = hsv.toRGB();
	SetFillColor(RGB(rgb.r, rgb.g, rgb.b), GetFillAlpha(), FALSE);

	if(m_stcH.IsTracking()==FALSE && m_stcSV.IsTracking()==FALSE)
	{
		HandleFillStyleChange();
	}
}

void CPageCommon::UpdateAllControls(BOOL bUpdatePalette)
{
	if(bUpdatePalette)
	{
		UpdatePalette();
	}
	UpdateColorButton();
	UpdateGradientFillSample();
}

void CPageCommon::UpdatePalette()
{
	RGBType rgb = {GetRValue(m_clrFillColor), GetGValue(m_clrFillColor), GetBValue(m_clrFillColor)};
	HSVType hsv = rgb.toHSV();

	int nHValue = hsv.h;
	int nSValue = hsv.s;
	int nVValue = hsv.v;

	if (m_stcH.GetHValue() != nHValue)
		m_stcH.SetHValue(nHValue);

	if (m_stcSV.GetHValue() != nHValue || 
		m_stcSV.GetSValue() != nSValue || 
		m_stcSV.GetVValue() != nVValue)
	{
		m_stcSV.SetHValue(nHValue, FALSE);
		m_stcSV.SetSValue(nSValue, FALSE);
		m_stcSV.SetVValue(nVValue, TRUE);
	}
}

void CPageCommon::UpdateColorButton()
{
	m_btnFillColor.SetColor(m_clrFillColor, GetFillAlpha());
}

void CPageCommon::UpdateGradientFillSample()
{
	m_stcLinearGradientFill.SetCurrentColor(TColor(GetRValue(m_clrFillColor), GetGValue(m_clrFillColor), GetBValue(m_clrFillColor), GetFillAlpha()));
}

void CPageCommon::UpdateControlsUI()
{
	GetDlgItem(IDC_CHECK_TEXT_BORDER)->EnableWindow(m_bBreakApart);
	GetDlgItem(IDC_EDIT_BORDER_WIDTH)->EnableWindow(m_bTextBorder && m_bBreakApart);
	GetDlgItem(IDC_SPIN_BORDER_WIDTH)->EnableWindow(m_bTextBorder && m_bBreakApart);
	GetDlgItem(IDC_BUTTON_BORDER_COLOR)->EnableWindow(m_bTextBorder && m_bBreakApart);

	GetDlgItem(IDC_CHECK_CUSTOM_FILL)->EnableWindow(m_bBreakApart);
	GetDlgItem(IDC_EDIT_SHADOW_SIZE)->EnableWindow(m_bAddShadow && m_bBreakApart);
	GetDlgItem(IDC_SPIN_SHADOW_SIZE)->EnableWindow(m_bAddShadow && m_bBreakApart);
	GetDlgItem(IDC_BUTTON_SHADOW_COLOR)->EnableWindow(m_bAddShadow && m_bBreakApart);

	GetDlgItem(IDC_CHECK_ADD_SHADOW)->EnableWindow(m_bBreakApart);
	GetDlgItem(IDC_COMBO_FILL_TYPE)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_BUTTON_FILL_COLOR)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_STATIC_LINEAR_GRADIENT_FILL)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_STATIC_SV)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_STATIC_H)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_CHECK_FILL_INDIVIDUALLY)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_EDIT_ANGLE)->EnableWindow(m_bCustomFill && m_bBreakApart);
	GetDlgItem(IDC_STATIC_SPIN_ANGLE)->EnableWindow(m_bCustomFill && m_bBreakApart);
}

void CPageCommon::HandleFillStyleChange()
{
	m_ptFillStyle.Free();

	int nFillType = ((CComboBox*)GetDlgItem(IDC_COMBO_FILL_TYPE))->GetCurSel();
	switch(nFillType)
	{
	case 0:
		{
			COLORREF clr = GetFillColor();
			TColor color(GetRValue(clr), GetGValue(clr), GetBValue(clr), 0);
			m_ptFillStyle.Attach(new TSolidFillStyle(color));
		}
		break;
	case 1:
		{
			COLORREF clr = GetFillColor();
			TColor color(GetRValue(clr), GetGValue(clr), GetBValue(clr), GetFillAlpha());
			m_ptFillStyle.Attach(new TSolidFillStyle(color));
		}
		break;
	case 2:
		{
			TLinearGradientFillStyle* ptFillStyle = new TLinearGradientFillStyle();
			const gradient_color_list& gradColors = m_stcLinearGradientFill.GetGradientColors();
			for(gradient_color_list::const_iterator it = gradColors.begin(); it != gradColors.end(); ++it)
			{
				const CGradientColor& color = **it;
				ptFillStyle->AddRecord(TColor(color.color.m_r, color.color.m_g, color.color.m_b, color.color.m_a), color.ratio);					
			}
			m_ptFillStyle.Attach(ptFillStyle);
		}
		break;
	case 3:
		{
			TRadialGradientFillStyle* ptFillStyle = new TRadialGradientFillStyle();
			const gradient_color_list& gradColors = m_stcLinearGradientFill.GetGradientColors();
			for(gradient_color_list::const_iterator it = gradColors.begin(); it != gradColors.end(); ++it)
			{
				const CGradientColor& color = **it;
				ptFillStyle->AddRecord(TColor(color.color.m_r, color.color.m_g, color.color.m_b, color.color.m_a), color.ratio);					
			}
			m_ptFillStyle.Attach(ptFillStyle);
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	UpdatePreviewText();
}

void CPageCommon::UpdatePreviewText()
{
	if(m_pgPreviewText)
	{
		CTransAdaptor::DestroyTShapePtr(m_pgPreviewText);
		m_pgPreviewText.Free();
	}

	CTextStyleData textStyle;
	textStyle.m_bBreakApart		= m_bBreakApart;
	textStyle.m_bTextBorder		= m_bTextBorder;
	textStyle.m_fBorderWidth	= m_fBorderWidth;
	textStyle.m_clrBorderColor	= m_clrBorderColor;
	textStyle.m_bAddShadow		= m_bAddShadow;
	textStyle.m_fShadowSize		= m_fShadowSize;
	textStyle.m_clrShadowColor	= m_clrShadowColor;
	textStyle.m_bCustomFill			= m_bCustomFill;
	textStyle.m_bFillIndividually	= m_bFillIndividually;
	textStyle.m_nAngle				= m_nAngle;
	gldFillStyle gfs;
	CTransAdaptor::TFS2GFS(*m_ptFillStyle, gfs);
	textStyle.SetFillStyle(&gfs);

	gldShape* pShape = textStyle.GeneratePreviewText();
	if(pShape)
	{
		m_stcTextPreview.SetObj(pShape, GetSysColor(COLOR_WINDOW));
		m_pgPreviewText.Attach(pShape);
	}
	else
	{
		ASSERT(0);
		m_stcTextPreview.SetObj(NULL, GetSysColor(COLOR_WINDOW));
	}
}

void CPageCommon::OnBorderWidth()
{
	m_fBorderWidth = (float)m_edtBorderWidth.GetValueFloat();
	UpdatePreviewText();
}

void CPageCommon::OnShadowSize()
{
	m_fShadowSize = (float)m_edtShadowSize.GetValueFloat();
	UpdatePreviewText();
}

void CPageCommon::OnBorderColor()
{
	m_clrBorderColor = m_btnBorderColor.GetColor() | (m_btnBorderColor.GetAlpha() << 24);
	UpdatePreviewText();
}

void CPageCommon::OnShadowColor()
{
	m_clrShadowColor = m_btnShadowColor.GetColor() | (m_btnShadowColor.GetAlpha() << 24);
	UpdatePreviewText();
}

void CPageCommon::OnBnClickedCheckFillIndividually()
{
	m_bFillIndividually = IsDlgButtonChecked(IDC_CHECK_FILL_INDIVIDUALLY)==BST_CHECKED ? TRUE : FALSE;
	HandleFillStyleChange();
}

void CPageCommon::OnAngle()
{
	m_nAngle = m_edtFillAngle.GetValueInt();
	HandleFillStyleChange();
}

void CPageCommon::OnBnClickedCheckAutoPlay()
{
	m_bAutoPlay = IsDlgButtonChecked(IDC_CHECK_AUTO_PLAY)==BST_CHECKED ? TRUE : FALSE;
}

void CPageCommon::OnBnClickedCheckLoop()
{
	m_bLoop = IsDlgButtonChecked(IDC_CHECK_LOOP)==BST_CHECKED ? TRUE : FALSE;
}

void CPageCommon::OnDestroy()
{
	CGLD_PropertyPage::OnDestroy();

	if(m_pgPreviewText)
	{
		CTransAdaptor::DestroyTShapePtr(m_pgPreviewText);
		m_pgPreviewText.Free();
	}
}

void CPageCommon::OnBnClickedButtonTextStyle1()
{
	ApplyTextStyle(m_btnTextStyles[0].GetTextStyle());
}

void CPageCommon::OnBnClickedButtonTextStyle2()
{
	ApplyTextStyle(m_btnTextStyles[1].GetTextStyle());
}

void CPageCommon::OnBnClickedButtonTextStyle3()
{
	ApplyTextStyle(m_btnTextStyles[2].GetTextStyle());
}

void CPageCommon::OnBnClickedButtonTextStyle4()
{
	ApplyTextStyle(m_btnTextStyles[3].GetTextStyle());
}

void CPageCommon::OnBnClickedButtonTextStyle5()
{
	ApplyTextStyle(m_btnTextStyles[4].GetTextStyle());
}

void CPageCommon::OnBnClickedButtonTextStyle6()
{
	ApplyTextStyle(m_btnTextStyles[5].GetTextStyle());
}

void CPageCommon::ApplyTextStyle(CTextStyleData* pTextStyle)
{
	if(pTextStyle)
	{
		// 处理BreakApart
		m_bBreakApart		= pTextStyle->m_bBreakApart;

		// 处理TextBorder
		m_bTextBorder		= pTextStyle->m_bTextBorder;
		m_fBorderWidth		= pTextStyle->m_fBorderWidth;
		m_clrBorderColor	= pTextStyle->m_clrBorderColor;
		m_btnBorderColor.SetColor(m_clrBorderColor, m_clrBorderColor>>24);

		// 处理Shadow
		m_bAddShadow		= pTextStyle->m_bAddShadow;
		m_fShadowSize		= pTextStyle->m_fShadowSize;
		m_clrShadowColor	= pTextStyle->m_clrShadowColor;
		m_btnShadowColor.SetColor(m_clrShadowColor, m_clrShadowColor>>24);

		// 处理填充方式
		m_bCustomFill		= pTextStyle->m_bCustomFill;
		//m_bFillIndividually	= pTextStyle->m_bFillIndividually;
		m_nAngle			= pTextStyle->m_nAngle;

		TFillStyle* ptFillStyle = NULL;
		if (pTextStyle->GetFillStyle())
		{
			CTransAdaptor::GFS2TFS(*pTextStyle->GetFillStyle(), &ptFillStyle);
		}
		if(ptFillStyle==NULL)
		{
			ptFillStyle = new TSolidFillStyle(TColor(GetRValue(m_clrFillColor), GetGValue(m_clrFillColor), GetBValue(m_clrFillColor), 0));
		}

		m_ptFillStyle.Free();
		m_ptFillStyle.Attach(ptFillStyle);

		LoadFillStyle(ptFillStyle);
		UpdateData(FALSE);
		UpdateControlsUI();
		UpdatePreviewText();
	}
}

void CPageCommon::OnTextStyleApplyStyle()
{
	ApplyTextStyle(m_btnTextStyles[m_nSelButton].GetTextStyle());
}

void CPageCommon::OnTextStyleSaveStyle()
{
	CTextStyleData textStyle;
	textStyle.m_bBreakApart			= m_bBreakApart;
	textStyle.m_bTextBorder			= m_bTextBorder;
	textStyle.m_fBorderWidth		= m_fBorderWidth;
	textStyle.m_clrBorderColor		= m_clrBorderColor;
	textStyle.m_bAddShadow			= m_bAddShadow;
	textStyle.m_fShadowSize			= m_fShadowSize;
	textStyle.m_clrShadowColor		= m_clrShadowColor;
	textStyle.m_bCustomFill			= m_bCustomFill;
	textStyle.m_bFillIndividually	= m_bFillIndividually;
	textStyle.m_nAngle				= m_nAngle;
	gldFillStyle gfs;
	CTransAdaptor::TFS2GFS(*m_ptFillStyle, gfs);
	textStyle.SetFillStyle(&gfs);

	HRESULT hr = CTextStyleManager::SaveStyle(textStyle, m_nSelButton);
	if(SUCCEEDED(hr))
	{
		m_btnTextStyles[m_nSelButton].SetTextStyle(&textStyle);
	}
}

void CPageCommon::OnUpdateTextStyleSaveStyle(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bBreakApart);
}

void CPageCommon::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CGLD_PropertyPage::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	UpdateMenuCmdUI(this, pPopupMenu, nIndex, bSysMenu);
}

void CPageCommon::OnContextMenu(CWnd* pWnd, CPoint point)
{
	for(int i = 0; i < 6; ++i)
	{
		if(pWnd == &m_btnTextStyles[i])
		{
			CRect rc;
			pWnd->GetWindowRect(&rc);

			CMenu menu;
			menu.LoadMenu(IDR_POPUP_TEXT_STYLE);
			CMenu* pPopup = menu.GetSubMenu(0);
			if(pPopup)
			{
				m_nSelButton = i;
				m_btnTextStyles[i].SetCheck(BST_CHECKED);
				pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTALIGN | TPM_RIGHTBUTTON, rc.right, rc.bottom, this);
				m_btnTextStyles[i].SetCheck(BST_UNCHECKED);
			}
		}
	}
}
