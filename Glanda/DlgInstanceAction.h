#pragma once

#include "ResizableToolTipDialog.h"
#include "HistoryComboBox.h"
#include "flatbitmapbutton.h"

class CASView;
class TCommand;
class gldInstance;

// CDlgInstanceAction dialog

class CDlgInstanceAction : public CResizableToolTipDialog
{
public:
	CDlgInstanceAction();
	virtual ~CDlgInstanceAction();

// Dialog Data
	enum { IDD = IDD_INSTANCE_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CHistoryComboBox m_cmbURL;
	CComboBox m_cmbScenes;
	CString m_strURL;
	CString m_strTarget;
	CString m_strScene;
	CFlatBitmapButton m_btnCheckSyntax;
	CFlatBitmapButton m_btnMemberList;
	CFlatBitmapButton m_btnParameterInfo;
	CFlatBitmapButton m_btnAutoComplete;

	BOOL CheckSyntax(LPCTSTR lpszCode);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedRadioNone();
	afx_msg void OnBnClickedRadioGetUrl();
	afx_msg void OnBnClickedRadioGotoScene();
	afx_msg void OnBnClickedRadioPlay();
	afx_msg void OnBnClickedRadioStop();
	afx_msg void OnBnClickedRadioCustom();
	afx_msg void OnBnClickedRadioPrevScene();
	afx_msg void OnBnClickedRadioNextScene();

	afx_msg void OnBnClickedButtonMemberList();
	afx_msg void OnBnClickedButtonParameterInfo();
	afx_msg void OnBnClickedButtonAutoComplete();
	afx_msg void OnBnClickedButtonCheckSyntax();

public:
	CASView *m_pView;
	CString *m_pstrAction;

	BOOL UpdateAction();
	void ParseAction();
	void UpdateControls();
};
