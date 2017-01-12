#pragma once

#include <string>

#include "gld_graph.h"
#include "se_const.h"
#include "gld_draw.h"
#include "gld_selection.h"
#include "tNoneLinearEquation.h"
#include "fit_curves.h"
#include "tGuidLine.h"
#include "Command.h"
#include "tool.h"
#include <vector>
#include "CurvedPath.h"
#include "Observer.h"

using namespace std;

#define THE_BASE(Node)	Node

const int S_WorkSpaceWidth = 3277;
const int S_WorkSpaceHeight = 3277;

#define EDIT_SCENE_MODE		1
#define	EDIT_COMPONENT_MODE	2

struct TShapeEditorGrid
{
	COLORREF	color;
	int			xDist;
	int			yDist;

	TShapeEditorGrid();
	bool operator!=(const TShapeEditorGrid &other) const;
};

struct TShapeEditorAuxLine
{
	typedef vector<int> AuxLines;

	AuxLines	HLines;
	AuxLines	VLines;	
	
	int Find(int pos, bool horz);

};

struct ShapeEditorBase
{
	gld_frame		CurFrame;

	TLineStyle		*pCurLineStyle;
	TFillStyle		*pCurFillStyle;	
	
	TCommands		Commands;
	TTools			Tools;
	
	HPEN			hStockPens[SEP_COUNT * 2];
	HBRUSH			hStockBrushes[SEB_COUNT * 2];
	HINSTANCE		hinstance_res;
	HCURSOR			hStockCursors[IDC_G_LAST - IDC_G_FIRST + 1];

	TGraphicsDevice GDevice;
	HWND			hWnd;	
	double			fZoom;
	double			fOffsetX;
	double			fOffsetY;
	CSize			WorkSpaceSize;
	CSize			MovieSize;
	CSize			ViewSize;
	COLORREF		clMovieBackground;

	gld_shape_sel	sel;

	tGuidLine		*pGuidLine;	

	bool			bShowGuidLine;
	int				nEditMode;
	bool			bLockTool;

	int				nRedraw;	
	gld_rect		rcRedraw;
	int				nRepaint;
	gld_rect		rcRepaint;
	COLORREF		clWorkspaceBkColor;

	TShapeEditorGrid	Grid;
	bool				bShowGrid;
	bool				bSnapToGrid;

	TShapeEditorAuxLine	auxLine;
	bool				bShowAuxLine;

	bool				bShowCanvasBorder;

	ShapeEditorBase() 
	: pCurLineStyle(0)
	, pCurFillStyle(0)
	, fZoom(1.0)	
	, WorkSpaceSize(S_WorkSpaceWidth, S_WorkSpaceHeight)
	, MovieSize(400, 300)
	, ViewSize(400, 300)
	, clMovieBackground(RGB(255, 255, 255))
	, clWorkspaceBkColor(RGB(210, 210, 210))
	, pGuidLine(NULL)
	, hinstance_res(0)
	, bShowGuidLine(true)
	, fOffsetX(0.0)
	, fOffsetY(0.0)
	, nEditMode(EDIT_SCENE_MODE)
	, bLockTool(false)
	, nRedraw(0)
	, nRepaint(0)
	, rcRedraw(0, 0, 0, 0)
	, rcRepaint(0, 0, 0, 0)
	, hWnd(NULL)
	, bShowGrid(false)
	, bSnapToGrid(false)
	, bShowAuxLine(true)
	, bShowCanvasBorder(true)
	{
	}
};

struct TShapeEditorLayout
{
	double	fZoom;
	double	fOffsetX;
	double	fOffsetY;
	CSize	szWorkspace;
	int		nEditMode;

	TShapeEditorLayout();

	bool operator==(const TShapeEditorLayout &other) const;
	bool operator!=(const TShapeEditorLayout &other) const;
};

struct TFillTransformData;
struct TReshapeObject;
struct _path;

class TShapeEditor:public CCmdTarget
{
public:
	TShapeEditor(void) : THE_BASE(m_pBase)(0) {}
	virtual ~TShapeEditor(void) {}


	void Create();
	void Destroy();

	bool Ready() const;
	gld_frame CurFrame();	
	HWND DesignWnd();
	void DesignWnd(HWND _wnd);
	TCommands &Commands();
	TTools &Tools();
	TTool *CurTool();

	CSize GetWorkSpaceSize();

	void SetWorkSpaceSize(const CSize &_size);

	int GetZoomPercent();

	void Redraw(const gld_rect &_r);
	void Redraw();
	void SetRedraw(bool bRedraw);
	void SetRepaint(bool bRepaint);
	int GetRedraw() const;
	int GetRepaint() const;

	void RefreshDesignWnd(const gld_rect &_r);
	void RefreshDesignWnd();
	
	void ScreenToDataSpace(int &_x, int &_y);
	void DataSpaceToScreen(int &_x, int &_y);
	TMatrix ScreenToDataSpace();
	TMatrix DataSpaceToScreen();

	void HorzScroll(int _dx);
	void VertScroll(int _dy);
	void Scroll(int dx, int dy);

	void ZoomTo(int percent, int x, int y);	
	void ZoomIn(int x, int y);
	void ZoomOut(int x, int y);
	void ZoomTo(int percent);
	void ZoomIn(int x1, int y1, int x2, int y2);
	void ZoomOut(int x1, int y1, int x2, int y2);
	void ShowAll();
	void ShowFrame();

	void Display(HDC hdc, int x, int y);
	void SetViewSize(int cx, int cy);
	CSize GetViewSize();
	
	void SetMovieSize(CSize &sz, bool bRedraw);
	CSize GetMovieSize();
	
	gld_shape PtOnShape(int x, int y);

	void SetCurLayer(const gld_layer &layer);
	gld_layer GetCurLayer();

	void SetCurMatrix(float scalex, float scaley, float skewx, float skewy);
	void GetCurMatrix(float &scalex, float &scaley, float &skewx, float &skewy);

	void SetCurDimension(int x, int y, int w, int h);
	void GetCurDimension(int &x, int &y, int &w, int &h);

	void SetDesignWnd(HWND hWnd);
	HWND GetDesignWnd();

	void SetCurTool(int nIndex);
	int GetCurTool();

	void ShowGuidLine(bool bShow);
	bool ShowGuidLine();

	void CenterMovie();
	void GetHScrollRange(int &xmin, int &xmax);
	void GetVScrollRange(int &ymin, int &ymax);
	
	TGraphicsDevice	*GetGraphicsDevice();
	COLORREF GetMovieBackground();
	COLORREF GetWorkspaceBkColor();
	void SetWorkspaceBkColor(COLORREF cl);
	// 不要使用VC++的缺省参数调用, 编译器存在BUG
	void SetMovieBackground(COLORREF clBk, bool bRedraw /*= true*/);

	gld_shape LookupShape(int x, int y);	

public:
	// Grid managment function
	bool IsShowGrid() const;
	void ShowGrid(bool bShow);
	bool IsSnapToGrid() const;
	void SnapToGrid(bool bSnap);
	void SetGrid(const TShapeEditorGrid &grid);
	void DrawGrid();
	void GetGrid(TShapeEditorGrid *grid);

	// canvas style
	bool ShowCanvasBorder() const;
	void ShowCanvasBorder(bool bShow);

protected:
	void ChangeMapMode();
	void LoadRes();	
	void DrawMovieFrame();
	void RebuildBitmap();
	void RedrawObject(gld_rect &rcClip);
	void DrawSelection(HDC hdc);
	void VertScrollBase(int dy);
	void HorzScrollBase(int dx);

	void CreateGDIObject();
	void DestroyGDIObject();
	
public:
	HPEN GetStockPen(int pen);
	HBRUSH GetStockBrush(int brush);
	bool SetStockPen(SEStockPenTypes pen, const LOGPEN *active, const LOGPEN *unactive);
	bool SetStockBrush(SEStockBrushTypes brush, const LOGBRUSH *active, const LOGBRUSH *unactive);
	HCURSOR GetStockCursor(int index);
	
	bool CanAddShape();
	bool CanTransform();
	bool CanReshape();
	bool CanChangeLineStyle();
	bool CanChangeLineStyle(int x, int y);
	bool CanChangeFillStyle();
	bool CanChangeFillStyle(int x, int y);
	bool CanTransformFill();
	bool CanTransformFill(int x, int y);	
	bool ChangeFillStyleCanDrag(int x, int y);
	bool CanAddGuidLine();

	bool CanRepositionGuidPoint(tGuidLine *guidLine, tGuidPoint *guidPoint);
	bool CanRelineGuidLine(tGuidLine *guidLine, tGuidPoint *what);	
	bool CanModifyGuidPoint();

	// Add Shape
	void AddLine(int x1, int y1, int x2, int y2);
	void AddRect(int x1, int y1, int x2, int y2);
	void AddOval(int x1, int y1, int x2, int y2);	
	void AddPath(const _path &_p);
	void AddPath(Point2 *p, int numOfPoints);
	void AddGuidLine(const tPoint *lpPoints, int numOfPoints);	
	void AddGuidLine(tGuidLine *pGuidLine);

	void RepositionGuidPoint(tGuidLine *guidLine, tGuidPoint *what, const tPoint &to);
	void RelineGuidLine(tGuidLine *guidLine, tGuidPoint *what, const tPoint &where, const tPoint &to);
	
	void RemoveGuidPoint(tGuidLine *guidLine, tGuidPoint *guidPoint);
	void InsertGuidPoint(tGuidLine *guidLine, tGuidPoint *what, int x, int y);	
	void OffsetGuidLine(tGuidLine *pGuidLine, int nOffsetX, int nOffsetY);

	// Edit
	void MoveTransform(int dx, int dy, UINT nFlags);
	void ChangeLineStyle(int x, int y);
	void ChangeFillStyle(int x, int y);
	void ChangeFillStyle(int sx, int sy, int ex, int ey);
	void TransformBefore(gld_shape_sel &sel, TMatrix &mat);
	void TransformAfter(gld_shape_sel &sel, TMatrix &mat);
	bool Transform(const gld_shape_sel &sel, const gld_list<TMatrix> &mat);	
	void TransformFill(TFillTransformData &Data, TMatrix &mat);
	void Reshape(TReshapeObject &obj, int x, int y);	
	void ChangeTransOrigin(const gld_shape &shape, int x, int y);

	// sel
public:
	bool GetTransformFill(int x, int y, TFillTransformData &Data, TTool *pTool);
	void GetSelection(gld_shape_sel &sel, TTool *pTool);	
	void NetSelect(int x1, int y1, int x2, int y2, TTool *pTool);
	void Select(int x, int y, gld_shape_sel &sel, TTool *pTool);
	gld_shape PtOnShape(int x, int y, TTool *pTool);
	void DoSelect(gld_shape_sel &sel, bool bsort = true);	
	void ActivateShape(const gld_shape &active);
	gld_shape_sel &CurSel();
	void SelectAll();
	void SelectByLayer(gld_layer &layer);

protected:
	void SortSelection(gld_shape_sel &sel);
	void FilterSelection(gld_shape_sel &sel, TTool *pTool);
	void SetSelection(gld_shape_sel &sel);
	void SelectShapeIfIntersect(gld_shape_sel &sel, const gld_rect &box, TTool *pTool);
	void SelectShapeIfContain(gld_shape_sel &sel, const gld_rect &box, TTool *pTool);

	tGuidLine *SnapToGuidLine(int &x, int &y);
	bool SnapToGrid(int &x, int &y);
	bool SnapToAuxLine(int &x, int &y);

public:
	// Others
	int GetPencilDiameter();
	void GetEffectBox(POINT *effectBox);
	void AdjustAnchorPoint(int &x, int &y);
	void AdjustPoint(int &x, int &y);

	tGuidLine *GetGuidLine(int x, int y);	
	tGuidLine *GetGuidLine();
	void SelectGuidLine(tGuidLine *pGuidLine);

	HRESULT AddPathEffect(REFCLSID clsid);
	HRESULT AddPathEffect(REFCLSID clsid, IGLD_CurvedPath *pIPath);

	HCURSOR	InkBottleCursor(bool Enable);
	HCURSOR PaintBucketCursor(bool Enable);
	HCURSOR FillTransformCursor(bool Enable);
	HCURSOR	LineCursor(bool Enable);
	HCURSOR	RectCursor(bool Enable);
	HCURSOR	OvalCursor(bool Enable);
	HCURSOR	PenCursor(bool Enable);
	HCURSOR	PencilCursor(bool Enable);	

	// Polygon tool
	void AddPolygon(const POINT *v, int count);
	HCURSOR PolygonCursor(bool Enable);

	// Round rectangle tool
	void AddRoundRect(const PiecewiseQuadraticBezier &round);
	HCURSOR RoundRectCursor(bool Enable);

	// Start tool
	HCURSOR StarCursor(bool Enable);
	void AddStar(const POINT *v1, const POINT *v2, int count);
	
	// Anchor point tool
	void RemoveAnchorPoint(gld_shape &shape, gld_draw_obj &obj, gld_a_point &point);
	void InsertAnchorPoint(gld_shape &shape, gld_draw_obj &obj, gld_edge &edge, int x, int y);

	void Reline(TReshapeObject &obj, double u, int x, int y);
	// Auxialiary procedure
	bool CurLayerIsNormal();
	bool IsNormalLayer(gld_layer &layer);

	int PtOnLineStyle(int x, int y, gld_layer &layer, gld_shape &shape, gld_draw_obj &obj);
	int PtOnFillStyle(int x, int y, gld_layer &layer, gld_shape &shape, gld_draw_obj &obj);	
	bool ToolMatchShape(TTool *pTool, gld_shape &shape);

	void CreateTools();	
	// End new interface
	
	void Repaint(HDC hdc);
	void DrawGuidLine(HDC hdc, tGuidLine *pGuidLine);
	void Repaint();

	void AddShape(gld_shape &shape);
	bool LockTool();
	void LockTool(bool bLock);
protected:
	bool InkBottleFetchHitEdge(int x, int y, gld_shape &shape, gld_draw_obj &obj, gld_edge &edge);
	bool PaintBucketFetchObj(int x, int y, gld_shape &shape, gld_draw_obj &obj);

protected:
	int NormalizeZoomPercent(int percent);

protected:
	void RefreshScrollBar();
	void RefreshHScrollBar(bool bRedraw);
	void RefreshVScrollBar(bool bRedraw);	
	bool NormalizeOffset();

public:
	int GetHScrollPos();
	int GetVScrollPos();
	void GetHorzOffsetRange(double &xmin, double &xmax);
	void GetVertOffsetRange(double &ymin, double &ymax);
	
	void SetEditMode(int nMode, bool bRedraw);
	int GetEditMode();

	void SetLayout(const TShapeEditorLayout *pLayout, bool bRedraw);
	void GetLayout(TShapeEditorLayout *pLayout);
	void GetDefaultLayout(TShapeEditorLayout *pLayout, int nEditMode);

	void SetCurFillStyle(TFillStyle *pFillStyle);
	void SetCurLineStyle(TLineStyle *pLineStyle);

	TLineStyle *GetCurLineStyle();
	TFillStyle *GetCurFillStyle();

	gld_rect GetViewRect();
	gld_rect GetClientRect();


	// 辅助线函数
	int AddAuxLine(int pos, bool horz);
	int RemoveAuxLine(int pos, bool horz);	
	TShapeEditorAuxLine &GetAuxLine();
	bool ShowAuxLine();
	void ShowAuxLine(bool show);

protected:
	void DrawAuxLine(HDC hdc);

public:
	struct IGuidLine
	{
		bool CanAddShape();
		void AddLine(int x1, int y1, int x2, int y2);
		void AddPolygon(const POINT *v, int count);
		void AddOval(int x1, int y1, int x2, int y2);	
		void AddPath(const _path &_p);
		void AddPath(Point2 *p, int numOfPoints);
		HCURSOR LineCursor(bool Enable);
		HCURSOR PolygonCursor(bool Enable);
		HCURSOR OvalCursor(bool Enable);
		HCURSOR PenCursor(bool Enable);
		HCURSOR PencilCursor(bool Enable);
		HPEN GetStockPen(int pen);
		HBRUSH GetStockBrush(int brush);
		HCURSOR GetStockCursor(int index);
		int GetPencilDiameter();
		void GetSelection(gld_shape_sel &sel, TTool *pTool);
		void DoSelect(gld_shape_sel &sel, bool bsort = true);
		void Repaint();
		void SetRepaint(bool bRepaint);	
		void AdjustPoint(int &x, int &y);
	} m_xIGuidLine;

	class ISelChange : public CObserver
	{
	public:		
		virtual void Update(void *pData);
	} m_xISelChange;

protected:
	static void OnNotify(void *pThis, int nNotifyID, LPARAM lParam);
	void OnSelectionChanged();

protected:
	ShapeEditorBase *m_pBase;
	CLSID m_clsidPathEffect;
};

extern TShapeEditor my_app;
