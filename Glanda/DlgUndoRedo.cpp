// DlgUndoRedo.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgUndoRedo.h"

#include "my_app.h"
#include ".\dlgundoredo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgUndo dialog

IMPLEMENT_DYNAMIC(CDlgUndo, CDialog)
IMPLEMENT_SINGLETON(CDlgUndo)

CDlgUndo::CDlgUndo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUndo::IDD, pParent)
{
	m_bUndo = TRUE;
}

CDlgUndo::~CDlgUndo()
{
}

void CDlgUndo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CDlgUndo, CDialog)
	ON_MESSAGE(WM_FLOATSTATUS, OnFloatStatus)
	ON_LBN_KILLFOCUS(IDC_LIST, OnKillFocusList)
	ON_STN_CLICKED(IDC_STATIC_STATUS, OnStnClickedStaticStatus)
END_MESSAGE_MAP()


// CDlgUndo message handlers

BOOL CDlgUndo::TrackMessage()
{
	if (m_list.TrackMessage())
	{
		int index = m_list.GetCaretIndex();
		ASSERT(index >= 0);

		if (index >= 0)
		{
			if (m_bUndo)
			{
				TRACE1("Undo %d Actions.\n", index + 1);

				// KillFocus() must have been triggered
				//LeaveEditControl();
				//LeaveActionScriptEdit();

				my_app.CurTool()->Leave();

				my_app.SetRedraw(false);
				my_app.SetRepaint(false);
				my_app.Commands().Undo(index + 1);
				my_app.SetRedraw(true);
				my_app.SetRepaint(true);
			}
			else
			{
				TRACE1("Redo %d Actions.\n", index + 1);

				// KillFocus() must have been triggered
				//LeaveEditControl();
				//LeaveActionScriptEdit();

				my_app.CurTool()->Leave();

				my_app.SetRedraw(false);
				my_app.SetRepaint(false);
				my_app.Commands().Redo(index + 1);
				my_app.SetRedraw(true);
				my_app.SetRepaint(true);
			}

		}

		return TRUE;
	}

	return FALSE;
}

void CDlgUndo::LoadList(BOOL bUndo)
{
	m_bUndo = bUndo;
	m_list.LoadList(bUndo);

	int count = m_list.GetCount();
	int nHeight = m_list.GetItemHeight(0);

	CRect rcList;
	m_list.GetWindowRect(&rcList);
	if (count < 8)
		rcList.bottom = rcList.top + nHeight * count;
	else
		rcList.bottom = rcList.top + nHeight * 8;

	ScreenToClient(&rcList);
	m_list.SetWindowPos(0, 0, 0, rcList.Width(), rcList.Height(), SWP_NOZORDER | SWP_NOMOVE);

	CRect rcStatic;
	CWnd *pStatic = GetDlgItem(IDC_STATIC_STATUS);
	pStatic->GetWindowRect(&rcStatic);
	ScreenToClient(&rcStatic);

	rcStatic.OffsetRect(rcList.left - rcStatic.left, rcList.bottom + 2 - rcStatic.top);
	pStatic->SetWindowPos(0, rcStatic.left, rcStatic.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	CRect rc;
	rc.UnionRect(&rcList, &rcStatic);
	CalcWindowRect(&rc);

	SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOMOVE);
}

LRESULT CDlgUndo::OnFloatStatus(WPARAM wParam, LPARAM)
{
	ASSERT(!((wParam & FS_SHOW) && (wParam & FS_HIDE)));
	ASSERT(!((wParam & FS_ENABLE) && (wParam & FS_DISABLE)));
	ASSERT(!((wParam & FS_ACTIVATE) && (wParam & FS_DEACTIVATE)));

	// FS_SYNCACTIVE is used to detect MFS_SYNCACTIVE windows
	LRESULT lResult = 0;
	if ((GetStyle() & MFS_SYNCACTIVE) && (wParam & FS_SYNCACTIVE))
		lResult = 1;

	if (wParam & (FS_SHOW|FS_HIDE))
	{
		SetWindowPos(NULL, 0, 0, 0, 0,
			((wParam & FS_SHOW) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW) | SWP_NOZORDER |
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	if (wParam & (FS_ENABLE|FS_DISABLE))
		EnableWindow((wParam & FS_ENABLE) != 0);

	if ((wParam & (FS_ACTIVATE|FS_DEACTIVATE)) &&
		GetStyle() & MFS_SYNCACTIVE)
	{
		ModifyStyle(MFS_SYNCACTIVE, 0);
		SendMessage(WM_NCACTIVATE, (wParam & FS_ACTIVATE) != 0);
		ModifyStyle(0, MFS_SYNCACTIVE);
	}

	return lResult;
}

void CDlgUndo::OnKillFocusList()
{
	// TODO: Add your control notification handler code here
	
	m_list.PostMessage(WM_HIDELISTBOX, 0, 0L);
}

void CDlgUndo::UpdateStatus(int nCurSel)
{
	CString str;
	
	if (nCurSel == 0)
	{
		str.LoadString(m_bUndo ? IDS_ACTION_UNDO_ACTION : IDS_ACTION_REDO_ACTION);
	}
	else
	{
		str.Format(m_bUndo ? IDS_ACTION_UNDO_ACTIONS1 : IDS_ACTION_REDO_ACTIONS1, nCurSel + 1);
	}

	SetDlgItemText(IDC_STATIC_STATUS, str);
}

BOOL CDlgUndo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	ModifyStyle(0, MFS_SYNCACTIVE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUndo::OnStnClickedStaticStatus()
{
	// TODO: 在此添加控件通知处理程序代码
}
