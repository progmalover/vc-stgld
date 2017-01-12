#pragma once

#include "SmartEdit.h"

// CNumberEdit

class CNumberEdit : public CSmartEdit
{
	DECLARE_DYNAMIC(CNumberEdit)

public:
	CNumberEdit();
	virtual ~CNumberEdit();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	int m_nDefault;
	int m_nMin;
	int m_nMax;

	int m_nPrecision;
	double m_fDefault;
	double m_fMin;
	double m_fMax;

	int m_nType;
	BOOL m_bAllowEmpty;
	CString m_strError;

	BOOL ValidateText();
	void ReportError(UINT fmt, ...);

public:
	// Return a value between nMin and nMax or return nEmptyValue if it is empty and allowed empty
	void SaveDefaultValue();
	int GetValueInt();
	void SetRange(int nMin, int nMax, int nDefault, BOOL bAllowEmpty, BOOL bPrompt);
	double GetValueFloat();
	void SetRange(int nPrecision, double fMin, double fMax, double fDefault, BOOL bAllowEmpty, BOOL bPrompt);
	afx_msg LRESULT OnSliderPosChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSliderEndTracking(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnNumberEditError(WPARAM wp, LPARAM lp);
};

void RemoveFloatTrailZero(CString &strText);
double GetDlgItemFloat(HWND hDlg, int nID, BOOL *pTranslated);
void SetDlgItemFloat(HWND hDlg, int nID, double fValue, int nPrecision, BOOL bRemoveTrailZero);
