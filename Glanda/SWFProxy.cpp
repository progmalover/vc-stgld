#include "StdAfx.h"
#include "swfproxy.h"
#include "gldFrameClip.h"
#include "gldLayer.h"
#include "gldSprite.h"
#include "gldButton.h"
#include "gldMovieClip.h"
#include "gldObj.h"
#include "gldFont.h"
#include "gldTextGlyphRecord.h"
#include "gldText.h"
#include "gldImage.h"
#include "gldShape.h"
#include "gldCharacter.h"
#include "gldDataKeeper.h"
#include "TransAdaptor.h"
#include "gldLibrary.h"
#include "image.h"
#include "Observer.h"
#include "gld_shape_builder.h"
#include "BinStream.h"
#include "my_app.h"
#include "Resource.h"

#include "gldInstance.h"
#include "TextEffect.h"
#include "gldGradientRecord.h"
#include "gldGradient.h"
#include "Glanda_i.h"

#include "EffectCommonParameters.h"
#include "I2C.h"
#include "gldMainMovie2.h"
#include "GlandaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSWFProxy::CSWFProxy(void)
{
}

CSWFProxy::~CSWFProxy(void)
{
}

gldLayer *CSWFProxy::GetFrameClipLayer(gldFrameClip *pFC)
{
	return pFC->m_parentLayer;
}

gldFrameClip *CSWFProxy::GetNextFrameClip(gldFrameClip *pFC)
{
	return GetFrameClipLayer(pFC)->GetFrameClip(pFC->m_startTime + pFC->m_length);
}

gldFrameClip *CSWFProxy::GetPrevFrameClip(gldFrameClip *pFC)
{
	return GetFrameClipLayer(pFC)->GetFrameClip(pFC->m_startTime - 1);
}

gldFrameClip *CSWFProxy::GetPrevFrameClip(gldLayer *pLayer, const GFRAMECLIP_LIST_IT &pos)
{
	GFRAMECLIP_LIST_IT it = pos;

	if (--it != pLayer->m_frameClipList.end())
	{
		return *it;
	}
	else
	{
		return NULL;
	}
}

gldFrameClip *CSWFProxy::GetNextFrameClip(gldLayer *pLayer, const GFRAMECLIP_LIST_IT &pos)
{
	GFRAMECLIP_LIST_IT it = pos;

	if (++it != pLayer->m_frameClipList.end())
	{
		return *it;
	}
	else
	{
		return NULL;
	}
}

void CSWFProxy::UpdateFrameClipValidFlag(gldFrameClip *pFC)
{
	if (!pFC->IsMotion())
	{
		return;
	}

	gldFrameClip *pNextFC = GetNextFrameClip(pFC);

	if (pNextFC == NULL)
    {
        pFC->m_flag &= (~FC_VALID);

        return;
    }

    // Two character key isn't the same, can't create the motion tween
    if (pFC->m_characterKeyList.size() != pNextFC->m_characterKeyList.size())
    {
        pFC->m_flag &= (~FC_VALID);

        return;
    }

    GCHARACTERKEY_LIST_IT itThis, itNext;
    bool theSame = true;
    for (itThis = pFC->m_characterKeyList.begin(), itNext = pNextFC->m_characterKeyList.begin();
         itThis != pFC->m_characterKeyList.end();
         itThis++, itNext++)
    {
        gldCharacterKey *_thisKey = (*itThis);
        gldCharacterKey *_nextKey = (*itNext);

        if (_thisKey->GetObj() != _nextKey->GetObj())
        {
            theSame = false;
            break;
        }
    }

    if (!theSame)
    {
        pFC->m_flag &= (~FC_VALID);

        return;
    }

	pFC->m_flag |= FC_VALID;
}

gldMovieClip *CSWFProxy::GetCurMovieClip()
{
	return gldDataKeeper::Instance()->m_curMovieClip;
}

gldObj *CSWFProxy::GetCurObj()
{
	return gldDataKeeper::Instance()->m_curObj;
}

gldLayer *CSWFProxy::GetCurLayer()
{
	return GetCurMovieClip()->GetCurLayerPtr();
}

int CSWFProxy::GetCurLayerIndex()
{
	gldMovieClip *pMC = GetCurMovieClip();
	return pMC->m_curLayer;
}

int CSWFProxy::GetCurTime()
{
	return GetCurMovieClip()->m_curTime;
}

GFRAMECLIP_LIST_IT CSWFProxy::GetCurFrameClipIndex()
{
	return GetCurLayer()->GetFrameClipIndex(GetCurTime());
}

gldFrameClip *CSWFProxy::GetCurFrameClip()
{
	return GetCurLayer()->GetFrameClip(GetCurTime());
}

void CSWFProxy::UpdateCurFrameClipValidFlags()
{	
	gldFrameClip	*pFC = NULL;

	if ((pFC = GetCurFrameClip()) != NULL)
	{
		UpdateFrameClipValidFlags(pFC);
	}
}

void CSWFProxy::UpdateFrameClipValidFlags(gldFrameClip *pFC)
{		
	UpdateFrameClipValidFlag(pFC);

	if ((pFC = GetPrevFrameClip(pFC)) != NULL)
	{
		UpdateFrameClipValidFlag(pFC);
	}	
}

void CSWFProxy::UpdateFrameClip(gldFrameClip *pFC)
{
	ASSERT(pFC);

	UpdateFrameClipValidFlag(pFC);

	if (pFC->IsMotion() && pFC->IsValid())
	{
		pFC->CreateMotionTween();
	}
	else
	{
		SetFrameClipStatic(pFC);
	}	
}

void CSWFProxy::SetFrameClipStatic(gldFrameClip *pFC)
{
	ASSERT(pFC);

	for (GCHARACTERKEY_LIST_IT it = pFC->m_characterKeyList.begin(); it != pFC->m_characterKeyList.end(); it++)
    {
        gldCharacterKey *_key = (*it);
		ASSERT(_key->GetCharacterCount() > 0);
		gldCharacter *_firstCharacter = _key->GetCharacter(0);
        gldMatrix _gmx = _firstCharacter->m_matrix;
        gldCxform _gcx = _firstCharacter->m_cxform;
        for (int i = 1; i < pFC->m_length; i++)
        {
			_key->GetCharacter(i)->m_matrix = _gmx;
            _key->GetCharacter(i)->m_cxform = _gcx;
        }
    }
}

int CSWFProxy::GetFrameClipEndTime(gldFrameClip *pFC)
{
	return pFC->m_startTime + pFC->m_length;
}

bool CSWFProxy::TimeInFrameClip(gldFrameClip *pFC, int nTime)
{
	return nTime >= pFC->m_startTime - 1 && nTime < GetFrameClipEndTime(pFC);
}

void CSWFProxy::RefreshAllMotionTweenInLayer(gldLayer *pLayer)
{
	ASSERT(pLayer);

	for (GFRAMECLIP_LIST_IT i = pLayer->m_frameClipList.begin(); i != pLayer->m_frameClipList.end(); ++i)
	{
		gldFrameClip	*pFC = *i;
		if (pFC->IsMotion() && pFC->IsValid())
		{
			pFC->CreateMotionTween();
		}
	}
}

void CSWFProxy::RefreshMotionTween(gldFrameClip *pFC)
{
	if (pFC->IsMotion() && pFC->IsValid())
	{
		pFC->UpdateCharacters();
	}
}

gldCharacterKey *CSWFProxy::CloneCkForFrameClip(gldFrameClip *pFrame, gldCharacterKey *pTemp
											  , const gldMatrix &mx, const gldCxform &cx
											  , int ratio /*= 0*/)
{
	gldCharacterKey *_cloner = CloneCkForFrameClip(pFrame, pTemp);
	//_cloner->ResetCList(NULL,pFrame->m_length,true);
    for (int i = 0; i < pFrame->m_length; i++)
    {
        gldCharacter *_character = new gldCharacter();
        _character->m_matrix = mx;
        _character->m_cxform = cx;
        _character->m_key = _cloner;
        _character->m_ratio = ratio;
		_cloner->IncreaseCList( _character);
    }    

    return _cloner;
}

gldCharacterKey *CSWFProxy::CloneCkForFrameClip(gldFrameClip *pFrame, gldCharacterKey *pTemp)
{
	gldCharacterKey *_cloner = _Factory()->CreateCharacterKey();

    _cloner->m_parentFC = pFrame;
    _cloner->m_ptr = NULL;

    _cloner->SetObj(pTemp->GetObj());    
    _cloner->m_depth = pTemp->m_depth;
    _cloner->m_name = pTemp->m_name;
    _cloner->m_clipDepth = pTemp->m_clipDepth;

    if (pTemp->m_action != NULL)
    {
        ASSERT(_cloner->m_action == NULL);
        _cloner->m_action = (gldAction *)pTemp->m_action->Clone();
    }
    _cloner->m_tx = pTemp->m_tx;
    _cloner->m_ty = pTemp->m_ty;
	_cloner->m_cxStyle = pTemp->m_cxStyle;
    _cloner->m_tint = pTemp->m_tint;

	_cloner->ResetCList(NULL,pFrame->m_length,true);

	return _cloner;
}

void CSWFProxy::PutCkToFrameClip(gldFrameClip *pFrame, gldCharacterKey *pKey)
{	
	pFrame->m_flag |= FC_CHARACTER;	
	
	pFrame->m_characterKeyList.push_back(pKey);
	
	pKey->m_parentFC = pFrame;	
}

void CSWFProxy::PutSelToFrameClip(gldFrameClip *pFrame, const gld_shape_sel &sel, int offsetx, int offsety)
{
	gld_shape_sel::iterator isel = sel.rbegin();

	for (; isel != sel.rend(); --isel)
	{		
		gldCharacter	*pChar = CTraitCharacter(*isel);		
		gldMatrix		gmat = pChar->m_matrix;
		gmat.m_x += offsetx;
		gmat.m_y += offsety;
		PutCkToFrameClip(pFrame, CloneCkForFrameClip(pFrame, pChar->m_key,
			gmat, pChar->m_cxform));
		//pFrame->AddCharacter(pChar->m_key->GetObj(), gmat, pChar->m_cxform);
	}
}

void CSWFProxy::PutSelToFrameClipCenter(gldFrameClip *pFrame, const gld_shape_sel &sel)
{
	gld_rect	box = sel.SurroundBox();
	int			offsetx = -(box.left + box.right) / 2;
	int			offsety = -(box.top + box.bottom) / 2;

	PutSelToFrameClip(pFrame, sel, offsetx, offsety);
}

void CSWFProxy::PrepareTShapeForGObj(gldObj *pObj)
{
	if (pObj->m_ptr == NULL)
	{
		CSWFParse	parse;
		CTransAdaptor::CreateTShapePtr(&parse, pObj);
		CTransAdaptor::UpdateTShape(pObj);
	}
}

gldSprite *CSWFProxy::NewGroup()
{
	gldSprite		*pSprite = new gldSprite;
	gldMovieClip	*pMovie = pSprite->m_mc;
	
	pSprite->m_isGroup = true;
	pMovie->m_editFlags |= gldMovieClip::FLAG_GROUP;

	return pSprite;
}

gldSprite *CSWFProxy::CreateGroupFromSel(const gld_shape_sel &sel)
{	
	gldSprite		*pSprite = NewGroup();
	gldMovieClip	*pMovie = pSprite->m_mc;
	
	// Build a layer that contain the selection
	CString strLayerName;
	strLayerName.Format(IDS_LAYER_N1, 1);

	gldLayer		*pLayer = new gldLayer(strLayerName, 1);
	gldFrameClip	*pFrame = pLayer->GetFrameClip(0);
	ASSERT(pFrame);
	PutSelToFrameClipCenter(pFrame, sel);
	pMovie->AddLayer(pLayer);
	
	PrepareTShapeForGObj(pSprite);

	return pSprite;
}

gldButton *CSWFProxy::CreateButtonFromSel(const gld_shape_sel &sel)
{
	gldButton		*pButton = new gldButton;

	gldMovieClip	*pMovie = pButton->m_bmc;

	CString strLayerName;
	strLayerName.Format(IDS_LAYER_N1, 1);
	gldLayer		*pLayer = pMovie->m_layerList[0];
	ASSERT(pLayer);

	// remove frame clips but first	
	while (pLayer->m_frameClipList.size() > 1)
	{
		delete (*(--pLayer->m_frameClipList.end()));
		pLayer->m_frameClipList.pop_back();
	}
	// Build 4 frames for button
	/*for (int i = 0; i < 4; i++)
	{
		gldFrameClip	*pFrame = pLayer->GetFrameClip(i);
		ASSERT(pFrame);
		PutSelToFrameClipCenter(pFrame, sel);
	}*/
	gldFrameClip	*pFrame = pLayer->GetFrameClip(0);
	ASSERT(pFrame);
	PutSelToFrameClipCenter(pFrame, sel);

	PrepareTShapeForGObj(pButton);

	return pButton;
}

gldSprite *CSWFProxy::CreateSpriteFromSel(const gld_shape_sel &sel)
{
	
	gldSprite		*pSprite = new gldSprite;

	// Build a layer for sprite
	gldMovieClip	*pMovie = pSprite->m_mc;	

	CString strLayerName;
	strLayerName.Format(IDS_LAYER_N1, 1);
	gldLayer		*pLayer = new gldLayer(strLayerName, 1);	
	gldFrameClip	*pFrame = pLayer->GetFrameClip(0);	
	ASSERT(pFrame);
	PutSelToFrameClipCenter(pFrame, sel);
	pMovie->AddLayer(pLayer);
	
	PrepareTShapeForGObj(pSprite);

	return pSprite;
}

gldSprite *CSWFProxy::CreateSpriteFromButton(gldButton *pButton)
{
	ASSERT(pButton);

	gldSprite		*pSprite = new gldSprite;	
	
	// Copy button's movie clip
	gldMovieClip	*pBMovie = pButton->m_bmc;
	if (pBMovie)
	{
		if (pSprite->m_mc)
		{
			delete pSprite->m_mc;
		}
		pSprite->m_mc = pBMovie->Clone();
		ASSERT(pSprite->m_mc);
		pSprite->m_mc->m_editFlags &= ~gldMovieClip::FLAG_BUTTON;
		pSprite->m_mc->m_parentObj = pSprite;
	}	

	PrepareTShapeForGObj(pSprite);

	return pSprite;
}

gldButton *CSWFProxy::CreateButtonFromSprite(gldSprite *pSprite)
{
	ASSERT(pSprite);
	
	gldButton		*pButton = new gldButton;	
	
	// Copy sprite's movie clip to button
	gldMovieClip	*pSMovie = pSprite->m_mc;
	if (pSMovie)
	{
		if (pButton->m_bmc)
		{
			delete pButton->m_bmc;
		}
		pButton->m_bmc = pSMovie->Clone();
		ASSERT(pButton->m_bmc);
		pButton->m_bmc->m_editFlags |= gldMovieClip::FLAG_BUTTON;
		pButton->m_bmc->m_parentObj = pButton;
	}

	PrepareTShapeForGObj(pButton);

	return pButton;
}

gldCharacterKey *CSWFProxy::CreateCharacterKey(gldFrameClip *pFC, gldObj *pObj, const gldMatrix &mx, const gldCxform &cx)
{
	ASSERT(pFC && pObj);

	gldCharacterKey *pKey = _Factory()->CreateCharacterKey();
    pKey->SetObj(pObj);
	pKey->m_parentFC = pFC;
	pKey->ResetCList(NULL,pFC->m_length,true);	

    for (int i = 0; i < pFC->m_length; i++)
    {
        gldCharacter *pChar = new gldCharacter();
        pChar->m_matrix = mx;
        pChar->m_cxform = cx;
        pChar->m_key = pKey;
		pKey->IncreaseCList(pChar);		
    }

	return pKey;
}

gldShape *CSWFProxy::CreateShapeWrapImage(gldImage *gimage)
{
	TImage	*timage = CTraitImage(gimage);
	if (timage)
	{			
		TClippedBitmapFillStyle	*fs = new TClippedBitmapFillStyle(timage);
		int x = timage->Width() * 10;
		int y = timage->Height() * 10;
		gld_shape	image = TShapeBuilder::BuildRect(-x, -y, x, y, NULL, fs);
		// BuildRect will clone the fill style so delete this
		delete fs;
		gldShape	*gshape = new gldShape;
		gld_shape	*tshape = new gld_shape(image);
		gshape->m_ptr = tshape;
		CTransAdaptor::TShape2GShape(*tshape, *gshape);

		return gshape;
	}

	return NULL;
}

bool CSWFProxy::GObjIsValidGroup(gldObj *pObj)
{	
	if (!pObj->IsGObjInstanceOf(gobjSprite))
	{
		return false;
	}

	gldSprite    *pSprite = (gldSprite *)pObj;	
	gldMovieClip *pMovie = pSprite->m_mc;
	if (pMovie->m_layerList.size() != 1)
	{
		return false;
	}

	// first layer is stream sound layer
	gldLayer	 *pLayer = *pMovie->m_layerList.begin();
	if (pLayer->m_frameClipList.size() != 1)
	{
		return false;
	}

	gldFrameClip *pFrame = *pLayer->m_frameClipList.begin();
	if (pFrame->m_length != 1)
	{
		return false;
	}

	return pSprite->m_isGroup && ((pMovie->m_editFlags & gldMovieClip::FLAG_GROUP) == gldMovieClip::FLAG_GROUP);
}

void CSWFProxy::BreakApartTextToShapes(gld_shape &shape, OBJINSTANCE_LIST &lstInst)
{	
	//gldCharacter	*pChar = CTraitCharacter(shape);
	//gldObj			*pObj = CTraitCharacter(shape);	
	//ASSERT(pObj->IsGObjInstanceOf(gobjText));
	//gldText			*pText = (gldText *)pObj;

	//gldMatrix	mx = pChar->m_matrix;
	//gldCxform	cx = pChar->m_cxform;
	//
	//BreakApartTextToShapes(pText, mx, cx, lstInst);

	gldInstance *pInstance = CTraitInstance(shape);
	gldObj *pObj = pInstance->m_obj;	
	ASSERT(pObj->IsGObjInstanceOf(gobjText));
	gldText *pText = (gldText *)pObj;

	gldMatrix mx = pInstance->m_matrix;
	gldCxform cx = pInstance->m_cxform;	

	BreakApartTextToShapes(pText, mx, cx, lstInst);	
}

void CSWFProxy::BreakApartTextToShapes(gldText *pText, gldMatrix &mx, gldCxform &cx, OBJINSTANCE_LIST &lstInst)
{
	CTextEffect effect((gldText2 *)pText);	
	effect.SetMatrix(mx);
	effect.SetCxform(cx);
	effect.GetEffect(lstInst);

	return;	
}

void CSWFProxy::BreakApartTextToShapes(gldText *pText, gldMatrix &mx, gldCxform &cx, IGLD_Parameters *pIParameters, OBJINSTANCE_LIST &lstInst)
{	
	CTextEffect effect((gldText2 *)pText);

	VARIANT_BOOL bCustomFill = VARIANT_FALSE;
	GetParameter(pIParameters, EP_CUSTOM_FILL, &bCustomFill);
	if (bCustomFill == VARIANT_TRUE)
	{
		gldFillStyle *pFillStyle = NULL;

		CComPtr<IGLD_FillStyle> pIFillStyle;	
		GetParameter(pIParameters, EP_FILL_STYLE, &pIFillStyle);
		if (pIFillStyle != NULL)
		{
			HRESULT hr = CI2C::Create(pIFillStyle, &pFillStyle);
			if (FAILED(hr))
				throw 1;
			ASSERT(pFillStyle != NULL);
			effect.SetFill(pFillStyle);
			delete pFillStyle;

			int grad = 0;
			GetParameter(pIParameters, EP_FILL_ANGLE, &grad);
			effect.SetGradient(grad * 3.1415927 / 180);
		}
	}

	VARIANT_BOOL bApplyPart = VARIANT_TRUE;
	GetParameter(pIParameters, EP_FILL_INDIVIDUALLY, &bApplyPart);
	effect.SetApplyPart(bApplyPart == VARIANT_TRUE);

	VARIANT_BOOL bBorder = VARIANT_FALSE;
	GetParameter(pIParameters, EP_TEXT_BORDER, &bBorder);
	if (bBorder)
	{
		double width = 1;
		COLORREF borderColor = 0;
		GetParameter(pIParameters, EP_BORDER_WIDTH, &width);
		GetParameter(pIParameters, EP_BORDER_COLOR, &borderColor);
		effect.SetBorder((int)ceil(width * 20), gldColor(GetRValue(borderColor),
			GetGValue(borderColor), GetBValue(borderColor), 255));		
	}

	VARIANT_BOOL bShadow = VARIANT_FALSE;
	GetParameter(pIParameters, EP_ADD_SHADOW, &bShadow);	
	if (bShadow)
	{
		double shadowSize = 1;
		COLORREF shadowColor = 0;
		GetParameter(pIParameters, EP_SHADOW_SIZE, &shadowSize);
		GetParameter(pIParameters, EP_SHADOW_COLOR, &shadowColor);
		int shadowX = (int)ceil(shadowSize * 20);
		effect.SetShadow(shadowX, shadowX, gldColor(GetRValue(shadowColor),
			GetGValue(shadowColor), GetBValue(shadowColor), 255));
	}

	effect.SetMatrix(mx);
	effect.SetCxform(cx);
	effect.GetEffect(lstInst);
}

void CSWFProxy::SortSelLeftRight(gld_list<gld_shape> &lst)
{
	gld_list<gld_shape>		tmp;
	gld_list<gld_rect>		pos;
	gld_list<gld_shape>::iterator p1;
	gld_list<gld_shape>::iterator p2;
	gld_list<gld_rect>::iterator  p3;

	for (p1 = lst.begin(); p1 != lst.end(); ++p1)
	{
		gld_rect	rc = (*p1).effect_box();
		for (p3 = pos.begin(), p2 = tmp.begin(); p3 != pos.end(); ++p3, ++p2)
		{
			if ((*p3).left > rc.left || ((*p3).left == rc.left && (*p3).top > rc.top))
			{
				break;
			}
		}
		tmp.insert(*p1, p2);
		pos.insert(rc, p3);		
	}

	lst.clear();

	for (p2 = tmp.begin(); p2 != tmp.end(); ++p2)
	{
		lst.push_back(*p2);
	}
}

gldMainMovie2 *CSWFProxy::GetMainMovie()
{
	return _GetMainMovie2();
}

gldMovieClip *CSWFProxy::GetObjMovieClip(gldObj *pObj)
{
	if (pObj)
	{
		ASSERT(pObj->IsGObjInstanceOf(gobjButton) || pObj->IsGObjInstanceOf(gobjSprite));

		if (pObj->IsGObjInstanceOf(gobjButton))
		{
			gldButton *pBtn = (gldButton *)pObj;
			return pBtn->m_bmc;
		}
		else	// Is sprite
		{
			gldSprite *pSpt = (gldSprite *)pObj;
			return pSpt->m_mc;
		}
	}
	else	// Is main movie
	{
		return gldDataKeeper::Instance()->GetCurMovieClip();
	}
}

gldLayer *CSWFProxy::GetObjFirstLayer(gldObj *pObj)
{
	gldMovieClip *pMC = GetObjMovieClip(pObj);
	return pMC->m_layerList[0];
}

int CSWFProxy::GetObjRowData(gldObj *pObj, U8 **pData)
{
	ASSERT(*pData == NULL && pObj != NULL);

	oBinStream	os;
	pObj->WriteToBinStream(os);
	if (os.Size() > 0)
	{
		*pData = new U8[os.Size()];
		os.WriteToMemory(*pData);
	}

	return os.Size();
}

void CSWFProxy::GetRefObjList(gldObj *pObj, list<gldObj *> &lstObj)
{
	lstObj.clear();

	if (pObj)
	{	
		for (GCHARACTERKEY_LIST_IT i = pObj->m_cKeyList.begin(); i != pObj->m_cKeyList.end(); ++i)
		{
			GetRefObjList((*i), lstObj);
		}
	}
}

void CSWFProxy::GetRefObjList(gldCharacterKey *pKey, list<gldObj *> &lstObj)
{
	if (pKey->m_parentFC->m_parentLayer->m_parentMC != NULL)
	{
		gldObj *pObj = pKey->m_parentFC->m_parentLayer->m_parentMC->m_parentObj;

		if (pObj != NULL && find(lstObj.begin(), lstObj.end(), pObj) == lstObj.end())
		{
			lstObj.push_back(pObj);

			for (GCHARACTERKEY_LIST_IT i = pObj->m_cKeyList.begin(); i != pObj->m_cKeyList.end(); ++i)
			{
				GetRefObjList((*i), lstObj);	
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTraitCharacter::CTraitCharacter(gld_shape &shape)
: m_pCharacter((gldCharacter *)shape.ptr())
{
	ASSERT(m_pCharacter);
}

CTraitCharacter::operator gldCharacter*()
{
	return m_pCharacter;
}

CTraitCharacter::operator gldCharacterKey*()
{
	return m_pCharacter->m_key;
}

CTraitCharacter::operator gldObj*()
{
	return m_pCharacter->m_key->GetObj();
}

void CTraitCharacter::Detach(gld_shape &shape)
{
	Detach((gldCharacter *)CTraitCharacter(shape));
}

void CTraitCharacter::Detach(gldCharacter *pChar)
{
	if (pChar && pChar->m_key->m_ptr)
	{
		gld_shape(pChar->m_key->m_ptr).ptr(NULL);
		pChar->m_key->m_ptr = NULL;
	}
}

void CTraitCharacter::Attach(gldCharacter *pChar, gld_shape &shape)
{
	ASSERT(pChar && shape.validate());

	pChar->m_key->m_ptr = shape.get_row_data();
	shape.ptr(pChar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTraitInstance::CTraitInstance(gld_shape &shape)
: m_pInstance((GInstance *)shape.ptr())
{
	ASSERT(m_pInstance);
}

CTraitInstance::CTraitInstance(GInstance *pInstance)
: m_pInstance(pInstance)
{
}

CTraitInstance::operator GInstance*()
{
	return m_pInstance;
}

CTraitInstance::operator gldObj*()
{
	ASSERT(m_pInstance);
	return m_pInstance->m_obj;
}

void CTraitInstance::Detach(gld_shape &shape)
{
	Detach((GInstance*)CTraitInstance(shape));
}

void CTraitInstance::Detach(GInstance *pInstance)
{
	if (pInstance && pInstance->m_ptr)
	{
		gld_shape(pInstance->m_ptr).ptr(NULL);
		pInstance->m_ptr = NULL;
	}
}

void CTraitInstance::Attach(GInstance *pInstance, gld_shape &shape)
{
	ASSERT(pInstance && shape.validate());

	pInstance->m_ptr = shape.get_row_data();
	shape.ptr(pInstance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTraitShape::CTraitShape(gldCharacter *pChar)
: m_Shape(pChar->m_key->m_ptr)
{
}

CTraitShape::CTraitShape(gldCharacterKey *pKey)
: m_Shape(pKey->m_ptr)
{
}

CTraitShape::CTraitShape(gldObj *pObj)
: m_Shape(*(gld_shape *)pObj->m_ptr)
{
}

CTraitShape::CTraitShape(gldInstance *pInstance)
: m_Shape(pInstance->m_ptr)
{
}

CTraitShape::operator gld_shape()
{
	return m_Shape;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTraitLayer::CTraitLayer(gld_layer &layer)
: m_layer(layer)
{	
}

CTraitLayer::CTraitLayer(gldLayer *pLayer)
: m_layer(pLayer->m_ptr)
{	
}

CTraitLayer::CTraitLayer(gldInstance *pInstance)
{
	m_layer = my_app.CurFrame().cur_layer();
}

CTraitLayer::operator gld_layer()
{
	return m_layer;
}

CTraitLayer::operator gldLayer*()
{
	return (gldLayer *)m_layer.ptr();
}

void CTraitLayer::Detach(gldLayer *pLayer)
{
	if (pLayer && pLayer->m_ptr)
	{
		gld_layer(pLayer->m_ptr).ptr(NULL);
		pLayer->m_ptr = NULL;
	}
}

void CTraitLayer::Detach(gld_layer &layer)
{
	Detach((gldLayer *)CTraitLayer(layer));
}

void CTraitLayer::Attach(gldLayer *pLayer, gld_layer &layer)
{
	ASSERT(pLayer && layer.validate());

	pLayer->m_ptr = layer.get_row_data();	
	layer.ptr(pLayer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTraitImage::CTraitImage(TImage *ptImg)
: m_pImage((gldImage *)ptImg->Ptr())
{	
}

CTraitImage::CTraitImage(gldImage *pgImg)
: m_pImage(pgImg)
{	
}

CTraitImage::operator gldImage*()
{
	return m_pImage;
}

CTraitImage::operator TImage*()
{
	return (TImage *)m_pImage->m_ptr;
}

void CTraitImage::Bind(gldImage *pgImg, TImage *ptImg)
{
	Unbind(pgImg);
	Unbind(ptImg);

	pgImg->m_ptr = ptImg;
	ptImg->Ptr(pgImg);
}

void CTraitImage::Unbind(gldImage *pgImg)
{
	if (pgImg->m_ptr)
	{
		TImage *ptImg = (TImage *)pgImg->m_ptr;
		ASSERT(ptImg->Ptr() == (void *)pgImg);

		ptImg->Ptr(NULL);
		pgImg->m_ptr = NULL;
	}
}

void CTraitImage::Unbind(TImage *ptImg)
{
	if (ptImg->Ptr())
	{		
		gldImage *pgImg = (gldImage *)ptImg->Ptr();
		ASSERT(pgImg->m_ptr == (void *)ptImg);

		ptImg->Ptr(NULL);
		pgImg->m_ptr = NULL;
	}
}

bool CTraitImage::Binded(gldImage *pgImg)
{
	return pgImg->m_ptr != NULL;
}

bool CTraitImage::Binded(TImage *ptImg)
{
	return ptImg->Ptr() != NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CGuardUpdateCurFCUI::CGuardUpdateCurFCUI()
{
	m_pOldFC = CSWFProxy::GetCurFrameClip();
	m_nOldTime = CSWFProxy::GetCurTime();
}

CGuardUpdateCurFCUI::~CGuardUpdateCurFCUI()
{
	if (m_nOldTime == CSWFProxy::GetCurTime())
	{
		if (m_pOldFC != CSWFProxy::GetCurFrameClip())
		{
			CSubjectManager::Instance()->GetSubject("ChangeCurFrame")->Notify(0);
		}
	}
}
