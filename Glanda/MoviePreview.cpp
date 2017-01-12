#include "stdafx.h"
#include "TextToolEx.h"
#include "ITextTool.h"
#include "toolsdef.h"
#include "Glanda_i.h"
#include "gldText.h"
#include "TextEffect.h"
#include "C2I.h"
#include "I2C.h"
#include "TransAdaptor.h"
#include "gldSWFEngine.h"
#include "gldSprite.h"
#include "gldLibrary.h"
#include "gldDataKeeper.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldCharacter.h"
#include "gldFillStyle.h"
#include "GlandaDoc.h"
#include "filepath.h"
#include "gldEffect.h"
#include "EffectCommonUtils.h"
#include "EffectCommonParameters.h"
#include "gldShapeRecordChange.h"
#include "gldShapeRecordEdgeStraight.h"
#include "MoviePreview.h"

#define FNEARZERO(v) (fabs(v) < 1e-4)
#ifdef _DEBUG
#define DEBUG_(statement) statement
#else
#define DEBUG_(statement)
#endif

// Helper function
BOOL IsUnitMatrix(gldMatrix &mx)
{
	return FNEARZERO(mx.m_e11 - 1.0) && FNEARZERO(mx.m_e22 - 1.0) &&
		FNEARZERO((double)mx.m_e12) && FNEARZERO((double)mx.m_e21) && 
		FNEARZERO((double)mx.m_x) && FNEARZERO((double)mx.m_y);
}

BOOL IsUnitCxform(gldCxform &cx)
{
	return cx.aa == 1 && cx.ra == 1 && cx.ga == 1 && cx.ba == 1 &&
		cx.ab == 0 && cx.rb == 0 && cx.gb == 0 && cx.bb == 0;
}

CSize GetTextExtent(const CString &text, CTextFormat &fmt)
{
	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(NULL));
	dcMem.SetMapMode(MM_TEXT);
	CFont font;
	VERIFY(font.CreateFont(-STD_FONT_HEIGHT, 0, 0, 0, fmt.GetBold() ? FW_BOLD : FW_REGULAR,
		fmt.GetItalic(), FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		fmt.GetFontFace().c_str()));
	CFont *pOldFont = (CFont *)dcMem.SelectObject(&font);
	CSize size = dcMem.GetTextExtent(text);
	double scale = fmt.GetFontSize() / (double)STD_FONT_HEIGHT;
	size.cx = (int)(size.cx * scale) + 4 * BORDER_SIZE + text.GetLength() * fmt.GetSpacing();
	size.cy = (int)(size.cy * scale) + 2 * BORDER_SIZE;
	dcMem.SelectObject(pOldFont);
	return size;
}

void CreateFrameShape(gldShape *shape, const CRect &rect, COLORREF color)
{		
	shape->Clear();

	gldLineStyle *pLs = new gldLineStyle;
	pLs->SetColor(gldColor(0, 0, 0, 255));
	pLs->SetWidth(20);
	shape->GetLineStyles()->push_back(pLs);
	gldFillStyle *pFs = new gldFillStyle(solid_fill);
	pFs->SetSolidFillColor(gldColor(GetRValue(color), GetGValue(color), GetBValue(color), 255));
	shape->GetFillStyles()->push_back(pFs);

	gldShapeRecordChange *r1 = new gldShapeRecordChange;
	r1->m_stateLS = 1;
	r1->m_ls = 1;
	r1->m_stateFS0 = 1;
	r1->m_fs0 = 1;
	r1->m_stateMoveTo = 1;
	r1->m_moveDX = rect.left;
	r1->m_moveDY = rect.top;
	gldShapeRecordEdgeStraight *r2 = new gldShapeRecordEdgeStraight;
	r2->m_dx = rect.Width();
	r2->m_dy = 0;
	gldShapeRecordEdgeStraight *r3 = new gldShapeRecordEdgeStraight;
	r3->m_dx = 0;
	r3->m_dy = rect.Height();
	gldShapeRecordEdgeStraight *r4 = new gldShapeRecordEdgeStraight;
	r4->m_dx = -rect.Width();
	r4->m_dy = 0;
	gldShapeRecordEdgeStraight *r5 = new gldShapeRecordEdgeStraight;
	r5->m_dx = 0;
	r5->m_dy = -rect.Height();
	shape->GetShapeRecords()->push_back(r1);
	shape->GetShapeRecords()->push_back(r2);
	shape->GetShapeRecords()->push_back(r3);
	shape->GetShapeRecords()->push_back(r4);
	shape->GetShapeRecords()->push_back(r5);
	shape->m_bounds = rect;	
}

gldText2 *CreateSingleLineText2(const CString &text, CTextFormat &txtFmt)
{
	gldText2 *pText = new gldText2;
	CParagraph *pPara = new CParagraph(NULL);
	pText->m_ParagraphList.push_back(pPara);
	USES_CONVERSION;
	CTextBlock *pBlock = new CTextBlock(txtFmt, CT2W((LPCTSTR)text));
	pPara->m_TextBlockList.push_back(pBlock);

	CSize szText = GetTextExtent(text, txtFmt);	
	pText->m_bounds.left = 0;
	pText->m_bounds.top = 0;
	pText->m_bounds.right = szText.cx;
	pText->m_bounds.bottom = szText.cy;
	pText->bNoConvert = FALSE;
	ITextTool IClass;
	SETextToolEx TTool(&IClass);
	TTool.ConvertFromGldText(*pText);
	TTool.ConvertToGldText(*pText);
	CSWFProxy::PrepareTShapeForGObj(pText);

	return pText;
}

// class CMoviePreview
CMoviePreview::CMoviePreview()
{
	m_pMovie = NULL;
	m_bAutoDeleteMovie = TRUE;
	m_pEffect = NULL;
}

CMoviePreview::~CMoviePreview()
{
	Clear();
}

void CMoviePreview::Clear()
{
	if (m_pMovie != NULL && m_bAutoDeleteMovie)
	{
		delete m_pMovie;
	}
	m_pMovie = NULL;
	if (m_pEffect != NULL)
	{
		delete m_pEffect;
		m_pEffect = NULL;
	}	
	OBJECT2TYPE::iterator ito = m_ObjMap.begin();
	for (; ito != m_ObjMap.end(); ++ito)
	{
		if ((*ito).second != SHARED_OBJECT)
		{
			gldObj *pObj = (*ito).first;
			CObjectMap::FreeSymbol(&pObj);
		}
	}
	m_ObjMap.clear();
}

void CMoviePreview::DeleteEffect()
{
	if (m_pEffect != NULL)
	{
		delete m_pEffect;
		m_pEffect = NULL;
		
		OBJECT2TYPE::iterator ito = m_ObjMap.begin();
		while (ito != m_ObjMap.end())
		{
			if ((*ito).second == ACCESSIONAL_OBJECT)
			{
				gldObj *pObj = (*ito).first;
				CObjectMap::FreeSymbol(&pObj);
				OBJECT2TYPE::iterator in = ito;
				++in;
				m_ObjMap.erase(ito);
				ito = in;
			}
			else
			{
				++ito;
			}
		}
	}
}

void CMoviePreview::SetMovieClip(gldMovieClip *pMovie, const gldMatrix &mx, const gldCxform &cx,
								 BOOL bAutoDeleteMovie /*= TRUE*/, BOOL bShareObject /*= TRUE*/)
{
	ASSERT(pMovie != NULL);

	Clear();

	m_bAutoDeleteMovie = bAutoDeleteMovie;

	int oT = (bShareObject ? SHARED_OBJECT : OWNER_OBJECT);

	GOBJECT_LIST objList;
	pMovie->GetUsedObjs(objList);
	// fix gldMovieClip::GetUsedObjs() bug
	// GOBJECT_LIST must be a list otherwise follow is error
	GOBJECT_LIST::iterator it = objList.begin();
	for (; it != objList.end(); ++it)
		(*it)->GetUsedObjs(objList);
	// fix CSWF lib bug, gldText::GetUsedObjs that not return fonts is used	
	for (it = objList.begin(); it != objList.end(); ++it)
	{
		gldObj *pObj = *it;
		if (pObj->IsGObjInstanceOf(gobjText))
		{
			gldText *pText = (gldText *)pObj;
			if (pText->m_dynaInfo.m_font != NULL)
			{
				if (find(objList.begin(), objList.end(), pText->m_dynaInfo.m_font) == objList.end())
					objList.push_front(pText->m_dynaInfo.m_font);
			}
			list<gldTextGlyphRecord*>::iterator it = pText->m_glyphRecords.begin();
			for(; it != pText->m_glyphRecords.end(); ++it)
			{
				gldTextGlyphRecord *rec = (*it);			    
				if (rec->m_font != NULL)
				{
					if (find(objList.begin(), objList.end(), rec->m_font) == objList.end())
						objList.push_front(rec->m_font);
				}
			}
		}
	}

	for (GOBJECT_LIST_IT ito = objList.begin(); ito != objList.end(); ++ito)
	{
		if ((*ito)->IsGObjInstanceOf(gobjFont))
			m_ObjMap.insert(OBJECT2TYPE::value_type(*ito, SHARED_OBJECT));
		else
			m_ObjMap.insert(OBJECT2TYPE::value_type(*ito, oT));
	}

	m_pMovie = pMovie;
	m_Matrix = mx;
	m_Cxform = cx;
}

BOOL CMoviePreview::ApplyEffect(IGLD_Effect *pIEffect, IGLD_Parameters *pIParas, IGLD_Parameters *pIExtra)
{
	ASSERT(m_pMovie != NULL);

	// get ideal length
	int nKeys = 1;
	for (GLAYER_LIST_IT itl = m_pMovie->m_layerList.begin()
		; itl != m_pMovie->m_layerList.end(); ++itl)
	{
		gldLayer *pLayer = *itl;
		if (pLayer->m_frameClipList.size() > 0)
		{
			gldFrameClip *pFrame = *(pLayer->m_frameClipList.begin());
			nKeys += pFrame->m_characterKeyList.size();
		}
	}	
	int nLength = 24;
	CComQIPtr<IGLD_EffectInfo> pIInfo(pIEffect);
	if (pIInfo != NULL)
	{
		long nFrames = 0;
		if (SUCCEEDED(pIInfo->GetIdealFrames(nKeys, &nFrames)))
		{
			nLength = max(1, (int)nFrames);			
		}
	}
	
	return ApplyEffect(pIEffect, 0, nLength, pIParas, pIExtra);
}

BOOL CMoviePreview::ApplyEffect(IGLD_Effect *pIEffect, int nStart, int nLength, 
								IGLD_Parameters *pIParas, IGLD_Parameters *pIExtra)
{
	ASSERT(pIEffect != NULL && pIParas != NULL && m_pMovie != NULL && pIExtra != NULL);

	// free previous effect movie-clip
	DeleteEffect();

	m_pEffect = new gldMovieClip;

	HRESULT hr;

	CComPtr<IGLD_Sprite> pISprite;
	hr = pISprite.CoCreateInstance(__uuidof(GLD_Sprite));
	if (FAILED(hr)) return FALSE;
	CComPtr<IGLD_MovieClip> pIMovie;
	hr = pISprite->get_MovieClip(&pIMovie);
	if (FAILED(hr)) return FALSE;

	// build IGLD_MovieClip interface from m_pMovie
	CC2IMap c2i;
	CC2IMap *oc2i = CC2I::m_pC2IMap;
	CC2I::m_pC2IMap = &c2i;
	hr = CC2I::Convert(m_pMovie, pIMovie);
	CC2I::m_pC2IMap = oc2i;
	if (FAILED(hr)) return FALSE;

	// build effect movie by effect interface
	CComPtr<IGLD_Matrix> pIMatrix;	
	hr = pIMatrix.CoCreateInstance(__uuidof(GLD_Matrix));
	if (FAILED(hr)) return FALSE;
	CComPtr<IGLD_Cxform> pICxform;
	hr = pICxform.CoCreateInstance(__uuidof(GLD_Cxform));
	if (FAILED(hr)) return FALSE;
	pIMatrix->SetData(m_Matrix.m_e11, m_Matrix.m_e12, 
		m_Matrix.m_e21, m_Matrix.m_e22,
		m_Matrix.m_x, m_Matrix.m_y);
	pICxform->SetData(m_Cxform.ra, m_Cxform.rb,
		m_Cxform.ga, m_Cxform.gb, 
		m_Cxform.ba, m_Cxform.bb, 
		m_Cxform.aa, m_Cxform.ab);
	PutParameter(pIExtra, "MovieClip.Matrix", pIMatrix);
	PutParameter(pIExtra, "MovieClip.Cxform", pICxform);

	hr = pIEffect->Build(pIMovie, nStart, nLength, pIParas, pIExtra);
	if (FAILED(hr)) return FALSE;

	// convert IGLD_MovieClip to m_pEffect
	CI2CMap i2c;
	c2i.CopyTo(&i2c);
	CI2CMap *oi2c = CI2C::m_pI2CMap;
	CI2C::m_pI2CMap = &i2c;	
	hr = CI2C::Convert(pIMovie, m_pEffect);
	CI2C::m_pI2CMap = oi2c;
	for (CI2CMap::_I2CMap::iterator ito = i2c.m_Map.begin(); ito != i2c.m_Map.end(); ++ito)
	{
		m_ObjMap.insert(OBJECT2TYPE::value_type((*ito).second, ACCESSIONAL_OBJECT));
	}	
	if (FAILED(hr)) return FALSE;	
	
	// add action to apply auto play options
	int effectTime = m_pEffect->GetMaxTime();
	if (effectTime > 0)
	{
		// auto loop
		BOOL bLoop = FALSE;
		GetParameter(pIParas, EP_LOOP, &bLoop);
		if (!bLoop)
		{
			gldLayer *actionLayer = new gldLayer("Action", 0);
			if (effectTime > 1)
			{
				gldFrameClip *emptyFC = new gldFrameClip(0, effectTime - 1);
				actionLayer->m_frameClipList.push_back(emptyFC);

			}
			gldFrameClip *actionFC = new gldFrameClip(effectTime - 1, 1);
			actionFC->m_action = new gldAction();
			actionFC->m_action->m_type = actionSegFrame;
			actionFC->m_action->SetSourceCode("stop();");
			actionLayer->m_frameClipList.push_back(actionFC);
			m_pEffect->AddLayer(actionLayer);
		}
	}

	return TRUE;
}

void CMoviePreview::SetMovieClip(const CString &text, CTextFormat &fmt, IGLD_Parameters *opts)
{
	gldMovieClip *pMovie = new gldMovieClip;

	gldLayer *pLayer = new gldLayer("Layer1", 1);
	pMovie->AddLayer(pLayer);
	gldFrameClip *pFrame = *pLayer->m_frameClipList.begin();

	gldText2 *pText = CreateSingleLineText2(text, fmt);

	int left = ((int)_GetMainMovie2()->m_width * 20 - pText->m_bounds.Width()) / 2;
	int top = ((int)_GetMainMovie2()->m_height * 20 - pText->m_bounds.Height()) / 2;

	VARIANT_BOOL bBreakApart = VARIANT_TRUE;
	GetParameter(opts, EP_BREAK_APART, &bBreakApart);
	if (bBreakApart)
	{
		OBJINSTANCE_LIST lstInst;
		CSWFProxy::BreakApartTextToShapes(pText, gldMatrix(), gldCxform(), opts, lstInst);
		for (OBJINSTANCE_LIST::iterator iti = lstInst.begin(); iti != lstInst.end(); ++iti)
		{
			pFrame->AddCharacter((*iti).m_obj, (*iti).m_mx, (*iti).m_cx);
		}		
		CObjectMap::FreeSymbol((gldObj **)&pText);		
	}
	else
	{
		pFrame->AddCharacter(pText, gldMatrix(), gldCxform());
	}

	gldMatrix mx;
	mx.m_x = left;
	mx.m_y = top;
	
	SetMovieClip(pMovie, mx, gldCxform(), TRUE, FALSE);
}

void CMoviePreview::SetMovieClip(gldInstance *pInst, IGLD_Parameters *opts)
{
	if (pInst->m_obj->IsGObjInstanceOf(gobjText))
	{
		gldMovieClip *pMovie = new gldMovieClip;		
		gldLayer *pLayer = new gldLayer("Layer1", 1);
		pMovie->AddLayer(pLayer);
		gldFrameClip *pFrame = *pLayer->m_frameClipList.begin();

		gldText2 *pText = (gldText2 *)pInst->m_obj;
		VARIANT_BOOL bBreakApart = VARIANT_TRUE;
		GetParameter(opts, EP_BREAK_APART, &bBreakApart);
		if (bBreakApart)
		{
			OBJINSTANCE_LIST lstInst;
			CSWFProxy::BreakApartTextToShapes(pText, gldMatrix(), gldCxform(), opts, lstInst);
			for (OBJINSTANCE_LIST::iterator iti = lstInst.begin(); iti != lstInst.end(); ++iti)
			{
				pFrame->AddCharacter((*iti).m_obj, (*iti).m_mx, (*iti).m_cx);
			}

			SetMovieClip(pMovie, pInst->m_matrix, pInst->m_cxform, TRUE, FALSE);
		}
		else
		{
			pFrame->AddCharacter(pText, gldMatrix(), gldCxform());

			SetMovieClip(pMovie, pInst->m_matrix, pInst->m_cxform, TRUE, TRUE);
		}
		return;
	}
	
	if (CSWFProxy::GObjIsValidGroup(pInst->m_obj))
	{		
		gldSprite *pGroup = (gldSprite *)pInst->m_obj;		
		VARIANT_BOOL bBreakApart = VARIANT_TRUE;
		GetParameter(opts, EP_BREAK_APART, &bBreakApart);
		if (bBreakApart)
		{
			gldMovieClip *pMovie = pGroup->m_mc;			
			SetMovieClip(pMovie, pInst->m_matrix, pInst->m_cxform, FALSE, TRUE);
			return;
		}
	}

	gldMovieClip *pMovie = new gldMovieClip;
	gldLayer *pLayer = new gldLayer("Layer1", 1);
	pMovie->AddLayer(pLayer);
	gldFrameClip *pFrame = *pLayer->m_frameClipList.begin();
	pFrame->AddCharacter(pInst->m_obj, gldMatrix(), gldCxform());

	SetMovieClip(pMovie, pInst->m_matrix, pInst->m_cxform, TRUE, TRUE);		
}

BOOL CMoviePreview::CreatePreview(const CString &sPath, COLORREF crBackground, BOOL bShowCanvas /*= TRUE*/)
{
	if (m_pEffect == NULL && m_pMovie == NULL)
	{
		return FALSE;
	}

	gldSprite *mainSprite = new gldSprite;
	gldMovieClip *mainMovie = mainSprite->m_mc;
	gldLayer *mainLayer = new gldLayer("", 1);	
	gldFrameClip *mainFrame = *(mainLayer->m_frameClipList.begin());
	
	mainMovie->AddLayer(mainLayer);

	// put canvas frame to main frame
	CRect movieRect;
	movieRect.left = 0;
	movieRect.top = 0;
	movieRect.right = _GetMainMovie2()->m_width * 20;
	movieRect.bottom = _GetMainMovie2()->m_height * 20;
	gldShape *canvas = NULL;
	if (bShowCanvas)
	{
		canvas = new gldShape;
		CreateFrameShape(canvas, movieRect, crBackground);
		CSWFProxy::PrepareTShapeForGObj(canvas);
		mainFrame->AddCharacter(canvas, gldMatrix(), gldCxform());
		crBackground = my_app.GetWorkspaceBkColor();
	}

	// put effect or origin movie-clip to main frame
	gldSprite *placeSprite = new gldSprite;
	gldMovieClip *pOldPlaceMovie = placeSprite->m_mc;
	placeSprite->m_mc = (m_pEffect == NULL ? m_pMovie : m_pEffect);
	gldObj *pOldParent = placeSprite->m_mc->m_parentObj; // don't forge this!!!!!!!!
	placeSprite->m_mc->m_parentObj = placeSprite;
	CSWFProxy::PrepareTShapeForGObj(placeSprite);
	gldMatrix mx = m_Matrix;
	if (!bShowCanvas)
	{		
		CRect &bound = placeSprite->m_bounds;
		int cenx = (bound.left + bound.right) / 2;
		int ceny = (bound.top + bound.bottom) / 2;
		int offsetX = (movieRect.left + movieRect.right) / 2 - m_Matrix.GetX(cenx, ceny);
		int offsetY = (movieRect.top + movieRect.bottom) / 2 - m_Matrix.GetY(cenx, ceny);
		mx.m_x += offsetX;
		mx.m_y += offsetY;
	}
	mainFrame->AddCharacter(placeSprite, mx, m_Cxform);
	VERIFY(CompileActionScript(placeSprite, FALSE));

	// calculate bounds for sprite
	CSWFProxy::PrepareTShapeForGObj(mainSprite);	

	// use cswf lib to create swf file for preview	
	gldLibrary lib;
	for (OBJECT2TYPE::iterator ito = m_ObjMap.begin(); ito != m_ObjMap.end(); ++ito)
	{
		lib.AddObj((*ito).first);
	}
	if (canvas != NULL)
	{
		lib.AddObj(canvas);
	}	
	lib.AddObj(placeSprite);	

	DEBUG_(int libSize = lib.GetObjList().size();)

	gldLibrary *oLib = gldDataKeeper::Instance()->m_objLib;
	if (oLib != NULL)
		oLib->ClearAllObjID();
	gldDataKeeper::Instance()->m_objLib = &lib;
	gldSWFEngine gen;
	movieRect.InflateRect(MARGIN, MARGIN, MARGIN, MARGIN);
	bool ok = gen.CreatePreview(sPath, mainSprite
		, _GetMainMovie2()->m_frameRate, crBackground
		, movieRect.left, movieRect.top, movieRect.Width(), movieRect.Height());
	gldDataKeeper::Instance()->m_objLib = oLib;	
	
	ASSERT(lib.GetObjList().size() == libSize); // create preview must be not add some object to library

	lib.ClearList();

	// free resource	
	CTransAdaptor::DestroyTShapePtr(placeSprite);
	placeSprite->m_mc->m_parentObj = pOldParent;
	placeSprite->m_mc = pOldPlaceMovie;
	delete placeSprite;
	if (canvas != NULL)
	{
		CTransAdaptor::DestroyTShapePtr(canvas);
		delete canvas;
	}	
	CTransAdaptor::DestroyTShapePtr(mainSprite);
	delete mainSprite;

	return (ok == true);
}