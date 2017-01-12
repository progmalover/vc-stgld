#pragma once
#include "FlatColorButton.h"
#include "StaticFillStyle.h"
#include "ImageListCtrl.h"
#include "StaticSpinButton.h"
#include "ColorPicker.h"
#include "ColorPickerSV.h"
#include "ColorPickerH.h"
#include "HexColorEdit.h"
#include "FlatBitmapButton.h"
#include "StaticLinearGradientFill.h"
#include "NumberEdit.h"
#include "gldFillStyle.h"
#include "FileDialogEx.h"
#include "gldInstance.h"
#include "CmdModifyFillStyle.h"
#include "StaticBitmap.h"

// CEditShapeFillPage dialog

class TCommandGroup;
class CCmdModifyFillStyle;

class CEditShapeFillPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditShapeFillPage)

public:
	CEditShapeFillPage(gldObj* pObj);
	virtual ~CEditShapeFillPage();

// Dialog Data
	enum { IDD = IDD_EDIT_SHAPE_FILL_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	

protected:
	CFlatColorButton m_btnFillColor;
	CComboBox m_cmbFillStyle;
	CStaticSpinButton	m_btnA;
	CStaticFillStyle	m_stcFillStyle;
	CStaticSpinButton	m_btnB;
	CStaticSpinButton	m_btnG;
	CStaticSpinButton	m_btnR;
	CStaticSpinButton	m_btnV;
	CStaticSpinButton	m_btnS;
	CStaticSpinButton	m_btnH;
	CColorPickerSV	m_stcSV;
	CColorPickerH	m_stcH;
	CHexColorEdit m_edtHex;

	CImageListCtrl m_lstImage;
	CFlatBitmapButton m_btnImportImage;
	CFlatBitmapButton m_btnDeleteImage;


	CNumberEdit m_edtH;
	CNumberEdit m_edtS;
	CNumberEdit m_edtV;
	CNumberEdit m_edtR;
	CNumberEdit m_edtG;
	CNumberEdit m_edtB;
	CNumberEdit m_edtA;

	UINT	m_nHValue;
	UINT	m_nSValue;
	UINT	m_nVValue;
	UINT	m_nRValue;
	UINT	m_nGValue;
	UINT	m_nBValue;
	UINT	m_nAValue;

	CString m_strHEXValue;
	CStaticLinearGradientFill m_stcLinearGradientFill;


	BOOL m_bInit;
	int m_nColorMode;

	gldObj* m_pObj;
public:
	// Fill Matrix
	CButton m_btnConstrainAngle;
	CButton m_btnConstrainSize;

	CStaticBitmap m_stcDegreeX;
	CStaticBitmap m_stcDegreeY;

	CNumberEdit m_edtCoorX;
	CNumberEdit m_edtCoorY;
	CNumberEdit m_edtWidth;
	CNumberEdit m_edtHeight;
	CNumberEdit m_ScaleW;
	CNumberEdit m_ScaleH;
	CNumberEdit m_SkewX;
	CNumberEdit m_SkewY;


	TMatrix m_matrix;
	int m_nCoorX;
	int m_nCoorY;
	int m_nWidth;
	int m_nHeight;
	float m_fScaleX;
	float m_fScaleY;
	float m_fSkewX;
	float m_fSkewY;

public:
	std::map<int, CCmdModifyFillStyle*> m_CmdModifyFillStyleList;
	TCommandGroup* m_pCmdImportImages;
	TCommandGroup* m_pCmdDeleteImages;
	static BOOL HasFills(gldObj* pObj);

public:
	void SetColorMode(int nMode);
	int GetColorMode(){return m_nColorMode;}

	int GetAlpha();
	COLORREF GetColor();
	void SetColor(COLORREF color, int alpha);

	void HandleFillTypeChange();
	TFillStyle::FillStyleType GetFillStyleType();

	void SyncHSVValue();
	void SyncRGBValue();
	void SyncHEXValue();

	void UpdatePalette();
	void UpdateColorButton();
	void UpdateGradientFillSample();
	void UpdateColorSample();
	void UpdateAllControls();
	TFillStyle *GetFillStyle();
	void HandleFillSelChange();
	void SetFillType(int type);

	TFillStyle* GetCurSelFill();
	void UpdateSelectedShapes();
	afx_msg void OnLbnSelchangeFillList();
	afx_msg void OnFillcolorChanged();
	afx_msg void OnEditChangeHSV();
	afx_msg void OnEditChangeRGB();
	afx_msg void OnEditChangeA();
	afx_msg void OnEditChangeHex();
	afx_msg void OnPaletteChangeH();
	afx_msg void OnPaletteChangeSV();
	afx_msg void OnCbnSelchangeComboFillType();
	afx_msg void OnGradientFillCurrentHandleChanged();
	afx_msg void OnGradientFillHandlePosChanged();
	afx_msg void OnImageListSelChanged();
	afx_msg void OnImportImage();
	afx_msg void OnDeleteImage();
	gldImage* ImportImage();
	afx_msg void OnDestroy();	

	public:

	afx_msg void OnEditModifyDimension();
	afx_msg void OnEditModifyMatrix();

	afx_msg void OnEditModifyWScale();
	afx_msg void OnEditModifyHScale();
	afx_msg void OnEditModifyXSkew();
	afx_msg void OnEditModifyYSkew();
	afx_msg void OnBnClickedButtonConstrainAngle();
	afx_msg void OnBnClickedButtonConstrainSize();

	void UpdateMatrixControls();
	void SetDimension(int x, int y, int w, int h);
	void SetMatrix(float scalex, float scaley, float skewx, float skewy);

};
