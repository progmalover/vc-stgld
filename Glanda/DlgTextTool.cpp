// DlgTextTool.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgTextTool.h"

#include "my_app.h"

#include "gldText.h"
#include "gldColor.h"
#include "gldFont.h"
#include "gldTextGlyphRecord.h"
#include "gldLibrary.h"
#include "gldDataKeeper.h"

#include "cmdchangetext.h"
#include "TransAdaptor.h"

#include "toolsdef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgTextTool dialog

CDlgTextTool::CDlgTextTool(UINT nIDTemplate, CWnd *pParentWnd)
: CDlgToolOption(nIDTemplate, pParentWnd)
{
	m_bShowAdvOptions = AfxGetApp()->GetProfileInt("Tools\\TextEx", "Show Advanced Options", FALSE);
}

CDlgTextTool::~CDlgTextTool()
{
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Show Advanced Options", m_bShowAdvOptions);
}

void CDlgTextTool::DoDataExchange(CDataExchange* pDX)
{
	CDlgToolOption::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_COLOR, m_btnColor);
	DDX_Control(pDX, IDC_BUTTON_BOLD, m_btnBold);
	DDX_Control(pDX, IDC_BUTTON_ITALIC, m_btnItalic);
	DDX_Control(pDX, IDC_COMBO_FONT, m_cmbFont);
	DDX_Control(pDX, IDC_STATIC_SPIN_SIZE, m_btnSize);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_edtSize);
	DDX_Control(pDX, IDC_STATIC_CHAR_SPACING, m_stcCharSpacing);
	DDX_Control(pDX, IDC_STATIC_LINE_SPACING, m_stcLineSpacing);

	DDX_Control(pDX, IDC_EDIT_CHAR_SPACING, m_edtCharSpacing);
	DDX_Control(pDX, IDC_STATIC_SPIN_CHAR_SPACING, m_btnCharSpacing);
	DDX_Control(pDX, IDC_EDIT_LINE_SPACING, m_edtLineSpacing);
	DDX_Control(pDX, IDC_STATIC_SPIN_LINE_SPACING, m_btnLineSpacing);

	DDX_Control(pDX, IDC_BUTTON_ALIGN_LEFT, m_btnAlignLeft);
	DDX_Control(pDX, IDC_BUTTON_ALIGN_CENTER, m_btnAlignCenter);
	DDX_Control(pDX, IDC_BUTTON_ALIGN_RIGHT, m_btnAlignRight);

	DDX_Control(pDX, IDC_BUTTON_SELECTABLE, m_btnSelectable);
	DDX_Control(pDX, IDC_BUTTON_BORDER, m_btnBorder);
	DDX_Control(pDX, IDC_BUTTON_EDITABLE, m_btnEditable);

	DDX_Control(pDX, IDC_STATIC_INDENT, m_stcIndent);
	DDX_Control(pDX, IDC_STATIC_LEFT_MARGIN, m_stcLeftMargin);
	DDX_Control(pDX, IDC_STATIC_RIGHT_MARGIN, m_stcRightMargin);
	DDX_Control(pDX, IDC_STATIC_SPIN_INDENT, m_btnIndent);
	DDX_Control(pDX, IDC_STATIC_SPIN_LEFT_MARGIN, m_btnLeftMargin);
	DDX_Control(pDX, IDC_STATIC_SPIN_RIGHT_MARGIN, m_btnRightMargin);
	DDX_Control(pDX, IDC_EDIT_INDENT, m_edtIndent);
	DDX_Control(pDX, IDC_EDIT_LEFT_MARGIN, m_edtLeftMargin);
	DDX_Control(pDX, IDC_EDIT_RIGHT_MARGIN, m_edtRightMargin);

	DDX_Control(pDX, IDC_COMBO_CHAR_POS, m_cmbCharPos);
	DDX_Control(pDX, IDC_STATIC_CHAR_POS, m_stcCharPos);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cmbTextType);

	DDX_Control(pDX, IDC_STATIC_LINK, m_stcLink);
	DDX_Control(pDX, IDC_EDIT_LINK, m_edtLink);
	DDX_Control(pDX, IDC_COMBO_TARGET, m_cmbTarget);

	DDX_Control(pDX, IDC_EDIT_VARIABLE, m_edtVariable);

	DDX_Control(pDX, IDC_COMBO_MULTILINE, m_cmbMultiline);
	DDX_Control(pDX, IDC_EDIT_MAXLENGTH, m_edtMaxLength);

	DDX_Control(pDX, IDC_STATIC_ADV_OPTIONS, m_stcAdvOptions);
}


BEGIN_MESSAGE_MAP(CDlgTextTool, CDlgToolOption)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_COLOR, OnColorChanged)
	ON_BN_CLICKED(IDC_BUTTON_BOLD, OnBold)
	ON_BN_CLICKED(IDC_BUTTON_ITALIC, OnItalic)
	ON_CONTROL(CBN_MODIFY, IDC_COMBO_FONT, OnEditChangeFont)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SIZE, OnSizeModified)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_CHAR_SPACING, OnCharSpacingModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_LINE_SPACING, OnLineSpacingModified)
	ON_BN_CLICKED(IDC_BUTTON_ALIGN_LEFT, OnBnClickedButtonAlignLeft)
	ON_BN_CLICKED(IDC_BUTTON_ALIGN_CENTER, OnBnClickedButtonAlignCenter)
	ON_BN_CLICKED(IDC_BUTTON_ALIGN_RIGHT, OnBnClickedButtonAlignRight)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_INDENT, OnIndentModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_LEFT_MARGIN, OnLeftModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_RIGHT_MARGIN, OnRightMarginModified)
	ON_CBN_SELCHANGE(IDC_COMBO_CHAR_POS, OnCbnSelchangeComboCharPos)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BUTTON_BORDER, OnBnClickedButtonBorder)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_LINK, OnLinkModified)
	ON_CONTROL(CBN_MODIFY, IDC_COMBO_TARGET, OnEditChangeTarget)
	ON_BN_CLICKED(IDC_BUTTON_SELECTABLE, OnBnClickedButtonSelectable)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_VARIABLE, OnVariableModified)
	ON_BN_CLICKED(IDC_BUTTON_EDITABLE, OnBnClickedButtonEditable)

	ON_CBN_SELCHANGE(IDC_COMBO_MULTILINE, OnCbnChangeMultiline)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_MAXLENGTH, OnEditChangeMaxLength)

	ON_BN_CLICKED(IDC_STATIC_ADV_OPTIONS, OnBnClickedAdvOptions)
END_MESSAGE_MAP()


// CDlgTextTool message handlers

BOOL CDlgTextTool::OnInitDialog()
{
	CDlgToolOption::OnInitDialog();

	FillComboBox(&m_cmbTextType, IDS_LIST_TEXTTYPE);
	FillComboBox(&m_cmbMultiline, IDS_LIST_MULTITYPE);

	m_cmbFont.EnumFonts();
	CDC *pDC = GetDC();
	CFont *pFont = GetFont();
	CFont *pFontLast = NULL;
	if (pFont)
		pFontLast = pDC->SelectObject(pFont);

	int nWidthMax = 0;
	for (int i = 0; i < m_cmbFont.GetCount(); i++)
	{
		CString strText;
		m_cmbFont.GetLBText(i, strText);
		nWidthMax = max(nWidthMax, pDC->GetTextExtent(strText).cx + ::GetSystemMetrics(SM_CXVSCROLL) + 5);
	}

	if (pFontLast)
		pDC->SelectObject(pFontLast);
	ReleaseDC(pDC);

	if (nWidthMax > m_cmbFont.GetDroppedWidth())
		m_cmbFont.SetDroppedWidth(nWidthMax);

	m_edtSize.SetRange(1, 2048, 9, FALSE, TRUE);
	m_btnSize.SetRange(8, 96);
	m_btnSize.SetBuddyWindow(m_edtSize.m_hWnd);

	m_btnBold.LoadBitmap(IDB_BOLD);
	m_btnItalic.LoadBitmap(IDB_ITALIC);

	//m_btnBold.ModifyFBStyle(FBS_FLAT, 0);
	//m_btnItalic.ModifyFBStyle(FBS_FLAT, 0);

	m_edtCharSpacing.SetRange(-60, 60, 0, FALSE, TRUE);
	m_btnCharSpacing.SetRange(-60, 60);
	m_btnCharSpacing.SetBuddyWindow(m_edtCharSpacing.m_hWnd);

	m_edtLineSpacing.SetRange(-360, 360, 0, FALSE, TRUE);
	m_btnLineSpacing.SetRange(-360, 360);
	m_btnLineSpacing.SetBuddyWindow(m_edtLineSpacing.m_hWnd);

	m_btnAlignLeft.LoadBitmap(IDB_ALIGN_LEFT);
	m_btnAlignCenter.LoadBitmap(IDB_ALIGN_CENTER);
	m_btnAlignRight.LoadBitmap(IDB_ALIGN_RIGHT);

	//m_btnAlignLeft.ModifyFBStyle(FBS_FLAT, 0);
	//m_btnAlignCenter.ModifyFBStyle(FBS_FLAT, 0);
	//m_btnAlignRight.ModifyFBStyle(FBS_FLAT, 0);

	m_btnSelectable.LoadBitmap(IDB_BUTTON_SELECTABLE);
	m_btnBorder.LoadBitmap(IDB_BUTTON_BORDER);
	m_btnEditable.LoadBitmap(IDB_BUTTON_EDITABLE);

	m_btnSelectable.ModifyFBStyle(FBS_FLAT, 0);
	m_btnBorder.ModifyFBStyle(FBS_FLAT, 0);
	m_btnEditable.ModifyFBStyle(FBS_FLAT, 0);

	m_edtIndent.SetRange(-200, 200, 0, FALSE, TRUE);
	m_btnIndent.SetRange(-200, 200);
	m_btnIndent.SetBuddyWindow(m_edtIndent.m_hWnd);

	m_edtLeftMargin.SetRange(0, 200, 0, FALSE, TRUE);
	m_btnLeftMargin.SetRange(0, 200);
	m_btnLeftMargin.SetBuddyWindow(m_edtLeftMargin.m_hWnd);

	m_edtRightMargin.SetRange(0, 200, 0, FALSE, TRUE);
	m_btnRightMargin.SetRange(0, 200);
	m_btnRightMargin.SetBuddyWindow(m_edtRightMargin.m_hWnd);

	::FillComboBox(&m_cmbCharPos, IDS_LIST_CHAR_POS);

	::FillComboBox(&m_cmbTarget, IDS_LIST_TARGET);

	m_stcAdvOptions.SetHoverCursor(AfxGetApp()->LoadCursor(IDC_MY_HAND));

	AddToolTip(IDC_COMBO_TYPE, IDS_TIP_TEXT_TYPE);
	AddToolTip(IDC_COMBO_FONT, IDS_TIP_TEXT_FONT);
	AddToolTip(IDC_EDIT_SIZE, IDS_TIP_TEXT_FONT_SIZE);
	AddToolTip(IDC_EDIT_CHAR_SPACING, IDS_TIP_TEXT_CHAR_SPACING);
	AddToolTip(IDC_COMBO_CHAR_POS, IDS_TIP_TEXT_CHAR_POS);
	AddToolTip(IDC_EDIT_INDENT, IDS_TIP_TEXT_INDENT);
	AddToolTip(IDC_EDIT_LINE_SPACING, IDS_TIP_TEXT_LINE_SPACING);
	AddToolTip(IDC_EDIT_LEFT_MARGIN, IDS_TIP_TEXT_LEFT_MARGIN);
	AddToolTip(IDC_EDIT_RIGHT_MARGIN, IDS_TIP_TEXT_RIGHT_MARGIN);
	AddToolTip(IDC_EDIT_VARIABLE, IDS_TIP_TEXT_VARIABLE);

	AddToolTip(IDC_BUTTON_SELECTABLE);
	AddToolTip(IDC_BUTTON_BORDER);
	AddToolTip(IDC_BUTTON_EDITABLE);
	AddToolTip(IDC_BUTTON_COLOR);
	AddToolTip(IDC_BUTTON_BOLD);
	AddToolTip(IDC_BUTTON_ITALIC);
	AddToolTip(IDC_BUTTON_ALIGN_LEFT);
	AddToolTip(IDC_BUTTON_ALIGN_CENTER);
	AddToolTip(IDC_BUTTON_ALIGN_RIGHT);

	AddToolTip(IDC_EDIT_LINK, IDS_TIP_URL);
	AddToolTip(IDC_COMBO_TARGET, IDS_TIP_TARGET);
	AddToolTip(IDC_COMBO_MULTILINE, IDS_TIP_TEXT_STYLE);
	AddToolTip(IDC_EDIT_MAXLENGTH, IDS_TIP_TEXT_MAX_CHARS);

	AddAnchor(IDC_COMBO_FONT, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_EDIT_SIZE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_SPIN_SIZE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_BUTTON_COLOR, TOP_RIGHT, TOP_RIGHT);

	AddAnchor(IDC_EDIT_VARIABLE, TOP_LEFT, TOP_RIGHT);

	AddAnchor(IDC_EDIT_LINK, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_COMBO_TARGET, TOP_RIGHT, TOP_RIGHT);

	UpdateAdvOptions();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTextTool::OnEditChangeFont()
{
	CString strFont;
	m_cmbFont.GetText(strFont);

	SETextToolEx *pTool = GetTextTool();
	pTool->SetFontFace(strFont);
	OnTextChanged();
}

void CDlgTextTool::OnSizeModified()
{
	int nSize = m_edtSize.GetValueInt();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetFontSize(nSize * 20);
	OnTextChanged();
}

void CDlgTextTool::OnColorChanged()
{
	COLORREF color = m_btnColor.GetColor();

	SETextToolEx *pTool = GetTextTool();
	pTool->SetFontColor(color);
	OnTextChanged();
}

void CDlgTextTool::OnBold()
{
	SETextToolEx *pTool = GetTextTool();
	pTool->SetFontBold(!m_btnBold.GetCheck());
	OnTextChanged();
}

void CDlgTextTool::OnItalic()
{
	SETextToolEx *pTool = GetTextTool();
	pTool->SetFontItalic(!m_btnItalic.GetCheck());
	OnTextChanged();
}

void CDlgTextTool::OnCharSpacingModified()
{
	int nValue = m_edtCharSpacing.GetValueInt();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetCharSpacing(nValue * 20);
	OnTextChanged();
}

void CDlgTextTool::OnLineSpacingModified()
{
	int nValue = m_edtLineSpacing.GetValueInt();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetLineSpacing(nValue * 20);
	OnTextChanged();
}

void CDlgTextTool::UpdateControls()
{
	//	SetRedraw(FALSE);

	SETextToolEx *pTool = GetTextTool();

	// Text Property
	TEXT_PROPERTY tp;

	DWORD tpFlags = pTool->GetTextProperty(tp);

	if (tpFlags & TP_TYPE)
	{
		switch(tp.TextType)
		{
		case TextType_Static:
			m_cmbTextType.SetCurSel(0);
			break;
		case TextType_Dynamic:
			m_cmbTextType.SetCurSel(1);
			break;
		case TextType_HTML:
			m_cmbTextType.SetCurSel(2);
			break;
		default:
			m_cmbTextType.SetCurSel(-1);
		}
	}
	else
	{
		m_cmbTextType.SetCurSel(-1);
	}

	if (tpFlags & TP_BORDER)
	{
		m_btnBorder.SetCheck(tp.bBorder ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		m_btnBorder.SetCheck(BST_UNCHECKED);
	}

	if (tpFlags & TP_CANSELECT)
	{
		m_btnSelectable.SetCheck(tp.bCanSelect ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		m_btnSelectable.SetCheck(BST_UNCHECKED);
	}

	if (tpFlags & TP_CANEDIT)
	{
		m_btnEditable.SetCheck(tp.bCanEdit ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		m_btnEditable.SetCheck(BST_UNCHECKED);
	}

	m_edtVariable.SetWindowText(tpFlags & TP_BINDVAR ? tp.strBindVar.c_str() : NULL);

	// Caps
	TTCAPS ttCaps = pTool->GetTextToolCaps();

	GetDlgItem(IDC_COMBO_FONT)->EnableWindow((ttCaps.dwCaps & TTCAPS_FONTFACE) != 0);

	GetDlgItem(IDC_EDIT_SIZE)->EnableWindow((ttCaps.dwCaps & TTCAPS_FONTSIZE) != 0);
	GetDlgItem(IDC_STATIC_SPIN_SIZE)->EnableWindow((ttCaps.dwCaps & TTCAPS_FONTSIZE) != 0);

	GetDlgItem(IDC_BUTTON_COLOR)->EnableWindow((ttCaps.dwCaps & TTCAPS_FONTCOLOR) != 0);
	GetDlgItem(IDC_BUTTON_BOLD)->EnableWindow((ttCaps.dwCaps & TTCAPS_BOLD) != 0);
	GetDlgItem(IDC_BUTTON_ITALIC)->EnableWindow((ttCaps.dwCaps & TTCAPS_ITALIC) != 0);

	GetDlgItem(IDC_BUTTON_ALIGN_LEFT)->EnableWindow((ttCaps.dwCaps & TTCAPS_ALIGN) != 0);
	GetDlgItem(IDC_BUTTON_ALIGN_CENTER)->EnableWindow((ttCaps.dwCaps & TTCAPS_ALIGN) != 0);
	GetDlgItem(IDC_BUTTON_ALIGN_RIGHT)->EnableWindow((ttCaps.dwCaps & TTCAPS_ALIGN) != 0);

	GetDlgItem(IDC_EDIT_CHAR_SPACING)->EnableWindow((ttCaps.dwCaps & TTCAPS_CHARSPACING) != 0);
	GetDlgItem(IDC_STATIC_SPIN_CHAR_SPACING)->EnableWindow((ttCaps.dwCaps & TTCAPS_CHARSPACING) != 0);

	GetDlgItem(IDC_COMBO_CHAR_POS)->EnableWindow((ttCaps.dwCaps & TTCAPS_LOCATE) != 0);

	GetDlgItem(IDC_EDIT_INDENT)->EnableWindow((ttCaps.dwCaps & TTCAPS_INDENT) != 0);
	GetDlgItem(IDC_STATIC_SPIN_INDENT)->EnableWindow((ttCaps.dwCaps & TTCAPS_INDENT) != 0);

	GetDlgItem(IDC_EDIT_LINE_SPACING)->EnableWindow((ttCaps.dwCaps & TTCAPS_LINESPACING) != 0);
	GetDlgItem(IDC_STATIC_SPIN_LINE_SPACING)->EnableWindow((ttCaps.dwCaps & TTCAPS_LINESPACING) != 0);

	GetDlgItem(IDC_EDIT_LEFT_MARGIN)->EnableWindow((ttCaps.dwCaps & TTCAPS_MARGINLEFT) != 0);
	GetDlgItem(IDC_STATIC_SPIN_LEFT_MARGIN)->EnableWindow((ttCaps.dwCaps & TTCAPS_MARGINLEFT) != 0);

	GetDlgItem(IDC_EDIT_RIGHT_MARGIN)->EnableWindow((ttCaps.dwCaps & TTCAPS_MARGINRIGHT) != 0);
	GetDlgItem(IDC_STATIC_SPIN_RIGHT_MARGIN)->EnableWindow((ttCaps.dwCaps & TTCAPS_MARGINRIGHT) != 0);

	GetDlgItem(IDC_BUTTON_BORDER)->EnableWindow((ttCaps.dwCaps & TTCAPS_BORDER) != 0);
	GetDlgItem(IDC_BUTTON_SELECTABLE)->EnableWindow((ttCaps.dwCaps & TTCAPS_CANSELECT) != 0);
	GetDlgItem(IDC_EDIT_VARIABLE)->EnableWindow((ttCaps.dwCaps & TTCAPS_BINDVAR) != 0);
	GetDlgItem(IDC_BUTTON_EDITABLE)->EnableWindow((ttCaps.dwCaps & TTCAPS_CANEDIT) != 0);
	GetDlgItem(IDC_EDIT_MAXLENGTH)->EnableWindow((ttCaps.dwCaps & TTCAPS_MAXLENGTH) != 0);
	GetDlgItem(IDC_COMBO_MULTILINE)->EnableWindow((ttCaps.dwCaps & TTCAPS_MULTITYPE) != 0);

	GetDlgItem(IDC_EDIT_LINK)->EnableWindow((ttCaps.dwCaps & TTCAPS_LINK) != 0);
	GetDlgItem(IDC_COMBO_TARGET)->EnableWindow((ttCaps.dwCaps & TTCAPS_TARGET) != 0);

	// TextFormat
	CTextFormat tFmt;

	DWORD dwFlags = pTool->GetFormat(tFmt);

	m_cmbFont.SetWindowText((dwFlags & TF_FONTFACE) ? tFmt.GetFontFace().c_str() : NULL);

	if (dwFlags & TF_FONTSIZE)
		SetDlgItemInt(IDC_EDIT_SIZE,  tFmt.GetFontSize() / 20);
	else
		SetDlgItemText(IDC_EDIT_SIZE,  NULL);

	m_btnColor.SetColor((dwFlags & TF_FONTCOLOR) ? (tFmt.GetFontColor() & 0x00ffffff) : CLR_INVALID, 255);
	m_btnBold.SetCheck((dwFlags & TF_BOLD) ? (tFmt.GetBold() ? BST_CHECKED : BST_UNCHECKED) : BST_UNCHECKED);
	m_btnItalic.SetCheck((dwFlags & TF_ITALIC) ? (tFmt.GetItalic() ? BST_CHECKED : BST_UNCHECKED) : BST_UNCHECKED);

	if (dwFlags & TF_SPACING)
		SetDlgItemInt(IDC_EDIT_CHAR_SPACING, tFmt.GetSpacing() / 20);
	else
		SetDlgItemText(IDC_EDIT_CHAR_SPACING, NULL);

	if (dwFlags & TF_LOCATE)
		m_cmbCharPos.SetCurSel((int)tFmt.GetLocate());
	else
		m_cmbCharPos.SetCurSel(-1);

	CParaFormat pFmt;
	dwFlags = pTool->GetParaFormat(pFmt);

	if (dwFlags & PF_SPACING)
		SetDlgItemInt(IDC_EDIT_LINE_SPACING, pFmt.GetSpacing() / 20);
	else
		SetDlgItemText(IDC_EDIT_LINE_SPACING, NULL);

	if (dwFlags & PF_ALIGNTYPE)
	{
		CParagraph::emAlignType align = pFmt.GetAlignType();
		m_btnAlignLeft.SetCheck(align == CParagraph::AlignLeft ? BST_CHECKED : BST_UNCHECKED);
		m_btnAlignCenter.SetCheck(align == CParagraph::AlignCenter ? BST_CHECKED : BST_UNCHECKED);
		m_btnAlignRight.SetCheck(align == CParagraph::AlignRight ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		m_btnAlignLeft.SetCheck(BST_UNCHECKED);
		m_btnAlignCenter.SetCheck(BST_UNCHECKED);
		m_btnAlignRight.SetCheck(BST_UNCHECKED);
	}

	if (dwFlags & PF_INDENT)
		SetDlgItemInt(IDC_EDIT_INDENT, pFmt.GetIndent() / 20);
	else
		SetDlgItemText(IDC_EDIT_INDENT, NULL);

	if (dwFlags & PF_MARGINLEFT)
		SetDlgItemInt(IDC_EDIT_LEFT_MARGIN, pFmt.GetMarginLeft() / 20);
	else
		SetDlgItemText(IDC_EDIT_LEFT_MARGIN, NULL);

	if (dwFlags & PF_MARGINRIGHT)
		SetDlgItemInt(IDC_EDIT_RIGHT_MARGIN, pFmt.GetMarginRight() / 20);
	else
		SetDlgItemText(IDC_EDIT_RIGHT_MARGIN, NULL);

	if (dwFlags & TF_LINK)
		SetDlgItemText(IDC_EDIT_LINK, tFmt.GetLink().c_str());
	else
		SetDlgItemText(IDC_EDIT_LINK, NULL);

	if (dwFlags & TF_TARGET)
		SetDlgItemText(IDC_COMBO_TARGET, tFmt.GetTarget().c_str());
	else
		SetDlgItemText(IDC_COMBO_TARGET, NULL);

	if (ttCaps.dwCaps & TTCAPS_MAXLENGTH)
		SetDlgItemInt(IDC_EDIT_MAXLENGTH, tp.nMaxLength);
	else
		SetDlgItemText(IDC_EDIT_MAXLENGTH, NULL);

	if (ttCaps.dwCaps & TTCAPS_MULTITYPE)
	{
		if (tp.MultilineType == Text_Unknown)
		{
			m_cmbMultiline.SetCurSel(-1);
		}
		else
		{
			m_cmbMultiline.SetCurSel((int)tp.MultilineType);
		}
	}
	else
		m_cmbMultiline.SetCurSel(-1);

	//	SetRedraw(TRUE);
	//	RedrawWindow(0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);

	UpdateWindow();
}

void CDlgTextTool::HandleSelChange()
{
	UpdateControls();
}

void CDlgTextTool::OnBnClickedButtonAlignLeft()
{
	if (m_btnAlignLeft.GetCheck() == BST_UNCHECKED)
	{
		SETextToolEx *pTool = GetTextTool();
		pTool->SetAlignType(CParagraph::AlignLeft);
		OnTextChanged();
	}
}

void CDlgTextTool::OnBnClickedButtonAlignCenter()
{
	if (m_btnAlignCenter.GetCheck() == BST_UNCHECKED)
	{
		SETextToolEx *pTool = GetTextTool();
		pTool->SetAlignType(CParagraph::AlignCenter);
		OnTextChanged();
	}
}

void CDlgTextTool::OnBnClickedButtonAlignRight()
{
	if (m_btnAlignRight.GetCheck() == BST_UNCHECKED)
	{
		SETextToolEx *pTool = GetTextTool();
		pTool->SetAlignType(CParagraph::AlignRight);
		OnTextChanged();
	}
}

void CDlgTextTool::OnIndentModified()
{
	int nValue = m_edtIndent.GetValueInt();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetIndent(nValue * 20);
	OnTextChanged();
}

void CDlgTextTool::OnLeftModified()
{
	int nValue = m_edtLeftMargin.GetValueInt();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetMarginLeft(nValue * 20);
	OnTextChanged();
}

void CDlgTextTool::OnRightMarginModified()
{
	int nValue = m_edtRightMargin.GetValueInt();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetMarginRight(nValue * 20);
	OnTextChanged();
}

void CDlgTextTool::OnCbnSelchangeComboCharPos()
{
	ASSERT((int)CTextFormat::emCharLocate::CL_NONE == 0);

	int nSel = m_cmbCharPos.GetCurSel();
	SETextToolEx *pTool = GetTextTool();
	pTool->SetCharLocate((CTextFormat::emCharLocate)nSel);
	OnTextChanged();
}

void CDlgTextTool::OnCbnSelchangeComboType()
{
	int nSel = m_cmbTextType.GetCurSel();
	SETextToolEx *pTool = GetTextTool();

	switch(nSel)
	{
	case 0:
		pTool->ChangeTextType(TextType_Static);
		break;
	case 1:
		pTool->ChangeTextType(TextType_Dynamic);
		break;
	case 2:
		pTool->ChangeTextType(TextType_HTML);
		break;
	}
	OnTextChanged();
}

void CDlgTextTool::OnBnClickedButtonBorder()
{
	TEXT_PROPERTY prop;
	SETextToolEx *pTool = GetTextTool();

	pTool->GetTextProperty(prop);
	pTool->SetBorder(!prop.bBorder);
	OnTextChanged();
}

void CDlgTextTool::OnLinkModified()
{
	CString strValue;
	m_edtLink.GetWindowText(strValue);

	SETextToolEx *pTool = GetTextTool();
	pTool->SetLink(std::string(strValue));
	OnTextChanged();
}

void CDlgTextTool::OnEditChangeTarget()
{
	TRACE0("CDlgTextTool::OnEditChangeTarget()\n");

	CString strValue;
	m_cmbTarget.GetText(strValue);

	SETextToolEx *pTool = GetTextTool();
	pTool->SetTarget(std::string(strValue));
	OnTextChanged();
}

void CDlgTextTool::OnBnClickedButtonSelectable()
{
	TEXT_PROPERTY prop;
	SETextToolEx *pTool = GetTextTool();

	pTool->GetTextProperty(prop);
	pTool->SetCanSelect(!prop.bCanSelect);
	OnTextChanged();
}

void CDlgTextTool::OnVariableModified()
{
	CString strVar;
	SETextToolEx *pTool = GetTextTool();

	m_edtVariable.GetWindowText(strVar);
	pTool->SetBindVar(strVar.GetBuffer());
	OnTextChanged();
}

void CDlgTextTool::OnBnClickedButtonEditable()
{
	TEXT_PROPERTY prop;
	SETextToolEx *pTool = GetTextTool();

	pTool->GetTextProperty(prop);
	pTool->SetCanEdit(!prop.bCanEdit);
	OnTextChanged();
}

void CDlgTextTool::OnCbnChangeMultiline()
{
	SETextToolEx *pTool = GetTextTool();

	pTool->SetMultilineType((emMultilineType)m_cmbMultiline.GetCurSel());
	OnTextChanged();
}

void CDlgTextTool::OnEditChangeMaxLength()
{
	SETextToolEx *pTool = GetTextTool();

	pTool->SetMaxLength(m_edtMaxLength.GetValueInt());
	OnTextChanged();
}

void CDlgTextTool::OnBnClickedAdvOptions()
{
	m_bShowAdvOptions = !m_bShowAdvOptions;
	UpdateAdvOptions();
}

void CDlgTextTool::UpdateAdvOptions()
{
	CString strCap;
	if (m_bShowAdvOptions)
		strCap.LoadString(IDS_HIDE_ADV_OPTIONS);
	else
		strCap.LoadString(IDS_SHOW_ADV_OPTIONS);
	m_stcAdvOptions.SetWindowText(strCap);
	m_stcAdvOptions.Invalidate();

	UINT nCmdShow = m_bShowAdvOptions ? SW_SHOW : SW_HIDE;
	CWnd *pWnd = & m_stcAdvOptions;
	TCHAR szClass[10];
	while (pWnd = pWnd->GetNextWindow())
	{
		if (GetClassName(pWnd->m_hWnd, szClass, 10) > 0 && _tcsicmp(szClass, "SCROLLBAR") != 0)
			pWnd->ShowWindow(nCmdShow);	
	}
}
