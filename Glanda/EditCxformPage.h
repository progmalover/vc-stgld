#pragma once
#include "FlatColorButton.h"
#include "NumberEdit.h"
#include "StaticSpinButton.h"
#include "gldInstance.h"
#include "TransAdaptor.h"


class CCmdModifyCxform;

// CEditCxformPage dialog

class CEditCxformPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditCxformPage)

public:
	CEditCxformPage(gldInstance* pInstance);
	virtual ~CEditCxformPage();

// Dialog Data
	enum { IDD = IDD_EDIT_CXFORM_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CFlatColorButton m_btnColor;
	CStatic m_stcColorPercent;
	CStaticSpinButton m_btnColorFactor;
	CNumberEdit m_edtRFactor;
	CNumberEdit m_edtGFactor;
	CNumberEdit m_edtBFactor;
	CNumberEdit m_edtAFactor;

	CNumberEdit m_edtR;
	CNumberEdit m_edtG;
	CNumberEdit m_edtB;
	CNumberEdit m_edtA;

	CStaticSpinButton m_btnRFactor;
	CStaticSpinButton m_btnGFactor;
	CStaticSpinButton m_btnBFactor;
	CStaticSpinButton m_btnAFactor;

	CStaticSpinButton m_btnR;
	CStaticSpinButton m_btnG;
	CStaticSpinButton m_btnB;
	CStaticSpinButton m_btnA;

	CNumberEdit m_edtColorFactor;
	CNumberEdit m_edtAlphaFactor;

	CStaticSpinButton m_btnAlphaFactor;
	CStatic m_stcAlphaPercent;
public:
	virtual BOOL OnInitDialog();
	COLORREF m_cxcolor;
	TCxform m_cxform;
	COLOR_TRANS_STYLE m_cxstyle;

	gldInstance* m_pInstance;

	CCmdModifyCxform* m_pCmd;

	afx_msg void OnBnClickedColorTransNone();
	afx_msg void OnBnClickedColorTransFadeToAlpha();
	afx_msg void OnBnClickedColorTransFadeToColor();
	afx_msg void OnBnClickedColorTransAdvanced();
	afx_msg void OnModifyNone();
	afx_msg void OnModifyAlphaFactor();
	afx_msg void OnModifyColor();
	afx_msg void OnModifyAdvanced();

	void UpdateControls(BOOL bUpdateAll = FALSE);
	void ShowColorStyleAlphaControls(BOOL bShow);
	void ShowColorTransStyleColorControls(BOOL bShow);
	void ShowColorStyleAdvancedControls(BOOL bShow);
};
