// DlgViewPane.cpp : implementation file
//

#include "stdafx.h"
#include "TextToolEx.h"
#include "Glanda.h"
#include "DlgEffectPanel.h"

#include "gldMovieClip.h"

#include "gld_selection.h"
#include "my_app.h"
#include "gldEffect.h"
#include "gldInstance.h"
#include "SWFProxy.h"
#include "my_app.h"
#include "CmdInsertEffect.h"
#include "EffectWnd.h"
#include "CmdMoveEffect.h"
#include <algorithm>
#include "GlandaDoc.h"

#include "gldEffectManager.h"
#include "DlgSelectEffect.h"
#include "CheckMessageBox.h"
#include "CmdRemoveInstance.h"
#include "../GlandaCommandRes/resource.h"
#include "EffectCommonUtils.h"
#include "toolsdef.h"

#include "Tips.h"
#include "CmdGroup.h"
#include "DesignWnd.h"
#include "EffectCommonParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ID_SELECT_EFFECT	10000	// normal dialog control will not use id larger than this value
#define ID_ADD_EFFECT_BASE	10001

// CDlgEffectPanel dialog

CDlgEffectPanel::CDlgEffectPanel(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgEffectPanel::IDD, pParent)
{
}

CDlgEffectPanel::~CDlgEffectPanel()
{
}

void CDlgEffectPanel::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_ADD_EFFECT, m_btnAddEffect);
	DDX_Control(pDX, IDC_BTN_ADD_SOUND, m_btnAddSound);
	DDX_Control(pDX, IDC_BTN_REMOVE, m_btnDel);
	DDX_Control(pDX, IDC_BTN_EFFECT_SETTINGS, m_btnConfig);
	DDX_Control(pDX, IDC_BTN_EFFECT_MOVE_UP, m_btnUp);
	DDX_Control(pDX, IDC_BTN_EFFECT_MOVE_DOWN, m_btnDown);
}


BEGIN_MESSAGE_MAP(CDlgEffectPanel, CResizableDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_EFFECT, OnBnClickedBtnAddEffect)
	ON_BN_CLICKED(IDC_BTN_ADD_SOUND, OnBnClickedBtnAddSound)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_EFFECT_MOVE_UP, OnBnClickedBtnEffectMoveUp)
	ON_BN_CLICKED(IDC_BTN_EFFECT_MOVE_DOWN, OnBnClickedBtnEffectMoveDown)
	ON_BN_CLICKED(IDC_BTN_EFFECT_SETTINGS, OnBnClickedBtnEffectSettings)

	ON_UPDATE_COMMAND_UI(IDC_BTN_ADD_EFFECT, OnUpdateBtnAddEffect)
	ON_UPDATE_COMMAND_UI(IDC_BTN_ADD_SOUND, OnUpdateBtnAddSound)
	ON_UPDATE_COMMAND_UI(IDC_BTN_REMOVE, OnUpdateBtnRemove)
	ON_UPDATE_COMMAND_UI(IDC_BTN_EFFECT_MOVE_UP, OnUpdateBtnEffectMoveUp)
	ON_UPDATE_COMMAND_UI(IDC_BTN_EFFECT_MOVE_DOWN, OnUpdateBtnEffectMoveDown)
	ON_UPDATE_COMMAND_UI(IDC_BTN_EFFECT_SETTINGS, OnUpdateBtnEffectSettings)

	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND_RANGE(ID_ADD_EFFECT_BASE, ID_ADD_EFFECT_BASE + 4096, OnAddEffect)
	ON_COMMAND(ID_SELECT_EFFECT, OnSelectEffect)
	ON_WM_INITMENUPOPUP()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()


// CDlgEffectPanel message handlers

BOOL CDlgEffectPanel::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_btnAddEffect.LoadBitmap(IDB_BUTTON_EFFECT_ADD, 1);
	m_btnAddEffect.ModifyFBStyle(0, FBS_RIGHTTEXT | FBS_DROPDOWN);

	m_btnAddSound.LoadBitmap(IDB_ADD_SOUND, 1);
	m_btnAddSound.ModifyFBStyle(0, FBS_RIGHTTEXT);

	m_btnDel.LoadBitmap(IDB_BUTTON_DELETE, 1);
	m_btnDel.ModifyFBStyle(0, FBS_RIGHTTEXT);

	m_btnConfig.LoadBitmap(IDB_BUTTON_EFFECT_CONFIG, 1);
	m_btnConfig.ModifyFBStyle(0, FBS_RIGHTTEXT);

	m_btnUp.LoadBitmap(IDB_BUTTON_UP, 1);
	//m_btnUp.ModifyFBStyle(0, 0);

	m_btnDown.LoadBitmap(IDB_BUTTON_DOWN, 1);
	//m_btnDown.ModifyFBStyle(0, 0);

	SetSizeGripVisibility(FALSE);

	AddAnchor(IDC_BTN_EFFECT_MOVE_UP, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_BTN_EFFECT_MOVE_DOWN, TOP_RIGHT, TOP_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgEffectPanel::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	// Get from MFC source code of CControlBar

	// Important: enable TAB navigation in child window
	// since 'IsDialogMessage' will eat frame window accelerators,
	// we call all frame windows' PreTranslateMessage first
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		CWnd* pOwner = GetOwner();
		while (pOwner != NULL)
		{
			// allow owner & frames to translate before IsDialogMessage does
			if (pOwner->PreTranslateMessage(pMsg))
				return TRUE;

			// try parent frames until there are no parent frames
			pOwner = pOwner->GetParentFrame();
		}
	}

	return CResizableDialog::PreTranslateMessage(pMsg);
}

void CDlgEffectPanel::OnAddEffect(UINT nID)
{
	// Leave text tool and select it
	my_app.CurTool()->Leave();

	int count = my_app.CurSel().count();
	if (count == 0)
	{
		AfxMessageBox(IDS_NEED_SELECT_ELEMENT);
		return;
	}

	if (count > 1)
	{
		ShowGroupBeforeAddEffectTip();

		CCmdGroupInstance *pCmd = new CCmdGroupInstance(my_app.CurSel());
		my_app.Commands().Do(pCmd);
	}

	int index = nID - ID_ADD_EFFECT_BASE;
	GEFFECTINFO_LIST &infoList = gldEffectManager::Instance()->GetEffectInfoList();
	ASSERT(index >= 0 && index < (int)infoList.size());
	if (index >= 0 && index < (int)infoList.size())
	{
		gldEffectInfo *pEffectInfo = infoList[index];

		if (pEffectInfo->m_type == gldEffectInfo::typeMotionPath1 ||
			pEffectInfo->m_type == gldEffectInfo::typeMotionPath2)
		{
			CDesignWnd::Instance()->SetFocus();
			my_app.AddPathEffect(pEffectInfo->m_clsid);
			return;
		}

		gldEffect* pEffect = new gldEffect(pEffectInfo->m_clsid);
		if (pEffect->m_pIEffect == NULL || pEffect->m_pIParas == NULL)
		{
			delete pEffect;

			BSTR bstrProgID;
			if (::ProgIDFromCLSID(pEffectInfo->m_clsid, &bstrProgID))
			{
				CString strProgID(bstrProgID);
				::CoTaskMemFree(bstrProgID);
				AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_FAILED_CREATE_EFFECT_1, (LPCTSTR)strProgID);
			}
			return;
		}

		gld_shape_sel::iterator it = my_app.CurSel().begin();
		gldInstance* pInstance = CTraitInstance(*it);

		// calculate ideal start time
		if (pInstance->m_effectList.size() > 0)
		{
			gldEffect *pEffectLast = *pInstance->m_effectList.rbegin();
			pEffect->m_startTime = pEffectLast->m_startTime + pEffectLast->m_length;
		}
		else
		{
			pEffect->m_startTime = 0;
		}

		// calculate ideal length
		pEffect->CalcIdealLength(pInstance->GetEffectKeyCount());		
		
		if (pEffect->m_pEffectInfo->m_autoShowConfig)
		{
			if (!pInstance->ConfigEffect(pInstance, pEffect))
			{
				delete pEffect;
				return;
			}
		}		

		my_app.Commands().Do(new CCmdInsertEffect(pInstance, pEffect, NULL));
	}
}

void CDlgEffectPanel::OnBnClickedBtnAddEffect()
{
	if (m_menuEffect.m_hMenu != NULL)
	{
		CRect rc;
		GetDlgItem(IDC_BTN_ADD_EFFECT)->GetWindowRect(&rc);

		m_btnAddEffect.SetHover(FALSE);
		m_btnAddEffect.SetCheck(BST_CHECKED);
		m_btnAddEffect.UpdateWindow();

		m_menuEffect.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, rc.right, rc.top, this);

		//for (BITMAP_LIST::iterator it = bmpList.begin();it != bmpList.end(); it++)
		//	::DeleteObject(*it);

		m_btnAddEffect.SetCheck(BST_UNCHECKED);
	}
}

void CDlgEffectPanel::OnBnClickedBtnRemove()
{
	CEffectWnd::Instance()->OnTimeLineDelete();
}

void CDlgEffectPanel::OnBnClickedBtnEffectMoveUp()
{
	CEffectWnd::Instance()->OnTimeLineMoveUp();
}

void CDlgEffectPanel::OnBnClickedBtnEffectMoveDown()
{
	CEffectWnd::Instance()->OnTimeLineMoveDown();
}

void CDlgEffectPanel::OnBnClickedBtnEffectSettings()
{
	CEffectWnd::Instance()->OnTimeLineConfig();
}

void CDlgEffectPanel::OnOK()
{
}

void CDlgEffectPanel::OnCancel()
{
}

int CDlgEffectPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CResizableDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	GetWindowRect(&m_rcInitial);

	// Init the menus
	if (m_menuEffect.CreatePopupMenu() && 
		m_menuEffectEnter.CreatePopupMenu() && 
		m_menuEffectEmphasize.CreatePopupMenu() && 
		m_menuEffectExit.CreatePopupMenu() && 
		m_menuMotionPath.CreatePopupMenu())
	{
		m_menuEffect.SetBitmapBackground(gldEffectManager::ICON_TRANSPARENT_COLOR);

		if (m_imgList.m_hImageList != NULL)
		{
			m_imgList.DeleteImageList();
		}
		gldEffectManager::Instance()->GetImageList(m_imgList);		

		CString strEntr;
		strEntr.LoadString(IDS_EFFECT_ENTRANCE);
		CString strEmph;
		strEmph.LoadString(IDS_EFFECT_EMPHASIS);
		CString strExit;
		strExit.LoadString(IDS_EFFECT_EXIT);
		CString strMotionPath;		
		strMotionPath.LoadString(IDS_EFFECT_MOTION_PATH);
		CString strSelect;
		strSelect.LoadString(IDS_EFFECT_SELECT);

		m_menuEffect.AppendMenu(MF_POPUP, (UINT)m_menuEffectEnter.m_hMenu
			, strEntr, &m_imgList, gldEffectManager::ICON_CAT_ENTRANCE);
		m_menuEffect.AppendMenu(MF_POPUP, (UINT)m_menuEffectEmphasize.m_hMenu
			, strEmph, &m_imgList, gldEffectManager::ICON_CAT_EMPHASIS);
		m_menuEffect.AppendMenu(MF_POPUP, (UINT)m_menuEffectExit.m_hMenu
			, strExit, &m_imgList, gldEffectManager::ICON_CAT_EXIT);
		m_menuEffect.AppendMenu(MF_POPUP, (UINT)m_menuMotionPath.m_hMenu
			, strMotionPath, &m_imgList, gldEffectManager::ICON_CAT_PATH);

		m_menuEffect.AppendMenu(MF_SEPARATOR, 0, NULL);
		m_menuEffect.AppendMenu(MF_BYCOMMAND, ID_SELECT_EFFECT, strSelect);

		GEFFECTINFO_LIST &infoList = gldEffectManager::Instance()->GetEffectInfoList();
		int i = 0;

		BOOL bAddSep = FALSE;
		for (GEFFECTINFO_LIST::iterator it = infoList.begin(); it != infoList.end(); it++, i++)
		{
			gldEffectInfo *pEffectInfo = *it;
			if (!pEffectInfo->m_name.empty())
			{
				HBITMAP hBmp = pEffectInfo->GetBitmap();

				CNewMenu *pMenu;
				switch (pEffectInfo->m_type)
				{
				case gldEffectInfo::typeEnter:
					pMenu = &m_menuEffectEnter;
					break;

				case gldEffectInfo::typeExit:
					pMenu = &m_menuEffectExit;
					break;

				case gldEffectInfo::typeMotionPath1:
					pMenu = &m_menuMotionPath;
					bAddSep = TRUE;
					break;

				case gldEffectInfo::typeMotionPath2:
					pMenu = &m_menuMotionPath;
					if (bAddSep)
					{
						bAddSep = FALSE;
						pMenu->AppendMenu(MF_BYPOSITION | MF_SEPARATOR);
					}
					break;

				case gldEffectInfo::typeEmphasize:
				default:
					pMenu = &m_menuEffectEmphasize;
					break;
				}

				pMenu->AppendMenu(MF_BYPOSITION, ID_ADD_EFFECT_BASE + i, pEffectInfo->m_name.c_str(), CBitmap::FromHandle(hBmp));
			}
		#ifdef _DEBUG
			else
			{
				BSTR bstrProgID = NULL;
				if (SUCCEEDED(ProgIDFromCLSID(pEffectInfo->m_clsid, &bstrProgID)))
				{
					CString strProgID(bstrProgID);
					::CoTaskMemFree(bstrProgID);

					m_menuEffect.AppendMenu(MF_BYPOSITION, ID_ADD_EFFECT_BASE + i, (LPCTSTR)strProgID);
				}
			}
		#endif
		}
	}

	return 0;
}

void CDlgEffectPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CResizableDialog::OnLButtonDown(nFlags, point);

	CControlBar *pBar = (CControlBar *)GetParent();
	BOOL bFloating = pBar->IsFloating();
	CWnd *pWnd = pBar->GetParent();
	if (!bFloating)
	{
		ClientToScreen(&point);
		pBar->ScreenToClient(&point);
		pBar->SendMessage(WM_LBUTTONDOWN, (WPARAM)MK_LBUTTON, MAKELPARAM(point.x, point.y));
	}
	else
	{
		pWnd = pWnd->GetParent();

		// must call this before SendMessage(), because the bar may be 
		// docked after it received a WM_NCLBUTTONDOWN message
		pWnd->SetForegroundWindow();

		ClientToScreen(&point);
		pWnd->SendMessage(WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, MAKELPARAM(point.x, point.y));
	}
}

void CDlgEffectPanel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd *pParent = this;
	while (pParent->GetStyle() & WS_CHILD)
	{
		pParent = pParent->GetParent();
		if (pParent->IsKindOf(RUNTIME_CLASS(CControlBar)))
		{
			ClientToScreen(&point);
			pParent->ScreenToClient(&point);
			pParent->SendMessage(WM_LBUTTONDBLCLK, (WPARAM)MK_LBUTTON, MAKELPARAM(point.x, point.y));
			break;
		}
	}

	CResizableDialog::OnLButtonDblClk(nFlags, point);
}

void CDlgEffectPanel::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CResizableDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here

	CNewMenu::OnInitMenuPopup(m_hWnd, pPopupMenu, nIndex, bSysMenu);
}

void CDlgEffectPanel::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	if (!CNewMenu::OnMeasureItem(GetCurrentMessage()))
		CResizableDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CDlgEffectPanel::OnSelectEffect()
{
	// Leave text tool and select it
	my_app.CurTool()->Leave();

	/*UINT nRet = IDYES;
	if (_GetCurInstance() == NULL)
	{	
		if (AfxGetApp()->GetProfileInt("Tips", "Cannot Apply Effect", TRUE))
		{
			BOOL bChecked = FALSE;
			nRet = CheckMessageBox(
				IDS_CANT_APPLY_EFFECT, 
				IDS_DONOT_SHOW_AGAIN, 
				bChecked, 
				MB_ICONQUESTION | MB_YESNO);
			AfxGetApp()->WriteProfileInt("Tips", "Cannot Apply Effect", !bChecked);
		}
	}
	if (nRet == IDYES)
	{*/
		int count = my_app.CurSel().count();
		if (count == 0)
		{
			AfxMessageBox(IDS_NEED_SELECT_ELEMENT);
			return;
		}

		if (count > 1)
		{
			ShowGroupBeforeAddEffectTip();

			CCmdGroupInstance *pCmd = new CCmdGroupInstance(my_app.CurSel());
			my_app.Commands().Do(pCmd);
		}

		CDlgSelectEffect dlg;

		if (dlg.DoModal() == IDOK)
		{
			CComPtr<IGLD_Parameters> pIParas;
			dlg.GetEffectOptions(&pIParas);				
			gldEffectInfo *pEInfo = dlg.GetSelEffectInfo();
			if (pEInfo == NULL || pIParas == NULL)
				return;			

			gldInstance *pInst = _GetCurInstance();	
			if (pInst != NULL)
			{
				gldEffect *pEffect = new gldEffect(pEInfo->m_clsid);
				if (pEffect->m_pIEffect == NULL || pEffect->m_pIParas == NULL)
				{
					delete pEffect;
					// show tips that effect is unvalidate
				}
				else
				{
					// calculate ideal start time
					if (pInst->m_effectList.size() > 0)
					{
						gldEffect *pEffectLast = *pInst->m_effectList.rbegin();
						pEffect->m_startTime = pEffectLast->m_startTime + pEffectLast->m_length;
					}
					else
					{
						pEffect->m_startTime = 0;
					}

					TCommandGroup *cmd = new TCommandGroup(IDS_CMD_ADDEFFECT);
					// copy effect options...
					if (pInst->m_pIParas != NULL)
					{
						CComPtr<IGLD_Parameters> common;
						VERIFY(SUCCEEDED(common.CoCreateInstance(__uuidof(GLD_Parameters))));
						CopyParameters(pIParas, PT_COMMON, common, NULL, 0);
						cmd->Do(new CCmdChangeEffectParameters(pInst->m_pIParas, common));
					}
					if (pEffect->m_pIParas != NULL)
						CopyParameters(pIParas, PT_CUSTOM, pEffect->m_pIParas, NULL, 0);							

					// calculate ideal length
					pEffect->CalcIdealLength(pInst->GetEffectKeyCount());
					//pEffect->m_startTime = pInst->GetMaxTime() - 1;
					// pEffect->m_length be initialized by gldEffect constructor to effect's Ideal length
					//pEffect->m_length = __max(24, pEffect->m_length * StatInstanceKeysCount(pInst));

					cmd->Do(new CCmdInsertEffect(pInst, pEffect, NULL));
					my_app.Commands().Do(cmd);
				}		
			}
		}
	//}
}

void CDlgEffectPanel::OnUpdateBtnAddEffect(CCmdUI *pCmdUI)
{
	gld_shape_sel& sel = my_app.CurSel();
	gld_shape_sel::iterator it = sel.begin();
	if (sel.count() > 0)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else
	{
		SETextToolEx *pTool = (SETextToolEx *)my_app.Tools()[IDT_TEXTTOOLEX - IDT_FIRST];
		if (pTool->IsActive())
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}

void CDlgEffectPanel::OnUpdateBtnRemove(CCmdUI *pCmdUI)
{
	CEffectWnd::Instance()->OnUpdateTimeLineDelete(pCmdUI);
}

void CDlgEffectPanel::OnUpdateBtnEffectMoveUp(CCmdUI *pCmdUI)
{
	CEffectWnd::Instance()->OnUpdateTimeLineMoveUp(pCmdUI);
}

void CDlgEffectPanel::OnUpdateBtnEffectMoveDown(CCmdUI *pCmdUI)
{
	CEffectWnd::Instance()->OnUpdateTimeLineMoveDown(pCmdUI);
}

void CDlgEffectPanel::OnUpdateBtnEffectSettings(CCmdUI *pCmdUI)
{
	CString strConfig;
	strConfig.LoadString(IDS_BTN_CONFIG_CAPTION);
	CString strInfo;
	strInfo.LoadString(IDS_BTN_INFO_CAPTION);
	if(CEffectWnd::Instance()->GetSelType()==CEffectWnd::SEL_SOUND
		|| CEffectWnd::Instance()->GetSelType()==CEffectWnd::SEL_SCAPT)
	{
		pCmdUI->SetText(strInfo);
	}
	else
	{
		pCmdUI->SetText(strConfig);
	}
	CEffectWnd::Instance()->OnUpdateTimeLineConfig(pCmdUI);
}

void CDlgEffectPanel::OnBnClickedBtnAddSound()
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_INSERT_SOUND, 0);
}

void CDlgEffectPanel::OnUpdateBtnAddSound(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}

