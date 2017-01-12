#pragma once
#include "gldInstance.h"

// CStaticShapePreview

class gldShape;
class CDib;

class CStaticShapePreview : public CStatic
{	
	DECLARE_DYNAMIC(CStaticShapePreview)

public:
	CStaticShapePreview();
	virtual ~CStaticShapePreview();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetObj(gldObj* pObj, COLORREF clrBackground = RGB(0xff,0xff,0xff))
	{
		m_pObj = pObj;
		m_clrBackground = clrBackground;
		ReleaseBitmapBuffer();
		Invalidate();
	}
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void RenderEx(CDC& dc, CRect& rc);
	void ReleaseBitmapBuffer();
	void SetMargin(int nMargin)
	{
		m_nMargin = nMargin;
	}
	void SetBorderWidth(int nWidth)
	{
		m_nBorderWidth = nWidth;
	}

protected:

	int			m_nBorderWidth;
	int			m_nMargin;
	gldObj*		m_pObj;
	COLORREF	m_clrBackground;
	CAutoPtr<CDib>	m_pBitmapBuffer;
};


