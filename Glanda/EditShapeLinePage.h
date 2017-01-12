#pragma once
#include "FlatColorButton.h"
#include "StaticFillStyle.h"
#include "StaticSpinButton.h"
#include "ColorPickerSV.h"
#include "ColorPickerH.h"
#include "HexColorEdit.h"
#include "LineWidthEdit.h"
#include "gldLineStyle.h"
#include "gldInstance.h"
#include "CmdModifyLineStyle.h"

// CEditShapeLinePage dialog

class CEditShapeLinePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditShapeLinePage)

public:
	CEditShapeLinePage(gldObj* m_pObj);
	virtual ~CEditShapeLinePage();

// Dialog Data
	enum { IDD = IDD_EDIT_SHAPE_LINE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	

protected:
	CFlatColorButton m_btnLineColor;
	CStaticSpinButton	m_btnA;
	CStaticFillStyle	m_stcFillStyle;
	CStaticSpinButton	m_btnB;
	CStaticSpinButton	m_btnG;
	CStaticSpinButton	m_btnR;
	CStaticSpinButton	m_btnV;
	CStaticSpinButton	m_btnS;
	CStaticSpinButton	m_btnH;
	CStaticSpinButton	m_btnLineWidth;
	CColorPickerSV	m_stcSV;
	CColorPickerH	m_stcH;
	CHexColorEdit m_edtHex;

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
	CLineWidthEdit m_edtLineWidth;
	float m_fLineWidth;

	BOOL m_bInit;
	int m_nColorMode;

	gldObj* m_pObj;

public:
	std::map<int, CCmdModifyLineStyle*> m_CmdModifyLineStyleList;

	static BOOL HasLines(gldObj* pObj);

public:
	void SetColorMode(int nMode);
	int GetColorMode(){return m_nColorMode;}

	int GetAlpha();
	COLORREF GetColor();
	void SetColor(COLORREF color, int alpha);

	void HandleLineSelChange();
	void SyncHSVValue();
	void SyncRGBValue();
	void SyncHEXValue();

	void UpdatePalette();
	void UpdateColorButton();
	void UpdateColorSample();
	void UpdateAllControls();

	TLineStyle* GetCurSelLine();

	void UpdateSelectedShapes();
	afx_msg void OnLbnSelchangeLineList();
	afx_msg void OnLinecolorChanged();
	afx_msg void OnEditChangeHSV();
	afx_msg void OnEditChangeRGB();
	afx_msg void OnEditChangeA();
	afx_msg void OnEditChangeLineWidth();
	afx_msg void OnEditChangeHex();
	afx_msg void OnPaletteChangeH();
	afx_msg void OnPaletteChangeSV();
	afx_msg void OnDestroy();
};
