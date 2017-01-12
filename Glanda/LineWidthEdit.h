#pragma once

#include "NumberEdit.h"

// CLineWidthEdit

class CLineWidthEdit : public CNumberEdit
{
	DECLARE_DYNAMIC(CLineWidthEdit)

public:
	CLineWidthEdit();
	virtual ~CLineWidthEdit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnSliderPosChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSliderPosInit(WPARAM wp, LPARAM lp);
};


