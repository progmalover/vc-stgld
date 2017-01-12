#include "StdAfx.h"
#include "my_app.h"

#include "Image.h"
#include "tips.h"

#include "Observer.h"
#include "gld_func.h"
#include "ScopeGuard.h"

#include "toolsdef.h"

#include "gld_shape_builder.h"
#include "CmdAddShape.h"
#include "CmdMoveControlPoint.h"
#include "CmdMoveAnchorPoint.h"
#include "CmdInsertAnchorPoint.h"
#include "CmdRemoveAnchorPoint.h"
#include "CmdReline.h"
#include "CmdTransformFillStyle.h"
#include "CmdChangeLineStyle.h"
#include "CmdChangeFillStyle.h"
#include "CmdChangeTransformOrigin.h"
#include "EffectWnd.h"
#include "EffectCommonUtils.h"
#include <atlsafe.h>
#include "..\GlandaCOM\GLD_ATL.h"
#include "CmdInsertEffect.h"
#include "GlandaDoc.h"
#include "EffectInplaceEditor.h"
#include "GlandaClipboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TShapeEditorGrid::TShapeEditorGrid()
: color(RGB(192, 192, 192))
, xDist(400)
, yDist(400)
{
}

bool TShapeEditorGrid::operator!=(const TShapeEditorGrid &other) const
{
	return color != other.color || xDist != other.xDist || yDist != other.yDist;
}

////////////////////////////////////////////////////////////////////

static ITextTool	TextToolIClass;	// text tool interface

template <class _Tx>
void LimitValue(_Tx &v, const _Tx &vmin, const _Tx &vmax)
{
	if (v < vmin)
	{
		v = vmin;
	}
	else if (v > vmax)
	{
		v = vmax;
	}
}

void TShapeEditor::Create()
{
	m_pBase = new ShapeEditorBase();

	// Create default fill and line style
	TLineStyle *_ls = new TLineStyle(20, TColor(0, 0, 0, 255));
	_ls->AddRef();
	THE_BASE(m_pBase)->pCurLineStyle = _ls;
	TFillStyle *_fs = new TSolidFillStyle(TColor(255, 0, 0, 255));
	_fs->AddRef();
	THE_BASE(m_pBase)->pCurFillStyle = _fs;
	// Load grid setting
	THE_BASE(m_pBase)->Grid.xDist = min(PIXEL_TO_TWIPS(40), max(PIXEL_TO_TWIPS(4), (int)AfxGetApp()->GetProfileInt("Views\\Grid", "X-Distance", PIXEL_TO_TWIPS(20))));
	THE_BASE(m_pBase)->Grid.yDist = min(PIXEL_TO_TWIPS(40), max(PIXEL_TO_TWIPS(4), (int)AfxGetApp()->GetProfileInt("Views\\Grid", "Y-Distance", PIXEL_TO_TWIPS(20))));
	THE_BASE(m_pBase)->Grid.color = AfxGetApp()->GetProfileInt("Views\\Grid", "Color", 0x00C0C0C0);
	THE_BASE(m_pBase)->bShowGrid = (AfxGetApp()->GetProfileInt("Views\\Grid", "Show", 0) != 0);
	THE_BASE(m_pBase)->bSnapToGrid = (AfxGetApp()->GetProfileInt("Views\\Grid", "Snap", 0) != 0);
	THE_BASE(m_pBase)->bShowGuidLine = (AfxGetApp()->GetProfileInt("Views\\Guide Line", "Show", 1) != 0);
	THE_BASE(m_pBase)->bShowAuxLine = (AfxGetApp()->GetProfileInt("Views\\Ruler", "Show", 0) != 0);

	// Make frame and default layer
	THE_BASE(m_pBase)->CurFrame.create();	
	//gld_layer	layer;
	//layer.create();
	//THE_BASE(m_pBase)->CurFrame.push_back(layer);
	//THE_BASE(m_pBase)->CurFrame.cur_layer(layer);	

	// Make GDI object for tools
	CreateGDIObject();
	LoadRes();
	
	COLORREF _ws_cl = THE_BASE(m_pBase)->clWorkspaceBkColor;
	THE_BASE(m_pBase)->GDevice.SetBkColor(TColor(GetRValue(_ws_cl), GetGValue(_ws_cl), GetBValue(_ws_cl), 255));

	CreateTools();

	ChangeMapMode();

	CSubjectManager::Instance()->GetSubject("Select")->Attach(&m_xISelChange);
}

void TShapeEditor::Destroy()
{
	CSubjectManager::Instance()->GetSubject("Select")->Detach(&m_xISelChange);

	// save tools' options
	AfxGetApp()->WriteProfileInt("Tools\\Round Rect", "Radius", ((SERoundRectTool *)Tools()[IDT_ROUNDRECT])->GetRoundRadius());
	AfxGetApp()->WriteProfileInt("Tools\\Polygon", "Vertex", ((SEPolygonTool *)Tools()[IDT_POLYGON])->GetVertexNum());
	AfxGetApp()->WriteProfileInt("Tools\\Star", "", ((SEStarTool *)Tools()[IDT_STAR])->GetVertexNum());
	AfxGetApp()->WriteProfileInt("Tools\\Polygon Guide Line", "Vertex", ((SEPolygonGuidLineTool *)Tools()[IDT_POLYGONGUIDLINE])->GetVertexNum());
	// save grid settings	
	AfxGetApp()->WriteProfileInt("Views\\Grid", "X-Distance", THE_BASE(m_pBase)->Grid.xDist);
	AfxGetApp()->WriteProfileInt("Views\\Grid", "Y-Distance", THE_BASE(m_pBase)->Grid.yDist);
	AfxGetApp()->WriteProfileInt("Views\\Grid", "Color", THE_BASE(m_pBase)->Grid.color);
	AfxGetApp()->WriteProfileInt("Views\\Grid", "Show", THE_BASE(m_pBase)->bShowGrid ? 1 : 0);
	AfxGetApp()->WriteProfileInt("Views\\Grid", "Snap", THE_BASE(m_pBase)->bSnapToGrid ? 1 : 0);
	// save guide line settings
	AfxGetApp()->WriteProfileInt("Views\\Guide Line", "Show", THE_BASE(m_pBase)->bShowGuidLine ? 1 : 0);
	AfxGetApp()->WriteProfileInt("Views\\Ruler", "Show", THE_BASE(m_pBase)->bShowAuxLine ? 1 : 0);

	// release resource
	RELEASE(THE_BASE(m_pBase)->pCurLineStyle);
	RELEASE(THE_BASE(m_pBase)->pCurFillStyle);
	THE_BASE(m_pBase)->CurFrame.abandon();
	THE_BASE(m_pBase)->CurFrame.destroy();
	DestroyGDIObject();
	delete m_pBase;
	m_pBase = 0;	
}

COLORREF TShapeEditor::GetWorkspaceBkColor()
{
	return THE_BASE(m_pBase)->clWorkspaceBkColor;
}

void TShapeEditor::SetWorkspaceBkColor(COLORREF cl)
{
	if (GetWorkspaceBkColor() != cl)
	{
		THE_BASE(m_pBase)->clWorkspaceBkColor = cl;		
		THE_BASE(m_pBase)->GDevice.SetBkColor(TColor(GetRValue(cl), GetGValue(cl), GetBValue(cl), 255));
		THE_BASE(m_pBase)->GDevice.ClearRect();
		Redraw();
		Repaint();
	}
}

// 重绘失效矩形
void TShapeEditor::Redraw(const gld_rect &_r)
{
	if (THE_BASE(m_pBase)->nRedraw > 0)
	{
		if (THE_BASE(m_pBase)->rcRedraw.is_empty())
		{
			THE_BASE(m_pBase)->rcRedraw = _r;
		}
		else
		{
			THE_BASE(m_pBase)->rcRedraw = THE_BASE(m_pBase)->rcRedraw + _r;
		}

		return;
	}

	gld_rect	iv_rc = _r;

	if (!THE_BASE(m_pBase)->rcRedraw.is_empty())
	{
		iv_rc = iv_rc + THE_BASE(m_pBase)->rcRedraw;
	}

	iv_rc.inflate(-21, -21, 21, 21);
	DataSpaceToScreen(iv_rc.left, iv_rc.top);
	DataSpaceToScreen(iv_rc.right, iv_rc.bottom);
	int	xmin = iv_rc.left / 20;
	int ymin = iv_rc.top / 20;
	int xmax = (iv_rc.right + 19) / 20;
	int ymax = (iv_rc.bottom + 19) / 20;

	iv_rc.left = PIXEL_TO_TWIPS(xmin);
	iv_rc.top = PIXEL_TO_TWIPS(ymin);
	iv_rc.right = PIXEL_TO_TWIPS(xmax);
	iv_rc.bottom = PIXEL_TO_TWIPS(ymax);

	// Draw background and canvas frame
	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		THE_BASE(m_pBase)->GDevice.ClearRect(xmin, ymin, xmax, ymax);			
		GGRECT	oldClip = THE_BASE(m_pBase)->GDevice.GetClipRect();
		THE_BASE(m_pBase)->GDevice.SetClipRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom);
		DrawMovieFrame();		
		THE_BASE(m_pBase)->GDevice.SetClipRect(oldClip);
	}
	else
	{
		COLORREF bk_cl = THE_BASE(m_pBase)->clMovieBackground;
		THE_BASE(m_pBase)->GDevice.FillRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom, TColor(GetRValue(bk_cl), GetGValue(bk_cl), GetBValue(bk_cl), 255));
	}	

	// Draw Object
	RedrawObject(iv_rc);

	THE_BASE(m_pBase)->rcRedraw.empty();		
}

// 重绘整个画布
void TShapeEditor::Redraw()
{		
	Redraw(GetViewRect());

}

// 绘画整个绘图区窗口，和里面的shape 
void TShapeEditor::Repaint(HDC hdc)
{	
	ASSERT(m_pBase);

	if (!m_pBase)
	{
		return;
	}

	if (THE_BASE(m_pBase)->nRepaint > 0)
	{
		
		THE_BASE(m_pBase)->rcRepaint = GetClientRect();
		
		return;
	}

	if (hdc)
	{
		HDC	hMemDC = ::CreateCompatibleDC(hdc);
		if (hMemDC)
		{	
			ScopeGuard	g1 = MakeGuard(&DeleteDC, hMemDC);

			HBITMAP	hBitmap = ::CreateCompatibleBitmap(hdc, THE_BASE(m_pBase)->ViewSize.cx, THE_BASE(m_pBase)->ViewSize.cy);
			if (hBitmap)
			{
				ScopeGuard g2 = MakeGuard(&DeleteObject, hBitmap);

				HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
				if (hOldBitmap)
				{
					ScopeGuard g3 = MakeGuard(&SelectObject, hMemDC, hOldBitmap);

					Display(hMemDC, 0, 0);
					
					// Draw effect
					EffectInplaceEditor::Instance()->DrawEffect(hMemDC);					

					if (THE_BASE(m_pBase)->nEditMode == EDIT_COMPONENT_MODE)
					{
						// 画原点
						int x = 0;
						int y = 0;
						DataSpaceToScreen(x, y);
						x = TWIPS_TO_PIXEL(x);
						y = TWIPS_TO_PIXEL(y);
						
						// 保证原点在任何背景下都显示出来
						int	 nOldR2 = ::SetROP2(hMemDC, R2_NOTXORPEN);
						HPEN hOldPen = (HPEN)::SelectObject(hMemDC, GetStockPen(SEP_HANDLER));

						ScopeGuard g4 = MakeGuard(&SetROP2, hMemDC, nOldR2);
						ScopeGuard g5 = MakeGuard(&SelectObject, hMemDC, hOldPen);

						::MoveToEx(hMemDC, x - 4, y, NULL);
						::LineTo(hMemDC, x + 5, y);
						::MoveToEx(hMemDC, x, y - 4, NULL);
						::LineTo(hMemDC, x, y + 5);
					}

					if (GetCurTool() > -1)
					{
						TTool *pTool = CurTool();

						if ((pTool->Capabilities() & TC_DRAWSELECTION) != TC_DRAWSELECTION)
						{
							DrawSelection(hMemDC);
						}

						pTool->Draw(hMemDC);
					}

					if (ShowAuxLine())
					{
						DrawAuxLine(hMemDC);
					}

					::BitBlt(hdc, 0, 0, THE_BASE(m_pBase)->ViewSize.cx, THE_BASE(m_pBase)->ViewSize.cy, hMemDC, 0, 0, SRCCOPY);
				}				
			}			
		}		
	}

	THE_BASE(m_pBase)->rcRepaint.empty();
}

// 重新绘制设计窗口
void TShapeEditor::Repaint()
{
	ASSERT(m_pBase);

	if (m_pBase)
	{
		ASSERT(THE_BASE(m_pBase)->hWnd);

		if (THE_BASE(m_pBase)->hWnd != NULL)
		{
			HDC	hDC = ::GetDC(THE_BASE(m_pBase)->hWnd);

			if (hDC)
			{
				ScopeGuard	g = MakeGuard(&::ReleaseDC, THE_BASE(m_pBase)->hWnd, hDC);

				Repaint(hDC);			
			}
		}
	}
}

gld_rect TShapeEditor::GetViewRect()
{
	int	cx, cy;

	THE_BASE(m_pBase)->GDevice.GetSize(cx, cy);
	
	int xmax = PIXEL_TO_TWIPS(cx - 1);
	int ymax = PIXEL_TO_TWIPS(cy - 1);

	gld_rect rc(0, 0, xmax, ymax);

	ScreenToDataSpace(rc.left, rc.top);
	ScreenToDataSpace(rc.right, rc.bottom);

	return rc;
}

// 绘制网格
void TShapeEditor::DrawGrid()
{
	if (!GetCurLayer().validate())
		return;

	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		int	cx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cx);
		int	cy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cy);
		int	xmin = 0, ymin = 0;
		int	xmax = cx, ymax = cy;
		int xDist = _round(THE_BASE(m_pBase)->fZoom * THE_BASE(m_pBase)->Grid.xDist);
		int yDist = _round(THE_BASE(m_pBase)->fZoom * THE_BASE(m_pBase)->Grid.yDist);
		COLORREF cl = THE_BASE(m_pBase)->Grid.color;
		TColor	 tcl(GetRValue(cl), GetGValue(cl), GetBValue(cl), 255);
		
		DataSpaceToScreen(xmin, ymin);
		DataSpaceToScreen(xmax, ymax);
		
		if (ShowCanvasBorder())
		{
			// 水平线
			for (int y = ymin + yDist; y < ymax; y += yDist)		
				THE_BASE(m_pBase)->GDevice.HorzLine(y, xmin + 20, xmax - 20, tcl);
			// 垂直线
			for (int x = xmin + xDist; x < xmax; x += xDist)			
				THE_BASE(m_pBase)->GDevice.VertLine(x, ymin + 20, ymax - 20, tcl);
		}
		else
		{
			// 水平线
			for (int y = ymin + yDist; y < ymax; y += yDist)		
				THE_BASE(m_pBase)->GDevice.HorzLine(y, xmin, xmax, tcl);
			// 垂直线
			for (int x = xmin + xDist; x < xmax; x += xDist)			
				THE_BASE(m_pBase)->GDevice.VertLine(x, ymin, ymax, tcl);
		}
	}
	else
	{
		int	cx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cx);
		int	cy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cy);
		int	xmin = 0, ymin = 0;
		int	xmax = cx - 20, ymax = cy - 20;
		int xDist = _round(THE_BASE(m_pBase)->fZoom * THE_BASE(m_pBase)->Grid.xDist);
		int yDist = _round(THE_BASE(m_pBase)->fZoom * THE_BASE(m_pBase)->Grid.yDist);
		COLORREF cl = THE_BASE(m_pBase)->Grid.color;
		TColor	 tcl(GetRValue(cl), GetGValue(cl), GetBValue(cl), 255);

		int x = xmin, y = ymin;
		ScreenToDataSpace(x, y);
		x = x / THE_BASE(m_pBase)->Grid.xDist * THE_BASE(m_pBase)->Grid.xDist;
		y = y / THE_BASE(m_pBase)->Grid.yDist * THE_BASE(m_pBase)->Grid.yDist;
		DataSpaceToScreen(x, y);

		// 水平线
		for (; y < ymax + yDist; y += yDist)
		{
			THE_BASE(m_pBase)->GDevice.HorzLine(y, xmin, xmax, tcl);
		}

		// 垂直线
		for (; x < xmax + xDist; x += xDist)
		{
			THE_BASE(m_pBase)->GDevice.VertLine(x, ymin, ymax, tcl);
		}
	}
}

// 绘制影片边框
void TShapeEditor::DrawMovieFrame()
{
	if (!GetCurLayer().validate())
		return;

	int			cx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cx);
	int			cy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cy);
	COLORREF	bk_cl = THE_BASE(m_pBase)->clMovieBackground;
	
	int			xmin = 0;
	int			xmax = cx;//cx - 20;
	int			ymin = 0;
	int			ymax = cy;//cy - 20;	

	DataSpaceToScreen(xmin, ymin);
	DataSpaceToScreen(xmax, ymax);
	
	THE_BASE(m_pBase)->GDevice.FillRect(xmin, ymin, xmax, ymax, 
			TColor(GetRValue(bk_cl), GetGValue(bk_cl), GetBValue(bk_cl), 255));
	if (ShowCanvasBorder())
		THE_BASE(m_pBase)->GDevice.Rectangle(xmin, ymin, xmax, ymax, TColor(0, 0, 0, 255));	
}

bool TShapeEditor::ShowCanvasBorder() const
{
	return THE_BASE(m_pBase)->bShowCanvasBorder;
}

void TShapeEditor::ShowCanvasBorder(bool bShow)
{
	if (THE_BASE(m_pBase)->bShowCanvasBorder != bShow)
	{
		THE_BASE(m_pBase)->bShowCanvasBorder = bShow;

		Redraw();
		Repaint();		
	}
}

void TShapeEditor::RefreshDesignWnd(const gld_rect &_r)
{
	RECT _t;

	_t.left = _r.left;
	_t.right = _r.right;
	_t.top = _r.top;
	_t.bottom = _r.bottom;

	::InvalidateRect(THE_BASE(m_pBase)->hWnd, &_t, FALSE);	
	::UpdateWindow(THE_BASE(m_pBase)->hWnd);
}

void TShapeEditor::RefreshDesignWnd()
{
	::InvalidateRect(THE_BASE(m_pBase)->hWnd, 0, FALSE);
	::UpdateWindow(THE_BASE(m_pBase)->hWnd);
}

void TShapeEditor::ScreenToDataSpace(int &_x, int &_y)
{
	_x = _round((_x - THE_BASE(m_pBase)->fOffsetX) / THE_BASE(m_pBase)->fZoom);
	_y = _round((_y - THE_BASE(m_pBase)->fOffsetY) / THE_BASE(m_pBase)->fZoom);	
}

void TShapeEditor::DataSpaceToScreen(int &_x, int &_y)
{
	_x = _round(THE_BASE(m_pBase)->fZoom * _x + THE_BASE(m_pBase)->fOffsetX);
	_y = _round(THE_BASE(m_pBase)->fZoom * _y + THE_BASE(m_pBase)->fOffsetY);
}

TMatrix TShapeEditor::ScreenToDataSpace()
{
	float	scale = (float)(1.0 / THE_BASE(m_pBase)->fZoom);
	int		offsetx = -_round(THE_BASE(m_pBase)->fOffsetX / THE_BASE(m_pBase)->fZoom);
	int		offsety = -_round(THE_BASE(m_pBase)->fOffsetY / THE_BASE(m_pBase)->fZoom);

	return TMatrix(scale, 0.0f, 0.0f, scale, offsetx, offsety);
}

TMatrix TShapeEditor::DataSpaceToScreen()
{
	return TMatrix((float)THE_BASE(m_pBase)->fZoom, 0.0f, 
					0.0f, (float)THE_BASE(m_pBase)->fZoom, 
					_round(THE_BASE(m_pBase)->fOffsetX), _round(THE_BASE(m_pBase)->fOffsetY));
}

// 水平滚动例程
void TShapeEditor::HorzScroll(int dx)
{
	HorzScrollBase(dx);
	RefreshHScrollBar(true);
	Repaint();
}

// 垂直滚动例程
void TShapeEditor::VertScroll(int dy)
{	
	VertScrollBase(dy);
	RefreshVScrollBar(true);
	Repaint();
}

void TShapeEditor::VertScrollBase(int dy)
{	
	double	ymin, ymax;
	double fOffsetY = THE_BASE(m_pBase)->fOffsetY - dy * 20.0;	
	GetVertOffsetRange(ymin, ymax);	
	if (fOffsetY < ymin)
	{
		fOffsetY = ymin;
	}
	else if (fOffsetY > ymax)
	{
		fOffsetY = ymax;
	}	
	dy = _round((THE_BASE(m_pBase)->fOffsetY - fOffsetY) / 20.0);	
	if (dy == 0)
	{
		return;
	}	
	// 计算失效区域
	gld_rect iv_rc;
	int wnd_width, wnd_height;
	THE_BASE(m_pBase)->GDevice.GetSize(wnd_width, wnd_height);
	if (dy > 0) {
		iv_rc.left = 0;
		iv_rc.right = wnd_width - 1;
		iv_rc.top = wnd_height - dy;
		iv_rc.bottom = wnd_height - 1;
	}
	else {
		iv_rc.left = 0;
		iv_rc.right = wnd_width - 1;
		iv_rc.top = 0;	
		iv_rc.bottom = -dy - 1;
	}

	// 更新映射矩阵	
	THE_BASE(m_pBase)->fOffsetY = fOffsetY;
	// 状态同步
	ChangeMapMode();

	// 滚动位图并清除失效区域
	THE_BASE(m_pBase)->GDevice.VScrollBitmap(dy);

	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		THE_BASE(m_pBase)->GDevice.ClearRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom);	
		iv_rc.left = PIXEL_TO_TWIPS(iv_rc.left);
		iv_rc.top = PIXEL_TO_TWIPS(iv_rc.top);
		iv_rc.right = PIXEL_TO_TWIPS(iv_rc.right);
		iv_rc.bottom = PIXEL_TO_TWIPS(iv_rc.bottom);	
		GGRECT clipOld = THE_BASE(m_pBase)->GDevice.GetClipRect();
		THE_BASE(m_pBase)->GDevice.SetClipRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom);
		DrawMovieFrame();
		THE_BASE(m_pBase)->GDevice.SetClipRect(clipOld);
	}
	else
	{
		iv_rc.left = PIXEL_TO_TWIPS(iv_rc.left);
		iv_rc.top = PIXEL_TO_TWIPS(iv_rc.top);
		iv_rc.right = PIXEL_TO_TWIPS(iv_rc.right);
		iv_rc.bottom = PIXEL_TO_TWIPS(iv_rc.bottom);
		COLORREF bk_cl = THE_BASE(m_pBase)->clMovieBackground;
		THE_BASE(m_pBase)->GDevice.FillRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom, TColor(GetRValue(bk_cl), GetGValue(bk_cl), GetBValue(bk_cl), 255));	
	}

	RedrawObject(iv_rc);	
}

void TShapeEditor::HorzScrollBase(int dx)
{		
	// 滚动范围的边界检测
	double	xmin, xmax;
	double fOffsetX = THE_BASE(m_pBase)->fOffsetX - dx * 20.0;	
	GetHorzOffsetRange(xmin, xmax);	
	if (fOffsetX < xmin)
	{
		fOffsetX = xmin;
	}
	else if (fOffsetX > xmax)
	{
		fOffsetX = xmax;
	}	
	dx = _round((THE_BASE(m_pBase)->fOffsetX - fOffsetX) / 20.0);
	if (dx == 0)
	{
		return;
	}
	// 计算失效区域	
	gld_rect iv_rc;
	int wnd_width, wnd_height;
	THE_BASE(m_pBase)->GDevice.GetSize(wnd_width, wnd_height);
	if (dx > 0) {
		iv_rc.left = wnd_width - dx - 1;
		iv_rc.right = wnd_width - 1;
		iv_rc.top = 0;
		iv_rc.bottom = wnd_height - 1;
	}
	else {
		iv_rc.left = 0;
		iv_rc.right = -dx;
		iv_rc.top = 0;		
		iv_rc.bottom = wnd_height - 1;
	}

	// 更新映射矩阵	
	THE_BASE(m_pBase)->fOffsetX = fOffsetX;
	// 状态同步
	ChangeMapMode();

	// 滚动位图并清除失效区域
	THE_BASE(m_pBase)->GDevice.HScrollBitmap(dx);

	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		THE_BASE(m_pBase)->GDevice.ClearRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom);	
		iv_rc.left = PIXEL_TO_TWIPS(iv_rc.left);
		iv_rc.top = PIXEL_TO_TWIPS(iv_rc.top);
		iv_rc.right = PIXEL_TO_TWIPS(iv_rc.right);
		iv_rc.bottom = PIXEL_TO_TWIPS(iv_rc.bottom);	
		GGRECT clipOld = THE_BASE(m_pBase)->GDevice.GetClipRect();
		THE_BASE(m_pBase)->GDevice.SetClipRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom);
		DrawMovieFrame();
		THE_BASE(m_pBase)->GDevice.SetClipRect(clipOld);
	}
	else
	{
		iv_rc.left = PIXEL_TO_TWIPS(iv_rc.left);
		iv_rc.top = PIXEL_TO_TWIPS(iv_rc.top);
		iv_rc.right = PIXEL_TO_TWIPS(iv_rc.right);
		iv_rc.bottom = PIXEL_TO_TWIPS(iv_rc.bottom);
		COLORREF bk_cl = THE_BASE(m_pBase)->clMovieBackground;
		THE_BASE(m_pBase)->GDevice.FillRect(iv_rc.left, iv_rc.top, iv_rc.right, iv_rc.bottom, TColor(GetRValue(bk_cl), GetGValue(bk_cl), GetBValue(bk_cl), 255));	
	}

	RedrawObject(iv_rc);
}

void TShapeEditor::Scroll(int dx, int dy)
{
	if (dx != 0)
	{
		HorzScrollBase(dx);
	}

	if (dy != 0)
	{
		VertScrollBase(dy);
	}
	RefreshScrollBar();
	Repaint();
}

// 使影片居中显示
void TShapeEditor::CenterMovie()
{
	double	gcx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cx - 1) / 2.0;
	double	gcy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cy - 1) / 2.0;
	if (GetEditMode() == EDIT_SCENE_MODE)
	{		
		double	mcx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cx - 1) / 2.0;
		double	mcy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cy - 1) / 2.0;

		THE_BASE(m_pBase)->fOffsetX = gcx - mcx * THE_BASE(m_pBase)->fZoom;
		THE_BASE(m_pBase)->fOffsetY = gcy - mcy * THE_BASE(m_pBase)->fZoom;
	}
	else
	{
		THE_BASE(m_pBase)->fOffsetX = gcx;
		THE_BASE(m_pBase)->fOffsetY = gcy;
	}

	NormalizeOffset();
	ChangeMapMode();	
	Redraw();
	Repaint();
	RefreshScrollBar();	
}

// 重绘落入失效矩形中的Shape
void TShapeEditor::RedrawObject(gld_rect &rcClip)
{
	GGRECT	rcOld = THE_BASE(m_pBase)->GDevice.GetClipRect();
	THE_BASE(m_pBase)->GDevice.SetClipRect(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);

	// Draw grid line
	if (IsShowGrid())
	{
		DrawGrid();
	}

	gld_rect rcBound = rcClip;
	ScreenToDataSpace(rcBound.left, rcBound.top);
	ScreenToDataSpace(rcBound.right, rcBound.bottom);
	rcBound.normalize();
	rcBound.inflate(-21, -21, 21, 21);

	THE_BASE(m_pBase)->GDevice.SetLineWidthScaleFactor(GetZoomPercent());
	THE_BASE(m_pBase)->GDevice.SetMatrix(DataSpaceToScreen());
	gld_layer_iter iLayer = THE_BASE(m_pBase)->CurFrame.begin_layer();
	//禎与层：桢 〉层
	for (; iLayer != THE_BASE(m_pBase)->CurFrame.end_layer(); ++iLayer)
	{
		gld_layer	Layer = *iLayer;
		if (Layer.validate() && ((Layer.attributes() & LA_SHOW) != 0))
		{
			gld_shape_iter iShape = Layer.begin_shape();
			for (; iShape != Layer.end_shape(); ++iShape) 
			{
				gld_shape	Shape = *iShape;				
				gld_rect	ShapeBox = _transform_rect(Shape.bound(), Shape.matrix());
				if (ShapeBox.intersect(rcBound))//只要有相交就重新绘画，没有剪切的概念
				{
					TDraw::draw(Shape, THE_BASE(m_pBase)->GDevice);
				}
			}
		}
	}

	TNotifyTransceiver::Notify(this, SEN_REDRAW, (LPARAM)&rcBound);

	THE_BASE(m_pBase)->GDevice.SetClipRect(rcOld);
}

//-------------------------------------------------------------------------------------------------------------------------
//
//						缩放函数
//
//-------------------------------------------------------------------------------------------------------------------------
// 以(x, y)点为中心缩放视图
void TShapeEditor::ZoomTo(int percent, int x, int y)
{	
	percent = NormalizeZoomPercent(percent);
	if (percent == GetZoomPercent())
	{
		return;
	}
	double	fZoom = percent / 100.0;
	// 调整工作区大小	
	THE_BASE(m_pBase)->WorkSpaceSize.cx = max(THE_BASE(m_pBase)->ViewSize.cx, _round(S_WorkSpaceWidth * fZoom));
	THE_BASE(m_pBase)->WorkSpaceSize.cy = max(THE_BASE(m_pBase)->ViewSize.cy, _round(S_WorkSpaceHeight * fZoom));
	// 滚动范围的边界检测
	double	xmin, xmax, ymin, ymax;
	double fOffsetX = THE_BASE(m_pBase)->fOffsetX + (THE_BASE(m_pBase)->fZoom - fZoom) * x;
	double fOffsetY = THE_BASE(m_pBase)->fOffsetY + (THE_BASE(m_pBase)->fZoom - fZoom) * y;	

	THE_BASE(m_pBase)->fZoom = fZoom;	// GetHorzOffsetRange() function used THE_BASE(m_pBase)->fZoom, so must do this !
	GetHorzOffsetRange(xmin, xmax);
	GetVertOffsetRange(ymin, ymax);
	if (fOffsetX < xmin)
	{
		fOffsetX = xmin;
	}
	else if (fOffsetX > xmax)
	{
		fOffsetX = xmax;
	}
	
	if (fOffsetY < ymin)
	{
		fOffsetY = ymin;
	}
	else if (fOffsetY > ymax)
	{
		fOffsetY = ymax;
	}
	// 调整缩放矩阵		
	THE_BASE(m_pBase)->fOffsetX = fOffsetX;
	THE_BASE(m_pBase)->fOffsetY = fOffsetY;
	// 状态同步
	ChangeMapMode();
	RefreshScrollBar();
	Redraw();
	Repaint();
	CSubjectManager::Instance()->GetSubject("Zoom")->Notify(0);
}

void TShapeEditor::ZoomIn(int x1, int y1, int x2, int y2)
{
	// 缩小视图到矩形
	// 确定缩放比例
	double fZoomX = abs(x2 - x1) / ((THE_BASE(m_pBase)->ViewSize.cx - 1) * 20.0);
	double fZoomY = abs(y2 - y1) / ((THE_BASE(m_pBase)->ViewSize.cy - 1) * 20.0);
	double fZoom = max(fZoomX, fZoomY);
	fZoom = NormalizeZoomPercent(_round(fZoom * 100.0)) / 100.0;
	// 调整工作区大小	
	THE_BASE(m_pBase)->WorkSpaceSize.cx = max(THE_BASE(m_pBase)->ViewSize.cx, _round(S_WorkSpaceWidth * fZoom));
	THE_BASE(m_pBase)->WorkSpaceSize.cy = max(THE_BASE(m_pBase)->ViewSize.cy, _round(S_WorkSpaceHeight * fZoom));
	// 确定偏移
	int nMapToX = (x2 + x1) / 2;
	int nMapToY = (y2 + y1) / 2;
	int nMapX = THE_BASE(m_pBase)->ViewSize.cx * 10;
	int nMapY = THE_BASE(m_pBase)->ViewSize.cy * 10;		
	double fOffsetX = nMapToX * fZoom - nMapX;
	double fOffsetY = nMapToY * fZoom - nMapY;
	THE_BASE(m_pBase)->fZoom = fZoom;
	double xmin, xmax, ymin, ymax;
	GetHorzOffsetRange(xmin, xmax);
	GetVertOffsetRange(ymin, ymax);
	if (fOffsetX < xmin)
	{
		fOffsetX = xmin;
	}
	else if (fOffsetX > xmax)
	{
		fOffsetX = xmax;
	}
	if (fOffsetY < ymin)
	{
		fOffsetY = ymin;
	}
	else if (fOffsetY > ymax)
	{
		fOffsetY = ymax;
	}
	// 状态同步
	THE_BASE(m_pBase)->fOffsetX = fOffsetX;
	THE_BASE(m_pBase)->fOffsetY = fOffsetY;
	ChangeMapMode();
	RefreshScrollBar();
	Redraw();
	Repaint();
	CSubjectManager::Instance()->GetSubject("Zoom")->Notify(0);
}

void TShapeEditor::ZoomOut(int x1, int y1, int x2, int y2)
{
	// 放大视图到窗口
	// 确定缩放比例
	double fZoomX = ((THE_BASE(m_pBase)->ViewSize.cx - 1) * 20.0) / (x2 - x1);
	double fZoomY = ((THE_BASE(m_pBase)->ViewSize.cy - 1) * 20.0) / (y2 - y1);
	double fZoom = min(fZoomX, fZoomY);
	fZoom = NormalizeZoomPercent(_round(fZoom * 100.0)) / 100.0;
	// 调整工作区大小	
	THE_BASE(m_pBase)->WorkSpaceSize.cx = max(THE_BASE(m_pBase)->ViewSize.cx, _round(S_WorkSpaceWidth * fZoom));
	THE_BASE(m_pBase)->WorkSpaceSize.cy = max(THE_BASE(m_pBase)->ViewSize.cy, _round(S_WorkSpaceHeight * fZoom));
	// 确定偏移
	int nMapToX = THE_BASE(m_pBase)->ViewSize.cx * 10;
	int nMapToY = THE_BASE(m_pBase)->ViewSize.cy * 10;
	int nMapX = (x2 + x1) / 2;
	int nMapY = (y2 + y1) / 2;
	double fOffsetX = nMapToX - nMapX * fZoom;
	double fOffsetY = nMapToY - nMapY * fZoom;
	THE_BASE(m_pBase)->fZoom = fZoom;
	double xmin, xmax, ymin, ymax;
	GetHorzOffsetRange(xmin, xmax);
	GetVertOffsetRange(ymin, ymax);
	if (fOffsetX < xmin)
	{
		fOffsetX = xmin;
	}
	else if (fOffsetX > xmax)
	{
		fOffsetX = xmax;
	}
	if (fOffsetY < ymin)
	{
		fOffsetY = ymin;
	}
	else if (fOffsetY > ymax)
	{
		fOffsetY = ymax;
	}
	// 状态同步
	THE_BASE(m_pBase)->fOffsetX = fOffsetX;
	THE_BASE(m_pBase)->fOffsetY = fOffsetY;
	ChangeMapMode();
	RefreshScrollBar();
	Redraw();
	Repaint();
	CSubjectManager::Instance()->GetSubject("Zoom")->Notify(0);
}

void TShapeEditor::ZoomIn(int x, int y)
{
	ZoomTo(GetZoomPercent() / 2, x, y);	
}

void TShapeEditor::ZoomOut(int x, int y)
{
	ZoomTo(GetZoomPercent() * 2, x, y);	
}

void TShapeEditor::ZoomTo(int percent)
{
	int nMapX = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cx - 1) / 2;
	int nMapY = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cy - 1) / 2;
	
	ScreenToDataSpace(nMapX, nMapY);

	ZoomTo(percent, nMapX, nMapY);
}

void TShapeEditor::ShowAll()
{
	// 计算画布上所有shape的bounding box
	gld_shape_sel	sel;	
	gld_layer_iter	li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();
	for (; li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --li)
	{
		gld_layer layer = *li;
		if ((layer.attributes() & LA_SHOW) != 0)
		{
			gld_shape_iter	si = layer.rbegin_shape();
			for (; si != layer.rend_shape(); --si) 
				sel.select(*si);			
		}
	}
	gld_rect bound = sel.SurroundBox();
	// 留两个像素的边界
	bound.inflate(PIXEL_TO_TWIPS(2), PIXEL_TO_TWIPS(2));
	// 以屏幕为中心进行缩放
	ZoomOut(bound.left, bound.top, bound.right, bound.bottom);	
}

void TShapeEditor::ShowFrame()
{
	// 取影片大小
	CSize szMovie = GetMovieSize();
	// 留两个像素的边界
	szMovie.cx += 2;
	szMovie.cy += 2;
	ZoomOut(-40, -40, PIXEL_TO_TWIPS(szMovie.cx), PIXEL_TO_TWIPS(szMovie.cy));
}

//-------------------------------------------------------------------------------------------------------------------------

// 设置工作区域大小
void TShapeEditor::SetWorkSpaceSize(const CSize &_size)
{
	THE_BASE(m_pBase)->WorkSpaceSize = _size;	
}

CSize TShapeEditor::GetWorkSpaceSize()
{
	return THE_BASE(m_pBase)->WorkSpaceSize;
}

void TShapeEditor::ChangeMapMode()
{
	TMatrix	mat = ScreenToDataSpace();

	for (int _i = 0; _i < THE_BASE(m_pBase)->Tools.Count(); _i++)
	{
		THE_BASE(m_pBase)->Tools[_i]->Matrix(mat);
	}

	CSubjectManager::Instance()->GetSubject("ChangeMapMode")->Notify(NULL);
}

void TShapeEditor::LoadRes()
{
	HINSTANCE hInst = ::LoadLibrary("ShapeEditorRes.dll");
	if (hInst == 0) {
		for (UINT i = 0; i < IDC_G_LAST - IDC_G_FIRST + 1; i++)
			THE_BASE(m_pBase)->hStockCursors[i] = 0;
		return;
	}

	for (UINT i = IDC_G_FIRST; i < IDC_G_LAST + 1; i++)
		THE_BASE(m_pBase)->hStockCursors[i - IDC_G_FIRST] = ::LoadCursor(hInst, MAKEINTRESOURCE(i));
}

void TShapeEditor::Display(HDC hdc, int x, int y)
{
	THE_BASE(m_pBase)->GDevice.Display(hdc, x, y);
}

void TShapeEditor::SetViewSize(int cx, int cy)
{
	cx = __max(1, cx);
	cy = __max(1, cy);

	if (THE_BASE(m_pBase)->ViewSize.cx != cx || THE_BASE(m_pBase)->ViewSize.cy != cy)
	{
		THE_BASE(m_pBase)->ViewSize.cx = cx;
		THE_BASE(m_pBase)->ViewSize.cy = cy;		
		
		// 调整工作区大小，如果(视图大小 > 工作区大小)计算将出错		
		THE_BASE(m_pBase)->WorkSpaceSize.cx = max(THE_BASE(m_pBase)->ViewSize.cx, _round(S_WorkSpaceWidth * THE_BASE(m_pBase)->fZoom));
		THE_BASE(m_pBase)->WorkSpaceSize.cy = max(THE_BASE(m_pBase)->ViewSize.cy, _round(S_WorkSpaceHeight * THE_BASE(m_pBase)->fZoom));

		NormalizeOffset();
		ChangeMapMode();	

		RebuildBitmap();

		RefreshScrollBar();
	}
}

CSize TShapeEditor::GetViewSize()
{
	return THE_BASE(m_pBase)->ViewSize;
}

void TShapeEditor::RebuildBitmap()
{
	int cx = THE_BASE(m_pBase)->ViewSize.cx;
	int cy = THE_BASE(m_pBase)->ViewSize.cy;

	THE_BASE(m_pBase)->GDevice.SetSize(cx, cy);

	Redraw();
}

void TShapeEditor::CreateGDIObject()
{
	// initialize gdi object arrays
	for (int PenIndex = 0; PenIndex < SEP_COUNT * 2; PenIndex++)	
		THE_BASE(m_pBase)->hStockPens[PenIndex] = NULL;
	for (int BrushIndex = 0; BrushIndex < SEB_COUNT * 2; BrushIndex++)	
		THE_BASE(m_pBase)->hStockBrushes[BrushIndex] = NULL;

	// create stock pens
	LOGPEN ActLogPen, UnactLogPen;

	ActLogPen.lopnWidth.x = 1;
	ActLogPen.lopnStyle = PS_SOLID;
	ActLogPen.lopnColor = RGB(0, 128, 255);
	UnactLogPen.lopnWidth.x = 1;
	UnactLogPen.lopnStyle = PS_DOT;
	UnactLogPen.lopnColor = RGB(200, 200, 200);
	VERIFY(SetStockPen(SEP_OUTLINE, &ActLogPen, &UnactLogPen));

	ActLogPen.lopnWidth.x = 1;
	ActLogPen.lopnStyle = PS_DOT;
	ActLogPen.lopnColor = RGB(0, 128, 255);
	UnactLogPen.lopnWidth.x = 1;
	UnactLogPen.lopnStyle = PS_DOT;
	UnactLogPen.lopnColor = RGB(128, 128, 128);
	VERIFY(SetStockPen(SEP_AUXILIARY, &ActLogPen, &UnactLogPen));

	ActLogPen.lopnWidth.x = 1;
	ActLogPen.lopnStyle = PS_SOLID;
	ActLogPen.lopnColor = RGB(0, 128, 255);
	UnactLogPen.lopnWidth.x = 1;
	UnactLogPen.lopnStyle = PS_SOLID;
	UnactLogPen.lopnColor = RGB(200, 200, 200);
	VERIFY(SetStockPen(SEP_HANDLER, &ActLogPen, &UnactLogPen));

	ActLogPen.lopnWidth.x = 1;
	ActLogPen.lopnStyle = PS_SOLID;
	ActLogPen.lopnColor = RGB(255, 128, 0);
	UnactLogPen.lopnWidth.x = 1;
	UnactLogPen.lopnStyle = PS_SOLID;
	UnactLogPen.lopnColor = RGB(200, 200, 200);
	VERIFY(SetStockPen(SEP_ORIGIN, &ActLogPen, &UnactLogPen));

	ActLogPen.lopnWidth.x = 1;
	ActLogPen.lopnStyle = PS_DOT;
	ActLogPen.lopnColor = RGB(0, 128, 255);
	UnactLogPen.lopnWidth.x = 1;
	UnactLogPen.lopnStyle = PS_DOT;
	UnactLogPen.lopnColor = RGB(200, 200, 200);
	VERIFY(SetStockPen(SEP_GUIDLINE, &ActLogPen, &UnactLogPen));
	
	ActLogPen.lopnWidth.x = 1;
	ActLogPen.lopnStyle = PS_SOLID;
	ActLogPen.lopnColor = RGB(0, 255, 0);
	UnactLogPen.lopnWidth.x = 1;
	UnactLogPen.lopnStyle = PS_SOLID;
	UnactLogPen.lopnColor = RGB(0, 255, 0);
	VERIFY(SetStockPen(SEP_AUXLINE, &ActLogPen, &UnactLogPen));

	// create stock brushes
	LOGBRUSH ActLogBrush, UnactLogBrush;
	
	ActLogBrush.lbHatch = 0;
	ActLogBrush.lbStyle = BS_SOLID;
	ActLogBrush.lbColor = RGB(197, 214, 252);
	UnactLogBrush.lbHatch = 0;
	UnactLogBrush.lbStyle = BS_SOLID;
	UnactLogBrush.lbColor = RGB(255, 255, 255);
	VERIFY(SetStockBrush(SEB_SOLID_HANDLER, &ActLogBrush, &UnactLogBrush));	

	ActLogBrush.lbHatch = 0;
	ActLogBrush.lbStyle = BS_SOLID;
	ActLogBrush.lbColor = RGB(255, 255, 255);
	UnactLogBrush.lbHatch = 0;
	UnactLogBrush.lbStyle = BS_SOLID;
	UnactLogBrush.lbColor = RGB(255, 255, 255);
	VERIFY(SetStockBrush(SEB_HOLLOW_HANDLER, &ActLogBrush, &UnactLogBrush));

	ActLogBrush.lbHatch = 0;
	ActLogBrush.lbStyle = BS_SOLID;
	ActLogBrush.lbColor = RGB(252, 214, 197);
	UnactLogBrush.lbHatch = 0;
	UnactLogBrush.lbStyle = BS_SOLID;
	UnactLogBrush.lbColor = RGB(255, 255, 255);
	VERIFY(SetStockBrush(SEB_ORIGIN, &ActLogBrush, &UnactLogBrush));
}

void TShapeEditor::DestroyGDIObject()
{
	for (int PenIndex = 0; PenIndex < SEP_COUNT * 2; PenIndex++)	
		VERIFY(DeleteObject(THE_BASE(m_pBase)->hStockPens[PenIndex]));

	for (int BrushIndex = 0; BrushIndex < SEB_COUNT * 2; BrushIndex++)	
		VERIFY(DeleteObject(THE_BASE(m_pBase)->hStockBrushes[BrushIndex]));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin new interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
gld_shape TShapeEditor::PtOnShape(int x, int y)
{
	gld_pair<gld_shape, bool> _ret = CurFrame().pt_on_shape(x, y, ScreenToDataSpace());
	if (_ret.second)
	{
		gld_shape shape = _ret.first;
		if ((shape.capabilities() & SC_ENABLETRANSFORM) == SC_ENABLETRANSFORM)
			return _ret.first;		
	}

	if (EffectInplaceEditor::Instance()->PtOnPath(CPoint(x, y)))
	{
		ASSERT(!CurSel().empty());
		return *CurSel().begin();
	}

	return gld_shape();
}

void TShapeEditor::SetCurLayer(const gld_layer &layer)
{
	if (THE_BASE(m_pBase)->CurFrame.cur_layer() != layer)
	{
		THE_BASE(m_pBase)->CurFrame.cur_layer(layer);		
		SelectGuidLine(NULL);
		Repaint();
	}
}

gld_layer TShapeEditor::GetCurLayer()
{	
	if (THE_BASE(m_pBase)->CurFrame.begin_layer() == THE_BASE(m_pBase)->CurFrame.end_layer())
		return gld_layer();
	else
		return THE_BASE(m_pBase)->CurFrame.cur_layer();
}

void TShapeEditor::SetCurTool(int nIndex)
{
	ASSERT((nIndex > -1) && (nIndex < THE_BASE(m_pBase)->Tools.Count()));

	int nCurrent = THE_BASE(m_pBase)->Tools.Current();
	if (nCurrent != nIndex)
	{
		TTool *pOldTool = CurTool();		
		if (pOldTool)
		{
			pOldTool->Leave();
		}
		THE_BASE(m_pBase)->Tools.Current(nIndex);
		CurTool()->Enter();
		TNotifyTransceiver::Notify(this, SEN_TOOL_CHANGED, nIndex);
	}
}

int TShapeEditor::GetCurTool()
{
	return THE_BASE(m_pBase)->Tools.Current();
}

void TShapeEditor::SetCurMatrix(float scalex, float scaley, float skewx, float skewy)
{
	//if (fabsf(scalex) < 0.01f) scalex = 0.01f;
	//if (fabsf(scaley) < 0.01f) scaley = 0.01f;
	//
	//if (!THE_BASE(m_pBase)->sel.empty())
	//{
	//	if (THE_BASE(m_pBase)->sel.count() > 1)
	//	{	
	//		gld_rect	box = THE_BASE(m_pBase)->sel.SurroundBox();
	//		int			cx = (box.left + box.right) >> 1;
	//		int			cy = (box.top + box.bottom) >> 1;

	//		TMatrix		mat;
	//		mat.SetTransformParam(scalex, scaley, skewx, skewy, 0, 0);
	//		mat.SetOffsetWithOrigin(cx, cy);

	//		gld_list<TMatrix>	mats;
	//		gld_shape_sel::iterator ishape = THE_BASE(m_pBase)->sel.begin();
	//		for (; ishape != THE_BASE(m_pBase)->sel.end(); ++ishape)
	//		{
	//			mats.push_back((*ishape).matrix() * mat);
	//		}

	//		CCmdTransformShape *pCmd = new CCmdTransformShape(THE_BASE(m_pBase)->sel, mats);
	//		THE_BASE(m_pBase)->Commands.Do(pCmd);			
	//	}
	//	else
	//	{
	//		gld_shape	shape = *THE_BASE(m_pBase)->sel.begin();
	//		TMatrix		mat = shape.matrix();			
	//		gld_point   op = shape.trans_origin();
	//		int			ox = op.x;
	//		int			oy = op.y;
	//		mat.Transform(ox, oy);

	//		mat.SetTransformParam(scalex, scaley, skewx, skewy, 0, 0);			
	//		mat.Transform(op.x, op.y);
	//		mat.m_dx = ox - op.x;
	//		mat.m_dy = oy - op.y;

	//		gld_list<TMatrix>	mats;
	//		gld_shape_sel::iterator ishape = THE_BASE(m_pBase)->sel.begin();
	//		for (; ishape != THE_BASE(m_pBase)->sel.end(); ++ishape)
	//		{
	//			mats.push_back(mat);
	//		}
	//		
	//		CCmdTransformShape *pCmd = new CCmdTransformShape(THE_BASE(m_pBase)->sel, mats);
	//		THE_BASE(m_pBase)->Commands.Do(pCmd);			
	//	}
	//}
}

void TShapeEditor::GetCurMatrix(float &scalex, float &scaley, float &skewx, float &skewy)
{
	//if (!THE_BASE(m_pBase)->sel.empty())
	//{
	//	if (THE_BASE(m_pBase)->sel.count() > 1)
	//	{
	//		scalex = 1.0f;
	//		scaley = 1.0f;
	//		skewx = 0.0f;
	//		skewy = 0.0f;
	//	}
	//	else
	//	{
	//		gld_shape shape = *(THE_BASE(m_pBase)->sel.begin());

	//		TMatrix	mat = shape.matrix();

	//		mat.GetTransformParam(scalex, scaley, skewx, skewy);
	//	}
	//}
}

void TShapeEditor::SetCurDimension(int x, int y, int w, int h)
{
	//if (!THE_BASE(m_pBase)->sel.empty())
	//{	
	//	LimitValue(w, IDI_MIN_SHAPE_WIDTH, IDI_MAX_SHAPE_WIDTH);
	//	LimitValue(h, IDI_MIN_SHAPE_HEIGHT, IDI_MAX_SHAPE_HEIGHT);
	//	
	//	gld_rect	box = THE_BASE(m_pBase)->sel.SurroundBox();

	//	int ox, oy;

	//	if (THE_BASE(m_pBase)->sel.count() > 1)
	//	{
	//		ox = box.left;
	//		oy = box.top;
	//	}
	//	else
	//	{
	//		gld_shape shape = *(THE_BASE(m_pBase)->sel.begin());
	//		gld_point op = shape.trans_origin();
	//		ox = op.x;
	//		oy = op.y;
	//		TMatrix   smat = shape.matrix();
	//		smat.Transform(ox, oy);
	//	}

	//	int ow = box.width();
	//	int oh = box.height();

	//	TMatrix	scalemat, movemat;
	//	scalemat.SetScaleMatrix((float)w / ow, (float)h / oh, ox, oy);
	//	movemat.SetMoveMatrix(x - ox, y - oy);
	//	TMatrix	mat = scalemat * movemat;

	//	gld_list<TMatrix>	mats;
	//	gld_shape_sel::iterator ishape = THE_BASE(m_pBase)->sel.begin();
	//	for (; ishape != THE_BASE(m_pBase)->sel.end(); ++ishape)
	//	{
	//		mats.push_back((*ishape).matrix() * mat);
	//	}

	//	CCmdTransformShape *pCmd = new CCmdTransformShape(THE_BASE(m_pBase)->sel, mats);
	//	THE_BASE(m_pBase)->Commands.Do(pCmd);		
	//}
}

void TShapeEditor::GetCurDimension(int &x, int &y, int &w, int &h)
{
	//if (!THE_BASE(m_pBase)->sel.empty())
	//{
	//	gld_rect	box = THE_BASE(m_pBase)->sel.SurroundBox();

	//	if (THE_BASE(m_pBase)->sel.count() > 1)
	//	{
	//		x = box.left;
	//		y = box.top;
	//	}
	//	else
	//	{
	//		gld_shape shape = *(THE_BASE(m_pBase)->sel.begin());
	//		gld_point op = shape.trans_origin();
	//		x = op.x;
	//		y = op.y;
	//		TMatrix   smat = shape.matrix();
	//		smat.Transform(x, y);
	//	}

	//	w = box.width();
	//	h = box.height();
	//}
}

void TShapeEditor::SetDesignWnd(HWND hWnd)
{
	ASSERT(hWnd);

	for (int i = 0; i < THE_BASE(m_pBase)->Tools.Count(); i++)
	{
		THE_BASE(m_pBase)->Tools[i]->Hwnd(hWnd);
	}

	THE_BASE(m_pBase)->hWnd = hWnd;
}

HWND TShapeEditor::GetDesignWnd()
{
	return THE_BASE(m_pBase)->hWnd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// End new interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// New interface	2003.5.12 Append by yhhv

// Tools interface

// Query state
bool TShapeEditor::CanAddShape()
{	
	return CurLayerIsNormal();
}

bool TShapeEditor::CanTransform()
{
	return true;
}

bool TShapeEditor::CanReshape()
{
	return true;
}

bool TShapeEditor::CanChangeLineStyle()
{
	return true;
}

bool TShapeEditor::CanChangeLineStyle(int x, int y)
{
	// if point(x, y) in some edge then return true
	gld_shape		shape;
	gld_draw_obj	obj;
	gld_edge		edge;
	
	return InkBottleFetchHitEdge(x, y, shape, obj, edge);	
}

bool TShapeEditor::CanChangeFillStyle()
{
	return true;
}

bool TShapeEditor::CanAddGuidLine()
{
	return true;
}

bool TShapeEditor::CanChangeFillStyle(int x, int y)
{		
	gld_shape		Shape;
	gld_draw_obj	Obj;

	return PaintBucketFetchObj(x, y, Shape, Obj) && CCmdChangeFillStyleBase::CanChangeFill(Shape, THE_BASE(m_pBase)->pCurFillStyle);
}

bool TShapeEditor::CanTransformFill()
{
	return true;
}

bool TShapeEditor::CanTransformFill(int x, int y)
{
	// if point(x, y) in some fill style and fill style is linear gradient or radial gradient fill style then return true

	gld_layer		l;
	gld_shape		s;
	gld_draw_obj	o;

	int				index = PtOnFillStyle(x, y, l, s, o);

	if (index > 0)
	{
		TFillStyle	*pFillStyle = o.fill_styles().Get(index);

		return TSETrivial::FillStyleUseMatrix(pFillStyle);
	}

	return false;
}

bool TShapeEditor::ChangeFillStyleCanDrag(int x, int y)
{
	// if point(x, y) in some fill style and fill style is linear gradient or radial gradient fill style then return true
	gld_shape		shape;
	gld_draw_obj	obj;

	if (PaintBucketFetchObj(x, y, shape, obj) 
		&& CCmdChangeFillStyleBase::CanChangeFill(shape, THE_BASE(m_pBase)->pCurFillStyle))
	{		
		TFillStyle	*pFillStyle = THE_BASE(m_pBase)->pCurFillStyle;

		if (pFillStyle)
		{
			int			FillType = pFillStyle->GetType();
			
			return (FillType == TFillStyle::linear_gradient_fill) || (FillType == TFillStyle::radial_gradient_fill);
		}		
	}
	
	return false;
}

bool TShapeEditor::CanRepositionGuidPoint(tGuidLine *guidLine, tGuidPoint *guidPoint)
{
	return IsNormalLayer(GetCurLayer());
}

bool TShapeEditor::CanRelineGuidLine(tGuidLine *guidLine, tGuidPoint *what)
{
	return IsNormalLayer(GetCurLayer());
}

bool TShapeEditor::CanModifyGuidPoint()
{
	return IsNormalLayer(GetCurLayer());
}

// Add Shape
void TShapeEditor::AddLine(int x1, int y1, int x2, int y2)
{
	gld_shape line = TShapeBuilder::BuildLine(x1, y1, x2, y2, THE_BASE(m_pBase)->pCurLineStyle);
	
	AddShape(line);	// Select shape and transform tool
}

void TShapeEditor::AddRect(int x1, int y1, int x2, int y2)
{
	gld_shape rect = TShapeBuilder::BuildRect(x1, y1, x2, y2, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);

	AddShape(rect);	// Select shape and transform tool
}

void TShapeEditor::AddOval(int x1, int y1, int x2, int y2)
{
	gld_shape oval = TShapeBuilder::BuildOval(x1, y1, x2, y2, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);

	AddShape(oval);	// Select shape and transform tool
}

void TShapeEditor::AddPath(const _path &_p)
{
	gld_shape path = TShapeBuilder::BuildPath(_p, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);
	
	AddShape(path);
}

void TShapeEditor::AddPath(Point2 *p, int numOfPoints)
{
	gld_shape path = TShapeBuilder::BuildPath(p, numOfPoints, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);

	AddShape(path);
}

void TShapeEditor::AddPolygon(const POINT *v, int count)
{
	gld_shape polygon = TShapeBuilder::BuildPolygon(v, count, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);

	AddShape(polygon);
}

void TShapeEditor::AddRoundRect(const PiecewiseQuadraticBezier &_round)
{
	gld_shape roundrect = TShapeBuilder::BuildRoundRect(_round, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);

	AddShape(roundrect);
}

void TShapeEditor::AddStar(const POINT *v1, const POINT *v2, int count)
{
	gld_shape star = TShapeBuilder::BuildStar(v1, v2, count, THE_BASE(m_pBase)->pCurLineStyle, THE_BASE(m_pBase)->pCurFillStyle);

	AddShape(star);
}

// Edit
void TShapeEditor::MoveTransform(int dx, int dy, UINT nFlags)
{
	if ((nFlags & MK_CONTROL) == MK_CONTROL)
	{		
		HGLOBAL hMem = NULL;
		WriteInstancesToGlobal(THE_BASE(m_pBase)->sel, &hMem);
		if (hMem != NULL)
		{
			AddInstancesFromGlobal(hMem, dx, dy);
			GlobalFree(hMem);
		}
	}
	else
	{
		//CCmdMoveShape	*pCmd = new CCmdMoveShape(THE_BASE(m_pBase)->sel, dx, dy);
		CCmdMoveInstance *pCmd = new CCmdMoveInstance(THE_BASE(m_pBase)->sel, dx, dy);
		THE_BASE(m_pBase)->Commands.Do(pCmd, false);
	}
}

void TShapeEditor::ChangeLineStyle(int x, int y)
{
	gld_shape		Shape;
	gld_draw_obj	Obj;
	gld_edge		Edge;

	if (InkBottleFetchHitEdge(x, y, Shape, Obj, Edge))
	{
		gld_shape_sel	Sel;

		Sel.select(Shape);

		CCmdChangeLineStyle	*pCmd = new CCmdChangeLineStyle(Sel, THE_BASE(m_pBase)->pCurLineStyle);

		THE_BASE(m_pBase)->Commands.Do(pCmd, false);

		Repaint();
	}
}

void TShapeEditor::ChangeFillStyle(int x, int y)
{
	gld_shape		Shape;
	gld_draw_obj	Obj;

	if (PaintBucketFetchObj(x, y, Shape, Obj))
	{
		gld_shape_sel	Sel;
		Sel.select(Shape);

		if ((THE_BASE(m_pBase)->pCurFillStyle) && (THE_BASE(m_pBase)->pCurFillStyle->GetType() == TFillStyle::radial_gradient_fill))
		{
			gld_rect	bound = Shape.bound();
			float		scalex = (float)bound.width() / 0x8000;
			float		scaley = (float)bound.height() / 0x8000;			
			TMatrix		rmat = Shape.rmatrix();
			rmat.Transform(x, y);
			TMatrix		mat(scalex, 0.0f, 0.0f, scaley, x, y);

			TFillStyle	*clone = (TFillStyle *)(THE_BASE(m_pBase)->pCurFillStyle->Clone());
			clone->SetMatrix(mat);

			CCmdChangeFillStyle *pCmd = new CCmdChangeFillStyle(Sel, clone, false);
			THE_BASE(m_pBase)->Commands.Do(pCmd, false);			
		}
		else
		{
			CCmdChangeFillStyle *pCmd = new CCmdChangeFillStyle(Sel, THE_BASE(m_pBase)->pCurFillStyle, true);
			THE_BASE(m_pBase)->Commands.Do(pCmd, false);
		}
	}
}

void TShapeEditor::ChangeFillStyle(int sx, int sy, int ex, int ey)
{
	gld_shape		Shape;
	gld_draw_obj	Obj;

	if (PaintBucketFetchObj(sx, sy, Shape, Obj))
	{
		ASSERT(THE_BASE(m_pBase)->pCurFillStyle);
		int FillType = THE_BASE(m_pBase)->pCurFillStyle->GetType();		
		ASSERT(FillType == TFillStyle::linear_gradient_fill || FillType == TFillStyle::radial_gradient_fill);

		gld_shape_sel	Sel;
		Sel.select(Shape);

		TMatrix	rmat = Shape.rmatrix();
		rmat.Transform(sx, sy);
		rmat.Transform(ex, ey);

		int	dx = ex - sx;
		int dy = ey - sy;
		float r = sqrtf((float)dx * dx + (float)dy * dy);		

		if (r < 80.0) // 移动距离太小
		{
			if ((THE_BASE(m_pBase)->pCurFillStyle) 
				&& (THE_BASE(m_pBase)->pCurFillStyle->GetType() == TFillStyle::radial_gradient_fill))
			{
				gld_rect	bound = Shape.bound();
				float		scalex = (float)bound.width() / 0x8000;
				float		scaley = (float)bound.height() / 0x8000;
				TMatrix		mat(scalex, 0.0f, 0.0f, scaley, sx, sy);

				TFillStyle	*clone = (TFillStyle *)(THE_BASE(m_pBase)->pCurFillStyle->Clone());
				clone->SetMatrix(mat);

				CCmdChangeFillStyle *pCmd = new CCmdChangeFillStyle(Sel, clone, false);
				THE_BASE(m_pBase)->Commands.Do(pCmd, false);				
			}
			else
			{
				CCmdChangeFillStyle *pCmd = new CCmdChangeFillStyle(Sel, THE_BASE(m_pBase)->pCurFillStyle, true);
				THE_BASE(m_pBase)->Commands.Do(pCmd, false);
			}
		}
		else
		{
			TMatrix	mat;

			if (FillType == TFillStyle::linear_gradient_fill)
			{
				float		scale = r / 0x8000;
				float		sinx = dy / r;
				float		cosx = dx / r;
				int			ox = -0x4000;				
				float		e11 = scale * cosx;
				float		e12 = scale * sinx;
				float		e21 = - sinx;
				float		e22 = cosx;
				int			offsetx = (int)((1.0f - e11) * ox) + (sx + 0x4000);
				int			offsety = (int)(e12 * ox) + sy;

				mat.SetMatrix(e11, e12, e21, e22, offsetx, offsety);					
				
			}
			else
			{
				float		scale = r / 0x4000;
				mat.SetMatrix(scale, 0.0f, 0.0f, scale, sx, sy);			
			}

			TFillStyle	*clone = (TFillStyle *)(THE_BASE(m_pBase)->pCurFillStyle->Clone());			
			clone->SetMatrix(mat);
			
			CCmdChangeFillStyle *pCmd = new CCmdChangeFillStyle(Sel, clone, false);
			THE_BASE(m_pBase)->Commands.Do(pCmd, false);
		}
	}
}

void TShapeEditor::TransformBefore(gld_shape_sel &sel, TMatrix &mat)
{
	gld_list<TMatrix>	mats;

	for(gld_shape_sel::iterator i = sel.begin(); i != sel.end(); ++i)
	{
		mats.push_back(mat * (*i).matrix());
	}
	
	Transform(sel, mats);
}

void TShapeEditor::TransformAfter(gld_shape_sel &sel, TMatrix &mat)
{
	gld_list<TMatrix>	mats;

	for(gld_shape_sel::iterator i = sel.begin(); i != sel.end(); ++i)
	{
		mats.push_back((*i).matrix() * mat);
	}
	
	Transform(sel, mats);
}

bool TShapeEditor::Transform(const gld_shape_sel &sel, const gld_list<TMatrix> &mat)
{
	//CCmdTransformShape	*pCmd = new CCmdTransformShape(sel, mat);
	
	CCmdTransformInstance *pCmd = new CCmdTransformInstance(sel, mat);

	return THE_BASE(m_pBase)->Commands.Do(pCmd, false);
}

void TShapeEditor::TransformFill(TFillTransformData &Data, TMatrix &mat)
{
	CCmdTransformFillStyle	*pCmd = new CCmdTransformFillStyle(Data.shape, Data.fill_style, mat);
	THE_BASE(m_pBase)->Commands.Do(pCmd, false);
}

// sel
bool TShapeEditor::GetTransformFill(int x, int y, TFillTransformData &Data, TTool *pTool)
{
	pTool->Matrix().Transform(x, y);

	gld_layer_iter _layer_i = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

	for (; _layer_i != THE_BASE(m_pBase)->CurFrame.rend_layer(); --_layer_i)
	{
		gld_layer		_layer = *_layer_i;
		gld_shape_iter	_shape_i = _layer.rbegin_shape();

		if (IsNormalLayer(_layer))
		{
			for (; _shape_i != _layer.rend_shape(); --_shape_i) 
			{	
				gld_shape	_shape = *_shape_i;
				
				if (ToolMatchShape(pTool, _shape))
				{					
					gld_draw_obj	_draw_obj;
					int				_index = _shape.pt_on_cover(x, y, _draw_obj);

					if (_index > 0)
					{
						TFillStyle *_pFillStyle = _draw_obj.fill_styles().Get(_index);
						
						if (TSETrivial::FillStyleUseMatrix(_pFillStyle))
						{
							Data.shape = _shape;
							Data.draw_obj = _draw_obj;
							Data.fill_style = _pFillStyle;

							return true;						
						}
					}
				}				
			}
		}
	}

	return false;
}

/*
	从选择集中挑选适合工具的子集
*/
void TShapeEditor::FilterSelection(gld_shape_sel &sel, TTool *pTool)
{		
	if (!pTool)
	{
		return;
	}

	gld_shape_sel	tmp;
	for(gld_shape_sel::iterator iter = sel.begin(); iter != sel.end(); ++iter)
	{
		gld_shape	shape = *iter;

		if (ToolMatchShape(pTool, shape))
		{				
			tmp.select(shape);

			if (iter == sel.active())
			{
				tmp.active(tmp.rbegin());
			}
		}
	}

	sel = tmp;
}

void TShapeEditor::GetSelection(gld_shape_sel &sel, TTool *pTool)
{	
	sel = CurSel();

	FilterSelection(sel, pTool);
	
	if (sel != CurSel())
	{
		SetSelection(sel);
	}
}

void TShapeEditor::SetSelection(gld_shape_sel &sel)
{
	if (THE_BASE(m_pBase)->sel == sel)
	{
		return;
	}

	THE_BASE(m_pBase)->sel = sel;
	
	SetRepaint(false);
	if (!sel.empty() && GetGuidLine() != NULL)
		SelectGuidLine(NULL);
	CSubjectManager::Instance()->GetSubject("Select")->Notify(0);
	TNotifyTransceiver::Notify(this, SEN_SELECTION_CHANGED, 0);
	Repaint();
	SetRepaint(true);
}

void TShapeEditor::SelectShapeIfIntersect(gld_shape_sel &sel, const gld_rect &box, TTool *pTool)
{
	gld_layer_iter	li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

	for (; li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --li)
	{
		gld_layer		layer = *li;

		if (IsNormalLayer(layer))
		{
			gld_shape_iter	si = layer.rbegin_shape();
			for (; si != layer.rend_shape(); --si) 
			{
				gld_shape	shape = *si;
				if (ToolMatchShape(pTool, shape))
				{
					if (box.intersect(shape.effect_box())) 
					{
						sel.select(shape);
					}
				}
			}
		}
	}
}

void TShapeEditor::SelectShapeIfContain(gld_shape_sel &sel, const gld_rect &box, TTool *pTool)
{
	gld_layer_iter	li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

	for (; li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --li)
	{
		gld_layer		layer = *li;

		if (IsNormalLayer(layer))
		{
			gld_shape_iter	si = layer.rbegin_shape();
			for (; si != layer.rend_shape(); --si) 
			{
				gld_shape	shape = *si;
				if (ToolMatchShape(pTool, shape))
				{
					if (shape.effect_box().in_rect(box))
					{
						sel.select(shape);
					}
				}
			}
		}
	}
}

void TShapeEditor::NetSelect(int x1, int y1, int x2, int y2, TTool *pTool)
{
	if (THE_BASE(m_pBase)->CurFrame.validate())
	{
		gld_rect		_b(x1, y1, x2, y2);
		gld_shape_sel	_sel;		
		
		if (_b.right < _b.left || _b.bottom < _b.top) 
		{
			_b.normalize();

			SelectShapeIfIntersect(_sel, _b, pTool);
		}
		else 
		{
			SelectShapeIfContain(_sel, _b, pTool);
		}		

		SetSelection(_sel);
	}
}

void TShapeEditor::Select(int x, int y, gld_shape_sel &sel, TTool *pTool)
{
	ASSERT(pTool);
	
	sel.clear();

	if (THE_BASE(m_pBase)->CurFrame.validate())
	{
		gld_pair<gld_shape, bool> ret = THE_BASE(m_pBase)->CurFrame.pt_on_shape(x, y, pTool->Matrix());

		if (ret.second)
		{
			if (ToolMatchShape(pTool, ret.first))
			{
				sel.select(ret.first);
			}			
		}
	}

	SetSelection(sel);
}

void TShapeEditor::SelectAll()
{
	gld_shape_sel	sel;
	
	gld_layer_iter	li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();
	
	for (; li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --li)
	{
		gld_layer		layer = *li;

		if (IsNormalLayer(layer))
		{
			gld_shape_iter	si = layer.rbegin_shape();
			for (; si != layer.rend_shape(); --si) 
			{
				gld_shape	shape = *si;
				if (CurTool() == NULL || ToolMatchShape(CurTool(), shape))
				{
					sel.select(shape);					
				}
			}
		}
	}

	SetSelection(sel);
}

void TShapeEditor::SelectByLayer(gld_layer &layer)
{
	if (IsNormalLayer(layer))
	{
		gld_shape_sel	sel;
		
		for (gld_shape_iter	si = layer.rbegin_shape(); si != layer.rend_shape(); --si) 
		{
			gld_shape	shape = *si;
			if (CurTool() == NULL || ToolMatchShape(CurTool(), shape))
			{
				sel.select(shape);					
			}
		}
		
		SetSelection(sel);
	}
}

gld_shape TShapeEditor::PtOnShape(int x, int y, TTool *pTool)
{
	ASSERT(pTool);

	if (THE_BASE(m_pBase)->CurFrame.validate())
	{
		gld_pair<gld_shape, bool> ret = THE_BASE(m_pBase)->CurFrame.pt_on_shape(x, y, pTool->Matrix());

		if (ret.second)
		{
			if (ToolMatchShape(pTool, ret.first))
			{
				return ret.first;
			}
		}

		if (EffectInplaceEditor::Instance()->PtOnPath(CPoint(x, y)))
		{
			ASSERT(!CurSel().empty());
			return *CurSel().begin();
		}
	}

	return gld_shape();
}

gld_shape TShapeEditor::LookupShape(int x, int y)
{
	for (gld_layer_iter ilayer = CurFrame().rbegin_layer(); ilayer != CurFrame().rend_layer(); --ilayer)
	{
		for (gld_shape_iter ishape = (*ilayer).rbegin_shape(); ishape != (*ilayer).rend_shape(); --ishape)
		{
			if ((*ishape).intersect(x, y, ScreenToDataSpace()) 
				&& ((*ishape).capabilities() & SC_ENABLETRANSFORM) == SC_ENABLETRANSFORM)
			{
				return (*ishape);
			}
		}
	}

	return gld_shape();
}

void TShapeEditor::DoSelect(gld_shape_sel &sel, bool bsort /*= true*/)
{	
	if (CurTool())
	{
		FilterSelection(sel, CurTool());
	}	

	if (bsort)
	{
		SortSelection(sel);
	}

	SetSelection(sel);	
}

// 按Zorder排序选择集
void TShapeEditor::SortSelection(gld_shape_sel &sel)
{	
	gld_shape_sel	_tmp;

	gld_layer_iter	_li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();		
	
	for (; _li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --_li)
	{
		gld_layer		_layer = *_li;

		if (IsNormalLayer(_layer))
		{
			gld_shape_iter	_si = _layer.rbegin_shape();
			for (; _si != _layer.rend_shape(); --_si) 
			{				
				if (sel.exist(*_si))
				{
					_tmp.select(*_si);
				}
			}
		}
	}

	sel = _tmp;
}

// Others
int TShapeEditor::GetPencilDiameter()
{
	TLineStyle	*pLineStyle = THE_BASE(m_pBase)->pCurLineStyle;
	int			width = 0;

	if (pLineStyle)
	{
		width = pLineStyle->GetWidth() * GetZoomPercent() / 2000;
	}

	return width;
}

void TShapeEditor::GetEffectBox(POINT *box)
{
	if (THE_BASE(m_pBase)->sel.count() == 1)
	{
		gld_shape	shape = *(THE_BASE(m_pBase)->sel.begin());
		gld_rect	rc = shape.bound();
		TMatrix		mat = shape.matrix();

		box[0].x = rc.left;
		box[0].y = rc.top;
		box[1].x = rc.right;
		box[1].y = rc.top;
		box[2].x = rc.right;
		box[2].y = rc.bottom;
		box[3].x = rc.left;
		box[3].y = rc.bottom;
		
		gld_point op = shape.trans_origin();
		box[4].x = op.x;
		box[4].y = op.y;

		for (int i = 0; i < 5; i++)
		{
			mat.Transform(box[i].x, box[i].y);
		}
	}
	else
	{
		if (THE_BASE(m_pBase)->sel.empty())		
			memset(box, 0, sizeof(POINT) * 5);
		else
		{
			gld_shape_sel::iterator iter = THE_BASE(m_pBase)->sel.begin();
			gld_rect				rc = (*iter).effect_box();

			for (++iter; iter != THE_BASE(m_pBase)->sel.end(); ++iter)
			{
				rc = rc + (*iter).effect_box();
			}

			box[0].x = rc.left;
			box[0].y = rc.top;
			box[1].x = rc.right;
			box[1].y = rc.top;
			box[2].x = rc.right;
			box[2].y = rc.bottom;
			box[3].x = rc.left;
			box[3].y = rc.bottom;
			box[4].x = (rc.left + rc.right) >> 1;
			box[4].y = (rc.top + rc.bottom) >> 1;
		}
	}
}

void TShapeEditor::AdjustAnchorPoint(int &x, int &y)
{
	// 捕作导引线
	if (THE_BASE(m_pBase)->sel.count() == 1 && SnapToGuidLine(x, y) != NULL)
	{		
		return;
	}

	gld_point	snap[2];
	double		err[2];
	int			i;

	for (i = 0; i < 2; i++)
	{
		snap[i].x = x;
		snap[i].y = y;
		err[i] = -1;
	}

	// 捕作网格
	if (THE_BASE(m_pBase)->bSnapToGrid && SnapToGrid(snap[0].x, snap[0].y))
	{
		err[0] = (snap[0].x - x) * (snap[0].x - x) + (snap[0].y - y) * (snap[0].y - y);
	}

	// 捕作辅助线
	if (ShowAuxLine() && SnapToAuxLine(snap[1].x, snap[1].y))
	{
		err[1] = (snap[1].x - x) * (snap[1].x - x) + (snap[1].y - y) * (snap[1].y - y);	
	}

	// 选择最近的捕作
	int		sel = -1;
	double	min = 1e24;

	for (i = 0; i < 2; i++)
	{
		if (err[i] != -1 && err[i] < min)
		{
			min = err[i];
			sel = i;
		}
	}

	if (sel != -1)
	{
		x = snap[sel].x;
		y = snap[sel].y;
	}
}

void TShapeEditor::AdjustPoint(int &x, int &y)
{		
	gld_point	snap[2];
	double		err[2];
	int			i;

	for (i = 0; i < 2; i++)
	{
		snap[i].x = x;
		snap[i].y = y;
		err[i] = -1;
	}

	// 捕作网格
	if (THE_BASE(m_pBase)->bSnapToGrid && SnapToGrid(snap[0].x, snap[0].y))
	{
		err[0] = (snap[0].x - x) * (snap[0].x - x) + (snap[0].y - y) * (snap[0].y - y);
	}

	// 捕作辅助线
	if (ShowAuxLine() && SnapToAuxLine(snap[1].x, snap[1].y))
	{
		err[1] = (snap[1].x - x) * (snap[1].x - x) + (snap[1].y - y) * (snap[1].y - y);	
	}

	// 选择最近的捕作
	int		sel = -1;
	double	min = 1e24;

	for (i = 0; i < 2; i++)
	{
		if (err[i] != -1 && err[i] < min)
		{
			min = err[i];
			sel = i;
		}
	}

	if (sel != -1)
	{
		x = snap[sel].x;
		y = snap[sel].y;
	}
}

#define MAX_SNAP_ERROR	220

tGuidLine *TShapeEditor::SnapToGuidLine(int &x, int &y)
{
	gld_layer CurLayer = GetCurLayer();

	if (CurLayer.validate())
	{
		tGuidLine	*pGuidLine = NULL;
		double		fMinDst = MAX_SNAP_ERROR / THE_BASE(m_pBase)->fZoom;
		tPoint		Query;
		tPoint		Where;
		tGuidPoint	*pWhat;
		tPoint		Nearest;

		Query.x = x;
		Query.y = y;
		gld_list<tGuidLine *>::iterator iGuidLine = CurLayer.guid_lines()->begin();
		for (; iGuidLine != CurLayer.guid_lines()->end(); ++iGuidLine)
		{	
			double dst = tGuidLineNearestPoint(*iGuidLine, &Query, &pWhat, &Where);
			if (dst < fMinDst)
			{
				fMinDst = dst;
				pGuidLine = *iGuidLine;
				Nearest = Where;
			}				
		}

		if (pGuidLine)
		{
			x = Nearest.x;
			y = Nearest.y;

			return pGuidLine;
		}
	}

	return NULL;
}

bool TShapeEditor::SnapToGrid(int &x, int &y)
{
	bool	snap = false;
	int		maxErr = (int)(MAX_SNAP_ERROR / THE_BASE(m_pBase)->fZoom);
	int		tx = (int)((double)x / THE_BASE(m_pBase)->Grid.xDist + 0.5) * THE_BASE(m_pBase)->Grid.xDist;
	int		ty = (int)((double)y / THE_BASE(m_pBase)->Grid.yDist + 0.5) * THE_BASE(m_pBase)->Grid.yDist;
	
	if (abs(tx - x) < maxErr)
	{
		x = tx;
		snap = true;
	}

	if (abs(ty - y) < maxErr)
	{
		y = ty;	
		snap = true;
	}

	return snap;
}

bool TShapeEditor::SnapToAuxLine(int &x, int &y)
{
	int					maxErr = (int)(MAX_SNAP_ERROR / THE_BASE(m_pBase)->fZoom);
	int					hError = maxErr, vError = maxErr;
	int					hPos = 0, vPos = 0;
	TShapeEditorAuxLine	&al = GetAuxLine();

	if (al.HLines.size() > 0)
	{
		int hIndex = al.Find(y, true);
		if (hIndex == 0)
		{
			hError = al.HLines[0] - y;
			hPos = al.HLines[0];
		}
		else if (hIndex == al.HLines.size())
		{
			hError = y - al.HLines[al.HLines.size() - 1];
			hPos = al.HLines[al.HLines.size() - 1];
		}
		else
		{
			if (y - al.HLines[hIndex - 1] > al.HLines[hIndex] - y)
			{
				hError = al.HLines[hIndex] - y;
				hPos = al.HLines[hIndex];
			}
			else
			{
				hError = y - al.HLines[hIndex - 1];
				hPos = al.HLines[hIndex - 1];
			}
		}
	}

	if (al.VLines.size() > 0)
	{
		int vIndex = al.Find(x, false);
		if (vIndex == 0)
		{
			vError = al.VLines[0] - x;
			vPos = al.VLines[0];
		}
		else if (vIndex == al.VLines.size())
		{
			vError = x - al.VLines[al.VLines.size() - 1];
			vPos = al.VLines[al.VLines.size() - 1];
		}
		else
		{
			if (x - al.VLines[vIndex - 1] > al.VLines[vIndex] - x)
			{
				vError = al.VLines[vIndex] - x;
				vPos = al.VLines[vIndex];
			}
			else
			{
				vError = x - al.VLines[vIndex - 1];
				vPos = al.VLines[vIndex - 1];
			}
		}
	}

	bool ret = false;

	if (hError < maxErr)
	{
		y = hPos;
		ret = true;
	}

	if (vError < maxErr)
	{
		x = vPos;
		ret = true;
	}
	
	return ret;
}

HCURSOR	TShapeEditor::InkBottleCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_INK_BOTTLE : IDC_G_INK_BOTTLE_DISABLE) - IDC_G_FIRST];	
}

HCURSOR TShapeEditor::PaintBucketCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_PAINT_BUCKET : IDC_G_PAINT_BUCKET_DISABLE) - IDC_G_FIRST];	
}

HCURSOR TShapeEditor::FillTransformCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_FILL_TRANSFORM : IDC_G_FILL_TRANSFORM_DISABLE) - IDC_G_FIRST];
}

HCURSOR	TShapeEditor::LineCursor(bool Enable)
{	
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_CROSS : IDC_G_CROSS_DISABLE) - IDC_G_FIRST];
}

HCURSOR	TShapeEditor::RectCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_CROSS : IDC_G_CROSS_DISABLE) - IDC_G_FIRST];
}

HCURSOR	TShapeEditor::OvalCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_CROSS : IDC_G_CROSS_DISABLE) - IDC_G_FIRST];
}

HCURSOR	TShapeEditor::PenCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_PEN : IDC_G_PEN_DISABLE) - IDC_G_FIRST];
}

HCURSOR	TShapeEditor::PencilCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_PENCIL : IDC_G_PENCIL_DISABLE) - IDC_G_FIRST];
}

HCURSOR TShapeEditor::PolygonCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_CROSS : IDC_G_CROSS_DISABLE) - IDC_G_FIRST];
}

HCURSOR TShapeEditor::StarCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_CROSS : IDC_G_CROSS_DISABLE) - IDC_G_FIRST];
}

HCURSOR TShapeEditor::RoundRectCursor(bool Enable)
{
	return THE_BASE(m_pBase)->hStockCursors[(Enable ? IDC_G_CROSS : IDC_G_CROSS_DISABLE) - IDC_G_FIRST];
}

bool TShapeEditor::CurLayerIsNormal()
{
	return IsNormalLayer(GetCurLayer());
}

bool TShapeEditor::IsNormalLayer(gld_layer &layer)
{
	/*
	if (layer.validate())
	{
		// first layer is stream sound
		return (layer != *CurFrame().rbegin_layer()) && (layer.attributes() == LA_NORMAL);
	}

	return false;
	*/

	return layer.validate();
}

int TShapeEditor::PtOnLineStyle(int x, int y, gld_layer &layer, gld_shape &shape, gld_draw_obj &obj)
{
	if (THE_BASE(m_pBase)->CurFrame.validate())
	{
		gld_layer_iter	li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

		for ( ;li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --li)
		{
			gld_layer	l = *li;

			if (IsNormalLayer(l))
			{
				gld_shape_iter	si = l.rbegin_shape();

				for (; si != l.rend_shape(); --si)
				{
					gld_shape	s = *si;

					if (s.validate() && (s.capabilities() & SC_DIRECTFETCH) == SC_DIRECTFETCH)
					{
						gld_draw_obj	o;
						gld_edge		e;

						if (s.pt_on_edge(x, y, o, e, ScreenToDataSpace()))
						{
							layer = l;
							shape = s;
							obj = o;

							return e.line_style();
						}
					}
				}
			}
		}
	}

	return 0;
}

int TShapeEditor::PtOnFillStyle(int x, int y, gld_layer &layer, gld_shape &shape, gld_draw_obj &obj)
{
	if (THE_BASE(m_pBase)->CurFrame.validate())
	{
		gld_layer_iter	li = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

		for ( ;li != THE_BASE(m_pBase)->CurFrame.rend_layer(); --li)
		{
			gld_layer	l = *li;

			if (IsNormalLayer(l))
			{
				gld_shape_iter	si = l.rbegin_shape();

				for (; si != l.rend_shape(); --si)
				{
					gld_shape	s = *si;

					if (s.validate() && (s.capabilities() & SC_DIRECTFETCH) == SC_DIRECTFETCH)
					{
						gld_draw_obj	o;

						int				i = s.pt_on_cover(x, y, o);

						if (i > 0)
						{
							layer = l;
							shape = s;
							obj = o;

							return i;
						}
					}
				}
			}
		}
	}

	return 0;
}

bool TShapeEditor::ToolMatchShape(TTool *pTool, gld_shape &shape)
{
	if ((pTool == NULL) || (!shape.validate()))
	{
		return false;
	}

	int	nToolAttr = pTool->Capabilities();
	int nShapeAttr = shape.capabilities();
	
	if ((nToolAttr & TC_RESHAPE) == TC_RESHAPE)
	{
		return ((nShapeAttr & SC_ENABLERESHAPE) == SC_ENABLERESHAPE);
	}
	else if ((nToolAttr & TC_TRANSFORM) == TC_TRANSFORM)
	{
		return ((nShapeAttr & SC_ENABLETRANSFORM) == SC_ENABLETRANSFORM);
	}
	else if ((nToolAttr & TC_CHANGELINESTYLE) == TC_CHANGELINESTYLE || (nToolAttr & TC_CHANGEFILLSTYLE) == TC_CHANGEFILLSTYLE)
	{
		return ((nShapeAttr & SC_DIRECTFETCH) == SC_DIRECTFETCH);
	}

	return true;	
}

void TShapeEditor::CreateTools()
{	
	THE_BASE(m_pBase)->Tools.AddTool(new SETransformTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SEReshapeTool(this));	
	THE_BASE(m_pBase)->Tools.AddTool(new SELineTool(this));		
	THE_BASE(m_pBase)->Tools.AddTool(new SEOvalTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SERectTool(this));	
	THE_BASE(m_pBase)->Tools.AddTool(new SEPencilTool(this));	
	THE_BASE(m_pBase)->Tools.AddTool(new SEPenTool(this));	
	THE_BASE(m_pBase)->Tools.AddTool(new SEInkBottleTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SEPaintBucketTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SEFillTransformTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SEPencilGuidLineTool(&m_xIGuidLine));
	THE_BASE(m_pBase)->Tools.AddTool(new SEZoomTool(this, SEZoomTool::ZTO_OUT));
	THE_BASE(m_pBase)->Tools.AddTool(new SEZoomTool(this, SEZoomTool::ZTO_IN));
	THE_BASE(m_pBase)->Tools.AddTool(new SEHandTool(this));
	
	// sub tools
    THE_BASE(m_pBase)->Tools.AddTool(new SEPolygonTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SERoundRectTool(this));
	THE_BASE(m_pBase)->Tools.AddTool(new SEStarTool(this));	
	THE_BASE(m_pBase)->Tools.AddTool(new SELineGuidLineTool(&m_xIGuidLine));
	THE_BASE(m_pBase)->Tools.AddTool(new SEPolygonGuidLineTool(&m_xIGuidLine));
	THE_BASE(m_pBase)->Tools.AddTool(new SEOvalGuidLineTool(&m_xIGuidLine));
	THE_BASE(m_pBase)->Tools.AddTool(new SEPenGuidLineTool(&m_xIGuidLine));		

	// Text Tool	
	THE_BASE(m_pBase)->Tools.AddTool(new SETextToolEx(&TextToolIClass));	

	// Load tools' options
	((SERoundRectTool *)Tools()[IDT_ROUNDRECT])->SetRoundRadius(max(1 * 20, (int)AfxGetApp()->GetProfileInt("Tools\\Round Rect", "Radius", ((SERoundRectTool *)Tools()[IDT_ROUNDRECT])->GetRoundRadius())));
	((SEPolygonTool *)Tools()[IDT_POLYGON])->SetVertexNum(max(3, (int)AfxGetApp()->GetProfileInt("Tools\\Polygon", "Vertex", ((SEPolygonTool *)Tools()[IDT_POLYGON])->GetVertexNum())));
	((SEStarTool *)Tools()[IDT_STAR])->SetVertexNum(max(3, (int)AfxGetApp()->GetProfileInt("Tools\\Star", "Vertex", ((SEStarTool *)Tools()[IDT_STAR])->GetVertexNum())));

	((SEPolygonGuidLineTool *)Tools()[IDT_POLYGONGUIDLINE])->SetVertexNum(max(3, (int)AfxGetApp()->GetProfileInt("Tools\\Polygon Guide Line", "Vertex", ((SEPolygonGuidLineTool *)Tools()[IDT_POLYGONGUIDLINE])->GetVertexNum())));
}
// End new interface

// Process ink bottle tool hit test
bool TShapeEditor::InkBottleFetchHitEdge(int x, int y, gld_shape &shape, gld_draw_obj &obj, gld_edge &edge)
{
	ASSERT(THE_BASE(m_pBase)->CurFrame.validate());

	gld_layer_iter		iLayer = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

	for(; iLayer != THE_BASE(m_pBase)->CurFrame.rend_layer(); --iLayer)
	{
		gld_layer	Layer = *iLayer;

		if (IsNormalLayer(Layer))
		{
			gld_shape_iter iShape = Layer.rbegin_shape();

			for (; iShape != Layer.rend_shape(); --iShape)
			{
				gld_shape Shape = *iShape;

				if ((Shape.capabilities() & SC_DIRECTFETCH) != 0)
				{	// is single level shape
					int	tx = x;
					int ty = y;
					TMatrix mat = Shape.matrix().Invert();
					gld_rect bound = Shape.bound();

					mat.Transform(tx, ty);
					bound.inflate(-40, -40, 40, 40);

					if (bound.pt_in_rect(tx, ty))
					{
						gld_draw_obj_iter	iObj = Shape.rbegin_draw_obj();

						for(; iObj != Shape.rend_draw_obj(); --iObj)
						{
							gld_draw_obj	Obj = *iObj;
							gld_a_point_iter iAPoint = Obj.begin_a_point();

							for (; iAPoint != Obj.end_a_point(); ++iAPoint)
							{
								gld_a_point APoint = *iAPoint;
								gld_edge_iter iEdge = APoint.begin_edge();

								for(; iEdge != APoint.end_edge(); iEdge.right())
								{
									gld_edge Edge = *iEdge;

									int StyleIndex = Edge.line_style();
									int Radius = 61;
									
									if (StyleIndex > 0)
									{
										TLineStyle	*pLineStyle = Obj.line_styles().Get(StyleIndex);

										if (pLineStyle)
										{
											Radius = pLineStyle->GetWidth() * GetZoomPercent() / 200 + 1;
										}
									}

									Radius = max(Radius, 61);

									if (Edge.edge_type() == S_straight_edge)
									{
										int x1 = Edge.s_point().x();
										int x2 = Edge.e_point().x();
										int y1 = Edge.s_point().y();
										int y2 = Edge.e_point().y();

										gld_rect box(x1, y1, x2, y2);

										box.normalize();
										box.inflate(-Radius, -Radius, Radius, Radius);
										
										if (box.pt_in_rect(tx, ty))
										{
											tPoint P1, P2, P;
											
											P1.x = x1;
											P1.y = y1;
											P2.x = x2;
											P2.y = y2;
											P.x = tx;
											P.y = ty;

											int Dist = (int)tPointDistance(&P, &P1, &P2);

											if (Dist < Radius)
											{
												shape = Shape;
												obj = Obj;
												edge = Edge;

												return true;
											}
										}
									}
									else if (Edge.edge_type() == S_qbezier_edge)
									{
										gld_q_edge	QEdge = Edge;

										tQBezier	QBezier;

										QBezier.P0.x = QEdge.s_point().x();
										QBezier.P0.y = QEdge.s_point().y();
										QBezier.P1.x = QEdge.c_point().x();
										QBezier.P1.y = QEdge.c_point().y();
										QBezier.P2.x = QEdge.e_point().x();
										QBezier.P2.y = QEdge.e_point().y();

										tRect	Box;
										
										tQBezierSurroundBox(&QBezier, &Box);

										Box.xmin -= Radius;
										Box.ymin -= Radius;
										Box.xmax += Radius;
										Box.ymax += Radius;

										if (tx > Box.xmin && tx < Box.xmax && ty > Box.ymin && ty < Box.ymax)
										{
											tPoint	P;
											double	u;

											P.x = tx;
											P.y = ty;											

											int Dist = (int)tPointDistance(&P, &QBezier, &u);

											if (Dist < Radius)
											{
												shape = Shape;
												obj = Obj;
												edge = Edge;

												return true;
											}
										}
									}
									else
									{
										ASSERT(false);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool TShapeEditor::PaintBucketFetchObj(int x, int y, gld_shape &shape, gld_draw_obj &obj)
{
	ASSERT(THE_BASE(m_pBase)->CurFrame.validate());

	gld_layer_iter	iLayer = THE_BASE(m_pBase)->CurFrame.rbegin_layer();

	for(; iLayer != THE_BASE(m_pBase)->CurFrame.rend_layer(); --iLayer)
	{
		gld_layer		Layer = *iLayer;

		if (IsNormalLayer(Layer))
		{
			gld_shape_iter	iShape = Layer.rbegin_shape();
		
			for (; iShape != Layer.rend_shape(); --iShape)
			{
				gld_shape	Shape = *iShape;

				if ((Shape.capabilities() & SC_DIRECTFETCH) != 0)
				{				
					if (Shape.inner_point(x, y, obj))
					{
						shape = Shape;

						return true;
					}
				}
			}
		}
	}

	return false;
}

// 从draw object对象中移除锚点
void TShapeEditor::RemoveAnchorPoint(gld_shape &shape, gld_draw_obj &obj, gld_a_point &point)
{	
	CCmdRemoveAnchorPoint *pCmd = new CCmdRemoveAnchorPoint(shape, obj, point);
	THE_BASE(m_pBase)->Commands.Do(pCmd, false);	
}

// 向draw object对象中插入锚点(x, y)
void TShapeEditor::InsertAnchorPoint(gld_shape &shape, gld_draw_obj &obj, gld_edge &edge, int x, int y)
{
	TMatrix	mat = shape.rmatrix();
	mat.Transform(x, y);

	CCmdInsertAnchorPoint	*pCmd = new CCmdInsertAnchorPoint(shape, obj, edge, x, y);
	THE_BASE(m_pBase)->Commands.Do(pCmd, false);
}

void TShapeEditor::Reline(TReshapeObject &obj, double u, int x, int y)
{
	CCmdReline	*pCmd = new CCmdReline(obj.shape, obj.draw_obj, obj.edge, u, x, y);
	THE_BASE(m_pBase)->Commands.Do(pCmd);
}

// 移动控制点
void TShapeEditor::Reshape(TReshapeObject &obj, int _x, int _y)
{
	if (obj.type == RESHAPE_A_POINT) {			
		TMatrix _m = obj.shape.rmatrix();
		_m.Transform(_x, _y);

		CCmdMoveAnchorPoint *pCmd = new CCmdMoveAnchorPoint(obj.shape, obj.draw_obj, obj.a_points, _x, _y);
		THE_BASE(m_pBase)->Commands.Do(pCmd);		
	}
	else if (obj.type == RESHAPE_C_POINT) {
		TMatrix _m = obj.shape.rmatrix();
		_m.Transform(_x, _y);
		
		CCmdMoveControlPoint *pCmd = new CCmdMoveControlPoint(obj.shape, obj.draw_obj, obj.c_point, _x, _y);
		THE_BASE(m_pBase)->Commands.Do(pCmd);		
	}
}

int TShapeEditor::NormalizeZoomPercent(int percent)
{
	if (percent < 10)
	{
		percent = 10;
	}
	else if (percent > 1000)
	{
		percent = 1000;
	}
	
	return percent;
}

// 计算并设置设计窗口滚动条的位置
void TShapeEditor::RefreshScrollBar()
{
	ASSERT(THE_BASE(m_pBase)->hWnd);

	RefreshHScrollBar(true);
	RefreshVScrollBar(true);
}

void TShapeEditor::GetHScrollRange(int &xmin, int &xmax)
{
	xmin = 0;
	xmax = THE_BASE(m_pBase)->WorkSpaceSize.cx - 1;
}

void TShapeEditor::GetVScrollRange(int &ymin, int &ymax)
{
	ymin = 0;
	ymax = THE_BASE(m_pBase)->WorkSpaceSize.cy - 1;
}

// 计算并设置水平滚动条的位置
void TShapeEditor::RefreshHScrollBar(bool bRedraw)
{
	SCROLLINFO	si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if (::GetScrollInfo(THE_BASE(m_pBase)->hWnd, SB_HORZ, &si))
	{
		UINT fMask = 0;
		UINT nPage = THE_BASE(m_pBase)->ViewSize.cx - 1;
		if (si.nPage != nPage)
		{
			fMask |= SIF_PAGE;
			si.nPage = nPage;
		}
		int nMax = THE_BASE(m_pBase)->WorkSpaceSize.cx - 1;
		int nMin = 0;
		if (nMax != si.nMax || nMin != si.nMin)
		{
			fMask |= SIF_RANGE;
			si.nMax = nMax;
			si.nMin = nMin;
		}
		int nPos = GetHScrollPos();
		if (nPos != si.nPos)
		{
			fMask |= SIF_POS;
			si.nPos = nPos;
		}
		::SetScrollInfo(THE_BASE(m_pBase)->hWnd, SB_HORZ, &si, (BOOL)bRedraw);
	}
}

// 计算并设置垂直滚动条的位置
void TShapeEditor::RefreshVScrollBar(bool bRedraw)
{
	SCROLLINFO	si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if (::GetScrollInfo(THE_BASE(m_pBase)->hWnd, SB_VERT, &si))
	{
		UINT fMask = 0;
		UINT nPage = THE_BASE(m_pBase)->ViewSize.cy - 1;
		if (si.nPage != nPage)
		{
			fMask |= SIF_PAGE;
			si.nPage = nPage;
		}
		int nMax = THE_BASE(m_pBase)->WorkSpaceSize.cy - 1;
		int nMin = 0;
		if (nMax != si.nMax || nMin != si.nMin)
		{
			fMask |= SIF_RANGE;
			si.nMax = nMax;
			si.nMin = nMin;
		}
		int nPos = GetVScrollPos();
		if (nPos != si.nPos)
		{
			fMask |= SIF_POS;
			si.nPos = nPos;
		}
		::SetScrollInfo(THE_BASE(m_pBase)->hWnd, SB_VERT, &si, (BOOL)bRedraw);
	}
}

// 根据影片在画布上的偏移计算水平滚动条的位置
int TShapeEditor::GetHScrollPos()
{
	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		return (int)((THE_BASE(m_pBase)->WorkSpaceSize.cx - THE_BASE(m_pBase)->MovieSize.cx * THE_BASE(m_pBase)->fZoom) / 2.0)
			- _round(THE_BASE(m_pBase)->fOffsetX / 20.0);
	}
	else
	{
		return THE_BASE(m_pBase)->WorkSpaceSize.cx / 2 - _round(THE_BASE(m_pBase)->fOffsetX / 20.0);
	}
}

// 根据影片在画布上的偏移计算垂直滚动条的位置
int TShapeEditor::GetVScrollPos()
{
	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		return (int)((THE_BASE(m_pBase)->WorkSpaceSize.cy - THE_BASE(m_pBase)->MovieSize.cy * THE_BASE(m_pBase)->fZoom) / 2.0)
			-_round(THE_BASE(m_pBase)->fOffsetY / 20.0);
	}
	else
	{
		return THE_BASE(m_pBase)->WorkSpaceSize.cy / 2 - _round(THE_BASE(m_pBase)->fOffsetY / 20.0);
	}
}

// 根据可滚动区域的大小计算水平偏移的范围
void TShapeEditor::GetHorzOffsetRange(double &xmin, double &xmax)
{
	if (GetEditMode() == EDIT_SCENE_MODE)
	{
		xmin = THE_BASE(m_pBase)->ViewSize.cx * 20.0 - 
			(THE_BASE(m_pBase)->WorkSpaceSize.cx + THE_BASE(m_pBase)->MovieSize.cx * THE_BASE(m_pBase)->fZoom) * 10.0;
		xmax = (THE_BASE(m_pBase)->WorkSpaceSize.cx - THE_BASE(m_pBase)->MovieSize.cx * THE_BASE(m_pBase)->fZoom) * 10.0;
	}
	else
	{
		xmin = THE_BASE(m_pBase)->ViewSize.cx * 20.0 - THE_BASE(m_pBase)->WorkSpaceSize.cx * 10.0;
		xmax = THE_BASE(m_pBase)->WorkSpaceSize.cx * 10.0;
	}
}

// 根据可滚动区域的大小计算垂直偏移的范围
void TShapeEditor::GetVertOffsetRange(double &ymin, double &ymax)
{
	if (GetEditMode() == EDIT_SCENE_MODE)
	{	
		ymin = THE_BASE(m_pBase)->ViewSize.cy * 20.0 - 
			(THE_BASE(m_pBase)->WorkSpaceSize.cy + THE_BASE(m_pBase)->MovieSize.cy * THE_BASE(m_pBase)->fZoom) * 10.0;
		ymax = (THE_BASE(m_pBase)->WorkSpaceSize.cy - THE_BASE(m_pBase)->MovieSize.cy * THE_BASE(m_pBase)->fZoom) * 10.0;
	}
	else
	{
		ymin = THE_BASE(m_pBase)->ViewSize.cy * 20.0 - THE_BASE(m_pBase)->WorkSpaceSize.cy * 10.0;
		ymax = THE_BASE(m_pBase)->WorkSpaceSize.cy * 10.0;
	}
}

// 取影片大小
CSize TShapeEditor::GetMovieSize()
{ 
	return THE_BASE(m_pBase)->MovieSize; 
}

// 设置影片大小
void TShapeEditor::SetMovieSize(CSize &sz, bool bRedraw /*= true*/)
{
	if (sz.cx < IDI_MIN_MOVIE_WIDTH)
	{
		sz.cx = IDI_MIN_MOVIE_WIDTH;
	}
	else if (sz.cx > IDI_MAX_MOVIE_WIDTH)
	{
		sz.cx = IDI_MAX_MOVIE_WIDTH;
	}

	if (sz.cy < IDI_MIN_MOVIE_HEIGHT)
	{
		sz.cy = IDI_MIN_MOVIE_HEIGHT;
	}
	else if (sz.cy > IDI_MAX_MOVIE_HEIGHT)
	{
		sz.cy = IDI_MAX_MOVIE_HEIGHT;
	}

	if (sz != THE_BASE(m_pBase)->MovieSize)
	{
		THE_BASE(m_pBase)->MovieSize = sz;

		NormalizeOffset();
		ChangeMapMode();
		RefreshScrollBar();
		
		if (bRedraw)
		{
			Redraw();
			Repaint();	
		}
	}
}

// 限制偏移范围
bool TShapeEditor::NormalizeOffset()
{
	double	xmin, xmax, ymin, ymax;
	bool	ret = false;

	GetHorzOffsetRange(xmin, xmax);
	GetVertOffsetRange(ymin, ymax);

	if (THE_BASE(m_pBase)->fOffsetX < xmin)
	{
		THE_BASE(m_pBase)->fOffsetX = xmin;
		ret = true;
	}
	else if (THE_BASE(m_pBase)->fOffsetX > xmax)
	{
		THE_BASE(m_pBase)->fOffsetX = xmax;
		ret = true;
	}

	if (THE_BASE(m_pBase)->fOffsetY < ymin)
	{
		THE_BASE(m_pBase)->fOffsetY = ymin;
		ret = true;
	}
	else if (THE_BASE(m_pBase)->fOffsetY > ymax)
	{
		THE_BASE(m_pBase)->fOffsetY = ymax;
		ret = true;
	}

	return ret;
}

// 取绘图对象
TGraphicsDevice	*TShapeEditor::GetGraphicsDevice()
{
	return &(THE_BASE(m_pBase)->GDevice);
}

// 取影片背景颜色
COLORREF TShapeEditor::GetMovieBackground()
{
	return THE_BASE(m_pBase)->clMovieBackground;
}

// 设置影片背景颜色
void TShapeEditor::SetMovieBackground(COLORREF clBk, bool bRedraw)
{
	if (THE_BASE(m_pBase)->clMovieBackground != clBk)
	{
		THE_BASE(m_pBase)->clMovieBackground = clBk;

		if (bRedraw)
		{
			Redraw();
			Repaint();
		}
	}
}

// 设置编辑模式
void TShapeEditor::SetEditMode(int nMode, bool bRedraw)
{
	ASSERT(nMode == EDIT_SCENE_MODE || nMode == EDIT_COMPONENT_MODE);

	if (GetEditMode() != nMode)
	{
		THE_BASE(m_pBase)->nEditMode = nMode;

		NormalizeOffset();
		ChangeMapMode();
		RefreshScrollBar();

		if (bRedraw)
		{
			Redraw();
			Repaint();
		}
	}
}

// 取当前编辑模式
int TShapeEditor::GetEditMode()
{
	return THE_BASE(m_pBase)->nEditMode;
}

// 设置当前屏幕布局
void TShapeEditor::SetLayout(const TShapeEditorLayout *pLayout, bool bRedraw)
{
	if (pLayout)
	{
		THE_BASE(m_pBase)->fOffsetX = pLayout->fOffsetX;
		THE_BASE(m_pBase)->fOffsetY = pLayout->fOffsetY;
		if (THE_BASE(m_pBase)->fZoom != pLayout->fZoom)
		{
			THE_BASE(m_pBase)->fZoom = pLayout->fZoom;
			CSubjectManager::Instance()->GetSubject("Zoom")->Notify(0);
		}
		THE_BASE(m_pBase)->WorkSpaceSize = pLayout->szWorkspace;
		THE_BASE(m_pBase)->nEditMode = pLayout->nEditMode;
	}
	else
	{
		TShapeEditorLayout layout;		
		GetDefaultLayout(&layout, GetEditMode());
		SetLayout(&layout, bRedraw);

		return;
	}

	NormalizeOffset();
	ChangeMapMode();
	RefreshScrollBar();

	if (bRedraw)
	{
		Repaint();
	}
}

// 取当前屏幕布局
void TShapeEditor::GetLayout(TShapeEditorLayout *pLayout)
{
	ASSERT(pLayout);

	if (pLayout)
	{
		pLayout->fOffsetX = THE_BASE(m_pBase)->fOffsetX;
		pLayout->fOffsetY = THE_BASE(m_pBase)->fOffsetY;
		pLayout->fZoom = THE_BASE(m_pBase)->fZoom;		
		pLayout->szWorkspace = THE_BASE(m_pBase)->WorkSpaceSize;
		pLayout->nEditMode = THE_BASE(m_pBase)->nEditMode;
	}
}

// 取默认布局
void TShapeEditor::GetDefaultLayout(TShapeEditorLayout *pLayout, int nEditMode)
{
	ASSERT(pLayout && (nEditMode == EDIT_SCENE_MODE || nEditMode == EDIT_COMPONENT_MODE));	

	if (pLayout)
	{
		// Center movie
		double	gcx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cx - 1) / 2.0;
		double	gcy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->ViewSize.cy - 1) / 2.0;
		if (nEditMode == EDIT_SCENE_MODE)
		{		
			double	mcx = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cx - 1) / 2.0;
			double	mcy = PIXEL_TO_TWIPS(THE_BASE(m_pBase)->MovieSize.cy - 1) / 2.0;

			pLayout->fOffsetX = gcx - mcx;
			pLayout->fOffsetY = gcy - mcy;
		}
		else
		{
			pLayout->fOffsetX = gcx;
			pLayout->fOffsetY = gcy;
		}
		
		pLayout->fZoom = 1.0;
		pLayout->szWorkspace = THE_BASE(m_pBase)->WorkSpaceSize;
		pLayout->nEditMode = nEditMode;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface of guid line
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TShapeEditor::IGuidLine::GetSelection(gld_shape_sel &sel, TTool *pTool)
{
}

void TShapeEditor::IGuidLine::DoSelect(gld_shape_sel &sel, bool bsort /*= true*/)
{
}

bool TShapeEditor::IGuidLine::CanAddShape()
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->CanAddShape();
}

void TShapeEditor::IGuidLine::AdjustPoint(int &x, int &y)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->AdjustPoint(x, y);
}

void TShapeEditor::IGuidLine::AddLine(int x1, int y1, int x2, int y2)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	tGuidLine	*guidline = new tGuidLine;
	tGuidLineCreate(guidline);

	tGuidPoint	*p[3];	
	for (int i = 0; i < 3; i++)
	{
		p[i] = (tGuidPoint *)malloc(sizeof(tGuidPoint));	
	}

	p[0]->type = GPT_QBEZIER_ANCHOR_POINT;
	p[0]->x = x1;
	p[0]->y = y1;
	p[1]->type = GPT_QBEZIER_CONTROL_POINT;
	p[1]->x = (x1 + x2) >> 1;
	p[1]->y = (y1 + y2) >> 1;
	p[2]->type = GPT_QBEZIER_ANCHOR_POINT;
	p[2]->x = x2;
	p[2]->y = y2;

	tGuidLineInsertPointAfter(guidline, p[0], guidline->head);
	tGuidLineInsertPointAfter(guidline, p[1], p[0]);
	tGuidLineInsertPointAfter(guidline, p[2], p[1]);
	
	pThis->AddGuidLine(guidline);	
}

void TShapeEditor::IGuidLine::AddPolygon(const POINT *v, int count)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	tGuidLine	*guidline = new tGuidLine;
	tGuidLineCreate(guidline);

	int num = 2 * count;
	int i;
	tGuidPoint **p = new tGuidPoint*[num + 1];
	for (i = 0; i < num + 1; i++)
	{
		p[i] = (tGuidPoint *)malloc(sizeof(tGuidPoint));
	}

	p[0]->type = GPT_QBEZIER_ANCHOR_POINT;
	p[0]->x = v[0].x;
	p[0]->y = v[0].y;
	p[num]->type = GPT_QBEZIER_ANCHOR_POINT;
	p[num]->x = v[0].x;
	p[num]->y = v[0].y;	
	for (i = 0; i < count - 1; i++)
	{
		p[2 * i + 1]->type = GPT_QBEZIER_CONTROL_POINT;
		p[2 * i + 1]->x = (v[i].x + v[i + 1].x) / 2;
		p[2 * i + 1]->y = (v[i].y + v[i + 1].y) / 2;
		p[2 * i + 2]->type = GPT_QBEZIER_ANCHOR_POINT;
		p[2 * i + 2]->x = v[i + 1].x;
		p[2 * i + 2]->y = v[i + 1].y;
	}
	p[2 * i + 1]->type = GPT_QBEZIER_CONTROL_POINT;
	p[2 * i + 1]->x = (v[i].x + v[0].x) / 2;
	p[2 * i + 1]->y = (v[i].y + v[0].y) / 2;

	tGuidLineInsertPointAfter(guidline, p[0], guidline->head);
	for (i = 1; i < num + 1; i++)
	{
		tGuidLineInsertPointAfter(guidline, p[i], p[i - 1]);
	}
	delete[] p;
	
	pThis->AddGuidLine(guidline);	
}

void TShapeEditor::IGuidLine::AddOval(int _x1, int _y1, int _x2, int _y2)	
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	tGuidLine	*guidline = new tGuidLine;
	tGuidLineCreate(guidline);

	int _xmin, _xmax, _ymin, _ymax;	
	
	if (_x1 > _x2) {
		_xmin = _x2;
		_xmax = _x1;
	}
	else {
		_xmin = _x1;
		_xmax = _x2;
	}
	if (_y1 > _y2) {
		_ymin = _y2;
		_ymax = _y1;
	}
	else {
		_ymin = _y1;
		_ymax = _y2;
	}

	// 用quadratic bezier拟合椭圆
	PiecewiseQuadraticBezier q;
	fit_ellipse(_xmin, _ymin, _xmax, _ymax, &q);	
	
	// 构建圆
	tGuidPoint	**_tp = new tGuidPoint*[q.num * 2 + 1];
	int i;
	for (i = 0; i < q.num * 2 + 1; i++)
	{
		_tp[i] = (tGuidPoint *)malloc(sizeof(tGuidPoint));
	}

	_tp[0]->type = GPT_QBEZIER_ANCHOR_POINT;
	_tp[0]->x = (int)q.curves[0].x;
	_tp[0]->y = (int)q.curves[0].y;
	tGuidLineInsertPointAfter(guidline, _tp[0], guidline->head);

	for (i = 1; i < 2 * q.num + 1; i++) 
	{
		_tp[i]->type = (i % 2 == 0) ? GPT_QBEZIER_ANCHOR_POINT : GPT_QBEZIER_CONTROL_POINT;
		_tp[i]->x = (int)q.curves[i].x;
		_tp[i]->y = (int)q.curves[i].y;
		tGuidLineInsertPointAfter(guidline, _tp[i], _tp[i - 1]);
	}

	free(q.curves);
	delete _tp;
	
	pThis->AddGuidLine(guidline);	
}

void TShapeEditor::IGuidLine::AddPath(const _path &_p)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	tGuidLine	*guidline = new tGuidLine;
	tGuidLineCreate(guidline);

	tGuidPoint	*_a1 = (tGuidPoint *)malloc(sizeof(tGuidPoint));
	_a1->type = GPT_QBEZIER_ANCHOR_POINT;
	_a1->x = _p.s_x;
	_a1->y = _p.s_y;
	tGuidLineInsertPointAfter(guidline, _a1, guidline->head);

	_edge *_pe = _p.head_edge;
	while (_pe != 0) 
	{
		
		tGuidPoint	*_pp[2];
		_pp[0] = (tGuidPoint *)malloc(sizeof(tGuidPoint));
		_pp[1] = (tGuidPoint *)malloc(sizeof(tGuidPoint));
		_pp[0]->type = GPT_QBEZIER_CONTROL_POINT;
		_pp[1]->type = GPT_QBEZIER_ANCHOR_POINT;
		if (_pe->_type() == _edge::S_s_edge) 
		{
			_pp[0]->x = (_a1->x + _pe->e_x) / 2;
			_pp[0]->y = (_a1->y + _pe->e_y) / 2;
			_pp[1]->x = _pe->e_x;
			_pp[1]->y = _pe->e_y;
		}
		else 
		{
			_q_bezier_edge *_t = static_cast<_q_bezier_edge *>(_pe);
			
			_pp[0]->x = _t->c_x;
			_pp[0]->y = _t->c_y;
			_pp[1]->x = _t->e_x;
			_pp[1]->y = _t->e_y;			
		}
		
		tGuidLineInsertPointAfter(guidline, _pp[0], _a1);
		tGuidLineInsertPointAfter(guidline, _pp[1], _pp[0]);
		_a1 = _pp[1];

		_pe = _pe->next_edge;
	}	
	
	pThis->AddGuidLine(guidline);	
}

void TShapeEditor::IGuidLine::AddPath(Point2 *p, int numOfPoints)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)
	
	tGuidLine *pGuidLine = new tGuidLine;
	tGuidLineCreate(pGuidLine);
	tPoint	*tp = new tPoint[numOfPoints];
	for (int i = 0; i < numOfPoints; i++)
	{
		tp[i].x = (int)p[i].x;
		tp[i].y = (int)p[i].y;
	}
	tGuidLineFix(tp, numOfPoints, pGuidLine, 6400);
	delete[] tp;

	pThis->AddGuidLine(pGuidLine);	
}

HCURSOR TShapeEditor::IGuidLine::LineCursor(bool Enable)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->LineCursor(Enable);
}

HCURSOR TShapeEditor::IGuidLine::PolygonCursor(bool Enable)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->PolygonCursor(Enable);
}

HCURSOR TShapeEditor::IGuidLine::OvalCursor(bool Enable)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->OvalCursor(Enable);
}

HCURSOR TShapeEditor::IGuidLine::PenCursor(bool Enable)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->PenCursor(Enable);
}

HCURSOR TShapeEditor::IGuidLine::PencilCursor(bool Enable)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->PencilCursor(Enable);
}

HPEN TShapeEditor::IGuidLine::GetStockPen(int pen)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->GetStockPen(pen);
}

HBRUSH TShapeEditor::IGuidLine::GetStockBrush(int brush)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->GetStockBrush(brush);
}

HCURSOR TShapeEditor::IGuidLine::GetStockCursor(int index)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	return pThis->GetStockCursor(index);
}

int TShapeEditor::IGuidLine::GetPencilDiameter()
{
	return 1;
}

void TShapeEditor::IGuidLine::Repaint()
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	pThis->Repaint();
}

void TShapeEditor::IGuidLine::SetRepaint(bool bRepaint)
{
	METHOD_PROLOGUE(TShapeEditor, IGuidLine)

	pThis->SetRepaint(bRepaint);
}

// 画导引线
void TShapeEditor::DrawGuidLine(HDC hdc, tGuidLine *pGuidLine)
{
	TDraftDC	dc(hdc);
	TMatrix		mat = DataSpaceToScreen();

	dc.Matrix(mat);

	tGuidPoint	*cur = pGuidLine->head->next;

	if (cur != pGuidLine->head)
	{
		HPEN hOldPen = dc.SelectPen(pGuidLine == GetGuidLine() ? 
			GetStockPen(SEP_GUIDLINE)
			: GetStockPen(GetUnactivePen(SEP_GUIDLINE)));

		int	x1 = cur->x;
		int y1 = cur->y;
		dc.MoveTo(x1, y1, TDraftDC::_trans_tag());
		
		while (cur != pGuidLine->head->prev)
		{	
			if (cur->type == GPT_LINE_KEY_POINT || cur->type == GPT_LINE_ANCHOR_POINT)
			{	// is a line
				cur = cur->next;

				int x2 = cur->x;
				int y2 = cur->y;
		
				dc.LineTo(x2, y2, TDraftDC::_trans_tag());
			}
			else
			{	// is quadratic bezier
				cur = cur->next;

				int x2 = cur->x;
				int y2 = cur->y;

				cur = cur->next;

				int x3 = cur->x;
				int y3 = cur->y;
				
				dc.BezierTo(x2, y2, x3, y3, TDraftDC::_trans_tag());
			}
		}		
		
		HBRUSH hOldBrush = dc.SelectBrush(GetStockBrush(SEB_HOLLOW_HANDLER));
		dc.SelectPen(GetStockPen(SEP_HANDLER));

		// start point	
		x1 = pGuidLine->head->next->x;
		y1 = pGuidLine->head->next->y;
		mat.Transform(x1, y1);
		dc.Ellipse(x1 - 40, y1 - 40, x1 + 60, y1 + 60, TDraftDC::_no_trans_tag());

		x1 = pGuidLine->head->prev->x;
		y1 = pGuidLine->head->prev->y;
		mat.Transform(x1, y1);
		// end point		
		dc.Ellipse(x1 - 40, y1 - 40, x1 + 60, y1 + 60, TDraftDC::_no_trans_tag());

		dc.SelectPen(hOldPen);
		dc.SelectBrush(hOldBrush);
	}
}

// 当前状态是否显示导引线
bool TShapeEditor::ShowGuidLine()
{
	return THE_BASE(m_pBase)->bShowGuidLine;
}

// 隐藏或者显示导引线
void TShapeEditor::ShowGuidLine(bool bShow)
{
	if (THE_BASE(m_pBase)->bShowGuidLine != bShow)
	{
		THE_BASE(m_pBase)->bShowGuidLine = bShow;

		Repaint();
	}
}

// 取指定位置的导引线
tGuidLine *TShapeEditor::GetGuidLine(int x, int y)
{
	if (!CurLayerIsNormal())
	{
		return NULL;
	}

	tGuidLine	*pGuidLine = NULL;
	double		fMinDst = 41.0 / THE_BASE(m_pBase)->fZoom;

	tPoint			query;
	tGuidPoint		*what;
	tPoint			where;

	query.x = x;
	query.y = y;
	
	gld_layer curLayer = GetCurLayer();
	gld_list<tGuidLine *>::iterator iGuidLine = curLayer.guid_lines()->begin();
	for (; iGuidLine != curLayer.guid_lines()->end(); ++iGuidLine)
	{
		tGuidLine	*p = *iGuidLine;
		double dst = tGuidLineNearestPoint(p, &query, &what, &where);
		if (dst < fMinDst)
		{
			fMinDst = dst;
			pGuidLine = p;
		}
	}

	return pGuidLine;	
}

// 取当前选中的导引线
tGuidLine *TShapeEditor::GetGuidLine()
{
	return THE_BASE(m_pBase)->pGuidLine;
}

// 设置当前选中的导引线
void TShapeEditor::SelectGuidLine(tGuidLine *pGuidLine)
{	
	if (pGuidLine != THE_BASE(m_pBase)->pGuidLine)
	{
		THE_BASE(m_pBase)->pGuidLine = pGuidLine;

		SetRepaint(FALSE);

		TNotifyTransceiver::Notify(this, SEN_SELECT_GUIDLINE, (LPARAM)pGuidLine);

		if (GetGuidLine() != NULL)
			DoSelect(gld_shape_sel());

		Repaint();

		SetRepaint(TRUE);
	}
}

// 添加导引线
void TShapeEditor::AddGuidLine(const tPoint *lpPoints, int numOfPoints)
{
	gld_layer	cur = GetCurLayer();

	if (cur.validate())
	{
		tGuidLine *pGuidLine = new tGuidLine;
		tGuidLineCreate(pGuidLine);
		tGuidLineFix(lpPoints, numOfPoints, pGuidLine, 6400);
		AddGuidLine(pGuidLine);
	}
}

HRESULT TShapeEditor::AddPathEffect(REFCLSID clsid, IGLD_CurvedPath *pIPath)
{
	CAutoPtr<gldEffect> pEffect(new gldEffect(clsid));
	if (pEffect->m_pIEffect == NULL || pEffect->m_pIParas == NULL)
		return E_FAIL;

	gldInstance* pInstance = _GetCurInstance();
	// calculate ideal start time
	if (pInstance->m_effectList.size() > 0)
	{
		gldEffect *pEffectLast = *pInstance->m_effectList.rbegin();
		pEffect->m_startTime = pEffectLast->m_startTime + pEffectLast->m_length;
	}
	else
	{
		pEffect->m_startTime = 0;
	}
	// calculate ideal length
	pEffect->CalcIdealLength(pInstance->GetEffectKeyCount());	
	// predefine path effect
	HRESULT hr = PutParameter(pEffect->m_pIParas, "Path", pIPath);
	CHECKRESULT();

	if (pEffect->m_pEffectInfo->m_autoShowConfig)
	{
		if (!pInstance->ConfigEffect(pInstance, pEffect))
			return E_FAIL;
	}

	Commands().Do(new CCmdInsertEffect(pInstance, pEffect.Detach(), NULL));

	return S_OK;
}

HRESULT TShapeEditor::AddPathEffect(REFCLSID clsid)
{
	memset(&m_clsidPathEffect, 0, sizeof(m_clsidPathEffect));

	CComPtr<IGLD_PathEffect> pIEffect;
	HRESULT hr = pIEffect.CoCreateInstance(clsid);
	CHECKRESULT();

	GLD_PredefinedPathTool toolId = gceNoTool;
	pIEffect->GetTool(&toolId);
	if (toolId == gceNoTool)
	{
		CComPtr<IGLD_CurvedPath> pIPath;
		pIEffect->GetPath(&pIPath);
		ASSERT(pIPath != NULL); // the path effect is error
		if (pIPath == NULL)
			return E_FAIL;
		return AddPathEffect(clsid, pIPath);
	}
	else
	{
		int tool = IDT_PENCILGUIDLINE;
		if (toolId == gcePencilTool)
			tool = IDT_PENCILGUIDLINE;
		else if (toolId == gcePenTool)
			tool = IDT_PENGUIDLINE;
		else if (toolId == gceLineTool)
			tool = IDT_LINEGUIDLINE;
		else
			ASSERT(FALSE);
		SetCurTool(tool);
		m_clsidPathEffect = clsid;
		return S_OK;
	}	
}

void TShapeEditor::AddGuidLine(tGuidLine *pGuidLine)
{
	ASSERT(pGuidLine != NULL);

	CComSafeArray<DOUBLE> psa;
	SAFEARRAYBOUND bnd[2];
	bnd[0].lLbound = 0;
	bnd[0].cElements = pGuidLine->numOfGuidPoints;
	bnd[1].lLbound = 0;
	bnd[1].cElements = pGuidLine->numOfGuidPoints;
	COMVERIFY(psa.Create(bnd, 2));
	DOUBLE *pvData = (DOUBLE *)psa.m_psa->pvData;
	tGuidPoint *cur = pGuidLine->head->next;
	while (cur != pGuidLine->head)
	{
		pvData[0] = cur->x;
		pvData[1] = cur->y;
		pvData += 2;
		cur = cur->next;
	}
	tGuidLineDestroy(pGuidLine);
	delete pGuidLine;

	if (CurSel().count() > 0)
	{
		gldInstance *pInst = _GetCurInstance();
		ASSERT(pInst != NULL);

		SetCurTool(IDT_TRANSFORM);
		
		CComPtr<IGLD_CurvedPath> pIPath;
		HRESULT hr = pIPath.CoCreateInstance(__uuidof(GLD_CurvedPath));
		if (FAILED(hr))
		{
			AfxMessageBox(IDS_FAILED_CREATE_MOTION_PATH);
			return;
		}
		COMVERIFY(pIPath->put_Data(psa.m_psa));

		if (FAILED(AddPathEffect(m_clsidPathEffect, pIPath)))
		{
			AfxMessageBox(IDS_FAILED_CREATE_MOTION_PATH);
			return;
		}
	}
}

// 移动导引点
void TShapeEditor::RepositionGuidPoint(tGuidLine *guidLine, tGuidPoint *what, const tPoint &to)
{
	//CCmdMoveGuidPoint	*pCmd = new CCmdMoveGuidPoint(guidLine, what, to.x, to.y);
	//Commands().Do(pCmd, false);	
}


void TShapeEditor::RelineGuidLine(tGuidLine *guidLine, tGuidPoint *what, const tPoint &where, const tPoint &to)
{
	//tQBezier	qBezier;

	//tGuidLineQueryQBezier(guidLine, what, &qBezier, GLD_FORWARD);

	//if (tQBezierReline(&qBezier, &where, &to))
	//{
	//	CCmdMoveGuidPoint *pCmd = new CCmdMoveGuidPoint(guidLine, what->next, qBezier.P1.x, qBezier.P1.y);
	//	Commands().Do(pCmd, false);
	//}
}

void TShapeEditor::OffsetGuidLine(tGuidLine *pGuidLine, int nOffsetX, int nOffsetY)
{
	//CCmdMoveGuidLine *pCmd = new CCmdMoveGuidLine(pGuidLine, nOffsetX, nOffsetY);
	//Commands().Do(pCmd, false);
}

void TShapeEditor::RemoveGuidPoint(tGuidLine *guidLine, tGuidPoint *guidPoint)
{
	//CCmdRemoveGuidPoint	*pCmd = new CCmdRemoveGuidPoint(guidLine, guidPoint);
	//Commands().Do(pCmd, false);
}

void TShapeEditor::InsertGuidPoint(tGuidLine *guidLine, tGuidPoint *what, int x, int y)
{
	//CCmdInsertGuidPoint *pCmd = new CCmdInsertGuidPoint(guidLine, what, x, y);
	//Commands().Do(pCmd, false);
}


void TShapeEditor::AddShape(gld_shape &shape)
{
	CCmdAddShape	*pCmd = new CCmdAddShape(shape);

	THE_BASE(m_pBase)->Commands.Do(pCmd, false);
	
	
	if (!THE_BASE(m_pBase)->bLockTool)
	{
		SetCurTool(IDT_TRANSFORM);
	}	
}

bool TShapeEditor::LockTool()
{
	return THE_BASE(m_pBase)->bLockTool;
}

void TShapeEditor::LockTool(bool bLock)
{
	THE_BASE(m_pBase)->bLockTool = bLock;
}

void TShapeEditor::ChangeTransOrigin(const gld_shape &shape, int x, int y)
{
	CCmdChangeTransformOrigin *pCmd = new CCmdChangeTransformOrigin(shape, gld_point(x, y));
	THE_BASE(m_pBase)->Commands.Do(pCmd);

	//TCmdChangeTransOrigin	*pCmd = new TCmdChangeTransOrigin(shape, gld_point(x, y));

	//THE_BASE(m_pBase)->Commands.Do(pCmd, false);
}

void TShapeEditor::SetCurLineStyle(TLineStyle *pLineStyle)
{
	REFRENCE(pLineStyle);
	RELEASE(THE_BASE(m_pBase)->pCurLineStyle);	
	THE_BASE(m_pBase)->pCurLineStyle = pLineStyle;

	if (!THE_BASE(m_pBase)->sel.empty())
	{
		CCmdChangeLineStyle *pCmd = new CCmdChangeLineStyle(THE_BASE(m_pBase)->sel, pLineStyle);
		THE_BASE(m_pBase)->Commands.Do(pCmd, false);
	}
}

void TShapeEditor::SetCurFillStyle(TFillStyle *pFillStyle)
{
	REFRENCE(pFillStyle);
	RELEASE(THE_BASE(m_pBase)->pCurFillStyle);	
	THE_BASE(m_pBase)->pCurFillStyle = pFillStyle;

	if (!THE_BASE(m_pBase)->sel.empty())
	{
		CCmdChangeFillStyle *pCmd = new CCmdChangeFillStyle(THE_BASE(m_pBase)->sel, pFillStyle);
		THE_BASE(m_pBase)->Commands.Do(pCmd, false);
	}
}

TLineStyle *TShapeEditor::GetCurLineStyle()
{
	if (!THE_BASE(m_pBase)->sel.empty())
	{
		gld_shape shape = *(THE_BASE(m_pBase)->sel.begin());
        if ((shape.capabilities() & SC_DIRECTFETCH) != 0)
        {
		    gld_draw_obj obj = *shape.begin_draw_obj();

		    TLineStyle *pLineStyle = NULL;
		    if (obj.line_styles().Count() != 0)
			{
			    pLineStyle = obj.line_styles().Get(1);

				return pLineStyle;
			}			
        }
	}
	
	return THE_BASE(m_pBase)->pCurLineStyle;
}

TFillStyle *TShapeEditor::GetCurFillStyle()
{
	if (!THE_BASE(m_pBase)->sel.empty())
	{
		gld_shape shape = *(THE_BASE(m_pBase)->sel.begin());
        if ((shape.capabilities() & SC_DIRECTFETCH) != 0)
        {
		    gld_draw_obj obj = *shape.begin_draw_obj();

		    TFillStyle *pFillStyle = NULL;
		    if (obj.fill_styles().Count() != 0)
			{
			    pFillStyle = obj.fill_styles().Get(1);

				return pFillStyle;
			}			
        }
	}

	return THE_BASE(m_pBase)->pCurFillStyle;
}

bool TShapeEditor::Ready() const
{ 
	return THE_BASE(m_pBase) != 0; 
}

gld_frame TShapeEditor::CurFrame() 
{ 
	return THE_BASE(m_pBase)->CurFrame; 
}

HWND TShapeEditor::DesignWnd()
{ 
	return THE_BASE(m_pBase)->hWnd; 
}

void TShapeEditor::DesignWnd(HWND _wnd)
{ 
	THE_BASE(m_pBase)->hWnd = _wnd;		
}

TCommands &TShapeEditor::Commands()
{ 
	return THE_BASE(m_pBase)->Commands; 
}

TTools &TShapeEditor::Tools()
{ 
	return THE_BASE(m_pBase)->Tools; 
}

HBRUSH TShapeEditor::GetStockBrush(int brush)
{ 
	int BrushIndex = brush;
	
	if (IsActiveBrush(brush) && GetFocus() != THE_BASE(m_pBase)->hWnd)
		BrushIndex = GetUnactiveBrush(brush);

	return THE_BASE(m_pBase)->hStockBrushes[BrushIndex];	
}

HPEN TShapeEditor::GetStockPen(int pen) 
{ 
	int PenIndex = pen;
	
	if (IsActivePen(pen) && GetFocus() != THE_BASE(m_pBase)->hWnd)
		PenIndex = GetUnactivePen(pen);

	return THE_BASE(m_pBase)->hStockPens[PenIndex];	
}

bool TShapeEditor::SetStockPen(SEStockPenTypes pen, const LOGPEN *active, const LOGPEN *unactive)
{
	ASSERT(active);
	ASSERT(unactive);

	HPEN ActPen = CreatePenIndirect(active);
	if (ActPen == NULL)
		return false;
	HPEN UnactPen = CreatePenIndirect(unactive);
	if (UnactPen == NULL)
	{
		DeleteObject(ActPen);
		return false;
	}
	
	int ActPenIndex = (int)pen;
	int UnactPenIndex = ActPenIndex + SEP_COUNT;
	if (THE_BASE(m_pBase)->hStockPens[ActPenIndex] != NULL)
		VERIFY(DeleteObject(THE_BASE(m_pBase)->hStockPens[ActPenIndex]));
	THE_BASE(m_pBase)->hStockPens[ActPenIndex] = ActPen;

	if (THE_BASE(m_pBase)->hStockPens[UnactPenIndex] != NULL)
		VERIFY(DeleteObject(THE_BASE(m_pBase)->hStockPens[UnactPenIndex]));
	THE_BASE(m_pBase)->hStockPens[UnactPenIndex] = UnactPen;

	return true;
}

bool TShapeEditor::SetStockBrush(SEStockBrushTypes brush, const LOGBRUSH *active, const LOGBRUSH *unactive)
{
	ASSERT(active);
	ASSERT(unactive);

	HBRUSH ActBrush = CreateBrushIndirect(active);
	if (ActBrush == NULL)
		return false;
	HBRUSH UnactBrush = CreateBrushIndirect(unactive);
	if (UnactBrush == NULL)
	{
		DeleteObject(ActBrush);
		return false;
	}

	int ActBrushIndex = (int)brush;
	int UnactBrushIndex = ActBrushIndex + SEB_COUNT;
	if (THE_BASE(m_pBase)->hStockBrushes[ActBrushIndex] != NULL)
		VERIFY(DeleteObject(THE_BASE(m_pBase)->hStockBrushes[ActBrushIndex]));
	THE_BASE(m_pBase)->hStockBrushes[ActBrushIndex] = ActBrush;

	if (THE_BASE(m_pBase)->hStockBrushes[UnactBrushIndex] != NULL)
		VERIFY(DeleteObject(THE_BASE(m_pBase)->hStockBrushes[UnactBrushIndex]));
	THE_BASE(m_pBase)->hStockBrushes[UnactBrushIndex] = UnactBrush;

	return true;
}

HCURSOR TShapeEditor::GetStockCursor(int _i)
{ 
	return THE_BASE(m_pBase)->hStockCursors[_i - IDC_G_FIRST]; 
}

TTool *TShapeEditor::CurTool()
{
	if (THE_BASE(m_pBase)->Tools.Current() == -1)
	{
		return NULL;
	}
	else
	{
		return THE_BASE(m_pBase)->Tools[THE_BASE(m_pBase)->Tools.Current()]; 
	}
}

int TShapeEditor::GetZoomPercent()
{ 
	return _round(THE_BASE(m_pBase)->fZoom * 100.0); 
}

// 画选择集合
void TShapeEditor::DrawSelection(HDC hdc)
{
	if (!THE_BASE(m_pBase)->sel.empty())
	{
		TDraftDC	dc(hdc);

		dc.Matrix(DataSpaceToScreen());
		HPEN hOldPen = dc.SelectPen(GetStockPen(SEP_OUTLINE));

		gld_rect	bound;
		POINT		box[4];
		TMatrix		smat;

		gld_shape_sel::iterator ishape = THE_BASE(m_pBase)->sel.begin();
		gld_shape_sel::iterator iactive = THE_BASE(m_pBase)->sel.active();

		if (iactive == THE_BASE(m_pBase)->sel.end())
		{
			iactive = THE_BASE(m_pBase)->sel.begin();
		}

		for (; ishape != iactive; ++ishape)
		{
			bound = (*ishape).bound();
			smat = (*ishape).matrix();
			box[0].x = bound.left;
			box[0].y = bound.top;
			box[1].x = bound.right;
			box[1].y = bound.top;
			box[2].x = bound.right;
			box[2].y = bound.bottom;
			box[3].x = bound.left;
			box[3].y = bound.bottom;

			for(int i = 0; i < 4; i++)
			{
				smat.Transform(box[i].x, box[i].y);
			}

			dc.DrawPolygon(box, 4, TDraftDC::_trans_tag());
		}
		// Draw active shape
		dc.SelectPen(GetStockPen(SEP_OUTLINE));

		bound = (*ishape).bound();
		smat = (*ishape).matrix();
		box[0].x = bound.left;
		box[0].y = bound.top;
		box[1].x = bound.right;
		box[1].y = bound.top;
		box[2].x = bound.right;
		box[2].y = bound.bottom;
		box[3].x = bound.left;
		box[3].y = bound.bottom;

		for(int i = 0; i < 4; i++)
		{
			smat.Transform(box[i].x, box[i].y);
		}

		dc.DrawPolygon(box, 4, TDraftDC::_trans_tag());
		
		// Draw other shape
		dc.SelectPen(GetStockPen(SEP_OUTLINE));

		for (++ishape; ishape != THE_BASE(m_pBase)->sel.end(); ++ishape)
		{
			bound = (*ishape).bound();
			smat = (*ishape).matrix();
			box[0].x = bound.left;
			box[0].y = bound.top;
			box[1].x = bound.right;
			box[1].y = bound.top;
			box[2].x = bound.right;
			box[2].y = bound.bottom;
			box[3].x = bound.left;
			box[3].y = bound.bottom;

			for(int i = 0; i < 4; i++)
			{
				smat.Transform(box[i].x, box[i].y);
			}

			dc.DrawPolygon(box, 4, TDraftDC::_trans_tag());
		}
	}
}

void TShapeEditor::OnNotify(void *pThis, int nNotifyID, LPARAM lParam)
{
	TShapeEditor *pObj = (TShapeEditor *)pThis;
	
	if (pObj)
	{
		switch(nNotifyID)
		{
		case SEN_SELECTION_CHANGED:
			pObj->OnSelectionChanged();			
			break;	
		}
	}
}

void TShapeEditor::OnSelectionChanged()
{
	TTool *pTool = CurTool();

	if ((pTool == NULL) || (pTool->Capabilities() & TC_DRAWSELECTION) != TC_DRAWSELECTION)
	{
		Repaint();
	}
}

void TShapeEditor::SetRedraw(bool bRedraw)
{
	THE_BASE(m_pBase)->nRedraw = bRedraw ? max(0, THE_BASE(m_pBase)->nRedraw - 1) : THE_BASE(m_pBase)->nRedraw + 1;

	if (THE_BASE(m_pBase)->nRedraw == 0 && !THE_BASE(m_pBase)->rcRedraw.is_empty())
	{
		Redraw(THE_BASE(m_pBase)->rcRedraw);
	}	
}

void TShapeEditor::SetRepaint(bool bRepaint)
{
	THE_BASE(m_pBase)->nRepaint = bRepaint ? max(0, THE_BASE(m_pBase)->nRepaint - 1) : THE_BASE(m_pBase)->nRepaint + 1;

	if (THE_BASE(m_pBase)->nRepaint == 0 && !THE_BASE(m_pBase)->rcRepaint.is_empty())
	{
		Repaint();
	}	
}

int TShapeEditor::GetRedraw() const
{
	return THE_BASE(m_pBase)->nRedraw;
}

int TShapeEditor::GetRepaint() const
{
	return	THE_BASE(m_pBase)->nRepaint;
}

gld_rect TShapeEditor::GetClientRect()
{
	int cx, cy;

	THE_BASE(m_pBase)->GDevice.GetSize(cx, cy);

	return gld_rect(0, 0, cx - 1, cy - 1);
}

/*
	函数名：
		Activate
	描述：
		改变当前活动Shape；
	参数：
		active : 设置的活动Shape；
*/
void TShapeEditor::ActivateShape(const gld_shape &active)
{
	if (active.validate())
	{	
		THE_BASE(m_pBase)->sel.Activate(active);

		// 发送通知		
		CSubjectManager::Instance()->GetSubject("Select")->Notify(0);	
		TNotifyTransceiver::Notify(this, SEN_SELECTION_CHANGED, 0);
	}
}

/*
	取当前选择集
*/
gld_shape_sel &TShapeEditor::CurSel()
{
	return THE_BASE(m_pBase)->sel;
}

// Grid function
bool TShapeEditor::IsShowGrid() const
{
	return THE_BASE(m_pBase)->bShowGrid;
}

void TShapeEditor::ShowGrid(bool bShow)
{
	if (IsShowGrid() != bShow)
	{
		THE_BASE(m_pBase)->bShowGrid = bShow;

		Redraw();
		Repaint();
	}
}

bool TShapeEditor::IsSnapToGrid() const
{
	return THE_BASE(m_pBase)->bSnapToGrid;
}

void TShapeEditor::SnapToGrid(bool bSnap)
{
	if (IsSnapToGrid() != bSnap)
	{
		THE_BASE(m_pBase)->bSnapToGrid = bSnap;
	}
}

void TShapeEditor::SetGrid(const TShapeEditorGrid &grid)
{
	if (THE_BASE(m_pBase)->Grid != grid)
	{
		THE_BASE(m_pBase)->Grid = grid;
		if (IsShowGrid())
		{
			Redraw();
			Repaint();
		}
	}
}

void TShapeEditor::GetGrid(TShapeEditorGrid *grid)
{
	*grid = THE_BASE(m_pBase)->Grid;
}

// 辅助线函数
int TShapeEditor::AddAuxLine(int pos, bool horz)
{
	TShapeEditorAuxLine	&al = THE_BASE(m_pBase)->auxLine;
	TShapeEditorAuxLine::AuxLines *als = &al.VLines;
	if (horz)
	{
		als = &al.HLines;
	}
	int	index = al.Find(pos, horz);
	if (index == als->size())
	{
		als->push_back(pos);
	}
	else
	{
		if ((*als)[index] == pos)	// already exists
		{
			return -1;
		}
		else
		{
			als->insert(als->begin() + index, pos);
		}
	}

	return index;
}

int TShapeEditor::RemoveAuxLine(int pos, bool horz)
{
	TShapeEditorAuxLine	&al = THE_BASE(m_pBase)->auxLine;
	TShapeEditorAuxLine::AuxLines *als = &al.VLines;
	if (horz)
	{
		als = &al.HLines;
	}
	int	index = al.Find(pos, horz);
	if (index == als->size())
	{
		return -1;
	}
	else
	{
		if ((*als)[index] != pos)	// is existing
		{
			return -1;
		}
		else
		{
			als->erase(als->begin() + index);
		}
	}

	return index;
}

TShapeEditorAuxLine &TShapeEditor::GetAuxLine()
{
	return THE_BASE(m_pBase)->auxLine;
}

bool TShapeEditor::ShowAuxLine()
{
	return THE_BASE(m_pBase)->bShowAuxLine;
}

void TShapeEditor::ShowAuxLine(bool show)
{
	if (THE_BASE(m_pBase)->bShowAuxLine != show)
	{
		THE_BASE(m_pBase)->bShowAuxLine = show;

		Repaint();
	}
}

void TShapeEditor::DrawAuxLine(HDC hdc)
{
	TMatrix	mx = DataSpaceToScreen();
	CSize	szView = GetViewSize();
	HPEN	hOldPen = (HPEN)SelectObject(hdc, GetStockPen(SEP_AUXLINE));
	assert(hOldPen != NULL);

	int tx, ty;
	tx = 0;
	TShapeEditorAuxLine::AuxLines::iterator y = THE_BASE(m_pBase)->auxLine.HLines.begin();	
	for (; y != THE_BASE(m_pBase)->auxLine.HLines.end(); ++y)
	{
		ty = *y;
		mx.Transform(tx, ty);
		ty = TWIPS_TO_PIXEL(ty);
		MoveToEx(hdc, 0, ty, NULL);
		LineTo(hdc, szView.cx, ty);
	}

	ty = 0;
	TShapeEditorAuxLine::AuxLines::iterator x = THE_BASE(m_pBase)->auxLine.VLines.begin();
	for (; x != THE_BASE(m_pBase)->auxLine.VLines.end(); ++x)
	{
		tx = *x;
		mx.Transform(tx, ty);
		tx = TWIPS_TO_PIXEL(tx);
		MoveToEx(hdc, tx, 0, NULL);
		LineTo(hdc, tx, szView.cy);
	}

	SelectObject(hdc, hOldPen);
}

///////////////////////////////////////////////////////////////////////////
TShapeEditorLayout::TShapeEditorLayout()
: fZoom(1.0)
, fOffsetX(0.0)
, fOffsetY(0.0)
, szWorkspace(S_WorkSpaceWidth, S_WorkSpaceHeight)
, nEditMode(EDIT_SCENE_MODE)
{
}

bool TShapeEditorLayout::operator==(const TShapeEditorLayout &other) const
{
	return fabs(fZoom - other.fZoom) < EPSILON &&
		   fabs(fOffsetX - other.fOffsetX) < EPSILON &&
		   fabs(fOffsetY - other.fOffsetY) < EPSILON &&
		   szWorkspace == other.szWorkspace &&
		   nEditMode == other.nEditMode;
}

bool TShapeEditorLayout::operator!=(const TShapeEditorLayout &other) const
{
	return fabs(fZoom - other.fZoom) > EPSILON ||
		   fabs(fOffsetX - other.fOffsetX) > EPSILON ||
		   fabs(fOffsetY - other.fOffsetY) > EPSILON ||
		   szWorkspace != other.szWorkspace ||
		   nEditMode != other.nEditMode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int TShapeEditorAuxLine::Find(int pos, bool horz)
{
	AuxLines *lines = &VLines;

	if (horz)
	{
		lines = &HLines;
	}	

	int first = 0, last = (int)lines->size() - 1;
	int	p, value;

	while (first <= last)
	{
		p = (first + last) / 2;

		value = (*lines)[p];

		if (value == pos)
		{
			first = p;
			break;
		}
		else if (value < pos)
		{			
			first = p + 1;
		}
		else
		{
			last = p - 1;
		}		
	}

	return first;
}

void TShapeEditor::ISelChange::Update(void *pData)
{
	METHOD_PROLOGUE(TShapeEditor, ISelChange)

	if (pThis->GetCurTool() == IDT_PENCILGUIDLINE)
	{
		pThis->SetCurTool(IDT_TRANSFORM);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
TShapeEditor my_app;