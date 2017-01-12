#pragma once
#include "StaticBitmap.h"
#include "NumberEdit.h"
#include "gldInstance.h"
#include "TransAdaptor.h"

// CEditMatrixPage dialog

template <class _Tx>
void AfxLimitValue(_Tx &v, const _Tx &vmin, const _Tx &vmax)
{
	if (v < vmin)
	{
		v = vmin;
	}
	else if (v > vmax)
	{
		v = vmax;
	}
}


class CCmdModifyMatrix;

void RecalcTransformParam(int& x, int& y, int& w, int& h, float& scalex, float& scaley, float& skewx, float& skewy, gld_shape shape, TMatrix& matrix);


class CEditMatrixPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditMatrixPage)

public:
	CEditMatrixPage(gldInstance* pInstance);
	virtual ~CEditMatrixPage();

// Dialog Data
	enum { IDD = IDD_EDIT_MATRIX_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CButton m_btnConstrainAngle;
	CButton m_btnConstrainSize;

	CStaticBitmap m_stcDegreeX;
	CStaticBitmap m_stcDegreeY;

	CNumberEdit m_edtX;
	CNumberEdit m_edtY;
	CNumberEdit m_edtW;
	CNumberEdit m_edtH;
	CNumberEdit m_WScale;
	CNumberEdit m_HScale;
	CNumberEdit m_XSkew;
	CNumberEdit m_YSkew;
public:
	virtual BOOL OnInitDialog();
	void SetMatrix(float scalex, float scaley, float skewx, float skewy);
	void SetDimension(int x, int y, int w, int h);

	TMatrix m_matrix;
	int m_x;
	int m_y;
	int m_w;
	int m_h;
	float m_scalex;
	float m_scaley;
	float m_skewx;
	float m_skewy;
	gldInstance* m_pInstance;
	CCmdModifyMatrix* m_pCmd;

	afx_msg void OnEditModifyDimension();
	afx_msg void OnEditModifyMatrix();

	afx_msg void OnEditModifyWScale();
	afx_msg void OnEditModifyHScale();
	afx_msg void OnEditModifyXSkew();
	afx_msg void OnEditModifyYSkew();
	afx_msg void OnBnClickedButtonConstrainAngle();
	afx_msg void OnBnClickedButtonConstrainSize();

	void UpdateControls();
	afx_msg void OnDestroy();
};
