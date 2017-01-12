// InstanceActionPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "InstanceActionPage.h"

#include "GlandaDoc.h"
#include "my_app.h"
#include "SWFProxy.h"
#include "gldInstance.h"

#include "ASView.h"
#include "ASDoc.h"

#include "Regexx.h"
using namespace regexx;

// CInstanceActionPage dialog

CInstanceActionPage::CInstanceActionPage()
	: CResizablePage(CInstanceActionPage::IDD)
{
	m_nCurSel = -1;
	m_pCmd = NULL;
}

CInstanceActionPage::~CInstanceActionPage()
{
}

void CInstanceActionPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EVENTS, m_list);
}


BEGIN_MESSAGE_MAP(CInstanceActionPage, CResizablePage)
	ON_LBN_SELCHANGE(IDC_LIST_EVENTS, OnSelChangeEvents)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL, OnBnClickedButtonClearAll)
END_MESSAGE_MAP()

// CInstanceActionPage message handlers
struct InstanceEvent
{
	LPCTSTR	name;
	UINT	disp;
};

void CInstanceActionPage::ParseAction()
{
	gld_shape_sel sel = my_app.CurSel();
	gldInstance *pInstance = CTraitInstance(*sel.begin());

	LPCTSTR lpszAction = pInstance->m_action.c_str();

	const InstanceEvent events[] =	
	{
		{"release", IDS_RELEASE},
		{"press", IDS_PRESS},
		{"releaseOutside", IDS_RELEASEOUTSIDE},
		{"rollOver", IDS_ROLLOVER},
		{"rollOut", IDS_ROLLOUT},
		{"dragOver", IDS_DRAGOVER},
		{"dragOut", IDS_DRAGOUT}
	};

	for (int i = 0; i < sizeof(events) / sizeof(InstanceEvent); i++)
	{
		EVENT event;
		event.strEvent = events[i].name;

		BOOL bFound = FindEvent(lpszAction, events[i].name, event.strAction);
		m_eventList.push_back(event);

		CString strDisp;
		strDisp.LoadString(events[i].disp);
		m_list.AddString(strDisp);
		m_list.SetItemImage(i, bFound);
	}
}

BOOL CInstanceActionPage::FindEvent(LPCTSTR lpszAction, LPCTSTR lpszEvent, CString &strAction)
{
	CString strExp;
	strExp.Format(
		"//{{GLANDA_EVENT\r\n"
		"on[ \t]*\\(%s\\)[\r\n]*"
		"{\r\n"
		"(.*?)\r\n"
		"}\r\n"
		"//}}GLANDA_EVENT", 
		lpszEvent);

	Regexx re;
	if (re.exec(lpszAction, (LPCTSTR)strExp, Regexx::nocase | Regexx::dotall) != 0)
	{
		strAction =re.match[0].atom[0].str().c_str();
		return TRUE;
	}
	return FALSE;
}

BOOL CInstanceActionPage::OnInitDialog()
{
	CResizablePage::OnInitDialog();

	// TODO:  Add extra initialization here

	m_list.LoadImages(IDB_EVENTS, 16, RGB(255, 0, 255));

	ParseAction();

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

void CInstanceActionPage::OnSelChangeEvents()
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
	ASSERT(index >= 0);

	m_nCurSel = index;

	m_dlgAction.m_pstrAction = &m_eventList[index].strAction;
	m_dlgAction.ParseAction();
}

BOOL CInstanceActionPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!m_dlgAction.UpdateAction())
		return FALSE;

	UpdateListBoxItems();

	if (m_pCmd)
		delete m_pCmd;

	CString strAction;
	for (int i = 0; i < m_eventList.size(); i++)
	{
		EVENT &event = m_eventList[i];
		if (!event.strAction.IsEmpty())
		{
			CString strHandler;
			strHandler.Format(
				"//{{GLANDA_EVENT\r\n"
				"on(%s)\r\n"
				"{\r\n"
				"%s\r\n"
				"}\r\n"
				"//}}GLANDA_EVENT\r\n", 
				(LPCTSTR)event.strEvent, 
				(LPCTSTR)event.strAction);
			strAction += strHandler;	
		}
	}

	m_pCmd = new TCommandGroup(IDS_CMD_CHANGEACTION);

	gld_shape_sel sel = my_app.CurSel();
	gldInstance *pInstance = CTraitInstance(*sel.begin());

	m_pCmd->Do(new CCmdSetAction(pInstance, strAction));

	return CResizablePage::OnApply();
}

BOOL CInstanceActionPage::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!m_dlgAction.UpdateAction())
		return FALSE;

	UpdateListBoxItems();

	return CResizablePage::OnKillActive();
}

void CInstanceActionPage::UpdateListBoxItems()
{
	for (int i = 0; i < m_eventList.size(); i++)
	{
		EVENT &event = m_eventList[i];
		m_list.SetItemImage(i, event.strAction.IsEmpty() ? 0 : 1);
	}
}

void CInstanceActionPage::OnBnClickedButtonClearAll()
{
	// TODO: Add your control notification handler code here

	for (int i = 0; i < m_eventList.size(); i++)
	{
		EVENT &event = m_eventList[i];
		event.strAction.Empty();
	}

	UpdateListBoxItems();

	m_dlgAction.ParseAction();
}
