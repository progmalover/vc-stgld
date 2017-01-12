#pragma once

class gldLayer;
class gldObj;
struct tGuidLine;

void ShowAddShapeTip();
void ShowPenToolTip(CPoint point, BOOL bPenTool);
void ShowConvertSymbolTip(int nType);
void ShowGroupBeforeAddEffectTip();
void ShowInplaceEditTip(gldObj *pObj);
void ShowImportSoundTip();
void ShowInsertGuideLineTip(tGuidLine *pGuidLine);
void ShowNoDrawStreamSoundLayerTip();
void ShowNoDrawLayerLockedTip(gldLayer *pLayer);
void ShowNoDrawLayerInvisibleTip(gldLayer *pLayer);
void ShowNoDrawNoFrameClipTip(gldLayer *pLayer, int nFrame);
void ShowColorButtonTip(CPoint point);
void ShowShowGuidLineTip();