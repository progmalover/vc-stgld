#pragma once

#include "Command.h"
#include <list>
#include "TextToolEx.h"
#include "toolsdef.h"
#include "FontComboBox.h"
#include "NumberEdit.h"
#include "StaticSpinButton.h"
#include "FlatBitmapButton.h"
#include "FlatColorButton.h"
#include "LineWidthEdit.h"

class gldCharacterKey;
// CEditMCEditTextDialog dialog

class CEditMCEditTextDialog : public CDialog
{
	DECLARE_DYNAMIC(CEditMCEditTextDialog)

	BOOL m_bInit;
public:
	CEditMCEditTextDialog(std::list<gldCharacterKey*>& keys, gldText2* pText);   // standard constructor
	virtual ~CEditMCEditTextDialog();

// Dialog Data
	enum { IDD = IDD_EDIT_MC_EDIT_TEXT_DIALOG };

	TCommandGroup* m_pCmd;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnTextChanged();

	DECLARE_MESSAGE_MAP()

	std::list<gldCharacterKey*>& m_keys;
	gldText2* m_pText;

	// Controls
	CSmartEdit m_edtText;


	CFontComboBox m_cmbFont;
	CFlatColorButton m_btnColor;
	CNumberEdit m_edtSize;
	CStaticSpinButton m_btnSize;

	CFlatBitmapButton m_btnBold;
	CFlatBitmapButton m_btnItalic;

	CNumberEdit m_edtCharSpacing;
	CStaticSpinButton m_btnCharSpacing;

	CNumberEdit m_edtOffsetX;
	CStaticSpinButton m_btnOffsetX;

	CNumberEdit m_edtOffsetY;
	CStaticSpinButton m_btnOffsetY;

	CLineWidthEdit m_edtTextWidth;
	CStaticSpinButton m_btnTextWidth;
	CLineWidthEdit m_edtTextHeight;
	CStaticSpinButton m_btnTextHeight;
public:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnEditChangeFont();
	afx_msg void OnColorChanged();
	afx_msg void OnSizeModified();
	afx_msg void OnBold();
	afx_msg void OnItalic();
	afx_msg void OnCharSpacingModified();
	afx_msg void OnOffsetXModified();
	afx_msg void OnOffsetYModified();
	afx_msg void OnButtonText();
	afx_msg void OnTextSizeModified();
public:
	virtual INT_PTR DoModal();
	afx_msg void OnBnClickedRadioKeepCurrentSize();
	afx_msg void OnBnClickedRadioAutoCalculate();
	afx_msg void OnBnClickedRadioSpecify();
	void UpdateUI();
};