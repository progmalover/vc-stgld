#pragma once
#include "FlatButton.h"

class CTextStyleData;
class CDib;

// CFlatShapeButton

class CFlatShapeButton : public CFlatButton
{
	//DECLARE_DYNAMIC(CFlatShapeButton)

	enum	{ SPACING = 2	};
	CAutoPtr<CTextStyleData>	m_pTextStyle;
	CAutoPtr<CDib>				m_pBitmapBuffer;
public:
	CFlatShapeButton();
	virtual ~CFlatShapeButton();

	virtual void DrawStuff(CDC *pDC, const CRect &rc);
	virtual CSize GetStuffSize();
	CTextStyleData* GetTextStyle();
	void SetTextStyle(CTextStyleData* pTextStyle);
	void RenderEx(CDC& dc, CRect& rc);
	void ReleaseBitmapBuffer();
protected:
	DECLARE_MESSAGE_MAP()
};


