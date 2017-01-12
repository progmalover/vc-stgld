#pragma once

#include "../GlandaCOM/GlandaCOM.h"
#include "../GlandaCom/GLD_ATL.h"
#include <vector>
#include <map>
using namespace std;

class gldShape;
class gldLineStyle;
class gldFillStyle;
class gldMatrix;
class gldColor;
class gldMovieClip;
class gldButton;
class gldObj;
class gldCxform;
class gldImage;
class gldSprite;
class gldText2;
class gldShapeRecord;
class gldGradientRecord;
class gldSound;
class gldAction;
struct gldMotionTweenParam;
struct tGuidLine;

typedef vector<gldLineStyle *> GLineStyles;
typedef vector<gldFillStyle *> GFillStyles;
typedef vector<gldShapeRecord *> GShapeRecords;
typedef vector<gldGradientRecord *> GGradientRecords;
typedef map<CComGUID, gldObj *> GI2Object;

class CObjectMap;

// CI2CMap
class CI2CMap
{
public:
	inline HRESULT QueryObject(IGLD_Object *pIObj, gldObj **ppObj)
	{
		IF_ARG_NULL_RETURN(pIObj);
		IF_ARG_NULL_RETURN(ppObj);
		_I2CMap::iterator i = m_Map.find(pIObj);
		if (i == m_Map.end())
			return E_FAIL;
		*ppObj = (*i).second;
		return S_OK;
	}
	
	inline void Insert(IGLD_Object *pIObj, gldObj *pObj)
	{
		ASSERT(pIObj != NULL);
		ASSERT(pObj != NULL);

		m_Map.insert(_I2CMap::value_type(pIObj, pObj));
	}

	void CopyTo(CObjectMap *pObjectMap);

	void FreeAllSymbol();

public:
	typedef map<CComPtr<IGLD_Object>, gldObj *> _I2CMap;

	_I2CMap m_Map;
};

// CI2C
class CI2C
{
public:
	CI2C(void);
	~CI2C(void);

public:	
	static void UpdateTShapePtr(gldObj *pObj);

	static HRESULT Convert(IGLD_Shape *pIShape, gldShape *pShape);
	static HRESULT Convert(IGLD_Button *pIButton, gldButton *pButton);
	static HRESULT Convert(IGLD_Sprite *pISprite, gldSprite *pSprite);
	static HRESULT Convert(IGLD_Image *pIImage, gldImage *pImage);
	static HRESULT Convert(IGLD_Sound *pISound, gldSound *pSound);
	static HRESULT Convert(IGLD_Text *pIText, gldText2 *pText);
	static HRESULT Convert(IGLD_StaticText *pIText, gldText2 *pText);
	static HRESULT Convert(IGLD_DynamicText *pIText, gldText2 *pText);

	static HRESULT Convert(IGLD_Motion *pIMotion, gldMotionTweenParam *pMotion);
	static HRESULT Convert(IGLD_MovieClip *pIMovieClip, gldMovieClip *pMovieClip);
	static HRESULT Convert(IGLD_FillStyle *pIFill, gldFillStyle *pFill);
	static HRESULT Convert(IGLD_LineStyle *pILine, gldLineStyle *pLine);
	static HRESULT Convert(IGLD_Matrix *pIMatrix, gldMatrix *pMatrix);
	static HRESULT Convert(IGLD_Cxform *pICxform, gldCxform *pCxform);
	static HRESULT Convert(IGLD_Color *pIColor, gldColor *pColor);
	static HRESULT Convert(IGLD_FillStyles *pIFills, GFillStyles *pFills);
	static HRESULT Convert(IGLD_LineStyles *pILines, GLineStyles *pLines);
	static HRESULT Convert(IGLD_Action *pIAction, gldAction *pAction);
	static HRESULT Convert(IGLD_CurvedPath *pIPath, tGuidLine *pGuidLine);

	static HRESULT Create(IGLD_Object *pIObject, gldObj **ppObject);
	static HRESULT Create(IGLD_Image *pIImage, gldImage **ppImage);
	static HRESULT Create(IGLD_Sound *pISound, gldSound **ppSound);
	static HRESULT Create(IGLD_Text *pIText, gldText2 **ppText);
	static HRESULT Create(IGLD_Shape *pIShape, gldShape **ppShape);
	static HRESULT Create(IGLD_Button *pIButton, gldButton **ppButton);
	static HRESULT Create(IGLD_Sprite *pISprite, gldSprite **ppSprite);
	static HRESULT Create(IGLD_LineStyle *pILine, gldLineStyle **ppLine);
	static HRESULT Create(IGLD_FillStyle *pIFill, gldFillStyle **ppFill);
	static HRESULT Create(IGLD_CurvedPath *pIPath, tGuidLine **ppGuidLine);

	static HRESULT GetShowList(IGLD_MovieClip *pIMovieClip, int nTime, gldMovieClip *pMovieClip);

	static CI2CMap *m_pI2CMap;
};
