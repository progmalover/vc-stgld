// EditMCEditTextDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCEditTextDialog.h"
#include "CmdReplaceButtonObj.h"
#include "my_app.h"
#include "CharConvert.h"
#include "CommandDrawOnce.h"


// CEditMCEditTextDialog dialog

IMPLEMENT_DYNAMIC(CEditMCEditTextDialog, CDialog)
CEditMCEditTextDialog::CEditMCEditTextDialog(std::list<gldCharacterKey*>& keys, gldText2* pText)
	: CDialog(CEditMCEditTextDialog::IDD, NULL)
	, m_keys(keys)
	, m_pText(pText)
	, m_bInit(FALSE)
	, m_pCmd(NULL)
{
	m_btnOffsetX.SetSliderStyle(TRUE, LEFTRIGHT);
	m_btnOffsetY.SetSliderStyle(FALSE, TOPBOTTOM);
}

CEditMCEditTextDialog::~CEditMCEditTextDialog()
{
}

void CEditMCEditTextDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BUTTON_TEXT, m_edtText);
	DDX_Control(pDX, IDC_COMBO_FONT, m_cmbFont);
	DDX_Control(pDX, IDC_BUTTON_COLOR, m_btnColor);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_edtSize);
	DDX_Control(pDX, IDC_STATIC_SPIN_SIZE, m_btnSize);
	DDX_Control(pDX, IDC_BUTTON_BOLD, m_btnBold);
	DDX_Control(pDX, IDC_BUTTON_ITALIC, m_btnItalic);
	DDX_Control(pDX, IDC_EDIT_CHAR_SPACING, m_edtCharSpacing);
	DDX_Control(pDX, IDC_STATIC_SPIN_CHAR_SPACING, m_btnCharSpacing);
	DDX_Control(pDX, IDC_EDIT_OFFSET_X, m_edtOffsetX);
	DDX_Control(pDX, IDC_STATIC_SPIN_OFFSET_X, m_btnOffsetX);
	DDX_Control(pDX, IDC_EDIT_OFFSET_Y, m_edtOffsetY);
	DDX_Control(pDX, IDC_STATIC_SPIN_OFFSET_Y, m_btnOffsetY);
}


BEGIN_MESSAGE_MAP(CEditMCEditTextDialog, CDialog)
	ON_CONTROL(CBN_MODIFY, IDC_COMBO_FONT, OnEditChangeFont)
	ON_BN_CLICKED(IDC_BUTTON_BOLD, OnBold)
	ON_BN_CLICKED(IDC_BUTTON_ITALIC, OnItalic)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_COLOR, OnColorChanged)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SIZE, OnSizeModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_CHAR_SPACING, OnCharSpacingModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_OFFSET_X, OnOffsetXModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_OFFSET_Y, OnOffsetYModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_BUTTON_TEXT, OnButtonText)
END_MESSAGE_MAP()


// CEditMCEditTextDialog message handlers
void CEditMCEditTextDialog::OnTextChanged()
{
	CGuardDrawOnce xDrawOnce;
	CGuardSelKeeper xSelKeeper;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
		m_pCmd = NULL;
	}

	CString	strText;
	GetDlgItemText(IDC_EDIT_BUTTON_TEXT, strText);
	CString strFont;
	m_cmbFont.GetText(strFont);
	int nSize			= m_edtSize.GetValueInt() * 20;
	COLORREF clrColor	= m_btnColor.GetColor();
	BOOL bBold			= m_btnBold.GetCheck()==BST_CHECKED;
	BOOL bItalic		= m_btnItalic.GetCheck()==BST_CHECKED;
	int nSpacing		= m_edtCharSpacing.GetValueInt() * 20;
	int nOffsetX		= m_edtOffsetX.GetValueInt();
	int nOffsetY		= m_edtOffsetY.GetValueInt();

	gldText2* pText = new gldText2();
	pText->m_textType	= m_pText->m_textType;
	pText->m_type		= m_pText->m_type;
	pText->m_dynaInfo	= m_pText->m_dynaInfo;
	pText->rtRegion		= m_pText->rtRegion;
	pText->m_importBounds	= m_pText->m_importBounds;

	CParagraph *para = new CParagraph(NULL);
	if(m_pText->m_ParagraphList.size()==1)
	{
		CParagraph* para2 = *(m_pText->m_ParagraphList.begin());
		para->SetAlignTypeType(para2->GetAlignType());
		para->SetIndent(para2->GetIndent());
		para->SetMarginLeft(para2->GetMarginLeft());
		para->SetMarginRight(para2->GetMarginRight());
		para->SetSpacing(para2->GetSpacing());
	}

	pText->m_ParagraphList.push_back(para);
	CTextFormat tfmt(strFont, nSize, clrColor,
		bBold, bItalic, nSpacing, CTextFormat::CL_NONE, "", "");
	USES_CONVERSION;
	CTextBlock *block = new CTextBlock(tfmt, A2W(strText));
	para->m_TextBlockList.push_back(block);
	pText->m_bounds = m_pText->m_bounds;

	//calculate bouding box
	CDC dc;
	VERIFY(dc.CreateCompatibleDC(NULL));

	CFont font;
	int nPy = dc.GetDeviceCaps(LOGPIXELSY);
	int nPointSize = nSize * 72 / nPy;
	if(font.CreatePointFont(nPointSize * 10, strFont))
	{
		dc.SetTextCharacterExtra(nSpacing);
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		font.GetLogFont(&lf);
		font.DeleteObject();
		lf.lfItalic = bItalic;
		lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
		CFont font2;
		if(font2.CreateFontIndirect(&lf))
		{
			CFont* pOldFont = dc.SelectObject(&font2);
			CSize sz = dc.GetTextExtent(strText);
			sz.cx += BORDER_SIZE * 2;
			sz.cy += BORDER_SIZE * 2;
			dc.SelectObject(pOldFont);

			pText->m_bounds.left	= 0;
			pText->m_bounds.top		= 0;
			pText->m_bounds.right	= sz.cx;
			pText->m_bounds.bottom	= sz.cy;
			//pText->m_bounds.right	= max(sz.cx, m_pText->m_bounds.Width());
			//pText->m_bounds.bottom	= max(sz.cy, m_pText->m_bounds.Height());
		}
	}

	pText->bNoConvert = FALSE;
	ITextTool IClass;
	SETextToolEx TTool(&IClass);
	TTool.ConvertFromGldText(*pText);
	TTool.ConvertToGldText(*pText);
	CSWFProxy::PrepareTShapeForGObj(pText);

	float rx = float(pText->m_bounds.Width()) / float(m_pText->m_bounds.Width()); 
	float ry = float(pText->m_bounds.Height()) / float(m_pText->m_bounds.Height());

	CCommandBuildDrawOnce* pCmd = new CCommandBuildDrawOnce();
	pCmd->Do(new CCmdAddObj(pText));
	for(std::list<gldCharacterKey*>::iterator iter = m_keys.begin(); iter != m_keys.end(); ++iter)
	{
		pCmd->Do(new CCmdReplaceButtonObj(*iter, pText, rx, ry, nOffsetX * 20, nOffsetY * 20));
	}
	pCmd->EndDo();
	m_pCmd = pCmd;
}

BOOL CEditMCEditTextDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Init Controls
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

	m_btnBold.ModifyFBStyle(FBS_FLAT, FBS_CHECK);
	m_btnItalic.ModifyFBStyle(FBS_FLAT, FBS_CHECK);

	m_edtCharSpacing.SetRange(-60, 60, 0, FALSE, TRUE);
	m_btnCharSpacing.SetRange(-60, 60);
	m_btnCharSpacing.SetBuddyWindow(m_edtCharSpacing.m_hWnd);

	m_edtOffsetX.SetRange(-60, 60, 0, FALSE, TRUE);
	m_btnOffsetX.SetRange(-60, 60);
	m_btnOffsetX.SetBuddyWindow(m_edtOffsetX.m_hWnd);

	m_edtOffsetY.SetRange(-60, 60, 0, FALSE, TRUE);
	m_btnOffsetY.SetRange(-60, 60);
	m_btnOffsetY.SetBuddyWindow(m_edtOffsetY.m_hWnd);

	// 初始化控件的值
	CParagraph*	pParagraph	= *m_pText->m_ParagraphList.begin();
	CTextBlock*	pTextBlock	= *pParagraph->m_TextBlockList.begin();
	CTextFormat& textFormat	= pTextBlock->TextFormat;

	CString strText = MY_W2A(pTextBlock->strText.c_str());
	SetDlgItemText(IDC_EDIT_BUTTON_TEXT, strText);
	m_btnColor.SetColor(textFormat.GetFontColor(), 255);
	m_cmbFont.SetWindowText(textFormat.GetFontFace().c_str());
	m_cmbFont.SetWindowText(textFormat.GetFontFace().c_str());
	SetDlgItemInt(IDC_EDIT_SIZE, textFormat.GetFontSize()/20);
	m_btnBold.SetCheck(textFormat.GetBold() ? BST_CHECKED : BST_UNCHECKED);
	m_btnItalic.SetCheck(textFormat.GetItalic() ? BST_CHECKED : BST_UNCHECKED);
	SetDlgItemInt(IDC_EDIT_CHAR_SPACING, textFormat.GetSpacing()/20);

	SetDlgItemInt(IDC_EDIT_OFFSET_X, 0);
	SetDlgItemInt(IDC_EDIT_OFFSET_Y, 0);

	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMCEditTextDialog::OnEditChangeFont()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnSizeModified()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnColorChanged()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnBold()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnItalic()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnCharSpacingModified()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnButtonText()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnOffsetXModified()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

void CEditMCEditTextDialog::OnOffsetYModified()
{
	if(m_bInit)
	{
		OnTextChanged();
	}
}

INT_PTR CEditMCEditTextDialog::DoModal()
{
	INT_PTR ret = CDialog::DoModal();


	return ret;
}
