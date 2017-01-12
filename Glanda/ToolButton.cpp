// ToolButton.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ToolButton.h"

#include "ToolButtonPopup.h"
#include ".\toolbutton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CToolButton

CToolButton::CToolButton()
{
	m_pPopup = NULL;
}

CToolButton::~CToolButton()
{
}


BEGIN_MESSAGE_MAP(CToolButton, CFlatBitmapButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CToolButton message handlers


void CToolButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);

	if (m_pPopup)
		SetTimer(2, 150, NULL);
}

void CToolButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pPopup)
		KillTimer(2);

	//CFlatBitmapButton::OnLButtonUp(nFlags, point);
}

void CToolButton::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 2)
	{
		ASSERT(m_pPopup);

		KillTimer(2);

		CRect rc;
		GetWindowRect(&rc);
		BOOL ret = m_pPopup->Track(CPoint(rc.right, rc.top));
		m_pPopup->ShowWindow(SW_HIDE);
		if (ret)
		{
			SetCurrentTool(m_pPopup->GetCurrentTool());
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}

		return;
	}

	CFlatBitmapButton::OnTimer(nIDEvent);
}

void CToolButton::AddTool(UINT nID, UINT nIDBitmap, UINT nStringID)
{
	if (!m_pPopup)
	{
		m_pPopup = new CToolButtonPopup();
		m_pPopup->Create();

		ModifyFBStyle(0, FBS_DROPDOWN2);
	}

	CString strTool;
	strTool.LoadString(nStringID);
	m_pPopup->AddTool(nID + TOOL_BUTTON_CTLID_BASE, nIDBitmap, strTool);
}

void CToolButton::SetCurrentTool(int index)
{
	m_pPopup->SetCurrentTool(index);
	LoadBitmap(m_pPopup->m_list[index].m_nIDBitmap);
	SetDlgCtrlID(m_pPopup->m_list[index].m_nID);
}

void CToolButton::OnDestroy()
{
	CFlatBitmapButton::OnDestroy();

	// TODO: Add your message handler code here

	if (m_pPopup)
		m_pPopup->DestroyWindow();
	delete m_pPopup;
	m_pPopup = NULL;
}

BOOL CToolButton::GetToolTip(CString &strTip)
{
	if (m_pPopup == NULL)
	{
		GetWindowText(strTip);
		return TRUE;
	}
	else
	{
		int index = m_pPopup->GetCurrentTool();
		if (index >= 0)
		{
			m_pPopup->GetItemText(index, strTip);
			return TRUE;
		}
	}

	return FALSE;
}
