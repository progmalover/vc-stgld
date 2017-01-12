// EditShapeLinePage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditShapeLinePage.h"
#include ".\EditShapeLinePage.h"
#include "gldFillStyle.h"
#include "gldInstance.h"
#include "gldShape.h"
#include "TransAdaptor.h"
#include "Graphics.h"
#include "ColorPicker.h"
#include "gldShapeRecordChange.h"
#include "DrawHelper.h"

#define COLOR_MODE_INVALID			0
#define COLOR_MODE_RGB				1
#define COLOR_MODE_HSV				2


// CEditShapeLinePage dialog

IMPLEMENT_DYNAMIC(CEditShapeLinePage, CPropertyPage)
CEditShapeLinePage::CEditShapeLinePage(gldObj* pObj)
: CPropertyPage(CEditShapeLinePage::IDD)
, m_pObj(pObj)
, m_fLineWidth(1.00)	
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

	m_nColorMode = COLOR_MODE_INVALID;

}

CEditShapeLinePage::~CEditShapeLinePage()
{
}

BOOL CEditShapeLinePage::HasLines(gldObj* pObj)
{
	BOOL bHasLines = FALSE;
	gld_shape shape = CTraitShape(pObj);
	for (gld_draw_obj_iter il = shape.begin_draw_obj(); il != shape.end_draw_obj(); ++il)
	{
		TLineStyleTable &lsTab = (*il).line_styles();
		if(lsTab.Count())
		{
			bHasLines = TRUE;
			break;
		}
	}
	return bHasLines;
}

void CEditShapeLinePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_LINE_COLOR, m_btnLineColor);
	DDX_Control(pDX, IDC_STATIC_SLIDER_A, m_btnA);
	DDX_Control(pDX, IDC_STATIC_COLOR_NEW, m_stcFillStyle);
	DDX_Control(pDX, IDC_STATIC_SLIDER_B, m_btnB);
	DDX_Control(pDX, IDC_STATIC_SLIDER_G, m_btnG);
	DDX_Control(pDX, IDC_STATIC_SLIDER_R, m_btnR);
	DDX_Control(pDX, IDC_STATIC_SLIDER_V, m_btnV);
	DDX_Control(pDX, IDC_STATIC_SLIDER_S, m_btnS);
	DDX_Control(pDX, IDC_STATIC_SLIDER_H, m_btnH);
	DDX_Control(pDX, IDC_STATIC_SLIDER_LINE_WIDTH, m_btnLineWidth);
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
	DDX_Control(pDX, IDC_EDIT_LINE_WIDTH, m_edtLineWidth);
	DDX_Control(pDX, IDC_EDIT_HEX, m_edtHex);
	DDX_Text(pDX, IDC_EDIT_HEX, m_strHEXValue);

	DDX_Control(pDX, IDC_EDIT_H, m_edtH);
	DDX_Control(pDX, IDC_EDIT_S, m_edtS);
	DDX_Control(pDX, IDC_EDIT_V, m_edtV);
	DDX_Control(pDX, IDC_EDIT_R, m_edtR);
	DDX_Control(pDX, IDC_EDIT_G, m_edtG);
	DDX_Control(pDX, IDC_EDIT_B, m_edtB);
	DDX_Control(pDX, IDC_EDIT_A, m_edtA);

}


BEGIN_MESSAGE_MAP(CEditShapeLinePage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_LINE_LIST, OnLbnSelchangeLineList)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_LINE_COLOR, OnLinecolorChanged)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_H, OnEditChangeHSV)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_S, OnEditChangeHSV)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_V, OnEditChangeHSV)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_A, OnEditChangeA)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_R, OnEditChangeRGB)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_G, OnEditChangeRGB)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_B, OnEditChangeRGB)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_LINE_WIDTH, OnEditChangeLineWidth)

	ON_EN_CHANGE(IDC_EDIT_HEX, OnEditChangeHex)

	ON_CONTROL(CPN_HCHANGE, IDC_STATIC_H, OnPaletteChangeH)
	ON_CONTROL(CPN_SVCHANGE, IDC_STATIC_SV, OnPaletteChangeSV)

	ON_WM_DESTROY()
END_MESSAGE_MAP()


struct _LineItemData
{
	gld_draw_obj drawObj;
	int lineIndex;
};
// CEditShapeLinePage message handlers

BOOL CEditShapeLinePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LINE_LIST);
	ASSERT(pListBox != NULL);
	int index = 1;
	CString lineName;
	gld_shape shape = CTraitShape(m_pObj);
	for (gld_draw_obj_iter i = shape.begin_draw_obj(); i != shape.end_draw_obj(); ++i)
	{
		TLineStyleTable &lsTab = (*i).line_styles();
		for (int l = 0; l < lsTab.Count(); l++)
		{
			lineName.Format(IDS_LINESTYLE_d, index++);
			int itemIndex = pListBox->AddString(lineName);
			_LineItemData *pData = new _LineItemData;
			pData->drawObj = *i;
			pData->lineIndex = l + 1;
			pListBox->SetItemData(itemIndex, (DWORD_PTR)pData);
		}
	}

	m_edtH.SetRange(0, 360, 0, FALSE, TRUE);
	m_edtS.SetRange(0, 100, 0, FALSE, TRUE);
	m_edtV.SetRange(0, 100, 0, FALSE, TRUE);

	m_edtR.SetRange(0, 255, 0, FALSE, TRUE);
	m_edtG.SetRange(0, 255, 0, FALSE, TRUE);
	m_edtB.SetRange(0, 255, 0, FALSE, TRUE);

	m_edtA.SetRange(0, 100, 0, FALSE, TRUE);

	m_edtLineWidth.SetRange(2, 0, 10, 1, FALSE, TRUE);

	m_btnH.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_H));
	m_btnS.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_S));
	m_btnV.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_V));

	m_btnR.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_R));
	m_btnG.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_G));
	m_btnB.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_B));

	m_btnA.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_A));

	m_btnLineWidth.SetBuddyWindow(::GetDlgItem(m_hWnd, IDC_EDIT_LINE_WIDTH));

	m_btnH.SetRange(0, MAX_H);
	m_btnS.SetRange(0, 100);
	m_btnV.SetRange(0, 100);

	m_btnR.SetRange(0, 255);
	m_btnG.SetRange(0, 255);
	m_btnB.SetRange(0, 255);

	m_btnA.SetRange(0, 100);

	m_btnLineWidth.SetRange(0, 1000 / 25);

	m_stcH.SetStyle(FALSE);

	SetColorMode(COLOR_MODE_RGB);


	m_bInit = TRUE;
	SetColor(RGB(0xff, 0x00, 0x00), 255);

	// 选择LISTBOX第一项
	pListBox->SetCurSel(0);
	OnLbnSelchangeLineList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditShapeLinePage::SetColorMode(int nMode)
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


void CEditShapeLinePage::SetColor(COLORREF color, int alpha)
{
	m_nRValue = GetRValue(color);
	m_nGValue = GetGValue(color);
	m_nBValue = GetBValue(color);

	m_nAValue = alpha * 100 / 255;
	if (alpha * 100 % 255 >= 128)
		m_nAValue++;

	if (::IsWindow(m_hWnd))
	{
		SyncHSVValue();
		SyncHEXValue();

		UpdateAllControls();
	}
}

COLORREF CEditShapeLinePage::GetColor()
{
	return RGB(m_nRValue, m_nGValue, m_nBValue);
}

int CEditShapeLinePage::GetAlpha()
{
	return m_nAValue * 255 / 100;
}

void CEditShapeLinePage::HandleLineSelChange()
{
	TLineStyle* pLineStyle = GetCurSelLine();
	if(pLineStyle)
	{
		TColor clr =pLineStyle->GetColor();
		if(pLineStyle->GetWidth()==1 && clr.m_a==0)
		{
			m_fLineWidth = 0;
			SetDlgItemFloat(m_hWnd, IDC_EDIT_LINE_WIDTH, m_fLineWidth, 2, TRUE);
			SetColor(RGB(clr.m_r, clr.m_g, clr.m_b), 255);
		}
		else
		{
			m_fLineWidth = (float)pLineStyle->GetWidth() / 20;
			SetDlgItemFloat(m_hWnd, IDC_EDIT_LINE_WIDTH, m_fLineWidth, 2, TRUE);
			SetColor(RGB(clr.m_r, clr.m_g, clr.m_b), clr.m_a);
		}
	}

	BOOL bEnable = pLineStyle ? TRUE : FALSE;
	m_edtR.EnableWindow(bEnable);
	m_edtG.EnableWindow(bEnable);
	m_edtB.EnableWindow(bEnable);
	m_edtH.EnableWindow(bEnable);
	m_edtS.EnableWindow(bEnable);
	m_edtV.EnableWindow(bEnable);
	m_edtA.EnableWindow(bEnable);
	m_edtHex.EnableWindow(bEnable);

	m_btnR.EnableWindow(bEnable);
	m_btnG.EnableWindow(bEnable);
	m_btnB.EnableWindow(bEnable);
	m_btnH.EnableWindow(bEnable);
	m_btnS.EnableWindow(bEnable);
	m_btnV.EnableWindow(bEnable);
	m_btnA.EnableWindow(bEnable);

	m_edtLineWidth.EnableWindow(bEnable);
	m_btnLineWidth.EnableWindow(bEnable);
	m_stcH.EnableWindow(bEnable);
	m_stcSV.EnableWindow(bEnable);
	m_btnLineColor.EnableWindow(bEnable);
}

void CEditShapeLinePage::SyncRGBValue()
{
	HSVType hsv = {m_nHValue, m_nSValue, m_nVValue};
	RGBType rgb = hsv.toRGB();

	m_nRValue = rgb.r;
	m_nGValue = rgb.g;
	m_nBValue = rgb.b;
}

void CEditShapeLinePage::SyncHSVValue()
{
	RGBType rgb = {m_nRValue, m_nGValue, m_nBValue};
	HSVType hsv = rgb.toHSV();

	m_nHValue = hsv.h;
	m_nSValue = hsv.s;
	m_nVValue = hsv.v;
}

void CEditShapeLinePage::SyncHEXValue()
{
	COLORREF rgb = RGB(m_nRValue, m_nGValue, m_nBValue);
	m_strHEXValue = RGBToString(rgb);
}

void CEditShapeLinePage::UpdateAllControls()
{
	UpdateData(FALSE);

	SetDlgItemInt(IDC_EDIT_H, m_nHValue);
	SetDlgItemInt(IDC_EDIT_S, round((float)m_nSValue * 100 / MAX_S));
	SetDlgItemInt(IDC_EDIT_V, round((float)m_nVValue * 100 / MAX_V));

	UpdatePalette();
	UpdateColorButton();
	UpdateColorSample();
}

void CEditShapeLinePage::UpdatePalette()
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


void CEditShapeLinePage::UpdateColorSample()
{
	COLORREF color = m_btnLineColor.GetColor();
	TSolidFillStyle *pStyle = new TSolidFillStyle(TColor(GetRValue(color), GetGValue(color), GetBValue(color), m_btnLineColor.GetAlpha()));
	m_stcFillStyle.SetFillStyle(pStyle);
}

void CEditShapeLinePage::UpdateColorButton()
{
	m_btnLineColor.SetColor(RGB(m_nRValue, m_nGValue, m_nBValue), m_nAValue * 255 / 100);
}

void CEditShapeLinePage::OnLbnSelchangeLineList()
{
	HandleLineSelChange();
}

TLineStyle* CEditShapeLinePage::GetCurSelLine()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LINE_LIST);
	int nIndex = pListBox->GetCurSel();
	if(nIndex > -1)
	{
		_LineItemData *pData = (_LineItemData *)pListBox->GetItemData(nIndex);
		ASSERT(pData != NULL);
		return pData->drawObj.line_styles().Get(pData->lineIndex);
	}
	return NULL;
}


void CEditShapeLinePage::UpdateSelectedShapes()
{
	TLineStyle *pLineStyle;
	if(m_fLineWidth==0)
	{
		pLineStyle = new TLineStyle(1, TColor(m_nRValue, m_nGValue, m_nBValue, 0));
	}
	else
	{
		pLineStyle = new TLineStyle((int)(m_fLineWidth * 20), TColor(m_nRValue, m_nGValue, m_nBValue, GetAlpha()));
	}
	
	CGuardDrawOnce xDraw;

	REFRENCE(pLineStyle);


	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LINE_LIST);
	ASSERT(pListBox != NULL);
	int nIndex = pListBox->GetCurSel();
	if(nIndex > -1)
	{
		// Undo当前选中线型
		for(std::map<int, CCmdModifyLineStyle*>::iterator iLine = m_CmdModifyLineStyleList.begin();
			iLine != m_CmdModifyLineStyleList.end(); ++iLine)
		{
			if(iLine->first==nIndex)
			{
				iLine->second->Unexecute();
				delete iLine->second;
				m_CmdModifyLineStyleList.erase(iLine);
				break;
			}
		}

		_LineItemData *pData = (_LineItemData *)pListBox->GetItemData(nIndex);
		ASSERT(pData != NULL);

		CCmdModifyLineStyle* pCmd = new CCmdModifyLineStyle(m_pObj, pData->drawObj, pData->lineIndex, pLineStyle);
		pCmd->Execute();
		m_CmdModifyLineStyleList[nIndex] = pCmd;
	}
	
	RELEASE(pLineStyle);
	
}

void CEditShapeLinePage::OnLinecolorChanged()
{
	SetColor(m_btnLineColor.GetColor(), m_btnLineColor.GetAlpha());
	UpdateSelectedShapes();
}

void CEditShapeLinePage::OnEditChangeHSV() 
{
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

void CEditShapeLinePage::OnEditChangeRGB() 
{
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


void CEditShapeLinePage::OnEditChangeA() 
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


void CEditShapeLinePage::OnEditChangeLineWidth()
{
	if (!m_bInit)
		return;

	m_fLineWidth = (float)m_edtLineWidth.GetValueFloat();

	UpdateSelectedShapes();
}

void CEditShapeLinePage::OnEditChangeHex()
{
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

void CEditShapeLinePage::OnPaletteChangeH() 
{
	m_stcSV.SetHValue(m_stcH.GetHValue());
	OnPaletteChangeSV();
}

void CEditShapeLinePage::OnPaletteChangeSV() 
{
	m_nHValue = m_stcSV.GetHValue();
	m_nSValue = m_stcSV.GetSValue();
	m_nVValue = m_stcSV.GetVValue();

	SyncRGBValue();
	SyncHEXValue();

	UpdateAllControls();

	if (!m_stcH.IsTracking() && !m_stcSV.IsTracking())
		UpdateSelectedShapes();
}

void CEditShapeLinePage::OnDestroy()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LINE_LIST);
	ASSERT(pListBox != NULL);
	for (int i = 0; i < pListBox->GetCount(); i++)
	{
		_LineItemData *pData = (_LineItemData *)pListBox->GetItemData(i);
		ASSERT(pData != NULL);
		delete pData;
	}

	CPropertyPage::OnDestroy();

	// TODO: Add your message handler code here
}
