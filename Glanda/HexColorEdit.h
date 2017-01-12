#pragma once


// CHexColorEdit

class CHexColorEdit : public CEdit
{
	DECLARE_DYNAMIC(CHexColorEdit)

public:
	CHexColorEdit();
	virtual ~CHexColorEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnUpdate();
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetSel(WPARAM wParam, LPARAM lParam);

protected:
	CString m_strText;
	DWORD m_dwSel;
	BOOL m_bInternalSetWindowText;
	BOOL ValidateText(LPCTSTR lpszText);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


