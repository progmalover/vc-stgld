#pragma once
#include "gldCharacter.h"
#include "gld_selection.h"
#include "gldMatrix.h"
#include "gldCxform.h"
#include "ObjInstance.h"

class gldFrameClip;
class gldLayer;
class gldMovieClip;
class gldObj;
class gldSprite;
class gldButton;
class gldCharacterKey;
class gldText;
class gldMainMovie2;
class gldImage;
class gldShape;

class gldInstance;
interface IGLD_Parameters;

class CSWFProxy
{
public:
	CSWFProxy(void);
	~CSWFProxy(void);

	static void UpdateFrameClipValidFlag(gldFrameClip *pFC);
	static gldLayer *GetFrameClipLayer(gldFrameClip *pFC);
	static gldFrameClip *GetNextFrameClip(gldFrameClip *pFC);
	static gldFrameClip *GetPrevFrameClip(gldFrameClip *pFC);
	static gldFrameClip *GetPrevFrameClip(gldLayer *pLayer, const GFRAMECLIP_LIST_IT &pos);
	static gldFrameClip *GetNextFrameClip(gldLayer *pLayer, const GFRAMECLIP_LIST_IT &pos);

	static int GetCurTime();
	static gldMovieClip *GetCurMovieClip();
	static gldMainMovie2 *GetMainMovie();
	static gldLayer *GetCurLayer();
	static int GetCurLayerIndex();
	static gldFrameClip *GetCurFrameClip();
	static gldObj *GetCurObj();
	static GFRAMECLIP_LIST_IT GetCurFrameClipIndex();
	
	static void UpdateCurFrameClipValidFlags();
	static void UpdateFrameClipValidFlags(gldFrameClip *pFC);

	static int GetFrameClipEndTime(gldFrameClip *pFC);
	static bool TimeInFrameClip(gldFrameClip *pFC, int nTime);

	static void UpdateFrameClip(gldFrameClip *pFC);
	static void SetFrameClipStatic(gldFrameClip *pFC);
	static void RefreshAllMotionTweenInLayer(gldLayer *pLayer);
	static void RefreshMotionTween(gldFrameClip *pFC);
	static void PrepareTShapeForGObj(gldObj *pObj);

	static gldCharacterKey *CloneCkForFrameClip(gldFrameClip *pFrame, gldCharacterKey *pTemp, 
		const gldMatrix &mx, const gldCxform &cx, int ratio = 0);
	static gldCharacterKey *CloneCkForFrameClip(gldFrameClip *pFrame, gldCharacterKey *pTemp);

	static void PutCkToFrameClip(gldFrameClip *pFrame, gldCharacterKey *pKey);
	static void PutSelToFrameClip(gldFrameClip *pFrame, const gld_shape_sel &sel, int offsetx, int offsety);
	static void PutSelToFrameClipCenter(gldFrameClip *pFrame, const gld_shape_sel &sel);
	static gldSprite *NewGroup();
	static gldSprite *CreateGroupFromSel(const gld_shape_sel &sel);
	static gldButton *CreateButtonFromSel(const gld_shape_sel &sel);
	static gldButton *CreateButtonFromSprite(gldSprite *pSprite);
	static gldSprite *CreateSpriteFromSel(const gld_shape_sel &sel);	
	static gldSprite *CreateSpriteFromButton(gldButton *pButton);
	static gldCharacterKey *CreateCharacterKey(gldFrameClip *pFC, gldObj *pObj, const gldMatrix &mx, const gldCxform &cx);	
	static gldShape *CreateShapeWrapImage(gldImage *gimage);

	static void BreakApartTextToShapes(gld_shape &shape, OBJINSTANCE_LIST &lstInst);
	static void BreakApartTextToShapes(gldText *pText, gldMatrix &mx, gldCxform &cx, OBJINSTANCE_LIST &lstInst);
	static void BreakApartTextToShapes(gldText *pText, gldMatrix &mx, gldCxform &cx, IGLD_Parameters *pIParameters, OBJINSTANCE_LIST &lstInst);

	static bool GObjIsValidGroup(gldObj *pObj);
	static void SortSelLeftRight(gld_list<gld_shape> &lst);
	static gldLayer *GetObjFirstLayer(gldObj *pObj);

	static int GetObjRowData(gldObj *pObj, U8 **pData);
	static void GetRefObjList(gldObj *pObj, list<gldObj *> &lstObj);

	static gldMovieClip *GetObjMovieClip(gldObj *pObj);

protected:
	static void GetRefObjList(gldCharacterKey *pKey, list<gldObj *> &lstObj);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTraitCharacter
{
public:
	CTraitCharacter(gld_shape &shape);

	operator gldCharacter*();
	operator gldCharacterKey*();
	operator gldObj*();

	static void Detach(gldCharacter *pChar);
	static void Detach(gld_shape &shape);
	static void Attach(gldCharacter *pChar, gld_shape &shape);
protected:
	gldCharacter	*m_pCharacter;
};

class CTraitInstance
{
public:
	typedef gldInstance GInstance;

	CTraitInstance(gld_shape &shape);
	CTraitInstance(GInstance *pInstance);

	operator GInstance*();
	operator gldObj*();

	static void Detach(GInstance *pInstance);
	static void Detach(gld_shape &shape);
	static void Attach(GInstance *pInstance, gld_shape &shape);	

protected:
	GInstance *m_pInstance;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTraitShape
{
public:
	CTraitShape(gldCharacter *pChar);
	CTraitShape(gldCharacterKey *pKey);
	CTraitShape(gldObj *pObj);
	CTraitShape(gldInstance *pInstance);
	operator gld_shape();
	
protected:
	gld_shape	m_Shape;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTraitLayer
{
public:
	CTraitLayer(gld_layer &layer);
	CTraitLayer(gldLayer *pLayer);
	CTraitLayer(gldInstance *pInstance);

	operator gld_layer();
	operator gldLayer*();

	static void Detach(gld_layer &layer);
	static void Detach(gldLayer *pLayer);
	static void Attach(gldLayer *pLayer, gld_layer &layer);
protected:
	gld_layer	m_layer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TImage;
class gldImage;
class CTraitImage
{
public:
	CTraitImage(TImage *ptImg);
	CTraitImage(gldImage *pgImg);

	operator gldImage*();
	operator TImage*();

	static void Bind(gldImage *pgImg, TImage *ptImg);
	static void Unbind(gldImage *pgImg);
	static void Unbind(TImage *ptImg);
	static bool Binded(gldImage *pgImg);
	static bool Binded(TImage *ptImg);
protected:
	gldImage *m_pImage;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGuardUpdateCurFCUI
{
public:
	CGuardUpdateCurFCUI();
	~CGuardUpdateCurFCUI();

protected:
	gldFrameClip	*m_pOldFC;
	int				m_nOldTime;
};