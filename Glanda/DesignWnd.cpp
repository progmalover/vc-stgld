// DesignWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DesignWnd.h"

#include "GGraphics.h"
#include "gld_draw.h"
#include "gldCharacter.h"
#include "my_app.h"
#include "Accelerator.h"

#include "gldDataKeeper.h"
#include "gldObj.h"
#include "gldSprite.h"
#include "gldLayer.h"

#include "Observer.h"

#include "GlandaDoc.h"
#include "CmdRemoveInstance.h"
#include "SWFProxy.h"

#include "Clipboard.h"

#include "Options.h"

#include "GroupUpdate.h"
#include "DrawHelper.h"

#include "toolsdef.h"
#include "Tips.h"

#include "MainFrm.h"
#include "DrawHelper.h"

#include "event_code.h"

#include "InstanceActionSheet.h"

#include "EditShapeSheet.h"
#include "EditMCSheet.h"
#include "EditTextSheet.h"
#include "gldSound.h"
#include "CmdAddSound.h"
#include "filepath.h"
#include "EffectCommonUtils.h"
#include "..\GlandaCOM\GLD_ATL.h"
#include "gldEffect.h"
#include "ProfileManager.h"
#include "EffectInplaceEditor.h"
#include "InputBox.h"
#include "filepath.h"
#include "ListTemplate.h"
#include "DlgApplyTemplate.h"
#include "CheckMessageBox.h"
#include "CmdChangeButtonCap.h"
#include "DlgChangeButtonText.h"
#include "Background.h"
#include "GlandaClipboard.h"
#include "DlgSelectBackgrndPos.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern UINT CF_GMOVIE_TEXT2;
inline SETextToolEx *GET_TEXTTOOL()
{
	return (SETextToolEx *)my_app.Tools()[IDT_TEXTTOOLEX - IDT_FIRST];
}

class CToolShortcutKey
{
public:
	static UINT m_Key[];

public:
	static bool OnKey(UINT nKey)
	{
		for (int i = 0; i < IDT_LAST + 1; i++)
		{
			if (nKey == m_Key[i])
			{
				my_app.SetCurTool(i + IDT_FIRST);

				return true;
			}
		}

		return false;
	}
};

UINT CToolShortcutKey::m_Key[] = 
{
	// transform, reshape, line, oval, rect, pencil, pen, 
	// inkbottle, paintbucket, transform fill, guid line pen, zoom out, zoom in, hand,
	// polygon, round rect, star, guid line line, guid line polygon, guid line oval, guid line pen
	// text
	// correspond to A ~ U
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x50, 0x51, 0x52, 0x53, 0x54, 
	0x55, 0x56, 0x57, 0x58, 0x59, 0x60, 0x61,
	0x62
};

// resize movie tool
static bool PtOnObject(const CPoint &pt)
{
	int x = PIXEL_TO_TWIPS(pt.x);
	int y = PIXEL_TO_TWIPS(pt.y);

	if (my_app.PtOnShape(x, y).validate())
		return true;

	my_app.ScreenToDataSpace(x, y);

	if (my_app.GetGuidLine(pt.x, pt.y) != NULL)
		return true;

	return false;
}

class CResizeMovieTool : public TEvent
{
public:
	CResizeMovieTool();
	virtual ~CResizeMovieTool();

	enum
	{
		UNKNOWN_BORDER,
		RIGHT_BORDER,
		BOTTOM_BORDER,
		RB_BORDER,
	};

	static int PtOnBorder(const POINT &pt);
	
	void SetDesignWnd(CWnd *pWnd);
	
	virtual POINT GetPoint(const POINT &p);

protected:
	virtual void OnLButtonDown(UINT nFlags, const POINT &point);
	virtual void OnBeginDrag(UINT nFlags, const POINT &point);
	virtual void OnDragOver(UINT nFlags, const POINT &point);
	virtual void OnCancelDrag(UINT nFlags, const POINT &point);
	virtual void OnDragDrop(UINT nFlags, const POINT &point);
	virtual bool OnSetCursor(UINT nFlags, const POINT &point);

protected:
	void PrepareDC();
	void UnprepareDC();
	void DrawEffect(int x, int y, int border);
	void NormalizePoint(int &x, int &y);

	CDC		*m_pDC;
	CPen	m_DragPen;
	CPen	*m_OldPen;
	int		m_OldR2;
	CWnd	*m_DesignWnd;
	int		m_PrevX;
	int		m_PrevY;
	int		m_Border;
};

CResizeMovieTool::CResizeMovieTool()
{
	m_pDC = NULL;
	m_OldR2 = 0;
	m_DragPen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	m_OldPen = NULL;
	m_DesignWnd = NULL;
	m_PrevX = 0;
	m_PrevY = 0;
	m_Border = UNKNOWN_BORDER;
}

CResizeMovieTool::~CResizeMovieTool()
{
}

void CResizeMovieTool::SetDesignWnd(CWnd *pWnd)
{
	ASSERT(pWnd != NULL);

	m_DesignWnd = pWnd;

	Hwnd(pWnd->m_hWnd);
}

void CResizeMovieTool::PrepareDC()
{
	ASSERT(m_DesignWnd != NULL);

	m_pDC = m_DesignWnd->GetDC();	
	ASSERT(m_pDC != NULL);
	m_OldR2 = m_pDC->SetROP2(R2_NOTXORPEN);
	m_OldPen = (CPen *)m_pDC->SelectObject(&m_DragPen);
	ASSERT(m_OldPen != NULL);
}

void CResizeMovieTool::UnprepareDC()
{
	ASSERT(m_DesignWnd != NULL);
	ASSERT(m_pDC != NULL);

	m_pDC->SetROP2(m_OldR2);
	m_pDC->SelectObject(m_OldPen);
	m_DesignWnd->ReleaseDC(m_pDC);
	m_pDC = NULL;
}

POINT CResizeMovieTool::GetPoint(const POINT &p)
{	
	return p;
}

void CResizeMovieTool::OnLButtonDown(UINT nFlags, const POINT &point)
{
	m_Border = PtOnBorder(point);

	if (m_Border != UNKNOWN_BORDER)
		Track(point);	
}

void CResizeMovieTool::OnBeginDrag(UINT nFlags, const POINT &point)
{
	ASSERT(m_Border != UNKNOWN_BORDER);

	PrepareDC();

	m_PrevX = point.x;
	m_PrevY = point.y;

	DrawEffect(m_PrevX, m_PrevY, m_Border);
}

void CResizeMovieTool::NormalizePoint(int &x, int &y)
{
	x = PIXEL_TO_TWIPS(x);
	y = PIXEL_TO_TWIPS(y);
	my_app.ScreenToDataSpace(x, y);	
	if (x < IDI_MIN_MOVIE_WIDTH * 20)
		x = IDI_MIN_MOVIE_WIDTH * 20;
	if (x > IDI_MAX_MOVIE_WIDTH * 20)
		x = IDI_MAX_MOVIE_WIDTH * 20;
	if (y < IDI_MIN_MOVIE_HEIGHT * 20)
		y = IDI_MIN_MOVIE_HEIGHT * 20;
	if (y > IDI_MAX_MOVIE_HEIGHT * 20)
		y = IDI_MAX_MOVIE_HEIGHT * 20;
	my_app.DataSpaceToScreen(x, y);
	x = TWIPS_TO_PIXEL(x);
	y = TWIPS_TO_PIXEL(y);
}

void CResizeMovieTool::OnDragOver(UINT nFlags, const POINT &point)
{
	DrawEffect(m_PrevX, m_PrevY, m_Border);

	int x = point.x;
	int y = point.y;
	
	NormalizePoint(x, y);

	m_PrevX = x;
	m_PrevY = y;

	DrawEffect(m_PrevX, m_PrevY, m_Border);
}

void CResizeMovieTool::OnCancelDrag(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	m_DesignWnd->Invalidate(FALSE);
}

void CResizeMovieTool::OnDragDrop(UINT nFlags, const POINT &point)
{
	UnprepareDC();

	gldMainMovie2 *mv = _GetMainMovie2();
	CSize szMovie = my_app.GetMovieSize();
	
	int x = PIXEL_TO_TWIPS(m_PrevX);
	int y = PIXEL_TO_TWIPS(m_PrevY);
	my_app.ScreenToDataSpace(x, y);
	x = (int)(x / 20.0 + 0.5);
	y = (int)(y / 20.0 + 0.5);

	if (m_Border == RIGHT_BORDER)	
		szMovie.cx = x;
	else if (m_Border == BOTTOM_BORDER)
		szMovie.cy = y;
	else
	{
		szMovie.cx = x;
		szMovie.cy = y;
	}

	my_app.Commands().Do(new CCmdChangeMovieProperties(
		mv->m_frameRate,szMovie.cx, szMovie.cy, mv->m_color));
}

void CResizeMovieTool::DrawEffect(int x, int y, int border)
{
	ASSERT(m_DesignWnd != NULL);
	ASSERT(m_pDC != NULL);

	CRect	rc;
	m_DesignWnd->GetClientRect(&rc);

	if (border == RIGHT_BORDER)
	{
		m_pDC->MoveTo(x, 0);
		m_pDC->LineTo(x, rc.bottom);
	}
	else if (border == BOTTOM_BORDER)
	{
		m_pDC->MoveTo(0, y);
		m_pDC->LineTo(rc.right, y);
	}
	else
	{
		m_pDC->MoveTo(x, 0);
		m_pDC->LineTo(x, rc.bottom);
		m_pDC->MoveTo(0, y);
		m_pDC->LineTo(rc.right, y);
	}
}

bool CResizeMovieTool::OnSetCursor(UINT nFlags, const POINT &point)
{
	int		border = PtOnBorder(point);
	HCURSOR cursor = NULL;

	if (border == RIGHT_BORDER)
		cursor = my_app.GetStockCursor(IDC_G_WE_STRETCH);
	else if (border == BOTTOM_BORDER)
		cursor = my_app.GetStockCursor(IDC_G_NS_STRETCH);
	else if (border == RB_BORDER)
		cursor = my_app.GetStockCursor(IDC_G_WN_STRETCH);
	
	if (cursor != NULL)
		return ::SetCursor(cursor) != NULL;
	else
		return false;
}

int CResizeMovieTool::PtOnBorder(const POINT &pt)
{
	CSize szMovie = my_app.GetMovieSize();	
	szMovie.cx = PIXEL_TO_TWIPS(szMovie.cx);
	szMovie.cy = PIXEL_TO_TWIPS(szMovie.cy);

	int x = PIXEL_TO_TWIPS(pt.x);
	int y = PIXEL_TO_TWIPS(pt.y);
	
	my_app.ScreenToDataSpace(x, y);
	
	double err = 8000.0 / my_app.GetZoomPercent();
	int dx = abs(x - szMovie.cx);
	int dy = abs(y - szMovie.cy);

	if (dx < err && dy < err)
		return RB_BORDER;
	else if (dx < err && y > -1 && y < szMovie.cy + 1)
		return RIGHT_BORDER;
	else if (dy < err && x > -1 && x < szMovie.cx + 1)
		return BOTTOM_BORDER;
	else
		return UNKNOWN_BORDER;
}

CResizeMovieTool	sResizeMovieTool;

// CDesignWnd

const int SCROLL_LINE_SIZE = 10;

IMPLEMENT_SINGLETON(CDesignWnd)
IMPLEMENT_DYNAMIC(CDesignWnd, CWnd)
CDesignWnd::CDesignWnd()
{
	m_bInit = FALSE;
	m_hAccel = NULL;
	m_pObj = NULL;
	m_bEraseBkgnd = FALSE;
}

CDesignWnd::~CDesignWnd()
{
}


BEGIN_MESSAGE_MAP(CDesignWnd, CWnd)

	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)

	ON_COMMAND(ID_TEXT_DELETEBACK, OnTextDeleteBack)
	ON_UPDATE_COMMAND_UI(ID_TEXT_DELETEBACK, OnUpdateTextDeleteBack)
	ON_COMMAND(ID_TEXT_UNDO, OnTextUndo)
	ON_UPDATE_COMMAND_UI(ID_TEXT_UNDO, OnUpdateTextUndo)
	ON_COMMAND(ID_TEXT_REDO, OnTextRedo)
	ON_UPDATE_COMMAND_UI(ID_TEXT_REDO, OnUpdateTextRedo)
	ON_COMMAND(ID_TEXT_GOTO_BEGIN_OF_TEXT, OnTextGotoBeginOfText)
	ON_COMMAND(ID_TEXT_GOTO_END_OF_TEXT, OnTextGotoEndOfText)

	ON_MESSAGE(UM_BEGIN_DRAG, OnBeginDrag)
	ON_MESSAGE(UM_CANCEL_DRAG, OnCancelDrag)
	ON_MESSAGE(UM_DRAG_OVER, OnDragOver)
	ON_MESSAGE(UM_DRAG_DROP, OnDragDrop)

	ON_WM_DROPFILES()

	ON_COMMAND(ID_EDIT_INSTANCE_ACTIONSCRIPT, OnEditActionScript)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSTANCE_ACTIONSCRIPT, OnUpdateEditActionScript)

	ON_COMMAND(ID_EDIT_INSTANCE_PROPERTIES, OnEditInstanceProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSTANCE_PROPERTIES, OnUpdateEditInstanceProperties)

	ON_COMMAND(ID_EDIT_PREV_INSTANCE, OnEditPrevInstance)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PREV_INSTANCE, OnUpdateEditPrevInstance)
	ON_COMMAND(ID_EDIT_NEXT_INSTANCE, OnEditNextInstance)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEXT_INSTANCE, OnUpdateEditNextInstance)
	ON_COMMAND(ID_ELEMENT_SAVEASTEMPLATE, OnSaveElementAsTemplate)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_SAVEASTEMPLATE, OnUpdateSaveElementAsTemplate)
	ON_COMMAND(ID_ELEMENT_APPLYTEMPLATE, OnApplyTemplateToElement)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_APPLYTEMPLATE, OnUpdateApplyTemplateToElement)
	ON_COMMAND(ID_CHANGE_BUTTON_TEXT, OnChangeButtonText)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_BUTTON_TEXT, OnUpdateChangeButtonText)

	ON_COMMAND(ID_SETASBACKGROUND, OnSetAsBackground)
	ON_UPDATE_COMMAND_UI(ID_SETASBACKGROUND, OnUpdateSetAsBackground)

	ON_COMMAND(ID_CHANGEBACKGROUND_EMPTY, OnEmptyBackground)
	ON_UPDATE_COMMAND_UI(ID_CHANGEBACKGROUND_EMPTY, OnUpdateEmptyBackground)
	ON_COMMAND(ID_CHANGEBACKGROUND_RESTORE, OnRestoreBackground)
	ON_UPDATE_COMMAND_UI(ID_CHANGEBACKGROUND_RESTORE, OnUpdateRestoreBackground)
	ON_COMMAND(ID_CHANGEBACKGROUND_POSITION, OnChangeBackgroundPos)
	ON_UPDATE_COMMAND_UI(ID_CHANGEBACKGROUND_POSITION, OnUpdateChangeBackgroundPos)

	//ON_COMMAND(ID_TOOLS_TEST, OnTest)

END_MESSAGE_MAP()



// CDesignWnd message handlers

BOOL CDesignWnd::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
	LPCTSTR lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, 0);
	BOOL ret = CWnd::Create(lpszClass, "DesignWnd", dwStyle, rect, pParentWnd, nID);
	if (ret)
	{
		m_MoveRuleTool.Hwnd(m_hWnd);
		m_MoveRuleTool.SetDesignWnd(this);
	}
	return ret;
}

void CDesignWnd::OnPaint()
{
	// do not paint before InitScrollPos is called, 
	// or users will see the cavans with incorrect position.
	if (m_bInit)
	{
		CPaintDC dc(this); // device context for painting
		// TODO: Add your message handler code here
		// Do not call CWnd::OnPaint() for painting messages

		if (m_bEraseBkgnd)
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			dc.FillSolidRect(&rcClient, my_app.GetWorkspaceBkColor());
		}

		my_app.Repaint(dc.m_hDC);
	}
}

void CDesignWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_bInit && m_hWnd)
		my_app.SetViewSize(cx, cy);
}

int CDesignWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_DESIGNWND));

	SCROLLINFO	si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 10;
	si.nPage = 1;
	si.nPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);
	SetScrollInfo(SB_VERT, &si, TRUE);

	m_tooltip.Create(this, TTS_ALWAYSTIP);
	m_tooltip.SetDelayTime(TTDT_INITIAL, 200);
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, 1000 * 30);
	m_tooltip.SetMaxTipWidth(INT_MAX);

	sResizeMovieTool.SetDesignWnd(this);

	::DragAcceptFiles(m_hWnd, TRUE);

	return 0;
}

void CDesignWnd::Reset()
{
	m_bInit = FALSE;

	my_app.SetDesignWnd(m_hWnd);

	CRect rc;
	GetClientRect(&rc);
	my_app.SetViewSize(rc.Width(), rc.Height());

	TShapeEditorLayout layout;	
	my_app.GetDefaultLayout(&layout, EDIT_SCENE_MODE);	
	my_app.SetLayout(&layout, FALSE);

	m_bInit = TRUE;
	Invalidate(FALSE);
}

void CDesignWnd::RecalcScrollPos()
{
	CRect rc;
	GetClientRect(&rc);

	CSize size = my_app.GetWorkSpaceSize();
	int cx = size.cx - 1;
	int cy = size.cy - 1;
	int nPosX = GetScrollPos(SB_HORZ);;
	int nPosY = GetScrollPos(SB_VERT);;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	si.nMin = 0;
	si.nMax = cx;
	si.nPage = rc.Width();
	si.nPos = nPosX;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	si.nMin = 0;
	si.nMax = cy;
	si.nPage = rc.Height();
	si.nPos = nPosY;
	SetScrollInfo(SB_VERT, &si, TRUE);
}

void CDesignWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	GetClientRect(&rc);

	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si, SIF_ALL);
	//ASSERT(si.nMax == my_app.WorkSpaceSize().cx - 1);
	int nMin = si.nMin;
	int nMax = si.nMax;
	int nPage = si.nPage;
	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	CRect rcInvalid(0, 0, 0, 0);

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = min(nMax - nPosOld, SCROLL_LINE_SIZE);
		nPosNew = nPosOld + ds;
		rcInvalid = rc;
		break;
	case SB_LINEUP:
		ds = min(nPosOld, SCROLL_LINE_SIZE);
		nPosNew = nPosOld - ds;
		rcInvalid = rc;
		break;
	case SB_PAGEDOWN:
		ds = min(nMax - nPosOld, nPage);
		nPosNew = nPosOld + ds;
		rcInvalid = rc;
		break;
	case SB_PAGEUP:
		ds = min(nPosOld, nPage);
		nPosNew = nPosOld - ds;
		rcInvalid = rc;
		break;
	case SB_TOP:
		nPosNew = 0;
		rcInvalid = rc;
		break;
	case SB_BOTTOM:
		nPosNew = nMax;
		rcInvalid = rc;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ds = nPos - nPosOld;
		nPosNew = nPosOld + ds;
		rcInvalid = rc;
		break;
	default:
		return;
	}

	ASSERT(nPosNew >= nMin && nPosNew <= nMax);
	nPosNew = min(nMax, max(nMin, nPosNew));

	if (nPosNew != nPosOld)
	{
		si.fMask = SIF_POS;
		si.nPos = nPosNew;
		SetScrollInfo(SB_HORZ, &si, TRUE);

		my_app.HorzScroll(nPosNew - nPosOld);
		Invalidate(FALSE);
		UpdateWindow();
		/*if (!rcInvalid.IsRectEmpty())
		{			
			InvalidateRect(&rcInvalid, FALSE);
			UpdateWindow();
		}*/
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDesignWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	GetClientRect(&rc);

	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si, SIF_ALL);
	
	int nMin = si.nMin;
	int nMax = si.nMax;
	int nPage = si.nPage;
	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	CRect rcInvalid(0, 0, 0, 0);

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = min(nMax - nPosOld, SCROLL_LINE_SIZE);
		nPosNew = nPosOld + ds;
		rcInvalid = rc;
		break;
	case SB_LINEUP:
		ds = min(nPosOld, SCROLL_LINE_SIZE);
		nPosNew = nPosOld - ds;
		rcInvalid = rc;
		break;
	case SB_PAGEDOWN:
		ds = min(nMax - nPosOld, nPage);
		nPosNew = nPosOld + ds;
		rcInvalid = rc;
		break;
	case SB_PAGEUP:
		ds = min(nPosOld, nPage);
		nPosNew = nPosOld - ds;
		rcInvalid = rc;
		break;
	case SB_TOP:
		nPosNew = 0;
		rcInvalid = rc;
		break;
	case SB_BOTTOM:
		nPosNew = nMax;
		rcInvalid = rc;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ds = nPos - nPosOld;
		nPosNew = nPosOld + ds;
		rcInvalid = rc;
		break;
	default:
		return;
	}

	ASSERT(nPosNew >= nMin && nPosNew <= nMax);
	nPosNew = min(nMax, max(nMin, nPosNew));

	if (nPosNew != nPosOld)
	{
		my_app.VertScroll(nPosNew - nPosOld);
		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDesignWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
#ifdef _DEBUG
	int nTick = GetTickCount();
	for (int  i = 0; i < 20; i++)
		SendMessage(WM_VSCROLL, SB_LINEUP, NULL);
	AfxMessageBoxEx(0, "%d", GetTickCount() - nTick);
#endif

	CWnd::OnMButtonDown(nFlags, point);
}

void CDesignWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		int x = PIXEL_TO_TWIPS(point.x);
		int y = PIXEL_TO_TWIPS(point.y);
		my_app.ScreenToDataSpace(x, y);
		pTool->teMatrix.Invert().Transform(x, y);
		if (PtInRect(&pTool->m_rtRegion, CPoint(x, y)))
		{
			CWnd::OnRButtonDown(nFlags, point);
			return;
		}
	}

	CGuardDrawOnce	xDraw;

	if (!my_app.CurTool()->RButtonDown(nFlags, point))
	{	
		my_app.SetCurTool(IDT_TRANSFORM);	// 如果不切换到选择工具对Reshape工具可能某些Shape不能选择

		int	x = PIXEL_TO_TWIPS(point.x);
		int y = PIXEL_TO_TWIPS(point.y);
		gld_shape shape = my_app.PtOnShape(x, y);
		if (shape.validate())
		{
			if (!my_app.CurSel().exist(shape))
			{
				gld_shape_sel sel;
				sel.select(shape);
				my_app.DoSelect(sel);
			}
		}
	
		CWnd::OnRButtonDown(nFlags, point);	
	}
	else
	{
		SetCapture();
	}
}

BOOL CDesignWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

BOOL CDesignWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si, SIF_ALL);
	int ds = zDelta / WHEEL_DELTA * SCROLL_LINE_SIZE;

	int nPosNew = max(si.nMin, min(si.nMax, si.nPos - ds));
	if (nPosNew != si.nPos)
	{
		si.fMask = SIF_POS;
		si.nPos = nPosNew;
		SetScrollInfo(SB_VERT, &si, TRUE);

		my_app.VertScroll(-ds);
		Invalidate(FALSE);
		UpdateWindow();

		return TRUE;
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

int CDesignWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

#define MAX_RULE_SNAP	60

bool PtOnAuxLine(const CPoint &point, int &pos, bool &horz)
{
	if (my_app.Ready() &&
		my_app.ShowAuxLine() && 
		(my_app.CurTool()->Capabilities() & TC_DRAWSELECTION) != 0)
	{
		int maxErr = MAX_RULE_SNAP * 100 / my_app.GetZoomPercent();
		int x = PIXEL_TO_TWIPS(point.x), y = PIXEL_TO_TWIPS(point.y);
		my_app.ScreenToDataSpace(x, y);
		TShapeEditorAuxLine &al = my_app.GetAuxLine();		
		int	hError = maxErr + 20;
		int vError = maxErr + 20;
		int	hPos = 0;
		int vPos = 0;

		if (al.HLines.size() > 0)
		{
			int	hIndex = al.Find(y, true);
			if (hIndex == 0)
			{
				hError = al.HLines[0] - y;
				hPos = al.HLines[0];
			}
			else if (hIndex == al.HLines.size())
			{
				hError = y - al.HLines.back();
				hPos = al.HLines.back();
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
			int	vIndex = al.Find(x, false);
			if (vIndex == 0)
			{
				vError = al.VLines[0] - x;
				vPos = al.VLines[0];
			}
			else if (vIndex == al.VLines.size())
			{
				vError = x - al.VLines.back();
				vPos = al.VLines.back();
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

		if ((hError > vError) && (vError < maxErr))
		{
			pos = vPos;
			horz = false;			
			
			return true;
		}
		else if (hError < maxErr)
		{
			pos = hPos;
			horz = true;

			return true;
		}
	}

	return false;
}

void CDesignWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	// TODO: Add your message handler code here and/or call default

	int pos = 0;
	bool horz = false;

	if (PtOnAuxLine(point, pos, horz))
	{
		m_MoveRuleTool.SetTarget(pos, horz);
		m_MoveRuleTool.Track(point.x, point.y);
		return;
	}

	POINT pt = {point.x, point.y};

	if (EffectInplaceEditor::Instance()->HitTest(point))
	{
		EffectInplaceEditor::Instance()->Track(point);
		return;
	}
	
	if (!PtOnObject(point))
	{
		if (sResizeMovieTool.PtOnBorder(sResizeMovieTool.GetPoint(pt)))
		{
			sResizeMovieTool.LButtonDown(nFlags, pt);
			return;
		}		
	}

	my_app.CurTool()->LButtonDown(nFlags, point);

	int nTool = my_app.GetCurTool();
	if (nTool == IDT_PEN || nTool == IDT_PENGUIDLINE)
		ShowPenToolTip(point, nTool == IDT_PEN);

	/*
	gldMovieClip *pMC = _GetCurMovieClip();
	gldLayer *pLayer = pMC->m_curLayer;
	if (pLayer)
	{
		if ((my_app.CurTool()->Capabilities() & TC_ADDSHAPE) != 0)
		{
			if (pLayer == pMC->m_layerList[0])
			{
				ShowNoDrawStreamSoundLayerTip();
				return;
			}

			if (pLayer->m_locked)
			{
				ShowNoDrawLayerLockedTip(pLayer);
				return;
			}

			if (!pLayer->m_visible)
			{
				ShowNoDrawLayerInvisibleTip(pLayer);
				return;
			}

			gldFrameClip *pFC = pLayer->GetFrameClip(pMC->m_curTime);
			if (!pFC)
			{
				ShowNoDrawNoFrameClipTip(pLayer, pMC->m_curTime);
				return;
			}
		}

		my_app.CurTool()->LButtonDown(nFlags, point);

		int nTool = my_app.GetCurTool();
		if (nTool == IDT_PEN || nTool == IDT_PENGUIDLINE)
			ShowPenToolTip(point, nTool == IDT_PEN);
	}
	*/
}

void CDesignWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	my_app.CurTool()->LButtonUp(nFlags, point);

	CWnd::OnLButtonUp(nFlags, point);
}

/*
void CDesignWnd::OnEditDelete()
{
	// TODO: Add your command handler code here
	
	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Key_Del();
	}
	else
	{
		my_app.SetRedraw(false);
		my_app.SetRepaint(false);
		if (my_app.CurSel().count() > 0)
		{			
			my_app.Commands().Do(new CCmdRemoveInstance(my_app.CurSel()));
		}
		if (my_app.GetGuidLine() != NULL)
		{
			my_app.Commands().Do(new CCmdRemoveGuidLine(my_app.GetGuidLine()));
		}
		my_app.SetRedraw(true);
		my_app.SetRepaint(true);
	}
}

void CDesignWnd::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(my_app.CurSel().count() > 0 || my_app.GetGuidLine() != NULL);
	}
}
*/

BOOL CDesignWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if (nHitTest == HTCLIENT)
	{
		int pos = 0;
		bool horz = false;
		CPoint point;		

		VERIFY(GetCursorPos(&point));
		ScreenToClient(&point);

		if (PtOnAuxLine(point, pos, horz))
		{
			//因线尺改变图标
			if (::SetCursor(my_app.GetStockCursor(horz ? IDC_G_VRULER : IDC_G_HRULER)))
				return TRUE;
		}
		else 
		{
			if (EffectInplaceEditor::Instance()->SetCursor(point))
				return TRUE;

			if (!PtOnObject(point))
			{
				if (sResizeMovieTool.SetCursor(pWnd->GetSafeHwnd(), nHitTest, message))
					return TRUE;				
			}
			
			if (my_app.CurTool()->SetCursor(pWnd->GetSafeHwnd(), nHitTest, message))
				return TRUE;			
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

// 更新滚动条的显示状态
void CDesignWnd::RefreshScrollBar()
{
	/*
	CRect	cr;
	int		cw, ch;
	GetClientRect(cr);
	cw = cr.Width();
	ch = cr.Height();

	CSize	ws = my_app.GetWorkSpaceSize();
	int		maxhscroll = ws.cx - cw;
	int		maxvscroll = ws.cy - ch;

	int		vporgx, vporgy;
	my_app.GetViewPortOrg(vporgx, vporgy);	
	
	if (maxhscroll < 0)
		ShowScrollBar(SB_HORZ, FALSE);
	else {
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si, SIF_ALL);
		if (si.nMax != maxhscroll || si.nPos != vporgx) {
			si.nMax = maxhscroll;
			si.nPos = vporgx;
			SetScrollInfo(SB_HORZ, &si, FALSE);
		}
		ShowScrollBar(SB_HORZ, TRUE);
	}

	if (maxvscroll < 0)
		ShowScrollBar(SB_VERT, FALSE);
	else {
		SCROLLINFO si;
		GetScrollInfo(SB_VERT, &si, SIF_ALL);
		if (si.nMax != maxvscroll || si.nPos != vporgy) {
			si.nMax = maxvscroll;
			si.nPos = vporgy;
			SetScrollInfo(SB_VERT, &si, FALSE);
		}
		ShowScrollBar(SB_VERT, TRUE);
	}
	*/
}

#ifdef _DEBUG
void CDesignWnd::OnDebugTestdrawing()
{
	// TODO: Add your command handler code here

	DWORD dwTick = GetTickCount();
	for (int i = 0; i < 100; i++)
	{
		my_app.Redraw();
		Invalidate(FALSE);
		UpdateWindow();
	}

	AfxMessageBoxEx(0, "FPS: %f", (float)100 * 1000 / (GetTickCount() - dwTick));
}
#endif

void CDesignWnd::EditInstance(gld_shape *pShape)
{
	gldInstance* pInstance = CTraitInstance(*pShape);
	gldObj* pObj = pInstance->m_obj;

	my_app.SetCurTool(IDT_TRANSFORM);
	gld_shape_sel sel;
	sel.select(*pShape);
	my_app.DoSelect(sel);

	if(pObj->IsGObjInstanceOf(gobjShape))
	{
		CEditShapeSheet editShapeSheet(pInstance);
		editShapeSheet.DoModal();
	}
	else if(pObj->IsGObjInstanceOf(gobjButton))
	{
		//CEditShapeSheet editShapeSheet(pInstance);
		//editShapeSheet.DoModal();
		CEditMCSheet EditMCSheet(pInstance);
		EditMCSheet.DoModal();
	}
	else if(pObj->IsGObjInstanceOf(gobjSprite))
	{
		//CEditShapeSheet editShapeSheet(pInstance);
		//editShapeSheet.DoModal();
		CEditMCSheet EditMCSheet(pInstance);
		EditMCSheet.DoModal();
	}
	else if(pObj->IsGObjInstanceOf(gobjText))
	{
		//CEditShapeSheet editShapeSheet(pInstance);
		//editShapeSheet.DoModal();
		CEditTextSheet editTextSheet(pInstance);
		editTextSheet.DoModal();
	}
	else
	{
	}
}

void CDesignWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int nTool = my_app.GetCurTool();
	if (nTool == IDT_PEN || nTool == IDT_PENGUIDLINE || (nTool == IDT_TEXTTOOLEX && GET_TEXTTOOL()->bEdit))
	{
		// do nothing
	}
	else
	{
		int	x = PIXEL_TO_TWIPS(point.x);
		int y = PIXEL_TO_TWIPS(point.y);
		
		if (EffectInplaceEditor::Instance()->PtOnPath(CPoint(x, y)))
		{
			gldInstance *pInst = _GetCurInstance();
			gldEffect *pEffect = _GetCurEffect();
			if (pInst != NULL && pEffect != NULL)
			{
				pInst->ConfigEffect(pInst, pEffect);
				return;
			}
		}

		gld_shape shape = my_app.PtOnShape(x, y);
		if (shape.validate())
		{
			gldObj *pObj = CTraitInstance(shape);

			if (pObj->IsGObjInstanceOf(gobjText))
			{
				// if is text then set text tool to inplace edit state
				if (my_app.GetCurTool() != IDT_TEXTTOOLEX)
				{
					my_app.SetCurTool(IDT_TEXTTOOLEX);

					ASSERT(my_app.CurTool() != NULL);
				}

				ASSERT(my_app.GetCurTool() == IDT_TEXTTOOLEX);

				my_app.ScreenToDataSpace(x, y);

				SETextToolEx *pTool = (SETextToolEx *)my_app.CurTool();
				pTool->StartEditText(CTraitInstance(shape), &CPoint(x, y));
			}
			else
			{
				EditInstance(&shape);
			}
			return;
		}		
	}

	if (my_app.GetCurTool() > -1)
		my_app.Tools()[my_app.GetCurTool()]->LButtonDbClick(nFlags, point);

	CWnd::OnLButtonDblClk(nFlags, point);
}

const int DOT_SIZE = 2;

void DrawLineH(CDC *pDC, int x1, int x2, int y)
{
	if (x1 != x2)
	{
		if (x1 > x2)
		{
			int x3 = x1;
			x1 = x2;
			x2 = x3;
		}

		int x = x1;
		while (x < x2)
		{
			pDC->MoveTo(x, y);
			x+= DOT_SIZE;
			if (x > x2)
				x = x2;
			pDC->LineTo(x, y);
			x+= DOT_SIZE;
			if (x > x2)
				x = x2;
		}
	}
}

void DrawLineV(CDC *pDC, int x, int y1, int y2)
{
	if (y1 != y2)
	{
		if (y1 > y2)
		{
			int y3 = y1;
			y1 = y2;
			y2 = y3;
		}

		int y = y1;
		while (y < y2)
		{
			pDC->MoveTo(x, y);
			y+= DOT_SIZE;
			if (y > y2)
				y = y2;
			pDC->LineTo(x, y);
			y+= DOT_SIZE;
			if (y > y2)
				y = y2;
		}
	}
}

void DrawRect(CDC *pDC, CRect &rc)
{
	//pDC->MoveTo(rc.left, rc.top);
	//pDC->LineTo(rc.right - 1, rc.top);
	//pDC->LineTo(rc.right - 1, rc.bottom - 1);
	//pDC->LineTo(rc.left, rc.bottom - 1);
	//pDC->LineTo(rc.left, rc.top);

	DrawLineV(pDC, rc.left, rc.top, rc.bottom);
	DrawLineH(pDC, rc.left, rc.right, rc.top);
	DrawLineV(pDC, rc.right, rc.top, rc.bottom);
	DrawLineH(pDC, rc.left, rc.right, rc.bottom);
}

/*
void CDesignWnd::DrawAnimatedRects(CRect *pRectStart, CRect *pRectEnd)
{
	if (!COptions::Instance()->m_bShowAnimateRects)
		return;
	
	CRect rcStart, rcEnd;
	if (pRectEnd == NULL)
	{
		GetClientRect(&rcEnd);
		ClientToScreen(&rcEnd);
		pRectEnd = &rcEnd;
	}

	if (pRectStart == NULL)
	{
		pRectStart = &rcStart;
		pRectStart->SetRect(pRectEnd->CenterPoint().x - 1, pRectEnd->CenterPoint().y - 1, pRectEnd->CenterPoint().x + 1, pRectEnd->CenterPoint().y + 1);
	}

	CDC dc;
	dc.CreateDC("DISPLAY", NULL, NULL, NULL);

	int nROP2 = dc.SetROP2(R2_NOTXORPEN);

	CPen pen(PS_SOLID, 1, RGB(128, 128, 128));
	CPen *pPenOld = dc.SelectObject(&pen);

	int left = pRectEnd->left - pRectStart->left;
	int top = pRectEnd->top - pRectStart->top;
	int right = pRectEnd->right - pRectStart->right;
	int bottom = pRectEnd->bottom - pRectStart->bottom;

	const int times = 10;
	const start = 1;
	const delta = 1;

	int left1 = (abs(left) - delta * (1 + times) * times / 2) / times;
	int top1 = (abs(top) - delta * (1 + times) * times / 2) / times;
	int right1 = (abs(right) - delta * (1 + times) * times / 2) / times;
	int bottom1 = (abs(bottom) - delta * (1 + times) * times / 2) / times;

	left = left > 0 ? max(start, left1) : min(-start, -left1);
	top = top > 0 ? max(start, top1) : min(-start, -top1);
	right = right > 0 ? max(start, right1) : min(-start, -right1);
	bottom = bottom > 0 ? max(start, bottom1) : min(-start, -bottom1);

	for (int i = 0; i < times; i++)
	{
		DrawRect(&dc, *pRectStart);
		Sleep(20);
		DrawRect(&dc, *pRectStart);
		Sleep(1);

		pRectStart->left += left; pRectStart->left = (left > 0 ? min(pRectEnd->left, pRectStart->left) : max(pRectEnd->left, pRectStart->left));
		pRectStart->top += top; pRectStart->top = (top > 0 ? min(pRectEnd->top, pRectStart->top) : max(pRectEnd->top, pRectStart->top));
		pRectStart->right += right; pRectStart->right = (right > 0 ? min(pRectEnd->right, pRectStart->right) : max(pRectEnd->right, pRectStart->right));
		pRectStart->bottom += bottom; pRectStart->bottom = (bottom > 0 ? min(pRectEnd->bottom, pRectStart->bottom) : max(pRectEnd->bottom, pRectStart->bottom));

		if (*pRectStart == *pRectEnd)
			break;

		left += left > 0 ? delta : -delta;
		top += top > 0 ? delta : -delta;
		right += right > 0 ? delta : -delta;
		bottom += bottom > 0 ? delta : -delta;
	}

	dc.SelectObject(pPenOld);

	dc.SetROP2(nROP2);
}
*/

void CDesignWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here

	CRect rc;

	GetClientRect(rc);
	ClientToScreen(&rc);

	if (rc.PtInRect(point))
	{
		SETextToolEx *pTool = GET_TEXTTOOL();

		CMenu menu;
		if (menu.LoadMenu(pTool->IsActive() ? IDR_TEXT2 : IDR_DESIGNWND))
		{
			CMenu *pPopup = menu.GetSubMenu(0);
			if (pPopup)
				pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
		}
	}
}

void CDesignWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	my_app.CurTool()->RButtonUp(nFlags, point);

	if (GetCapture() != this)
		CWnd::OnRButtonUp(nFlags, point);
	else
		ReleaseCapture();
}

void CDesignWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	if (my_app.GetCurTool() != IDT_TEXTTOOLEX || !(GET_TEXTTOOL()->bEdit))
	{
		// process tool shortcut key
		//if (CToolShortcutKey::OnKey(nChar))
		//{
		//	return;
		//}

		if (!my_app.CurSel().empty())	// 方向键移动选择集
		{
			static TAccelerator	acc(20, 160, 4, 200);

			int nValue = acc.GetValue();

			switch(nChar)
			{
			case VK_LEFT:
				my_app.MoveTransform(-nValue * nRepCnt, 0, 0);
				return;
			case VK_RIGHT:
				my_app.MoveTransform(nValue * nRepCnt, 0, 0);
				return;
			case VK_UP:
				my_app.MoveTransform(0, -nValue * nRepCnt, 0);
				return;
			case VK_DOWN:
				my_app.MoveTransform(0, nValue * nRepCnt,  0);
				return;
			}
		}
		else	// 方向键滚动视图
		{
			switch(nChar)
			{
			case VK_LEFT:
				SendMessage(WM_HSCROLL, SB_LINEUP, 0);
				return;
			case VK_RIGHT:
				SendMessage(WM_HSCROLL, SB_LINEDOWN, 0);				
				return;
			case VK_UP:
				SendMessage(WM_VSCROLL, SB_LINEUP, 0);
				return;
			case VK_DOWN:
				SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);				
				return;
			}
		}
	}

	my_app.CurTool()->KeyDown(nChar, nRepCnt, nFlags);

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDesignWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{		
	my_app.CurTool()->KeyUp(nChar, nRepCnt, nFlags);

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CDesignWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	my_app.CurTool()->Char(nChar, nRepCnt, nFlags);

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CDesignWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		m_tooltip.RelayEvent(pMsg);

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		if (m_hAccel && ::TranslateAccelerator(GetTopLevelFrame()->m_hWnd, m_hAccel, pMsg))
			return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

void CDesignWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	CRuleWnd::Instance()->SetVernierPos(point.x, point.y);

	if (COptions::Instance()->m_bShowDesignToolTip)
	{
		int	x = PIXEL_TO_TWIPS(point.x);
		int y = PIXEL_TO_TWIPS(point.y);
		gld_shape shape = my_app.LookupShape(x, y);
		if (shape.validate())
		{
			CString strTip;

			gldInstance *pInstance = (gldInstance *)shape.ptr();
			if (pInstance->m_obj != m_pObj)
			{
				m_tooltip.Pop();
				m_tooltip.DelTool(this, 1);

				m_pObj = pInstance->m_obj;

				gldObj *pObj = pInstance->m_obj;
				if (pObj->IsGObjInstanceOf(gobjSprite))
				{
					gldSprite *pSprite = (gldSprite *)pObj;
					strTip.Format(pSprite->m_isGroup ? IDS_TIP_GROUP_INSTANCE1 : IDS_TIP_MC_INSTANCE1, 
						pInstance->m_name.c_str());
				}
				else 
				if (pObj->IsGObjInstanceOf(gobjButton))
				{
					strTip.Format(IDS_TIP_BUTTON_INSTANCE1, pInstance->m_name.c_str());
				}
				else 
				if (pObj->IsGObjInstanceOf(gobjText))
				{
					strTip.Format(IDS_TIP_TEXT_INSTANCE1, pInstance->m_name.c_str());
				}
				else
				if (pObj->IsGObjInstanceOf(gobjShape))
				{
					strTip.Format(IDS_TIP_SHAPE_INSTANCE1, pInstance->m_name.c_str());
				}
				else
				{
					ASSERT(FALSE);
					return;
				}

				// if no instance name, trim the colon and space
				strTip.Trim(": ");
				if (strTip.GetLength() > 80)
					strTip = strTip.Left(80);

				// this will replace last tip and not change tip pos.
				gld_rect rc = _transform_rect(shape.bound(), shape.matrix());
				
				my_app.DataSpaceToScreen(rc.left, rc.top);
				my_app.DataSpaceToScreen(rc.right, rc.bottom);

				rc.left = TWIPS_TO_PIXEL(rc.left);
				rc.top = TWIPS_TO_PIXEL(rc.top);
				rc.right = TWIPS_TO_PIXEL(rc.right);
				rc.bottom = TWIPS_TO_PIXEL(rc.bottom);

				AFX_OLDTOOLINFO ti;
				memset(&ti, 0, sizeof(ti));
				ti.cbSize = sizeof(ti);
				ti.hinst = 0;
				ti.hwnd = m_hWnd;
				ti.uFlags = COptions::Instance()->m_bCenterDesignToolTip ? TTF_CENTERTIP : 0;
				ti.lpszText = (LPSTR)(LPCTSTR)strTip;
				ti.uId = 1;

				ti.rect.left = rc.left;
				ti.rect.top = rc.top;
				ti.rect.right = rc.right;
				ti.rect.bottom = rc.bottom + 5;

				m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);

				m_tooltip.Activate(TRUE);
			}
		}
		else
		{
			m_pObj = NULL;
			m_tooltip.Pop();
			m_tooltip.DelTool(this, 1);
		}
	}	

	CWnd::OnMouseMove(nFlags, point);
}

void CDesignWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	if (my_app.Ready())
	{
		CGuardDrawOnce	xDraw;

		if (my_app.GetCurTool() > -1)
			my_app.Tools()[my_app.GetCurTool()]->SetFocus(pOldWnd != NULL ? pOldWnd->m_hWnd : NULL);	

		my_app.Repaint();
	}
}

void CDesignWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	if (my_app.Ready())
	{
		CGuardDrawOnce	xDraw;

		if (my_app.GetCurTool() > -1)
			my_app.Tools()[my_app.GetCurTool()]->KillFocus(pNewWnd != NULL ? pNewWnd->m_hWnd : NULL);

		my_app.Repaint();
	}
}

/*
void CDesignWnd::OnEditCopy()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Copy();
	}
	else
	{
		gld_shape_sel _sel = my_app.CurSel();

		if (_sel.count() > 0)
		{
			GOBJECT_LIST _objs;
			GCHARACTERKEY_LIST _keys;

			for (gld_shape_sel::iterator it = _sel.begin(); it != _sel.end(); it++)
			{
				gld_shape _shape = (*it);
				gldCharacter *_char = (gldCharacter*)(_shape.ptr());
				ASSERT(_char != NULL);
				ASSERT(_char->m_key != NULL);
				gldCharacterKey *_key= _char->m_key;
				_keys.push_back(_key);
				_objs.push_back(_key->GetObj());
			}

			_objs.unique();

			gldLibrary _libTemp;

			for (GOBJECT_LIST_IT it = _objs.begin(); it != _objs.end(); it++)
				GetUsedObjsRecursive(*it, &_libTemp);

			// no longer used
			_objs.clear();

			// write symboles
			ASSERT(_libTemp.GetObjList().size());
			if (_libTemp.GetObjList().size() > 0)
			{
				if (::OpenClipboard(NULL))
				{
					oBinStream os;

					_libTemp.SortObjList();
					::WriteSymbolsToBinStream(os, _libTemp.GetObjList(), _objs);
					_libTemp.ClearList();

					HGLOBAL hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, os.Size());
					if (hData != NULL)
					{
						U8 *pData = (U8 *)::GlobalLock(hData);
						os.WriteToMemory(pData);
						::GlobalUnlock(hData);

						::EmptyClipboard();
						if (!::SetClipboardData(CF_GMOVIE_SYMBOL, hData))
							::ReportLastError();
					}
					::CloseClipboard();
				}
			}

			// write instance (gldCharacterKey & gldCharacter list)
			ASSERT(_keys.size());
			if (_keys.size() > 0)
			{
				if (::OpenClipboard(NULL))
				{
					oBinStream os;

					os << (U32)_keys.size();
					for (GCHARACTERKEY_LIST_IT it = _keys.begin(); it != _keys.end(); it++)
					{
						gldCharacterKey *_key = *it;
						_key->WriteToBinStream(os);
					}

					HGLOBAL hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, os.Size());
					if (hData != NULL)
					{
						U8 *pData = (U8 *)::GlobalLock(hData);
						os.WriteToMemory(pData);
						::GlobalUnlock(hData);

						if (!::SetClipboardData(CF_GMOVIE_INSTANCE, hData))
							::ReportLastError();
					}
					::CloseClipboard();
				}
			}
		}
		else // copy guid line
		{
			tGuidLine *pGuidLine = my_app.GetGuidLine();

			if (pGuidLine != NULL)
			{
				if (::OpenClipboard(NULL))
				{
					oBinStream os;

					gldGuidLineSerializer::WriteToBinStream(pGuidLine, os);

					HGLOBAL hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, os.Size());
					if (hData != NULL)
					{
						U8 *pData = (U8 *)::GlobalLock(hData);
						os.WriteToMemory(pData);
						::GlobalUnlock(hData);

						::EmptyClipboard();

						if (!::SetClipboardData(CF_GMOVIE_GUIDLINE, hData))
							::ReportLastError();
					}

					::CloseClipboard();
				}
			}
		}		
	}
}

void CDesignWnd::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		CPoint ptStart, ptEnd;
		pTool->GetSel(ptStart, ptEnd);
		pCmdUI->Enable(ptStart != ptEnd);
	}
	else
	{
		gld_shape_sel _sel = my_app.CurSel();
		pCmdUI->Enable(_sel.count() > 0 || my_app.GetGuidLine() != NULL);
	}
}
*/

/*
static void PasteGuidLine()
{
	if (!::OpenClipboard(NULL))
	{
		::ReportLastError();
		return;
	}	
	
	tGuidLine *pGuidLine = NULL;
	HGLOBAL hData = ::GetClipboardData(CF_GMOVIE_GUIDLINE);
	
	if (hData)
	{
		U32 size = (U32)::GlobalSize(hData);

		U8 *pData = (U8 *)::GlobalLock(hData);
		iBinStream is;
		is.ReadFromMemory(size, pData);
		::GlobalUnlock(hData);

		pGuidLine = gldGuidLineSerializer::ReadFromBinStream(is);		
	}

	::CloseClipboard();

	if (pGuidLine != NULL && my_app.CanAddShape())
	{
		my_app.Commands().Do(new CCmdInsertGuidLine(pGuidLine));
		if (!my_app.ShowGuidLine())
			ShowShowGuidLineTip();
	}
}
*/

/*
static void PasteInstance()
{
	if (!::OpenClipboard(NULL))
	{
		::ReportLastError();
		return;
	}

	TCommandGroup *_cmdGroup = new TCommandGroup(IDS_CMD_INSERTINSTANCE);
			
	CGroupUpdate	xUpdate;

	HGLOBAL hData = ::GetClipboardData(CF_GMOVIE_SYMBOL);
	if (hData)
	{
		U32 size = (U32)::GlobalSize(hData);

		U8 *pData = (U8 *)::GlobalLock(hData);
		iBinStream is;
		is.ReadFromMemory(size, pData);
		::GlobalUnlock(hData);

		::ReadSymbolsFromBinStream(is, _cmdGroup);
	}

	GCHARACTERKEY_LIST _ckList;

	hData = ::GetClipboardData(CF_GMOVIE_INSTANCE);
	if (hData)
	{
		U32 size = (U32)::GlobalSize(hData);

		U8 *pData = (U8 *)::GlobalLock(hData);
		iBinStream is;
		is.ReadFromMemory(size, pData);
		::GlobalUnlock(hData);

		is >> size;
		for (size_t i = 0; i < size; i++)
		{
			gldCharacterKey *_key = new gldCharacterKey();
			_key->ReadFromBinStream(is);
			_ckList.push_back(_key);
		}
	}

	::CloseClipboard();

	ASSERT(_ckList.size() > 0);

	gldMovieClip *_mc = _GetCurMovieClip();
	gldLayer *_layer = _mc->m_curLayer;
	gldFrameClip *_fc = _layer->GetFrameClip(_mc->m_curTime);

	CRect _rcAll;
	bool _first = true;
	for (GCHARACTERKEY_LIST_IT it = _ckList.begin(); it != _ckList.end(); it++)
	{
		gldCharacterKey *_key = *it;
		gldCharacter *_char = _key->m_cList[0];
		gldObj *pObj = _key->GetObj();

		gldMatrix	_gmat = _char->m_matrix;
		CRect _rc = pObj->m_bounds;
		_gmat.TransformRect(_rc);

		if (_first)
		{
			_first = false;
			_rcAll = _rc;
		}
		else
		{
			CRect _rcTemp = _rcAll;
			_rcAll.UnionRect(_rcTemp, _rc);
		}
	}
	CSize _sz = my_app.GetViewSize();
	int	_x = _sz.cx * 20 / 2;
	int	_y = _sz.cy * 20 / 2;
	my_app.ScreenToDataSpace(_x, _y);
	S32 _offset_x = _x - (_rcAll.left + _rcAll.right) / 2;
	S32 _offset_y = _y - (_rcAll.top + _rcAll.bottom) / 2;

	for (GCHARACTERKEY_LIST::reverse_iterator it = _ckList.rbegin(); it != _ckList.rend(); it++)
	{
		gldCharacterKey *_key = *it;
		gldCharacter *_char = _key->m_cList[0];
		gldObj *pObj = _key->GetObj();

		gldMatrix	_gmat = _char->m_matrix;
		_gmat.m_x += _offset_x;
		_gmat.m_y += _offset_y;

		if (CanPlaceInsideCurObj(pObj))
		{
			gldCharacterKey *_clone = CSWFProxy::CloneCkForFrameClip(_fc
				, _key, _gmat, _char->m_cxform);
			_cmdGroup->Do(new CCmdInsertInstance(_fc, _clone));
		}
		else
		{
			if (AfxMessageBoxEx(MB_ICONWARNING | MB_OKCANCEL, IDS_ERROR_PASTE_SYMBOL_INSIDE_ITSELF1, pObj->m_name.c_str()) == IDCANCEL)
				break;
		}
	}

	for (GCHARACTERKEY_LIST_IT it = _ckList.begin(); it != _ckList.end(); it++)
	{
		gldCharacterKey *_key = *it;
		delete _key;
	}

	my_app.Commands().Do(_cmdGroup);
}
*/

static void PasteDIB()
{	
	if (!::OpenClipboard(NULL))
	{
		::ReportLastError();
		return;
	}

	CString sTempPath = GetTempFile(NULL, "dib", "bmp");
	FILE *fp;
	if ((fp = fopen(sTempPath, "wb")) == NULL)
	{
		VERIFY(::CloseClipboard());
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_CREATE_TEMP_FILE);
		return;
	}	

	HGLOBAL hData = ::GetClipboardData(CF_DIB);
	if (hData == NULL)
	{
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);		
		AfxMessageBox(IDS_E_GET_CLIPBOARD_DATA);
		return;
	}
	
	size_t dwSize = ::GlobalSize(hData);
	void *pData = ::GlobalLock(hData);	
	ASSERT(pData);
	if (dwSize < sizeof(BITMAPINFOHEADER))
	{		
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_CLIPBOARD_FORMAT);
		return;
	}
	BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)pData;
	if (bih->biSize != sizeof(BITMAPINFOHEADER))
	{		
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_CLIPBOARD_FORMAT);
		return;
	}
	size_t cbPalette = 0;
	if (bih->biBitCount <=8 )
	{
		if (bih->biClrUsed != 0)
			cbPalette = sizeof(RGBQUAD) * bih->biClrUsed;
		else
			cbPalette = (1 << bih->biBitCount) * sizeof(RGBQUAD);
	}
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 0x4d42;
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + (DWORD)dwSize;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + cbPalette;
	if (fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER))
	{		
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_WRITE_TEMP_FILE);
		return;
	}
	if (fwrite(pData, 1, dwSize, fp) < dwSize)
	{
		VERIFY(::GlobalUnlock(hData));
		VERIFY(::CloseClipboard());
		fclose(fp);
		DeleteFile(sTempPath);
		AfxMessageBox(IDS_E_WRITE_TEMP_FILE);
		return;
	}	
	VERIFY(::GlobalUnlock(hData));
	VERIFY(::CloseClipboard());
	fclose(fp);
	
	CSize size = my_app.GetViewSize();
	CPoint point(size.cx / 2, size.cy / 2);
	_GetCurDocument()->ImportImage(sTempPath, point);
	DeleteFile(sTempPath);	
}

static void PasteBitmap()
{
}

/*
void CDesignWnd::OnEditPaste()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Paste();
	}
	else
	{
		if (!CanDraw())
			return;

		if (::IsClipboardFormatAvailable(CF_GMOVIE_SYMBOL)
			&& ::IsClipboardFormatAvailable(CF_GMOVIE_INSTANCE))
			PasteInstance();
		else if (::IsClipboardFormatAvailable(CF_GMOVIE_GUIDLINE))
			PasteGuidLine();
		else if (::IsClipboardFormatAvailable(CF_DIB))
			PasteDIB();
		else if (::IsClipboardFormatAvailable(CF_BITMAP))
			PasteBitmap();
	}
}

void CDesignWnd::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pCmdUI->Enable(pTool->CanPaste());
	}
	else
	{
		pCmdUI->Enable((::IsClipboardFormatAvailable(CF_GMOVIE_SYMBOL) 
			&& ::IsClipboardFormatAvailable(CF_GMOVIE_INSTANCE))
			|| ::IsClipboardFormatAvailable(CF_DIB)
			|| ::IsClipboardFormatAvailable(CF_BITMAP)
			|| ::IsClipboardFormatAvailable(CF_GMOVIE_GUIDLINE));
	}
}
*/

BOOL CDesignWnd::CanDraw(bool bAlert)
{
	/*gldMovieClip *pMC = _GetCurMovieClip();
	gldLayer *pLayer = pMC->m_curLayer;
	if (pLayer == pMC->m_layerList[0])
	{
		if (bAlert)
			AfxMessageBox(IDS_PROMPT_DRAW_STREAM_SOUND_LAYER);
		return FALSE;
	}

	if (!pLayer || pLayer->m_locked || !pLayer->m_visible)
	{
		if (bAlert)
			AfxMessageBox(IDS_PROMPT_DRAW_LOCKED_INVISIBLE_LAYER);
		return FALSE;
	}

	gldFrameClip *pFC = pLayer->GetFrameClip(pMC->m_curTime);
	if (!pFC)
	{
		if (bAlert)
			AfxMessageBox(IDS_PROMPT_DRAW_NO_FRAME);
		return FALSE;
	}*/

	BOOL ret = (_GetCurScene2() != NULL);
	if (!ret && bAlert)
		AfxMessageBox(IDS_NO_CURRENT_SCENE);
	return ret;
}

void CDesignWnd::OnEditSelectAll()
{
	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->SelectAll();
	}
	else
	{
		my_app.SelectAll();
	}
}

void CDesignWnd::OnTextDeleteBack()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Key_Backspace();
	}
}

void CDesignWnd::OnUpdateTextDeleteBack(CCmdUI *pCmdUI)
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	pCmdUI->Enable(pTool->IsActive());
}

void CDesignWnd::OnTextUndo()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		if (pTool->CanUndo())
			pTool->Undo();
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_UNDO, 0), 0);
	}
}

void CDesignWnd::OnUpdateTextUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pCmdUI->Enable(pTool->CanUndo());
	}
	else
	{
		// because we want to route this comand to the framework 
		// and if the edit window is not active, this menu will not show.
		pCmdUI->Enable(TRUE);
	}
}

void CDesignWnd::OnTextRedo()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		if (pTool->CanRedo())
			pTool->Redo();
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_REDO, 0), 0);
	}
}

void CDesignWnd::OnUpdateTextRedo(CCmdUI *pCmdUI)
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pCmdUI->Enable(pTool->CanRedo());
	}
	else
	{
		// because we want to route this comand to the framework 
		// and if the edit window is not active, this menu will not show.
		pCmdUI->Enable(TRUE);
	}
}

void CDesignWnd::OnTextGotoBeginOfText()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
		pTool->CursorToFirst();
}

void CDesignWnd::OnTextGotoEndOfText()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
		pTool->CursorToLast();
}

LRESULT CDesignWnd::OnBeginDrag(WPARAM wParam, LPARAM lParam)
{
	RECT *rc = (RECT *)lParam;
	
	CRuleWnd::Instance()->SetVernierPos(rc->left, rc->top, 
		rc->right - rc->left, rc->bottom - rc->top);

	return 0;
}

LRESULT CDesignWnd::OnCancelDrag(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDesignWnd::OnDragOver(WPARAM wParam, LPARAM lParam)
{
	RECT *rc = (RECT *)lParam;
	
	CRuleWnd::Instance()->SetVernierPos(rc->left, rc->top,
		rc->right - rc->left, rc->bottom - rc->top);

	return 0;
}

LRESULT CDesignWnd::OnDragDrop(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CDesignWnd::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	GetTopLevelFrame()->SetActiveWindow();
	SetFocus();

	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);
	CGlandaDoc *pDoc = _GetCurDocument();
	ASSERT_VALID(pDoc);

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);

		_LOG1("DropFile:\t%s", szFileName);

		TCHAR *pszExt = ::PathFindExtension(szFileName);
		if (_tcsicmp(pszExt, ".bmp") == 0 || 
			_tcsicmp(pszExt, ".jpg") == 0 ||  
			_tcsicmp(pszExt, ".jpe") == 0 || 
			_tcsicmp(pszExt, ".jpeg") == 0 || 
			_tcsicmp(pszExt, ".png") == 0)
		{
			if (CanDraw(true))
				pDoc->ImportImage(szFileName, point, TRUE);
		}
		else 
		if (_tcsicmp(pszExt, ".wmf") == 0 || 
			_tcsicmp(pszExt, ".emf") == 0 || 
			_tcsicmp(pszExt, ".svg") == 0 || 
			_tcsicmp(pszExt, ".ai") == 0)
		{
			if (CanDraw(true))
				pDoc->ImportVectorFormat(szFileName, point);
		}
		else
		if (_tcsicmp(pszExt, ".gls") == 0)
		{
			if (CanDraw(true))
				pDoc->ImportGLS(szFileName, point);
		}
		else if (_tcsicmp(pszExt, ".glb") == 0 || _tcsicmp(pszExt, ".glm") == 0)
		{
			if (CanDraw(true))
				pDoc->ImportGLC(szFileName, point);
		}
		else if(_tcsicmp(pszExt, ".wav") == 0 || _tcsicmp(pszExt, ".mp3") == 0)
		{
			if(CanDraw(true))
				pDoc->ImportSound(szFileName);
		}
		else
		{
			if (iFile == 0)
			{
				pApp->OpenDocumentFile(szFileName);
			}
			else
			{
				TCHAR szName[MAX_PATH + 1];
				if (::GetModuleFileName(NULL, szName, _MAX_PATH))
					::ShellExecute(NULL, "open", szName, CString("\"") + szFileName + "\"", NULL, SW_SHOWNORMAL);
			}
		}
	}
	::DragFinish(hDropInfo);

	//CWnd::OnDropFiles(hDropInfo);
}

void CDesignWnd::OnEditActionScript()
{
	if (_GetCurScene2() != NULL && my_app.CurSel().count() == 1)
	{
		CString strCaption;
		strCaption.LoadString(IDS_KEY_ACTION);
		CInstanceActionSheet dlg(strCaption);
		dlg.DoModal();
	}
}

void CDesignWnd::OnUpdateEditActionScript(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL && my_app.CurSel().count() == 1);
}

void CDesignWnd::OnEditInstanceProperties()
{
	if (_GetCurScene2() != NULL)
	{
		gld_shape_sel sel = my_app.CurSel();
		if (sel.count() == 1)
		{
			gld_shape_sel::iterator it = sel.begin();
			EditInstance(&(*it));
		}
	}
}

void CDesignWnd::OnUpdateEditInstanceProperties(CCmdUI *pCmdUI)
{
	if (_GetCurScene2() != NULL)
	{
		gld_shape_sel sel = my_app.CurSel();
		if (sel.count() == 1)
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}

void CDesignWnd::OnEditPrevInstance()
{
	gldScene2 *pScene = _GetCurScene2();
	if (pScene && pScene->m_instanceList.size() > 0)
	{
		GINSTANCE_LIST::reverse_iterator it = pScene->m_instanceList.rbegin();
	
		gld_shape_sel &sel = my_app.CurSel();
		if (sel.count() > 0)
		{
			gld_shape shape = *sel.rbegin();
			gldInstance *pInstance = CTraitInstance(shape);
			ASSERT(pInstance);
			if (pInstance)
			{
				for (; it != pScene->m_instanceList.rend(); it++)
				{
					if (*it == pInstance)
					{
						it++;
						break;
					}
				}

				if (it == pScene->m_instanceList.rend())
					it = pScene->m_instanceList.rbegin();
			}
		}

		gld_shape_sel selNew;
		selNew.select(CTraitShape(*it));
		my_app.DoSelect(selNew);
	}
}

void CDesignWnd::OnUpdateEditPrevInstance(CCmdUI *pCmdUI)
{
	gldScene2 *pScene = _GetCurScene2();
	pCmdUI->Enable(pScene && pScene->m_instanceList.size() > 0);
}

void CDesignWnd::OnEditNextInstance()
{
	gldScene2 *pScene = _GetCurScene2();
	if (pScene && pScene->m_instanceList.size() > 0)
	{
		GINSTANCE_LIST::iterator it = pScene->m_instanceList.begin();
	
		gld_shape_sel &sel = my_app.CurSel();
		if (sel.count() > 0)
		{
			gld_shape shape = *sel.rbegin();
			gldInstance *pInstance = CTraitInstance(shape);
			ASSERT(pInstance);
			if (pInstance)
			{
				for (; it != pScene->m_instanceList.end(); it++)
				{
					if (*it == pInstance)
					{
						it++;
						break;
					}
				}

				if (it == pScene->m_instanceList.end())
					it = pScene->m_instanceList.begin();
			}
		}

		gld_shape_sel selNew;
		selNew.select(CTraitShape(*it));
		my_app.DoSelect(selNew);
	}
}

void CDesignWnd::OnUpdateEditNextInstance(CCmdUI *pCmdUI)
{
	gldScene2 *pScene = _GetCurScene2();
	pCmdUI->Enable(pScene && pScene->m_instanceList.size() > 0);
}

CString GetElementTemplatePath()
{
	CString strPath;
	strPath.Format("%s\\%s", GetModuleFilePath(), "Templates\\Element");
	if (!PathFileExists(strPath))
	{
		CreateDirectoryNest(strPath);
	}
	return strPath;
}

bool ElementTemplateNameIsValid(void *pData, LPCTSTR lpszText)
{
	CString fileName(lpszText);
	fileName.Trim();
	if (fileName.GetLength() == 0)
	{
		AfxMessageBox(IDS_NEED_TEMPLATE_NAME);
		return FALSE;
	}
	if (!IsValidFileName(fileName))
	{
		AfxMessageBox(IDS_ERROR_INVALID_FILE_NAME);
		return FALSE;
	}
	CString filePath;
	filePath.Format("%s\\%s.gmd", (LPCTSTR)GetElementTemplatePath(), lpszText);
	if (PathFileExists(filePath))
	{
		if (AfxMessageBoxEx(MB_YESNO, IDS_REPLACE_EXISTS_TEMPLATE_1, lpszText) == IDYES)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

void CDesignWnd::OnSaveElementAsTemplate()
{
	gldInstance *pInst = _GetCurInstance();
	
	if ((pInst != NULL) && pInst->SaveAsTemplate())
	{
		CInputBox inputName(this, IDS_SAVE_EFFECT_SETTINGS, IDS_INPUT_SETTINGS_NAME
			, pInst->m_name.c_str(), ElementTemplateNameIsValid, NULL);
		if (inputName.DoModal() == IDOK)
		{
			CWaitCursor xWait;
			CString fileName = inputName.m_strText;
			fileName.Trim();
			CString filePath;
			filePath.Format("%s\\%s.gmd", (LPCTSTR)GetElementTemplatePath(), (LPCTSTR)fileName);
			if (FAILED(pInst->SaveAsTemplate(filePath, fileName)))
			{
				AfxMessageBox(IDS_FAILED_SAVE_TEMPLATE);
			}
		}
	}
}

void CDesignWnd::OnUpdateSaveElementAsTemplate(CCmdUI *pCmdUI)
{
	BOOL bEnabled = FALSE;
	gldInstance *pInst = _GetCurInstance();
	if (pInst != NULL)
	{
		bEnabled = pInst->SaveAsTemplate();
	}
	pCmdUI->Enable(bEnabled);
}

void CDesignWnd::OnApplyTemplateToElement()
{
	gldInstance *pInst = _GetCurInstance();
	if (pInst == NULL)
	{
		return;
	}
	if (pInst->m_effectList.size() > 0)
	{
		if (AfxGetApp()->GetProfileInt("Tips", "Change Common Setting", TRUE))
		{
			BOOL bChecked = FALSE;
			UINT nRet = CheckMessageBox(IDS_CONTINUE_APPLY_TEMPLATE, 
				IDS_NO_PROMPT_AGAIN, 
				bChecked, 
				MB_ICONQUESTION | MB_YESNO);
			AfxGetApp()->WriteProfileInt("Tips", "Change Common Setting", !bChecked);
			if (nRet == IDNO)
			{
				return;
			}
		}
	}
	CDlgApplyTemplate dlg(CListTemplate::FILTER_ELEMENT);
	if (dlg.DoModal() == IDOK)
	{
		if (FAILED(pInst->ApplyTemplate(dlg.GetTemplatePath())))
		{
			AfxMessageBox(IDS_FAILED_APPLY_TEMPLATE, MB_OK);
		}
	}
}

void CDesignWnd::OnUpdateApplyTemplateToElement(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurInstance() != NULL);
}

void CDesignWnd::OnChangeButtonText()
{
	gldInstance *inst = _GetCurInstance();
	if (inst == NULL || !inst->m_obj->IsGObjInstanceOf(gobjButton))
		return;

	gldButton *button = (gldButton *)inst->m_obj;
	CString strCap = CCmdChangeButtonCap::GetButtonCaption(button);
	if (strCap.GetLength() == 0 || !CCmdChangeButtonCap::PromptChangeComplexText(button, strCap))
		return;

	CDlgChangeButtonText dlg;
	dlg.m_strText = strCap;
	if (dlg.DoModal() == IDOK)
	{
		my_app.Commands().Do(new CCmdChangeButtonCap(button, strCap, dlg.m_strText, dlg.m_Method));
	}
}

void CDesignWnd::OnUpdateChangeButtonText(CCmdUI *pCmdUI)
{
	gldInstance *inst = _GetCurInstance();
	if (inst != NULL)
	{
		if (inst->m_obj->IsGObjInstanceOf(gobjButton))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}




/* scene background */

void SetAsBackgrnd(int pos)
{
	if (_GetCurScene2() != NULL)
	{
		gldInstance *pInst = _GetCurInstance();
		if (pInst != NULL)
		{		
			CGuardDrawOnce xDraw;
			CGuardSelKeeper xSel;
			CCmdGroupX<CGuardDrawOnce, CGuardSelKeeper> *cmd = new CCmdGroupX<CGuardDrawOnce, CGuardSelKeeper>(IDS_CMD_CHANGESCENEBACKGRND);
			cmd->Do(new CCmdRemoveInstance(my_app.CurSel(), false));
			cmd->Do(new CCmdChangeSceneBackgrnd(_GetCurScene2(), pInst, pos, true));
			my_app.Commands().Do(cmd);
		}
	}
}

void CDesignWnd::OnEmptyBackground()
{
	if (_GetCurScene2() != NULL)
	{
		CBackground *backgrnd = _GetCurScene2()->m_backgrnd;
		if (backgrnd != NULL)
		{
			my_app.Commands().Do(new CCmdChangeSceneBackgrnd(_GetCurScene2()));
		}
	}
}

void CDesignWnd::OnUpdateEmptyBackground(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL && _GetCurScene2()->m_backgrnd != NULL);
}

void CDesignWnd::OnChangeBackgroundPos()
{
	if (_GetCurScene2() != NULL)
	{
		CBackground *backgrnd = _GetCurScene2()->m_backgrnd;
		if (backgrnd != NULL)
		{
			int pos = backgrnd->GetPosition();
			CDlgSelectBackgrndPos dlg(pos);
			if (dlg.DoModal() == IDOK && pos != dlg.GetSelected())
				my_app.Commands().Do(new CCmdChangeBackgrndPos(backgrnd, dlg.GetSelected()));
		}
	}
}

void CDesignWnd::OnUpdateChangeBackgroundPos(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL && _GetCurScene2()->m_backgrnd != NULL);
}

class CCmdTraitInstanceFromBackgrnd : public TCommand
{
public:
	CCmdTraitInstanceFromBackgrnd(CBackground *backgrnd)
		: m_Backgrnd(backgrnd)
		, m_pInst(NULL)
	{
	}

	virtual bool Execute()
	{
		m_pInst = m_Backgrnd->Detach();

		return TCommand::Execute();
	}

	virtual bool Unexecute()
	{
		m_Backgrnd->Attach(m_pInst);
		m_pInst = NULL;

		return TCommand::Unexecute();
	}

	gldInstance *m_pInst;

private:
	CBackground *m_Backgrnd;
};

void CDesignWnd::OnRestoreBackground()
{
	if (_GetCurScene2() != NULL)
	{
		CBackground *backgrnd = _GetCurScene2()->m_backgrnd;
		if (backgrnd != NULL)
		{
			CGuardDrawOnce xDraw;
			CGuardSelAppend xSel;
			CCmdGroupX<CGuardDrawOnce, CGuardSelAppend> *cmd = new CCmdGroupX<CGuardDrawOnce, CGuardSelAppend>(IDS_CMD_CHANGESCENEBACKGRND);
			CCmdTraitInstanceFromBackgrnd *traitInst = new CCmdTraitInstanceFromBackgrnd(backgrnd);			
			cmd->Do(new CCmdChangeSceneBackgrnd(_GetCurScene2()));
			cmd->Do(traitInst);
			cmd->Do(new CCmdAddInstance(_GetCurScene2(), traitInst->m_pInst, false));
			my_app.Commands().Do(cmd);
		}
	}
}

void CDesignWnd::OnUpdateRestoreBackground(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL && _GetCurScene2()->m_backgrnd != NULL);
}

void CDesignWnd::OnSetAsBackground()
{
	if (_GetCurScene2() != NULL)
	{
		CDlgSelectBackgrndPos dlg;
		if (dlg.DoModal() == IDOK)
		{
			SetAsBackgrnd(dlg.GetSelected());
		}
	}
}

void CDesignWnd::OnUpdateSetAsBackground(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL && _GetCurInstance() != NULL && my_app.CurSel().count() == 1);
}

/*
#include "DlgSelectScene.h"

void CDesignWnd::OnTest()
{
	CDlgSelectScene dlg(NULL, "Goto Scene");

	dlg.DoModal();
}
*/