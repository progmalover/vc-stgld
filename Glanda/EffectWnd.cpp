// EffectWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EffectWnd.h"
#include "gldInstance.h"
#include "gldEffect.h"
#include "GlandaDoc.h"
#include "MemDC.h"
#include "Graphics.h"
#include "DeferWindowPos.h"
#include "my_app.h"
#include "CmdModifyEffectTime.h"
#include "CmdModifySceneSoundTime.h"
#include "SWFProxy.h"
#include "CmdMoveEffect.h"
#include <algorithm>
#include <atlwin.h>
#include "my_app.h"
#include "Graphics.h"
#include "gldSound.h"
#include "gldSprite.h"
#include "Symbols.h"
#include "EffectBar.h"
#include "GlandaClipboard.h"
#include "Options.h"
#include "PlaySoundDialog.h"
#include "CmdRemoveEffect.h"
#include "CmdRemoveSound.h"
#include "ResourceThumbItem.h"
#include "ResourceThumbObjPropertiesDialog.h"
#include "DlgSelectEffect.h"
#include "EffectCommonParameters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const int HSCROLL_MARGIN			= 1;

const int TOTAL_FRAMES_DEFAULT		= 600;
const int TOTAL_FRAMES_INCREMENT	= 600;

const int SPLITTER_SIZE				= 6;
const int HSB_LINE_SIZE				= 5;
const int VSB_LINE_SIZE				= 1;

const UINT IDC_HSCROLL				= 1;
const UINT IDC_VSCROLL				= 2;

const COLORREF crDarkGray			= RGB(128, 128, 128);

// Release版不能定义
//#define EXTERNAL_RES
//#define PARAM_FROM_INI
//#define TRANS_DRAW_LINE

#if defined(EXTERNAL_RES) && !defined(_DEBUG)
#error Define EXTERNAL_RES not allowed in release version.
#endif

#if defined(PARAM_FROM_INI) && !defined(_DEBUG)
#error Define PARAM_FROM_INI not allowed in release version.
#endif

#if defined(TRANS_DRAW_LINE) && !defined(_DEBUG)
#error Define TRANS_DRAW_LINE not allowed in release version.
#endif

#if defined(PARAM_FROM_INI)
#define INT_FROM_INI_FILE(Section, Key, Default)									\
GetPrivateProfileInt(Section, Key, Default, Default, "res\\effectwnd.ini")
#else
#define INT_FROM_INI_FILE(Section, Key, Default)									\
Default
#endif

#if defined(PARAM_FROM_INI)
#define COLOR_FROM_INI_FILE(Section, Default)										\
RGB(																				\
	GetPrivateProfileInt(Section, "R", GetRValue(Default), "res\\effectwnd.ini"),	\
	GetPrivateProfileInt(Section, "G", GetGValue(Default), "res\\effectwnd.ini"),	\
	GetPrivateProfileInt(Section, "B", GetBValue(Default), "res\\effectwnd.ini")	\
)
#else
#define COLOR_FROM_INI_FILE(Section, Default)										\
Default
#endif

#if defined(EXTERNAL_RES)
#define LOADBMPRES(bmp, ext_res, id)									\
{																		\
	HBITMAP hbmp = (HBITMAP)LoadImage(0, ext_res, IMAGE_BITMAP, 0, 0,	\
	LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);			\
	if(hbmp)															\
	{																	\
		bmp##.Attach(hbmp);												\
	}																	\
	else																\
	{																	\
		bmp##.LoadBitmap(id);											\
	}																	\
}
#else
#define LOADBMPRES(bmp, ext_res, id)									\
{																		\
	bmp##.LoadBitmap(id);												\
}
#endif

// CEffectWnd

IMPLEMENT_DYNAMIC(CEffectWnd, CWnd)
CEffectWnd::CEffectWnd()
: m_nAutoScrollDir(none)
, m_bAutoScrollUpdateWindow(FALSE)
, m_bScrolled(FALSE)
, m_pSelInstance(NULL)
, m_pSelEffect(NULL)
, m_pSelSound(NULL)
, m_nSelType(SEL_VOID)
, m_bTrackingTimeLine(NULL)
, m_bSelBlocked(FALSE)
{
	CWinApp* pApp = AfxGetApp();

	// 读取分隔线的位置
	m_nSplitterPos = pApp->GetProfileInt("Timeline", "Splitter Pos", 150);

	m_sizeFrame.cx	= INT_FROM_INI_FILE("Frame", "Width",	8);
	m_sizeFrame.cy	= INT_FROM_INI_FILE("Frame", "Height",	23);
	m_nTitleHeight	= INT_FROM_INI_FILE("Title", "Height",	20);
	m_clrEffectLineText		= COLOR_FROM_INI_FILE("EffectLineText",		RGB(150, 150, 150));
	m_clrEffectLineTextSel	= COLOR_FROM_INI_FILE("EffectLineTextSel",	RGB(100, 100, 100));
	m_clrSoundLineText		= COLOR_FROM_INI_FILE("SoundLineText",		RGB(150, 150, 150));
	m_clrSoundLineTextSel	= COLOR_FROM_INI_FILE("SoundLineTextSel",	RGB(100, 100, 100));

	m_nTotalFrames = TOTAL_FRAMES_DEFAULT;

	SetShowAllInstances(COptions::Instance()->m_bShowAllElementsInTimeline);
	CreateDrawTools();
}

CEffectWnd::~CEffectWnd()
{
	ReleaseDrawTools();
}


BEGIN_MESSAGE_MAP(CEffectWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_INITMENUPOPUP()

	ON_COMMAND(ID_TIME_LINE_PLAY_SOUND, OnTimeLinePlaySound)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_PLAY_SOUND, OnUpdateTimeLinePlaySound)

	ON_COMMAND(ID_TIME_LINE_CUT, OnTimeLineCut)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_CUT, OnUpdateTimeLineCut)
	ON_COMMAND(ID_TIME_LINE_COPY, OnTimeLineCopy)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_COPY, OnUpdateTimeLineCopy)
	ON_COMMAND(ID_TIME_LINE_PASTE, OnTimeLinePaste)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_PASTE, OnUpdateTimeLinePaste)
	ON_COMMAND(ID_TIME_LINE_REMOVE, OnTimeLineDelete)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_REMOVE, OnUpdateTimeLineDelete)

	ON_COMMAND(ID_TIME_LINE_MOVE_UP, OnTimeLineMoveUp)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_MOVE_UP, OnUpdateTimeLineMoveUp)
	ON_COMMAND(ID_TIME_LINE_MOVE_DOWN, OnTimeLineMoveDown)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_MOVE_DOWN, OnUpdateTimeLineMoveDown)

	ON_COMMAND(ID_TIME_LINE_CONFIG, OnTimeLineConfig)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_CONFIG, OnUpdateTimeLineConfig)
	ON_COMMAND(ID_TIME_LINE_SOUND_INFO, OnTimeLineConfig)
	ON_UPDATE_COMMAND_UI(ID_TIME_LINE_SOUND_INFO, OnUpdateTimeLineConfig)

	ON_COMMAND(ID_TIMELINE_REPLACE, OnTimeLineReplace)
	ON_UPDATE_COMMAND_UI(ID_TIMELINE_REPLACE, OnUpdateTimeLineReplace)
END_MESSAGE_MAP()


void CEffectWnd::DrawHorzDotLine(CDC *pDC, int y, int x1, int x2)
{
	const int DOT_SIZE = 1;

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

void CEffectWnd::DrawVertDotLine(CDC *pDC, int x, int y1, int y2)
{
	const int DOT_SIZE = 1;

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

HBRUSH CEffectWnd::CreatePatternBrush(HINSTANCE hInst, UINT nID)
{	
	HBRUSH hBrush = NULL;
	HBITMAP hBitmap = ::LoadBitmap(hInst, MAKEINTRESOURCE(nID));
	if (hBitmap)
	{
		hBrush = ::CreatePatternBrush(hBitmap);
		::DeleteObject(hBitmap);
	}

	return hBrush;
}
// CEffectWnd message handlers


int CEffectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rc(0, 0, 0, 0);

	// 创建水平及垂直滚动条
	m_sbHorz.Create(WS_CHILD | WS_VISIBLE | SB_HORZ, rc, this, IDC_HSCROLL);
	m_sbVert.Create(WS_CHILD | WS_VISIBLE | SB_VERT, rc, this, IDC_VSCROLL);


	// 初始化滚动条位置
	m_sbHorz.SetScrollPos(0, TRUE);
	m_sbVert.SetScrollPos(0, TRUE);

	// Initialize tooltip control
	m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_tooltip.SetMaxTipWidth(INT_MAX);
	m_tooltip.SetMargin(CRect(5, 5, 5, 5));
	//m_tooltip.SetDelayTime(TTDT_INITIAL, 200);
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, 1000 * 60);



	m_fntFrameNum.CreatePointFont(7 * 10, "Small Fonts", NULL);

	m_bmpSoundIcon.LoadBitmap(IDB_EFFWND_SOUND_ICON);

	if(m_lstSoundList.Create(16, 19, ILC_MASK|ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		if(bmp.LoadBitmap(IDB_EFFWND_SOUND_LIST))
		{
			m_lstSoundList.Add(&bmp, RGB(255, 0, 255));
		}
	}

	if(m_lstExpanded.Create(16, 19, ILC_MASK|ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		if(bmp.LoadBitmap(IDB_EFFWND_EXPANDED_ICON))
			m_lstExpanded.Add(&bmp, RGB(255, 0, 255));
	}
	if (m_lstInstanceIcon.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		if (bmp.LoadBitmap(IDB_SYMBOLS))
			m_lstInstanceIcon.Add(&bmp, RGB(255, 0, 255));
	}
	if(m_lstTreeLine.Create(24, 23, ILC_MASK|ILC_COLOR32, 0, 9))
	{
		CBitmap bmp;
		if(bmp.LoadBitmap(IDB_EFFWND_TREE_LINE))
			m_lstTreeLine.Add(&bmp, RGB(255, 0, 255));
	}

	m_bmpEffectIcon.LoadBitmap(IDB_EFFECT_DEFAULT);
	ModifyStyle(WS_HSCROLL | WS_VSCROLL, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_TIMELINE));

	return 0;
}

// 重新计算滚动条位置
int CEffectWnd::GetMaxTime()
{
	int nMaxTime = 0;

	gldScene2* pScene = _GetCurScene2();
	if(pScene)
	{
		nMaxTime = pScene->GetMaxTime(FALSE);
	}
	return nMaxTime;
}

int CEffectWnd::GetMaxTimeWithSound()
{
	int nMaxTime = 0;

	gldScene2* pScene = _GetCurScene2();
	if(pScene)
	{
		nMaxTime = pScene->GetMaxTime(TRUE);
		nMaxTime = max(nMaxTime, GetAllSoundsRange(pScene).cy);
	}
	return nMaxTime;
}

SIZE CEffectWnd::GetAllEffectsRange(gldInstance* pInstance)
{
	BOOL bTracking = FALSE;
	if (m_bTrackingTimeLine)
	{
		int nSelType = GetSelType();
		if (nSelType == SEL_INST || nSelType == SEL_EFFECT)
			bTracking = (pInstance == m_pSelInstance);
	}

	SIZE sz;
	sz.cx = -1;
	sz.cy = -1;

	for(GEFFECT_LIST::iterator iter = pInstance->m_effectList.begin(); iter != pInstance->m_effectList.end(); ++iter)
	{
		gldEffect* pEffect = *iter;
		if(sz.cx < 0)
			sz.cx = bTracking ? pEffect->m_startTimeTracking : pEffect->m_startTime;
		else
			sz.cx = min(sz.cx, bTracking ? pEffect->m_startTimeTracking : pEffect->m_startTime);
		if(sz.cy < 0)
			sz.cy = bTracking ? pEffect->m_startTimeTracking + pEffect->m_lengthTracking: pEffect->m_startTime + pEffect->m_length;
		else
			sz.cy = max(sz.cy, bTracking ? pEffect->m_startTimeTracking + pEffect->m_lengthTracking: pEffect->m_startTime + pEffect->m_length);
	}
	return sz;
}

SIZE CEffectWnd::GetAllSoundsRange(gldScene2* pScene)
{
	BOOL bTracking = FALSE;
	if (m_bTrackingTimeLine)
	{
		int nSelType = GetSelType();
		if (nSelType == SEL_SCAPT || nSelType == SEL_SOUND)
			bTracking = TRUE;
	}

	SIZE sz;
	sz.cx = -1;
	sz.cy = -1;

	for(GSCENESOUND_LIST::iterator iter = pScene->m_soundList.begin(); iter != pScene->m_soundList.end(); ++iter)
	{
		gldSceneSound* pSound = *iter;
		if(sz.cx < 0)
			sz.cx = bTracking ? pSound->m_startTimeTracking : pSound->m_startTime;
		else
			sz.cx = min(sz.cx, bTracking ? pSound->m_startTimeTracking : pSound->m_startTime);
		if(sz.cy < 0)
			sz.cy = bTracking ? pSound->m_startTimeTracking + pSound->m_lengthTracking: pSound->m_startTime + pSound->m_length;
		else
			sz.cy = max(sz.cy, bTracking ? pSound->m_startTimeTracking + pSound->m_lengthTracking: pSound->m_startTime + pSound->m_length);
	}

	return sz;
}

void CEffectWnd::RecalcScrollPos(void)
{
	RecalcScrollRange(GetMaxTimeWithSound() + GetVisibleFrames());

	CRect rc;
	GetTimeLineRect(rc);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	/*
	GetVisibleFrames()：
	CRect rc;
	GetTimeLineRect(rc);
	int count = rc.Width() / m_sizeFrame.cx;
	if (rc.Width() % m_sizeFrame.cx > 1)
	count++;
	return count;
	*/
	// horz
	si.nMin = 0;
	si.nMax = m_nTotalFrames - 1;
	si.nPage = min(si.nMax + 1, GetVisibleFrames());
	si.nPos = min(m_nTotalFrames - 1, m_sbHorz.GetScrollPos());
	m_sbHorz.SetScrollInfo(&si, TRUE);

	// vert
	si.nMin = 0;

	/*
	GetVisibleRows（）：
	CRect rc;
	GetTimeLineRect(rc);
	int count = rc.Height() / m_sizeFrame.cy;
	if (rc.Height() % m_sizeFrame.cy > 0)
	count++;
	return count;
	*/
	si.nMax = max(GetRowsCount(), GetVisibleRows());
	si.nPage = min(si.nMax, GetVisibleRows());
	si.nPos = min(GetRowsCount() - 1, m_sbVert.GetScrollPos());
	m_sbVert.SetScrollInfo(&si, TRUE);

	Invalidate();
}

void CEffectWnd::RecalcScrollRange(int nTotalFrames)
{
	m_nTotalFrames = max(TOTAL_FRAMES_DEFAULT, nTotalFrames);
	if ((m_nTotalFrames - TOTAL_FRAMES_DEFAULT) % TOTAL_FRAMES_INCREMENT > 0)
		m_nTotalFrames += (TOTAL_FRAMES_INCREMENT - (m_nTotalFrames - TOTAL_FRAMES_DEFAULT) % TOTAL_FRAMES_INCREMENT);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;

	si.nMin = 0;
	si.nMax = m_nTotalFrames - 1;

	si.nPage = GetVisibleFrames();
	m_sbHorz.SetScrollInfo(&si, TRUE);
}

void CEffectWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CGlandaMemDC dcMem(&dc);
	dcMem.SetBkMode(TRANSPARENT);

	CRect rc;
	GetClientRect(&rc);

	CRect rcClip;
	dc.GetClipBox(&rcClip);

	CRect rcIntersect;
	rcIntersect.IntersectRect(&rc, &rcClip);

	//填充右下角为底色
	dcMem.FillRect(&rcIntersect, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DFACE)));

	if (_GetCurScene2() != NULL)
	{
		//右侧的特效显示条，与桢显示
		GetTimeLineRect(rc);
		if (rcIntersect.IntersectRect(&rc, &rcClip))
			PaintTimeLine(dcMem);

		//左侧特效树
		GetEffectBarRect(rc);
		if (rcIntersect.IntersectRect(&rc, &rcClip))
			PaintEffectBar(dcMem);
	}

	GetEffectBarTitleRect(rc);
	if (rcIntersect.IntersectRect(&rc, &rcClip))
	   PaintEffectBarTitle(dcMem);//上部的矩形

	GetTimeLineTitleRect(rc);
	if (rcIntersect.IntersectRect(&rc, &rcClip))
		PaintTimeLineTitle(dcMem);//上部的标尺

	PaintWidgets(dcMem);

	GetSplitterRect(rc);
	if (rcIntersect.IntersectRect(&rc, &rcClip))
		PaintSplitter(dcMem);
}

int CEffectWnd::GetRowsCount()
{
	int nRowCount = 0;

	gldScene2* pScene = _GetCurScene2();
	if(pScene)
	{
		for(GSCAPT_PAIR gScapt = GetBeginScaptPair(); !IsEndPair(gScapt); gScapt = GetNextScaptPair(gScapt))
		{
			++nRowCount;
		}

		for(GSOUND_PAIR gSound = GetBeginSoundPair(); !IsEndPair(gSound); gSound = GetNextSoundPair(gSound))
		{
			++nRowCount;
		}

		for(GINSTANCE_PAIR gInst = GetBeginInstPair(); !IsEndPair(gInst); gInst = GetNextInstPair(gInst))
		{
			++nRowCount;
		}

		for(GEFFECT_PAIR gEffect = GetBeginEffectPair(); !IsEndPair(gEffect); gEffect = GetNextEffectPair(gEffect))
		{
			++nRowCount;
		}
	}
	return nRowCount;
}

void CEffectWnd::PaintTimeLineRowBase(CDC& dc, int nRow, BOOL bSel, int nStartFrame, int nEndFrame, CBitmap& bmpTile, CBitmap& bmpLine, LPCTSTR lpszText, int nTopMargin, int nBottomMargin, COLORREF clrTextSel, COLORREF clrText)
{
	gldScene2*	pScene		= _GetCurScene2();
	int			nMaxTime	= pScene->GetMaxTime(FALSE);

	CRect rcVisible;
	GetTimeLineRect(rcVisible);

	CRect	rc;
	GetTimeLineRowRect(rc, nRow);

	int nFirstVisibleFrame	= GetFirstVisibleFrame();
	int nVisibleFrames		= GetVisibleFrames();

	for(int nFrame = nFirstVisibleFrame; nFrame < nFirstVisibleFrame + nVisibleFrames; ++nFrame)
	{
		// 仅有的可见区

		// 计算源矩形区
		BITMAP bm;
		bmpTile.GetBitmap(&bm);

		BOOL bExceed = nFrame >= nMaxTime;
		CRect rcTileBlock(0, 0, m_sizeFrame.cx, m_sizeFrame.cy);
		if(bExceed)
		{
			rcTileBlock.OffsetRect(bm.bmWidth / 2, 0);
		}
		if(bSel)
		{
			rcTileBlock.OffsetRect(0, bm.bmHeight / 2);
		}

		int nTileFrames = bm.bmWidth / (m_sizeFrame.cx * 2);
		rcTileBlock.OffsetRect(m_sizeFrame.cx * (nFrame % nTileFrames), 0);

		// 计算目标矩形区
		CRect rcFrame(rc);
		rcFrame.left += (nFrame - nFirstVisibleFrame) * m_sizeFrame.cx;
		rcFrame.right = rcFrame.left + m_sizeFrame.cx;

		ASSERT(rcFrame.Width() == rcTileBlock.Width());
		ASSERT(rcFrame.Height() == rcTileBlock.Height());

		CRect rcDest;
		if(rcDest.IntersectRect(rcVisible, rcFrame))
		{
			rcTileBlock.left	+=	rcDest.left - rcFrame.left;
			rcTileBlock.top		+=	rcDest.top - rcFrame.top;
			rcTileBlock.right	-=	rcFrame.right - rcDest.right;
			rcTileBlock.bottom	-=	rcFrame.bottom - rcDest.bottom;

			ASSERT(rcTileBlock.Width()==rcDest.Width());
			ASSERT(rcTileBlock.Height()==rcDest.Height());

			DrawBitmap(dc.m_hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), (HBITMAP)bmpTile, rcTileBlock.left, rcTileBlock.top);

			CRect rcLineBlock(0, 0, m_sizeFrame.cx, m_sizeFrame.cy);
			BITMAP bm;
			bmpLine.GetBitmap(&bm);
			if(bSel)
			{
				rcLineBlock.OffsetRect(0, m_sizeFrame.cy);
			}
			if(nFrame >= nStartFrame && nFrame < nEndFrame)
			{
				if(nFrame == nStartFrame && nFrame == nEndFrame -1)
				{
					int lw = rcLineBlock.Width() / 2;
					int rw = rcLineBlock.Width() - lw;
					CRect rcLineBlockLeft(rcLineBlock);
					CRect rcLineBlockRight(rcLineBlock);
					rcLineBlockLeft.right = rcLineBlockLeft.left + lw;
					rcLineBlockRight.right = bm.bmWidth;
					rcLineBlockRight.left = rcLineBlockRight.right - rw;

					CRect rcFrameLeft(rcFrame);
					CRect rcFrameRight(rcFrame);
					rcFrameLeft.right = rcFrameLeft.left + lw;
					rcFrameRight.left = rcFrameRight.right - rw;

					CRect rcDestLeft;
					CRect rcDestRight;
					if(rcDestLeft.IntersectRect(rcVisible, rcFrameLeft))
					{
						rcLineBlockLeft.left	+=	rcDestLeft.left - rcFrameLeft.left;
						rcLineBlockLeft.top		+=	rcDestLeft.top - rcFrameLeft.top;
						rcLineBlockLeft.right	-=	rcFrameLeft.right - rcDestLeft.right;
						rcLineBlockLeft.bottom	-=	rcFrameLeft.bottom - rcDestLeft.bottom;

						ASSERT(rcLineBlockLeft.Width()==rcDestLeft.Width());
						ASSERT(rcLineBlockLeft.Height()==rcDestLeft.Height());

						DrawTransparent(&dc, rcDestLeft.left, rcDestLeft.top, rcDestLeft.Width(), rcDestLeft.Height(), &bmpLine, rcLineBlockLeft.left, rcLineBlockLeft.top, RGB(255, 0, 255));
					}
					if(rcDestRight.IntersectRect(rcVisible, rcFrameRight))
					{
						rcLineBlockRight.left	+=	rcDestRight.left - rcFrameRight.left;
						rcLineBlockRight.top		+=	rcDestRight.top - rcFrameRight.top;
						rcLineBlockRight.right	-=	rcFrameRight.right - rcDestRight.right;
						rcLineBlockRight.bottom	-=	rcFrameRight.bottom - rcDestRight.bottom;

						ASSERT(rcLineBlockRight.Width()==rcDestRight.Width());
						ASSERT(rcLineBlockRight.Height()==rcDestRight.Height());

						DrawTransparent(&dc, rcDestRight.left, rcDestRight.top, rcDestRight.Width(), rcDestRight.Height(), &bmpLine, rcLineBlockRight.left, rcLineBlockRight.top, RGB(255, 0, 255));
					}
				}
				else
				{
					if(nFrame == nStartFrame)
					{
					}
					else if(nFrame == nEndFrame - 1)
					{
						rcLineBlock.OffsetRect(bm.bmWidth - m_sizeFrame.cx, 0);
					}
					else
					{
						int nLineFrames = bm.bmWidth / m_sizeFrame.cx;
						rcLineBlock.OffsetRect(m_sizeFrame.cx * (1 + (nFrame - nStartFrame - 1) % (nLineFrames - 2)), 0);
						#if !defined(TRANS_DRAW_LINE)
						{
							rcFrame.DeflateRect(0, nTopMargin, 0, nBottomMargin);
							rcLineBlock.DeflateRect(0, nTopMargin, 0, nBottomMargin);
							if(rcDest.IntersectRect(rcVisible, rcFrame)==FALSE)
							{
								break;
							}
						}
						#endif
					}

					rcLineBlock.left	+=	rcDest.left - rcFrame.left;
					rcLineBlock.top		+=	rcDest.top - rcFrame.top;
					rcLineBlock.right	-=	rcFrame.right - rcDest.right;
					rcLineBlock.bottom	-=	rcFrame.bottom - rcDest.bottom;

					ASSERT(rcLineBlock.Width()==rcDest.Width());
					ASSERT(rcLineBlock.Height()==rcDest.Height());

					if(nFrame == nStartFrame || nFrame == nEndFrame -1)
					{
						DrawTransparent(&dc, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), &bmpLine, rcLineBlock.left, rcLineBlock.top, RGB(255, 0, 255));
					}
					else
					{
						#if defined(TRANS_DRAW_LINE)
							DrawTransparent(&dc, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), &bmpLine, rcLineBlock.left, rcLineBlock.top, RGB(255, 0, 255));
						#else
							DrawBitmap(dc.m_hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), (HBITMAP)bmpLine, rcLineBlock.left, rcLineBlock.top);
						#endif
					}
				}
			}
		}
	}
	
	if(COptions::Instance()->m_bShowTimelineObjName && lpszText && _tcslen(lpszText))
	{
		CRect rcRange(rc.left + (nStartFrame - nFirstVisibleFrame + 1) * m_sizeFrame.cx, rc.top, rc.left + (nEndFrame - nFirstVisibleFrame - 1) * m_sizeFrame.cx, rc.bottom);
		CRect rcTemp;
		if (rcTemp.IntersectRect(&rcVisible, &rcRange))
		{
			CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			CFont* pOldFont = dc.SelectObject(pFont);
			dc.SetTextColor(bSel ? clrTextSel: clrText);
			dc.DrawText(lpszText, &rcRange, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			dc.SelectObject(&pOldFont);
		}
	}
}

void CEffectWnd::PaintEffectLineRow(CDC& dc, GEFFECT_PAIR& gPair)
{
	BOOL bTracking = FALSE;
	if (m_bTrackingTimeLine)
	{
		int nSelType = GetSelType();
		if (nSelType == SEL_INST)
		{
			bTracking = (m_pSelInstance == *gPair.iterInstance);
		}
		else if (nSelType == SEL_EFFECT)
		{
			bTracking = (m_pSelEffect == *gPair.iterEffect);
		}
	}

	gldInstance* pInstance = InstanceFromPair(gPair);
	gldEffect* pEffect = EffectFromPair(gPair);
	BOOL bSel = IsInstSelected(pInstance) || IsEffectSelected(pInstance, pEffect);
	int nStartFrame = bTracking ? pEffect->m_startTimeTracking : pEffect->m_startTime;
	int nEndFrame = bTracking ? pEffect->m_startTimeTracking + pEffect->m_lengthTracking : pEffect->m_startTime + pEffect->m_length;

	PaintTimeLineRowBase(dc, gPair.nIndex, bSel, nStartFrame, nEndFrame, m_bmpTiles, m_bmpEffectLine, 
		pEffect->GetName().c_str(), 2, 3, m_clrEffectLineTextSel, m_clrEffectLineText);
}

void CEffectWnd::PaintInstLineRow(CDC& dc, GINSTANCE_PAIR& gPair)
{
	gldInstance* pInstance = InstanceFromPair(gPair);
	BOOL bSel = IsInstSelected(pInstance);

	SIZE sz = GetAllEffectsRange(pInstance);

	PaintTimeLineRowBase(dc, gPair.nIndex, bSel, sz.cx, sz.cy, m_bmpTiles, m_bmpEffectTotal, NULL, 5, 8);
}

void CEffectWnd::PaintSoundLineRow(CDC& dc, GSOUND_PAIR& gPair)
{
	BOOL bTracking = FALSE;
	if (m_bTrackingTimeLine)
	{
		int nSelType = GetSelType();
		if (nSelType == SEL_SCAPT)
		{
			bTracking = TRUE;
		}
		else if (nSelType == SEL_SOUND)
		{
			bTracking = (m_pSelSound == *gPair.iterSound);
		}
	}

	gldSceneSound* pSound = SoundFromPair(gPair);
	BOOL bSel = IsScaptSelected() || (IsSoundSelected(pSound));

	int nStartFrame = bTracking ? pSound->m_startTimeTracking : pSound->m_startTime;
	int nEndFrame = bTracking ? pSound->m_startTimeTracking + pSound->m_lengthTracking : pSound->m_startTime + pSound->m_length;

	PaintTimeLineRowBase(dc, gPair.nIndex, bSel, nStartFrame, nEndFrame, m_bmpTiles, m_bmpSoundLine, pSound->m_sound->m_soundObj->m_name.c_str(), 2, 3, m_clrSoundLineTextSel, m_clrSoundLineText);
}

void CEffectWnd::PaintScaptLineRow(CDC& dc, GSCAPT_PAIR& gPair)
{
	BOOL bSel = IsScaptSelected();

	gldScene2* pScene = _GetCurScene2();
	SIZE sz = GetAllSoundsRange(pScene);

	PaintTimeLineRowBase(dc, gPair.nIndex, bSel, sz.cx, sz.cy, m_bmpTiles, m_bmpSoundTotal, NULL, 5, 8);
}

void CEffectWnd::PaintEffectBarRow(CDC& dc, GEFFECT_PAIR& gPair)
{
	gldInstance* pInstance = InstanceFromPair(gPair);
	gldEffect* pEffect = EffectFromPair(gPair);

	CRect rc;
	// 得到可每个INSTANCE特效位置对应的矩形
	GetEffectBarRowRect(rc, gPair.nIndex);

	//是不是可见的行
	if(IsRowVisible(gPair.nIndex))
	{
		BOOL bSel = IsInstSelected(pInstance) || IsEffectSelected(pInstance, pEffect);
		if(bSel)
		{
			dc.FillSolidRect(&rc, RGB(255, 255, 255));
		}
		CRect rc2(rc);

		// Draw Expanded/Collapsed Bitmap
		{
			int nImage = 1;
			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstExpanded.GetImageInfo(nImage, &info);

			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
//			::DrawTransparent(&dc, rc2.left, rc2.top + nOffset, bm.bmWidth, bm.bmHeight, &bmp, 0, 0, RGB(255, 0, 255));
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Expanded Line Bitmap
		{
			int nImage = (*pInstance->m_effectList.rbegin()==pEffect ? 1 : 0);
			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstTreeLine.GetImageInfo(nImage, &info);

			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
			m_lstTreeLine.Draw(&dc, nImage, CPoint(rc2.left, rc2.top + nOffset), ILD_NORMAL);
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Effect Icon Bitmap
		{
			CBitmap& bmp = pEffect->m_pEffectInfo ? pEffect->m_pEffectInfo->m_bitmap : m_bmpEffectIcon;
			BITMAP bm;
			memset(&bm, 0, sizeof(bm));
			bmp.GetBitmap(&bm);
			int nOffset = (m_sizeFrame.cy - bm.bmHeight) / 2;
			rc2.left += nOffset;
			::DrawTransparent(&dc, rc2.left, rc2.top + nOffset, bm.bmWidth, bm.bmHeight, &bmp, 0, 0, RGB(255, 0, 255));
			rc2.left += nOffset + bm.bmWidth;
		}

		// Draw Text
		{
			CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			CFont* pOldFont = dc.SelectObject(pFont);

			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

			dc.DrawText(pEffect->GetName().c_str(), &rc2, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			dc.SelectObject(pOldFont);
		}
	}
}

void CEffectWnd::PaintInstBarRow(CDC& dc, GINSTANCE_PAIR& gPair)
{
	gldInstance* pInstance = InstanceFromPair(gPair);

	CRect rc;
	GetEffectBarRowRect(rc, gPair.nIndex);

	if(IsRowVisible(gPair.nIndex))
	{
		BOOL bSel = IsInstSelected(pInstance);
		if(bSel)
		{
			dc.FillSolidRect(&rc, RGB(255, 255, 255));
		}
		CRect rc2(rc);

		// Draw Expanded/Collapsed Bitmap
		{
			int nImage = pInstance->m_expanded ? 1 : 0;
			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstExpanded.GetImageInfo(nImage, &info);

			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
			if(pInstance->m_effectList.size())
			{
				m_lstExpanded.Draw(&dc, nImage, CPoint(rc2.left, rc2.top + nOffset), ILD_NORMAL);
			}
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Instance Type Icon Bitmap
		int nImage;
		UINT nTypeID = 0;
		gldObj* pObj = pInstance->m_obj;
		switch (pObj->GetGObjType())
		{
		case gobjShape:
			nImage = INDEX_SHAPE;
			nTypeID = IDS_SYMBOL_SHAPE;
			break;

		case gobjSprite:
			{
				gldSprite *pSprite = (gldSprite *)pObj;
				if (pSprite->m_isGroup)
				{
					nImage = INDEX_GROUP;
					nTypeID = IDS_SYMBOL_GROUP;
				}
				else
				{
					nImage = INDEX_SPRITE;
					nTypeID = IDS_SYMBOL_MOVIE_CLIP;
				}
				break;
			}

		case gobjButton:
			nImage = INDEX_BUTTON;
			nTypeID = IDS_SYMBOL_BUTTON;
			break;

		case gobjText:
			nImage = INDEX_TEXT;
			nTypeID = IDS_SYMBOL_TEXT;
			break;

		default:
			ASSERT(FALSE);
			nImage = INDEX_SHAPE;
			nTypeID = IDS_SYMBOL_SHAPE;
		}

		IMAGEINFO info;
		memset(&info, 0, sizeof(info));
		m_lstInstanceIcon.GetImageInfo(nImage, &info);

		int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
		rc2.left += nOffset;
		m_lstInstanceIcon.Draw(&dc, nImage, CPoint(rc2.left, rc2.top + nOffset), ILD_NORMAL);
		rc2.left += nOffset + info.rcImage.right - info.rcImage.left;

		// Draw Text
		CString str;
		if (!pInstance->m_name.empty())
			str = pInstance->m_name.c_str();
		else
			str.LoadString(nTypeID);

		CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		CFont* pOldFont = dc.SelectObject(pFont);

		dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

		dc.DrawText(str, &rc2, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		dc.SelectObject(pOldFont);
	}
}

void CEffectWnd::PaintScaptBarRow(CDC& dc, GSCAPT_PAIR& gPair)
{
	gldScene2* pScene = _GetCurScene2();
	CRect rc;
	GetEffectBarRowRect(rc, gPair.nIndex);

	if(IsRowVisible(gPair.nIndex))
	{
		BOOL bSel = IsScaptSelected();
		if(bSel)
		{
			dc.FillSolidRect(&rc, RGB(255, 255, 255));
		}
		CRect rc2(rc);

		// Draw Expanded/Collapsed Bitmap
		{
			int nImage = pScene->m_soundExpanded ? 1 : 0;
			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstExpanded.GetImageInfo(nImage, &info);

			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
			m_lstExpanded.Draw(&dc, nImage, CPoint(rc2.left, rc2.top + nOffset), ILD_NORMAL);
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Sound Icon Bitmap
		{
			BITMAP bm;
			memset(&bm, 0, sizeof(BITMAP));
			m_bmpSoundIcon.GetBitmap(&bm);

			int nOffset = (m_sizeFrame.cy - bm.bmHeight) / 2;
			rc2.left += nOffset;
			DrawTransparent(&dc, rc2.left, rc2.top + nOffset, bm.bmWidth, bm.bmHeight, &m_bmpSoundIcon, 0, 0, RGB(255, 0, 255));
			rc2.left += nOffset + bm.bmWidth;
		}

		// Draw Text
		{
			CString str;
			str.LoadString(IDS_SCENE_SOUND_CAPTION);

			CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			CFont* pOldFont = dc.SelectObject(pFont);

			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

			dc.DrawText(str, &rc2, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			dc.SelectObject(pOldFont);
		}
	}
}

void CEffectWnd::PaintSoundBarRow(CDC& dc, GSOUND_PAIR& gPair)
{
	gldScene2* pScene = _GetCurScene2();
	gldSceneSound* pSound = SoundFromPair(gPair);

	CRect rc;
	GetEffectBarRowRect(rc, gPair.nIndex);

	if(IsRowVisible(gPair.nIndex))
	{
		BOOL bSel = IsScaptSelected() || (IsSoundSelected(pSound));
		if(bSel)
		{
			dc.FillSolidRect(&rc, RGB(255, 255, 255));
		}
		CRect rc2(rc);

		// Draw Expanded/Collapsed Bitmap
		{
			int nImage = 1;
			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstExpanded.GetImageInfo(nImage, &info);

			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
			//::DrawTransparent(&dc, rc2.left, rc2.top + nOffset, bm.bmWidth, bm.bmHeight, &bmp, 0, 0, RGB(255, 0, 255));
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Expanded Line Bitmap
		{
			int nImage = (*pScene->m_soundList.rbegin()==pSound ? 1 : 0);
			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstTreeLine.GetImageInfo(nImage, &info);
			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
			m_lstTreeLine.Draw(&dc, nImage, CPoint(rc2.left, rc2.top + nOffset), ILD_NORMAL);
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Sound Icon Bitmap
		{
			int nImage = pSound->m_sound->m_soundObj->GetSoundFormat()==SOUND_FORMAT_MP3 ? 1 : 2;

			IMAGEINFO info;
			memset(&info, 0, sizeof(info));
			m_lstSoundList.GetImageInfo(nImage, &info);

			int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;
			rc2.left += nOffset;
			m_lstSoundList.Draw(&dc, nImage, CPoint(rc2.left, rc2.top + nOffset), ILD_NORMAL);
			rc2.left += nOffset + info.rcImage.right - info.rcImage.left;
		}

		// Draw Text
		{
			CString str = pSound->m_sound->m_soundObj->m_name.c_str();

			CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			CFont* pOldFont = dc.SelectObject(pFont);

			dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

			dc.DrawText(str, &rc2, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			dc.SelectObject(pOldFont);
		}
	}
}

int CEffectWnd::FrameFromPos(int x)
{
	CRect rc;
	GetTimeLineRect(rc);

	int nFrame = m_sbHorz.GetScrollPos();
	nFrame += (x - rc.left) / m_sizeFrame.cx;
	if((x - rc.left) % m_sizeFrame.cx)
		++nFrame;
	return nFrame;
}

void CEffectWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CWnd *pWnd = AfxGetMainWnd();
	if (pWnd != NULL && !pWnd->IsIconic())
	{
		RecalcLayout();

		if(_GetMainMovie2() && _GetMainMovie2()->GetCurScene())
		{
			RecalcScrollPos();
			EnsureSelectionVisible();
		}
	}
}

void CEffectWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar == &m_sbHorz);

	SCROLLINFO si;
	m_sbHorz.GetScrollInfo(&si, SIF_ALL);

	int nMin = si.nMin;
	int nMax = si.nMax;
	int nPage = si.nPage;
	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = min(nMax - nPosOld, HSB_LINE_SIZE);
		nPosNew = nPosOld + ds;
		break;
	case SB_LINEUP:
		ds = min(nPosOld, HSB_LINE_SIZE);
		nPosNew = nPosOld - ds;
		break;
	case SB_PAGEDOWN:
		ds = min(nMax - nPosOld, nPage);
		nPosNew = nPosOld + ds;
		break;
	case SB_PAGEUP:
		ds = min(nPosOld, nPage);
		nPosNew = nPosOld - ds;
		break;
	case SB_TOP:
		nPosNew = 0;
		break;
	case SB_BOTTOM:
		nPosNew = nMax;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nPosNew = si.nTrackPos;
		TRACE1("%d\n", nPosNew);
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
		m_sbHorz.SetScrollInfo(&si, TRUE);

		if (!m_bTrackingTimeLine)
			Invalidate(FALSE);
		//UpdateWindow();
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEffectWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar == &m_sbVert);

	SCROLLINFO si;
	m_sbVert.GetScrollInfo(&si, SIF_ALL);

	int nMin = si.nMin;
	int nMax = si.nMax;
	int nPage = si.nPage;
	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = min(nMax - nPosOld, VSB_LINE_SIZE);
		nPosNew = nPosOld + ds;
		break;
	case SB_LINEUP:
		ds = min(nPosOld, VSB_LINE_SIZE);
		nPosNew = nPosOld - ds;
		break;
	case SB_PAGEDOWN:
		ds = min(nMax - nPosOld, nPage);
		nPosNew = nPosOld + ds;
		break;
	case SB_PAGEUP:
		ds = min(nPosOld, nPage);
		nPosNew = nPosOld - ds;
		break;
	case SB_TOP:
		nPosNew = 0;
		break;
	case SB_BOTTOM:
		nPosNew = nMax;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ds = nPos - nPosOld;
		nPosNew = nPosOld + ds;
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
		m_sbVert.SetScrollInfo(&si, TRUE);

		Invalidate(FALSE);
		//UpdateWindow();
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEffectWnd::PaintTimeLine(CDC& dc)
{
	TRACE("PaintTimeLine\n");

	CRect rc;
	GetTimeLineRect(rc);

//	return ;

	dc.FillRect(&rc, &m_brWhite);

	int nRowCount = 0;


	for(GSCAPT_PAIR gScapt = GetBeginScaptPair(); !IsEndPair(gScapt); gScapt = GetNextScaptPair(gScapt))
	{
		++nRowCount;

		if(IsRowVisible(gScapt.nIndex))
		{
			PaintScaptLineRow(dc, gScapt);
		}
	}

	for(GSOUND_PAIR gSound = GetBeginSoundPair(); !IsEndPair(gSound); gSound = GetNextSoundPair(gSound))
	{
		++nRowCount;

		if(IsRowVisible(gSound.nIndex))
		{
			PaintSoundLineRow(dc, gSound);
		}
	}

	for(GINSTANCE_PAIR gInst = GetBeginInstPair(); !IsEndPair(gInst); gInst = GetNextInstPair(gInst))
	{
		++nRowCount;

		if(IsRowVisible(gInst.nIndex))
		{
			PaintInstLineRow(dc, gInst);
		}
	}

	for(GEFFECT_PAIR gEffect = GetBeginEffectPair(); !IsEndPair(gEffect); gEffect = GetNextEffectPair(gEffect))
	{
		++nRowCount;

		if(IsRowVisible(gEffect.nIndex))
		{
			PaintEffectLineRow(dc, gEffect);
		}
	}

	if(nRowCount==0)
	{
		rc.InflateRect(- m_sizeFrame.cx, -m_sizeFrame.cy);

		if(rc.IsRectEmpty()==FALSE)
		{
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(m_clrEffectLineTextSel);

			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			CFont font;
			font.Attach(hFont);

			CFont* pOldFont = dc.SelectObject(&font);
			CString str;
			str.LoadString(IDS_PROMPT_ADD_ANIMATED_EFFECTS);
			dc.DrawText(str, rc, DT_CENTER|DT_TOP|DT_WORDBREAK);
			dc.SelectObject(pOldFont);

			font.Detach();
		}
	}
}

void CEffectWnd::PaintTimeLineTitle(CDC& dc)
{
	TRACE("PaintTimeLineTitle\n");

	CRect rc;
	GetTimeLineTitleRect(rc);

	int nFirstVisibleFrame	= GetFirstVisibleFrame();
	int nVisibleFrames		= GetVisibleFrames();
	
	dc.FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));
	dc.DrawEdge(&rc, BDR_RAISEDINNER, BF_BOTTOM);

	int nFirst = GetFirstVisibleFrame();

	CPen *pOldPen = dc.SelectObject(&m_penDarkGray);

	CFont *pOldFont = dc.SelectObject(&m_fntFrameNum);
	COLORREF crOldText = dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

	int x = rc.left;
	for (int i = 0; i < GetVisibleFrames(); i++)
	{
		//dc.MoveTo(x + m_sizeFrame.cx, rc.top + 1);
		//dc.LineTo(x + m_sizeFrame.cx, rc.top + 3 + 1);

		dc.MoveTo(x + m_sizeFrame.cx - 1, rc.bottom - 3 - 2);
		dc.LineTo(x + m_sizeFrame.cx - 1, rc.bottom - 2);

		if (nFirst + i == 0 || ((nFirst + i + 1) % 5)== 0)
		{
			CRect rcNumber(x - m_sizeFrame.cx * 2, rc.top + 2, x + m_sizeFrame.cx * 3, rc.bottom);
			
			TCHAR szNumber[11];	// #define INT_MAX 2147483647
			itoa(nFirst + i + 1, szNumber, 10);

			dc.DrawText(szNumber, (int)_tcslen(szNumber), rcNumber, DT_SINGLELINE | DT_CENTER | DT_TOP);
		}

		x += m_sizeFrame.cx;
	}

	dc.SetTextColor(crOldText);
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
}

void CEffectWnd::PaintIndicatorFrame(CDC& dc)
{
}

void CEffectWnd::PaintEffectBar(CDC& dc)
{
	TRACE("PaintEffectBar\n");

	CRect rc;
	GetEffectBarRect(rc);
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));

	for(GSCAPT_PAIR gScapt = GetBeginScaptPair(); !IsEndPair(gScapt); gScapt = GetNextScaptPair(gScapt))
	{
		if(IsRowVisible(gScapt.nIndex))
		{
			PaintScaptBarRow(dc, gScapt);
		}
	}

	for(GSOUND_PAIR gSound = GetBeginSoundPair(); !IsEndPair(gSound); gSound = GetNextSoundPair(gSound))
	{
		if(IsRowVisible(gSound.nIndex))
		{
			PaintSoundBarRow(dc, gSound);
		}
	}

	for(GINSTANCE_PAIR gInst = GetBeginInstPair(); !IsEndPair(gInst); gInst = GetNextInstPair(gInst))
	{
		if(IsRowVisible(gInst.nIndex))
		{
			PaintInstBarRow(dc, gInst);
		}
	}


	for(GEFFECT_PAIR gEffect = GetBeginEffectPair(); !IsEndPair(gEffect); gEffect = GetNextEffectPair(gEffect))
	{
		if(IsRowVisible(gEffect.nIndex))
		{
			PaintEffectBarRow(dc, gEffect);
		}
	}
}

void CEffectWnd::PaintEffectBarTitle(CDC& dc)
{
	TRACE("PaintEffectBarTitle\n");

	CRect rc;
	GetEffectBarTitleRect(rc);

	dc.FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));
	dc.DrawEdge(&rc, BDR_RAISEDINNER, BF_BOTTOM);

	//CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	//CFont* pOldFont = dc.SelectObject(pFont);
	//dc.DrawText(" Effect/Sound", &rc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	//dc.SelectObject(pOldFont);
}

void CEffectWnd::PaintWidgets(CDC& dc)
{
	CRect rc;
	GetEffectBarPaneRect(rc);
	//rc.right += 1;	// overlap 1 pixels of the splitter
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
	//dc.DrawEdge(&rc, BDR_RAISEDINNER, BF_TOP);
	dc.DrawEdge(&rc, BDR_SUNKENOUTER, BF_TOP);

	GetHScrollBarPaneRect(rc);
	//rc.left -= 1;
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
	//dc.DrawEdge(&rc, BDR_RAISEDINNER, BF_TOP);
	dc.DrawEdge(&rc, BDR_SUNKENOUTER, BF_TOP);
}

void CEffectWnd::PaintSplitter(CDC& dc)
{
	TRACE("PaintSplitter\n");

	CRect rc;
	GetSplitterRect(rc);
	dc.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
	//dc.DrawEdge(&rc, EDGE_RAISED, BF_LEFT | BF_RIGHT);
	rc.top--;
	rc.bottom++;
	dc.Draw3dRect(&rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

int CEffectWnd::GetFirstVisibleRow()
{
	int nPos = m_sbVert.GetScrollPos();
	return nPos;
}

int CEffectWnd::GetLastVisibleRow()
{
	if (GetRowsCount() > 0)
		return min(GetRowsCount() - 1, GetFirstVisibleRow() + GetVisibleRows() - 1);
	return -1;
}

int CEffectWnd::GetLastWholeVisibleRow()
{
	CRect rc;
	GetTimeLineRect(rc);
	return min(GetFirstVisibleRow() + (rc.Height() / m_sizeFrame.cy) -1, GetRowsCount() -1);
}

int CEffectWnd::GetVisibleRows()
{
	CRect rc;
	GetTimeLineRect(rc);
	int count = rc.Height() / m_sizeFrame.cy;
	if (rc.Height() % m_sizeFrame.cy > 0)
		count++;
	return count;
}

BOOL CEffectWnd::IsLastVisibleRowPartInvisible()
{
	CRect rc;
	GetTimeLineRect(rc);
	return rc.Height() % m_sizeFrame.cy;
}



BOOL CEffectWnd::IsRowVisible(int index)
{
	return index >= GetFirstVisibleRow() && index <= GetLastVisibleRow();
}

int CEffectWnd::GetFrameCount()
{
	return m_nTotalFrames;
}

int CEffectWnd::GetFirstVisibleFrame()
{
	return m_sbHorz.GetScrollPos();
}

int CEffectWnd::GetLastVisibleFrame()
{
	return GetFirstVisibleFrame() + GetVisibleFrames() - 1;
}

int CEffectWnd::GetVisibleFrames()
{
	CRect rc;
	GetTimeLineRect(rc);
	int count = rc.Width() / m_sizeFrame.cx;
	if (rc.Width() % m_sizeFrame.cx > 1)
		count++;
	return count;
}

BOOL CEffectWnd::IsLastVisibleFramePartInvisible()
{
	CRect rc;
	GetTimeLineRect(rc);
	return rc.Width() % m_sizeFrame.cx;
}

BOOL CEffectWnd::IsFrameVisible(int nFrame)
{
	return nFrame <= GetLastVisibleFrame() && nFrame >= GetLastVisibleFrame();
}

void CEffectWnd::GetTimeLineRect(CRect &rc)
{
	GetClientRect(&rc);
	rc.left += (m_nSplitterPos + SPLITTER_SIZE / 2);
	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);
	rc.top += m_nTitleHeight;
	rc.bottom -= (::GetSystemMetrics(SM_CYHSCROLL) + HSCROLL_MARGIN * 2);

//	rc.top--;
}

void CEffectWnd::GetTimeLineTitleRect(CRect &rc)
{
	GetClientRect(&rc);
	rc.left += (m_nSplitterPos + SPLITTER_SIZE / 2);
	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);
	rc.bottom = rc.top + m_nTitleHeight;
}

void CEffectWnd::GetEffectBarRect(CRect &rc)
{
	GetClientRect(&rc);
	rc.top += m_nTitleHeight;
	rc.right = rc.left + m_nSplitterPos - SPLITTER_SIZE / 2;
	rc.bottom -= (::GetSystemMetrics(SM_CYHSCROLL) + HSCROLL_MARGIN);
}

void CEffectWnd::GetExpandIconRect(CRect& rc, int nRow)
{
	IMAGEINFO info;
	memset(&info, 0, sizeof(info));
	m_lstExpanded.GetImageInfo(0, &info);

	int nOffset = (m_sizeFrame.cy - (info.rcImage.bottom - info.rcImage.top)) / 2;

	//nRow 是个序列数，还是个具体位置值？？ 
	GetEffectBarRowRect(rc, nRow);

	//计算出”田“的 rect 
	rc.left	+= nOffset;
	rc.top	+= nOffset;
	rc.right	= rc.left + info.rcImage.right - info.rcImage.left;
	rc.bottom	= rc.top+ info.rcImage.bottom - info.rcImage.top;
}

void CEffectWnd::GetEffectBarTitleRect(CRect &rc)
{
	GetClientRect(&rc);
	rc.right = rc.left + m_nSplitterPos - SPLITTER_SIZE / 2;
	rc.bottom = rc.top + m_nTitleHeight;
}

void CEffectWnd::GetSplitterRect(CRect &rc)
{
	GetClientRect(&rc);
	rc.left = m_nSplitterPos - SPLITTER_SIZE / 2;
	rc.right = m_nSplitterPos + SPLITTER_SIZE / 2;
}

void CEffectWnd::GetHScrollBarPaneRect(CRect& rc)
{
	GetClientRect(&rc);
	rc.left += m_nSplitterPos + SPLITTER_SIZE / 2;
	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);
	rc.top = rc.bottom - (HSCROLL_MARGIN * 2 + ::GetSystemMetrics(SM_CYHSCROLL));
}

void CEffectWnd::GetVScrollBarPaneRect(CRect& rc)
{
	GetClientRect(&rc);

	rc.left = rc.right - ::GetSystemMetrics(SM_CYHSCROLL);
	rc.bottom -= ::GetSystemMetrics(SM_CXVSCROLL);
}

void CEffectWnd::GetEffectBarPaneRect(CRect &rc)
{
	GetClientRect(&rc);
	rc.top = rc.bottom - (::GetSystemMetrics(SM_CYHSCROLL) + HSCROLL_MARGIN * 2);
	rc.right = rc.left + m_nSplitterPos - SPLITTER_SIZE / 2;
}

void CEffectWnd::GetEffectBarRowRect(CRect& rc, int nIndex)
{
	GetEffectBarRect(rc);
/*
GetFirstVisibleRow（）：
int nPos = m_sbVert.GetScrollPos();
return nPos;
*/
	rc.top += (nIndex - GetFirstVisibleRow()) * m_sizeFrame.cy;
	rc.bottom = rc.top + m_sizeFrame.cy;
}


void CEffectWnd::GetTimeLineRowRect(CRect &rc, int index)
{
/*
简单地以 SPLITTER 左侧为TIMELINE 区。
GetTimeLineRect(rc)：
	GetClientRect(&rc);
	rc.left += (m_nSplitterPos + SPLITTER_SIZE / 2);
	rc.right -= ::GetSystemMetrics(SM_CXVSCROLL);
	rc.top += m_nTitleHeight;
	rc.bottom -= (::GetSystemMetrics(SM_CYHSCROLL) + HSCROLL_MARGIN * 2);
*/
	GetTimeLineRect(rc);

	/*
	GetFirstVisibleRow():
	int nPos = m_sbVert.GetScrollPos();
	return nPos;
	*/
	rc.top += (index - GetFirstVisibleRow()) * m_sizeFrame.cy;
	rc.bottom = rc.top + m_sizeFrame.cy;
}

void CEffectWnd::CreateDrawTools()
{
	// pens
	m_penDarkGray.CreatePen(PS_SOLID, 1, crDarkGray);


	// brushes
	m_brWhite.CreateSolidBrush(RGB(255, 255, 255));
	m_brThickBorder.Attach(CreatePatternBrush(AfxGetInstanceHandle(), IDB_BRUSH_SPLITTER));

	LOADBMPRES(m_bmpSoundTotal,  "res\\sound_t.bmp",  IDB_EFFWND_SOUND_TOTAL);
	LOADBMPRES(m_bmpEffectTotal, "res\\effwnd_s.bmp", IDB_EFFWND_EFFECT_TOTAL);
	LOADBMPRES(m_bmpSoundLine,   "res\\sound_l.bmp",  IDB_EFFWND_SOUND_LINE);
	LOADBMPRES(m_bmpEffectLine,  "res\\effect_l.bmp", IDB_EFFWND_EFFECT_LINE);
	LOADBMPRES(m_bmpTiles,       "res\\bg_tiles.bmp", IDB_EFFWND_BACKGROUND_TILES);
}

void CEffectWnd::ReleaseDrawTools()
{
	// bitmaps

	// pens
	m_penDarkGray.DeleteObject();

	// brushes
	m_brWhite.DeleteObject();

	m_brThickBorder.DeleteObject();
	
	m_bmpTiles.DeleteObject();
	m_bmpSoundTotal.DeleteObject();
	m_bmpEffectTotal.DeleteObject();
	m_bmpSoundLine.DeleteObject();
	m_bmpEffectLine.DeleteObject();
}


BOOL CEffectWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ((GetKeyState(VK_RBUTTON) & 0x8000) == 0)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);

		CRect rc;

		GetSplitterRect(rc);
		if (rc.PtInRect(point))
		{
			::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR));
			return TRUE;
		}

		if(_GetCurScene2() == NULL)
		{
			return CWnd::OnSetCursor(pWnd, nHitTest, message);
		}

		GetEffectBarRect(rc);
		if (rc.PtInRect(point))
		{
			for(GEFFECT_PAIR gEffect = GetBeginEffectPair(); !IsEndPair(gEffect); gEffect = GetNextEffectPair(gEffect))
			{
				if(IsRowVisible(gEffect.nIndex))
				{
					GetEffectBarRowRect(rc, gEffect.nIndex);
					if(rc.PtInRect(point))
					{
						::SetCursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HAND_POINT)));
						return TRUE;
					}
				}
			}
			for(GINSTANCE_PAIR gInst = GetBeginInstPair(); !IsEndPair(gInst); gInst = GetNextInstPair(gInst))
			{
				if(IsRowVisible(gInst.nIndex))
				{
					GetEffectBarRowRect(rc, gInst.nIndex);
					if(rc.PtInRect(point))
					{
						::SetCursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HAND_POINT)));
						return TRUE;
					}
				}
			}
			for(GSOUND_PAIR gSound = GetBeginSoundPair(); !IsEndPair(gSound); gSound = GetNextSoundPair(gSound))
			{
				if(IsRowVisible(gSound.nIndex))
				{
					GetEffectBarRowRect(rc, gSound.nIndex);
					if(rc.PtInRect(point))
					{
						::SetCursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HAND_POINT)));
						return TRUE;
					}
				}
			}
			for(GSCAPT_PAIR gScapt = GetBeginScaptPair(); !IsEndPair(gScapt); gScapt = GetNextScaptPair(gScapt))
			{
				if(IsRowVisible(gScapt.nIndex))
				{
					GetEffectBarRowRect(rc, gScapt.nIndex);
					if(rc.PtInRect(point))
					{
						::SetCursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HAND_POINT)));
						return TRUE;
					}
				}
			}
		}

		GetTimeLineRect(rc);
		if (rc.PtInRect(point))
		{
			UINT nTrackType = TRACK_VOID;
			GEFFECT_PAIR gPair = EffectPairFromPos(point.y);
			if(!IsEndPair(gPair))
			{
				nTrackType = GetTrackType(gPair, point);
			}
			else
			{
				GINSTANCE_PAIR gPair = InstPairFromPos(point.y);
				if(!IsEndPair(gPair))
				{
					nTrackType = GetTrackType(gPair, point);
				}
				else
				{
					GSOUND_PAIR gPair = SoundPairFromPos(point.y);
					if(!IsEndPair(gPair))
					{
						nTrackType = GetTrackType(gPair, point);
					}
					else
					{
						GSCAPT_PAIR gPair = ScaptPairFromPos(point.y);
						if(!IsEndPair(gPair))
						{
							nTrackType = GetTrackType(gPair, point);
						}
					}
				}
			}
				
			switch(nTrackType)
			{
			case TRACK_MOVE:
				// 移动光标
				::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND_DRAG));
				return TRUE;
				break;
			case TRACK_RESIZE_LEFT:
				// 左调光标
				::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR));
				return TRUE;
				break;
			case TRACK_RESIZE_RIGHT:
				// 右调光标
				::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR));
				return TRUE;
				break;
			default:
				break;
			}
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CEffectWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	SetFocus();
	SetActive(TRUE);

	// 如果鼠标在Splitter区间,则处理鼠标事件改变Splitter的位置
	CRect rc;
	GetSplitterRect(rc);
	if (rc.PtInRect(point))
	{
		TrackSplitter(point);
		return;
	}

	gldScene2 *pScene = _GetCurScene2();
	if (pScene == NULL)
	{
		return;
	}

	ProcessMouseEvent(WM_LBUTTONDOWN, nFlags, point);
}

void CEffectWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);

	SetFocus();
	SetActive(TRUE);

	gldScene2 *pScene = _GetCurScene2();
	if (pScene == NULL)
	{
		return;
	}

	ProcessMouseEvent(WM_RBUTTONDOWN, nFlags, point);
}

void CEffectWnd::SetActive(BOOL bActive)
{
	if (m_bActive != bActive)
	{
		m_bActive = bActive;

		Invalidate();
		UpdateWindow();
	}
}
//拖动特效节点是的消息追踪处理
void CEffectWnd::TrackMoveEffect(GEFFECT_PAIR& gPair, const CPoint& point)
{
	// 要先更新窗口,因为当前特效的选择改变会导致在空闲的时候会刷新窗口
	UpdateWindow();

	SetCapture();
	SetFocus();

	CRect rc;
	GetEffectBarRect(rc);

	CDC *pDC = GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
	CBrush *pOldBrush = pDC->SelectObject(&m_brThickBorder);

	gldInstance* pInstance = InstanceFromPair(gPair);
	gldEffect* pEffect = EffectFromPair(gPair);
	int nOldIndex = gPair.nIndex;
	int nFirstIndex = nOldIndex;
	int nLastIndex = nOldIndex;
	GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
	GEFFECT_LIST::iterator iter2;
	for(iter2 = iter; iter2 != pInstance->m_effectList.begin(); --iter2)
	{
		--nFirstIndex;
	}
	for(iter2 = iter; iter2 != pInstance->m_effectList.end(); ++iter2)
	{
		++nLastIndex;
	}

	BOOL bDraging = TRUE;		// 表示是否正在拖放

	// 绘制拖动矩形
	if(bDraging)
	{
		DrawMoveEffectDragRect(pDC, gPair, nOldIndex, FALSE);
	}

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			{
				if(bDraging)
				{
					// 记录上一次操作是否可以移动特效
					BOOL bOldCanMoveEffect = CanMoveEffect(gPair, nOldIndex);

					// 绘制旧的矩形区域
					if(!m_bScrolled)
					{
						TRACE1("Draw Move Effect: MOUSEMOVE_BEFORE %d\r\n", nOldIndex);
						DrawMoveEffectDragRect(pDC, gPair, nOldIndex, FALSE);
					}

					// 计算新的帧
					int y = max(rc.top, min(rc.bottom, GET_Y_LPARAM(msg.lParam)));
					// 根据坐标来计算索引位置
					nOldIndex = GetFirstVisibleRow() + max(0, (y - rc.top + (m_sizeFrame.cy/2)) / m_sizeFrame.cy);
					// 不能超过当前Instance的特效范围
					nOldIndex = max(nFirstIndex, min(nLastIndex , nOldIndex));

					if (m_sbVert.GetScrollPos() > 0 && y == rc.top && GetFirstVisibleRow()>nFirstIndex)
						EnableAutoScroll(top, TRUE);
					else if (y == rc.bottom && GetLastWholeVisibleRow() < nLastIndex -1)
					{
						TRACE2("EnableAutoScroll bottom true: %d %d\r\n", GetLastVisibleRow(), nLastIndex);
						EnableAutoScroll(bottom, TRUE);
					}
					else
						EnableAutoScroll(none, FALSE);

					// 绘制新的矩形区域
					TRACE1("Draw Move Effect: MOUSEMOVE_AFTER %d\r\n", nOldIndex);
					DrawMoveEffectDragRect(pDC, gPair, nOldIndex, FALSE);
					m_bScrolled = FALSE;

					// 设置光标
					BOOL bCanMoveEffect = CanMoveEffect(gPair, nOldIndex);
					if(bCanMoveEffect != bOldCanMoveEffect)
					{
						SetCursor(AfxGetApp()->LoadCursor(bCanMoveEffect ? IDC_DRAG_SINGLE : IDC_NO_DROP));
					}
				}
				else
				{
					// 判断鼠标的移动是否超过范围
					CPoint pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
					if(max(pt.y, point.y) - min(pt.y, point.y)>GetSystemMetrics(SM_CYDRAG))
					{
						bDraging = TRUE;
						SetCursor(AfxGetApp()->LoadCursor(CanMoveEffect(gPair, nOldIndex) ? IDC_DRAG_SINGLE : IDC_NO_DROP));
						DrawMoveEffectDragRect(pDC, gPair, nOldIndex, FALSE);
					}
				}
				continue;
			}

		case WM_LBUTTONUP:
			{
				SetCursor(NULL);
				ReleaseCapture();

				// 绘制旧的矩形区域
				if(bDraging)
				{
					DrawMoveEffectDragRect(pDC, gPair, nOldIndex, TRUE);

					// 保存修改的信息
					if(CanMoveEffect(gPair, nOldIndex))
					{
						GEFFECT_LIST::iterator iter3 = pInstance->m_effectList.begin();
						for(int i = nFirstIndex; i<nOldIndex; ++i)
						{
							++iter3;
						}
						my_app.Commands().Do(new CCmdMoveEffect(pInstance, pEffect, *iter3));
					}
				}
				goto _exit;
			}

			// 过滤右键及滚轮事件
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
			continue;

			// 系统消息退出
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			ReleaseCapture();
			break;

		default:
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		// 若失去捕获,则退出
		if (GetCapture() != this)
			break;
	}

	// 绘制旧的矩形区域
	if(bDraging)
	{
		DrawMoveEffectDragRect(pDC, gPair, nOldIndex, TRUE);
	}

_exit:
	EnableAutoScroll(none, FALSE);
	pDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
}

BOOL CEffectWnd::CanMoveEffect(GEFFECT_PAIR& gPair, int nIndex)
{
	return nIndex != gPair.nIndex && nIndex != gPair.nIndex + 1;
}

//拖动左侧特效树中特效的位置
void CEffectWnd::DrawMoveEffectDragRect(CDC* pDC, GEFFECT_PAIR& gPair, int nIndex, BOOL bRestore)
{
	// 不可移动时不画
	if(CanMoveEffect(gPair, nIndex))
	{
		CRect rc;
		GetEffectBarRowRect(rc, nIndex);

		rc.left += 3;
		rc.right-= 3;


		int nROP2 = pDC->SetROP2(R2_NOTXORPEN);

		CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		CPen *pPenOld = pDC->SelectObject(&pen);

		int y = rc.top;

		// line
		pDC->MoveTo(rc.left, y);
		pDC->LineTo(rc.right, y);
		pDC->MoveTo(rc.left, y + 1);
		pDC->LineTo(rc.right, y + 1);

		// left end
		pDC->MoveTo(rc.left - 1, y - 1);
		pDC->LineTo(rc.left - 1, y + 3);

		pDC->MoveTo(rc.left - 2, y - 2);
		pDC->LineTo(rc.left - 2, y + 4);

		// right end
		pDC->MoveTo(rc.right, y - 1);
		pDC->LineTo(rc.right, y + 3);

		pDC->MoveTo(rc.right + 1, y - 2);
		pDC->LineTo(rc.right + 1, y + 4);

		pDC->SelectObject(pPenOld);

		pDC->SetROP2(nROP2);
	}
}

void CEffectWnd::TrackSplitter(const CPoint& point)
{
	SetCapture();

	CRect rc;
	GetClientRect(&rc);

	CDC *pDC = GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
	CBrush *pOldBrush = pDC->SelectObject(&m_brThickBorder);

	// 绘制Splitter拖动
	int nPosOld = m_nSplitterPos;
	DrawSplitterDragRect(pDC, nPosOld, FALSE);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			{
				int x = (int)(short)LOWORD(msg.lParam);
				x = max(rc.left + SPLITTER_SIZE / 2 + 64, min(rc.right - ::GetSystemMetrics(SM_CXVSCROLL) - SPLITTER_SIZE / 2, x));

				DrawSplitterDragRect(pDC, nPosOld, TRUE);
				nPosOld = x;
				DrawSplitterDragRect(pDC, nPosOld, FALSE);

				continue;
			}

		case WM_LBUTTONUP:
			{
				// erase last drawing before invalidate the whole client rect
				ReleaseCapture();

				DrawSplitterDragRect(pDC, nPosOld, TRUE);
				m_nSplitterPos = nPosOld;
				Invalidate(FALSE);

				RecalcScrollPos();
				RecalcLayout();

				goto _exit;
			}

		// 过滤右键及滚轮事件
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
			continue;

		// 系统消息退出
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			ReleaseCapture();
			break;

		default:
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		// 若失去捕获,则退出
		if (GetCapture() != this)
			break;
	}

	DrawSplitterDragRect(pDC, nPosOld, TRUE);

_exit:
	pDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
}

int CEffectWnd::GetTrackTypeBase(int nIndex, int nStartTime, int nLength, const CPoint& point)
{
	CRect rc;
	GetTimeLineRowRect(rc, nIndex);

	rc.left += (nStartTime - m_sbHorz.GetScrollPos()) * m_sizeFrame.cx;
	rc.right = rc.left + nLength * m_sizeFrame.cx;

	int w = m_sizeFrame.cx;
	if(nLength < 3)
	{
		w /= 3;
	}
	CRect rcl(rc), rcm(rc), rcr(rc);

	rcm.left += w;
	rcm.right -= w;

	rcl.right = rcm.left;

	rcr.left = rcm.right;

	if(rcm.PtInRect(point))
	{
		return TRACK_MOVE;
	}

	if(rcl.PtInRect(point))
	{
		return TRACK_RESIZE_LEFT;
	}

	if(rcr.PtInRect(point))
	{
		return TRACK_RESIZE_RIGHT;
	}

	return TRACK_VOID;
}

int CEffectWnd::GetTrackType(GEFFECT_PAIR& gPair, const CPoint& point)
{
	gldEffect* pEffect = EffectFromPair(gPair);
	int nTrackType = GetTrackTypeBase(gPair.nIndex, pEffect->m_startTime, pEffect->m_length, point);
	return nTrackType;
}

int CEffectWnd::GetTrackType(GINSTANCE_PAIR& gPair, const CPoint& point)
{
	gldInstance* pInstance = InstanceFromPair(gPair);
	if(pInstance->m_effectList.size())
	{
		SIZE sz = GetAllEffectsRange(pInstance);
		int nTrackType = GetTrackTypeBase(gPair.nIndex, sz.cx, sz.cy - sz.cx, point);
		if(nTrackType==TRACK_RESIZE_RIGHT || nTrackType==TRACK_RESIZE_LEFT)
		{
			nTrackType = TRACK_MOVE;
		}
		return nTrackType;
	}
	return TRACK_VOID;
}

int CEffectWnd::GetTrackType(GSCAPT_PAIR& gPair, const CPoint& point)
{
	gldScene2* pScene = _GetCurScene2();
	if(pScene && pScene->m_soundList.size())
	{
		SIZE sz = GetAllSoundsRange(pScene);
		int nTrackType = GetTrackTypeBase(gPair.nIndex, sz.cx, sz.cy - sz.cx, point);
		if(nTrackType==TRACK_RESIZE_RIGHT || nTrackType==TRACK_RESIZE_LEFT)
		{
			nTrackType = TRACK_MOVE;
		}
		return nTrackType;
	}
	return TRACK_VOID;
}

int CEffectWnd::GetTrackType(GSOUND_PAIR& gPair, const CPoint& point)
{
	gldSceneSound* pSound = SoundFromPair(gPair);
	int nTrackType = GetTrackTypeBase(gPair.nIndex, pSound->m_startTime, pSound->m_length, point);
	if(nTrackType==TRACK_RESIZE_RIGHT || nTrackType==TRACK_RESIZE_LEFT)
	{
		nTrackType = TRACK_MOVE;
	}
	return nTrackType;
}

BOOL CEffectWnd::TrackTimeLine(int nIndex, int& nStartTime, int& nLength, int nFrame, const CPoint& point, UINT nTrackType)
{
	m_bTrackingTimeLine = TRUE;

	BOOL bRet = FALSE;

	UpdateWindow();

	SetCapture();

	CRect rc;
	GetTimeLineRect(rc);

	CRect rcTitle;
	GetTimeLineTitleRect(rcTitle);

	CDC *pDC = GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
	CBrush *pOldBrush = pDC->SelectObject(&m_brThickBorder);


	int nOldStartTime = nStartTime;
	int nOldLength = nLength;
	
	int nNewStartTime;
	int nNewLength;

	int nDelta = 0;

	if (nTrackType == TRACK_MOVE)
		::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND_HOLD));

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			{
				//if(!m_bScrolled)
				//{
				//	DrawTimeLineDragRect(pDC, nIndex, nOldStartTime, nOldLength, TRUE);
				//}

				// 计算新的帧
				int x = GET_X_LPARAM(msg.lParam);
				if (x < rc.left)
					m_bAutoScrollSpeed = (rc.left - x) / 10;
				else if(x > rc.right)
					m_bAutoScrollSpeed = (x - rc.right) / 10;
				else
					m_bAutoScrollSpeed = 1;
				m_bAutoScrollSpeed = max(1, min(5, m_bAutoScrollSpeed));
				TRACE("AutoScrollSpeed: %d\n", m_bAutoScrollSpeed);

				x = max(rc.left, min(rc.right, x));
				int nNewFrame = max(0, FrameFromPos(x));

				nDelta = nNewFrame - nFrame;

				switch(nTrackType)
				{
				case TRACK_RESIZE_LEFT:
					{
						// 固定结束时间,调整起始时间
						int nEndTime = nOldLength + nOldStartTime;
						// 限制其范围在0 到 nEndTime -1
						nNewStartTime = max(0, min(nEndTime -1, nStartTime + nDelta));
						nNewLength = nEndTime - nNewStartTime;
					}
					break;
				case TRACK_RESIZE_RIGHT:
					{
						// 调整长度
						nNewStartTime = nOldStartTime;
						nNewLength = max(1, nLength + nDelta);
					}
					break;
				case TRACK_MOVE:
					{
						// 调整起始时间
						nNewStartTime = max(0, nStartTime + nDelta);
						nNewLength = nOldLength;
					}
					break;
				default:
					ASSERT(0);
					break;
				}
				int nMin, nMax;
				m_sbHorz.GetScrollRange(&nMin, &nMax);
				int nMaxNew = nNewStartTime + nNewLength -1;

				TRACE("%d, %d\n", x, rc.right);
				if (m_sbHorz.GetScrollPos() > 0 && x == rc.left)
					EnableAutoScroll(left, TRUE);
				else if (x == rc.right)
					EnableAutoScroll(right, TRUE);
				else
				{
					// 调整滚动范围
					EnableAutoScroll(none, FALSE);
					int nMax, nMin;
					m_sbHorz.GetScrollRange(&nMin, &nMax);
					int nMaxNew = max(nNewStartTime + nNewLength, m_sbHorz.GetScrollPos() + GetVisibleFrames());
					if(nMaxNew > nMax + 1)
					{
						RecalcScrollRange(nMaxNew);
					}
				}

				if (nOldStartTime != nNewStartTime || nOldLength != nNewLength)
				{
					switch (m_nSelType)
					{
						case SEL_EFFECT:
						{
							m_pSelEffect->m_startTimeTracking = nNewStartTime;
							m_pSelEffect->m_lengthTracking = nNewLength;
							break;
						}

						case SEL_INST:
						{
							for(GEFFECT_LIST::iterator iter = m_pSelInstance->m_effectList.begin(); iter != m_pSelInstance->m_effectList.end(); ++iter)
							{
								gldEffect* pEffect = *iter;
								pEffect->m_startTimeTracking += (nNewStartTime - nOldStartTime);
								//pEffect->m_lengthTracking = pEffect->m_length + nNewLength - (sz.cy - sz.cx);
							}
							break;
						}

						case SEL_SOUND:
						{
							m_pSelSound->m_startTimeTracking = nNewStartTime;
							break;
						}

						case SEL_SCAPT:
						{
							gldScene2 *pScene = _GetCurScene2();
							//SIZE sz = GetAllSoundsRange(pScene);
							for(GSCENESOUND_LIST::iterator iter = pScene->m_soundList.begin(); iter != pScene->m_soundList.end(); ++iter)
							{
								gldSceneSound* pSound = *iter;
								pSound->m_startTimeTracking += (nNewStartTime - nOldStartTime);
							}
							break;
						}
					}

					InvalidateRect(&rcTitle, FALSE);
					InvalidateRect(&rc, FALSE);

					//DrawTimeLineDragRect(pDC, nIndex, nOldStartTime, nOldLength, FALSE);
					m_bScrolled = FALSE;

					nOldStartTime = nNewStartTime;
					nOldLength = nNewLength;
				}

				continue;
			}

		case WM_LBUTTONUP:
			{
				ReleaseCapture();

				//DrawTimeLineDragRect(pDC, nIndex, nOldStartTime, nOldLength, TRUE);

				// 保存修改的信息
				if (nStartTime != nOldStartTime || nLength != nOldLength)
				{
					nStartTime = nOldStartTime;
					nLength = nOldLength;
					bRet = TRUE;
				}

				goto _exit;
			}

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
			continue;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			ReleaseCapture();
			break;

		default:
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (GetCapture() != this)
			break;
	}

	//DrawTimeLineDragRect(pDC, nIndex, nOldStartTime, nOldLength, TRUE);
_exit:
	EnableAutoScroll(none, FALSE);
	pDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);

	m_bTrackingTimeLine = FALSE;

	if (!bRet)
		InvalidateRect(&rc, FALSE);

	return bRet;
}


void CEffectWnd::DrawSplitterDragRect(CDC *pDC, int nPos, BOOL bRestore)
{
	CRect rc;
	GetClientRect(&rc);
	rc.left = nPos - SPLITTER_SIZE / 2;
	rc.right = nPos + SPLITTER_SIZE / 2;
	pDC->PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), DSTINVERT);
}
//
void CEffectWnd::DrawTimeLineDragRect(CDC* pDC, int nIndex, int nStartTime, int nLength, BOOL bRestore)
{
	CRect rc;
	GetTimeLineRect(rc);
	rc.top -= m_nTitleHeight;

	CRect rcEffect;
	GetTimeLineRowRect(rcEffect, nIndex);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rc);
	pDC->SelectClipRgn(&rgn);

	rcEffect.left += (nStartTime - m_sbHorz.GetScrollPos()) * m_sizeFrame.cx;
	rcEffect.right = rcEffect.left + nLength * m_sizeFrame.cx;

	int nROP2 = pDC->SetROP2(R2_NOTXORPEN);

	DrawVertDotLine(pDC, rcEffect.left, rc.top, rc.bottom);
	DrawVertDotLine(pDC, rcEffect.right, rc.top, rc.bottom);

	pDC->SetROP2(nROP2);

	const int BORDER_WIDTH = 3;

	rcEffect.top++;
	rcEffect.right++;

	pDC->PatBlt(rcEffect.left, rcEffect.top, BORDER_WIDTH, rcEffect.Height(), DSTINVERT);
	pDC->PatBlt(rcEffect.left + BORDER_WIDTH, rcEffect.top, rcEffect.Width() - BORDER_WIDTH * 2, BORDER_WIDTH, DSTINVERT);
	pDC->PatBlt(rcEffect.right - BORDER_WIDTH, rcEffect.top, BORDER_WIDTH, rcEffect.Height(), DSTINVERT);
	pDC->PatBlt(rcEffect.left + BORDER_WIDTH, rcEffect.bottom - BORDER_WIDTH, rcEffect.Width() - BORDER_WIDTH * 2, BORDER_WIDTH, DSTINVERT);

	pDC->SelectClipRgn(NULL);
}

void CEffectWnd::RecalcLayout()
{
	CRect rc;
	GetClientRect(&rc);

	if (rc.Width() == 0 || rc.Height() == 0)
		return;

	// 调整Splitter的位置
	m_nSplitterPos = max(rc.left + SPLITTER_SIZE / 2 + 64, min(rc.right - ::GetSystemMetrics(SM_CXVSCROLL) - SPLITTER_SIZE / 2, m_nSplitterPos));

	CDeferWindowPos dp;

	dp.BeginDeferWindowPos();


	// 调整滚动条的位置
	GetHScrollBarPaneRect(rc);
	dp.DeferWindowPos(m_sbHorz.m_hWnd, NULL, rc, SWP_NOZORDER);


	GetVScrollBarPaneRect(rc);
	dp.DeferWindowPos(m_sbVert.m_hWnd, NULL, rc, SWP_NOZORDER);

	dp.EndDeferWindowPos();
}

void CEffectWnd::EnableAutoScroll(AUTOSCROLLDIR dir, BOOL bAutoScrollUpdateWindow)
{
	if (m_nAutoScrollDir == none && dir != none)
		SetTimer(1, dir == top || dir == bottom ? 200 : 1, NULL);
	else if (m_nAutoScrollDir != none && dir == none)
		KillTimer(1);

	m_nAutoScrollDir = dir;
	m_bAutoScrollUpdateWindow = bAutoScrollUpdateWindow;
}


void CEffectWnd::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	int nPos;
	switch (m_nAutoScrollDir)
	{
	case left:
		nPos = m_sbHorz.GetScrollPos();
		if (nPos > 0)
		{
			m_sbHorz.SetScrollPos(max(0, nPos - m_bAutoScrollSpeed), TRUE);
			//Invalidate(FALSE);
			//if (m_bAutoScrollUpdateWindow)
			//	UpdateWindow();

			m_bScrolled = TRUE;

			POINT point;
			GetCursorPos(&point);
			ScreenToClient(&point);
			PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));

			Invalidate(FALSE);
		}
		break;

	case right:
		nPos = m_sbHorz.GetScrollPos();
		RecalcScrollRange(nPos + GetVisibleFrames() /* page size */ + m_bAutoScrollSpeed);
		m_sbHorz.SetScrollPos(nPos + m_bAutoScrollSpeed, TRUE);

		//Invalidate(FALSE);
		//if (m_bAutoScrollUpdateWindow)
		//	UpdateWindow();

		m_bScrolled = TRUE;

		POINT point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));

		break;

	case top:
		nPos = m_sbVert.GetScrollPos();
		if (nPos > 0)
		{
			m_sbVert.SetScrollPos(nPos - 1, TRUE);
			Invalidate(FALSE);
			if (m_bAutoScrollUpdateWindow)
				UpdateWindow();

			m_bScrolled = TRUE;

			POINT point;
			GetCursorPos(&point);
			ScreenToClient(&point);
			PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
		}
		break;

	case bottom:
		nPos = m_sbVert.GetScrollPos();
		m_sbVert.SetScrollPos(nPos + 1, TRUE);
		if (m_sbVert.GetScrollPos() != nPos)
		{
			Invalidate(FALSE);
			if (m_bAutoScrollUpdateWindow)
				UpdateWindow();

			m_bScrolled = TRUE;

			POINT point;
			GetCursorPos(&point);
			ScreenToClient(&point);
			PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
		}
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

IMPLEMENT_OBSERVER(CEffectWnd, ChangeCurrentScene2)
{
	// 切换到新的场景时触发
	RecalcScrollPos();
	SetVoidSelection(TRUE);
}

IMPLEMENT_OBSERVER(CEffectWnd, ChangeMainMovie2)
{
	// 初始化主Movie时触发
	RecalcScrollPos();
	UpdateSelection();
}

IMPLEMENT_OBSERVER(CEffectWnd, ModifyCurrentScene2)
{
	// 当前场景的内容发生任何改变时触发, 这个还要做优化(如添加,删除,变序时)
	// Chen Hao: 好像只需要处理ChangeZorder
	RecalcScrollPos();
	UpdateSelection();
}

IMPLEMENT_OBSERVER(CEffectWnd, ChangeEffect)
{
	// 添加一个特效
	RecalcScrollPos();
}

IMPLEMENT_OBSERVER(CEffectWnd, Select)
{
	UpdateSelection();
}

IMPLEMENT_OBSERVER(CEffectWnd, ChangeSceneSound)
{
	RecalcScrollPos();
}

IMPLEMENT_OBSERVER(CEffectWnd, RenameSymbol)
{
	gldObj *pObj = (gldObj *)pData;
	// Redraw time line if it appears in the timeline
	if (pObj->GetGObjType() == gobjSound)
		Invalidate(FALSE);
}

IMPLEMENT_OBSERVER(CEffectWnd, RenameInstance)
{
	gldInstance *pInstance = (gldInstance *)pData;
	// Redraw time line if it appears in the timeline
	//if (pInstance->m_effectList.size() > 0)
		Invalidate(FALSE);
}

void CEffectWnd::UpdateSelection()
{
	if(m_bSelBlocked==FALSE)
	{
		gld_shape_sel& sel = my_app.CurSel();
		gld_shape_sel::iterator it = sel.begin();

		gldInstance* pInstance = NULL;
		gldEffect* pEffect = NULL;
		if(sel.count()==1)
		{
			//对象只有一个时保持当前的Instance不变就可以了
			pInstance = CTraitInstance(*it);
			if((GetSelType()==SEL_INST || GetSelType()==SEL_EFFECT) && GetSelInstance()==pInstance)
			{
			}
			else
			{
				SetInstSelection(pInstance, FALSE);
			}
		}
		else if(sel.count() >1)
		{
			// 若多选时选空集
			SetVoidSelection(FALSE);
		}
		else
		{
			// 一定要取消Inst和Effect的选择
			if(GetSelType()==SEL_INST || GetSelType()==SEL_EFFECT)
			{
				SetVoidSelection(TRUE);
			}
		}
	}
}

void CEffectWnd::EnsureSelectionVisible()
{
	switch(m_nSelType)
	{
	case SEL_VOID:
		break;
	case SEL_EFFECT:
		{
			GEFFECT_PAIR gPair = PairFromEffect(m_pSelInstance, m_pSelEffect);
			if(!IsEndPair(gPair))
			{
				int nIndex = IndexFromPair(gPair);
				EnsureRowVisible(nIndex);
				EnsureFrameVisible(m_pSelEffect->m_startTime, m_pSelEffect->m_startTime + m_pSelEffect->m_length);
			}
		}
		break;
	case SEL_INST:
		{
			GINSTANCE_PAIR gPair = PairFromInst(m_pSelInstance);
			if(!IsEndPair(gPair))
			{
				int nIndex = IndexFromPair(gPair);
				EnsureRowVisible(nIndex);
				CSize sz = GetAllEffectsRange(m_pSelInstance);
				EnsureFrameVisible(sz.cx, sz.cy);
			}
		}
		break;
	case SEL_SOUND:
		{
			GSOUND_PAIR gPair = PairFromSound(m_pSelSound);
			if(!IsEndPair(gPair))
			{
				int nIndex = IndexFromPair(gPair);
				EnsureRowVisible(nIndex);
				EnsureFrameVisible(m_pSelSound->m_startTime, m_pSelSound->m_startTime + m_pSelSound->m_length);
			}
		}
		break;
	case SEL_SCAPT:
		{
			GSCAPT_PAIR gPair = PairFromScapt();
			if(!IsEndPair(gPair))
			{
				int nIndex = IndexFromPair(gPair);
				EnsureRowVisible(nIndex);
				CSize sz = GetAllSoundsRange(_GetCurScene2());
				EnsureFrameVisible(sz.cx, sz.cy);
			}
		}
		break;
	default:
		break;
	}
}

void CEffectWnd::EnsureRowVisible(int nIndex)
{
	if(nIndex >= 0)
	{
		int nFirstVisibleRow = GetFirstVisibleRow();
		int nWholeVisibleRows = GetVisibleRows();
		if(IsLastVisibleRowPartInvisible())
		{
			--nWholeVisibleRows;
		}
		if(nIndex < nFirstVisibleRow)
		{
			m_sbVert.SetScrollPos(nIndex);
			Invalidate();
		}
		else if(nIndex >= nFirstVisibleRow + nWholeVisibleRows)
		{
			m_sbVert.SetScrollPos(nIndex + 1 - nWholeVisibleRows);
			Invalidate();
		}
	}
}

void CEffectWnd::EnsureFrameVisible(int nStartFrame, int nEndFrame)
{
	int nFirstVisibleFrame = GetFirstVisibleFrame();
	int nWholeVisibleFrames = GetVisibleFrames();
	if(IsLastVisibleFramePartInvisible())
	{
		--nWholeVisibleFrames;
	}

	if(nEndFrame - 1 < nFirstVisibleFrame || nStartFrame > nFirstVisibleFrame + nWholeVisibleFrames - 1)
	{
		if(nStartFrame < nFirstVisibleFrame && nEndFrame <= nFirstVisibleFrame + nWholeVisibleFrames)
		{
			m_sbHorz.SetScrollPos(nStartFrame);
			Invalidate();
		}
		else if(nStartFrame > nFirstVisibleFrame && nEndFrame > nFirstVisibleFrame + nWholeVisibleFrames)
		{
			int nOffset = nEndFrame - (nFirstVisibleFrame + nWholeVisibleFrames);
			int nPos = m_sbHorz.GetScrollPos() + nOffset;
			m_sbHorz.SetScrollPos(nPos);
			Invalidate();
		}
	}
}

BOOL CEffectWnd::PreTranslateMessage(MSG* pMsg)
{
	if(::IsWindow(m_tooltip.m_hWnd))
	{
		if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
			m_tooltip.RelayEvent(pMsg);
	}

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		if (m_hAccel && ::TranslateAccelerator(AfxGetMainWnd()->m_hWnd, m_hAccel, pMsg))
			return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

void CEffectWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseMove(nFlags, point);

	if (COptions::Instance()->m_bShowTimelineToolTip)
	{
		if (_GetCurScene2() == NULL)
		{
			return;
		}

		tagHOVER hover;

		GEFFECT_PAIR gPair = EffectPairFromPos(point.y);
		if(!IsEndPair(gPair))
		{
			hover.type	= SEL_EFFECT;
			hover.ptr	= EffectFromPair(gPair);
		}
		else
		{
			GSOUND_PAIR gPair = SoundPairFromPos(point.y);
			if(!IsEndPair(gPair))
			{
				hover.type	= SEL_SOUND;
				hover.ptr	= SoundFromPair(gPair);
			}
		}

		if(m_hover.type != hover.type || m_hover.ptr != hover.ptr)
		{
			if(hover.type==SEL_VOID)
			{
				if(m_hover.type != SEL_VOID)
				{
					m_tooltip.Pop();
					m_tooltip.DelTool(this, 1);
				}
			}
			else if(hover.type==SEL_EFFECT)
			{
				m_tooltip.Pop();
				m_tooltip.DelTool(this, 1);

				gldEffect* pEffect = (gldEffect*)hover.ptr;

				CString strText;
				pEffect->GetToolTipText(strText);

				AFX_OLDTOOLINFO ti;
				memset(&ti, 0, sizeof(ti));
				ti.cbSize = sizeof(ti);
				ti.hinst = 0;
				ti.hwnd = m_hWnd;
				ti.uFlags = 0 /*TTF_CENTERTIP*/;
				ti.lpszText = (LPSTR)(LPCTSTR)strText;
				ti.uId = 1;

				CRect rc;
				GetTimeLineRect(rc);
				ti.rect = rc;

				m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
				m_tooltip.Activate(TRUE);

				TRACE("CEffectWnd: Show ToolTip\n");
			}
			else if(hover.type == SEL_SOUND)
			{
				m_tooltip.Pop();
				m_tooltip.DelTool(this, 1);

				gldSceneSound* pSound = (gldSceneSound*)hover.ptr;

				CString strText;
				pSound->GetToolTipText(strText);

				AFX_OLDTOOLINFO ti;
				memset(&ti, 0, sizeof(ti));
				ti.cbSize = sizeof(ti);
				ti.hinst = 0;
				ti.hwnd = m_hWnd;
				ti.uFlags = 0 /*TTF_CENTERTIP*/;
				ti.lpszText = (LPSTR)(LPCTSTR)strText;
				ti.uId = 1;

				CRect rc;
				GetTimeLineRect(rc);
				ti.rect = rc;

				m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
				m_tooltip.Activate(TRUE);


				TRACE("CEffectWnd: Show ToolTip\n");
			}

			m_hover.type	= hover.type;
			m_hover.ptr		= hover.ptr;
		}
	}
}

void CEffectWnd::ExpandEffects(gldInstance* pInstance)
{
	if(!pInstance->m_expanded)
	{
		pInstance->m_expanded = TRUE;
		RecalcScrollPos();
	}
}

void CEffectWnd::ExpandSounds()
{
	gldScene2* pScene = _GetCurScene2();
	if(pScene && !pScene->m_soundExpanded)
	{
		pScene->m_soundExpanded = TRUE;
		RecalcScrollPos();
	}
}

void CEffectWnd::CollapseEffects(gldInstance* pInstance)
{
	if(pInstance->m_expanded)
	{
		pInstance->m_expanded = FALSE;
		RecalcScrollPos();
	}
}

void CEffectWnd::CollapseSounds()
{
	gldScene2* pScene = _GetCurScene2();
	if(pScene && pScene->m_soundExpanded)
	{
		pScene->m_soundExpanded = FALSE;
		RecalcScrollPos();
	}
}

BOOL CEffectWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetFirstVisibleRow() > 0 || GetVisibleRows() <= GetRowsCount())	// the last layer may be partial invsisble
	{
		SCROLLINFO si;
		m_sbVert.GetScrollInfo(&si, SIF_ALL);
		int ds = zDelta / WHEEL_DELTA * VSB_LINE_SIZE;
		int nPosNew = max(si.nMin, min(si.nMax, si.nPos - ds));
		if (nPosNew != si.nPos)
		{
			si.fMask = SIF_POS;
			si.nPos = nPosNew;
			m_sbVert.SetScrollInfo(&si, TRUE);

			Invalidate(FALSE);

			return TRUE;
		}
	}
	else
	{
		SCROLLINFO si;
		m_sbHorz.GetScrollInfo(&si, SIF_ALL);
		int ds = zDelta / WHEEL_DELTA * HSB_LINE_SIZE;
		int nPosNew = max(si.nMin, min(si.nMax, si.nPos - ds));
		if (nPosNew != si.nPos)
		{
			si.fMask = SIF_POS;
			si.nPos = nPosNew;
			m_sbHorz.SetScrollInfo(&si, TRUE);

			Invalidate(FALSE);

			return TRUE;
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CEffectWnd::SetInstSelection(gldInstance* pInstance, BOOL bNotify)
{
	m_pSelEffect = NULL;
	m_pSelSound = NULL;
	if(m_nSelType != SEL_INST || m_pSelInstance != pInstance)
	{
		m_nSelType = SEL_INST;
		m_pSelInstance = pInstance;
		EnsureSelectionVisible();

		++m_bSelBlocked;
		CSubjectManager::Instance()->GetSubject("ChangeTimelineSel")->Notify(NULL);
		--m_bSelBlocked;

		Invalidate();
	}
}

void CEffectWnd::SetEffectSelection(gldInstance* pInstance, gldEffect* pEffect, BOOL bNotify)
{
	m_pSelSound = NULL;
	if(m_nSelType != SEL_EFFECT || m_pSelInstance != pInstance || m_pSelEffect != pEffect)
	{
		m_nSelType = SEL_EFFECT;
		m_pSelInstance = pInstance;
		m_pSelEffect = pEffect;
		EnsureSelectionVisible();

		++m_bSelBlocked;
		CSubjectManager::Instance()->GetSubject("ChangeTimelineSel")->Notify(NULL);
		--m_bSelBlocked;

		Invalidate();
	}
}

void CEffectWnd::SetScaptSelection(BOOL bNotify)
{
	m_pSelInstance = NULL;
	m_pSelEffect = NULL;
	m_pSelSound = NULL;
	if(m_nSelType != SEL_SCAPT)
	{
		m_nSelType = SEL_SCAPT;
		EnsureSelectionVisible();

		++m_bSelBlocked;
		CSubjectManager::Instance()->GetSubject("ChangeTimelineSel")->Notify(NULL);
		--m_bSelBlocked;

		Invalidate();
	}
}
void CEffectWnd::SetSoundSelection(gldSceneSound* pSound, BOOL bNotify)
{
	m_pSelInstance = NULL;
	m_pSelEffect = NULL;
	if(m_nSelType != SEL_SOUND || m_pSelSound != pSound)
	{
		m_nSelType = SEL_SOUND;
		m_pSelSound = pSound;
		EnsureSelectionVisible();

		++m_bSelBlocked;
		CSubjectManager::Instance()->GetSubject("ChangeTimelineSel")->Notify(NULL);
		--m_bSelBlocked;

	//	Invalidate();
	}
}

void CEffectWnd::SetVoidSelection(BOOL bNodify)
{
	m_pSelInstance = NULL;
	m_pSelEffect = NULL;
	m_pSelSound = NULL;
	if(m_nSelType != SEL_VOID)
	{
		m_nSelType = SEL_VOID;
		EnsureSelectionVisible();

		++m_bSelBlocked;
		CSubjectManager::Instance()->GetSubject("ChangeTimelineSel")->Notify(NULL);
		--m_bSelBlocked;

		Invalidate();
	}
}

void CEffectWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);

	SetFocus();
	SetActive(TRUE);

	// 如果鼠标在Splitter区间,则处理鼠标事件改变Splitter的位置
	CRect rc;
	GetSplitterRect(rc);
	if (rc.PtInRect(point))
	{
		return;
	}

	gldScene2 *pScene = _GetCurScene2();
	if (pScene == NULL)
	{
		return;
	}

	ProcessMouseEvent(WM_LBUTTONDBLCLK, nFlags, point);
}

void CEffectWnd::OnDestroy()
{
	CWnd::OnDestroy();

	AfxGetApp()->WriteProfileInt("Timeline", "Splitter Pos", m_nSplitterPos);
}


//这个超长的函数实现如何在分割条两侧进行一系列的操作
void CEffectWnd::ProcessMouseEvent(UINT nMessage, UINT nFlags, CPoint point)
{
	CRect rc1;
	GetEffectBarRect(rc1);
	CRect rc2;
	GetTimeLineRect(rc2);

	gldScene2* pScene = _GetCurScene2();

	if(rc1.PtInRect(point) || rc2.PtInRect(point))
	{
		//////////////////////////////////////////////////////////////////////////
		//处理特效
		//////////////////////////////////////////////////////////////////////////
		GEFFECT_PAIR gEffect = EffectPairFromPos(point.y);
		if(!IsEndPair(gEffect))
		{
			gldInstance* pInstance = InstanceFromPair(gEffect);
			gldEffect* pEffect = EffectFromPair(gEffect);

			SetEffectSelection(pInstance, pEffect, TRUE);
			int nFrame = FrameFromPos(point.x);

			switch(nMessage)
			{
			case WM_LBUTTONDOWN:
				if(rc1.PtInRect(point))
				{
					TrackMoveEffect(gEffect, point);
				}
				else
				{
					UINT nTrackType = GetTrackType(gEffect, point);
					if(nTrackType != TRACK_VOID)
					{
						gldInstance* pInstance = InstanceFromPair(gEffect);

						for(GEFFECT_LIST::iterator iter = m_pSelInstance->m_effectList.begin(); iter != m_pSelInstance->m_effectList.end(); ++iter)
						{
							gldEffect* pEffect = *iter;
							pEffect->m_startTimeTracking = pEffect->m_startTime;
							pEffect->m_lengthTracking = pEffect->m_length;
						}

						gldEffect* pEffect = EffectFromPair(gEffect);
						int nStartTime = pEffect->m_startTime;
						int nLength = pEffect->m_length;
						if(TrackTimeLine(gEffect.nIndex, nStartTime, nLength, nFrame, point, nTrackType))
						{
							TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_MODIFY_EFFECT_TIME);
							pCmdGroup->Do(new CCmdModifyEffectTime(pInstance, pEffect, pEffect->m_startTimeTracking, pEffect->m_lengthTracking));
							my_app.Commands().Do(pCmdGroup);
						}
					}
				}
				break;
			case WM_LBUTTONDBLCLK:
				OnTimeLineConfig();
				break;
			case WM_CONTEXTMENU:
				{
					CMenu menu;
					menu.LoadMenu(IDR_POPUP_TIME_LINE_EFFECT);
					CMenu *pPopup = menu.GetSubMenu(0);
					CPoint pt(point);
					ClientToScreen(&pt);
					pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
				}
				break;
			default:
				break;
			}
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		//处理Instance
		//////////////////////////////////////////////////////////////////////////
		GINSTANCE_PAIR gInst = InstPairFromPos(point.y);
		if(!IsEndPair(gInst))
		{
			gldInstance* pInstance = InstanceFromPair(gInst);

			CRect rc;
			GetExpandIconRect(rc, gInst.nIndex);

			//对象上附加有对象，并且为鼠标点击消息消息，并被鼠标点中
			if(pInstance->m_effectList.size() && rc.PtInRect(point) && (nMessage==WM_LBUTTONDOWN || nMessage==WM_LBUTTONDBLCLK))
			{
				//对象是展开状态
				if(pInstance->m_expanded)
				{
					CollapseEffects(pInstance);
				}
				else
				{
					ExpandEffects(pInstance);
				}
			}
			else
			{
				SetInstSelection(pInstance, TRUE);
				int nFrame = FrameFromPos(point.x);

				switch(nMessage)
				{
				case WM_LBUTTONDOWN:
					if(rc2.PtInRect(point))
					{
						UINT nTrackType = GetTrackType(gInst, point);
						if(nTrackType!=TRACK_VOID)
						{
							gldInstance* pInstance = InstanceFromPair(gInst);

							for(GEFFECT_LIST::iterator iter = pInstance->m_effectList.begin(); iter != pInstance->m_effectList.end(); ++iter)
							{
								gldEffect* pEffect = *iter;
								pEffect->m_startTimeTracking = pEffect->m_startTime;
								pEffect->m_lengthTracking = pEffect->m_length;
							}

							SIZE sz = GetAllEffectsRange(pInstance);
							int nStartTime = sz.cx;
							int nLength = sz.cy - sz.cx;
							if(TrackTimeLine(gInst.nIndex, nStartTime, nLength, nFrame, point, nTrackType))
							{
								TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_MODIFY_EFFECT_TIME);
								for(GEFFECT_LIST::iterator iter = pInstance->m_effectList.begin(); iter != pInstance->m_effectList.end(); ++iter)
								{
									gldEffect* pEffect = *iter;
									pCmdGroup->Do(new CCmdModifyEffectTime(pInstance, pEffect, pEffect->m_startTimeTracking, pEffect->m_lengthTracking));
								}
								my_app.Commands().Do(pCmdGroup);
							}
						}
					}
					break;
				case WM_LBUTTONDBLCLK:
					OnTimeLineConfig();
					break;
				case WM_CONTEXTMENU:
					{
						CMenu menu;
						menu.LoadMenu(IDR_POPUP_TIME_LINE_EFFECT);
						CMenu *pPopup = menu.GetSubMenu(0);
						CPoint pt(point);
						ClientToScreen(&pt);
						pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
					}
					break;
				default:
					break;
				}
			}
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		//处理声音
		//////////////////////////////////////////////////////////////////////////
		GSOUND_PAIR gSound = SoundPairFromPos(point.y);
		if(!IsEndPair(gSound))
		{
			gldSceneSound* pSound = SoundFromPair(gSound);

			SetSoundSelection(pSound, TRUE);
			int nFrame = FrameFromPos(point.x);

			switch(nMessage)
			{
			case WM_LBUTTONDOWN:
				if(rc2.PtInRect(point))
				{
					UINT nTrackType = GetTrackType(gSound, point);
					if(nTrackType!=TRACK_VOID)
					{
						gldSceneSound* pSound = SoundFromPair(gSound);

						for(GSCENESOUND_LIST::iterator iter = pScene->m_soundList.begin(); iter != pScene->m_soundList.end(); ++iter)
						{
							gldSceneSound* pSound = *iter;
							pSound->m_startTimeTracking = pSound->m_startTime;
							pSound->m_lengthTracking = pSound->m_length;
						}

						int nStartTime = pSound->m_startTime;
						int nLength = pSound->m_length;
						if(TrackTimeLine(gSound.nIndex, nStartTime, nLength, nFrame, point, nTrackType))
						{
							TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_MODIFY_SCENE_SOUND_TIME);
							pCmdGroup->Do(new CCmdModifySceneSoundTime(pSound, pSound->m_startTimeTracking, pSound->m_lengthTracking));
							my_app.Commands().Do(pCmdGroup);
						}
					}
				}
				break;
			case WM_LBUTTONDBLCLK:
				OnTimeLineConfig();
				break;
			case WM_CONTEXTMENU:
				{
					CMenu menu;
					menu.LoadMenu(IDR_POPUP_TIME_LINE_SOUND);
					CMenu *pPopup = menu.GetSubMenu(0);
					CPoint pt(point);
					ClientToScreen(&pt);
					pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
				}
				break;
			default:
				break;
			}
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		//处理Scapt
		//////////////////////////////////////////////////////////////////////////
		GSCAPT_PAIR gScapt = ScaptPairFromPos(point.y);
		if(!IsEndPair(gScapt))
		{
			CRect rc;
			GetExpandIconRect(rc, gScapt.nIndex);
			if(rc.PtInRect(point) && (nMessage==WM_LBUTTONDOWN || nMessage==WM_LBUTTONDBLCLK))
			{
				if(pScene->m_soundExpanded)
				{
					CollapseSounds();
				}
				else
				{
					ExpandSounds();
				}
			}
			else
			{
				SetScaptSelection(TRUE);
				int nFrame = FrameFromPos(point.x);

				switch(nMessage)
				{
				case WM_LBUTTONDOWN:
					if(rc2.PtInRect(point))
					{
						UINT nTrackType = GetTrackType(gScapt, point);
						if(nTrackType!=TRACK_VOID)
						{
							for(GSCENESOUND_LIST::iterator iter = pScene->m_soundList.begin(); iter != pScene->m_soundList.end(); ++iter)
							{
								gldSceneSound* pSound = *iter;
								pSound->m_startTimeTracking = pSound->m_startTime;
								pSound->m_lengthTracking = pSound->m_length;
							}

							SIZE sz = GetAllSoundsRange(pScene);
							int nStartTime = sz.cx;
							int nLength = sz.cy - sz.cx;
							if(TrackTimeLine(gScapt.nIndex, nStartTime, nLength, nFrame, point, nTrackType))
							{
								TCommandGroup* pCmdGroup = new TCommandGroup(IDS_CMD_MODIFY_SCENE_SOUND_TIME);
								for(GSCENESOUND_LIST::iterator iter = pScene->m_soundList.begin(); iter != pScene->m_soundList.end(); ++iter)
								{
									gldSceneSound* pSound = *iter;
									pCmdGroup->Do(new CCmdModifySceneSoundTime(pSound, pSound->m_startTimeTracking, pSound->m_lengthTracking));
								}
								my_app.Commands().Do(pCmdGroup);
							}
						}
					}
					break;
				case WM_LBUTTONDBLCLK:
					break;
				case WM_CONTEXTMENU:
					{
						CMenu menu;
						menu.LoadMenu(IDR_POPUP_TIME_LINE_SOUND);
						CMenu *pPopup = menu.GetSubMenu(0);
						CPoint pt(point);
						ClientToScreen(&pt);
						pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
					}
					break;
				default:
					break;
				}
			}
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		//处理剩余
		//////////////////////////////////////////////////////////////////////////
		//SetVoidSelection(TRUE);
		if(nMessage==WM_CONTEXTMENU)
		{
			if(GetSelType()!=SEL_VOID)
			{

				CMenu menu;
				if(GetSelType()==SEL_EFFECT || GetSelType()==SEL_INST)
				{
					menu.LoadMenu(IDR_POPUP_TIME_LINE_EFFECT);
				}
				else
				{
					menu.LoadMenu(IDR_POPUP_TIME_LINE_SOUND);
				}
				CMenu *pPopup = menu.GetSubMenu(0);
				CPoint pt(point);
				ClientToScreen(&pt);
				pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
			}
		}
	}
}

void CEffectWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	SetFocus();
	SetActive(TRUE);

	// 如果鼠标在Splitter区间,则处理鼠标事件改变Splitter的位置
	CPoint pt(point);
	ScreenToClient(&pt);

	CRect rc;
	GetSplitterRect(rc);
	if (rc.PtInRect(pt))
	{
		return;
	}

	gldScene2 *pScene = _GetCurScene2();
	if (pScene == NULL)
	{
		return;
	}


	CRect rc2;
	GetClientRect(&rc2);
	if(rc2.PtInRect(pt))
	{
		ProcessMouseEvent(WM_CONTEXTMENU, NULL, pt);
	}
}

void CEffectWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	UpdateMenuCmdUI(this, pPopupMenu, nIndex, bSysMenu);
}

void CEffectWnd::OnTimeLinePlaySound()
{
	if(GetSelType()==SEL_SOUND)
	{
		gldSound* pSound = GetSelSound()->m_sound->m_soundObj;
		CPlaySoundDialog dlg(pSound);
		dlg.DoModal();
	}
}

void CEffectWnd::OnUpdateTimeLinePlaySound(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelType()==SEL_SOUND);
}

void CEffectWnd::OnTimeLineCut()
{
	if (Clipboard.IsDataAvailable(CGlandaClipboard::ForCut))
		Clipboard.Cut();
}

void CEffectWnd::OnUpdateTimeLineCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForCut));
}

void CEffectWnd::OnTimeLineCopy()
{
	if(Clipboard.IsDataAvailable(CGlandaClipboard::ForCopy))
	{
		Clipboard.Copy();
	}
}

void CEffectWnd::OnUpdateTimeLineCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForCopy));
}

void CEffectWnd::OnTimeLinePaste()
{
	if(Clipboard.IsDataAvailable(CGlandaClipboard::ForPaste))
	{
		Clipboard.Paste();
	}
}

void CEffectWnd::OnUpdateTimeLinePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForPaste));
}

void CEffectWnd::OnTimeLineDelete()
{
	if(GetSelType()==SEL_EFFECT)
	{
		gldInstance* pInstance = GetSelInstance();
		gldEffect* pEffect = GetSelEffect();
		my_app.Commands().Do(new CCmdRemoveEffect(pInstance, pEffect));
	}
	else if(GetSelType()==SEL_SOUND)
	{
		gldSceneSound* pSceneSound = GetSelSound();
		my_app.Commands().Do(new CCmdRemoveSound(pSceneSound));
	}
	else if(GetSelType()==SEL_INST)
	{
		if(GetSelInstance()->m_effectList.size())
		{
			my_app.Commands().Do(new CCmdRemoveAllEffects(GetSelInstance()));
		}
	}
	else if(GetSelType()==SEL_SCAPT)
	{
		if(_GetCurScene2() && _GetCurScene2()->m_soundList.size())
		{
			my_app.Commands().Do(new CCmdRemoveAllSounds());
		}
	}
}

void CEffectWnd::OnUpdateTimeLineDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelType()==SEL_SOUND
		|| GetSelType()==SEL_EFFECT
		|| GetSelType()==SEL_INST && GetSelInstance()->m_effectList.size()
		|| GetSelType()==SEL_SCAPT && _GetCurScene2() && _GetCurScene2()->m_soundList.size());
}

void CEffectWnd::OnTimeLineMoveUp()
{
	if(GetSelType()==SEL_EFFECT)
	{
		gldInstance* pInstance = GetSelInstance();
		gldEffect* pEffect = GetSelEffect();

		GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
		ASSERT(iter != pInstance->m_effectList.end());

		if(iter != pInstance->m_effectList.begin())
		{
			my_app.Commands().Do(new CCmdMoveEffect(pInstance, pEffect, *--iter));
		}
	}
}

void CEffectWnd::OnUpdateTimeLineMoveUp(CCmdUI* pCmdUI)
{
	if(GetSelType()==SEL_EFFECT)
	{
		gldInstance* pInstance = GetSelInstance();
		gldEffect* pEffect = GetSelEffect();

		GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
		ASSERT(iter != pInstance->m_effectList.end());

		if(iter != pInstance->m_effectList.begin())
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}

void CEffectWnd::OnTimeLineMoveDown()
{
	if(GetSelType()==SEL_EFFECT)
	{
		gldInstance* pInstance = GetSelInstance();
		gldEffect* pEffect = GetSelEffect();

		GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
		ASSERT(iter != pInstance->m_effectList.end());

		++iter;
		if(iter != pInstance->m_effectList.end())
		{
			++iter;
			if(iter == pInstance->m_effectList.end())
			{
				my_app.Commands().Do(new CCmdMoveEffect(pInstance, pEffect, NULL));
			}
			else
			{
				my_app.Commands().Do(new CCmdMoveEffect(pInstance, pEffect, *iter));
			}
		}
	}
}

void CEffectWnd::OnUpdateTimeLineMoveDown(CCmdUI* pCmdUI)
{
	if(GetSelType()==SEL_EFFECT)
	{
		gldInstance* pInstance = GetSelInstance();
		gldEffect* pEffect = GetSelEffect();

		GEFFECT_LIST::iterator iter = std::find(pInstance->m_effectList.begin(), pInstance->m_effectList.end(), pEffect);
		ASSERT(iter != pInstance->m_effectList.end());

		++iter;
		if(iter != pInstance->m_effectList.end())
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}

void CEffectWnd::OnTimeLineConfig()
{
	if (GetSelType()==SEL_EFFECT
		|| GetSelType()==SEL_INST && GetSelInstance()->m_effectList.size())
	{
		gldInstance* pInstance = GetSelInstance();
		gldEffect* pEffect = NULL;

		if (GetSelType()==SEL_EFFECT)
			pEffect = GetSelEffect();

		pInstance->ConfigEffect(pInstance, pEffect);
	}
	else if(GetSelType()==SEL_SOUND)
	{
		gldSceneSound* pSound = GetSelSound();

		CResourceThumbItem thumbItem(pSound->m_sound->m_soundObj);
		thumbItem.PrepareSoundInformation();
		CResourceThumbObjPropertiesDialog page(&thumbItem);

		CPropertySheet sheet("Properties");
		sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
		sheet.AddPage(&page);
		sheet.DoModal();
	}
}

void CEffectWnd::OnUpdateTimeLineConfig(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetSelType() == SEL_SOUND
		|| GetSelType() == SEL_EFFECT
		|| GetSelType() == SEL_INST && GetSelInstance()->m_effectList.size());
}



/* CCmdReplaceEffect */

class CCmdReplaceEffect : public TCommand
{
public:
	CCmdReplaceEffect(gldEffect *pEffect, REFIID clsid, IGLD_Parameters *pParas);
	virtual ~CCmdReplaceEffect();
	virtual bool Execute();
	virtual bool Unexecute();

private:
	gldEffect *m_pEffect;
	CLSID m_clsid;
	CComPtr<IGLD_Parameters> m_pIParas;
	CComPtr<IStream> m_Stream;
};

CCmdReplaceEffect::CCmdReplaceEffect(gldEffect *pEffect, REFIID clsid, IGLD_Parameters *pParas)
: m_pEffect(pEffect)
, m_clsid(clsid)
, m_pIParas(pParas)
{
	_M_Desc_ID = IDS_CMD_REPLACEEFFECT;
	VERIFY(SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &m_Stream)));
	VERIFY(SUCCEEDED(m_pEffect->WriteToStream(m_Stream, NULL)));
}

CCmdReplaceEffect::~CCmdReplaceEffect()
{
}

bool CCmdReplaceEffect::Execute()
{
	m_pEffect->ChangeTo(m_clsid, m_pIParas);

	CEffectWnd::Instance()->Invalidate(FALSE);

	return TCommand::Execute();
}

bool CCmdReplaceEffect::Unexecute()
{
	LARGE_INTEGER i0 = {0};	
	VERIFY(SUCCEEDED(m_Stream->Seek(i0, STREAM_SEEK_SET, NULL)));
	VERIFY(SUCCEEDED(m_pEffect->ReadFromStream(m_Stream, NULL)));

	CEffectWnd::Instance()->Invalidate(FALSE);

	return TCommand::Unexecute();
}

void CEffectWnd::OnTimeLineReplace()
{
	if (GetSelType() == SEL_EFFECT && GetSelEffect() != NULL)
	{
		gldEffect *effect = GetSelEffect();
		gldInstance *inst = GetSelInstance();
		if (inst != NULL)
		{
			CDlgSelectEffect dlg(effect->m_pEffectInfo, effect->m_pIParas);
			if (dlg.DoModal() == IDOK)
			{
				CComPtr<IGLD_Parameters> pIParas;
				dlg.GetEffectOptions(&pIParas);
				gldEffectInfo *pInfo = dlg.GetSelEffectInfo();
				if (pInfo != NULL && pIParas != NULL)
				{
					TCommandGroup *cmd = new TCommandGroup(IDS_CMD_REPLACEEFFECT);
					CComPtr<IGLD_Parameters> common;
					VERIFY(SUCCEEDED(common.CoCreateInstance(__uuidof(GLD_Parameters))));
					CopyParameters(pIParas, PT_COMMON, common, NULL, 0);
					cmd->Do(new CCmdChangeEffectParameters(inst->m_pIParas, common));
					CComPtr<IGLD_Parameters> custom;
					VERIFY(SUCCEEDED(custom.CoCreateInstance(__uuidof(GLD_Parameters))));
					CopyParameters(pIParas, PT_CUSTOM, custom, NULL, 0);
					cmd->Do(new CCmdReplaceEffect(effect, pInfo->m_clsid, custom));
					my_app.Commands().Do(cmd);
				}
			}
		}
	}
}

void CEffectWnd::OnUpdateTimeLineReplace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelType() == SEL_EFFECT && GetSelEffect() != NULL);
}

