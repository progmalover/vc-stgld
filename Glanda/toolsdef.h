#pragma once

#include "my_app.h"
#include "LineTool.h"
#include "RectTool.h"
#include "OvalTool.h"
#include "PenTool.h"
#include "PencilTool.h"
#include "TransformToolEx.h"
#include "ReshapeToolEx.h"
#include "FillTransformTool.h"
#include "InkBottleTool.h"
#include "PaintBucketTool.h"
#include "PolygonTool.h"
#include "RoundRectTool.h"
#include "StarTool.h"
#include "ZoomTool.h"
#include "ITextTool.h"
#include "HandTool.h"
#include "TextToolEx.h"


typedef TLineToolTmpl<TShapeEditor> SELineTool;
typedef TRectToolTmpl<TShapeEditor> SERectTool;
typedef TOvalToolTmpl<TShapeEditor> SEOvalTool;
typedef TPenToolTmpl<TShapeEditor>	SEPenTool;
typedef TPencilToolTmpl<TShapeEditor> SEPencilTool;
typedef TTransformToolExTmpl<TShapeEditor> SETransformTool;
typedef TReshapeToolExTmpl<TShapeEditor> SEReshapeTool;
typedef TFillTransformToolTmpl<TShapeEditor> SEFillTransformTool;
typedef TInkBottleToolTmpl<TShapeEditor> SEInkBottleTool;
typedef TPaintBucketToolTmpl<TShapeEditor> SEPaintBucketTool;
typedef TPolygonToolTmpl<TShapeEditor> SEPolygonTool;
typedef TRoundRectToolTmpl<TShapeEditor> SERoundRectTool;
typedef TStarToolTmpl<TShapeEditor> SEStarTool;		
typedef TZoomToolTmpl<TShapeEditor> SEZoomTool;
typedef THandToolTmpl<TShapeEditor> SEHandTool;
typedef TLineToolTmpl<TShapeEditor::IGuidLine>	SELineGuidLineTool;
typedef TPolygonToolTmpl<TShapeEditor::IGuidLine>	SEPolygonGuidLineTool;
typedef TOvalToolTmpl<TShapeEditor::IGuidLine>	SEOvalGuidLineTool;
typedef TPenToolTmpl<TShapeEditor::IGuidLine>		SEPenGuidLineTool;
typedef TPencilToolTmpl<TShapeEditor::IGuidLine>	SEPencilGuidLineTool;
typedef TTextToolExTmpl<ITextTool>	SETextToolEx;