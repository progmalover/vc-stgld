#pragma once

#define EN_MODIFY		1000


// CSmartEdit

class CSmartEdit : public CEdit
{
	DECLARE_DYNAMIC(CSmartEdit)

public:
	CSmartEdit();
	virtual ~CSmartEdit();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bPrompt;
	BOOL m_bChanged;
	virtual BOOL ValidateText();
	virtual void SaveDefaultValue();
public:
	virtual BOOL HandleModified();
	// Return a value between nMin and nMax or return nEmptyValue if it is empty and allowed empty
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEnUpdate();
	afx_msg LRESULT OnSetText(WPARAM wp, LPARAM lp);
};
