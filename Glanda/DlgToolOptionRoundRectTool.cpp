// DlgToolOptionRoundRectTool.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgToolOptionRoundRectTool.h"

#include "my_app.h"
#include "toolsdef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgToolOptionRoundRectTool dialog

IMPLEMENT_SINGLETON(CDlgToolOptionRoundRectTool)
CDlgToolOptionRoundRectTool::CDlgToolOptionRoundRectTool(CWnd* pParent /*=NULL*/)
	: CDlgToolOption(CDlgToolOptionRoundRectTool::IDD, pParent)
{
}

CDlgToolOptionRoundRectTool::~CDlgToolOptionRoundRectTool()
{
}

void CDlgToolOptionRoundRectTool::DoDataExchange(CDataExchange* pDX)
{
	CDlgToolOption::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SPIN_RADIUS, m_btnRadius);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_edtRadius);
	DDX_Control(pDX, IDC_STATIC_RADIUS, m_stcRadius);
}


BEGIN_MESSAGE_MAP(CDlgToolOptionRoundRectTool, CDlgToolOption)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_RADIUS, OnRadiusModified)
END_MESSAGE_MAP()


// CDlgToolOptionRoundRectTool message handlers

BOOL CDlgToolOptionRoundRectTool::OnInitDialog()
{
	CDlgToolOption::OnInitDialog();

	// TODO:  Add extra initialization here

	//AddAnchor(IDC_EDIT_RADIUS, TOP_LEFT, TOP_RIGHT);
	//AddAnchor(IDC_STATIC_SPIN_RADIUS, TOP_RIGHT, TOP_RIGHT);

	SetSizeGripVisibility(FALSE);

	m_edtRadius.SetRange(1, 200, 5, FALSE, TRUE);
	m_btnRadius.SetRange(1, 50);
	m_btnRadius.SetBuddyWindow(m_edtRadius.m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgToolOptionRoundRectTool::OnRadiusModified()
{
	if (my_app.GetCurTool() == IDT_ROUNDRECT)
		((SERoundRectTool *)my_app.CurTool())->SetRoundRadius(m_edtRadius.GetValueInt() * 20);
}

void CDlgToolOptionRoundRectTool::UpdateControls()
{
	if (my_app.GetCurTool() == IDT_ROUNDRECT)
		SetDlgItemInt(IDC_EDIT_RADIUS, ((SERoundRectTool *)my_app.CurTool())->GetRoundRadius() / 20, FALSE);
}
