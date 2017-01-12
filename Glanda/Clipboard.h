#pragma once

#include "BinStream.h"
#include "gldLibrary.h"
#include "gldCharacter.h"
#include "gldFrameClip.h"

#include "transadaptor.h"

class TCommandGroup;

//extern UINT CF_GMOVIE_SYMBOL;
//extern UINT CF_GMOVIE_INSTANCE;
//extern UINT CF_GMOVIE_FRAMECLIP;
extern UINT CF_GMOVIE_TEXT2;
//extern UINT CF_GMOVIE_GUIDLINE;

//void WriteSymbolsToBinStream(oBinStream &os, const GOBJECT_LIST &objList, const GOBJECT_LIST &objListSel);
//void ReadSymbolsFromBinStream(iBinStream &is, TCommandGroup *pCmdGroup);
void GetUsedObjsRecursive(gldObj *pObj, gldLibrary *pLib);
BOOL CanPlaceInsideCurObj(gldObj *pObj);
BOOL CanPlaceInsideObj(gldObj *pParentObj, gldObj *pChildObj);