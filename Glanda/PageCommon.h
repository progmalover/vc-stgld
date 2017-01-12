#pragma once

#include "GLD_PropertyPage.h"
#include "FlatColorButton.h"
#include "StaticSpinButton.h"
#include "ColorPicker.h"
#include "ColorPickerSV.h"
#include "ColorPickerH.h"
#include "StaticLinearGradientFill.h"
#include "gldFillStyle.h"
#include "StaticBitmap.h"
#include "NumberEdit.h"
#include "StaticFillStyle.h"
#include "HexColorEdit.h"
#include "LineWidthEdit.h"
#include "StaticShapePreview.h"
#include "TextStyleListCtrl.h"
#include "FlatShapeButton.h"
#include "FlatBitmapButton.h"

class TFillStyle;
// CPageCommon : Property page dialog

class CPageCommon : public CGLD_PropertyPage
{
	DECLARE_DYNCREATE(CPageCommon)
	DECLARE_OLECREATE_EX(CPageCommon)

// Constructors
public:
	CPageCommon();

// Dialog Data
	enum { IDD = IDD_PAGE_COMMON };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()

protected:

	//CTextStyleData m_textStyle;
	BOOL m_bBreakApart;
	BOOL m_bTextBorder;
	float m_fBorderWidth;
	COLORREF m_clrBorderColor;
	BOOL m_bAddShadow;
	float m_fShadowSize;
	COLORREF m_clrShadowColor;
	BOOL m_bCustomFill;
	BOOL m_bFillIndividually;
	int m_nAngle;
	CAutoPtr<TFillStyle>	m_ptFillStyle;
	CAutoPtr<gldShape>		m_pgPreviewText;

	BOOL m_bAutoPlay;
	BOOL m_bLoop;

	CFlatColorButton m_btnShadowColor;
	CComboBox m_cmbFillStyleType;
	CFlatColorButton m_btnFillColor;
	CColorPickerSV	m_stcSV;
	CColorPickerH	m_stcH;
	CStaticLinearGradientFill m_stcLinearGradientFill;
	//CTextStyleListCtrl m_textStyleListCtrl;

	virtual BOOL OnApply();

	COLORREF m_clrFillColor;
	int m_nFillAlpha;

	CStaticShapePreview m_stcTextPreview;
	CFlatShapeButton m_btnTextStyles[6];
	int m_nSelButton;
public:
	virtual BOOL OnInitDialog();
private:
	void LoadFillStyle(TFillStyle* ptFillStyle);
	void SetFillType(int type);
	void UpdateFillControlsUI();
	void UpdateAllControls(BOOL bUpdatePalette = TRUE);
	void UpdatePalette();
	void UpdateColorButton();
	void UpdateGradientFillSample();
	void UpdateControlsUI();
	void HandleFillStyleChange();
	void UpdatePreviewText();

	int GetFillAlpha();
	COLORREF GetFillColor();
	void SetFillColor(COLORREF color, int alpha, BOOL bUpdatePalette = TRUE);

	CFlatColorButton m_btnBorderColor;
	CLineWidthEdit m_edtBorderWidth;
	CLineWidthEdit m_edtShadowSize;
	CNumberEdit m_edtFillAngle;
	CStaticBitmap m_stcDefreeAngle;
	CStaticSpinButton m_btnFillAngle;
	CStaticSpinButton m_btnBorderWidth;
	CStaticSpinButton m_btnShadowSize;

	int m_nOldFillType;

public:
	afx_msg void OnBnClickedCheckBreakApart();
	afx_msg void OnBnClickedCheckTextBorder();
	afx_msg void OnBnClickedCheckAddShadow();
	afx_msg void OnBnClickedCheckCustomFill();
	afx_msg void OnCbnSelendokComboFillType();
	afx_msg void OnFillcolorChanged();
	afx_msg void OnGradientFillCurrentHandleChanged();
	afx_msg void OnGradientFillHandlePosChanged();
	afx_msg void OnPaletteChangeH();
	afx_msg void OnPaletteChangeSV();
	afx_msg void OnBorderWidth();
	afx_msg void OnShadowSize();
	afx_msg void OnBorderColor();
	afx_msg void OnShadowColor();
	afx_msg void OnBnClickedCheckFillIndividually();
	afx_msg void OnAngle();
	afx_msg void OnBnClickedCheckAutoPlay();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonTextStyle1();
	afx_msg void OnBnClickedButtonTextStyle2();
	afx_msg void OnBnClickedButtonTextStyle3();
	afx_msg void OnBnClickedButtonTextStyle4();
	afx_msg void OnBnClickedButtonTextStyle5();
	afx_msg void OnBnClickedButtonTextStyle6();
	void ApplyTextStyle(CTextStyleData* pTextStyle);
	afx_msg void OnTextStyleApplyStyle();
	afx_msg void OnTextStyleSaveStyle();
	afx_msg void OnUpdateTextStyleSaveStyle(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
