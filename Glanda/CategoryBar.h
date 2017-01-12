#pragma once

#include "GlandaSizingControlBar.h"
#include "Singleton.h"

// CCategoryBar

class CCategoryBar : public CGlandaSizingControlBar, public CSingleton<CCategoryBar>
{
	DECLARE_DYNAMIC(CCategoryBar)

public:
	CCategoryBar();
	virtual ~CCategoryBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};