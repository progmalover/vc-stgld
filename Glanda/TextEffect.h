#pragma once

#include <list>
using namespace std;
#include "ObjInstance.h"

class gldText2;
class gldLineStyle;
class gldFillStyle;
class gldColor;
class gldFont;
class gldShape;

class CTextEffect
{
public:
	CTextEffect(void);
	CTextEffect(gldText2 *pText, gldLineStyle *border = NULL, gldFillStyle *fill = NULL, bool bApplyPart = true);
	virtual ~CTextEffect(void);

	void SetFill(gldFillStyle *pFill);
	void SetBorder(int width, const gldColor &color);
	void SetApplyPart(bool bPart) { m_bApplyPart = bPart; }
	void SetShadow(int offsetX, int offsetY, const gldColor &color);
	void SetGradient(double angle) { m_gradient = angle; }
	void SetMatrix(const gldMatrix &matrix) { m_matrix = matrix; }
	void SetCxform(const gldCxform &cxform) { m_cxform = cxform; }

	void GetEffect(OBJINSTANCE_LIST &effect);

protected:
	gldShape *CreateLetter(gldFont *pFont, UINT uCode, const gldColor &color);

protected:
	gldText2 *m_pText;
	gldLineStyle *m_pLineStyle;
	gldFillStyle *m_pFillStyle;
	bool m_bApplyPart;
	bool m_bShadow;
	int m_shadowX;
	int m_shadowY;
	gldColor m_shadowColor;
	gldMatrix m_matrix;
	gldCxform m_cxform;
	double m_gradient;
};
