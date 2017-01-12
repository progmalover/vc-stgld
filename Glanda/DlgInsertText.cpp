// DlgInsertText.cpp : implementation file
//

#include "stdafx.h"
#include "TextToolEx.h"
#include "Glanda.h"
#include ".\dlginserttext.h"
#include "gldEffect.h"
#include "gldEffectManager.h"
#include "EffectCommonParameters.h"
#include "gldMainMovie2.h"
#include "GlandaDoc.h"
#include "filepath.h"
#include "my_app.h"
#include "CmdInsertEffect.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "GLD_PropertySheet.h"

// CDlgInsertText dialog
LPCTSTR CDlgInsertText::REG_KEY = "Dialogs\\Insert Text";

#define MIN_TEXT_SIZE	8
#define MAX_TEXT_SIZE	96
#define MIN_TEXT_SPACE	-60
#define MAX_TEXT_SPACE	60
#define DEFAULT_TEXT_SIZE 24
#define DEFAULT_TEXT_SPACE 0
#define DEFAULT_TEXT_COLOR RGB(0, 0, 0)

CDlgInsertText::CDlgInsertText(CWnd* pParent /*=NULL*/)
	: CResizableToolTipDialog(CDlgInsertText::IDD, pParent)	
	, m_face("")
	, m_size(DEFAULT_TEXT_SIZE)
	, m_space(DEFAULT_TEXT_SPACE)
	, m_bold(false)
	, m_italic(false)
	, m_color(DEFAULT_TEXT_COLOR)
	, m_init(false)	
{
	m_text.LoadString(IDS_INPUT_TEXT_HERE);

	VERIFY(SUCCEEDED(m_options.CoCreateInstance(__uuidof(GLD_Parameters))));
}

CDlgInsertText::~CDlgInsertText()
{
	if (::PathFileExists(m_strPreviewFile))
		::DeleteFile(m_strPreviewFile);
}

// private implement
void CDlgInsertText::FillEffectCombo()
{
	m_dropEffect.SetImageList(gldEffectManager::Instance()->GetImageList());

	CString strNone;
	strNone.LoadString(IDS_SOUND_NONE);
	CString strEntr;
	strEntr.LoadString(IDS_EFFECT_ENTRANCE);
	m_dropEffect.AddString(strNone);
	gldEffectInfo::EffectType curType = gldEffectInfo::typeEnter;
	m_dropEffect.AddString(strEntr, 0, CHierarchyPopup::SEPERATOR);
	GEFFECTINFO_LIST &infoList = gldEffectManager::Instance()->GetEffectInfoList();
	for (GEFFECTINFO_LIST::iterator it = infoList.begin(); it != infoList.end(); it++)
	{
		gldEffectInfo *pEffectInfo = *it;
		gldEffectInfo::EffectType thisType = pEffectInfo->m_type;
		if (thisType == gldEffectInfo::typeMotionPath1 || 
			thisType == gldEffectInfo::typeMotionPath2) // skip motion path effect
		{
			continue;		
		}
		if (!pEffectInfo->m_name.empty()) // skip noname effect
		{			
			if (thisType != curType)
			{
				CString typeName;
				typeName.LoadString(IDS_EFFECT_CUSTOM);				
				if (thisType == gldEffectInfo::typeEmphasize)
					typeName.LoadString(IDS_EFFECT_EMPHASIS);
				else if (thisType == gldEffectInfo::typeExit)
					typeName.LoadString(IDS_EFFECT_EXIT);
				m_dropEffect.AddString(typeName, 0, CHierarchyPopup::SEPERATOR);
				curType = thisType;
			}
			m_dropEffect.AddString(pEffectInfo->m_name.c_str()
				, 0, pEffectInfo->m_iconIndex, FALSE, (LPARAM)pEffectInfo);
		}
	}
	m_dropEffect.SetCurSel(0);
}

void CDlgInsertText::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_EDIT_TEXT, m_edtText);
	DDX_Control(pDX, IDC_BTN_COLOR, m_btnColor);
	DDX_Control(pDX, IDC_COMBO_FONT, m_cmbFont);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_edtSize);
	DDX_Control(pDX, IDC_STATIC_SIZE, m_ssbSize);	
	DDX_Control(pDX, IDC_BTN_BOLD, m_btnBold);
	DDX_Control(pDX, IDC_BTN_ITALIC, m_btnItalic);
	DDX_Control(pDX, IDC_EDIT_SPACE, m_edtSpace);
	DDX_Control(pDX, IDC_STATIC_SPACE, m_ssbSpace);
	DDX_Control(pDX, IDC_BTN_CONFIG, m_btnConfig);
	DDX_Control(pDX, IDC_DROP_EFFECT, m_dropEffect);
	DDX_Control(pDX, IDC_STATIC_TEXT_TOOL, m_sttTextTool);
	DDX_Control(pDX, IDC_FLASH_PLAYER, m_player);
	DDX_Control(pDX, IDC_COMBO_URL, m_cmbURL);
	DDX_Text(pDX, IDC_COMBO_URL, m_strURL);
	DDX_Text(pDX, IDC_COMBO_TARGET, m_strTarget);

	CResizableToolTipDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInsertText, CResizableToolTipDialog)
	ON_BN_CLICKED(IDC_BTN_BOLD, OnBnClickedBtnBold)
	ON_BN_CLICKED(IDC_BTN_ITALIC, OnBnClickedBtnItalic)
	ON_CBN_SELCHANGE(IDC_COMBO_FONT, OnCbnSelchangeComboFont)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_TEXT, OnTextModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SIZE, OnSizeModified)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_SPACE, OnSpaceModified)
	ON_BN_CLICKED(IDC_BTN_CONFIG, OnBnClickedBtnConfig)
	ON_CONTROL(BN_COLORCHANGE, IDC_BTN_COLOR, OnColorChanged)
	ON_CONTROL(HN_SEL_CHANGED, IDC_DROP_EFFECT, OnEffectSelChanged)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_STATIC_TEXT_TOOL, OnBnClickedTextTool)
END_MESSAGE_MAP()


// CDlgInsertText message handlers

BOOL CDlgInsertText::OnInitDialog()
{
	CResizableToolTipDialog::OnInitDialog();

	m_cmbFont.SetPreviewStyle(CFontPreviewCombo::NAME_ONLY, true);
	m_edtSize.SetRange(MIN_TEXT_SIZE, MAX_TEXT_SIZE, DEFAULT_TEXT_SIZE, FALSE, TRUE);
	m_ssbSize.SetRange(MIN_TEXT_SIZE, MAX_TEXT_SIZE);
	m_ssbSize.SetBuddyWindow(m_edtSize.GetSafeHwnd());	
	m_btnBold.LoadBitmap(IDB_BOLD);
	m_btnItalic.LoadBitmap(IDB_ITALIC);
	m_edtSpace.SetRange(MIN_TEXT_SPACE, MAX_TEXT_SPACE, DEFAULT_TEXT_SPACE, FALSE, TRUE);
	m_ssbSpace.SetRange(MIN_TEXT_SPACE, MAX_TEXT_SPACE);
	m_ssbSpace.SetBuddyWindow(m_edtSpace.GetSafeHwnd());
	FillEffectCombo();
	m_btnConfig.LoadBitmap(IDB_BUTTON_EFFECT_CONFIG, 1);
	m_player.ModifyStyleEx(0, WS_EX_STATICEDGE);
	FillComboBox((CComboBox *)GetDlgItem(IDC_COMBO_TARGET), IDS_LIST_TARGET);
	m_cmbURL.Init("getURL");
	m_cmbURL.Load();
	m_sttTextTool.SetHoverCursor(AfxGetApp()->LoadCursor(IDC_MY_HAND));

	AddToolTip(IDC_EDIT_TEXT, IDS_TIP_INPUT_TEXT_HERE);
	AddToolTip(IDC_COMBO_FONT, IDS_TIP_TEXT_FONT);
	AddToolTip(IDC_EDIT_SIZE, IDS_TIP_TEXT_FONT_SIZE);
	AddToolTip(IDC_EDIT_SPACE, IDS_TIP_TEXT_CHAR_SPACING);
	AddToolTip(IDC_BTN_COLOR);
	AddToolTip(IDC_BTN_BOLD);
	AddToolTip(IDC_BTN_ITALIC);
	AddToolTip(IDC_COMBO_URL, IDS_TIP_URL);
	AddToolTip(IDC_COMBO_TARGET, IDS_TIP_TARGET);
	AddToolTip(IDC_DROP_EFFECT, IDS_TIP_SELECT_EFFECT);
	AddToolTip(IDC_BTN_CONFIG, IDS_TIP_CONFIG_EFFECT);

	AddAnchor(IDC_BTN_COLOR, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_EDIT_TEXT, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_COMBO_FONT, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_LABEL_SIZE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_EDIT_SIZE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_SIZE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_BTN_BOLD, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_BTN_ITALIC, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_LABEL_SPACE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_EDIT_SPACE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_SPACE, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_DROP_EFFECT, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_BTN_CONFIG, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_LABEL_URL, TOP_LEFT, TOP_LEFT);
	AddAnchor(IDC_COMBO_URL, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_LABEL_TARGET, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_COMBO_TARGET, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_PREVIEW, TOP_LEFT, BOTTOM_RIGHT);	
	AddAnchor(IDC_STATIC_TEXT_TOOL, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(IDOK, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT, BOTTOM_RIGHT);

	EnableSaveRestore(REG_KEY);

	Reposition(); // flash player activex can't math to CResizableToolTipDialog correctly

	LoadState();

	UpdateControls();
	
	m_edtText.SetSel(0, -1);

	m_init = true;

	Preview();	// draw canvas frame	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInsertText::Reposition()
{
	if (GetDlgItem(IDC_STATIC_PREVIEW) != NULL)
	{
		CRect rect;
		GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_player.MoveWindow(rect);
	}	
}

void CDlgInsertText::UpdateControls()
{
	SetDlgItemText(IDC_EDIT_TEXT, (LPCTSTR)m_text);
	SetDlgItemInt(IDC_EDIT_SIZE, m_size);
	SetDlgItemInt(IDC_EDIT_SPACE, m_space);
	m_btnBold.SetCheck(m_bold ? BST_CHECKED : BST_UNCHECKED);
	m_btnItalic.SetCheck(m_italic ? BST_CHECKED : BST_UNCHECKED);
	int item = m_cmbFont.FindString(-1, m_face);
	if (item == -1)
	{
		m_cmbFont.GetLBText(m_cmbFont.GetCurSel(), m_face);
	}
	else
	{
		m_cmbFont.SetCurSel(item);
	}
	m_btnColor.SetColor(m_color, (BYTE)(m_color >> 24));
	if (m_dropEffect.GetCurSel() < 1)
	{
		m_btnConfig.EnableWindow(FALSE);
	}
	else
	{
		m_btnConfig.EnableWindow(TRUE);
	}
}

void CDlgInsertText::OnTextChanged()
{
	UpdateControls();
	
	Preview();
}

void CDlgInsertText::OnBnClickedBtnBold()
{
	m_bold = !m_bold;
	OnTextChanged();
}

void CDlgInsertText::OnBnClickedBtnItalic()
{
	m_italic = !m_italic;
	OnTextChanged();
}

void CDlgInsertText::OnTextModified()
{
	GetDlgItemText(IDC_EDIT_TEXT, m_text);
	OnTextChanged();
}

void CDlgInsertText::Preview()
{
	if (!m_init)
	{
		return;
	}

	gldEffectInfo *pEffect = NULL;
	int cur_sel = m_dropEffect.GetCurSel();
	if (cur_sel > -1)
	{
		pEffect = (gldEffectInfo *)m_dropEffect.GetItemData(cur_sel);		
	}

	CWaitCursor xWait;

	m_preview.SetMovieClip(m_text, CTextFormat(m_face, m_size * 20, m_color, m_bold, m_italic, m_space * 20, CTextFormat::CL_NONE, "", ""), m_options);

	if (pEffect != NULL)
	{
		HRESULT hr;	

		CComPtr<IGLD_Effect> pIEffect;
		hr = pIEffect.CoCreateInstance(pEffect->m_clsid);
		if (FAILED(hr)) return;
		CComPtr<IGLD_Parameters> pIExtraParas;
		hr = pIExtraParas.CoCreateInstance(__uuidof(GLD_Parameters));
		if (FAILED(hr)) return;

		PutParameter(pIExtraParas, "MovieClip.Width", _GetMainMovie2()->m_width);
		PutParameter(pIExtraParas, "MovieClip.Height", _GetMainMovie2()->m_height);

		m_preview.ApplyEffect(pIEffect, m_options, pIExtraParas);		
	}

	// delete old one
	if (::PathFileExists(m_strPreviewFile))
		::DeleteFile(m_strPreviewFile);

	COLORREF color = my_app.GetWorkspaceBkColor();
	m_strPreviewFile = GetTempFile(NULL, "~eff", "swf");
	if (m_preview.CreatePreview(m_strPreviewFile, _GetMainMovie2()->m_color))
	{
		m_player.Zoom(0);
		m_player.SetBackgroundColor(RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
		m_player.SetMovie("1");
		m_player.SetMovie(m_strPreviewFile);
	}
	else
	{
		m_player.SetMovie("1");	// that's some error
		AfxMessageBox(IDS_FAILED_CREATE_PREVIEW);
	}
}

void CDlgInsertText::LoadState()
{
	// text style
	m_color = AfxGetApp()->GetProfileInt(REG_KEY, "Color", m_color);
	m_face = AfxGetApp()->GetProfileString(REG_KEY, "Face", m_face);
	m_size = AfxGetApp()->GetProfileInt(REG_KEY, "Size", DEFAULT_TEXT_SIZE);
	if (m_size > MAX_TEXT_SIZE) m_size = MAX_TEXT_SIZE;
	else if (m_size < MIN_TEXT_SIZE) m_size = MIN_TEXT_SIZE;
	m_bold = AfxGetApp()->GetProfileInt(REG_KEY, "Bold", m_bold) != 0;
	m_italic = AfxGetApp()->GetProfileInt(REG_KEY, "Italic", m_italic) != 0;
	m_space = AfxGetApp()->GetProfileInt(REG_KEY, "Space", DEFAULT_TEXT_SPACE);
	if (m_space > MAX_TEXT_SPACE) m_space = MAX_TEXT_SPACE;
	else if (m_space < MIN_TEXT_SPACE) m_space = MIN_TEXT_SPACE;

	//// common effect setting
	//BYTE *buf = NULL;
	//UINT size = 0;
	//AfxGetApp()->GetProfileBinary(REG_KEY, "Effect Common Setting", &buf, &size);
	//if (buf != NULL)
	//{
	//	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
	//	if (hMem != NULL)
	//	{
	//		LPVOID pData = GlobalLock(hMem);
	//		if (pData != NULL)
	//		{
	//			memcpy(pData, buf, size);
	//			GlobalUnlock(hMem);				
	//			CComPtr<IStream> pIStm;
	//			HRESULT hr = CreateStreamOnHGlobal(hMem, TRUE, &pIStm);
	//			if (SUCCEEDED(hr))
	//			{
	//				IStreamWrapper stm(pIStm);
	//				stm.Read(m_options.p, NULL);	
	//			}
	//		}
	//	}
	//	delete[] buf;
	//}

	//VARIANT_BOOL custom = VARIANT_FALSE;
	//GetParameter(m_options, EP_CUSTOM_FILL, &custom);
	//if (custom)
	//{
	//	m_color &= 0xFFFFFF;
	//}
	//else
	//{
	//	m_color |= 0xFF000000;
	//}
}

void CDlgInsertText::SaveState()
{
	// text style
	AfxGetApp()->WriteProfileInt(REG_KEY, "Color", m_color);
	AfxGetApp()->WriteProfileString(REG_KEY, "Face", m_face);
	AfxGetApp()->WriteProfileInt(REG_KEY, "Size", m_size);
	AfxGetApp()->WriteProfileInt(REG_KEY, "Bold", m_bold);
	AfxGetApp()->WriteProfileInt(REG_KEY, "Italic", m_italic);
	AfxGetApp()->WriteProfileInt(REG_KEY, "Space", m_space);
	//// remove specify effect config parameters
	//RemoveParameters(m_options, PT_CUSTOM);
	//CComPtr<IStream> pIStm;
	//HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pIStm);
	//if (SUCCEEDED(hr))
	//{
	//	IStreamWrapper stm(pIStm);
	//	hr = stm.Write(m_options.p, NULL);
	//	if (SUCCEEDED(hr))
	//	{
	//		HGLOBAL hMem = NULL;
	//		GetHGlobalFromStream(pIStm, &hMem);
	//		if (hMem != NULL)
	//		{
	//			SIZE_T size = GlobalSize(hMem);
	//			if (size != 0)
	//			{
	//				LPVOID pData = GlobalLock(hMem);
	//				if (pData != NULL)
	//				{
	//					AfxGetApp()->WriteProfileBinary(REG_KEY, "Effect Common Setting", (LPBYTE)pData, (UINT)size);
	//					GlobalUnlock(hMem);
	//				}
	//			}
	//		}
	//	}
	//}
}

void CDlgInsertText::OnSizeModified()
{
	m_size = GetDlgItemInt(IDC_EDIT_SIZE);
	OnTextChanged();
}

void CDlgInsertText::OnColorChanged()
{
	m_color = m_btnColor.GetColor() | 0xFF000000;
	PutParameter(m_options, EP_CUSTOM_FILL, VARIANT_FALSE);
	OnTextChanged();
}

void CDlgInsertText::OnSpaceModified()
{
	m_space = GetDlgItemInt(IDC_EDIT_SPACE);
	OnTextChanged();
}

void CDlgInsertText::OnCbnSelchangeComboFont()
{
	m_cmbFont.GetLBText(m_cmbFont.GetCurSel(), m_face);
	OnTextChanged();
}

void CDlgInsertText::OnEffectSelChanged()
{
	OnTextChanged();
}

void CDlgInsertText::OnBnClickedBtnConfig()
{
	int cur_sel = m_dropEffect.GetCurSel();
	if (cur_sel > -1)
	{
		gldEffectInfo *pInfo = (gldEffectInfo *)m_dropEffect.GetItemData(cur_sel);
		if (pInfo != NULL)
		{
			if (pInfo->BrowseForProperties(m_hWnd
				, CW_USEDEFAULT, CW_USEDEFAULT, "Options", 1, &CLSID_CommonPage, -1, m_options) == IDOK)
			{
				VARIANT_BOOL custom = VARIANT_FALSE;
				GetParameter(m_options, EP_CUSTOM_FILL, &custom);
				if (custom)
				{
					m_color &= 0xFFFFFF;
				}
				else
				{
					m_color |= 0xFF000000;
				}
				OnTextChanged();
			}
		}
		else
		{			
			CGLD_PropertySheet dlg(this, CW_USEDEFAULT, CW_USEDEFAULT, "Options", m_options, 1, (CLSID *)&CLSID_CommonPage);
			dlg.m_nActiveInitial = 0;
			if (dlg.DoModal() == IDOK)
			{
				VARIANT_BOOL custom = VARIANT_FALSE;
				GetParameter(m_options, EP_CUSTOM_FILL, &custom);
				if (custom)
				{
					m_color &= 0xFFFFFF;
				}
				else
				{
					m_color |= 0xFF000000;
				}
				OnTextChanged();
			}
		}
	}
}

void CDlgInsertText::OnSize(UINT nType, int cx, int cy)
{
	if (m_hWnd != NULL)
		Reposition();

	CResizableToolTipDialog::OnSize(nType, cx, cy);
}

void CDlgInsertText::OnOK()
{	
	UpdateData(TRUE);

	if (m_text.GetLength() == 0)
	{
		GetDlgItem(IDC_EDIT_TEXT)->SetFocus();
		AfxMessageBox(IDS_NEED_TEXT);
		return;
	}

	gldEffect* pEffect = NULL;
	int cur_sel = m_dropEffect.GetCurSel();
	if (cur_sel > -1 && m_dropEffect.GetItemData(cur_sel) != NULL)
	{
		gldEffectInfo *pInfo = (gldEffectInfo *)m_dropEffect.GetItemData(cur_sel);
		pEffect = new gldEffect(pInfo->m_clsid);
		if (pEffect->m_pIEffect == NULL || pEffect->m_pIParas == NULL)
		{
			delete pEffect;

			BSTR bstrProgID;
			if (::ProgIDFromCLSID(pInfo->m_clsid, &bstrProgID))
			{
				CString strProgID(bstrProgID);
				::CoTaskMemFree(bstrProgID);
				AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_FAILED_CREATE_EFFECT_1, (LPCTSTR)strProgID);
			}
			return;
		}
	}
	if (m_strURL.GetLength() > 0)
	{
		m_cmbURL.AddCurrentText(TRUE);
		m_cmbURL.Save();
	}
	CCmdInsertTextEffect *pCmd = new CCmdInsertTextEffect(m_text
		,  CTextFormat(m_face, m_size * 20, m_color, m_bold, m_italic, m_space * 20
		, CTextFormat::CL_NONE, "", ""), pEffect, m_options, m_strURL, m_strTarget);
	my_app.Commands().Do(pCmd);

	return CResizableToolTipDialog::OnOK();
}

void CDlgInsertText::OnDestroy()
{
	SaveState();

	CResizableToolTipDialog::OnDestroy();
}

void CDlgInsertText::OnBnClickedTextTool()
{
	EndDialog(ID_USE_TEXT_TOOL);
}
