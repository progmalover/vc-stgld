// CategoryToolsPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "CategoryToolsPage.h"
#include "DesignWnd.h"

#include "ToolButtonPopup.h"
#include "se_const.h"
#include "NotifyTransceiver.h"
#include "Observer.h"

#include "DlgToolOptionRoundRectTool.h"
#include "DlgToolOptionPolygonTool.h"
#include "DlgToolOptionStarTool.h"
#include "DlgToolOptionTextTool.h"
#include "VisualStylesXP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define BUTTON_WIDTH 28
#define BUTTON_HIEGHT 24

#define MARGIN_H 8
#define MARGIN_V 5

// CCategoryToolsPage dialog

//IMPLEMENT_DYNAMIC(CCategoryToolsPage, CCategoryPage)
CCategoryToolsPage::CCategoryToolsPage(CWnd* pParent /*=NULL*/)
	: CCategoryPage(CCategoryToolsPage::IDD, pParent)
{
	m_bInit = FALSE;
	m_nCurTool = -1;
	m_bLockCurrentTool = AfxGetApp()->GetProfileInt("Tools\\Main", "Lock Current Tool", FALSE) ? TRUE : FALSE;

	TNotifyTransceiver::Attach(SEN_TOOL_CHANGED, this, OnToolChanged);
}

CCategoryToolsPage::~CCategoryToolsPage()
{
	AfxGetApp()->WriteProfileInt("Tools\\Main", "Lock Current Tool", m_bLockCurrentTool);

	TNotifyTransceiver::Detach(SEN_TOOL_CHANGED, this);
}

void CCategoryToolsPage::DoDataExchange(CDataExchange* pDX)
{
	CCategoryPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCategoryToolsPage, CCategoryPage)
	ON_COMMAND_RANGE(IDT_FIRST + 1000, IDT_LAST + 1000, OnTools)
	ON_COMMAND(IDT_LOCK + 1000, OnToolLock)

	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)
	ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipText)
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CCategoryToolsPage message handlers



// CCategoryToolsPage message handlers

BOOL CCategoryToolsPage::OnInitDialog()
{
	CCategoryPage::OnInitDialog();

	// TODO:  Add extra initialization here

	//m_tooltip.Create(this, TTS_ALWAYSTIP);
	//m_tooltip.Activate(TRUE);	

	EnableToolTips(TRUE);

	CreateTools();
	CreateToolOptionDialogs();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCategoryToolsPage::CreateToolOptionDialogs()
{
	struct CRETAE_DIALOG
	{
		CDlgToolOption *pDialog;
		UINT nIDD;
		int nIDTool;
	}
	cd[] = 
	{
		{CDlgToolOptionRoundRectTool::Instance(), IDD_TOOL_OPTION_ROUND_RECT_TOOL, IDT_ROUNDRECT}, 
		{CDlgToolOptionPolygonTool::Instance(), IDD_TOOL_OPTION_POLYGON_TOOL, IDT_POLYGON}, 
		{CDlgToolOptionStarTool::Instance(), IDD_TOOL_OPTION_STAR_TOOL, IDT_STAR}, 
		{CDlgToolOptionTextTool::Instance(), IDD_TOOL_OPTION_TEXT_TOOL, IDT_TEXTTOOLEX}
	};

	for (int i = 0; i < sizeof(cd) / sizeof(CRETAE_DIALOG); i++)
	{
		if (!::IsWindow(cd[i].pDialog->m_hWnd))
			cd[i].pDialog->Create(cd[i].nIDD, this);
		m_options[cd[i].nIDTool] = cd[i].pDialog;
	}
}

void CCategoryToolsPage::CreateTools()
{
	CRect rc(0, 0, 0, 0);
	for (int i = 0, j = 0; i < sizeof(tools) / sizeof(TOOL); i++)
	{
		ASSERT(GetDlgItem(tools[i].nIDTool + 1) == NULL);
		CString strTip;
		strTip.LoadString(tools[i].nTip);
		m_tools[i].Create(strTip, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rc, this, tools[i].nIDTool + 1000);
		m_tools[i].LoadBitmap(tools[i].nIDBitmap);
		
		switch (tools[i].nIDTool)
		{
		case IDT_RECT:
			m_tools[i].AddTool(IDT_RECT, IDB_TOOL_RECT, IDS_TOOL_RECT);
			m_tools[i].AddTool(IDT_ROUNDRECT, IDB_TOOL_ROUND_RECT, IDS_TOOL_ROUND_RECT);
			m_tools[i].AddTool(IDT_POLYGON, IDB_TOOL_POLYGON, IDS_TOOL_POLYGON);
			m_tools[i].AddTool(IDT_STAR, IDB_TOOL_STAR, IDS_TOOL_STAR);
			m_tools[i].SetCurrentTool(0);
			break;
		default:
			break;
		}
	}

	CToolButton *pButton = (CToolButton *)GetDlgItem(IDT_LOCK + 1000);
	pButton->SetCheck(m_bLockCurrentTool);

	m_bInit = TRUE;

	RecalcLayout();
}

void CCategoryToolsPage::RecalcLayout()
{
	if (!m_bInit)
		return;

	CRect rc;
	GetClientRect(&rc);

	int count = sizeof(tools) / sizeof(TOOL);

	//int cols = (rc.Width() - (MARGIN_H * 2)) / BUTTON_WIDTH;
	//cols = max(1, cols);

	int x = MARGIN_H;
	int y = MARGIN_V;
	for (int i = 0, j = 0; i < count; i++)
	{
		m_tools[i].SetWindowPos(NULL, x, y, BUTTON_WIDTH, BUTTON_HIEGHT	, SWP_NOZORDER | SWP_NOACTIVATE);

		if (tools[i].bAddSep || x + BUTTON_WIDTH * 2 > rc.right - MARGIN_H)
		{
			x = MARGIN_H;
			y += MARGIN_V + BUTTON_HIEGHT;
		}
		else
		{
			x += BUTTON_WIDTH;
		}
	}

	CDialog *pDlg = GetOptionDialog(m_nCurTool);
	if (pDlg)
	{
		pDlg->SetWindowPos(NULL, MARGIN_H, y, rc.Width() - MARGIN_H * 2, rc.Height() - MARGIN_V * 2 - y, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CCategoryToolsPage::OnTools(UINT nID)
{
	nID -= 1000;
	if (m_nCurTool != nID)
		my_app.SetCurTool(nID);
}

BOOL CCategoryToolsPage::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	//m_tooltip.RelayEvent(pMsg);

	return CCategoryPage::PreTranslateMessage(pMsg);
}

void CCategoryToolsPage::OnOK()
{
}

void CCategoryToolsPage::OnCancel()
{
}

void CCategoryToolsPage::OnToolChanged(void *sender, void *receiver, int nNotifyID, LPARAM lParam)
{
	ASSERT(nNotifyID == SEN_TOOL_CHANGED);
	if (nNotifyID == SEN_TOOL_CHANGED)
	{
		CCategoryToolsPage *pObj = (CCategoryToolsPage *)receiver;
		if (pObj)
		{
			ASSERT(lParam >= IDT_FIRST && lParam <= IDT_LAST);
			if (lParam >= IDT_FIRST && lParam <= IDT_LAST)
			{
				pObj->ToolChanged((int)lParam);
			}
		}
	}
}

void CCategoryToolsPage::ToolChanged(int nID)
{
	if (m_nCurTool >= IDT_FIRST && m_nCurTool <= IDT_LAST)
	{
		if (m_nCurTool == nID)
		{
			return;
		}
		CToolButton *pButton = (CToolButton *)GetDlgItem(m_nCurTool + 1000);		
		if (pButton)
		{
			pButton->SetCheck(BST_UNCHECKED);
		}
	}

	CToolButton *pButton = (CToolButton *)GetDlgItem(nID + 1000);
	//ASSERT_VALID(pButton);
	if (pButton)	// user can not select path tool
	{
		pButton->SetCheck(BST_CHECKED);

		CDlgToolOption *pDlg = GetOptionDialog(m_nCurTool);
		if (pDlg)
		{
			pDlg->EnableWindow(FALSE);
			pDlg->ShowWindow(SW_HIDE);
		}

		m_nCurTool = nID;
		pDlg = GetOptionDialog(m_nCurTool);
		if (pDlg)
		{
			pDlg->UpdateControls();
			pDlg->EnableWindow(TRUE);
			pDlg->ShowWindow(SW_SHOWNOACTIVATE);
		}

		RecalcLayout();
	}
	else
	{
		m_nCurTool = -1;
	}
}

CDlgToolOption *CCategoryToolsPage::GetOptionDialog(int nIDTool)
{
	OPTION_DIALOG_LIST::iterator it = m_options.find(nIDTool);
	if (it != m_options.end())
		return it->second;
	return NULL;
}

void CCategoryToolsPage::OnToolLock()
{
	CToolButton *pButton = (CToolButton *)GetDlgItem(IDT_LOCK + 1000);
	m_bLockCurrentTool = !(pButton->GetCheck() == BST_CHECKED);
	pButton->SetCheck(m_bLockCurrentTool ? BST_CHECKED : BST_UNCHECKED);

	my_app.LockTool(m_bLockCurrentTool ? true : false);
}

IMPLEMENT_OBSERVER(CCategoryToolsPage, ChangeMainMovie2)
{
	for (int i = 0; i < sizeof(tools) / sizeof(TOOL); i++)
		if (m_tools[i].HasSubTools())
			m_tools[i].SetCurrentTool(0);
	my_app.LockTool(m_bLockCurrentTool ? true : false);
}

BOOL CCategoryToolsPage::GetToolTipText(HWND hWnd, CString &strTip)
{
	for (int i = 0; i < sizeof(tools) / sizeof(TOOL); i++)
	{
		if (m_tools[i].m_hWnd == hWnd)
		{
			m_tools[i].GetToolTip(strTip);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCategoryToolsPage::OnToolTipText(UINT nID, NMHDR * pNMHDR, LRESULT * pResult)
{
	*pResult = 0;

	CString strTipText;
	if (pNMHDR->code == TTN_NEEDTEXTA)
	{
		TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
		if (pTTTA->uFlags & TTF_IDISHWND)
		{
			if (GetToolTipText((HWND)pNMHDR->idFrom, strTipText))
			{
				_tcsncpy(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
				return TRUE;
			}
		}
	}
	else if (pNMHDR->code == TTN_NEEDTEXTW)
	{
		TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
		if (pTTTW->uFlags & TTF_IDISHWND)
		{
			if (GetToolTipText((HWND)pNMHDR->idFrom, strTipText))
			{
				_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CCategoryToolsPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CCategoryPage::OnLButtonDown(nFlags, point);

	CControlBar *pBar = (CControlBar *)GetParent()->GetParent()->GetParent();
	BOOL bFloating = pBar->IsFloating();

	if (!bFloating)
	{
		ClientToScreen(&point);
		pBar->ScreenToClient(&point);
		pBar->SendMessage(WM_LBUTTONDOWN, (WPARAM)MK_LBUTTON, MAKELPARAM(point.x, point.y));
	}
	else
	{
		CWnd *pWnd = pBar->GetParent()->GetParent();

		// must call this before SendMessage(), because the bar may be 
		// docked after it received a WM_NCLBUTTONDOWN message
		pWnd->SetForegroundWindow();

		ClientToScreen(&point);
		pWnd->SendMessage(WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, MAKELPARAM(point.x, point.y));
	}
}

void CCategoryToolsPage::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CCategoryPage::OnLButtonDblClk(nFlags, point);

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
}

void CCategoryToolsPage::OnSize(UINT nType, int cx, int cy)
{
	CCategoryPage::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RecalcLayout();
}

void CCategoryToolsPage::OnDestroy()
{
	CCategoryPage::OnDestroy();

	// TODO: Add your message handler code here

	CDlgToolOptionRoundRectTool::ReleaseInstance();
	CDlgToolOptionPolygonTool::ReleaseInstance();
	CDlgToolOptionStarTool::ReleaseInstance();
	CDlgToolOptionTextTool::ReleaseInstance();
}
