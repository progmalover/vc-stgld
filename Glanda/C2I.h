#pragma once

#include "../GlandaCom/GLD_ATL.h"
#include "../GlandaCOM/GlandaCOM.h"
#include <vector>
#include <map>
using namespace std;

class gldImage;
class gldShape;
class gldLineStyle;
class gldFillStyle;
class gldShapeRecord;
class gldGradientRecord;
class gldMovieClip;
class gldButton;
class gldSprite;
class gldLayer;
class gldFrameClip;
class gldObj;
class gldText2;
class gldSound;
class CI2CMap;
class gldAction;
struct gldMotionTweenParam;
struct tGuidLine;

typedef vector<gldLineStyle *> GLineStyles;
typedef vector<gldFillStyle *> GFillStyles;
typedef vector<gldShapeRecord *> GShapeRecords;
typedef vector<gldGradientRecord *> GGradientRecords;

// CC2IMap
class CC2IMap
{
public:	
	inline HRESULT QueryInterface(gldObj *pObj, IGLD_Object **ppIObj)
	{
		IF_ARG_NULL_RETURN(pObj);
		IF_ARG_NULL_RETURN(ppIObj);

		_C2IMap::iterator i = m_Map.find(pObj);
		if (i == m_Map.end())
			return E_FAIL;

		return (*i).second.CopyTo(ppIObj);
	}

	inline void Insert(gldObj *pObj, IGLD_Object *pIObj)
	{
		ASSERT(pObj != NULL);
		ASSERT(pIObj != NULL);

		m_Map.insert(_C2IMap::value_type(pObj, pIObj));
	}

	void CopyTo(CI2CMap *pRevMap);

public:
	typedef map<gldObj *, CComPtr<IGLD_Object> > _C2IMap;

	_C2IMap	m_Map;
};

// CC2I
class CC2I
{
public:
	static HRESULT Convert(gldImage *pImage, IGLD_Image *pIImage);
	static HRESULT Convert(gldSound *pSound, IGLD_Sound *pISound);
	static HRESULT Convert(gldShape *pShape, IGLD_Shape *pIShape);
	static HRESULT Convert(GShapeRecords *pRecords, IGLD_ShapeRecords *pIRecords);
	static HRESULT Convert(GLineStyles *pLines, IGLD_LineStyles *pILines);
	static HRESULT Convert(GFillStyles *pFills, IGLD_FillStyles *pIFills);
	static HRESULT Convert(gldMovieClip *pMovie, IGLD_MovieClip *pIMovie);
	static HRESULT Convert(gldLayer *pLayer, IGLD_Layer *pILayer);
	static HRESULT Convert(gldFrameClip *pFrame, IGLD_FrameClip *pIFrame);
	static HRESULT Convert(gldMotionTweenParam *pMotion, IGLD_Motion *pIMotion);
	static HRESULT Convert(gldText2 *pText, IGLD_Text *pIText);
	static HRESULT Convert(gldText2 *pText, IGLD_StaticText *pIText);
	static HRESULT Convert(gldText2 *pText, IGLD_DynamicText *pIText);	
	static HRESULT Convert(gldAction *pAction, IGLD_Action *pIAction);
	static HRESULT Convert(tGuidLine *pGuidLine, IGLD_CurvedPath *pIPath);

	static HRESULT Create(gldShapeRecord *pRecord, IGLD_ShapeRecord **ppIRecord);
	static HRESULT Create(gldLineStyle *pLine, IGLD_LineStyle **ppILine);
	static HRESULT Create(gldFillStyle *pFill, IGLD_FillStyle **ppIFill);
	static HRESULT Create(gldImage *pImage, IGLD_Image **ppIImage);
	static HRESULT Create(gldButton *pButton, IGLD_Button **ppIButton);
	static HRESULT Create(gldSprite *pSprite, IGLD_Sprite **ppISprite);
	static HRESULT Create(gldShape *pShape, IGLD_Shape **ppIShape);
	static HRESULT Create(gldText2 *pText, IGLD_Text **ppIText);
	static HRESULT Create(gldSound *pSound, IGLD_Sound **ppISound);
	static HRESULT Create(gldObj *pObj, IGLD_Object **ppIObj);	
	static HRESULT Create(tGuidLine *pGuidLine, IGLD_CurvedPath **ppIPath);

	static CC2IMap *m_pC2IMap;
};