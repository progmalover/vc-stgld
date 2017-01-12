// DlgToolOptionPolygonTool.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgToolOptionPolygonTool.h"

#include "my_app.h"
#include "toolsdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgToolOptionPolygonTool dialog

IMPLEMENT_SINGLETON(CDlgToolOptionPolygonTool)
CDlgToolOptionPolygonTool::CDlgToolOptionPolygonTool(CWnd* pParent /*=NULL*/)
	: CDlgToolOption(CDlgToolOptionPolygonTool::IDD, pParent)
{
}

CDlgToolOptionPolygonTool::~CDlgToolOptionPolygonTool()
{
}

void CDlgToolOptionPolygonTool::DoDataExchange(CDataExchange* pDX)
{
	CDlgToolOption::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SPIN_POINTS, m_btnPoints);
	DDX_Control(pDX, IDC_EDIT_POINTS, m_edtPoints);
	DDX_Control(pDX, IDC_STATIC_POINTS, m_stcPoints);
}


BEGIN_MESSAGE_MAP(CDlgToolOptionPolygonTool, CDlgToolOption)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_POINTS, OnPointsModified)
END_MESSAGE_MAP()


// CDlgToolOptionPolygonTool message handlers

BOOL CDlgToolOptionPolygonTool::OnInitDialog()
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

void CDlgToolOptionPolygonTool::OnPointsModified()
{
	switch (my_app.GetCurTool())
	{
	case IDT_POLYGON:
		((SEPolygonTool *)my_app.CurTool())->SetVertexNum(m_edtPoints.GetValueInt());
		break;
	case IDT_POLYGONGUIDLINE:
		((SEPolygonGuidLineTool *)my_app.CurTool())->SetVertexNum(m_edtPoints.GetValueInt());
		break;
	default:
		ASSERT(FALSE);
	}
}

void CDlgToolOptionPolygonTool::UpdateControls()
{
	int nPoints;
	switch (my_app.GetCurTool())
	{
	case IDT_POLYGON:
		nPoints = ((SEPolygonTool *)my_app.CurTool())->GetVertexNum();
		break;
	case IDT_POLYGONGUIDLINE:
		nPoints = ((SEPolygonGuidLineTool *)my_app.CurTool())->GetVertexNum();
		break;
	default:
		ASSERT(FALSE);
	}

	SetDlgItemInt(IDC_EDIT_POINTS, nPoints, FALSE);
}
