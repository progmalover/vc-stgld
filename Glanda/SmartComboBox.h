#pragma once

#define CBN_MODIFY		1000

#include "SmartEdit.h"

void AFXAPI DDX_CBString2(CDataExchange* pDX, int nIDC, CString& value);

// CSmartComboBox

class CSmartComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CSmartComboBox)

public:
	CSmartComboBox();
	virtual ~CSmartComboBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnCbnSetfocus();
	afx_msg BOOL OnCbnSelchange();
	afx_msg BOOL OnCbnCloseup();
protected:
	CSmartEdit m_edit;
	CString m_strText;
public:
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	void GetText(CString & strText);
	afx_msg LRESULT CSmartComboBox::OnSetText(WPARAM wParam, LPARAM lParam);
protected:
	virtual void PreSubclassWindow();
	BOOL IsDropDownList();
};

int GetComboBoxMaxItemWidth(CComboBox *pCB);
