// EditMatrixPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMatrixPage.h"
#include "gld_selection.h"
#include "my_app.h"
#include "CmdModifyMatrix.h"
#include "SWFProxy.h"
#include "DrawHelper.h"


// CEditMatrixPage dialog

void RecalcTransformParam(int& x, int& y, int& w, int& h, float& scalex, float& scaley, float& skewx, float& skewy, gld_shape shape, TMatrix& matrix)
{
	matrix.GetTransformParam(scalex, scaley, skewx, skewy);

	gld_rect box = _transform_rect(shape.bound(), matrix);
	gld_point op = shape.trans_origin();
	x = op.x;
	y = op.y;
	matrix.Transform(x, y);
	w = box.width();
	h = box.height();
}

IMPLEMENT_DYNAMIC(CEditMatrixPage, CPropertyPage)
CEditMatrixPage::CEditMatrixPage(gldInstance* pInstance)
	: CPropertyPage(CEditMatrixPage::IDD)
	, m_pInstance(pInstance)
	, m_pCmd(NULL)
{
	CTransAdaptor::GM2TM(m_pInstance->m_matrix, m_matrix);
	RecalcTransformParam(m_x, m_y, m_w, m_h, m_scalex, m_scaley, m_skewx, m_skewy, CTraitShape(m_pInstance), m_matrix);
}

CEditMatrixPage::~CEditMatrixPage()
{
}

void CEditMatrixPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_CONSTRAIN_ANGLE, m_btnConstrainAngle);
	DDX_Control(pDX, IDC_BUTTON_CONSTRAIN_SIZE, m_btnConstrainSize);
	DDX_Control(pDX, IDC_STATIC_DEFREE_X, m_stcDegreeX);
	DDX_Control(pDX, IDC_STATIC_DEFREE_Y, m_stcDegreeY);

	DDX_Control(pDX, IDC_EDIT_X, m_edtX);
	DDX_Control(pDX, IDC_EDIT_Y, m_edtY);
	DDX_Control(pDX, IDC_EDIT_W, m_edtW);
	DDX_Control(pDX, IDC_EDIT_H, m_edtH);
	DDX_Control(pDX, IDC_EDIT_W_SCALE, m_WScale);
	DDX_Control(pDX, IDC_EDIT_H_SCALE, m_HScale);
	DDX_Control(pDX, IDC_EDIT_X_SKEW, m_XSkew);
	DDX_Control(pDX, IDC_EDIT_Y_SKEW, m_YSkew);
}


BEGIN_MESSAGE_MAP(CEditMatrixPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CONSTRAIN_ANGLE, OnBnClickedButtonConstrainAngle)
	ON_BN_CLICKED(IDC_BUTTON_CONSTRAIN_SIZE, OnBnClickedButtonConstrainSize)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_X, OnEditModifyDimension)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_Y, OnEditModifyDimension)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_W, OnEditModifyDimension)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_H, OnEditModifyDimension)

	ON_CONTROL(EN_MODIFY, IDC_EDIT_W_SCALE, OnEditModifyWScale)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_H_SCALE, OnEditModifyHScale)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_X_SKEW, OnEditModifyXSkew)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_Y_SKEW, OnEditModifyYSkew)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEditMatrixPage message handlers

BOOL CEditMatrixPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_edtX.SetRange(1, -SHRT_MAX, SHRT_MAX, 0, FALSE, TRUE);
	m_edtY.SetRange(1, -SHRT_MAX, SHRT_MAX, 0, FALSE, TRUE);
	m_edtW.SetRange(1, 0.05, SHRT_MAX, 0, FALSE, TRUE);
	m_edtH.SetRange(1, 0.05, SHRT_MAX, 0, FALSE, TRUE);
	m_WScale.SetRange(1, 0.01, SHRT_MAX, 100.0, FALSE, TRUE);
	m_HScale.SetRange(1, 0.01, SHRT_MAX, 100.0, FALSE, TRUE);
	m_XSkew.SetRange(1, -360, 360, 0, FALSE, TRUE);
	m_YSkew.SetRange(1, -360, 360, 0, FALSE, TRUE);

	UpdateControls();

	if (AfxGetApp()->GetProfileInt("Property Pages\\InstanceMatrix", "Constrain Angle", TRUE) != 0)
	{
		m_btnConstrainAngle.SetCheck(BST_CHECKED);
	}
	if (AfxGetApp()->GetProfileInt("Property Pages\\InstanceMatrix", "Constrain Size", TRUE) != 0)
	{
		m_btnConstrainSize.SetCheck(BST_CHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMatrixPage::OnBnClickedButtonConstrainAngle()
{
	if (m_btnConstrainAngle.GetCheck())
		OnEditModifyXSkew();
}

void CEditMatrixPage::OnBnClickedButtonConstrainSize()
{
	if (m_btnConstrainSize.GetCheck())
		OnEditModifyWScale();
}

void CEditMatrixPage::OnEditModifyDimension()
{
	BOOL bTrans;
	int x = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_X, &bTrans) * 20);
	int y = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_Y, &bTrans) * 20);
	int w = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_W, &bTrans) * 20);
	int h = round((float)GetDlgItemFloat(m_hWnd, IDC_EDIT_H, &bTrans) * 20);
	SetDimension(x, y, w, h);
	UpdateControls();
}

void CEditMatrixPage::OnEditModifyMatrix()
{
	BOOL bTrans;
	float scalex = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_W_SCALE, &bTrans) / 100;
	float scaley = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_H_SCALE, &bTrans) / 100;
	float skewx = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_X_SKEW, &bTrans);
	float skewy = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_Y_SKEW, &bTrans);
	SetMatrix(scalex, scaley, skewx, skewy);
	UpdateControls();
}

void CEditMatrixPage::OnEditModifyWScale()
{
	if (m_btnConstrainSize.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_W_SCALE, strText);
		SetDlgItemText(IDC_EDIT_H_SCALE, strText);
	}

	OnEditModifyMatrix();
}

void CEditMatrixPage::OnEditModifyHScale()
{
	if (m_btnConstrainSize.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_H_SCALE, strText);
		SetDlgItemText(IDC_EDIT_W_SCALE, strText);
	}

	OnEditModifyMatrix();
}

void CEditMatrixPage::OnEditModifyXSkew()
{
	if (m_btnConstrainAngle.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_X_SKEW, strText);
		SetDlgItemText(IDC_EDIT_Y_SKEW, strText);
	}

	OnEditModifyMatrix();
}

void CEditMatrixPage::OnEditModifyYSkew()
{
	if (m_btnConstrainAngle.GetCheck() != 0)
	{
		CString strText;
		GetDlgItemText(IDC_EDIT_Y_SKEW, strText);
		SetDlgItemText(IDC_EDIT_X_SKEW, strText);
	}

	OnEditModifyMatrix();
}

void CEditMatrixPage::UpdateControls()
{
	SetDlgItemFloat(m_hWnd, IDC_EDIT_X, (double)m_x / 20, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_Y, (double)m_y / 20, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_W, (double)m_w / 20, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_H, (double)m_h / 20, 1, TRUE);

	SetDlgItemFloat(m_hWnd, IDC_EDIT_W_SCALE, m_scalex * 100, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_H_SCALE, m_scaley * 100, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_X_SKEW, m_skewx, 1, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_EDIT_Y_SKEW, m_skewy, 1, TRUE);
}

void CEditMatrixPage::SetMatrix(float scalex, float scaley, float skewx, float skewy)
{
	if (fabsf(scalex) < 0.01f) scalex = 0.01f;
	if (fabsf(scaley) < 0.01f) scaley = 0.01f;

	gld_shape	shape = (gld_shape)CTraitShape(m_pInstance);
	gld_point   op = shape.trans_origin();
	int			ox = op.x;
	int			oy = op.y;
	m_matrix.Transform(ox, oy);

	m_matrix.SetTransformParam(scalex, scaley, skewx, skewy, 0, 0);			
	m_matrix.Transform(op.x, op.y);
	m_matrix.m_dx = ox - op.x;
	m_matrix.m_dy = oy - op.y;

	CGuardDrawOnce xDrawOnce;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
	}
	m_pCmd = new CCmdModifyMatrix(m_pInstance, m_matrix);
	m_pCmd->Execute();

	RecalcTransformParam(m_x, m_y, m_w, m_h, m_scalex, m_scaley, m_skewx, m_skewy, CTraitShape(m_pInstance), m_matrix);
}

void CEditMatrixPage::SetDimension(int x, int y, int w, int h)
{
	AfxLimitValue(w, IDI_MIN_SHAPE_WIDTH, IDI_MAX_SHAPE_WIDTH);
	AfxLimitValue(h, IDI_MIN_SHAPE_HEIGHT, IDI_MAX_SHAPE_HEIGHT);

	gld_shape shape = (gld_shape)CTraitShape(m_pInstance);
	gld_rect	box = _transform_rect(shape.bound(), m_matrix);

	int ox, oy;

	gld_point op = shape.trans_origin();
	ox = op.x;
	oy = op.y;
	TMatrix   smat = m_matrix;
	smat.Transform(ox, oy);

	int ow = box.width();
	int oh = box.height();

	TMatrix	scalemat, movemat;
	scalemat.SetScaleMatrix((float)w / ow, (float)h / oh, ox, oy);
	movemat.SetMoveMatrix(x - ox, y - oy);
	TMatrix	mat = scalemat * movemat;

	m_matrix = m_matrix * mat;

	CGuardDrawOnce xDrawOnce;

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
	}
	m_pCmd = new CCmdModifyMatrix(m_pInstance, m_matrix);
	m_pCmd->Execute();

	RecalcTransformParam(m_x, m_y, m_w, m_h, m_scalex, m_scaley, m_skewx, m_skewy, CTraitShape(m_pInstance), m_matrix);
}

void CEditMatrixPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	AfxGetApp()->WriteProfileInt("Property Pages\\InstanceMatrix", "Constrain Angle", m_btnConstrainAngle.GetCheck());
	AfxGetApp()->WriteProfileInt("Property Pages\\InstanceMatrix", "Constrain Size", m_btnConstrainSize.GetCheck());
}
