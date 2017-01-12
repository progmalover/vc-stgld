#if !defined(AFX_DLGCOLORADVANCED_H__8B889661_69D5_11D5_9A19_0080C82BC2DE__INCLUDED_)
#define AFX_DLGCOLORADVANCED_H__8B889661_69D5_11D5_9A19_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColorAdvanced.h : header file
//

#include "Dib.h"
#include "ColorPicker.h"
#include "ColorPickerH.h"
#include "ColorPickerSV.h"
#include "PopupSliderCtrl.h"	// Added by ClassView
#include "StaticSpinButton.h"
#include "StaticColorAlpha.h"
#include "ColorSwatch.h"
#include "hexcoloredit.h"

extern UINT WM_COLORCHANGE;

/////////////////////////////////////////////////////////////////////////////
// CDlgColorAdvanced dialog

class CDlgColorAdvanced : public CDialog
{
// Construction
public:
	void EnableAlpha(BOOL bEnable);
	int GetAlpha();
	COLORREF GetColor();
	void SetColor(COLORREF color, int alpha);
	void LoadOldColor();
	CDlgColorAdvanced(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorAdvanced)
	enum { IDD = IDD_COLOR_ADVANCED };
	CStaticSpinButton	m_btnA;
	CStaticColorAlpha	m_stcColorNew;
	CStaticColorAlpha	m_stcColorOld;
	CStaticSpinButton	m_btnB;
	CStaticSpinButton	m_btnG;
	CStaticSpinButton	m_btnR;
	CStaticSpinButton	m_btnV;
	CStaticSpinButton	m_btnS;
	CStaticSpinButton	m_btnH;
	CColorPickerSV	m_stcSV;
	CColorPickerH	m_stcH;
	UINT	m_nHValue;
	UINT	m_nSValue;
	UINT	m_nVValue;
	UINT	m_nRValue;
	UINT	m_nGValue;
	UINT	m_nBValue;
	UINT	m_nAValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorAdvanced)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bEnableAlpha;
	COLORREF m_crColorOld;
	int m_nAlphaOld;
	void UpdateColorSample();
	void SyncHSVValue();
	void SyncRGBValue();
	void SyncHEXValue();
	void UpdateH();
	void UpdateSV();
	BOOL m_bInit;
	void UpdateEdit();
	// Generated message map functions
	//{{AFX_MSG(CDlgColorAdvanced)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditChangeH();
	afx_msg void OnEditChangeSV();
	afx_msg void OnEditChangeRGB();
	afx_msg void OnEditChangeA();
	afx_msg void OnColorOld();
	//}}AFX_MSG
	afx_msg void OnPaletteChangeH();
	afx_msg void OnPaletteChangeSV();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAddToSwatch();
	afx_msg void OnEditChangeHex();
	CHexColorEdit m_edtHex;
	CString m_strHEXValue;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORADVANCED_H__8B889661_69D5_11D5_9A19_0080C82BC2DE__INCLUDED_)
