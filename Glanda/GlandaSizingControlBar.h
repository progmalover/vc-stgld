#pragma once

#include "SizingControlbarEx.h"

// CGlandaSizingControlBar

class CGlandaSizingControlBar : public CSizingControlBarEx
{
	DECLARE_DYNAMIC(CGlandaSizingControlBar)

public:
	CGlandaSizingControlBar();
	virtual ~CGlandaSizingControlBar();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void GetTitleRect(CRect *pRect);
	virtual void PaintTitle(CDC &dc);
	virtual void RecalcLayout();
	virtual void LoadCloseButtonBitmap(BOOL bActive);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


