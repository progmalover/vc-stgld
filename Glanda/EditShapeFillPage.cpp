// EditShapeFillPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditShapeFillPage.h"
#include "gldFillStyle.h"
#include "gldInstance.h"
#include "gldShape.h"
#include "gldImage.h"
#include "GlandaDoc.h"
#include "my_app.h"
#include "gldGradient.h"
#include "gldGradientRecord.h"
#include "gldFillStyle.h"
#include "Image.h"
#include "TransAdaptor.h"
#include "swfproxy.h"
#include "CmdAddShape.h"
#include "gldShapeRecordChange.h"
#include "Command.h"
#include "DrawHelper.h"
#include "EditMatrixPage.h"

#define COLOR_MODE_INVALID			0
#define COLOR_MODE_RGB				1
#define COLOR_MODE_HSV				2

const TFillStyle::FillStyleType TFillStyleTypes[]	= 
{
		TFillStyle::no_fill,
		TFillStyle::solid_fill,
		TFillStyle::linear_gradient_fill,
		TFillStyle::radial_gradient_fill,
		TFillStyle::clipped_bitmap_fill,
		TFillStyle::tiled_bitmap_fill
};


// CEditShapeFillPage dialog

IMPLEMENT_DYNAMIC(CEditShapeFillPage, CPropertyPage)
CEditShapeFillPage::CEditShapeFillPage(gldObj* pObj)
: CPropertyPage(CEditShapeFillPage::IDD)	
, m_pObj(pObj)
, m_nColorMode(-1)
{
	m_bInit = FALSE;

	m_nHValue = 0;
	m_nSValue = 0;
	m_nVValue = 0;
	m_nRValue = 0;
	m_nGValue = 0;
	m_nBValue = 0;
	m_nAValue = 100;

	m_strHEXValue = "#000000";

	m_pCmdImportImages = NULL;
	m_pCmdDeleteImages = NULL;
}

CEditShapeFillPage::~CEditShapeFillPage()
{
}

BOOL CEditShapeFillPage::HasFills(gldObj* pObj)
{
	BOOL bHasFills = FALSE;
	gld_shape shape = CTraitShape(pObj);
	for (gld_draw_obj_iter i = shape.begin_draw_obj(); i != shape.end_draw_obj(); ++i)
	{
		TFillStyleTable &fsTab = (*i).fill_styles();
		if(fsTab.Count())
		{
			bHasFills = TRUE;
			break;
		}
	}
	return bHasFills;
}

void CEditShapeFillPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_FILL_COLOR, m_btnFillColor);
	DDX_Control(pDX, IDC_COMBO_FILL_TYPE, m_cmbFillStyle);
	DDX_Control(pDX, IDC_STATIC_SLIDER_A, m_btnA);
	DDX_Control(pDX, IDC_STATIC_COLOR_NEW, m_stcFillStyle);
	DDX_Control(pDX, IDC_STATIC_SLIDER_B, m_btnB);
	DDX_Control(pDX, IDC_STATIC_SLIDER_G, m_btnG);
	DDX_Control(pDX, IDC_STATIC_SLIDER_R, m_btnR);
	DDX_Control(pDX, IDC_STATIC_SLIDER_V, m_btnV);
	DDX_Control(pDX, IDC_STATIC_SLIDER_S, m_btnS);
	DDX_Control(pDX, IDC_STATIC_SLIDER_H, m_btnH);
	DDX_Control(pDX, IDC_STATIC_SV, m_stcSV);
	DDX_Control(pDX, IDC_STATIC_H, m_stcH);
	DDX_Text(pDX, IDC_EDIT_R, m_nRValue);
	DDV_MinMaxUInt(pDX, m_nRValue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_G, m_nGValue);
	DDV_MinMaxUInt(pDX, m_nGValue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_B, m_nBValue);
	DDV_MinMaxUInt(pDX, m_nBValue, 0, 255);
	DDX_Text(pDX, IDC_EDIT_A, m_nAValue);
	DDV_MinMaxUInt(pDX, m_nAValue, 0, 100);
	DDX_Control(pDX, IDC_STATIC_LINEAR_GRADIENT_FILL, m_stcLinearGradientFill);
	DDX_Control(pDX, IDC_EDIT_HEX, m_edtHex);
	DDX_Text(pDX, IDC_EDIT_HEX, m_strHEXValue);

	DDX_Control(pDX, IDC_EDIT_H, m_edtH);
	DDX_Control(pDX, IDC_EDIT_S, m_edtS);
	DDX_Control(pDX, IDC_EDIT_V, m_edtV);
	DDX_Control(pDX, IDC_EDIT_R, m_edtR);
	DDX_Control(pDX, IDC_EDIT_G, m_edtG);
	DDX_Control(pDX, IDC_EDIT_B, m_edtB);
	DDX_Control(pDX, IDC_EDIT_A, m_edtA);


	DDX_Control(pDX, IDC_BUTTON_IMPORT_IMAGE, m_btnImportImage);
	DDX_Control(pDX, IDC_BUTTON_DELETE_IMAGE, m_btnDeleteImage);

	DDX_Control(pDX, IDC_BUTTON_CONSTRAIN_ANGLE, m_btnConstrainAngle);
	DDX_Control(pDX, IDC_BUTTON_CONSTRAIN_SIZE, m_btnConstrainSize);
	DDX_Control(pDX, IDC_STATIC_DEFREE_X, m_stcDegreeX);
	DDX_Control(pDX, IDC_STATIC_DEFREE_Y, m_stcDegreeY);

	DDX_Control(pDX, IDC_EDIT_COORX, m_edtCoorX);
	DDX_Control(pDX, IDC_EDIT_COORY, m_edtCoorY);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edtWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtHeight);
	DDX_Control(pDX, IDC_EDIT_SCALE_W, m_ScaleW);
	DDX_Control(pDX, IDC_EDIT_SCALE_H, m_ScaleH);
	DDX_Control(pDX, IDC_EDIT_SKEW_X, m_SkewX);
	DDX_Control(pDX, IDC_EDIT_SKEW_Y, m_SkewY);
}


BEGIN_MESSAGE_MAP(CEditShapeFillPage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_FILL_LIST, OnLbnSelchangeFillList)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_FILL_COLOR, OnFillcolorChanged)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_H, OnEditChangeHSV)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_S, OnEditChangeHSV)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_V, OnEditChangeHSV)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_A, OnEditChangeA)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_R, OnEditChangeRGB)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_G, OnEditChangeRGB)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_B, OnEditChangeRGB)
	ON_EN_CHANGE(IDC_EDIT_HEX, OnEditChangeHex)
	ON_CONTROL(CPN_HCHANGE, IDC_STATIC_H, OnPaletteChangeH)
	ON_CONTROL(CPN_SVCHANGE, IDC_STATIC_SV, OnPaletteChangeSV)
	ON_CBN_SELCHANGE(IDC_COMBO_FILL_TYPE, OnCbnSelchangeComboFillType)
	ON_CONTROL(LGF_CURRENTHANDLECHANGED, IDC_STATIC_LINEAR_GRADIENT_FILL, OnGradientFillCurrentHandleChanged)
	ON_CONTROL(LGF_HANDLEPOSCHANGED, IDC_STATIC_LINEAR_GRADIENT_FILL, OnGradientFillHandlePosChanged)
	ON_CONTROL(FLCN_SELCHANGED, IDC_LIST, OnImageListSelChanged)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_IMAGE, OnImportImage)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_IMAGE, OnDeleteImage)

	ON_BN_CLICKED(IDC_BUTTON_CONSTRAIN_ANGLE, OnBnClickedButtonConstrainAngle)
	ON_BN_CLICKED(IDC_BUTTON_CONSTRAIN_SIZE, OnBnClickedButtonConstrainSize)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_COORX, OnEditModifyDimension)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_COORY, OnEditModifyDimension)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_WIDTH, OnEditModifyDimension)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_HEIGHT, OnEditModifyDimension)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_SCALE_W, OnEditModifyWScale)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SCALE_H, OnEditModifyHScale)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SKEW_X, OnEditModifyXSkew)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SKEW_Y, OnEditModifyYSkew)


	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEditShapeFillPage message handlers

struct _FillItemData
{
	gld_draw_obj drawObj;
	int fillIndex;
};

BOOL CEditShapeFillPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_FILL_LIST);
	ASSERT(pListBox != NULL);
	int index = 1;
	CString fillName;
	gld_shape shape = CTraitShape(m_pObj);
	for (gld_draw_obj_iter i = shape.begin_draw_obj(); i != shape.end_draw_obj(); ++i)
	{
		TFillStyleTable &fsTab = (*i).fill_styles();
		for (int f = 0; f < fsTab.Count(); f++)
		{
			fillName.Format(IDS_FILLSTYLE_d, index++);
			int itemIndex = pListBox->AddString(fillName);
			_FillItemData *pData = new _FillItemData;
			pData->drawObj = *i;
			pData->fillIndex = f + 1;
			pListBox->SetItemData(itemIndex, (DWORD_PTR)pData);
		}
	}

	FillComboBox(&m_cmbFillStyle, IDS_LIST_FILL_STYLE);

	CRect rcGrad;
	m_stcLinearGradientFill.GetWindowRect(&rcGrad);
	rcGrad.left -= m_stcLinearGradientFill.GetMargin();
	rcGrad.right += m_stcLinearGradientFill.GetMargin();
	ScreenToClient(&rcGrad);
	m_stcLinearGradientFill.MoveWindow(&rcGrad);

	CRect rcFillStyle;
	m_stcFillStyle.GetWindowRect(&rcFillStyle);
	ScreenToClient(&rcFillStyle);

	CRect rcList(rcGrad.left, rcGrad.top, rcFillStyle.right, rcFillStyle.bottom);
	m_lstImage.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcList, this, IDC_LIST);

	// 填充ImageList
	CObjectMap* pObjectMap = _GetObjectMap();
	for(CObjectMap::_ObjectMap::iterator iter = pObjectMap->m_Map.begin(); iter != pObjectMap->m_Map.end(); ++iter)
	{
		gldObj* pObj = iter->first;
		if(pObj->IsGObjInstanceOf(gobjImage))
		{
			m_lstImage.AddImage((gldImage*)pObj);
		}
	}


	m_edtH.SetRange(0, 360, 0, FALSE, TRUE);
	m_edtS.SetRange(0, 100, 0, FALSE, TRUE);
	m_edtV.SetRange(0, 100, 0, FALSE, TRUE);

	m_edtR.SetRange(0, 255, 0, FALSE, TRUE);
	m_edtG.SetRange(0, 255, 0, FALSE, TRUE);
	m_edtB.SetRange(0, 255, 0, FALSE, TRUE);

	m_edtA.SetRange(0, 100, 0, FALSE, TRUE);

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

	m_stcH.SetStyle(FALSE);

	m_btnImportImage.LoadBitmap(IDB_BUTTON_BROWSE);
	m_btnDeleteImage.LoadBitmap(IDB_BUTTON_DELETE, 1);

	m_edtCoorX.SetRange(1, -SHRT_MAX, SHRT_MAX, 0, FALSE, TRUE);
	m_edtCoorY.SetRange(1, -SHRT_MAX, SHRT_MAX, 0, FALSE, TRUE);
	m_edtWidth.SetRange(1, 0.05, SHRT_MAX, 0, FALSE, TRUE);
	m_edtHeight.SetRange(1, 0.05, SHRT_MAX, 0, FALSE, TRUE);
	m_ScaleW.SetRange(1, 0.01, SHRT_MAX, 100.0, FALSE, TRUE);
	m_ScaleH.SetRange(1, 0.01, SHRT_MAX, 100.0, FALSE, TRUE);
	m_SkewX.SetRange(1, -360, 360, 0, FALSE, TRUE);
	m_SkewY.SetRange(1, -360, 360, 0, FALSE, TRUE);


	SetColorMode(COLOR_MODE_RGB);

	m_bInit = TRUE;

	SetColor(RGB(255, 0, 0), 255);
	pListBox->SetCurSel(0);
	OnLbnSelchangeFillList();


	/*
	if (AfxGetApp()->GetProfileInt("Property Pages\\TransformFillStyle", "Constrain Angle", TRUE) != 0)
	{
		m_btnConstrainAngle.SetCheck(BST_CHECKED);
	}
	if (AfxGetApp()->GetProfileInt("Property Pages\\TransformFillStyle", "Constrain Size", TRUE) != 0)
	{
		m_btnConstrainSize.SetCheck(BST_CHECKED);
	}
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditShapeFillPage::SetColorMode(int nMode)
{
	if (m_nColorMode != nMode)
	{
		m_nColorMode = nMode;

		GetDlgItem(IDC_STATIC_LABEL_R)->ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_R)->ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);
		m_btnR.ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);

		GetDlgItem(IDC_STATIC_LABEL_G)->ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_G)->ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);
		m_btnG.ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);

		GetDlgItem(IDC_STATIC_LABEL_B)->ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_B)->ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);
		m_btnB.ShowWindow(m_nColorMode == COLOR_MODE_RGB ? SW_SHOW : SW_HIDE);

		GetDlgItem(IDC_STATIC_LABEL_H)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_H)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		m_btnH.ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);

		GetDlgItem(IDC_STATIC_LABEL_S)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_S)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		m_btnS.ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_STATIC_PERCENT_S)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);

		GetDlgItem(IDC_STATIC_LABEL_V)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_EDIT_V)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		m_btnV.ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_STATIC_PERCENT_V)->ShowWindow(m_nColorMode == COLOR_MODE_HSV ? SW_SHOW : SW_HIDE);
	}
}

void CEditShapeFillPage::SetColor(COLORREF color, int alpha)
{
	m_nRValue = GetRValue(color);
	m_nGValue = GetGValue(color);
	m_nBValue = GetBValue(color);

	m_nAValue = alpha * 100 / 255;
	if (alpha * 100 % 255 >= 128)
		m_nAValue++;

	if (::IsWindow(this->m_hWnd))
	{
		SyncHSVValue();
		SyncHEXValue();

		UpdateAllControls();
	}
}

COLORREF CEditShapeFillPage::GetColor()
{
	return RGB(m_nRValue, m_nGValue, m_nBValue);
}

int CEditShapeFillPage::GetAlpha()
{
	return m_nAValue * 255 / 100;
}

TFillStyle::FillStyleType CEditShapeFillPage::GetFillStyleType()
{
	return TFillStyleTypes[m_cmbFillStyle.GetCurSel()];
}

void CEditShapeFillPage::HandleFillTypeChange()
{
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_FILL_TYPE))->GetCurSel();
	
	int swShowColor = (nIndex==1||nIndex==2||nIndex==3) ? SW_SHOW : SW_HIDE;
	int swShowImage = (nIndex == 4 || nIndex == 5) ? SW_SHOW : SW_HIDE;
	int swShowGradient = (nIndex == 2 || nIndex == 3) ? SW_SHOW : SW_HIDE;
	int swShowColorRGB = (swShowColor==SW_SHOW && m_nColorMode==COLOR_MODE_RGB) ? SW_SHOW : SW_HIDE;
	int swShowColorHSV = (swShowColor==SW_SHOW && m_nColorMode==COLOR_MODE_HSV) ? SW_SHOW : SW_HIDE;
	int swShowRotation = (nIndex ==2 ) ? SW_SHOW : SW_HIDE;


	m_btnFillColor.ShowWindow(swShowColor);
	m_stcH.ShowWindow(swShowColor);
	m_stcSV.ShowWindow(swShowColor);
	m_stcFillStyle.ShowWindow(swShowColor);
	m_stcLinearGradientFill.ShowWindow(swShowGradient);

	m_btnImportImage.ShowWindow(swShowImage);
	m_btnDeleteImage.ShowWindow(swShowImage);
	m_lstImage.ShowWindow(swShowImage);

	m_edtHex.ShowWindow(swShowColor);

	m_edtR.ShowWindow(swShowColorRGB);
	m_edtG.ShowWindow(swShowColorRGB);
	m_edtB.ShowWindow(swShowColorRGB);
	m_edtH.ShowWindow(swShowColorHSV);
	m_edtS.ShowWindow(swShowColorHSV);
	m_edtV.ShowWindow(swShowColorHSV);
	m_edtA.ShowWindow(swShowColor);
	m_edtHex.ShowWindow(swShowColor);

	m_btnR.ShowWindow(swShowColorRGB);
	m_btnG.ShowWindow(swShowColorRGB);
	m_btnB.ShowWindow(swShowColorRGB);
	m_btnH.ShowWindow(swShowColorHSV);
	m_btnS.ShowWindow(swShowColorHSV);
	m_btnV.ShowWindow(swShowColorHSV);
	m_btnA.ShowWindow(swShowColor);

	GetDlgItem(IDC_STATIC_LABEL_R)->ShowWindow(swShowColorRGB);
	GetDlgItem(IDC_STATIC_LABEL_G)->ShowWindow(swShowColorRGB);
	GetDlgItem(IDC_STATIC_LABEL_B)->ShowWindow(swShowColorRGB);
	GetDlgItem(IDC_STATIC_LABEL_A)->ShowWindow(swShowColor);
	GetDlgItem(IDC_STATIC_LABEL_H)->ShowWindow(swShowColorHSV);
	GetDlgItem(IDC_STATIC_LABEL_S)->ShowWindow(swShowColorHSV);
	GetDlgItem(IDC_STATIC_LABEL_V)->ShowWindow(swShowColorHSV);
	GetDlgItem(IDC_STATIC_PERCENT_S)->ShowWindow(swShowColorHSV);
	GetDlgItem(IDC_STATIC_PERCENT_V)->ShowWindow(swShowColorHSV);
	GetDlgItem(IDC_STATIC_PERCENT_A)->ShowWindow(swShowColor);

	//* Matrix Controls Enable/Disable
	int swShowFillTransform = (nIndex==2||nIndex==3||nIndex==4||nIndex==5) ? SW_SHOW : SW_HIDE;
	int swShowMatrix  = (nIndex==2||nIndex==3||nIndex==4||nIndex==5) ? SW_SHOW : SW_HIDE;
	int swShowMatrixAll = (nIndex==3||nIndex==4||nIndex==5) ? SW_SHOW : SW_HIDE;

	GetDlgItem(IDC_STATIC_FILL_TRANSFORM)->ShowWindow(swShowFillTransform);
	GetDlgItem(IDC_STATIC_LABEL_X)->ShowWindow(swShowMatrix);
	GetDlgItem(IDC_EDIT_COORX)->ShowWindow(swShowMatrix);
	GetDlgItem(IDC_STATIC_LABEL_Y)->ShowWindow(swShowMatrixAll);
	GetDlgItem(IDC_EDIT_COORY)->ShowWindow(swShowMatrixAll);
	GetDlgItem(IDC_STATIC_LABEL_WIDTH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_WIDTH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LABEL_HEIGHT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_HEIGHT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_CONSTRAIN_SIZE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LABEL_SCALE_W)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_SCALE_W)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PERCENT_SCALE_W)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LABEL_SCALE_H)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_SCALE_H)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PERCENT_SCALE_H)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_CONSTRAIN_ANGLE)->ShowWindow(SW_HIDE);

	if(swShowMatrix==SW_SHOW && swShowMatrixAll==SW_HIDE)
	{
		GetDlgItem(IDC_STATIC_LABEL_X_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LABEL_ROTATION)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_STATIC_LABEL_X_ANGLE)->ShowWindow(swShowMatrix);
		GetDlgItem(IDC_STATIC_LABEL_ROTATION)->ShowWindow(SW_HIDE);
	}
	GetDlgItem(IDC_EDIT_SKEW_X)->ShowWindow(swShowMatrix);
	GetDlgItem(IDC_STATIC_DEFREE_X)->ShowWindow(swShowMatrix);
	GetDlgItem(IDC_STATIC_LABEL_Y_ANGLE)->ShowWindow(swShowMatrixAll);
	GetDlgItem(IDC_EDIT_SKEW_Y)->ShowWindow(swShowMatrixAll);
	GetDlgItem(IDC_STATIC_DEFREE_Y)->ShowWindow(swShowMatrixAll);
}


void CEditShapeFillPage::SyncRGBValue()
{
	HSVType hsv = {m_nHValue, m_nSValue, m_nVValue};
	RGBType rgb = hsv.toRGB();

	m_nRValue = rgb.r;
	m_nGValue = rgb.g;
	m_nBValue = rgb.b;
}

void CEditShapeFillPage::SyncHSVValue()
{
	RGBType rgb = {m_nRValue, m_nGValue, m_nBValue};
	HSVType hsv = rgb.toHSV();

	m_nHValue = hsv.h;
	m_nSValue = hsv.s;
	m_nVValue = hsv.v;
}

void CEditShapeFillPage::SyncHEXValue()
{
	COLORREF rgb = RGB(m_nRValue, m_nGValue, m_nBValue);
	m_strHEXValue = RGBToString(rgb);
}

void CEditShapeFillPage::UpdateAllControls()
{
	UpdateData(FALSE);

	SetDlgItemInt(IDC_EDIT_H, m_nHValue);
	SetDlgItemInt(IDC_EDIT_S, round((float)m_nSValue * 100 / MAX_S));
	SetDlgItemInt(IDC_EDIT_V, round((float)m_nVValue * 100 / MAX_V));

	UpdatePalette();
	UpdateColorButton();
	UpdateGradientFillSample();
	UpdateColorSample();
}

void CEditShapeFillPage::UpdatePalette()
{
	if (m_stcH.GetHValue() != m_nHValue)
		m_stcH.SetHValue(m_nHValue);

	if (m_stcSV.GetHValue() != m_nHValue || 
		m_stcSV.GetSValue() != m_nSValue || 
		m_stcSV.GetVValue() != m_nVValue)
	{
		m_stcSV.SetHValue(m_nHValue, FALSE);
		m_stcSV.SetSValue(m_nSValue, FALSE);
		m_stcSV.SetVValue(m_nVValue, TRUE);
	}
}

void CEditShapeFillPage::UpdateColorSample()
{
	m_stcFillStyle.SetFillStyle(GetFillStyle());
}

void CEditShapeFillPage::UpdateColorButton()
{
	m_btnFillColor.SetColor(RGB(m_nRValue, m_nGValue, m_nBValue), m_nAValue * 255 / 100);
}

void CEditShapeFillPage::UpdateGradientFillSample()
{
	m_stcLinearGradientFill.SetCurrentColor(TColor(m_nRValue, m_nGValue, m_nBValue, m_nAValue * 255 / 100));
}

TFillStyle *CEditShapeFillPage::GetFillStyle()
{
	TFillStyle *pFillStyle = NULL;
	int type = GetFillStyleType();
	switch (type)
	{
	case TFillStyle::no_fill:
		{
			break;
		}
	case TFillStyle::solid_fill:
		{
			COLORREF rgb = m_btnFillColor.GetColor();
			TColor color(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), m_btnFillColor.GetAlpha());
			pFillStyle = new TSolidFillStyle(color);
			break;
		}
	case TFillStyle::linear_gradient_fill:
		{
			pFillStyle = new TLinearGradientFillStyle;
			m_stcLinearGradientFill.GetFillStyle((TGradientFillStyle *)pFillStyle);
			break;
		}
	case TFillStyle::radial_gradient_fill:
		{
			pFillStyle = new TRadialGradientFillStyle;
			m_stcLinearGradientFill.GetFillStyle((TGradientFillStyle *)pFillStyle);
			break;
		}
	case TFillStyle::clipped_bitmap_fill:
	case TFillStyle::tiled_bitmap_fill:
		{
			// Set the ptr of gldImage to the TImage
			int index = m_lstImage.GetCurSel();
			if (index < 0 && m_lstImage.GetItemCount() > 0)
			{
				m_lstImage.SelectItem(0);
			}

			index = m_lstImage.GetCurSel();
			if (index >= 0)
			{
				gldImage *_gimg = m_lstImage.GetImage(index);
				TImage *_timg = (TImage *)_gimg->m_ptr;
				ASSERT(_gimg->m_ptr);
				if (type == clipped_bitmap_fill)
				{
					pFillStyle = new TClippedBitmapFillStyle(_timg);
				}
				else
				{
					pFillStyle = new TTiledBitmapFillStyle(_timg);
				}
			}
			else
			{
				COLORREF rgb = m_btnFillColor.GetColor();
				TColor color(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), m_btnFillColor.GetAlpha());

				pFillStyle = new TSolidFillStyle(color);
				((TSolidFillStyle *)pFillStyle)->SetColor(color);

				SetFillType(solid_fill);
			}
			break;
		}
	}

	return pFillStyle;
}

void CEditShapeFillPage::HandleFillSelChange()
{
	TFillStyle* pFillStyle = GetCurSelFill();

	// 若为无填充或者ALPHA=0的SOLID填充,则认为是无填充
	if(pFillStyle==NULL || pFillStyle->GetType()==TFillStyle::solid_fill && ((TSolidFillStyle*)pFillStyle)->GetColor().m_a==0)
	{
		SetFillType(TFillStyle::no_fill);
		if(pFillStyle)
		{
			TSolidFillStyle* ptStyle = (TSolidFillStyle*)pFillStyle;
			TColor color = ptStyle->GetColor();
			SetColor(RGB(color.m_r, color.m_g, color.m_b), GetAlpha());
			m_btnFillColor.SetColor(RGB(color.m_r, color.m_g, color.m_b), GetAlpha());				
		}
	}
	else
	{
		int type = pFillStyle->GetType();
		SetFillType(type);

		switch (type)
		{
		case TFillStyle::no_fill:
			break;
		case TFillStyle::solid_fill:
			{
				TSolidFillStyle* ptStyle = (TSolidFillStyle*)pFillStyle;
				TColor color = ptStyle->GetColor();
				SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
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
					SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
					m_btnFillColor.SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
				}				
				break;
			}
		case TFillStyle::clipped_bitmap_fill:
		case TFillStyle::tiled_bitmap_fill:
			{
				TBitmapFillStyle* ptStyle = (TBitmapFillStyle*)pFillStyle;
				gldImage *pImage = CTraitImage(ptStyle->GetImage());
				int index = m_lstImage.FindImage(pImage);
				if (index >= 0)
				{
					m_lstImage.SelectItem(index);
					m_lstImage.EnsureVisible(index);
				}				
				break;
			}
		}
	}

	// 从pFillStyle中分析FillStyleMatrix
	if(pFillStyle)
	{
		m_matrix = pFillStyle->GetMatrix();
	}
	else
	{
		TMatrix matrix;
		m_matrix.SetMatrix(matrix.m_e11, matrix.m_e12, matrix.m_e21, matrix.m_e22, matrix.m_dx, matrix.m_dy);
	}

	RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, CTraitShape(m_pObj), m_matrix);
	UpdateMatrixControls();
}

void CEditShapeFillPage::SetFillType(int type)
{
	for (int i = 0; i < sizeof(TFillStyleTypes) / sizeof(TFillStyle::solid_fill); i++)
	{
		if (TFillStyleTypes[i] == type)
		{
			m_cmbFillStyle.SetCurSel(i);
			break;
		}
	}

	HandleFillTypeChange();
	UpdateColorSample();
}

TFillStyle* CEditShapeFillPage::GetCurSelFill()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_FILL_LIST);
	ASSERT(pListBox != NULL);
	int nIndex = pListBox->GetCurSel();
	if(nIndex > -1)
	{
		_FillItemData *pData = (_FillItemData *)pListBox->GetItemData(nIndex);
		ASSERT(pData != NULL);
		return pData->drawObj.fill_styles().Get(pData->fillIndex);
	}
	return NULL;
}

void CEditShapeFillPage::OnCbnSelchangeComboFillType()
{
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_FILL_TYPE))->GetCurSel();
	int type = GetFillStyleType();
	if (type == TFillStyle::tiled_bitmap_fill || type == TFillStyle::clipped_bitmap_fill)
	{
		// 若ImageListCtrl中无图像,必须先导入一个图像
		if (m_lstImage.GetItemCount() == 0)
		{
			gldImage* _gimg = ImportImage();
			m_cmbFillStyle.SetFocus();
			if (!_gimg)
			{
				HandleFillSelChange();
				return;
			}
			else
			{
				m_lstImage.SelectItem(m_lstImage.FindImage(_gimg));
			}
		}
		// 如果没有选中,则必须要选中一幅图
		if(m_lstImage.GetCurSel()<0)
		{
			m_lstImage.SelectItem(0);
		}
	}

	// 重新设置FillStyleMatrix
	TFillStyle* pFillStyle = GetCurSelFill();
	int nOldType = (pFillStyle ? pFillStyle->GetType() : TFillStyle::no_fill);
	if(type != nOldType && type != TFillStyle::solid_fill && type != TFillStyle::no_fill)
	{
		gld_shape shape = CTraitShape(m_pObj);
		switch(type)
		{
		case TFillStyle::linear_gradient_fill:
		case TFillStyle::radial_gradient_fill:
			if(nOldType==TFillStyle::linear_gradient_fill||nOldType==TFillStyle::radial_gradient_fill)
			{
				// 线型填充及圆周填充不改变
			}
			else
			{
				const gld_rect box = shape.bound();
				float		scale_x = (float)box.width() / 0x8000;
				float		scale_y = (float)box.height() / 0x8000;
				int			dx = (box.left + box.right) / 2;
				int			dy = (box.top + box.bottom) / 2;

				m_matrix.SetMatrix(scale_x, 0.0f, 0.0f, scale_y, dx, dy);
				RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);
				UpdateMatrixControls();
			}
			break;
		case TFillStyle::clipped_bitmap_fill:
			{
				const gld_rect box = shape.bound();
				int nImageIndex = m_lstImage.GetCurSel();
				if(nImageIndex >= 0)
				{
					gldImage* pImage = m_lstImage.GetImage(nImageIndex);
					int bmpWidth = pImage->GetWidth();
					int bmpHeight = pImage->GetHeight();
					if(bmpWidth > 0 && bmpHeight > 0)
					{
						float scale_x = (float)box.width() / bmpWidth;
						float scale_y = (float)box.height() / bmpHeight;
						m_matrix.SetMatrix(scale_x, 0.0f, 0.0f, scale_y, box.left, box.top);
						RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);
						UpdateMatrixControls();
					}
				}
			}
			break;
		case TFillStyle::tiled_bitmap_fill:
			{
				const gld_rect box = shape.bound();
				m_matrix.SetMatrix(20.0f, 0.0f, 0.0f, 20.0f, box.left, box.top);
				RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);
				UpdateMatrixControls();
			}
			break;
		default:
			break;
		}
	}

	HandleFillTypeChange();
	UpdateColorSample();
	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnLbnSelchangeFillList()
{
	HandleFillSelChange();
}

void CEditShapeFillPage::OnFillcolorChanged()
{
	SetColor(m_btnFillColor.GetColor(), m_btnFillColor.GetAlpha());
	UpdateSelectedShapes();
}

void CEditShapeFillPage::UpdateSelectedShapes()
{	
	TFillStyle *pNewFill = NULL;

	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_FILL_TYPE))->GetCurSel();

	if (nIndex > -1)
	{
		switch (nIndex)
		{
		case 0:
			{
				TSolidFillStyle *pSolidFill = new TSolidFillStyle(TColor(m_nRValue, m_nGValue, m_nBValue, 0));
				pNewFill = pSolidFill;
			}
			break;
		case 1:
			{
				TSolidFillStyle *pSolidFill = new TSolidFillStyle(TColor(m_nRValue, m_nGValue, m_nBValue, GetAlpha()));
				pNewFill = pSolidFill;
			}
			break;
		case 2:
			{
				TLinearGradientFillStyle *pLGFill = new TLinearGradientFillStyle;
				const gradient_color_list& gradColors = m_stcLinearGradientFill.GetGradientColors();
				for(gradient_color_list::const_iterator it = gradColors.begin(); it != gradColors.end(); ++it)
				{
					const CGradientColor& color = **it;
					pLGFill->AddRecord(TColor(color.color.m_r, color.color.m_g, color.color.m_b, color.color.m_a), color.ratio);					
				}				
				pLGFill->SetMatrix(m_matrix);
				pNewFill = pLGFill;
			}
			break;
		case 3:
			{
				TRadialGradientFillStyle *pRGFill = new TRadialGradientFillStyle;
				const gradient_color_list& gradColors = m_stcLinearGradientFill.GetGradientColors();
				for(gradient_color_list::const_iterator it = gradColors.begin(); it != gradColors.end(); ++it)
				{
					const CGradientColor& color = **it;
					pRGFill->AddRecord(TColor(color.color.m_r, color.color.m_g, color.color.m_b, color.color.m_a), color.ratio);					
				}
				pRGFill->SetMatrix(m_matrix);
				pNewFill = pRGFill;
			}
			break;
		case 4:
			{
				int index = m_lstImage.GetCurSel();
				gldImage *gImg= m_lstImage.GetImage(index);
				ASSERT((TImage *)CTraitImage(gImg) != NULL);
				TClippedBitmapFillStyle *pCBFill = new TClippedBitmapFillStyle(CTraitImage(gImg));
				pCBFill->SetMatrix(m_matrix);
				pNewFill = pCBFill;
			}
			break;
		case 5:
			{
				int index = m_lstImage.GetCurSel();
				gldImage *gImg= m_lstImage.GetImage(index);
				ASSERT((TImage *)CTraitImage(gImg) != NULL);
				TTiledBitmapFillStyle *pTBFill = new TTiledBitmapFillStyle(CTraitImage(gImg));
				pTBFill->SetMatrix(m_matrix);
				pNewFill = pTBFill;
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	REFRENCE(pNewFill);

	CGuardDrawOnce xGuad;

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_FILL_LIST);
	ASSERT(pListBox != NULL);
	nIndex = pListBox->GetCurSel();
	if (nIndex > -1)
	{
		// 撤销GROUP中当前的线型填充
		for(std::map<int, CCmdModifyFillStyle*>::iterator iter = m_CmdModifyFillStyleList.begin(); iter != m_CmdModifyFillStyleList.end(); ++iter)
		{
			if(iter->first==nIndex)
			{
				iter->second->Unexecute();
				delete iter->second;
				m_CmdModifyFillStyleList.erase(iter);
				break;
			}
		}

		_FillItemData *pData = (_FillItemData *)pListBox->GetItemData(nIndex);
		ASSERT(pData != NULL);

		gld_shape shape = CTraitShape(m_pObj);

		/*
		TFillStyle* pOldFill = GetCurSelFill();
		if (pNewFill)
		{
			if(pOldFill && pOldFill->GetType()==pNewFill->GetType())
			{
				if(pNewFill->GetType()==TFillStyle::clipped_bitmap_fill||pNewFill->GetType()==TFillStyle::tiled_bitmap_fill)
				{
					// 位图填充要重设
					TSETrivial::SetFillStyleMatrix(pNewFill, shape.bound());
				}
				else
				{
					pNewFill->SetMatrix(pOldFill->GetMatrix());
				}
			}
			else
			{
				TSETrivial::SetFillStyleMatrix(pNewFill, shape.bound());
			}
		}
		*/
		// 添加新的填充Command
		CCmdModifyFillStyle* pCmd = new CCmdModifyFillStyle(shape, m_pObj, pData->drawObj, pData->fillIndex, pNewFill);
		pCmd->Execute();
		m_CmdModifyFillStyleList[nIndex] = pCmd;
	}	
	
	RELEASE(pNewFill);
}

void CEditShapeFillPage::OnEditChangeHSV() 
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

	m_nHValue = m_edtH.GetValueInt();
	m_nSValue = round((float)m_edtS.GetValueInt() * MAX_S / 100);
	m_nVValue = round((float)m_edtV.GetValueInt() * MAX_V / 100);

	SyncRGBValue();
	SyncHEXValue();

	UpdateAllControls();

	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnEditChangeRGB() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here

	if (!m_bInit)
		return;

	m_nRValue = m_edtR.GetValueInt();
	m_nGValue = m_edtG.GetValueInt();
	m_nBValue = m_edtB.GetValueInt();

	SyncHSVValue();
	SyncHEXValue();

	UpdateAllControls();

	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnEditChangeA() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here

	if (!m_bInit)
		return;

	m_nAValue = m_edtA.GetValueInt();

	UpdateAllControls();

	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnEditChangeHex()
{
	// TODO:  Add your control notification handler code here

	if (!m_bInit)
		return;

	GetDlgItemText(IDC_EDIT_HEX, m_strHEXValue);

	int len = m_strHEXValue.GetLength();
	if (len > 0)
	{
		COLORREF color = StringToRGB(m_strHEXValue);
		m_nRValue = GetRValue(color);
		m_nGValue = GetGValue(color);
		m_nBValue = GetBValue(color);

		SyncHSVValue();

		UpdateAllControls();

		UpdateSelectedShapes();
	}
}



void CEditShapeFillPage::OnPaletteChangeH() 
{
	// TODO: Add your control notification handler code here

	m_stcSV.SetHValue(m_stcH.GetHValue());

	// sim a SV change event
	OnPaletteChangeSV();
}

void CEditShapeFillPage::OnPaletteChangeSV() 
{
	// TODO: Add your control notification handler code here

	m_nHValue = m_stcSV.GetHValue();
	m_nSValue = m_stcSV.GetSValue();
	m_nVValue = m_stcSV.GetVValue();

	SyncRGBValue();
	SyncHEXValue();

	UpdateAllControls();

	if (!m_stcH.IsTracking() && !m_stcSV.IsTracking())
		UpdateSelectedShapes();
}

void CEditShapeFillPage::OnGradientFillCurrentHandleChanged()
{
	TColor color = m_stcLinearGradientFill.GetCurrentColor();
	SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnGradientFillHandlePosChanged()
{
	TColor color = m_stcLinearGradientFill.GetCurrentColor();
	SetColor(RGB(color.m_r, color.m_g, color.m_b), color.m_a);
	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnImageListSelChanged()
{
	TFillStyle::FillStyleType type = GetFillStyleType();

	gld_shape shape = CTraitShape(m_pObj);
	if(type==TFillStyle::clipped_bitmap_fill)
	{
		const gld_rect box = shape.bound();
		int nImageIndex = m_lstImage.GetCurSel();
		if(nImageIndex >= 0)
		{
			gldImage* pImage = m_lstImage.GetImage(nImageIndex);
			int bmpWidth = pImage->GetWidth();
			int bmpHeight = pImage->GetHeight();
			if(bmpWidth > 0 && bmpHeight > 0)
			{
				float scale_x = (float)box.width() / bmpWidth;
				float scale_y = (float)box.height() / bmpHeight;
				m_matrix.SetMatrix(scale_x, 0.0f, 0.0f, scale_y, box.left, box.top);
				RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);
				UpdateMatrixControls();
			}
		}
	}
	if(type==TFillStyle::tiled_bitmap_fill)
	{
		const gld_rect box = shape.bound();
		m_matrix.SetMatrix(20.0f, 0.0f, 0.0f, 20.0f, box.left, box.top);
		RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);
		UpdateMatrixControls();
	}

	UpdateSelectedShapes();
}

void CEditShapeFillPage::OnImportImage()
{
	ImportImage();
}

void CEditShapeFillPage::OnDeleteImage()
{
	int index = m_lstImage.GetCurSel();

	if(index < 0)
	{
		return;
	}

	_FillItemData* pFillData = (_FillItemData*)m_lstImage.GetItemData(index);

	gldImage *_gimg= m_lstImage.GetImage(index);
	GOBJECT_LIST ObjList;
	if(_GetObjectMap()->FindShapesUseImage(_gimg, &ObjList))
	{
		gldObj* pObj = *ObjList.begin();
		if(ObjList.size()==1 && pObj==m_pObj)
		{
			gld_shape shape = CTraitShape(m_pObj);
			for (gld_draw_obj_iter i = shape.begin_draw_obj(); i != shape.end_draw_obj(); ++i)
			{
				TFillStyleTable &fsTab = (*i).fill_styles();
				for (int f = 0; f < fsTab.Count(); f++)
				{
					TFillStyle* pFillStyle = fsTab.Get(f);
					if(pFillStyle && (pFillStyle->GetType()==TFillStyle::clipped_bitmap_fill || pFillStyle->GetType()==TFillStyle::tiled_bitmap_fill))
					{
						TBitmapFillStyle* pBitmapFillStyle = (TBitmapFillStyle*)pFillStyle;
						if(CTraitImage(pBitmapFillStyle->GetImage())==_gimg)
						{
							if(GetCurSelFill()!=pFillStyle)
							{
								AfxMessageBox(IDS_ERROR_CANNOT_DELETE_USED_IMAGE);
								return;
							}
						}
					}
				}
			}
		}
		else
		{
			AfxMessageBox(IDS_ERROR_CANNOT_DELETE_USED_IMAGE);
			return;
		}
	}

	// 从List中删除
	if(m_lstImage.GetItemCount()==1)
	{
		// 改变填充风格为无
		m_cmbFillStyle.SetCurSel(0);
		OnCbnSelchangeComboFillType();
	}
	else
	{
		// 选中其他的项并且NOTIFY
		m_lstImage.SelectItem(m_lstImage.GetItemCount()==index+1 ? (index-1) : (index+1), TRUE);
	}

	// 再删除对应的Item
	m_lstImage.DeleteItem(index);

	if(!m_pCmdDeleteImages)
	{
		m_pCmdDeleteImages = new TCommandGroup();
	}
	m_pCmdDeleteImages->Do(new CCmdRemoveObj(_gimg));
}

gldImage* CEditShapeFillPage::ImportImage()
{
	CString strFile;

	CString strTitle, strFilter;
	strTitle.LoadString(IDS_IMPORT_IMAGE_TITLE);
	strFilter.LoadString(IDS_FILTER_IMAGE_ALL);
	CFileDialogEx dlg(strTitle, TRUE, NULL, strFilter);
	dlg.m_pOFN->lpstrTitle = strTitle;
	if (dlg.DoModal() != IDOK)
		return NULL;
	strFile = dlg.GetPathName();

	gldImage *_gimg = new gldImage();
	if (!_gimg->ReadImageData(strFile))
	{
		delete _gimg;
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, strFile);
		return NULL;
	}

	CSWFParse	parse;
	CTransAdaptor::CreateTShapePtr(&parse, _gimg);

	::PathStripPath(strFile.GetBuffer());
	strFile.ReleaseBuffer();
	_gimg->m_name = strFile;

	//_gimg->UpdateUniId();
	_GetObjectMap()->UpdateUniId(_gimg);
	_GetObjectMap()->GenerateNextSymbolName(_gimg->m_name);

	if(!m_pCmdImportImages)
	{
		m_pCmdImportImages = new TCommandGroup();
	}
	m_pCmdImportImages->Do(new CCmdAddObj(_gimg));

	m_lstImage.AddImage(_gimg);

	return _gimg;
}

void CEditShapeFillPage::OnDestroy()
{
	AfxGetApp()->WriteProfileInt("Property Pages\\TransformFillStyle", "Constrain Angle", m_btnConstrainAngle.GetCheck());
	AfxGetApp()->WriteProfileInt("Property Pages\\TransformFillStyle", "Constrain Size", m_btnConstrainSize.GetCheck());

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_FILL_LIST);
	ASSERT(pListBox != NULL);
	for (int i = 0; i < pListBox->GetCount(); i++)
	{
		_FillItemData *pData = (_FillItemData *)pListBox->GetItemData(i);
		ASSERT(pData != NULL);
		delete pData;
	}
	CPropertyPage::OnDestroy();
}


void CEditShapeFillPage::OnBnClickedButtonConstrainAngle()
{
	if (m_btnConstrainAngle.GetCheck())
		OnEditModifyXSkew();
}

void CEditShapeFillPage::OnBnClickedButtonConstrainSize()
{
	if (m_btnConstrainSize.GetCheck())
		OnEditModifyWScale();
}

void CEditShapeFillPage::OnEditModifyDimension()
{
	BOOL bTrans;
	int x = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_COORX, &bTrans) * 20);
	int y = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_COORY, &bTrans) * 20);
	int w = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_WIDTH, &bTrans) * 20);
	int h = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_HEIGHT, &bTrans) * 20);
	SetDimension(x, y, w, h);
	UpdateMatrixControls();
}

void CEditShapeFillPage::OnEditModifyMatrix()
{
	BOOL bTrans;
	float scalex = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_SCALE_W, &bTrans) / 100;
	float scaley = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_SCALE_H, &bTrans) / 100;
	float skewx = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_SKEW_X, &bTrans);
	float skewy = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_SKEW_Y, &bTrans);
	if(GetFillStyleType()==TFillStyle::linear_gradient_fill)
	{
		skewy = skewx;
	}
	SetMatrix(scalex, scaley, skewx, skewy);
	UpdateMatrixControls();
}

void CEditShapeFillPage::OnEditModifyWScale()
{
	if (m_btnConstrainSize.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_SCALE_W, strText);
		SetDlgItemText(IDC_EDIT_SCALE_H, strText);
	}

	OnEditModifyMatrix();
}

void CEditShapeFillPage::OnEditModifyHScale()
{
	if (m_btnConstrainSize.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_SCALE_H, strText);
		SetDlgItemText(IDC_EDIT_SCALE_W, strText);
	}

	OnEditModifyMatrix();
}

void CEditShapeFillPage::OnEditModifyXSkew()
{
	if (m_btnConstrainAngle.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_SKEW_X, strText);
		SetDlgItemText(IDC_EDIT_SKEW_Y, strText);
	}

	OnEditModifyMatrix();
}

void CEditShapeFillPage::OnEditModifyYSkew()
{
	if (m_btnConstrainAngle.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_SKEW_Y, strText);
		SetDlgItemText(IDC_EDIT_SKEW_X, strText);
	}

	OnEditModifyMatrix();
}

void CEditShapeFillPage::UpdateMatrixControls()
{
	SetDlgItemFloat(m_hWnd, IDC_EDIT_COORX, (double)m_nCoorX / 20, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_COORY, (double)m_nCoorY / 20, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_WIDTH, (double)m_nWidth / 20, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_HEIGHT, (double)m_nHeight / 20, 1, TRUE);

	SetDlgItemFloat(m_hWnd, IDC_EDIT_SCALE_W, m_fScaleX * 100, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_SCALE_H, m_fScaleY * 100, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_SKEW_X, m_fSkewX, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_SKEW_Y, m_fSkewY, 1, TRUE);
}

void CEditShapeFillPage::SetMatrix(float scalex, float scaley, float skewx, float skewy)
{
	if (fabsf(scalex) < 0.01f) scalex = 0.01f;
	if (fabsf(scaley) < 0.01f) scaley = 0.01f;

	gld_shape	shape = CTraitShape(m_pObj);
	gld_point   op = shape.trans_origin();
	int			ox = op.x;
	int			oy = op.y;
	m_matrix.Transform(ox, oy);

	m_matrix.SetTransformParam(scalex, scaley, skewx, skewy, 0, 0);			
	m_matrix.Transform(op.x, op.y);
	m_matrix.m_dx = ox - op.x;
	m_matrix.m_dy = oy - op.y;


	RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);

	UpdateSelectedShapes();
}

void CEditShapeFillPage::SetDimension(int x, int y, int w, int h)
{
	AfxLimitValue(w, IDI_MIN_SHAPE_WIDTH, IDI_MAX_SHAPE_WIDTH);
	AfxLimitValue(h, IDI_MIN_SHAPE_HEIGHT, IDI_MAX_SHAPE_HEIGHT);

	gld_shape shape = CTraitShape(m_pObj);
	gld_rect	box = _transform_rect(shape.bound(), m_matrix);

	int ox, oy;

	gld_point op = shape.trans_origin();
	ox = op.x;
	oy = op.y;
	TMatrix   smat = m_matrix;
	smat.Transform(ox, oy);

	int ow = box.width();
	int oh = box.height();

	TMatrix	scalemat, movemat;
	scalemat.SetScaleMatrix((float)w / ow, (float)h / oh, ox, oy);
	movemat.SetMoveMatrix(x - ox, y - oy);
	TMatrix	mat = scalemat * movemat;

	m_matrix = m_matrix * mat;

	RecalcTransformParam(m_nCoorX, m_nCoorY, m_nWidth, m_nHeight, m_fScaleX, m_fScaleY, m_fSkewX, m_fSkewY, shape, m_matrix);

	UpdateSelectedShapes();
}
