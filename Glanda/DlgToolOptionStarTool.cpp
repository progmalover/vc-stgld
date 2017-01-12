// DlgToolOptionStarTool.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgToolOptionStarTool.h"

#include "my_app.h"
#include "toolsdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgToolOptionStarTool dialog
IMPLEMENT_SINGLETON(CDlgToolOptionStarTool)
CDlgToolOptionStarTool::CDlgToolOptionStarTool(CWnd* pParent /*=NULL*/)
	: CDlgToolOption(CDlgToolOptionStarTool::IDD, pParent)
{
}

CDlgToolOptionStarTool::~CDlgToolOptionStarTool()
{
}

void CDlgToolOptionStarTool::DoDataExchange(CDataExchange* pDX)
{
	CDlgToolOption::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SPIN_POINTS, m_btnPoints);
	DDX_Control(pDX, IDC_EDIT_POINTS, m_edtPoints);
	DDX_Control(pDX, IDC_STATIC_POINTS, m_stcPoints);
}


BEGIN_MESSAGE_MAP(CDlgToolOptionStarTool, CDlgToolOption)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_POINTS, OnPointsModified)
END_MESSAGE_MAP()


// CDlgToolOptionStarTool message handlers

BOOL CDlgToolOptionStarTool::OnInitDialog()
{
	CDlgToolOption::OnInitDialog();

	// TODO:  Add extra initialization here

	//AddAnchor(IDC_EDIT_POINTS, TOP_LEFT, TOP_RIGHT);
	//AddAnchor(IDC_STATIC_SPIN_POINTS, TOP_RIGHT, TOP_RIGHT);

	SetSizeGripVisibility(FALSE);

	m_edtPoints.SetRange(3, 50, 5, FALSE, TRUE);
	m_btnPoints.SetRange(3, 50);
	m_btnPoints.SetBuddyWindow(m_edtPoints.m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgToolOptionStarTool::OnPointsModified()
{
	if (my_app.GetCurTool() == IDT_STAR)
		((SEStarTool *)my_app.CurTool())->SetVertexNum(m_edtPoints.GetValueInt());
}

void CDlgToolOptionStarTool::UpdateControls()
{
	if (my_app.GetCurTool() == IDT_STAR)
		SetDlgItemInt(IDC_EDIT_POINTS, ((SEStarTool *)my_app.CurTool())->GetVertexNum(), FALSE);
}
