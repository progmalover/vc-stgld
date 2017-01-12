#include "StdAfx.h"
#include ".\texteffect.h"
#include "gldLineStyle.h"
#include "gldFillStyle.h"
#include "gldShape.h"
#include "gldTextGlyphRecord.h"
#include "texttoolex.h"
#include "gldShapeRecordChange.h"

typedef vector<gldShapeRecord *> GShapeRecords;
typedef vector<gldLineStyle *> GLineStyles;
typedef vector<gldFillStyle *> GFillStyles;


CTextEffect::CTextEffect(void)
{
	m_pText = NULL;
	m_pLineStyle = NULL;
	m_pFillStyle = NULL;
	m_bApplyPart = true;
	m_bShadow = false;
	m_shadowX = 0;
	m_shadowY = 0;
	m_gradient = 0;
}

CTextEffect::CTextEffect(gldText2 *pText, gldLineStyle *border /*= NULL*/, gldFillStyle *fill /*= NULL*/, bool bApplyPart /*= true*/)
{
	m_pText = pText;
	m_pLineStyle = NULL;
	m_pFillStyle = NULL;
	m_bApplyPart = bApplyPart;
	m_bShadow = false;
	m_shadowX = 0;
	m_shadowY = 0;
	m_gradient = 0;
	if (border != NULL)
		SetBorder(border->GetWidth(), border->GetColor());
	SetFill(fill);		
}

CTextEffect::~CTextEffect(void)
{
	if (m_pFillStyle != NULL)
		delete m_pFillStyle;
	if (m_pLineStyle != NULL)
		delete m_pLineStyle;
}

void CTextEffect::SetFill(gldFillStyle *pFill)
{
	if (m_pFillStyle != NULL)
	{
		delete m_pFillStyle;
		m_pFillStyle = NULL;
	}

	if (pFill)
		m_pFillStyle = pFill->Clone();
}

void CTextEffect::SetBorder(int width, const gldColor &color)
{
	if (m_pLineStyle == NULL)
		m_pLineStyle = new gldLineStyle;
	m_pLineStyle->SetWidth(width);
	m_pLineStyle->SetColor(color);
}

void CTextEffect::SetShadow(int offsetX, int offsetY, const gldColor &color)
{
	m_bShadow = true;
	m_shadowX = offsetX;
	m_shadowY = offsetY;
	m_shadowColor = color;
}

void CTextEffect::GetEffect(OBJINSTANCE_LIST &effect)
{
	if (m_pText == NULL)
		return;

	typedef list<gldTextGlyphRecord*> GTextGlyphRecord;
	typedef GTextGlyphRecord::iterator GTextGlyphRecordIter;

	int	order = 1;

	for (GTextGlyphRecordIter it = m_pText->m_glyphRecords.begin(); it != m_pText->m_glyphRecords.end(); it++)
    {
		gldTextGlyphRecord *rec = (*it);

		if (rec != NULL)
        {			
			gldMatrix	bmx;

            bmx.m_e11 = rec->m_fontHeight / 1000.0f;
            bmx.m_e22 = rec->m_fontHeight / 1000.0f;
			bmx.m_x = rec->m_xOffset;
            bmx.m_y = rec->m_yOffset;
            
			gldShape *pLetter = CreateLetter(rec->m_font, rec->m_code, rec->m_color);

			if (pLetter != NULL)
			{
				gld_shape shape = CTraitShape(pLetter);
				shape.normalize();
				gldMatrix mat;
				CTransAdaptor::TM2GM(shape.matrix(), mat);
				bmx = mat * bmx;
				CTransAdaptor::TShape2GShape(shape, *((gldShape *)pLetter));

				effect.push_back(ObjInstance(pLetter, bmx * m_matrix, m_cxform, order++));
			}
		}
    }

	if ((m_pFillStyle != NULL) && 
		(m_pFillStyle->GetFillStyleType() == linear_gradient_fill ||
		m_pFillStyle->GetFillStyleType() == radial_gradient_fill))
	{
		if (m_bApplyPart)		
		{			
			OBJINSTANCE_LIST_IT it = effect.begin();
			for (; it != effect.end(); ++it)
			{
				gld_shape shape = CTraitShape((*it).m_obj);
				gld_rect bnd = shape.bound();
				float sx = (float)bnd.width() / 0x8000;
				float sy = (float)bnd.height() / 0x8000;
				gldMatrix mx;
				mx.m_e11 = (float)(sx * cos(m_gradient));
				mx.m_e12 = (float)(sx * sin(m_gradient));
				mx.m_e21 = -(float)(sy * sin(m_gradient));
				mx.m_e22 = (float)(sy * cos(m_gradient));
				mx.m_x = (bnd.left + bnd.right) / 2;
				mx.m_y = (bnd.top + bnd.bottom) / 2;
				
				gldShape *ps = (gldShape *)(*it).m_obj;
				GFillStyles::iterator itf = ps->GetFillStyles()->begin();
				for (; itf != ps->GetFillStyles()->end(); ++itf)
					(*itf)->SetGradientMatrix(mx);

				GShapeRecords::iterator itr = ps->GetShapeRecords()->begin();
				for (; itr != ps->GetShapeRecords()->end(); ++itr)
				{
					if ((*itr)->GetGShapeRecType() == gshapeRecChange)
					{
						gldShapeRecordChange *pSetRec = (gldShapeRecordChange *)(*itr);
						GFillStyles::iterator itf = pSetRec->m_fsTable.begin();
						for (; itf != pSetRec->m_fsTable.end(); ++itf)
							(*itf)->SetGradientMatrix(mx);
					}
				}				
			}
		}
		else
		{
			gld_rect box;
			bool bAssign = false;
			OBJINSTANCE_LIST_IT it = effect.begin();
			for (; it != effect.end(); ++it)
			{
				gld_shape shape = CTraitShape((*it).m_obj);
				ASSERT(shape.validate());
				gld_rect bnd = shape.bound();
				int x1 = (*it).m_mx.GetX(bnd.left, bnd.top);
				int y1 = (*it).m_mx.GetY(bnd.left, bnd.top);
				int x2 = (*it).m_mx.GetX(bnd.left, bnd.bottom);
				int y2 = (*it).m_mx.GetY(bnd.left, bnd.bottom);
				int x3 = (*it).m_mx.GetX(bnd.right, bnd.bottom);
				int y3 = (*it).m_mx.GetY(bnd.right, bnd.bottom);
				int x4 = (*it).m_mx.GetX(bnd.right, bnd.top);
				int y4 = (*it).m_mx.GetY(bnd.right, bnd.top);
				if (bAssign)
				{
					box.left = __min(__min(__min(__min(x1, x2), x3), x4), box.left);
					box.top = __min(__min(__min(__min(y1, y2), y3), y4), box.top);
					box.right = __max(__max(__max(__max(x1, x2), x3), x4), box.right);
					box.bottom = __max(__max(__max(__max(y1, y2), y3), y4), box.bottom);
				}
				else
				{
					box.left = __min(__min(__min(x1, x2), x3), x4);
					box.top = __min(__min(__min(y1, y2), y3), y4);
					box.right = __max(__max(__max(x1, x2), x3), x4);
					box.bottom = __max(__max(__max(y1, y2), y3), y4);
					bAssign = true;
				}
			}

			float sx = (float)box.width() / 0x8000;
			float sy = (float)box.height() / 0x8000;
			gldMatrix mx;
			mx.m_e11 = (float)(sx * cos(m_gradient));
			mx.m_e12 = (float)(sx * sin(m_gradient));
			mx.m_e21 = -(float)(sy * sin(m_gradient));
			mx.m_e22 = (float)(sy * cos(m_gradient));
			mx.m_x = (box.left + box.right) / 2;
			mx.m_y = (box.top + box.bottom) / 2;

			it = effect.begin();
			for (; it != effect.end(); ++it)
			{				
				gldShape *ps = (gldShape *)(*it).m_obj;
				gldMatrix invMat = (*it).m_mx.invert();
				GFillStyles::iterator itf = ps->GetFillStyles()->begin();
				for (; itf != ps->GetFillStyles()->end(); ++itf)
					(*itf)->SetGradientMatrix(mx * invMat);

				GShapeRecords::iterator itr = ps->GetShapeRecords()->begin();
				for (; itr != ps->GetShapeRecords()->end(); ++itr)
				{
					if ((*itr)->GetGShapeRecType() == gshapeRecChange)
					{
						gldShapeRecordChange *pSetRec = (gldShapeRecordChange *)(*itr);
						GFillStyles::iterator itf = pSetRec->m_fsTable.begin();
						for (; itf != pSetRec->m_fsTable.end(); ++itf)
							(*itf)->SetGradientMatrix(mx);
					}
				}				
			}
		}
	}

	OBJINSTANCE_LIST_IT ite = effect.begin();
	for (; ite != effect.end(); ++ite)
	{
		gldShape *ps = (gldShape *)(*ite).m_obj;
		CTransAdaptor::DestroyTShapePtr(ps);
		ps->m_ptr = NULL;
		CSWFProxy::PrepareTShapeForGObj(ps);
		gld_shape shape = CTraitShape(ps);
		gld_rect bnd = shape.calc_bound();
		shape.bound(bnd);				
		ps->m_bounds.left = bnd.left;
		ps->m_bounds.top = bnd.top;
		ps->m_bounds.right = bnd.right;
		ps->m_bounds.bottom = bnd.bottom;
	}
}

gldShape *CTextEffect::CreateLetter(gldFont *pFont, UINT uCode, const gldColor &color)
{
	gldShape *pTmpl = pFont->GetCharShape(uCode);
	ASSERT(pTmpl != NULL);
	if (pTmpl == NULL || pTmpl->GetShapeRecords()->size() == 0)
		return NULL;
	gldShape *pLetter = (gldShape *)pTmpl->Clone();
	pLetter->ClearFillStyles();
	int sIndex = 0;
	GShapeRecords *pRecs = pLetter->GetShapeRecords();
	GFillStyles *pFills = pLetter->GetFillStyles();
	GLineStyles *pLines = pLetter->GetLineStyles();

	if (m_bShadow)	// build shadow
	{
		int ls = 0;
		if (m_pLineStyle != NULL && m_pLineStyle->GetWidth() > 0)
		{
			gldLineStyle *pBorder = new gldLineStyle;
			pBorder->SetColor(m_shadowColor);
			pBorder->SetWidth(m_pLineStyle->GetWidth());
			ASSERT(pLines->size() == 0);
			pLines->push_back(pBorder);
			ls = 1;
		}

		ASSERT(pFills->size() == 0);		
		gldFillStyle *pFill = new gldFillStyle(solid_fill);
		pFill->SetSolidFillColor(m_shadowColor);
		pFills->push_back(pFill);

		ASSERT(pRecs->size() > 0);
		gldShapeRecord *pFirstRec = *pRecs->begin();
		ASSERT(pFirstRec->GetGShapeRecType() == gshapeRecChange);
		gldShapeRecordChange *pSetRec = (gldShapeRecordChange *)pFirstRec;
		pSetRec->m_stateFS0 = true;
		pSetRec->m_stateFS1 = false;
		pSetRec->m_stateLS = true;
		pSetRec->m_ls = ls;
		pSetRec->m_fs0 = 1;
		
		sIndex = (int)pRecs->size();	//****

		pRecs->push_back(new gldShapeRecordChange);

		for (int i = 0; i < sIndex; i++)
		{
			pRecs->push_back((*pRecs)[i]->Clone());
		}

		GShapeRecords::iterator itr = pRecs->begin();
		for (; itr < pRecs->begin() + sIndex; ++itr)
		{
			if ((*itr)->GetGShapeRecType() == gshapeRecChange)
			{
				gldShapeRecordChange *pSetRec = (gldShapeRecordChange *)(*itr);
				if (pSetRec->m_stateMoveTo)
				{
					pSetRec->m_moveDX += m_shadowX;
					pSetRec->m_moveDY += m_shadowY;
				}
			}
		}

		gldShapeRecordChange *pNextObj = (gldShapeRecordChange *)(*itr);
		pNextObj->m_stateNewStyles = true;
		pFills = &pNextObj->m_fsTable;
		pLines = &pNextObj->m_lsTable;

		sIndex++;
	}

	int ls = 0, fs = 0;
	if (m_pLineStyle != NULL && m_pLineStyle->GetWidth() > 0)
	{
		ASSERT(pLines->size() == 0);
		pLines->push_back(m_pLineStyle->Clone());
		ls = 1;
	}
	if (m_pFillStyle != NULL)
	{
		ASSERT(pFills->size() == 0);
		pFills->push_back(m_pFillStyle->Clone());
		fs = 1;
	}
	else
	{
		ASSERT(pFills->size() == 0);
		gldFillStyle *pSolidFill = new gldFillStyle(solid_fill);
		pSolidFill->SetSolidFillColor(color);
		pFills->push_back(pSolidFill);
		fs = 1;
	}

	gldShapeRecord *pFirstRec = (*pRecs)[sIndex];
	ASSERT(pFirstRec->GetGShapeRecType() == gshapeRecChange);
	gldShapeRecordChange *pSetRec = (gldShapeRecordChange *)pFirstRec;
	pSetRec->m_stateFS0 = true;
	pSetRec->m_stateFS1 = false;
	pSetRec->m_stateLS = true;
	pSetRec->m_ls = ls;
	pSetRec->m_fs0 = fs;
	
	pLetter->m_ptr = NULL;
	CSWFProxy::PrepareTShapeForGObj(pLetter);

	return pLetter;
}