#pragma once

#include "ResizableToolTipDialog.h"
#include "FontPreviewCombo.h"
#include "flatcolorbutton.h"
#include "flatbitmapbutton.h"
#include "staticspinbutton.h"
#include "numberedit.h"
#include "staticlink.h"
#include "shockwaveflash.h"
#include "HierarchyDropdown.h"
#include "Glanda_i.h"
#include "MoviePreview.h"
#include "HistoryComboBox.h"

// CDlgInsertText dialog

class CDlgInsertText : public CResizableToolTipDialog
{	
public:
	CDlgInsertText(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInsertText();

// Dialog Data
	enum { IDD = IDD_INSERT_TEXT };

private:
	CSmartEdit m_edtText;
	CFlatColorButton m_btnColor;
	CFontPreviewCombo m_cmbFont;
	CNumberEdit m_edtSize;
	CStaticSpinButton m_ssbSize;	
	CFlatBitmapButton m_btnBold;
	CFlatBitmapButton m_btnItalic;
	CNumberEdit m_edtSpace;
	CStaticSpinButton m_ssbSpace;
	CHierarchyDropdown m_dropEffect;
	CFlatBitmapButton m_btnConfig;
	CShockwaveFlash m_player;
	CStaticLink m_sttTextTool;
	CHistoryComboBox m_cmbURL;
	CString m_strTarget;
	CString m_strURL;

	CString m_text;
	CString m_face;
	COLORREF m_color;
	int m_size;
	int m_space;
	bool m_bold;
	bool m_italic;
	CComPtr<IGLD_Parameters> m_options;
	CMoviePreview m_preview;
	CString m_strPreviewFile;
	bool m_init;

	void UpdateControls();	
	void Preview();
	void FillEffectCombo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnTextChanged();
	void Reposition();
	virtual void OnOK();
	void LoadState();
	void SaveState();

	static LPCTSTR REG_KEY;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnBold();
	afx_msg void OnBnClickedBtnItalic();
	afx_msg void OnTextModified();
	afx_msg void OnSizeModified();
	afx_msg void OnSpaceModified();
	afx_msg void OnCbnSelchangeComboFont();
	afx_msg void OnBnClickedBtnConfig();
	afx_msg void OnColorChanged();
	afx_msg void OnEffectSelChanged();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedTextTool();
};
