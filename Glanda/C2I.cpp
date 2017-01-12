#include "StdAfx.h"
#include "C2I.h"
#include "gldImage.h"
#include "gldShape.h"
#include "gldLineStyle.h"
#include "gldFillStyle.h"
#include "gldShapeRecordChange.h"
#include "gldShapeRecordEdgeStraight.h"
#include "gldShapeRecordEdgeCurved.h"
#include "gldGradientRecord.h"
#include "gldMovieClip.h"
#include "gldButton.h"
#include "gldSprite.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldSound.h"
#include "TextToolEx.h"
#include "I2C.h"
#include <atlsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define VAR_BOOL(exp) ((exp) ? VARIANT_TRUE : VARIANT_FALSE)

CC2IMap *CC2I::m_pC2IMap = NULL;

void CC2IMap::CopyTo(CI2CMap *pRevMap)
{
	_C2IMap::iterator i = m_Map.begin();
	for (; i != m_Map.end(); ++i)
		pRevMap->m_Map.insert(CI2CMap::_I2CMap::value_type((*i).second, (*i).first));
}

template <typename T, typename I>
HRESULT SetLinkage(T *pObj, I *pIObj)
{
	HRESULT hr = S_OK;
	CComQIPtr<IGLD_Object2> pIObject2 = pIObj;
	if (pIObject2 == NULL)
	{
		hr = E_FAIL;
		CHECKRESULT();
	}
	CComPtr<IGLD_Linkage> pILinkage;
	hr = pIObject2->get_Linkage(&pILinkage);
	CHECKRESULT();
	GLD_LinkageType type = gceLinkageNone;
	if (pObj->m_objFlag == gobjExportFlag)
		type = gceLinkageExport;
	else if (pObj->m_objFlag == gobjImportFlag)
		type = gceLinkageImport;
	hr = pILinkage->put_Type(type);
	CHECKRESULT();
	if (type != gceLinkageNone)
	{
		CComBSTR bstrName(pObj->m_linkageName.c_str());
		hr = pILinkage->put_Name(bstrName);
		CHECKRESULT();
		CComBSTR bstrURL(pObj->m_linkageURL.c_str());
		hr = pILinkage->put_URL(bstrURL);
		CHECKRESULT();
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CC2I
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CC2I::Convert(gldImage *pImage, IGLD_Image *pIImage)
{
	IF_ARG_NULL_RETURN(pImage);
	IF_ARG_NULL_RETURN(pIImage);

	HRESULT hr;

	CComPtr<IGLD_BufferStream> pIStream;
	hr = pIStream.CoCreateInstance(__uuidof(GLD_BufferStream));
	CHECKRESULT();	
	hr = pIStream->Attach(pImage->GetRawData(), pImage->GetRawDataLen());
	CHECKRESULT();
	
	hr = pIImage->put_Name(CComBSTR(pImage->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pImage, pIImage);
	CHECKRESULT();

	return pIImage->Load(pIStream);
}

HRESULT CC2I::Convert(gldSound *pSound, IGLD_Sound *pISound)
{
	IF_ARG_NULL_RETURN(pSound);
	IF_ARG_NULL_RETURN(pISound);

	HRESULT hr;

	CComPtr<IGLD_BufferStream> pIStream;
	hr = pIStream.CoCreateInstance(__uuidof(GLD_BufferStream));
	CHECKRESULT();	
	hr = pIStream->Attach(pSound->GetRawData(), pSound->GetRawDataLen());
	CHECKRESULT();
	
	hr = pISound->put_Name(CComBSTR(pSound->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pSound, pISound);
	CHECKRESULT();

	return pISound->Load(pIStream);
}

HRESULT CC2I::Convert(gldShape *pShape, IGLD_Shape *pIShape)
{
	IF_ARG_NULL_RETURN(pShape);
	IF_ARG_NULL_RETURN(pIShape);

	HRESULT hr;	

	CComPtr<IGLD_ShapeRecords> pIRecords;
	CComPtr<IGLD_LineStyles> pILines;
	CComPtr<IGLD_FillStyles> pIFills;

	hr = pIShape->get_ShapeRecords(&pIRecords);
	CHECKRESULT();
	hr = pIShape->get_LineStyles(&pILines);
	CHECKRESULT();
	hr = pIShape->get_FillStyles(&pIFills);
	CHECKRESULT();

	GShapeRecords *pRecords = pShape->GetShapeRecords();
	ATLASSERT(pRecords != NULL);
	hr = Convert(pRecords, pIRecords);
	CHECKRESULT();

	GFillStyles *pFills = pShape->GetFillStyles();
	ATLASSERT(pFills != NULL);
	hr = Convert(pFills, pIFills);
	CHECKRESULT();

	GLineStyles *pLines = pShape->GetLineStyles();
	ATLASSERT(pLines != NULL);
	hr = Convert(pLines, pILines);
	CHECKRESULT();

	hr = pIShape->put_Name(CComBSTR(pShape->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pShape, pIShape);
	CHECKRESULT();

	hr = pIShape->Refresh();
	CHECKRESULT();

	return S_OK;
}

HRESULT CC2I::Convert(GShapeRecords *pRecords, IGLD_ShapeRecords *pIRecords)
{
	IF_ARG_NULL_RETURN(pRecords);
	IF_ARG_NULL_RETURN(pIRecords);

	HRESULT hr;

	GShapeRecords::iterator i = pRecords->begin();
	for (; i != pRecords->end(); ++i)
	{
		CComPtr<IGLD_ShapeRecord> pIRecord;
		hr = Create(*i, &pIRecord);
		CHECKRESULT();

		hr = pIRecords->Append(pIRecord);
		CHECKRESULT();		
	}

	return S_OK;
}

HRESULT CC2I::Convert(GLineStyles *pLines, IGLD_LineStyles *pILines)
{
	IF_ARG_NULL_RETURN(pLines);
	IF_ARG_NULL_RETURN(pILines);

	HRESULT hr;

	GLineStyles::iterator i = pLines->begin();
	for (; i != pLines->end(); ++i)
	{
		CComPtr<IGLD_LineStyle> pILine;
		hr = Create(*i, &pILine);
		CHECKRESULT();
		
		hr = pILines->Append(pILine);
		CHECKRESULT();		
	}

	return S_OK;
}

HRESULT CC2I::Convert(GFillStyles *pFills, IGLD_FillStyles *pIFills)
{
	IF_ARG_NULL_RETURN(pFills);
	IF_ARG_NULL_RETURN(pIFills);

	HRESULT hr;

	GFillStyles::iterator i = pFills->begin();
	for (; i != pFills->end(); ++i)
	{
		CComPtr<IGLD_FillStyle> pIFill;
		hr = Create(*i, &pIFill);
		CHECKRESULT();
		
		hr = pIFills->Append(pIFill);
		CHECKRESULT();		
	}

	return S_OK;
}

HRESULT CC2I::Convert(gldAction *pAction, IGLD_Action *pIAction)
{
	IF_ARG_NULL_RETURN(pAction);
	IF_ARG_NULL_RETURN(pIAction);

	HRESULT hr = S_OK;	
	
	if (pAction->m_useCompiledActionBin)
	{
		const string &strAction = pAction->GetSourceCode();
		if (strAction.length() > 0)
		{
			CComBSTR bstrAction(strAction.c_str());
			hr = pIAction->SetSource(bstrAction);
			CHECKRESULT();
		}
	}
	else
	{
		U8 *pBin = NULL;
		int binSize = pAction->GetBinCode(&pBin);
		if (binSize > 0)
		{
			hr = pIAction->SetBinary(pBin, (ULONG)binSize);
			delete[] pBin;
			pBin = NULL;
			CHECKRESULT();
		}
		if (pBin != NULL)
			delete[] pBin;
	}

	return S_OK;
}

HRESULT CC2I::Convert(gldMovieClip *pMovie, IGLD_MovieClip *pIMovie)
{
	IF_ARG_NULL_RETURN(pMovie);
	IF_ARG_NULL_RETURN(pIMovie);

	HRESULT hr;

	if (pMovie->m_initAction != NULL)
	{
		CComPtr<IGLD_Action> pIAction;
		hr = pIMovie->get_InitClipAction(&pIAction);
		CHECKRESULT();
		hr = Convert(pMovie->m_initAction, pIAction);
		CHECKRESULT();
	}

	CComPtr<IGLD_Layers> pILayers;
	hr = pIMovie->get_Layers(&pILayers);
	CHECKRESULT();

	GLAYER_LIST_IT iLayer = pMovie->m_layerList.begin();	
	for (; iLayer != pMovie->m_layerList.end(); ++iLayer)
	{
		gldLayer *pgl = *iLayer;
		GLD_LayerType layerType = gceNormalLayer;
		if (pgl->m_mask)
			layerType = gceMaskLayer;
		else if (pgl->m_masked)
			layerType = gceMaskedLayer;
		CComPtr<IGLD_Layer> pILayer;
		hr = pILayers->Append(layerType, CComBSTR(pgl->m_name.c_str()), &pILayer);
		CHECKRESULT();
		hr = Convert(pgl, pILayer);
		CHECKRESULT();
	}	

	hr = pIMovie->Refresh();
	CHECKRESULT();

	return S_OK;
}

HRESULT CC2I::Convert(gldLayer *pLayer, IGLD_Layer *pILayer)
{
	IF_ARG_NULL_RETURN(pLayer);
	IF_ARG_NULL_RETURN(pILayer);

	HRESULT hr;

	CComPtr<IGLD_CurvedPathes> pIGuidLines;
	hr = pILayer->get_GuideLines(&pIGuidLines);
	CHECKRESULT();

	GUIDLINE_LIST::iterator iGL = pLayer->m_guidlineList.begin();
	for (; iGL != pLayer->m_guidlineList.end(); ++iGL)
	{
		tGuidLine *pGL = *iGL;
		CComPtr<IGLD_CurvedPath> pIPath;
		hr = Create(pGL, &pIPath);
		CHECKRESULT();
		hr = pIGuidLines->Append(pIPath);
		CHECKRESULT();
	}

	CComPtr<IGLD_FrameClips> pIFrames;
	hr = pILayer->get_FrameClips(&pIFrames);
	CHECKRESULT();

	GFRAMECLIP_LIST_IT iFrame = pLayer->m_frameClipList.begin();
	for (; iFrame != pLayer->m_frameClipList.end(); ++iFrame)
	{
		gldFrameClip *pfc = *iFrame;
		GLD_FrameClipType type = gceStaticFrameClip;
		if (pfc->IsMotion())
			type = gceMotionFrameClip;
		CComPtr<IGLD_FrameClip> pIFrame;
		hr = pIFrames->Append(pfc->m_length, type, &pIFrame);
		CHECKRESULT();
		hr = Convert(pfc, pIFrame);
		CHECKRESULT();		
	}

	return S_OK;
}

HRESULT CC2I::Convert(gldFrameClip *pFrame, IGLD_FrameClip *pIFrame)
{
	IF_ARG_NULL_RETURN(pFrame);
	IF_ARG_NULL_RETURN(pIFrame);

	HRESULT hr;

	CComPtr<IGLD_Motion> pIMotion;
	hr = pIFrame->get_Motion(&pIMotion);
	CHECKRESULT();
	hr = Convert(&pFrame->m_motion, pIMotion);
	CHECKRESULT();
	if (pFrame->m_action != NULL)
	{
		CComPtr<IGLD_Action> pIAction;
		hr = pIFrame->get_Action(&pIAction);
		CHECKRESULT();
		hr = Convert(pFrame->m_action, pIAction);
		CHECKRESULT();		
	}

	hr = pIFrame->put_Label(CComBSTR(pFrame->m_label.c_str()));
	CHECKRESULT();

	CComPtr<IGLD_Sound> pISound;	
	if (pFrame->m_sound != NULL && pFrame->m_sound->m_soundObj != NULL)
	{
		gldSound *pSound = pFrame->m_sound->m_soundObj;
		hr = Create(pSound, &pISound);
		CHECKRESULT();
	}
	hr = pIFrame->put_Sound(pISound);
	CHECKRESULT();

	CComPtr<IGLD_CharacterKeys> pIKeys;
	hr = pIFrame->get_CharacterKeys(&pIKeys);
	CHECKRESULT();

	GCHARACTERKEY_LIST_IT iKey = pFrame->m_characterKeyList.begin();
	for (; iKey != pFrame->m_characterKeyList.end(); ++iKey)
	{
		gldCharacterKey *pgKey = *iKey;
		
		CComPtr<IGLD_Object> pIObj;
		hr = Create(pgKey->GetObj(), &pIObj);
		CHECKRESULT();

		CComPtr<IGLD_CharacterKey> pIKey;
		hr = pIKeys->Append(pIObj, NULL, NULL, &pIKey);
		CHECKRESULT();

		CComPtr<IGLD_Matrix> pIMatrix;
		hr = pIKey->get_Matrix(&pIMatrix);
		CHECKRESULT();
		gldMatrix &gmat = pgKey->GetCharacter(0)->m_matrix;
		hr = pIMatrix->SetData(gmat.m_e11, gmat.m_e12, gmat.m_e21, gmat.m_e22, gmat.m_x, gmat.m_y);
		CHECKRESULT();

		CComPtr<IGLD_Cxform> pICxform;
		hr = pIKey->get_Cxform(&pICxform);
		CHECKRESULT();
		gldCxform &gcx = pgKey->GetCharacter(0)->m_cxform;
		hr = pICxform->SetData(gcx.ra, gcx.rb, gcx.ga, gcx.gb, gcx.ba, gcx.bb, gcx.aa, gcx.ab);
		CHECKRESULT();

		if (pgKey->m_action != NULL)
		{			
			CComPtr<IGLD_Action> pIAction;
			hr = pIKey->get_Action(&pIAction);
			CHECKRESULT();
			hr = Convert(pgKey->m_action, pIAction);
			CHECKRESULT();
		}

		hr = pIKey->put_Name(CComBSTR(pgKey->m_name.c_str()));
		CHECKRESULT();
	}

	return S_OK;
}

HRESULT CC2I::Convert(gldMotionTweenParam *pMotion, IGLD_Motion *pIMotion)
{
	IF_ARG_NULL_RETURN(pMotion);
	IF_ARG_NULL_RETURN(pIMotion);

	HRESULT hr;

	GLD_MoveMotionType moveType = gceAutoMoveMotion;
	if (pMotion->moveType == MOTION_MOVE_NONE)
		moveType = gceNoneMoveMotion;
	hr = pIMotion->put_MoveMotionType(moveType);
	CHECKRESULT();
	hr = pIMotion->put_MoveEase(pMotion->moveEase);
	CHECKRESULT();

	GLD_ScaleMotionType scaleType = gceAutoScaleMotion;
	if (pMotion->scaleType == MOTION_SCALE_NONE)
		scaleType = gceNoneScaleMotion;
	hr = pIMotion->put_ScaleMotionType(scaleType);
	CHECKRESULT();
	hr = pIMotion->put_ScaleEase(pMotion->scaleEase);
	CHECKRESULT();

	GLD_CxformMotionType cxformType = gceAutoCxformMotion;
	if (pMotion->cxformType == MOTION_CXFORM_NONE)
		cxformType = gceNoneCxformMotion;
	hr = pIMotion->put_CxformMotionType(cxformType);
	CHECKRESULT();
	hr = pIMotion->put_CxformEase(pMotion->cxformEase);
	CHECKRESULT();

	GLD_RotateMotionType rotateType = gceAutoRotateMotion;
	if (pMotion->rotateType == MOTION_ROTATE_NONE)
		rotateType = gceNoneRotateMotion;
	else if (pMotion->rotateType == MOTION_ROTATE_CW)
		rotateType = gceCWRotateMotion;
	else if (pMotion->rotateType == MOTION_ROTATE_CCW)
		rotateType = gceCCWRotateMotion;
	hr = pIMotion->put_RotateMotionType(rotateType);
	CHECKRESULT();
	hr = pIMotion->put_RotateEase(pMotion->rotateEase);
	CHECKRESULT();

	hr = pIMotion->put_RotateTimes(pMotion->rotateTimes);
	CHECKRESULT();
	hr = pIMotion->put_SnapToPath(VAR_BOOL(pMotion->snapToPath));
	CHECKRESULT();
	hr = pIMotion->put_OrientToPath(VAR_BOOL(pMotion->orientToPath));
	CHECKRESULT();

	return S_OK;
}

HRESULT CC2I::Convert(gldText2 *pText, IGLD_StaticText *pIText)
{
	IF_ARG_NULL_RETURN(pText);
	IF_ARG_NULL_RETURN(pIText);
	
	HRESULT hr;
	
	// text bounding box
	CComPtr<IGLD_Rect> bound;
	hr = pIText->get_Bound(&bound);
	CHECKRESULT();
	hr = bound->SetData(pText->rtRegion.left, pText->rtRegion.top, pText->rtRegion.right, pText->rtRegion.bottom);
	CHECKRESULT();

	// Paragraphs
	CComPtr<IGLD_TextParagraphs> pIParas;
	hr = pIText->get_Paragraphs(&pIParas);
	CHECKRESULT();

	Paragraphs::iterator iPara = pText->m_ParagraphList.begin();
	for (; iPara != pText->m_ParagraphList.end(); ++iPara)
	{
		CParagraph *para = *iPara;
		
		CComPtr<IGLD_TextParagraph> pIPara;
		hr = pIPara.CoCreateInstance(__uuidof(GLD_TextParagraph));
		CHECKRESULT();

		CComPtr<IGLD_ParagraphFormat> pIFmt;
		hr = pIPara->get_Format(&pIFmt);
		CHECKRESULT();

		CParagraph::emAlignType align1 = para->GetAlignType();
		GLD_ParagraphAlignType align2;
		if (align1 == CParagraph::AlignLeft)
			align2 = gceParagraphAlignLeft;
		else if (align1 == CParagraph::AlignCenter)
			align2 = gceParagraphAlignCenter;
		else if (align1 == CParagraph::AlignRight)
			align2 = gceParagraphAlignRight;
		else
			hr = E_UNEXPECTED;
		CHECKRESULT();
		hr = pIFmt->put_Align(align2);
		CHECKRESULT();

		hr = pIFmt->put_Indent(para->GetIndent());
		CHECKRESULT();

		hr = pIFmt->put_LeftMargin(para->GetMarginLeft());
		CHECKRESULT();

		hr = pIFmt->put_RightMargin(para->GetMarginRight());
		CHECKRESULT();

		hr = pIFmt->put_LineSpacing(para->GetSpacing());
		CHECKRESULT();
		
		// Text Blocks
		CComPtr<IGLD_TextBlocks> pIBlocks;
		hr = pIPara->get_TextBlocks(&pIBlocks);
		CHECKRESULT();

		TextBlocks::iterator iBlock = para->m_TextBlockList.begin();
		for (; iBlock != para->m_TextBlockList.end(); ++iBlock)
		{
			CTextBlock *pBlock = *iBlock;
			CComPtr<IGLD_TextBlock> pIBlock;
			hr = pIBlock.CoCreateInstance(__uuidof(GLD_TextBlock));
			CHECKRESULT();
			
			CComPtr<IGLD_Font> pIFont;
			hr = pIBlock->get_Font(&pIFont);
			CHECKRESULT();

			hr = pIBlock->put_Text(CComBSTR(pBlock->strText.c_str()));
			CHECKRESULT();

			CTextFormat &txtFmt = pBlock->TextFormat;
			hr = pIFont->put_Bold(VAR_BOOL(txtFmt.GetBold()));
			CHECKRESULT();
			hr = pIFont->put_Italic(VAR_BOOL(txtFmt.GetItalic()));
			CHECKRESULT();
			hr = pIFont->put_FaceName(CComBSTR(txtFmt.GetFontFace().c_str()));
			CHECKRESULT();
			hr = pIFont->put_Size(txtFmt.GetFontSize());
			CHECKRESULT();
			hr = pIBlock->put_Spacing(txtFmt.GetSpacing());
			CHECKRESULT();
			CComPtr<IGLD_Color> pIColor;
			hr = pIFont->get_Color(&pIColor);
			CHECKRESULT();
			COLORREF color = txtFmt.GetFontColor();
			hr = pIColor->SetData(GetRValue(color), GetGValue(color), GetBValue(color), 255);
			CHECKRESULT();
			CTextFormat::emCharLocate loc1 = txtFmt.GetLocate();
			GLD_CharacterLocation loc2;
			if (loc1 == CTextFormat::CL_NONE)
				loc2 = gceNormalChar;
			else if (loc1 == CTextFormat::CL_SUBSCRIPT)
				loc2 = gceSubscriptChar;
			else if (loc1 == CTextFormat::CL_SUPERSCRIPT)
				loc2 = gceSupperscriptChar;
			else
				hr = E_UNEXPECTED;
			CHECKRESULT();
			hr = pIBlock->put_Location(loc2);
			CHECKRESULT();
			hr = pIBlock->put_Link(CComBSTR(txtFmt.GetLink().c_str()));
			CHECKRESULT();
			hr = pIBlock->put_Target(CComBSTR(txtFmt.GetTarget().c_str()));
			CHECKRESULT();

			hr = pIBlocks->Append(pIBlock);
			CHECKRESULT();
		}

		pIParas->Append(pIPara);
	}

	hr = pIText->put_Name(CComBSTR(pText->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pText, pIText);
	CHECKRESULT();

	return S_OK;
}

HRESULT CC2I::Convert(gldText2 *pText, IGLD_DynamicText *pIText)
{
	IF_ARG_NULL_RETURN(pText);
	IF_ARG_NULL_RETURN(pIText);
	
	HRESULT hr;
	
	// dynamic info
	gldDynaTextInfo &dynaInfo = pText->m_dynaInfo;
	hr = pIText->put_CanSelect(VAR_BOOL(dynaInfo.CanSelect()));
	CHECKRESULT();
	hr = pIText->put_HasBorder(VAR_BOOL(dynaInfo.HasBorder()));
	CHECKRESULT();
	hr = pIText->put_RenderAsHTML(VAR_BOOL(dynaInfo.IsHTML()));
	CHECKRESULT();
	hr = pIText->put_IsPassword(VAR_BOOL(dynaInfo.IsPassword()));
	CHECKRESULT();
	hr = pIText->put_ReadOnly(VAR_BOOL(dynaInfo.IsReadOnly()));
	CHECKRESULT();
	hr = pIText->put_VariableName(CComBSTR(dynaInfo.m_variableName.c_str()));
	CHECKRESULT();
	hr = pIText->put_Text(CComBSTR(dynaInfo.m_initialString.c_str()));

	// Format
	CComPtr<IGLD_ParagraphFormat> pIFmt;
	hr = pIText->get_Format(&pIFmt);
	CHECKRESULT();
	U8 align1 = dynaInfo.m_align;
	GLD_ParagraphAlignType align2;
	if (align1 == 0)
		align2 = gceParagraphAlignLeft;
	else if (align1 == 2)
		align2 = gceParagraphAlignCenter;
	else if (align1 == 1)
		align2 = gceParagraphAlignRight;
	else
		hr = E_UNEXPECTED;
	CHECKRESULT();
	hr = pIFmt->put_Align(align2);
	CHECKRESULT();
	hr = pIFmt->put_Indent(dynaInfo.m_indent);
	CHECKRESULT();
	hr = pIFmt->put_LeftMargin(dynaInfo.m_leftMargin);
	CHECKRESULT();
	hr = pIFmt->put_RightMargin(dynaInfo.m_rightMargin);
	CHECKRESULT();
	hr = pIFmt->put_LineSpacing(dynaInfo.m_leading);
	CHECKRESULT();

	// Font
	CComPtr<IGLD_Font> pIFont;
	hr = pIText->get_Font(&pIFont);
	CHECKRESULT();	
	hr = pIFont->put_Size(dynaInfo.m_fontHeight);
	CHECKRESULT();
	CComPtr<IGLD_Color> pIColor;
	hr = pIFont->get_Color(&pIColor);
	CHECKRESULT();
	gldColor &color = dynaInfo.m_textColor;
	hr = pIColor->SetData(color.red, color.green, color.blue, 255);
	CHECKRESULT();
	gldFont *pFont = dynaInfo.m_font;
	if (pFont != NULL)
	{
		hr = pIFont->put_Bold(VAR_BOOL(pFont->IsBold()));
		CHECKRESULT();
		hr = pIFont->put_Italic(VAR_BOOL(pFont->IsItalic()));
		CHECKRESULT();
		hr = pIFont->put_FaceName(CComBSTR(pFont->GetFontName().c_str()));
		CHECKRESULT();
	}

	// bounding box
	CComPtr<IGLD_Rect> pIBound;
	hr = pIText->get_Bound(&pIBound);
	CHECKRESULT();
	CRect &bnd = pText->m_bounds;
	hr = pIBound->SetData(bnd.left, bnd.top, bnd.right, bnd.bottom);
	CHECKRESULT();

	hr = pIText->put_Name(CComBSTR(pText->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pText, pIText);
	CHECKRESULT();

	return S_OK;
}

HRESULT CC2I::Convert(gldText2 *pText, IGLD_Text *pIText)
{
	IF_ARG_NULL_RETURN(pText);
	IF_ARG_NULL_RETURN(pIText);
	
	HRESULT hr;

	// text style
	if (pText->m_textType == gtextStatic)
	{
		CComQIPtr<IGLD_StaticText> pIStatic = pIText;
		if (pIStatic != NULL)
			hr = Convert(pText, pIStatic);
		else
			hr = E_UNEXPECTED;
	}
	else if (pText->m_textType == gtextDynamic)
	{
		CComQIPtr<IGLD_DynamicText> pIDyna = pIText;
		if (pIDyna != NULL)
			hr = Convert(pText, pIDyna);
		else
			hr = E_UNEXPECTED;
	}
	else
		hr = E_UNEXPECTED;

	return hr;
}

HRESULT CC2I::Convert(tGuidLine *pGuidLine, IGLD_CurvedPath *pIPath)
{
	IF_ARG_NULL_RETURN(pGuidLine);
	IF_ARG_NULL_RETURN(pIPath);
	
	HRESULT hr = S_OK;
	CComSafeArray<DOUBLE> psa;
	SAFEARRAYBOUND bnd[2];
	bnd[0].lLbound = 0;
	bnd[0].cElements = pGuidLine->numOfGuidPoints;
	bnd[1].lLbound = 0;
	bnd[1].cElements = pGuidLine->numOfGuidPoints;
	hr = psa.Create(bnd, 2);
	CHECKRESULT();
	DOUBLE *pvData = (DOUBLE *)psa.m_psa->pvData;
	tGuidPoint *cur = pGuidLine->head->next;
	while (cur != pGuidLine->head)
	{
		pvData[0] = cur->x;
		pvData[1] = cur->y;
		pvData += 2;
		cur = cur->next;
	}	

	return pIPath->put_Data(psa.m_psa);	
}

HRESULT CC2I::Create(gldShapeRecord *pRecord, IGLD_ShapeRecord **ppIRecord)
{
	IF_ARG_NULL_RETURN(pRecord);
	IF_ARG_NULL_RETURN(ppIRecord);

	*ppIRecord = NULL;

	HRESULT hr;

	GShapeRecType recordType = pRecord->GetGShapeRecType();

	if (recordType == gshapeRecChange)
	{
		CComPtr<IGLD_ShapeRecordChange> recordChange;
		hr = recordChange.CoCreateInstance(__uuidof(GLD_ShapeRecordChange));
		CHECKRESULT();

		gldShapeRecordChange *pc = (gldShapeRecordChange *)pRecord;
		if (pc->m_stateFS0)
			recordChange->put_FillStyle0(pc->m_fs0);
		if (pc->m_stateFS1)
			recordChange->put_FillStyle1(pc->m_fs1);
		if (pc->m_stateLS)
			recordChange->put_LineStyle(pc->m_ls);
		if (pc->m_stateMoveTo)
		{
			recordChange->put_MoveToX(pc->m_moveDX);
			recordChange->put_MoveToY(pc->m_moveDY);
		}
		if (pc->m_stateNewStyles)
		{
			CComPtr<IGLD_LineStyles> pILines;
			CComPtr<IGLD_FillStyles> pIFills;
			hr = recordChange->get_LineStyles(&pILines);
			CHECKRESULT();
			hr = recordChange->get_FillStyles(&pIFills);
			CHECKRESULT();
			hr = Convert(&pc->m_lsTable, pILines);
			CHECKRESULT();
			hr = Convert(&pc->m_fsTable, pIFills);
			CHECKRESULT();
			hr = recordChange->put_Flag(gceSetNewStyles, VARIANT_TRUE);
			CHECKRESULT();
		}

		return recordChange->QueryInterface(ppIRecord);
	}
	else if (recordType == gshapeRecEdgeStraight)
	{
		CComPtr<IGLD_ShapeRecordStraight> sEdge;
		hr = sEdge.CoCreateInstance(__uuidof(GLD_ShapeRecordStraight));
		CHECKRESULT();

		gldShapeRecordEdgeStraight *pc = (gldShapeRecordEdgeStraight *)pRecord;
		hr = sEdge->put_dx(pc->m_dx);
		CHECKRESULT();
		hr = sEdge->put_dy(pc->m_dy);
		CHECKRESULT();

		return sEdge->QueryInterface(ppIRecord);
	}
	else if (recordType == gshapeRecEdgeCurved)
	{
		CComPtr<IGLD_ShapeRecordCurved> cEdge;
		hr = cEdge.CoCreateInstance(__uuidof(GLD_ShapeRecordCurved));
		CHECKRESULT();

		gldShapeRecordEdgeCurved *pc = (gldShapeRecordEdgeCurved *)pRecord;
		hr = cEdge->put_cx(pc->m_cx);
		CHECKRESULT();
		hr = cEdge->put_cy(pc->m_cy);
		CHECKRESULT();
		hr = cEdge->put_ax(pc->m_ax);
		CHECKRESULT();
		hr = cEdge->put_ay(pc->m_ay);
		CHECKRESULT();

		return cEdge->QueryInterface(ppIRecord);	
	}
	
	return E_INVALIDARG;
}

HRESULT CC2I::Create(gldLineStyle *pLine, IGLD_LineStyle **ppILine)
{
	IF_ARG_NULL_RETURN(pLine);
	IF_ARG_NULL_RETURN(ppILine);

	*ppILine = NULL;

	HRESULT hr;

	CComPtr<IGLD_LineStyle> pILine;
	hr = pILine.CoCreateInstance(__uuidof(GLD_LineStyle));
	CHECKRESULT();

	hr = pILine->put_Width(pLine->GetWidth());
	CHECKRESULT();

	CComPtr<IGLD_Color> color;
	hr = pILine->get_Color(&color);
	CHECKRESULT();

	gldColor gc = pLine->GetColor();
	hr = color->SetData(gc.red, gc.green, gc.blue, gc.alpha);
	CHECKRESULT();

	return pILine.CopyTo(ppILine);
}

HRESULT CC2I::Create(gldFillStyle *pFill, IGLD_FillStyle **ppIFill)
{
	IF_ARG_NULL_RETURN(pFill);
	IF_ARG_NULL_RETURN(ppIFill);

	*ppIFill = NULL;

	HRESULT hr;

	FILLSTYLETYPE fillType = pFill->GetFillStyleType();

	if (fillType == linear_gradient_fill)
	{
		CComPtr<IGLD_LinearGradientFillStyle> lgf;
		hr = lgf.CoCreateInstance(__uuidof(GLD_LinearGradientFillStyle));
		CHECKRESULT();

		CComPtr<IGLD_GradientRecords> grs;
		hr = lgf->get_GradientRecords(&grs);
		CHECKRESULT();

		gldGradient *pgd = pFill->GetGradient();

		if (pgd != NULL)
		{
			GGradientRecords::iterator i = pgd->m_gradRecords.begin();

			for (; i != pgd->m_gradRecords.end(); ++i)
			{
				hr = grs->Add((*i)->m_ratio, (*i)->m_color.red,
					(*i)->m_color.green, (*i)->m_color.blue, (*i)->m_color.alpha);
				CHECKRESULT();
			}
		}

		CComPtr<IGLD_Matrix> matrix;
		hr = lgf->get_Matrix(&matrix);
		CHECKRESULT();

		const gldMatrix &mat = pFill->GetGradientMatrix();
		hr = matrix->SetData(mat.m_e11, mat.m_e12, mat.m_e21, mat.m_e22, mat.m_x, mat.m_y);
		CHECKRESULT();

		return lgf->QueryInterface(ppIFill);
	}
	else if (fillType == radial_gradient_fill)
	{
		CComPtr<IGLD_RadialGradientFillStyle> rgf;
		hr = rgf.CoCreateInstance(__uuidof(GLD_RadialGradientFillStyle));
		CHECKRESULT();

		CComPtr<IGLD_GradientRecords> grs;
		hr = rgf->get_GradientRecords(&grs);
		CHECKRESULT();

		gldGradient *pgd = pFill->GetGradient();

		if (pgd != NULL)
		{
			GGradientRecords::iterator i = pgd->m_gradRecords.begin();

			for (; i != pgd->m_gradRecords.end(); ++i)
			{
				hr = grs->Add((*i)->m_ratio, (*i)->m_color.red,
					(*i)->m_color.green, (*i)->m_color.blue, (*i)->m_color.alpha);
				CHECKRESULT();
			}
		}

		CComPtr<IGLD_Matrix> matrix;
		hr = rgf->get_Matrix(&matrix);
		CHECKRESULT();

		const gldMatrix &mat = pFill->GetGradientMatrix();
		hr = matrix->SetData(mat.m_e11, mat.m_e12, mat.m_e21, mat.m_e22, mat.m_x, mat.m_y);
		CHECKRESULT();

		return rgf->QueryInterface(ppIFill);
	}
	else if (fillType == tiled_bitmap_fill)
	{
		CComPtr<IGLD_TiledBitmapFillStyle> tbf;
		hr = tbf.CoCreateInstance(__uuidof(GLD_TiledBitmapFillStyle));
		CHECKRESULT();

		gldImage *pImage = pFill->GetBitmap();
		if (pImage != NULL)
		{
			CComPtr<IGLD_Image> pIImage;
			hr = Create(pImage, &pIImage);
			CHECKRESULT();
			hr = tbf->put_Image(pIImage);
			CHECKRESULT();
		}

		CComPtr<IGLD_Matrix> matrix;
		hr = tbf->get_Matrix(&matrix);
		CHECKRESULT();

		const gldMatrix &mat = pFill->GetBitmapMatrix();
		hr = matrix->SetData(mat.m_e11, mat.m_e12, mat.m_e21, mat.m_e22, mat.m_x, mat.m_y);
		CHECKRESULT();

		return tbf->QueryInterface(ppIFill);
	}
	else if (fillType == clipped_bitmap_fill)
	{
		CComPtr<IGLD_ClippedBitmapFillStyle> cbf;
		hr = cbf.CoCreateInstance(__uuidof(GLD_ClippedBitmapFillStyle));
		CHECKRESULT();

		gldImage *pImage = pFill->GetBitmap();
		if (pImage != NULL)
		{
			CComPtr<IGLD_Image> pIImage;
			hr = Create(pImage, &pIImage);
			CHECKRESULT();
			hr = cbf->put_Image(pIImage);
			CHECKRESULT();
		}

		CComPtr<IGLD_Matrix> matrix;
		hr = cbf->get_Matrix(&matrix);
		CHECKRESULT();

		const gldMatrix &mat = pFill->GetBitmapMatrix();
		hr = matrix->SetData(mat.m_e11, mat.m_e12, mat.m_e21, mat.m_e22, mat.m_x, mat.m_y);
		CHECKRESULT();

		return cbf->QueryInterface(ppIFill);
	}
	else
	{
		CComPtr<IGLD_SolidFillStyle> sf;
		hr = sf.CoCreateInstance(__uuidof(GLD_SolidFillStyle));
		CHECKRESULT();

		gldColor color = pFill->GetSolidFillColor();
		CComPtr<IGLD_Color> pIColor;
		hr = sf->get_Color(&pIColor);
		CHECKRESULT();
		hr = pIColor->SetData(color.red, color.green, color.blue, color.alpha);
		CHECKRESULT();

		return sf->QueryInterface(ppIFill);
	}

	return S_OK;
}

HRESULT CC2I::Create(gldImage *pImage, IGLD_Image **ppIImage)
{
	IF_ARG_NULL_RETURN(pImage);
	IF_ARG_NULL_RETURN(ppIImage);

	*ppIImage = NULL;

	HRESULT hr;

	CComPtr<IGLD_Object> pIObj;
	if ((m_pC2IMap != NULL) && SUCCEEDED(m_pC2IMap->QueryInterface(pImage, &pIObj)))
		return pIObj->QueryInterface(ppIImage);

	CComPtr<IGLD_Image> pIImage;	
	hr = pIImage.CoCreateInstance(__uuidof(GLD_Image));
	CHECKRESULT();

	hr = Convert(pImage, pIImage);
	CHECKRESULT();

	hr = pIImage.CopyTo(ppIImage);
	CHECKRESULT();

	if (m_pC2IMap != NULL)
		m_pC2IMap->Insert(pImage, pIImage);	

	return S_OK;
}

HRESULT CC2I::Create(gldButton *pButton, IGLD_Button **ppIButton)
{
	IF_ARG_NULL_RETURN(pButton);
	IF_ARG_NULL_RETURN(ppIButton);

	*ppIButton = NULL;

	HRESULT hr;

	CComPtr<IGLD_Object> pIObj;
	if ((m_pC2IMap != NULL) && SUCCEEDED(m_pC2IMap->QueryInterface(pButton, &pIObj)))
		return pIObj->QueryInterface(ppIButton);

	CComPtr<IGLD_Button> pIButton;
	hr = pIButton.CoCreateInstance(__uuidof(GLD_Button));
	CHECKRESULT();

	CComPtr<IGLD_MovieClip> pIMovie;
	hr = pIButton->get_MovieClip(&pIMovie);
	CHECKRESULT();

	hr = Convert(pButton->m_bmc, pIMovie);
	CHECKRESULT();
	
	hr = pIButton->put_Name(CComBSTR(pButton->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pButton, pIButton.p);
	CHECKRESULT();

	hr = pIButton->Refresh();
	CHECKRESULT();

	hr = pIButton.CopyTo(ppIButton);
	CHECKRESULT();

	if (m_pC2IMap != NULL)
		m_pC2IMap->Insert(pButton, pIButton);	

	return S_OK;
}

HRESULT CC2I::Create(gldSprite *pSprite, IGLD_Sprite **ppISprite)
{
	IF_ARG_NULL_RETURN(pSprite);
	IF_ARG_NULL_RETURN(ppISprite);

	*ppISprite = NULL;

	HRESULT hr;

	CComPtr<IGLD_Object> pIObj;
	if ((m_pC2IMap != NULL) && SUCCEEDED(m_pC2IMap->QueryInterface(pSprite, &pIObj)))
		return pIObj->QueryInterface(ppISprite);

	CComPtr<IGLD_Sprite> pISprite;
	hr = pISprite.CoCreateInstance(__uuidof(GLD_Sprite));
	CHECKRESULT();

	CComPtr<IGLD_MovieClip> pIMovie;
	hr = pISprite->get_MovieClip(&pIMovie);
	CHECKRESULT();

	hr = pISprite->put_Name(CComBSTR(pSprite->m_name.c_str()));
	CHECKRESULT();

	hr = SetLinkage(pSprite, pISprite.p);
	CHECKRESULT();

	hr = Convert(pSprite->m_mc, pIMovie);
	CHECKRESULT();
	
	hr = pISprite->Refresh();
	CHECKRESULT();

	hr = pISprite.CopyTo(ppISprite);
	CHECKRESULT();

	if (m_pC2IMap != NULL)
		m_pC2IMap->Insert(pSprite, pISprite);

	return S_OK;
}

HRESULT CC2I::Create(gldShape *pShape, IGLD_Shape **ppIShape)
{
	IF_ARG_NULL_RETURN(pShape);
	IF_ARG_NULL_RETURN(ppIShape);

	*ppIShape = NULL;

	HRESULT hr;
	
	CComPtr<IGLD_Object> pIObj;
	if ((m_pC2IMap != NULL) && SUCCEEDED(m_pC2IMap->QueryInterface(pShape, &pIObj)))
		return pIObj->QueryInterface(ppIShape);

	CComPtr<IGLD_Shape> pIShape;
	hr = pIShape.CoCreateInstance(__uuidof(GLD_Shape));
	CHECKRESULT();

	hr = Convert(pShape, pIShape);
	CHECKRESULT();

	hr = pIShape.CopyTo(ppIShape);
	CHECKRESULT();

	if (m_pC2IMap != NULL)
		m_pC2IMap->Insert(pShape, pIShape);

	return S_OK;
}

HRESULT CC2I::Create(gldText2 *pText, IGLD_Text **ppIText)
{
	IF_ARG_NULL_RETURN(pText);
	IF_ARG_NULL_RETURN(ppIText);

	*ppIText = NULL;

	HRESULT hr;
	
	CComPtr<IGLD_Object> pIObj;
	if ((m_pC2IMap != NULL) && SUCCEEDED(m_pC2IMap->QueryInterface(pText, &pIObj)))
		return pIObj->QueryInterface(ppIText);

	CComQIPtr<IGLD_Text> pIText;

	if (pText->m_textType == gtextStatic)
	{
		CComPtr<IGLD_StaticText> pIStatic;
		hr = pIStatic.CoCreateInstance(__uuidof(GLD_StaticText));
		CHECKRESULT();
		hr = Convert(pText, pIStatic);
		CHECKRESULT();
		pIText = pIStatic;
	}
	else if (pText->m_textType == gtextDynamic)
	{
		CComPtr<IGLD_DynamicText> pIDyna;
		hr = pIDyna.CoCreateInstance(__uuidof(GLD_DynamicText));
		CHECKRESULT();
		hr = Convert(pText, pIDyna);
		CHECKRESULT();
		pIText = pIDyna;
	}

	if (pIText == NULL)
		hr = E_UNEXPECTED;
	CHECKRESULT();
	
	hr = pIText.CopyTo(ppIText);
	CHECKRESULT();

	if (m_pC2IMap != NULL)
		m_pC2IMap->Insert(pText, pIText);

	return S_OK;
}

HRESULT CC2I::Create(gldSound *pSound, IGLD_Sound **ppISound)
{
	IF_ARG_NULL_RETURN(pSound);
	IF_ARG_NULL_RETURN(ppISound);

	*ppISound = NULL;

	HRESULT hr;

	CComPtr<IGLD_Object> pIObj;
	if ((m_pC2IMap != NULL) && SUCCEEDED(m_pC2IMap->QueryInterface(pSound, &pIObj)))
		return pIObj->QueryInterface(ppISound);

	CComPtr<IGLD_Sound> pISound;	
	hr = pISound.CoCreateInstance(__uuidof(GLD_Sound));
	CHECKRESULT();

	hr = Convert(pSound, pISound);
	CHECKRESULT();

	hr = pISound.CopyTo(ppISound);
	CHECKRESULT();

	if (m_pC2IMap != NULL)
		m_pC2IMap->Insert(pSound, pISound);

	return S_OK;
}

HRESULT CC2I::Create(gldObj *pObj, IGLD_Object **ppIObj)
{
	IF_ARG_NULL_RETURN(pObj);
	IF_ARG_NULL_RETURN(ppIObj);

	HRESULT hr;
	
	GObjType type = pObj->GetGObjType();
	if (type == gobjShape)
	{
		CComPtr<IGLD_Shape> pIShape;
		hr = Create((gldShape *)pObj, &pIShape);
		CHECKRESULT();
		return pIShape->QueryInterface(ppIObj);
	}
	else if (type == gobjButton)
	{
		CComPtr<IGLD_Button> pIButton;
		hr = Create((gldButton *)pObj, &pIButton);
		CHECKRESULT();
		return pIButton->QueryInterface(ppIObj);
	}
	else if (type == gobjSprite)
	{
		CComPtr<IGLD_Sprite> pISprite;
		hr = Create((gldSprite *)pObj, &pISprite);
		CHECKRESULT();
		return pISprite->QueryInterface(ppIObj);
	}
	else if (type == gobjImage)
	{
		CComPtr<IGLD_Image> pIImage;
		hr = Create((gldImage *)pObj, &pIImage);
		CHECKRESULT();
		return pIImage->QueryInterface(ppIObj);
	}
	else if (type == gobjText)
	{
		CComPtr<IGLD_Text> pIText;
		hr = Create((gldText2 *)pObj, &pIText);
		CHECKRESULT();
		return pIText->QueryInterface(ppIObj);
	}
	else if (type == gobjSound)
	{
		CComPtr<IGLD_Sound> pISound;
		hr = Create((gldSound *)pObj, &pISound);
		CHECKRESULT();
		return pISound->QueryInterface(ppIObj);
	}

	return E_INVALIDARG;
}

HRESULT CC2I::Create(tGuidLine *pGuidLine, IGLD_CurvedPath **ppIPath)
{
	IF_ARG_NULL_RETURN(pGuidLine);
	IF_ARG_NULL_RETURN(ppIPath);

	HRESULT hr = S_OK;

	CComPtr<IGLD_CurvedPath> pIPath;
	hr = pIPath.CoCreateInstance(__uuidof(GLD_CurvedPath));
	CHECKRESULT();

	hr = Convert(pGuidLine, pIPath);
	CHECKRESULT();

	return pIPath.CopyTo(ppIPath);	
}