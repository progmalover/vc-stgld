// DlgSelectEffect.cpp : implementation file
//

#include "stdafx.h"
#include "TextToolEx.h"
#include "Glanda.h"
#include ".\dlgselecteffect.h"
#include "gldEffectManager.h"
#include "gldEffect.h"
#include "CmdInsertEffect.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "EffectCommonUtils.h"
#include "EffectCommonParameters.h"
#include "GLD_PropertySheet.h"
#include "Global.h"
#include "DeferWindowPos.h"
#include "TextStyleManager.h"
#include "ComUtils.h"
#include "Tips.h"
#include "CmdGroup.h"
#include "GlandaClipboard.h"
#include "filepath.h"
#include "my_app.h"

#define EP_EFFECT_PREVIEW_TEXT		"Effect.Preview.Text"
#define EP_EFFECT_PREVIEW_HAS_SEL	"Effect.Preview.HasSel"
#define EP_EFFECT_PREVIEW_USE_SEL	"Effect.Preview.UseSel"
#define PREVIEW_DELAY_TIME 500
#define DEFAULT_COLOR_MASK RGB(255, 0, 255)
#define DEFAULT_PREVIEW_TEXT "Sothink Glanda"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IDC_BUTTON_STYLE1	1000

LPCTSTR CDlgSelectEffect::REG_KEY = "Dialogs\\Select Effect";

//IMPLEMENT_DYNCREATE(CDlgSelectEffect, CResizableDialog)
LPCTSTR CDlgSelectEffect::DEFINED_PARAS[] = {
	EP_EFFECT_PREVIEW_TEXT,
	EP_EFFECT_PREVIEW_HAS_SEL,
	EP_EFFECT_PREVIEW_USE_SEL
};

struct _MyDlgPos
{
	RECT rcWnd;
	int vx;
};

CDlgSelectEffect::CDlgSelectEffect(gldEffectInfo *pInfo /*=NULL*/, IGLD_Parameters *pIParas /*= NULL*/, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgSelectEffect::IDD, pParent)
{
	m_PreviewTimer = 0;
	m_Init = FALSE;
	m_hLastItem = NULL;
	m_pSelEffectInfo = pInfo;
	if (pIParas != NULL)
	{
		m_Options.CoCreateInstance(__uuidof(GLD_Parameters));
		if (m_Options != NULL)
			CopyParameters(pIParas, m_Options);
	}
}

CDlgSelectEffect::~CDlgSelectEffect()
{
	if (::PathFileExists(m_strPreviewFile))
		::DeleteFile(m_strPreviewFile);
}

void CDlgSelectEffect::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TREE_EFFECTS, m_treEffects);
	DDX_Control(pDX, IDC_PLAYER, m_Player);
	DDX_Control(pDX, IDC_BUTTON_BACKGROUND, m_btnBackground);
	DDX_Control(pDX, IDC_BUTTON_TEXT_COLOR, m_btnTextColor);

	CResizableDialog::DoDataExchange(pDX);
}

BOOL CloneBitmap(CBitmap &src, CBitmap &dst)
{
	BITMAP bi;
	memset(&bi, 0, sizeof(bi));
	if (src.GetBitmap(&bi) == 0)
		return FALSE;
	DWORD bytesCount = bi.bmHeight * bi.bmWidthBytes;
	CAutoPtr<BYTE> pBuf(new BYTE[bytesCount]);
	VERIFY(src.GetBitmapBits(bytesCount, pBuf.m_p) == bytesCount);
	bi.bmBits = pBuf.m_p;
	if (!dst.CreateBitmapIndirect(&bi))
		return FALSE;
	return TRUE;
}

BOOL CDlgSelectEffect::FillEffectTree()
{
	m_treEffects.SetImageList(gldEffectManager::Instance()->GetImageList(), TVSIL_NORMAL);

	CString strEntr;
	strEntr.LoadString(IDS_EFFECT_ENTRANCE);
	CString strEmha;
	strEmha.LoadString(IDS_EFFECT_EMPHASIS);
	CString strExit;
	strExit.LoadString(IDS_EFFECT_EXIT);
	HTREEITEM hItemEntrance = m_treEffects.InsertItem(strEntr, gldEffectManager::ICON_CAT_ENTRANCE, gldEffectManager::ICON_CAT_ENTRANCE);
	HTREEITEM hItemEmphasis = m_treEffects.InsertItem(strEmha, gldEffectManager::ICON_CAT_EMPHASIS, gldEffectManager::ICON_CAT_EMPHASIS);
	HTREEITEM hItemExit = m_treEffects.InsertItem(strExit, gldEffectManager::ICON_CAT_EXIT, gldEffectManager::ICON_CAT_EXIT);

	m_treEffects.SetItemData(hItemEntrance, 0);
	m_treEffects.SetItemData(hItemEmphasis, 0);
	m_treEffects.SetItemData(hItemExit, 0);
	
	GEFFECTINFO_LIST &infoList = gldEffectManager::Instance()->GetEffectInfoList();
	for (GEFFECTINFO_LIST::iterator it = infoList.begin(); it != infoList.end(); it++)
	{
		gldEffectInfo *pEffectInfo = *it;
		if (pEffectInfo->m_type == gldEffectInfo::typeMotionPath1 || 
			pEffectInfo->m_type == gldEffectInfo::typeMotionPath2) // skip motion path effect
			continue;		

		if (!pEffectInfo->m_name.empty())
		{
			HTREEITEM hParent = TVI_ROOT;
			switch (pEffectInfo->m_type)
			{
			case gldEffectInfo::typeEnter:
				hParent = hItemEntrance;
				break;
			case gldEffectInfo::typeExit:
				hParent = hItemExit;
				break;
			case gldEffectInfo::typeEmphasize:
				hParent = hItemEmphasis;
				break;
			case gldEffectInfo::typeMotionPath1:
			case gldEffectInfo::typeMotionPath2:
				ASSERT(FALSE);
			}			
			HTREEITEM hItem = m_treEffects.InsertItem(pEffectInfo->m_name.c_str()
				, pEffectInfo->m_iconIndex
				, pEffectInfo->m_iconIndex, hParent);
			ASSERT(hItem != NULL);
			m_treEffects.SetItemData(hItem, (DWORD_PTR)pEffectInfo);			
		}
	}

	return TRUE;
}

BOOL CDlgSelectEffect::OnInitDialog()
{
	if (!CResizableDialog::OnInitDialog())
		return FALSE;

	
	if (!FillEffectTree())
		return FALSE;

	BOOL canApply = (_GetCurInstance() != NULL);
	CButton *pBtnApply = (CButton *)GetDlgItem(IDC_BUTTON_APPLY);
	ASSERT_VALID(pBtnApply);
	pBtnApply->EnableWindow(canApply);
	if (!canApply)
	{
		// show why can't apply tips
	}

	// read options from registry
	// preview synchronous
	int nSync = AfxGetApp()->GetProfileInt(REG_KEY, "Preview", 1);
	CButton *pBtnPreviewSync = (CButton *)GetDlgItem(IDC_CHECK_PREVIEW_SYNC);
	ASSERT_VALID(pBtnPreviewSync);
	pBtnPreviewSync->SetCheck(nSync == 0 ? BST_UNCHECKED : BST_CHECKED);	

	m_btnBackground.SetColor(_GetMainMovie2()->m_color, 255);
	
	// options
	HRESULT hr;

	if (m_Options == NULL)
	{	
		hr = m_Options.CoCreateInstance(__uuidof(GLD_Parameters));
		if (FAILED(hr))
			return FALSE;

		BYTE *buf = NULL;
		UINT size = 0;
		AfxGetApp()->GetProfileBinary(REG_KEY, "Effect Preview", &buf, &size);
		if (buf != NULL)
		{
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
			if (hMem != NULL)
			{
				LPVOID pData = GlobalLock(hMem);
				if (pData != NULL)
				{
					memcpy(pData, buf, size);
					GlobalUnlock(hMem);				
					CComPtr<IStream> pIStm;
					hr = CreateStreamOnHGlobal(hMem, TRUE, &pIStm);
					if (SUCCEEDED(hr))
					{
						IStreamWrapper stm(pIStm);
						stm.Read(m_Options.p, NULL);					
					}
				}
			}
			delete[] buf;
		}
	}
	if (_GetCurInstance() != NULL)	// retrieve instance's common settting	
		CopyParameters(_GetCurInstance()->m_pIParas, m_Options);	
	
	// create base preview movie
	CreatePreviewMovie();

	// set last selected item, this step must place after all options set completed	
	CString curSel;
	OLECHAR *pwstrCLSID = NULL;		
	if (m_pSelEffectInfo != NULL)	
		StringFromCLSID(m_pSelEffectInfo->m_clsid, &pwstrCLSID);
	
	if (pwstrCLSID == NULL)
	{
		curSel = AfxGetApp()->GetProfileString(REG_KEY, "Effect Preview Selected");
	}
	else
	{
		curSel = pwstrCLSID;
		CoTaskMemFree(pwstrCLSID);
	}

	if (curSel.GetLength() > 0)
	{
		USES_CONVERSION;
		CLSID clsid;	
		hr = CLSIDFromString(A2W(curSel), &clsid);
		if (SUCCEEDED(hr))
		{
			HTREEITEM hItem = m_treEffects.GetRootItem();
			while (hItem != NULL)
			{				
				gldEffectInfo *pInfo = (gldEffectInfo *)m_treEffects.GetItemData(hItem);
				if (pInfo != NULL)
				{
					if (IsEqualCLSID(clsid, pInfo->m_clsid))
						break;
				}
				HTREEITEM hNext = m_treEffects.GetChildItem(hItem);
				if (hNext == NULL)
				{
					hNext = m_treEffects.GetNextSiblingItem(hItem);
					if (hNext == NULL)
					{
						HTREEITEM hParent = hItem;
						while (((hParent = m_treEffects.GetParentItem(hParent)) != NULL) && (hNext == NULL))
						{
							hNext = m_treEffects.GetNextSiblingItem(hParent);							
						}
					}
				}
				hItem = hNext;
			}
			m_treEffects.SelectItem(hItem);
		}
	}


	AddAnchor(IDC_TREE_EFFECTS, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_STATIC_PLAYER, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_OPTION, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_APPLY, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_CLOSE, BOTTOM_RIGHT, BOTTOM_RIGHT);

	EnableSaveRestore(REG_KEY);

	m_Player.ModifyStyleEx(0, WS_EX_STATICEDGE);


	CRect rcButton;
	m_btnTextColor.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	rcButton.OffsetRect(rcButton.Width() + 10, 0);

	CTextStyleData ts;
	for (int i = 0; i < 6; i++)
	{
		m_btnTextStyle[i].Create("", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rcButton, this, IDC_BUTTON_STYLE1 + i);
		m_btnTextStyle[i].ModifyFBStyle(FBS_FLAT, 0);

		rcButton.OffsetRect(rcButton.Width(), 0);
	}

	LoadTextStyleButtons();

	Reposition();

	m_Init = TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlgSelectEffect::LoadTextStyleButtons()
{
	CTextStyleData ts;
	for (int i = 0; i < 6; i++)
	{
		CTextStyleManager::LoadStyle(ts, i);
		m_btnTextStyle[i].SetTextStyle(&ts);
	}
}

BEGIN_MESSAGE_MAP(CDlgSelectEffect, CResizableDialog)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_EFFECTS, OnTvnSelchangingTreeEffects)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_CHECK_PREVIEW_SYNC, OnBnClickedCheckPreviewSync)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnBnClickedButtonOption)
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()

	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_BACKGROUND, OnBackgroundColorChanged)
	ON_CONTROL(BN_COLORCHANGE, IDC_BUTTON_TEXT_COLOR, OnTextColorChanged)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_STYLE1, IDC_BUTTON_STYLE1 + 5, OnBnClickedTextStyle)
END_MESSAGE_MAP()



void CDlgSelectEffect::OnTvnSelchangingTreeEffects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	// enable / disable [Apply] button
	BOOL canApply = (_GetCurInstance() != NULL);
	if (canApply)
	{
		canApply = (m_treEffects.GetItemData(pNMTreeView->itemNew.hItem) != NULL);
	}
	CButton *pBtnApply = (CButton *)GetDlgItem(IDC_BUTTON_APPLY);
	ASSERT_VALID(pBtnApply);
	pBtnApply->EnableWindow(canApply);

	if (m_PreviewTimer != 0)
	{
		KillTimer(m_PreviewTimer);
		m_PreviewTimer = 0;
	}

	if (IsPreviewSync())
	{	
		m_PreviewTimer = SetTimer(1, PREVIEW_DELAY_TIME, NULL);
	}
	else
	{
		m_Player.SetMovie("1");
	}

	// remove previous effect parameters
	if (m_hLastItem != NULL 
		&& pNMTreeView->itemNew.hItem != m_hLastItem
		&& m_treEffects.GetItemData(pNMTreeView->itemNew.hItem) != NULL)
		RemoveParameters(m_Options, PT_CUSTOM, DEFINED_PARAS, sizeof(DEFINED_PARAS) / sizeof(LPCTSTR));

	*pResult = 0;
}

void CDlgSelectEffect::Refresh(BOOL bForce)
{
	HTREEITEM hItem = m_treEffects.GetSelectedItem();
	if (hItem != NULL && (bForce || hItem != m_hLastItem))
	{			
		gldEffectInfo *pEffect = (gldEffectInfo *)m_treEffects.GetItemData(hItem);
		if (pEffect != NULL)
		{
			m_hLastItem = hItem;
			Preview(pEffect);
		}
	}
}

void CDlgSelectEffect::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	ASSERT(m_PreviewTimer != 0);
	KillTimer(m_PreviewTimer);
	m_PreviewTimer = 0;

	if (IsPreviewSync())
		Refresh(FALSE);

	CResizableDialog::OnTimer(nIDEvent);
}

void CDlgSelectEffect::OnDestroy()
{
	if (m_PreviewTimer != 0)
	{
		KillTimer(m_PreviewTimer);
		m_PreviewTimer = 0;
	}

	CResizableDialog::OnDestroy();	
}

void CDlgSelectEffect::CreatePreviewMovieFromText()
{
	ASSERT(FALSE);

	BSTR bstrText = NULL;
	GetParameter(m_Options, EP_EFFECT_PREVIEW_TEXT, &bstrText);
	CString text;
	if (bstrText == NULL)
	{
		text = DEFAULT_PREVIEW_TEXT;
	}
	else
	{
		if (SysStringLen(bstrText) == 0)
			text = DEFAULT_PREVIEW_TEXT;
		else
		{
			text = bstrText;
			text.Trim(_T(' '));
			if (text.GetLength() == 0)
				text = DEFAULT_PREVIEW_TEXT;
		}
		SysFreeString(bstrText);
	}

	U32 color = ~(_GetMainMovie2()->m_color);
	CTextFormat txtFmt("Times New Roman", 800, color,
		FALSE, FALSE, 0, CTextFormat::CL_NONE, "", "");

	m_Preview.SetMovieClip(text, txtFmt, m_Options);
}

void CDlgSelectEffect::CreatePreviewMovieFromSel()
{	
	gldInstance *pInst = _GetCurInstance();
	ASSERT(pInst != NULL);

	m_Preview.SetMovieClip(pInst, m_Options);
}

void CDlgSelectEffect::CreatePreviewMovie()
{
	if (_GetCurInstance() != NULL)
	{
		BOOL bUseSel = TRUE;
		GetParameter(m_Options, EP_EFFECT_PREVIEW_USE_SEL, &bUseSel);
		if (bUseSel)
			return CreatePreviewMovieFromSel();
	}
	return CreatePreviewMovieFromText();
}

void CDlgSelectEffect::Preview(gldEffectInfo *pEffect)
{
	ASSERT(pEffect != NULL);
	
	CWaitCursor xWait;

	HRESULT hr;	
	
	CComPtr<IGLD_Effect> pIEffect;
	hr = pIEffect.CoCreateInstance(pEffect->m_clsid);
	if (FAILED(hr)) return;
	CComPtr<IGLD_Parameters> pIExtraParas;
	hr = pIExtraParas.CoCreateInstance(__uuidof(GLD_Parameters));
	if (FAILED(hr)) return;

	PutParameter(pIExtraParas, "MovieClip.Width", _GetMainMovie2()->m_width);
	PutParameter(pIExtraParas, "MovieClip.Height", _GetMainMovie2()->m_height);
	
	BOOL bRet = m_Preview.ApplyEffect(pIEffect, m_Options, pIExtraParas);
	if (!bRet) return;	

	// delete old one
	if (::PathFileExists(m_strPreviewFile))
		::DeleteFile(m_strPreviewFile);

	COLORREF color = my_app.GetWorkspaceBkColor();
	m_strPreviewFile = GetTempFile(NULL, "~eff", "swf");
	if (m_Preview.CreatePreview(m_strPreviewFile, m_btnBackground.GetColor()))
	{
		m_Player.Zoom(0);
		m_Player.SetBackgroundColor(RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
		m_Player.SetMovie("1");
		m_Player.SetMovie(m_strPreviewFile);
	}
	else
	{
		m_Player.SetMovie("1");	// that's some error
		AfxMessageBox(IDS_FAILED_CREATE_PREVIEW);
	}
}

void CDlgSelectEffect::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	CResizableDialog::OnCancel();
}

BOOL CDlgSelectEffect::IsPreviewSync()
{
	CButton *pWnd = (CButton *)GetDlgItem(IDC_CHECK_PREVIEW_SYNC);
	ASSERT_VALID(pWnd);
	int nCheck = pWnd->GetCheck();
	return (nCheck == BST_CHECKED);	
}

void CDlgSelectEffect::OnBnClickedCheckPreviewSync()
{
	// TODO: Add your control notification handler code here
	if (IsPreviewSync())
	{
		if (m_PreviewTimer == 0)
		{
			m_PreviewTimer = SetTimer(1, 0, NULL);
		}
	}
	else
	{
		if (m_PreviewTimer != 0)
		{
			KillTimer(m_PreviewTimer);
			m_PreviewTimer = 0;
		}
	}
}

void CDlgSelectEffect::OnBnClickedButtonApply()
{
	HTREEITEM hItem = m_treEffects.GetSelectedItem();
	if (hItem != NULL)	
		m_pSelEffectInfo = (gldEffectInfo *)m_treEffects.GetItemData(hItem);

	if (m_PreviewTimer != 0)
	{
		KillTimer(m_PreviewTimer);
		m_PreviewTimer = 0;
	}
	
	// write options to registry	
	AfxGetApp()->WriteProfileInt(REG_KEY, "Preview", IsPreviewSync() ? 1 : 0);

	CComPtr<IStream> pIStm;
	HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pIStm);
	if (SUCCEEDED(hr))
	{
		IStreamWrapper stm(pIStm);
		hr = stm.Write(m_Options.p, NULL);
		if (SUCCEEDED(hr))
		{
			HGLOBAL hMem = NULL;
			GetHGlobalFromStream(pIStm, &hMem);
			if (hMem != NULL)
			{
				SIZE_T size = GlobalSize(hMem);
				if (size != 0)
				{
					LPVOID pData = GlobalLock(hMem);
					if (pData != NULL)
					{
						AfxGetApp()->WriteProfileBinary(REG_KEY, "Effect Preview", (LPBYTE)pData, (UINT)size);
						GlobalUnlock(hMem);
					}
				}
			}
		}
	}

	CString curSel;	
	if (m_pSelEffectInfo != NULL)
	{
		OLECHAR *wstrCLSID = NULL;
		StringFromCLSID(m_pSelEffectInfo->m_clsid, &wstrCLSID);
		if (wstrCLSID != NULL)
		{
			curSel = wstrCLSID;
			CoTaskMemFree(wstrCLSID);
		}
	}	
	AfxGetApp()->WriteProfileString(REG_KEY, "Effect Preview Selected", curSel);

	RemoveParameters(m_Options, PT_DEFINED, DEFINED_PARAS, sizeof(DEFINED_PARAS) / sizeof(LPCTSTR));

	CResizableDialog::OnOK();
}


void CDlgSelectEffect::OnBnClickedButtonOption()
{
	gldEffectInfo *pEffectInfo = NULL;

	HTREEITEM hItem = m_treEffects.GetSelectedItem();
	if (hItem != NULL)
	{
		pEffectInfo = (gldEffectInfo *)m_treEffects.GetItemData(hItem);		
	}
	if (pEffectInfo == NULL)
	{
		return;
	}
	
	BSTR bstrText = NULL;
	GetParameter(m_Options, EP_EFFECT_PREVIEW_TEXT, &bstrText);
	if (bstrText == NULL)
	{
		PutParameter(m_Options, EP_EFFECT_PREVIEW_TEXT, CComBSTR(DEFAULT_PREVIEW_TEXT));
	}
	else
	{
		if (SysStringLen(bstrText) == 0) // can't preview empty text
			PutParameter(m_Options, EP_EFFECT_PREVIEW_TEXT, CComBSTR(DEFAULT_PREVIEW_TEXT));

		SysFreeString(bstrText);
	}
	PutParameter(m_Options, EP_EFFECT_PREVIEW_HAS_SEL, (BOOL)(_GetCurInstance() != NULL));
	
	CString strCaption;
	strCaption.LoadString(IDS_PREVIEW_OPTIONS);
	UINT ret = pEffectInfo->BrowseForProperties(m_hWnd, CW_USEDEFAULT, CW_USEDEFAULT
		, strCaption, 1, &CLSID_CommonPage, -1, m_Options);
	
	// refresh buttons because they may be changed.
	LoadTextStyleButtons();

	if (ret == IDOK)
	{
		CreatePreviewMovie();
		m_hLastItem = NULL;
		if (IsPreviewSync())
		{
			HTREEITEM hItem = m_treEffects.GetSelectedItem();
			if (hItem != NULL)
			{
				gldEffectInfo *pInfo = (gldEffectInfo *)m_treEffects.GetItemData(hItem);
				if (pInfo != NULL)
				{
					if (m_PreviewTimer != 0)
					{
						KillTimer(m_PreviewTimer);
						m_PreviewTimer = 0;
					}
					m_PreviewTimer = SetTimer(1, 0, NULL);
				}
			}
		}
	}
}

void CDlgSelectEffect::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_Init)
		Reposition();
}

void CDlgSelectEffect::Reposition()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_PLAYER)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_Player.MoveWindow(&rc);
}

CRect CDlgSelectEffect::GetVertSplitBarRect()
{
	CRect rcWnd;
	CWnd *pWnd = GetDlgItem(IDC_TREE_EFFECTS);
	ASSERT_VALID(pWnd);
	pWnd->GetWindowRect(&rcWnd);
	ScreenToClient(rcWnd);
	rcWnd.left = rcWnd.right;
	rcWnd.right += 4;
	return rcWnd;
}

BOOL CDlgSelectEffect::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	//CPoint pt;
	//GetCursorPos(&pt);
	//ScreenToClient(&pt);
	//CRect rcSplit = GetVertSplitBarRect();	
	//if (rcSplit.PtInRect(pt))
	//{
	//	if (SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR)) != NULL)
	//		return TRUE;
	//}

	return CResizableDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CDlgSelectEffect::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//CRect rcSplit = GetVertSplitBarRect();
	//if (rcSplit.PtInRect(point))
	//{
	//	TrackVertSplit(point);
	//}
	CResizableDialog::OnLButtonDown(nFlags, point);
}

void CDlgSelectEffect::VertSplitTo(int x)
{
	CWnd *pWnd = GetDlgItem(IDC_TREE_EFFECTS);
	ASSERT_VALID(pWnd);
	CRect rcWnd;
	pWnd->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	rcWnd.right = x;
	pWnd->MoveWindow(rcWnd);

	pWnd = GetDlgItem(IDC_PLAYER);
	ASSERT_VALID(pWnd);
	pWnd->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	rcWnd.left = x + 4;
	pWnd->MoveWindow(rcWnd);

	UpdateWindow();
}

void CDlgSelectEffect::TrackVertSplit(CPoint point)
{
	ASSERT(m_hWnd != NULL);

	int xDrag = 0;//GetSystemMetrics(SM_CXDRAG);
	BOOL bShowContent = FALSE;
	//SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bShowContent, 0);
	CRect rcTack = GetVertSplitBarRect();
	int ox = rcTack.left; // origin x
	CRect rcClient;
	GetClientRect(&rcClient);
	int xmin = 95, xmax = rcClient.right - 115;	

	UpdateWindow();	
	SetCapture();
	if (GetCapture() != this)
	{
		return;
	}
	
	int x = point.x;
	int px = point.x;

	CDC *pDC = NULL;
	if (!bShowContent)
		pDC = GetDC();

	CBrush *pOldBrush = NULL;
	if (pDC != NULL)
	{
		CBrush *pBrush = CDC::GetHalftoneBrush();	
		if (pBrush != NULL)
			pBrush = pDC->SelectObject(pBrush);
	}

	BOOL dragging = false;
	MSG msg;	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (GetCapture() != this) 
		{
			if (dragging) // drag canceled
			{
				VertSplitTo(ox); // restore
			}
			break;
		}
		
		switch(msg.message)
		{
		case WM_LBUTTONUP:
			x = (UINT)(short)LOWORD(msg.lParam);
			x = __max(__min(x, xmax), xmin);
			if (dragging)
			{				
				// set window position
				VertSplitTo(x);
			}			
			goto ExitLoop;
		case WM_MOUSEMOVE:
			x = (UINT)(short)LOWORD(msg.lParam);
			x = __max(__min(x, xmax), xmin);
			// fixed: sometimes a message-box may pop up after WM_LBUTTONDOWN
			if ((GetKeyState(VK_LBUTTON) >> (sizeof(SHORT) - 1)) == 0) 
			{
				if (dragging)
				{
					VertSplitTo(ox); // restore
				}				
				goto ExitLoop;
			}			
			if (abs(x - px) > xDrag)
			{
				if (dragging)
				{
					if (!bShowContent)
					{
						ASSERT_VALID(pDC);
						rcTack.OffsetRect(px - rcTack.left, 0);
						pDC->PatBlt(rcTack.left, rcTack.top, rcTack.Width(), rcTack.Height(), PATINVERT);
						rcTack.OffsetRect(x - rcTack.left, 0);
						pDC->PatBlt(rcTack.left, rcTack.top, rcTack.Width(), rcTack.Height(), PATINVERT);						
					}
					else
					{
						VertSplitTo(x);
					}					
				}
				else
				{
					dragging = TRUE;					
					if (!bShowContent)
					{
						ASSERT_VALID(pDC);
						rcTack.OffsetRect(x - rcTack.left, 0);
						pDC->PatBlt(rcTack.left, rcTack.top, rcTack.Width(), rcTack.Height(), PATINVERT);
					}
					else
					{
						VertSplitTo(x);
					}
				}
				px = x;
			}
			break;
		case WM_KEYDOWN:
			if (msg.wParam == VK_ESCAPE)	// esc pressed
			{
				if (dragging)
					VertSplitTo(ox);
				goto ExitLoop;				
			}			
			break;
		default:	
			break;
		}		
	}

ExitLoop:
	ReleaseCapture();

	if (pDC != NULL)
	{
		if (pOldBrush != NULL)
			pDC->SelectObject(pOldBrush);
		ReleaseDC(pDC);
	}

	Invalidate();

	return;
}

void CDlgSelectEffect::OnBackgroundColorChanged()
{
	Refresh(TRUE);
}

void CDlgSelectEffect::OnTextColorChanged()
{
	COLORREF rgb = m_btnTextColor.GetColor();
	TColor color(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), 255);

	CAutoPtr<TFillStyle> tfs(new TSolidFillStyle(color));

	gldFillStyle gfs;
	CTransAdaptor::TFS2GFS(*tfs, gfs);

	CComPtr<IGLD_FillStyle> pIFillStyle;
	HRESULT hr = CC2I::Create(&gfs, &pIFillStyle);
	if (FAILED(hr))
	{
		::CoReportError(hr);
		return;
	}

	PutParameter(m_Options, EP_CUSTOM_FILL, VARIANT_TRUE);
	PutParameter(m_Options, EP_FILL_STYLE, pIFillStyle);

	CreatePreviewMovie();

	Refresh(TRUE);
}

void CDlgSelectEffect::OnBnClickedTextStyle(UINT nID)
{
	int i = nID - IDC_BUTTON_STYLE1;
	if (i >= 0 && i < 6)
	{
		CTextStyleData *pTextStyle = m_btnTextStyle[i].GetTextStyle();
		if (pTextStyle == NULL)
			return;

		// 处理BreakApart
		BOOL m_bBreakApart		= pTextStyle->m_bBreakApart;

		// 处理TextBorder
		BOOL m_bTextBorder		= pTextStyle->m_bTextBorder;
		float m_fBorderWidth		= pTextStyle->m_fBorderWidth;
		COLORREF m_clrBorderColor	= pTextStyle->m_clrBorderColor;

		// 处理Shadow
		BOOL m_bAddShadow		= pTextStyle->m_bAddShadow;
		float m_fShadowSize		= pTextStyle->m_fShadowSize;
		COLORREF m_clrShadowColor	= pTextStyle->m_clrShadowColor;

		// 处理填充方式
		BOOL m_bCustomFill		= pTextStyle->m_bCustomFill;
		int m_nAngle			= pTextStyle->m_nAngle;

		gldFillStyle *gfs = pTextStyle->GetFillStyle();
		CComPtr<IGLD_FillStyle> pIFillStyle;
		HRESULT hr = CC2I::Create(gfs, &pIFillStyle);
		if (FAILED(hr))
			return;

		PutParameter(m_Options, EP_BREAK_APART, m_bBreakApart);

		PutParameter(m_Options, EP_TEXT_BORDER, m_bTextBorder);
		PutParameter(m_Options, EP_BORDER_WIDTH, m_fBorderWidth);
		PutParameter(m_Options, EP_BORDER_COLOR, m_clrBorderColor);

		PutParameter(m_Options, EP_ADD_SHADOW, m_bAddShadow);
		PutParameter(m_Options, EP_SHADOW_SIZE, m_fShadowSize);
		PutParameter(m_Options, EP_SHADOW_COLOR, m_clrShadowColor);

		PutParameter(m_Options, EP_FILL_ANGLE, m_nAngle);
		PutParameter(m_Options, EP_FILL_STYLE, pIFillStyle);
		PutParameter(m_Options, EP_CUSTOM_FILL, m_bCustomFill);

		CreatePreviewMovie();

		Refresh(TRUE);
	}
}
