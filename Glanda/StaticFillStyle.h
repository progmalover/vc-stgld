#pragma once

#include "GGraphics.h"

// CStaticFillStyle

class CStaticFillStyle : public CStatic
{
	DECLARE_DYNAMIC(CStaticFillStyle)

public:
	CStaticFillStyle();
	virtual ~CStaticFillStyle();

	void SetFillStyle(TFillStyle *pFillStyle);

	TFillStyle *m_style;
	TFillStyle::FillStyleType m_type;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
};


