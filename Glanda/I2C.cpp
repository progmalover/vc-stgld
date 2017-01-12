#include "StdAfx.h"
#include ".\i2c.h"

#include "gldShape.h"
#include "gldLineStyle.h"
#include "gldFillStyle.h"
#include "gldColor.h"
#include "gldMatrix.h"
#include "gldGradient.h"
#include "gldGradientRecord.h"
#include "gldShapeRecordChange.h"
#include "gldShapeRecordEdgeStraight.h"
#include "gldShapeRecordEdgeCurved.h"

#include "gldButton.h"
#include "gldSprite.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldCharacter.h"
#include "gldImage.h"

#include "SWFProxy.h"
#include "SWFParse.h"
#include "TransAdaptor.h"
#include "TextToolEx.h"

#include "my_app.h"
#include "CmdAddShape.h"
#include "global.h"
#include "ObjectMap.h"
#include "gldSound.h"

#include "toolsdef.h"
#include "ITextTool.h"
#include "TextToolEx.h"
#include "filepath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CHECK(exp) ((exp) ? true : false)
#define OLE2AEX(str) (((str) != NULL) ? OLE2A(str) : _T(""))

CI2CMap *CI2C::m_pI2CMap = NULL;

void CI2CMap::CopyTo(CObjectMap *pObjectMap)
{
	ASSERT(pObjectMap != NULL);

	_I2CMap::iterator i = m_Map.begin();
	for (; i != m_Map.end(); ++i)
	{
		gldObj *pObj = (*i).second;
		if (pObj->m_name.empty())
			pObjectMap->GenerateNextSymbolName(pObj->m_name);
		pObjectMap->Use(pObj);
	}
}

void CI2CMap::FreeAllSymbol()
{
	for (_I2CMap::iterator i = m_Map.begin(); i != m_Map.end(); ++i)
		CObjectMap::FreeSymbol(&(*i).second);

	m_Map.clear();
}

template <typename I, typename T>
HRESULT SetLinkage(I *pIObj, T *pObj)
{
	HRESULT hr = S_OK;
	GObjFlag objFlag = gobjNormalFlag;
	CComQIPtr<IGLD_Object2> pIObject2 = pIObj;
	CComPtr<IGLD_Linkage> pILinkage;
	if (pIObject2 != NULL)
	{
		hr = pIObject2->get_Linkage(&pILinkage);
		CHECKRESULT();
		GLD_LinkageType type = gceLinkageNone;
		hr = pILinkage->get_Type(&type);
		CHECKRESULT();
		if (type == gceLinkageImport)
			objFlag = gobjImportFlag;
		else if (type == gceLinkageExport)
			objFlag = gobjExportFlag;
	}
	pObj->m_objFlag = objFlag;
	if (objFlag == gobjNormalFlag)
	{		
		pObj->m_linkageName.empty();
		pObj->m_linkageURL.empty();
	}
	else
	{
		CComBSTR bstrName, bstrURL;
		hr = pILinkage->get_Name(&bstrName);
		CHECKRESULT();
		hr = pILinkage->get_URL(&bstrURL);
		CHECKRESULT();
		USES_CONVERSION;
		pObj->m_linkageName = OLE2AEX(bstrName);
		pObj->m_linkageURL = OLE2AEX(bstrURL);
	}
	return hr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CI2C::CI2C(void)
{
}

CI2C::~CI2C(void)
{
}

HRESULT CI2C::Create(IGLD_Object *pIObject, gldObj **ppObject)
{
	IF_ARG_NULL_RETURN(pIObject);
	IF_ARG_NULL_RETURN(ppObject);

	HRESULT hr;

	GLD_ObjectType type;
	hr = pIObject->get_Type(&type);
	CHECKRESULT();

	if (type == gceShape)
	{	
		CComQIPtr<IGLD_Shape> pIShape = pIObject;
		if (pIShape == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = Create(pIShape, (gldShape **)ppObject);	
	}
	else if (type == gceButton)
	{
		CComQIPtr<IGLD_Button> pIButton = pIObject;
		if (pIButton == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = Create(pIButton, (gldButton **)ppObject);
	}
	else if (type == gceSprite)
	{
		CComQIPtr<IGLD_Sprite> pISprite = pIObject;
		if (pISprite == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = Create(pISprite, (gldSprite **)ppObject);
	}
	else if (type == gceImage)
	{
		CComQIPtr<IGLD_Image> pIImage = pIObject;
		if (pIImage == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = Create(pIImage, (gldImage **)ppObject);
	}
	else if (type == gceSound)
	{
		CComQIPtr<IGLD_Sound> pISound = pIObject;
		if (pISound == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = Create(pISound, (gldSound **)ppObject);
	}
	else if (type == gceText)
	{
		CComQIPtr<IGLD_Text> pIText = pIObject;
		if (pIText == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = Create(pIText, (gldText2 **)ppObject);
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}

HRESULT CI2C::Convert(IGLD_MovieClip *pIMovieClip, gldMovieClip *pMovieClip)
{
	IF_ARG_NULL_RETURN(pIMovieClip);
	IF_ARG_NULL_RETURN(pMovieClip);

	USES_CONVERSION;

	HRESULT hr;

	CComPtr<IGLD_Action> pIAction;
	hr = pIMovieClip->get_InitClipAction(&pIAction);
	CHECKRESULT();
	VARIANT_BOOL bEmpty = VARIANT_TRUE;
	hr = pIAction->IsEmpty(&bEmpty);
	CHECKRESULT();
	if (!bEmpty)
	{
		gldAction *pAction = pMovieClip->m_initAction;
		if (pAction == NULL)
		{
			pAction = new gldAction;
		}
		pAction->m_type = actionSegInitSprite;
		hr = Convert(pIAction, pAction);
		CHECKRESULT();
		pMovieClip->m_initAction = pAction;
	}

	CComPtr<IGLD_Layers> pILayers;
	hr = pIMovieClip->get_Layers(&pILayers);
	CHECKRESULT();

	CComPtr<IEnumVARIANT> enumVar;
	hr = pILayers->get__NewEnum(&enumVar);
	CHECKRESULT();
	CComVariant varLayer;
	while (enumVar->Next(1, &varLayer, NULL) == S_OK)
	{
		CComQIPtr<IGLD_Layer> pILayer = varLayer.punkVal;
		if (pILayer == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		varLayer.Clear();

		// create a layer
		CComBSTR bstrName;
		hr = pILayer->get_Name(&bstrName);
		CHECKRESULT();

		CAutoPtr<gldLayer> pLayer(new gldLayer(CString(bstrName), 0));

		// set layer property
		GLD_LayerType layerType;
		hr = pILayer->get_Type(&layerType);
		CHECKRESULT();

		if (layerType == gceMaskLayer)
			pLayer->m_mask = true;
		else if (layerType == gceMaskedLayer)
			pLayer->m_masked = true;
		else if (layerType != gceNormalLayer)
			hr = E_UNEXPECTED;
		CHECKRESULT();		

		// guide lines
		CComPtr<IGLD_CurvedPathes> pIGLs;
		hr = pILayer->get_GuideLines(&pIGLs);
		CHECKRESULT();
		CComPtr<IEnumVARIANT> pIEnumGL;
		hr = pIGLs->get__NewEnum(&pIEnumGL);
		CHECKRESULT();
		CComVariant varGL;
		while (pIEnumGL->Next(1, &varGL, NULL) == S_OK)
		{
			CComQIPtr<IGLD_CurvedPath> pIPath = varGL.punkVal;
			if (pIPath == NULL)
			{
				return E_NOINTERFACE;
			}
			varGL.Clear();
			tGuidLine *pGL = NULL;
			hr = Create(pIPath, &pGL);
			CHECKRESULT();
			pLayer->m_guidlineList.push_back(pGL);
		}

		CComPtr<IGLD_FrameClips> pIFrameClips;
		hr = pILayer->get_FrameClips(&pIFrameClips);
		CHECKRESULT();
		long nFrameClips = 1;

		CComPtr<IEnumVARIANT> pIEnum;
		hr = pIFrameClips->get__NewEnum(&pIEnum);
		CHECKRESULT();
		CComVariant var;
		CComPtr<IGLD_FrameClip> pIPrevFrameClip;
		gldFrameClip *pPrevFrameClip;

		while (pIEnum->Next(1, &var, 0) == S_OK)
		{
			CComPtr<IGLD_FrameClip> pIFrameClip;
			hr = var.punkVal->QueryInterface(__uuidof(IGLD_FrameClip), (void **)&pIFrameClip);
			CHECKRESULT();

			var.Clear();

			long nStartTime, nLength;
			hr = pIFrameClip->get_StartTime(&nStartTime);
			CHECKRESULT();
			hr = pIFrameClip->get_Length(&nLength);
			CHECKRESULT();
			gldFrameClip *pFrameClip = new gldFrameClip((U16)nStartTime, (U16)nLength);
			pFrameClip->m_parentLayer = pLayer;			
			pLayer->m_frameClipList.push_back(pFrameClip);
			CComBSTR bstrLabel;
			hr = pIFrameClip->get_Label(&bstrLabel);
			CHECKRESULT();
			pFrameClip->m_label = OLE2AEX(bstrLabel);
			
			pIAction.Release();
			hr = pIFrameClip->get_Action(&pIAction);
			CHECKRESULT();
			hr = pIAction->IsEmpty(&bEmpty);
			CHECKRESULT();
			if (!bEmpty)
			{
				if (pFrameClip->m_action == NULL)
					pFrameClip->m_action = new gldAction;				
				pFrameClip->m_action->m_type = actionSegFrame;
				hr = Convert(pIAction, pFrameClip->m_action);
				CHECKRESULT();
			}
			
			CComPtr<IGLD_Sound> pISound;
			hr = pIFrameClip->get_Sound(&pISound);
			CHECKRESULT();
			if (pISound != NULL)
			{
				gldSound *pSound;
				hr = Create(pISound, &pSound);
				CHECKRESULT();
				pFrameClip->m_sound->m_soundObj = pSound;
				pFrameClip->m_sound->m_soundType = soundEvent;
			}			
			CComPtr<IGLD_Motion> pIMotion;
			hr = pIFrameClip->get_Motion(&pIMotion);
			CHECKRESULT();
			hr = Convert(pIMotion, &pFrameClip->m_motion);
			CHECKRESULT();

			CComPtr<IGLD_CharacterKeys> pICharacterKeys;
			hr = pIFrameClip->get_CharacterKeys(&pICharacterKeys);
			CHECKRESULT();

			CComPtr<IEnumVARIANT> enumKeys;
			hr = pICharacterKeys->get__NewEnum(&enumKeys);
			CHECKRESULT();
			CComVariant varKey;
			while (enumKeys->Next(1, &varKey, NULL) == S_OK)
			{
				CComQIPtr<IGLD_CharacterKey> pICharacterKey = varKey.punkVal;
				if (pICharacterKey == NULL)
					hr = E_UNEXPECTED;
				CHECKRESULT();
				varKey.Clear();

				CComPtr<IGLD_Object> pIObject;
				hr = pICharacterKey->get_Object(&pIObject);
				CHECKRESULT();

				gldObj *pObject;
				hr = Create(pIObject, &pObject);
				CHECKRESULT();

				CComPtr<IGLD_Matrix> pIMatrix;
				CComPtr<IGLD_Cxform> pICxform;
				hr = pICharacterKey->get_Matrix(&pIMatrix);
				CHECKRESULT();
				hr = pICharacterKey->get_Cxform(&pICxform);
				CHECKRESULT();

				gldMatrix _matrix;
				gldCxform _cxform;
				hr = Convert(pIMatrix, &_matrix);
				CHECKRESULT();
				hr = Convert(pICxform, &_cxform);
				CHECKRESULT();
				gldCharacterKey *pKey = pFrameClip->AddCharacter(pObject, _matrix, _cxform, 0);
				pIAction.Release();
				hr = pICharacterKey->get_Action(&pIAction);
				CHECKRESULT();
				hr = pIAction->IsEmpty(&bEmpty);
				CHECKRESULT();
				if (!bEmpty)
				{
					if (pKey->m_action == NULL)
						pKey->m_action = new gldAction;					
					pKey->m_action->m_type = (pObject->IsGObjInstanceOf(gobjButton) ? actionSegButton : actionSegSprite);
					hr = Convert(pIAction, pKey->m_action);
					CHECKRESULT();
				}
				CComBSTR bstrName;
				hr = pICharacterKey->get_Name(&bstrName);
				CHECKRESULT();
				pKey->m_name = OLE2AEX(bstrName);
			}

			if (pIPrevFrameClip != NULL)
			{
				GLD_FrameClipType type;
				hr = pIPrevFrameClip->get_Type(&type);
				CHECKRESULT();
				if (type == gceMotionFrameClip)
					pPrevFrameClip->CreateMotionTween();
			}
			pIPrevFrameClip = pIFrameClip;
			pPrevFrameClip = pFrameClip;
		}		

		// add the layer to mc
		pMovieClip->AddLayer(pLayer.Detach());
	}

	GLAYER_LIST &layerList = pMovieClip->m_layerList;
	if (layerList.size() < 1)
		pMovieClip->AddLayer(new gldLayer(_T("Layer 1"), 0));
	pMovieClip->m_curLayer = 0;

	return S_OK;
}

HRESULT CI2C::GetShowList(IGLD_MovieClip *pIMovieClip, int nTime, gldMovieClip *pMovieClip)
{
	IF_ARG_NULL_RETURN(pIMovieClip);
	IF_ARG_NULL_RETURN(pMovieClip);

	HRESULT hr;

	CComPtr<IGLD_Layers> pILayers;
	hr = pIMovieClip->get_Layers(&pILayers);
	CHECKRESULT();

	CComPtr<IEnumVARIANT> enumVar;
	hr = pILayers->get__NewEnum(&enumVar);
	CHECKRESULT();
	CComVariant varLayer;
	while (enumVar->Next(1, &varLayer, NULL) == S_OK)
	{
		CComQIPtr<IGLD_Layer> pILayer = varLayer.punkVal;
		if (pILayer == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		varLayer.Clear();

		// create a layer
		CAutoPtr<gldLayer> pLayer(new gldLayer("L", 0));

		// set layer property
		GLD_LayerType layerType;
		hr = pILayer->get_Type(&layerType);
		CHECKRESULT();

		if (layerType == gceMaskLayer)
			pLayer->m_mask = true;
		else if (layerType == gceMaskedLayer)
			pLayer->m_masked = true;
		else if (layerType != gceNormalLayer)
			hr = E_UNEXPECTED;
		CHECKRESULT();		

		CComPtr<IGLD_FrameClips> pIFrameClips;
		hr = pILayer->get_FrameClips(&pIFrameClips);
		CHECKRESULT();

		CComPtr<IGLD_FrameClip> pIFrameClip;
		pIFrameClips->get_Item(0, &pIFrameClip);
		if (pIFrameClip != NULL)
		{			
			gldFrameClip *pFrameClip = new gldFrameClip(0, 1);
			pFrameClip->m_parentLayer = pLayer;
			pLayer->m_frameClipList.push_back(pFrameClip);

			CComPtr<IGLD_CharacterKeys> pICharacterKeys;
			hr = pIFrameClip->get_CharacterKeys(&pICharacterKeys);
			CHECKRESULT();

			CComPtr<IEnumVARIANT> enumKeys;
			hr = pICharacterKeys->get__NewEnum(&enumKeys);
			CHECKRESULT();
			CComVariant varKey;
			while (enumKeys->Next(1, &varKey, NULL) == S_OK)
			{
				CComQIPtr<IGLD_CharacterKey> pICharacterKey = varKey.punkVal;
				if (pICharacterKey == NULL)
					hr = E_UNEXPECTED;
				CHECKRESULT();
				varKey.Clear();

				CComPtr<IGLD_Object> pIObject;
				hr = pICharacterKey->get_Object(&pIObject);
				CHECKRESULT();

				gldObj *pObject;
				hr = Create(pIObject, &pObject);
				CHECKRESULT();

				CComPtr<IGLD_Matrix> pIMatrix;
				CComPtr<IGLD_Cxform> pICxform;
				hr = pICharacterKey->get_Matrix(&pIMatrix);
				CHECKRESULT();
				hr = pICharacterKey->get_Cxform(&pICxform);
				CHECKRESULT();

				gldMatrix _matrix;
				gldCxform _cxform;
				hr = Convert(pIMatrix, &_matrix);
				CHECKRESULT();
				hr = Convert(pICxform, &_cxform);
				CHECKRESULT();
				gldCharacterKey *pKey = pFrameClip->AddCharacter(pObject, _matrix, _cxform, 0);				
			}			
		}

		// add the layer to mc
		pMovieClip->AddLayer(pLayer.Detach());
	}

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Motion *pIMotion, gldMotionTweenParam *pMotion)
{
	IF_ARG_NULL_RETURN(pIMotion);
	IF_ARG_NULL_RETURN(pMotion);	

	HRESULT hr;

	GLD_MoveMotionType moveType;
	hr = pIMotion->get_MoveMotionType(&moveType);
	CHECKRESULT();
	if (moveType == gceNoneMoveMotion)
		pMotion->moveType = MOTION_MOVE_NONE;
	else
		pMotion->moveType = MOTION_MOVE_AUTO;
	byte moveEase;
	hr = pIMotion->get_MoveEase(&moveEase);
	CHECKRESULT();
	pMotion->moveEase = moveEase;

	GLD_ScaleMotionType scaleType;
	hr = pIMotion->get_ScaleMotionType(&scaleType);
	CHECKRESULT();
	if (scaleType == gceNoneScaleMotion)
		pMotion->scaleType = MOTION_SCALE_NONE;
	else
		pMotion->scaleType = MOTION_SCALE_AUTO;
	byte scaleEase;
	hr = pIMotion->get_ScaleEase(&scaleEase);
	CHECKRESULT();
	pMotion->scaleEase = scaleEase;

	GLD_CxformMotionType cxformType;
	hr = pIMotion->get_CxformMotionType(&cxformType);
	CHECKRESULT();
	if (cxformType == gceNoneCxformMotion)
		pMotion->cxformType = MOTION_CXFORM_NONE;
	else
		pMotion->cxformType = MOTION_CXFORM_AUTO;
	byte cxformEase;
	hr = pIMotion->get_CxformEase(&cxformEase);
	CHECKRESULT();
	pMotion->cxformEase = cxformEase;

	GLD_RotateMotionType rotateType;
	hr = pIMotion->get_RotateMotionType(&rotateType);
	CHECKRESULT();
	if (rotateType == gceNoneRotateMotion)
		pMotion->rotateType = MOTION_ROTATE_NONE;
	else if (rotateType == gceCCWRotateMotion)
		pMotion->rotateType = MOTION_ROTATE_CCW;
	else if (rotateType == gceCWRotateMotion)
		pMotion->rotateType = MOTION_ROTATE_CW;
	else
		pMotion->rotateType = MOTION_ROTATE_AUTO;
	byte rotateEase;
	hr = pIMotion->get_RotateEase(&rotateEase);
	CHECKRESULT();
	pMotion->rotateEase = rotateEase;
	long rotateTimes;
	hr = pIMotion->get_RotateTimes(&rotateTimes);
	CHECKRESULT();
	pMotion->rotateTimes = rotateTimes;
	VARIANT_BOOL bSet;
	hr = pIMotion->get_SnapToPath(&bSet);
	CHECKRESULT();
	pMotion->snapToPath = CHECK(bSet);
	hr = pIMotion->get_OrientToPath(&bSet);
	CHECKRESULT();
	pMotion->orientToPath = CHECK(bSet);

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Button *pIButton, gldButton *pButton)
{
	IF_ARG_NULL_RETURN(pIButton);
	IF_ARG_NULL_RETURN(pButton);

	HRESULT hr;
	
	CComPtr<IGLD_MovieClip> pIMovieClip;
	hr = pIButton->get_MovieClip(&pIMovieClip);
	CHECKRESULT();

	gldMovieClip *pMovieClip = pButton->m_bmc;

	hr = Convert(pIMovieClip, pMovieClip);
	CHECKRESULT();

	pMovieClip->m_parentObj = pButton;

	CComBSTR bstrName;
	hr = pIButton->get_Name(&bstrName);
	CHECKRESULT();
	USES_CONVERSION;
	pButton->m_name = OLE2AEX(bstrName);

	hr = SetLinkage(pIButton, pButton);
	CHECKRESULT();

	UpdateTShapePtr(pButton);

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Sprite *pISprite, gldSprite *pSprite)
{
	IF_ARG_NULL_RETURN(pISprite);
	IF_ARG_NULL_RETURN(pSprite);

	HRESULT hr;

	CComPtr<IGLD_MovieClip> pIMovieClip;
	hr = pISprite->get_MovieClip(&pIMovieClip);
	CHECKRESULT();

	gldMovieClip *pMovieClip = pSprite->m_mc;

	hr = Convert(pIMovieClip, pMovieClip);
	CHECKRESULT();

	pMovieClip->m_parentObj = pSprite;

	CComBSTR bstrName;
	hr = pISprite->get_Name(&bstrName);
	CHECKRESULT();
	USES_CONVERSION;
	pSprite->m_name = OLE2AEX(bstrName);

	hr = SetLinkage(pISprite, pSprite);
	CHECKRESULT();

	UpdateTShapePtr(pSprite);

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_FillStyles *pIFills, GFillStyles *pFills)
{
	IF_ARG_NULL_RETURN(pIFills);
	IF_ARG_NULL_RETURN(pFills);
	
	HRESULT hr;
	CComPtr<IEnumVARIANT> enumVar;
	hr = pIFills->get__NewEnum(&enumVar);
	CHECKRESULT();
	CComVariant varFill;
	while (enumVar->Next(1, &varFill, NULL) == S_OK)
	{		
		CComQIPtr<IGLD_FillStyle> pIFill = varFill.punkVal;
		if (pIFill == NULL)
			return E_UNEXPECTED;
		varFill.Clear();
		gldFillStyle *pFill = NULL;
		hr = Create(pIFill, &pFill);
		CHECKRESULT();
		pFills->push_back(pFill);
	}

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_LineStyles *pILines, GLineStyles *pLines)
{
	IF_ARG_NULL_RETURN(pILines);
	IF_ARG_NULL_RETURN(pLines);

	HRESULT hr;
	CComPtr<IEnumVARIANT> enumVar;
	hr = pILines->get__NewEnum(&enumVar);
	CHECKRESULT();
	CComVariant varLine;
	while (enumVar->Next(1, &varLine, NULL) == S_OK)
	{		
		CComQIPtr<IGLD_LineStyle> pILine = varLine.punkVal;
		if (pILine == NULL)
			return E_UNEXPECTED;
		varLine.Clear();
		gldLineStyle *pLine = NULL;
		hr = Create(pILine, &pLine);
		CHECKRESULT();
		pLines->push_back(pLine);
	}

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Shape *pIShape, gldShape *pShape)
{
	IF_ARG_NULL_RETURN(pIShape);
	IF_ARG_NULL_RETURN(pShape);

	HRESULT hr;

	// Clear the gshape content
    pShape->Clear();

    // convert the bound box
    CComPtr<IGLD_Rect> pIRect;
	hr = pIShape->get_Bound(&pIRect);
	CHECKRESULT();
	double l, t, r, b;
	hr = pIRect->GetData(&l, &t, &r, &b);
	CHECKRESULT();
	pShape->m_bounds.left = (long)l;
    pShape->m_bounds.top = (long)t;
    pShape->m_bounds.right = (long)r;
    pShape->m_bounds.bottom = (long)b;

	// convert fill styles
	CComPtr<IGLD_FillStyles> pIFills;
	hr = pIShape->get_FillStyles(&pIFills);
	CHECKRESULT();
	hr = Convert(pIFills, pShape->GetFillStyles());
	CHECKRESULT();

	// convert line styles
	CComPtr<IGLD_LineStyles> pILines;
	hr = pIShape->get_LineStyles(&pILines);
	CHECKRESULT();
	hr = Convert(pILines, pShape->GetLineStyles());
	CHECKRESULT();

    // convert the shape records
	vector<gldShapeRecord *> &gsrs = *pShape->GetShapeRecords();
    CComPtr<IGLD_ShapeRecords> pIRecords;
	hr = pIShape->get_ShapeRecords(&pIRecords);
	CHECKRESULT();
	CComPtr<IEnumVARIANT> enumVar;
	hr = pIRecords->get__NewEnum(&enumVar);
	CHECKRESULT();
	CComVariant varRec;
	while (enumVar->Next(1, &varRec, NULL) == S_OK)
    {
		CComQIPtr<IGLD_ShapeRecord> pIRecord = varRec.punkVal;
		if (pIRecord == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		varRec.Clear();
		GLD_ShapeRecordType type;
		hr = pIRecord->get_Type(&type);
		CHECKRESULT();
		if (type == gceShapeRecordChange)
		{
			CComQIPtr<IGLD_ShapeRecordChange> src = pIRecord;
			if (src == NULL)
				hr = E_UNEXPECTED;
			CHECKRESULT();
			CAutoPtr<gldShapeRecordChange> gsrc(new gldShapeRecordChange);
			VARIANT_BOOL flag;
			hr = src->get_Flag(gceSetFillStyle0, &flag);
			CHECKRESULT();
			if (flag)
			{
				long fs0 = 0;
				hr = src->get_FillStyle0(&fs0);
				CHECKRESULT();
				gsrc->m_fs0 = fs0;
				gsrc->m_stateFS0 = true;
			}
			
			hr = src->get_Flag(gceSetFillStyle1, &flag);
			CHECKRESULT();
			if (flag)
			{
				long fs1 = 0;
				hr = src->get_FillStyle1(&fs1);
				CHECKRESULT();
				gsrc->m_fs1 = fs1;
				gsrc->m_stateFS1 = true;
			}
			
			hr = src->get_Flag(gceSetLineStyle, &flag);
			CHECKRESULT();
			if (flag)
			{
				long ls = 0;
				hr = src->get_LineStyle(&ls);
				CHECKRESULT();
				gsrc->m_ls = ls;
				gsrc->m_stateLS = true;
			}

			hr = src->get_Flag(gceSetMoveTo, &flag);
			CHECKRESULT();
			if (flag)
			{
				long x, y;
				hr = src->get_MoveToX(&x);
				CHECKRESULT();
				hr = src->get_MoveToY(&y);
				CHECKRESULT();
				gsrc->m_moveDX = x;
				gsrc->m_moveDY = y;
				gsrc->m_stateMoveTo = true;
			}

			hr = src->get_Flag(gceSetNewStyles, &flag);
			CHECKRESULT();
			if (flag)
			{
				CComPtr<IGLD_FillStyles> fss;
				hr = src->get_FillStyles(&fss);
				CHECKRESULT();
				hr = Convert(fss, &gsrc->m_fsTable);
				CHECKRESULT();
				CComPtr<IGLD_LineStyles> lss;
				hr = src->get_LineStyles(&lss);
				CHECKRESULT();
				hr = Convert(lss, &gsrc->m_lsTable);
				CHECKRESULT();
				gsrc->m_stateNewStyles = true;
			}

			gsrs.push_back(gsrc.Detach());
		}
		else if (type == gceShapeRecordStraight)
		{
			CComQIPtr<IGLD_ShapeRecordStraight> srs = pIRecord;
			if (srs == NULL)
				hr = E_UNEXPECTED;
			CHECKRESULT();
			long x, y;
			hr = srs->get_dx(&x);
			CHECKRESULT();
			hr = srs->get_dy(&y);
			CHECKRESULT();
			CAutoPtr<gldShapeRecordEdgeStraight> psrs(new gldShapeRecordEdgeStraight);
			psrs->m_dx = x;
			psrs->m_dy = y;

			gsrs.push_back(psrs.Detach());
		}
		else if (type == gceShapeRecordCurved)
		{
			CComQIPtr<IGLD_ShapeRecordCurved> src = pIRecord;
			if (src == NULL)
				hr = E_UNEXPECTED;
			CHECKRESULT();
			long ax, ay, cx, cy;
			hr = src->get_ax(&ax);
			CHECKRESULT();
			hr = src->get_ay(&ay);
			CHECKRESULT();
			hr = src->get_cx(&cx);
			CHECKRESULT();
			hr = src->get_cy(&cy);
			CHECKRESULT();
			CAutoPtr<gldShapeRecordEdgeCurved> psrc(new gldShapeRecordEdgeCurved);
			psrc->m_ax = ax;
			psrc->m_ay = ay;
			psrc->m_cx = cx;
			psrc->m_cy = cy;

			gsrs.push_back(psrc.Detach());
		}
		else
			hr = E_UNEXPECTED;

		CHECKRESULT();
    }

	CComBSTR bstrName;
	hr = pIShape->get_Name(&bstrName);
	CHECKRESULT();
	USES_CONVERSION;
	pShape->m_name = OLE2AEX(bstrName);
	
	hr = SetLinkage(pIShape, pShape);
	CHECKRESULT();

	UpdateTShapePtr(pShape);

	return S_OK;
}

void CI2C::UpdateTShapePtr(gldObj *pObj)
{
	//pObj->UpdateUniId(); // why do this? this is update TShapePtr???

	if (pObj->m_ptr == NULL)
	{
		CSWFParse parse;
		CTransAdaptor::CreateTShapePtr(&parse, pObj);
		gld_shape shape = CTraitShape(pObj);		
		gld_rect box = shape.calc_bound();
		shape.bound(box);
		pObj->m_bounds.left = box.left;
		pObj->m_bounds.top = box.top;
		pObj->m_bounds.right = box.right;
		pObj->m_bounds.bottom = box.bottom;

		ASSERT(pObj->m_ptr != NULL);
	}
}

HRESULT CI2C::Convert(IGLD_Color *pIColor, gldColor *pColor)
{
	IF_ARG_NULL_RETURN(pIColor);
	IF_ARG_NULL_RETURN(pColor);

	HRESULT hr;

	byte r, g, b, a;
	hr = pIColor->GetData(&r, &g, &b, &a);
	CHECKRESULT();
	pColor->red = r;
	pColor->green = g;
	pColor->blue = b;
	pColor->alpha = a;

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Matrix *pIMatrix, gldMatrix *pMatrix)
{
	IF_ARG_NULL_RETURN(pIMatrix);
	IF_ARG_NULL_RETURN(pMatrix);

	HRESULT hr;

	double e11, e12, e21, e22, dx, dy;

	hr = pIMatrix->GetData(&e11, &e12, &e21, &e22, &dx, &dy);
	CHECKRESULT();	
	pMatrix->m_e11 = (float)e11;	
	pMatrix->m_e12 = (float)e12;	
	pMatrix->m_e21 = (float)e21;	
	pMatrix->m_e22 = (float)e22;	
	pMatrix->m_x = (S32)dx;	
	pMatrix->m_y = (S32)dy;

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Cxform *pICxform, gldCxform *pCxform)
{
	IF_ARG_NULL_RETURN(pICxform);
	IF_ARG_NULL_RETURN(pCxform);
	
	HRESULT hr;

	double ra, rb, ga, gb, ba, bb, aa, ab;
	hr = pICxform->GetData(&ra, &rb, &ga, &gb, &ba, &bb, &aa, &ab);
	CHECKRESULT();
	
	pCxform->ra = (float)ra;
	pCxform->rb = (S32)rb;
	pCxform->ga = (float)ga;
	pCxform->gb = (S32)gb;
	pCxform->ba = (float)ba;
	pCxform->bb = (S32)bb;
	pCxform->aa = (float)aa;
	pCxform->ab = (S32)ab;

	return TRUE;
}

HRESULT CI2C::Convert(IGLD_FillStyle *pIFill, gldFillStyle *pFill)
{
	IF_ARG_NULL_RETURN(pIFill);
	IF_ARG_NULL_RETURN(pFill);

	HRESULT hr;

	GLD_FillStyleType fillType;
	hr = pIFill->get_Type(&fillType);
	CHECKRESULT();
	if (fillType == gceSolidFill)
	{
		CComQIPtr<IGLD_SolidFillStyle> solidFill = pIFill;
		if (solidFill == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		CComPtr<IGLD_Color> color;
		hr = solidFill->get_Color(&color);
		CHECKRESULT();
		pFill->SetFillStyleType(solid_fill);
		gldColor gColor;
		hr = Convert(color, &gColor);
		CHECKRESULT();
		pFill->SetSolidFillColor(gColor);		
	}
	else if (fillType == gceLinearGradientFill)
	{
		CComQIPtr<IGLD_LinearGradientFillStyle> lgFill = pIFill;
		if (lgFill == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		CComPtr<IGLD_GradientRecords> grds;
		hr = lgFill->get_GradientRecords(&grds);
		CHECKRESULT();
		CComPtr<IGLD_Matrix> mat;
		hr = lgFill->get_Matrix(&mat);
		CHECKRESULT();
		gldMatrix gmx;
		hr = Convert(mat, &gmx);
		CHECKRESULT();		
		gldGradient *ggrds = new gldGradient;
		pFill->SetFillStyleType(linear_gradient_fill);
		pFill->SetGradient(ggrds);
		pFill->SetGradientMatrix(gmx);
		CComPtr<IEnumVARIANT> enumVar;
		hr = grds->get__NewEnum(&enumVar);
		CHECKRESULT();
		CComVariant varGrad;
		while (enumVar->Next(1, &varGrad, NULL) == S_OK)
		{
			CComQIPtr<IGLD_GradientRecord> rec = varGrad.punkVal;
			if (rec == NULL)
				hr = E_UNEXPECTED;
			CHECKRESULT();
			varGrad.Clear();
			CComPtr<IGLD_Color> color;
			hr = rec->get_Color(&color);
			CHECKRESULT();
			byte ratio;
			hr = rec->get_Ratio(&ratio);
			CHECKRESULT();
			CAutoPtr<gldGradientRecord> grec(new gldGradientRecord);
			hr = Convert(color, &grec->m_color);
			CHECKRESULT();
			grec->m_ratio = ratio;
			ggrds->AddGradRecord(grec.Detach());
		}
	}
	else if (fillType == gceRadialGradientFill)
	{
		CComQIPtr<IGLD_RadialGradientFillStyle> rgFill = pIFill;
		if (rgFill == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		CComPtr<IGLD_GradientRecords> grds;
		hr = rgFill->get_GradientRecords(&grds);
		CHECKRESULT();
		CComPtr<IGLD_Matrix> mat;
		hr = rgFill->get_Matrix(&mat);
		CHECKRESULT();
		gldMatrix gmx;
		hr = Convert(mat, &gmx);
		CHECKRESULT();		
		gldGradient *ggrds = new gldGradient;
		pFill->SetFillStyleType(radial_gradient_fill);
		pFill->SetGradient(ggrds);
		pFill->SetGradientMatrix(gmx);
		CComPtr<IEnumVARIANT> enumVar;
		hr = grds->get__NewEnum(&enumVar);
		CHECKRESULT();
		CComVariant varGrad;
		while (enumVar->Next(1, &varGrad, NULL) == S_OK)
		{
			CComQIPtr<IGLD_GradientRecord> rec = varGrad.punkVal;
			if (rec == NULL)
				hr = E_UNEXPECTED;
			CHECKRESULT();
			varGrad.Clear();
			CComPtr<IGLD_Color> color;
			hr = rec->get_Color(&color);
			CHECKRESULT();
			byte ratio;
			hr = rec->get_Ratio(&ratio);
			CHECKRESULT();
			CAutoPtr<gldGradientRecord> grec(new gldGradientRecord);
			hr = Convert(color, &grec->m_color);
			CHECKRESULT();
			grec->m_ratio = ratio;
			ggrds->AddGradRecord(grec.Detach());
		}	
	}
	else if (fillType == gceClippedBitmapFill)
	{
		CComQIPtr<IGLD_ClippedBitmapFillStyle> pIClipped = pIFill;
		if (pIClipped == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		CComPtr<IGLD_Image> pIImage;
		hr = pIClipped->get_Image(&pIImage);
		CHECKRESULT();
		gldImage *pImage;
		hr = Create(pIImage, &pImage);
		CHECKRESULT();
		pFill->SetFillStyleType(clipped_bitmap_fill);
		pFill->SetBitmap(pImage);
		gldMatrix gmx;
		CComPtr<IGLD_Matrix> pIMx;
		hr = pIClipped->get_Matrix(&pIMx);
		CHECKRESULT();
		hr = Convert(pIMx, &gmx);
		CHECKRESULT();
		pFill->SetBitmapMatrix(gmx);
	}
	else if (fillType == gceTiledBitmapFill)
	{
		CComQIPtr<IGLD_TiledBitmapFillStyle> pITiled = pIFill;
		if (pITiled == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		CComPtr<IGLD_Image> pIImage;
		hr = pITiled->get_Image(&pIImage);
		CHECKRESULT();
		gldImage *pgImage;
		hr = Create(pIImage, &pgImage);
		CHECKRESULT();
		pFill->SetFillStyleType(tiled_bitmap_fill);
		pFill->SetBitmap(pgImage);
		gldMatrix gmx;
		CComPtr<IGLD_Matrix> pIMx;
		hr = pITiled->get_Matrix(&pIMx);
		CHECKRESULT();
		hr = Convert(pIMx, &gmx);
		CHECKRESULT();
		pFill->SetBitmapMatrix(gmx);
	}
	else
	{
		hr = E_UNEXPECTED;
	}

	return hr;
}

HRESULT CI2C::Convert(IGLD_Action *pIAction, gldAction *pAction)
{
	IF_ARG_NULL_RETURN(pIAction);
	IF_ARG_NULL_RETURN(pAction);

	HRESULT hr = S_OK;
	CComBSTR bstrSrc;
	hr = pIAction->GetSource(&bstrSrc);
	CHECKRESULT();
	USES_CONVERSION;
	pAction->SetSourceCode(OLE2AEX(bstrSrc));

	ULONG binSize = 0;
	hr = pIAction->GetBinary(NULL, &binSize);
	CHECKRESULT();
	if (binSize > 0)
	{
		BYTE *pBin = new BYTE[binSize];
		hr = pIAction->GetBinary(pBin, &binSize);
		CHECKRESULT();
		pAction->m_useCompiledActionBin = false;
		pAction->SetBinCode(pBin, binSize); // this function will delete pBin, why do this?????
		if (bstrSrc.Length() < 1)
			pAction->DecompileImportAction();
	}
	else
	{
		pAction->m_useCompiledActionBin = true;
	}

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_LineStyle *pILine, gldLineStyle *pLine)
{	
	IF_ARG_NULL_RETURN(pILine);
	IF_ARG_NULL_RETURN(pLine);

	HRESULT hr;

	long width;
	hr = pILine->get_Width(&width);
	CHECKRESULT();
	pLine->SetWidth(width);	
	CComPtr<IGLD_Color> color;
	hr = pILine->get_Color(&color);
	CHECKRESULT();
	gldColor gColor;
	hr = Convert(color, &gColor);
	CHECKRESULT();
	pLine->SetColor(gColor);

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_CurvedPath *pIPath, tGuidLine *pGuidLine)
{
	IF_ARG_NULL_RETURN(pIPath);
	IF_ARG_NULL_RETURN(pGuidLine);

	HRESULT hr = S_OK;
	SAFEARRAY *psap = NULL;
	hr = pIPath->get_Data(&psap);
	CHECKRESULT();
	if (psap == NULL)
	{
		return S_OK;
	}

	LONG lb = 0, ub = 0, cb = 0;
	hr = SafeArrayGetLBound(psap, 1, &lb);
	CHECKRESULT();
	hr = SafeArrayGetUBound(psap, 1, &ub);
	cb = ub - lb + 1;
	if (cb < 1)
	{
		SafeArrayDestroy(psap);
		return E_FAIL;
	}	
	
	DOUBLE *pvData = (DOUBLE *)psap->pvData;	

	tGuidPoint *a1 = (tGuidPoint *)malloc(sizeof(tGuidPoint));
	a1->type = GPT_QBEZIER_ANCHOR_POINT;
	a1->x = (int)pvData[0];
	a1->y = (int)pvData[1];
	tGuidLineInsertPointAfter(pGuidLine, a1, pGuidLine->head);
	pvData += 2;

	for (LONG i = 1; i < cb; i += 2)
	{	
		tGuidPoint	*pp[2];
		pp[0] = (tGuidPoint *)malloc(sizeof(tGuidPoint));
		pp[1] = (tGuidPoint *)malloc(sizeof(tGuidPoint));
		pp[0]->type = GPT_QBEZIER_CONTROL_POINT;
		pp[1]->type = GPT_QBEZIER_ANCHOR_POINT;
			
		pp[0]->x = (int)pvData[0];
		pp[0]->y = (int)pvData[1];
		pp[1]->x = (int)pvData[2];
		pp[1]->y = (int)pvData[3];
		
		tGuidLineInsertPointAfter(pGuidLine, pp[0], a1);
		tGuidLineInsertPointAfter(pGuidLine, pp[1], pp[0]);
		a1 = pp[1];

		pvData += 4;
	}

	return S_OK;
}

HRESULT CI2C::Create(IGLD_CurvedPath *pIPath, tGuidLine **ppGuidLine)
{
	IF_ARG_NULL_RETURN(pIPath);
	IF_ARG_NULL_RETURN(ppGuidLine);

	CAutoPtr<tGuidLine>	guidLine(new tGuidLine);
	tGuidLineCreate(guidLine.m_p);
	HRESULT hr = Convert(pIPath, guidLine.m_p);
	CHECKRESULT();

	*ppGuidLine = guidLine.Detach();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_LineStyle *pILine, gldLineStyle **ppLine)
{
	IF_ARG_NULL_RETURN(pILine);
	IF_ARG_NULL_RETURN(ppLine);

	HRESULT hr;

	CAutoPtr<gldLineStyle> pLine(new gldLineStyle);
	hr = Convert(pILine, pLine);
	CHECKRESULT();

	*ppLine = pLine.Detach();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_FillStyle *pIFill, gldFillStyle **ppFill)
{
	IF_ARG_NULL_RETURN(pIFill);
	IF_ARG_NULL_RETURN(ppFill);

	HRESULT hr;

	CAutoPtr<gldFillStyle> pFill(new gldFillStyle);
	hr = Convert(pIFill, pFill);
	CHECKRESULT();

	*ppFill = pFill.Detach();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_Image *pIImage, gldImage **ppImage)
{
	IF_ARG_NULL_RETURN(pIImage);
	IF_ARG_NULL_RETURN(ppImage);
	
	HRESULT hr;

	if ((m_pI2CMap != NULL) && (SUCCEEDED(m_pI2CMap->QueryObject(pIImage, (gldObj **)ppImage))))
		return S_OK;
	
	CAutoPtr<gldImage> pImage(new gldImage);
	hr = Convert(pIImage, pImage);
	CHECKRESULT();

	if (m_pI2CMap != NULL)
		m_pI2CMap->Insert(pIImage, pImage);

	*ppImage = pImage.Detach();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_Sound *pISound, gldSound **ppSound)
{
	IF_ARG_NULL_RETURN(pISound);
	IF_ARG_NULL_RETURN(ppSound);
	
	HRESULT hr;

	if ((m_pI2CMap != NULL) && (SUCCEEDED(m_pI2CMap->QueryObject(pISound, (gldObj **)ppSound))))
		return S_OK;
	
	CAutoPtr<gldSound> pSound(new gldSound);
	hr = Convert(pISound, pSound);
	CHECKRESULT();

	if (m_pI2CMap != NULL)
		m_pI2CMap->Insert(pISound, pSound);

	*ppSound = pSound.Detach();

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Image *pIImage, gldImage *pImage)
{
	IF_ARG_NULL_RETURN(pIImage);
	IF_ARG_NULL_RETURN(pImage);

	HRESULT hr;
	
	CString tmpFile = GetTempFile(NULL, "~image", NULL);
	CComPtr<IGLD_FileStream> stream;
	hr = stream.CoCreateInstance(__uuidof(GLD_FileStream));	
	CHECKRESULT();
	hr = stream->Open(CComBSTR(tmpFile), CComBSTR("wb"));
	CHECKRESULT();
	hr = pIImage->Save(stream);
	CHECKRESULT();
	hr = stream->Close();
	CHECKRESULT();
	if (!pImage->ReadImageData(tmpFile))
		return E_FAIL;
	DeleteFile(tmpFile);

	CComBSTR bstrName;
	hr = pIImage->get_Name(&bstrName);
	CHECKRESULT();
	USES_CONVERSION;
	pImage->m_name = OLE2AEX(bstrName);

	hr = SetLinkage(pIImage, pImage);
	CHECKRESULT();

	CSWFProxy::PrepareTShapeForGObj(pImage);

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Sound *pISound, gldSound *pSound)
{
	IF_ARG_NULL_RETURN(pISound);
	IF_ARG_NULL_RETURN(pSound);

	HRESULT hr;
	
	CString tmpFile = GetTempFile(NULL, "~sound", NULL);
	CComPtr<IGLD_FileStream> stream;
	hr = stream.CoCreateInstance(__uuidof(GLD_FileStream));	
	CHECKRESULT();
	hr = stream->Open(CComBSTR(tmpFile), CComBSTR("wb"));
	CHECKRESULT();
	hr = pISound->Save(stream);
	CHECKRESULT();
	hr = stream->Close();
	CHECKRESULT();
	if (!pSound->ReadFile(tmpFile))
		return E_FAIL;
	DeleteFile(tmpFile);

	CComBSTR bstrName;
	hr = pISound->get_Name(&bstrName);
	CHECKRESULT();
	USES_CONVERSION;
	pSound->m_name = OLE2AEX(bstrName);

	hr = SetLinkage(pISound, pSound);
	CHECKRESULT();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_Shape *pIShape, gldShape **ppShape)
{
	IF_ARG_NULL_RETURN(pIShape);
	IF_ARG_NULL_RETURN(ppShape);
	
	HRESULT hr;

	if ((m_pI2CMap != NULL) && (SUCCEEDED(m_pI2CMap->QueryObject(pIShape, (gldObj **)ppShape))))
		return S_OK;
	
	CAutoPtr<gldShape> pShape(new gldShape);
	hr = Convert(pIShape, pShape);
	CHECKRESULT();

	if (m_pI2CMap != NULL)
		m_pI2CMap->Insert(pIShape, pShape);

	*ppShape = pShape.Detach();

	return S_OK;
}

static void DeleteAllLayers(gldMovieClip *pMovie)
{
	GLAYER_LIST_IT iLayer = pMovie->m_layerList.begin();
	for (; iLayer != pMovie->m_layerList.end(); ++iLayer)
		delete *iLayer;
	pMovie->m_layerList.clear();
}

HRESULT CI2C::Create(IGLD_Button *pIButton, gldButton **ppButton)
{
	IF_ARG_NULL_RETURN(pIButton);
	IF_ARG_NULL_RETURN(ppButton);
	
	HRESULT hr;

	if ((m_pI2CMap != NULL) && (SUCCEEDED(m_pI2CMap->QueryObject(pIButton, (gldObj **)ppButton))))
		return S_OK;
	
	CAutoPtr<gldButton> pButton(new gldButton);
	DeleteAllLayers(pButton->m_bmc);
	hr = Convert(pIButton, pButton);
	CHECKRESULT();

	if (m_pI2CMap != NULL)
		m_pI2CMap->Insert(pIButton, pButton);

	*ppButton = pButton.Detach();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_Sprite *pISprite, gldSprite **ppSprite)
{
	IF_ARG_NULL_RETURN(pISprite);
	IF_ARG_NULL_RETURN(ppSprite);
	
	HRESULT hr;

	if ((m_pI2CMap != NULL) && (SUCCEEDED(m_pI2CMap->QueryObject(pISprite, (gldObj **)ppSprite))))
		return S_OK;
	
	CAutoPtr<gldSprite> pSprite(new gldSprite);
	DeleteAllLayers(pSprite->m_mc);
	hr = Convert(pISprite, pSprite);
	CHECKRESULT();

	if (m_pI2CMap != NULL)
		m_pI2CMap->Insert(pISprite, pSprite);

	*ppSprite = pSprite.Detach();

	return S_OK;
}

HRESULT CI2C::Create(IGLD_Text *pIText, gldText2 **ppText)
{
	IF_ARG_NULL_RETURN(pIText);
	IF_ARG_NULL_RETURN(ppText);

	*ppText = NULL;

	HRESULT hr;

	if ((m_pI2CMap != NULL) && (SUCCEEDED(m_pI2CMap->QueryObject(pIText, (gldObj **)ppText))))
		return S_OK;

	CAutoPtr<gldText2> pText(new gldText2);	
	hr = Convert(pIText, pText);
	CHECKRESULT();
	
	if (m_pI2CMap != NULL)
		m_pI2CMap->Insert(pIText, pText);

	*ppText = pText.Detach();

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_Text *pIText, gldText2 *pText)
{
	IF_ARG_NULL_RETURN(pIText);
	IF_ARG_NULL_RETURN(pText);

	HRESULT hr;

	// text style
	GLD_TextStyle style;
	hr = pIText->get_Style(&style);
	CHECKRESULT();
	if (style == gceStaticText)
	{
		CComQIPtr<IGLD_StaticText> pIStatic = pIText;
		if (pIStatic != NULL)		
			hr = Convert(pIStatic, pText);		
		else
			hr = E_UNEXPECTED;
	}
	else if (style == gceDynamicText)
	{
		CComQIPtr<IGLD_DynamicText> pIDyna = pIText;
		if (pIDyna != NULL)
			hr = Convert(pIDyna, pText);
		else
			hr = E_UNEXPECTED;
	}
	else
		hr = E_UNEXPECTED;	
	
	return hr;
}

HRESULT CI2C::Convert(IGLD_StaticText *pIText, gldText2 *pText)
{
	IF_ARG_NULL_RETURN(pIText);
	IF_ARG_NULL_RETURN(pText);

	HRESULT hr;

	// set text style flag
	pText->m_textType = gtextStatic;

	// text bounding box
	CComPtr<IGLD_Rect> bound;
	hr = pIText->get_Bound(&bound);
	CHECKRESULT();
	double l, t, r, b;
	hr = bound->GetData(&l, &t, &r, &b);
	CHECKRESULT();
	pText->m_bounds.left = (int)l;
	pText->m_bounds.top = (int)t;
	pText->m_bounds.right = (int)r;
	pText->m_bounds.bottom = (int)b;
	
	// Paragraphs
	CComPtr<IGLD_TextParagraphs> pIParas;
	hr = pIText->get_Paragraphs(&pIParas);
	CHECKRESULT();

	CComPtr<IEnumVARIANT> enumParas;
	hr = pIParas->get__NewEnum(&enumParas);
	CHECKRESULT();
	CComVariant varPara;

	USES_CONVERSION;

	while (enumParas->Next(1, &varPara, NULL) == S_OK)
	{		
		CComQIPtr<IGLD_TextParagraph> pIPara = varPara.punkVal;
		if (pIPara == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		varPara.Clear();
		CComPtr<IGLD_ParagraphFormat> pIFmt;
		hr = pIPara->get_Format(&pIFmt);
		CHECKRESULT();
		CAutoPtr<CParagraph> pPara(new CParagraph(NULL));	
		GLD_ParagraphAlignType align;
		hr = pIFmt->get_Align(&align);
		if (align == gceParagraphAlignLeft)
			pPara->SetAlignTypeType(CParagraph::AlignLeft, FALSE);
		else if (align == gceParagraphAlignCenter)
			pPara->SetAlignTypeType(CParagraph::AlignCenter, FALSE);
		else if (align == gceParagraphAlignRight)
			pPara->SetAlignTypeType(CParagraph::AlignRight, FALSE);
		else
			hr = E_UNEXPECTED;
		CHECKRESULT();
		
		long lValue;		
		hr = pIFmt->get_Indent(&lValue);
		CHECKRESULT();
		pPara->SetIndent(lValue);

		hr = pIFmt->get_LeftMargin(&lValue);
		CHECKRESULT();
		pPara->SetMarginLeft(lValue);

		hr = pIFmt->get_RightMargin(&lValue);
		CHECKRESULT();
		pPara->SetMarginRight(lValue);

		hr = pIFmt->get_LineSpacing(&lValue);
		CHECKRESULT();
		pPara->SetSpacing(lValue);

		// text blocks
		CComPtr<IGLD_TextBlocks> pIBlocks;
		hr = pIPara->get_TextBlocks(&pIBlocks);
		CHECKRESULT();

		CComPtr<IEnumVARIANT> enumBlock;
		hr = pIBlocks->get__NewEnum(&enumBlock);
		CHECKRESULT();

		CComVariant varBlock;
		while (enumBlock->Next(1, &varBlock, NULL) == S_OK)
		{
			CComPtr<IGLD_TextBlock> pIBlock;
			hr = varBlock.punkVal->QueryInterface(&pIBlock);
			CHECKRESULT();			
			varBlock.Clear();

			CComPtr<IGLD_Font> pIFont;
			hr = pIBlock->get_Font(&pIFont);
			CHECKRESULT();

			CTextFormat txtFmt;		
			VARIANT_BOOL bSet;

			hr = pIFont->get_Bold(&bSet);
			CHECKRESULT();
			txtFmt.SetBold(CHECK(bSet));

			hr = pIFont->get_Italic(&bSet);
			CHECKRESULT();
			txtFmt.SetItalic(CHECK(bSet));

			CComBSTR bstrFaceName;
			hr = pIFont->get_FaceName(&bstrFaceName);
			CHECKRESULT();
			txtFmt.SetFontFace(OLE2AEX(bstrFaceName));
			
			hr = pIFont->get_Size(&lValue);
			CHECKRESULT();
			txtFmt.SetFontSize(lValue);			

			CComPtr<IGLD_Color> pIColor;
			hr = pIFont->get_Color(&pIColor);
			CHECKRESULT();
			byte r, g, b, a;
			hr = pIColor->GetData(&r, &g, &b, &a);
			CHECKRESULT();
			COLORREF color = RGB(r, g, b);
			txtFmt.SetFontColor(color);

			GLD_CharacterLocation loc;
			hr = pIBlock->get_Location(&loc);
			CHECKRESULT();
			if (loc == gceNormalChar)
				txtFmt.SetLocate(CTextFormat::CL_NONE);
			else if (loc == gceSubscriptChar)
				txtFmt.SetLocate(CTextFormat::CL_SUBSCRIPT);
			else if (loc == gceSupperscriptChar)
				txtFmt.SetLocate(CTextFormat::CL_SUPERSCRIPT);
			else
				hr = E_UNEXPECTED;
			CHECKRESULT();

			hr = pIBlock->get_Spacing(&lValue);
			CHECKRESULT();
			txtFmt.SetSpacing(lValue);

			CComBSTR bstrLink;
			hr = pIBlock->get_Link(&bstrLink);
			CHECKRESULT();
			txtFmt.SetLink(OLE2AEX(bstrLink));

			CComBSTR bstrTarget;
			hr = pIBlock->get_Target(&bstrTarget);
			CHECKRESULT();
			txtFmt.SetTarget(OLE2AEX(bstrTarget));

			CComBSTR bstrText;
			hr = pIBlock->get_Text(&bstrText);
			CHECKRESULT();		

			CAutoPtr<CTextBlock> pBlock(new CTextBlock(txtFmt, bstrText));
			pPara->m_TextBlockList.push_back(pBlock.Detach());
		}

		pText->m_ParagraphList.push_back(pPara.Detach());
	}

	CComBSTR bstrName;
	hr = pIText->get_Name(&bstrName);
	CHECKRESULT();
	pText->m_name = OLE2AEX(bstrName);

	hr = SetLinkage(pIText, pText);
	CHECKRESULT();

	pText->bNoConvert = FALSE;	
	ITextTool IClass;
	SETextToolEx TTool(&IClass);
	TTool.ConvertFromGldText(*pText);
	TTool.ConvertToGldText(*pText);

	CSWFProxy::PrepareTShapeForGObj(pText);

	return S_OK;
}

HRESULT CI2C::Convert(IGLD_DynamicText *pIText, gldText2 *pText)
{
	IF_ARG_NULL_RETURN(pIText);
	IF_ARG_NULL_RETURN(pText);

	HRESULT hr;	

	// set text style flag
	pText->m_textType = gtextDynamic;

	// dynamic info
	VARIANT_BOOL bSet;
	gldDynaTextInfo &dynaInfo = pText->m_dynaInfo;
	hr = pIText->get_CanSelect(&bSet);
	CHECKRESULT();
	dynaInfo.SetCanSelectFlag(CHECK(bSet));
	
	hr = pIText->get_HasBorder(&bSet);
	CHECKRESULT();
	dynaInfo.SetHasBorderFlag(CHECK(bSet));

	hr = pIText->get_RenderAsHTML(&bSet);
	CHECKRESULT();
	dynaInfo.SetIsHTMLFlag(CHECK(bSet));
	
	hr = pIText->get_IsPassword(&bSet);
	CHECKRESULT();
	dynaInfo.SetIsPasswordFlag(CHECK(bSet));

	hr = pIText->get_ReadOnly(&bSet);
	CHECKRESULT();
	dynaInfo.SetIsReadOnlyFlag(CHECK(bSet));

	USES_CONVERSION;
	CComBSTR varName;
	hr = pIText->get_VariableName(&varName);
	CHECKRESULT();
	dynaInfo.m_variableName = OLE2W(varName);

	// paragraph format
	CComPtr<IGLD_ParagraphFormat> pIFmt;
	hr = pIText->get_Format(&pIFmt);
	CHECKRESULT();

	GLD_ParagraphAlignType align;
	hr = pIFmt->get_Align(&align);
	if (align == gceParagraphAlignLeft)
		dynaInfo.m_align = 0;
	else if (align == gceParagraphAlignCenter)
		dynaInfo.m_align = 2;
	else if (align == gceParagraphAlignRight)
		dynaInfo.m_align = 1;
	else
		hr = E_UNEXPECTED;
	CHECKRESULT();
	
	long lValue;		
	hr = pIFmt->get_Indent(&lValue);
	CHECKRESULT();
	dynaInfo.m_indent = (U16)lValue;

	hr = pIFmt->get_LeftMargin(&lValue);
	CHECKRESULT();
	dynaInfo.m_leftMargin = (U16)lValue;

	hr = pIFmt->get_RightMargin(&lValue);
	CHECKRESULT();
	dynaInfo.m_rightMargin = (U16)lValue;

	hr = pIFmt->get_LineSpacing(&lValue);
	CHECKRESULT();
	dynaInfo.m_leading = (U16)lValue;

	// font
	CComPtr<IGLD_Font> pIFont;
	hr = pIText->get_Font(&pIFont);
	CHECKRESULT();
	
	gldFont *pFont = dynaInfo.m_font;
	if (pFont == NULL)
	{
		// new font
		if (m_pI2CMap == NULL || FAILED(m_pI2CMap->QueryObject(pIFont, (gldObj **)&pFont)))
		{
			pFont = new gldFont;
			m_pI2CMap->Insert(pIFont, pFont);
		}
	}
	ATLASSERT(pFont != NULL);
	dynaInfo.m_font = pFont;

	VARIANT_BOOL bBold, bItalic;
	hr = pIFont->get_Bold(&bBold);
	CHECKRESULT();
	hr = pIFont->get_Italic(&bItalic);
	CHECKRESULT();
	CComBSTR bstrFaceName;
	hr = pIFont->get_FaceName(&bstrFaceName);
	CHECKRESULT();	
	CComPtr<IGLD_Color> pIColor;
	hr = pIFont->get_Color(&pIColor);
	CHECKRESULT();
	byte r, g, b, a;
	hr = pIColor->GetData(&r, &g, &b, &a);
	CHECKRESULT();
	dynaInfo.m_textColor.red = r;
	dynaInfo.m_textColor.green = g;
	dynaInfo.m_textColor.blue = b;	

	pFont->SetFont(OLE2AEX(bstrFaceName), CHECK(bBold), CHECK(bItalic));
	
	hr = pIFont->get_Size(&lValue);
	CHECKRESULT();
	dynaInfo.m_fontHeight = (U16)lValue;

	CComBSTR bstrText;
	hr = pIText->get_Text(&bstrText);
	CHECKRESULT();
	dynaInfo.m_initialString = OLE2W(bstrText);

	CComPtr<IGLD_Rect> pIBound;
	hr = pIText->get_Bound(&pIBound);
	CHECKRESULT();
	double x1, y1, x2, y2;
	hr = pIBound->GetData(&x1, &y1, &x2, &y2);
	CHECKRESULT();
	pText->m_bounds.left = (int)x1;
	pText->m_bounds.top = (int)y1;
	pText->m_bounds.right = (int)x2;
	pText->m_bounds.bottom = (int)y2;

	CComBSTR bstrName;
	hr = pIText->get_Name(&bstrName);
	CHECKRESULT();
	pText->m_name = OLE2AEX(bstrName);

	hr = SetLinkage(pIText, pText);
	CHECKRESULT();

	pText->bNoConvert = TRUE;
	ITextTool IClass;
	SETextToolEx TTool(&IClass);
	TTool.ConvertFromGldText(*pText);
	TTool.ConvertToGldText(*pText);

	CSWFProxy::PrepareTShapeForGObj(pText);

	return S_OK;
}