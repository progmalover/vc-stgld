// SceneActionPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include ".\sceneactionpage.h"

#include "GlandaDoc.h"
#include "my_app.h"

// CSceneActionPage dialog

CSceneActionPage::CSceneActionPage()
	: CResizablePage(CSceneActionPage::IDD)
{
	m_nCurSel = -1;

	gldScene2 *pScene = _GetCurScene2();	
	m_strActionEnter = pScene->m_actionEnter.c_str();
	m_strActionExit = pScene->m_actionExit.c_str();

	m_pCmd = NULL;
}

CSceneActionPage::~CSceneActionPage()
{
}

void CSceneActionPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EVENTS, m_list);
}


BEGIN_MESSAGE_MAP(CSceneActionPage, CResizablePage)
	ON_LBN_SELCHANGE(IDC_LIST_EVENTS, OnSelChangeEvents)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL, OnBnClickedButtonClearAll)
END_MESSAGE_MAP()


// CSceneActionPage message handlers

BOOL CSceneActionPage::OnInitDialog()
{
	CResizablePage::OnInitDialog();

	// TODO:  Add extra initialization here

	FillListBox(&m_list, IDS_LIST_SCENE_EVENTS);
	m_list.LoadImages(IDB_EVENTS, 16, RGB(255, 0, 255));
	m_list.SetItemImage(0, 0);
	m_list.SetItemImage(1, 0);

	m_dlgAction.Create(IDD_SCENE_ACTION, this);

	CRect rc;
	GetDlgItem(IDC_STATIC_PLACEHOLDER)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_dlgAction.MoveWindow(&rc);

	AddAnchor(m_dlgAction.m_hWnd, TOP_LEFT, BOTTOM_RIGHT);

	m_dlgAction.ShowWindow(SW_SHOW);

	m_list.SetCurSel(0);
	OnSelChangeEvents();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneActionPage::OnSelChangeEvents()
{
	if (m_nCurSel >= 0)
	{
		if (!m_dlgAction.UpdateAction())
		{
			m_list.SetCurSel(m_nCurSel);
			return;
		}
	}

	UpdateListBoxItems();

	int index = m_list.GetCurSel();
	ASSERT(index >= 0 && index <= 1);

	m_nCurSel = index;

	gldScene2 *pScene = _GetCurScene2();
	m_dlgAction.m_pstrAction = &(index == 0 ? m_strActionEnter : m_strActionExit);
	m_dlgAction.ParseAction();
}

BOOL CSceneActionPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!m_dlgAction.UpdateAction())
		return FALSE;

	UpdateListBoxItems();

	if (m_pCmd)
		delete m_pCmd;

	m_pCmd = new TCommandGroup(IDS_CMD_CHANGEACTION);

	gldScene2 *pScene = _GetCurScene2();
	m_pCmd->Do(new CCmdSetSceneAction(pScene, m_strActionEnter, true));
	m_pCmd->Do(new CCmdSetSceneAction(pScene, m_strActionExit, false));

	return CResizablePage::OnApply();
}

BOOL CSceneActionPage::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!m_dlgAction.UpdateAction())
		return FALSE;

	UpdateListBoxItems();

	return CResizablePage::OnKillActive();
}

void CSceneActionPage::UpdateListBoxItems()
{
	m_list.SetItemImage(0, m_strActionEnter.IsEmpty() ? 0 : 1);
	m_list.SetItemImage(1, m_strActionExit.IsEmpty() ? 0 : 1);
}

void CSceneActionPage::OnBnClickedButtonClearAll()
{
	// TODO: Add your control notification handler code here

	m_strActionEnter.Empty();
	m_strActionExit.Empty();

	UpdateListBoxItems();

	m_dlgAction.ParseAction();
}
