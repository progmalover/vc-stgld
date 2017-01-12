#pragma once

#include "Observer.h"
#include "dlgtooloption.h"
#include "flatcolorbutton.h"
#include "flatbitmapbutton.h"
#include "fontcombobox.h"
#include "staticspinbutton.h"
#include "numberedit.h"
#include "staticlink.h"

#include "global.h"
#include "staticbitmap.h"
#include "ToolsDef.h"


// CDlgTextTool dialog

class CDlgTextTool : public CDlgToolOption
{
public:
	CDlgTextTool(UINT nIDTemplate, CWnd *pParentWnd = 0);
	virtual ~CDlgTextTool();

	// Dialog Data
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CFlatColorButton m_btnColor;
	CFlatBitmapButton m_btnBold;
	CFlatBitmapButton m_btnItalic;
	CStaticBitmap m_stcCharSpacing;
	CStaticBitmap m_stcLineSpacing;
	CNumberEdit m_edtSize;

	CNumberEdit m_edtCharSpacing;
	CNumberEdit m_edtLineSpacing;

	CStaticSpinButton m_btnCharSpacing;
	CStaticSpinButton m_btnLineSpacing;

	CFlatBitmapButton m_btnAlignLeft;
	CFlatBitmapButton m_btnAlignCenter;
	CFlatBitmapButton m_btnAlignRight;

	CFlatBitmapButton m_btnSelectable;
	CFlatBitmapButton m_btnBorder;
	CFlatBitmapButton m_btnEditable;

	CStaticBitmap m_stcIndent;
	CStaticBitmap m_stcLeftMargin;
	CStaticBitmap m_stcRightMargin;
	CNumberEdit m_edtIndent;
	CNumberEdit m_edtLeftMargin;
	CNumberEdit m_edtRightMargin;
	CStaticSpinButton m_btnIndent;
	CStaticSpinButton m_btnLeftMargin;
	CStaticSpinButton m_btnRightMargin;

	CStaticBitmap m_stcCharPos;

	CStaticBitmap m_stcLink;
	CSmartEdit m_edtLink;
	CSmartComboBox m_cmbTarget;
	CSmartEdit m_edtVariable;
	CNumberEdit m_edtMaxLength;
	CComboBox m_cmbMultiline;

	CStaticLink m_stcAdvOptions;

	virtual BOOL OnInitDialog();

public:
	afx_msg void OnColorChanged();
	afx_msg void OnBold();
	afx_msg void OnItalic();
	CFontComboBox m_cmbFont;
	CStaticSpinButton m_btnSize;
	afx_msg void OnEditChangeFont();
	afx_msg void OnSizeModified();
	afx_msg void OnCharSpacingModified();
	afx_msg void OnLineSpacingModified();


	virtual void UpdateControls();
	void HandleSelChange();
	void UpdateAdvOptions();

public:
	afx_msg void OnBnClickedButtonAlignLeft();
	afx_msg void OnBnClickedButtonAlignCenter();
	afx_msg void OnBnClickedButtonAlignRight();

	afx_msg void OnIndentModified();
	afx_msg void OnLeftModified();
	afx_msg void OnRightMarginModified();
	CComboBox m_cmbCharPos;
	afx_msg void OnCbnSelchangeComboCharPos();
	CComboBox m_cmbTextType;
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedButtonBorder();

	afx_msg void OnLinkModified();
	afx_msg void OnEditChangeTarget();
	afx_msg void OnBnClickedButtonSelectable();

	afx_msg void OnVariableModified();
	afx_msg void OnBnClickedButtonEditable();

	afx_msg void OnCbnChangeMultiline();
	afx_msg void OnEditChangeMaxLength();

	afx_msg void OnBnClickedAdvOptions();

	BOOL m_bShowAdvOptions;

	virtual SETextToolEx* GetTextTool() = 0;
	virtual void OnTextChanged() = 0;
};
