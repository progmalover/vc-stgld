#pragma once


// CListTemplate
#include "OwnerDrawListBox.h"

class CListTemplate : public COwnerDrawListBox
{
public:
	CListTemplate();
	virtual ~CListTemplate();

	enum FilterTemplate
	{
		FILTER_ELEMENT,
	};

	void Fill(FilterTemplate filter);
	LPCTSTR GetCurTemplate();
	void DeleteCurSel();
	void RenameCurSel();
	void SetCurSelNotify(int nCur);

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
};


