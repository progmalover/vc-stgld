// PropGrid.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PropGrid.h"
#include "my_app.h"
#include ".\propgrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CPropGrid dialog

IMPLEMENT_DYNAMIC(CPropGrid, CDialog)
CPropGrid::CPropGrid(CWnd* pParent /*=NULL*/)
	: CDialog(CPropGrid::IDD, pParent)
{
}

CPropGrid::~CPropGrid()
{
}

void CPropGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTNCOLOR, m_btnColor);
	DDX_Control(pDX, IDC_EDTXDIST, m_edtXDist);
	DDX_Control(pDX, IDC_EDTYDIST, m_edtYDist);	
}


BEGIN_MESSAGE_MAP(CPropGrid, CDialog)
	ON_CONTROL(EN_MODIFY, IDC_EDTXDIST, OnEditModifyXDist)
	ON_CONTROL(EN_MODIFY, IDC_EDTYDIST, OnEditModifyYDist)
	ON_CONTROL(BN_COLORCHANGE, IDC_BTNCOLOR, OnColorChanged)
	ON_BN_CLICKED(IDC_CHKSHOWGRID, OnBnClickedChkshowgrid)
	ON_BN_CLICKED(IDC_CHKSNAPTOGRID, OnBnClickedChksnaptogrid)
END_MESSAGE_MAP()

void CPropGrid::UpdateControls()
{
	TShapeEditorGrid	grid;

	my_app.GetGrid(&grid);
	
	SetDlgItemInt(IDC_EDTXDIST, TWIPS_TO_PIXEL(grid.xDist), FALSE);
	SetDlgItemInt(IDC_EDTYDIST, TWIPS_TO_PIXEL(grid.yDist), FALSE);
	m_btnColor.SetColor(grid.color & 0x00FFFFFF, 255);
	CheckDlgButton(IDC_CHKSHOWGRID, my_app.IsShowGrid() ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHKSNAPTOGRID, my_app.IsSnapToGrid() ? BST_CHECKED : BST_UNCHECKED);
}

// CPropGrid message handlers
afx_msg void CPropGrid::OnEditModifyXDist()
{
	HandleChange();
}

afx_msg void CPropGrid::OnEditModifyYDist()
{
	HandleChange();
}

afx_msg void CPropGrid::OnColorChanged()
{
	HandleChange();
}

void CPropGrid::HandleChange()
{
	TShapeEditorGrid grid;

	grid.xDist = PIXEL_TO_TWIPS(GetDlgItemInt(IDC_EDTXDIST, NULL, FALSE));
	grid.yDist = PIXEL_TO_TWIPS(GetDlgItemInt(IDC_EDTYDIST, NULL, FALSE));
	grid.color = m_btnColor.GetColor();

	my_app.SetGrid(grid);
}

BOOL CPropGrid::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_edtXDist.SetRange(4, 40, 20, FALSE, TRUE);
	m_edtYDist.SetRange(4, 40, 20, FALSE, TRUE);

	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropGrid::OnBnClickedChkshowgrid()
{
	// TODO: Add your control notification handler code here
	my_app.ShowGrid(IsDlgButtonChecked(IDC_CHKSHOWGRID) != BST_UNCHECKED);
}

void CPropGrid::OnBnClickedChksnaptogrid()
{
	// TODO: Add your control notification handler code here
	my_app.SnapToGrid(IsDlgButtonChecked(IDC_CHKSNAPTOGRID) != BST_UNCHECKED);
}
