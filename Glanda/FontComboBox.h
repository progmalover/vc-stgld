#pragma once

#include "SmartComboBox.h"
#include "DlgFontPreview.h"

// class CFontComboBoxEdit
class CFontComboBoxEdit : public CSmartEdit
{
public:
	CFontComboBoxEdit()
	{
		m_pComboBox = NULL;
	}
	virtual ~CFontComboBoxEdit(){}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	CComboBox *m_pComboBox;
};

/////////////////////////////////////////////////////////////////////////////
// CFontComboBox window

class CFontComboBox : public CSmartComboBox
{
// Construction
public:
	CFontComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL EnumFonts();
	void SetPreviewParameters(int nSize, BOOL bBold, BOOL bItalic);
	virtual ~CFontComboBox();

	// Generated message map functions
protected:
	static BOOL CALLBACK AFX_EXPORT EnumFamScreenCallBackEx(
		ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis);
	//{{AFX_MSG(CFontComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	void PreviewCurrentFont();

	CFontComboBoxEdit m_edit;
	BOOL m_bDroppedDown;
	CDlgFontPreview m_dlgPreview;
	int m_nCurSelTrascking;
	int m_nSize;
	BOOL m_bBold;
	BOOL m_bItalic;

public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnCbnCloseup();
	afx_msg BOOL OnCbnDropdown();
};
