#pragma once


// CAutoCompletionComboBoxEdit

class CAutoCompletionComboBoxEdit : public CEdit
{
	DECLARE_DYNAMIC(CAutoCompletionComboBoxEdit)

public:
	CAutoCompletionComboBoxEdit();
	virtual ~CAutoCompletionComboBoxEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


