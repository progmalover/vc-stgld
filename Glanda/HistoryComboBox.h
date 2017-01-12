#pragma once

#include "AutoCompletionComboBox.h"

// CHistoryComboBox

class CHistoryComboBox : public CAutoCompletionComboBox
{
public:
	CHistoryComboBox();
	virtual ~CHistoryComboBox();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	UINT m_nMaxListSize;
public:
	int Save();
	int Load();
	void AddCurrentText(BOOL bCaseSensitive = FALSE);
	BOOL ClearHistory();
protected:
	CString m_strKey;
public:
	void Init(LPCTSTR lpszKey, UINT nMaxListSize = 16);
};


