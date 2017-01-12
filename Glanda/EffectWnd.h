#pragma once


#include "Singleton.h"
#include "gldInstance.h"
#include "gldEffect.h"
#include "Observer.h"
#include "gldSceneSound.h"
#include "GPAIR.h"
// CEffectWnd

using namespace GPAIR;

class CEffectWnd;


class CEffectWnd : public CWnd, public CSingleton<CEffectWnd>
{
	DECLARE_DYNAMIC(CEffectWnd)
protected:
	DECLARE_GLANDA_OBSERVER(CEffectWnd, ChangeCurrentScene2)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, ChangeMainMovie2)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, ModifyCurrentScene2)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, ChangeEffect)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, Select)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, ChangeSceneSound)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, RenameSymbol)
	DECLARE_GLANDA_OBSERVER(CEffectWnd, RenameInstance)

public:

	CEffectWnd();
	virtual ~CEffectWnd();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	CScrollBar m_sbHorz;
	CScrollBar m_sbVert;
	HACCEL m_hAccel;
public:
	// ������ʱ�����
	int GetMaxTime();
	int GetMaxTimeWithSound();
	SIZE GetAllEffectsRange(gldInstance* pInstance);	// SIZE.cx = left, SIZE.cy = right
	SIZE GetAllSoundsRange(gldScene2* pScene);
	void RecalcScrollRange(int nTotalFrames);
	void RecalcScrollPos(void);
	void SetShowAllInstances(BOOL bShowAll)
	{
		GPAIR::ALWAYS_LIST_INST = bShowAll;
		if(::IsWindow(m_hWnd))
		{
			RecalcScrollPos();
		}
	}
public:
	// λ�ü���
	template <class T>
	T PairFromPos(int y, T (*pfnBegin)(), T (*pfnNext)(T&))
	{
		for(T gPair = pfnBegin(); !IsEndPair(gPair); gPair = pfnNext(gPair))
		{
			CRect rc;
			GetTimeLineRowRect(rc, gPair.nIndex);
			if(y >= rc.top && y < rc.bottom)
			{
				return gPair;
			}
		}
		return GetEndPair<T>();
	}

	GEFFECT_PAIR EffectPairFromPos(int y)
	{
		return PairFromPos(y, GetBeginEffectPair, GetNextEffectPair);
	}
	GSOUND_PAIR SoundPairFromPos(int y)
	{
		return PairFromPos(y, GetBeginSoundPair, GetNextSoundPair);
	}
	GINSTANCE_PAIR InstPairFromPos(int y)
	{
		return PairFromPos(y, GetBeginInstPair, GetNextInstPair);
	}
	GSCAPT_PAIR ScaptPairFromPos(int y)
	{
		return PairFromPos(y, GetBeginScaptPair, GetNextScaptPair);
	}
	int FrameFromPos(int x);

public:
	// �����ж������
	int GetRowsCount();									// ���������
	int GetFirstVisibleRow();							// ��õ�һ���ɼ���
	int GetLastVisibleRow();							// ������һ���ɼ���
	int GetLastWholeVisibleRow();						// ������һ�������ɼ���
	int GetVisibleRows();								// ��ÿɼ��е�����
	BOOL IsLastVisibleRowPartInvisible();				// �ж����һ���Ƿ񲿷ֲ��ɼ�
	BOOL IsRowVisible(int index);						// �����Ƿ��ڿɼ�����

	int GetFrameCount();								// ���֡������
	int GetFirstVisibleFrame();							// ��õ�һ���ɼ�֡
	int GetLastVisibleFrame();							// ������һ���ɼ�֡
	int GetVisibleFrames();								// ��ÿɼ�֡������
	BOOL IsLastVisibleFramePartInvisible();
	BOOL IsFrameVisible(int nFrame);

	// ��������
	void GetTimeLineRect(CRect &rc);					// ���ʱ���ߵ�����
	void GetTimeLineTitleRect(CRect &rc);				// ���ʱ���߱��������
	void GetTimeLineRowRect(CRect& rc, int index);		// ���ʱ������ָ���еľ�������			

	void GetEffectBarRect(CRect &rc);					// �����Ч����������
	void GetExpandIconRect(CRect& rc, int nRow);		// ���ͼ�������
	void GetEffectBarTitleRect(CRect &rc);				// �����Ч�������������
	void GetEffectBarRowRect(CRect& rc, int nIndex);

	void GetEffectBarPaneRect(CRect &rc);
	void GetSplitterRect(CRect &rc);					// ��÷ָ��ߵ�����
	void GetHScrollBarPaneRect(CRect &rc);				// ���ˮƽ������������
	void GetVScrollBarPaneRect(CRect &rc);				// ��ô�ֱ������������
	void GetGridSizeButtonRect(CRect &rc);

	// ��Ⱦ
	void PaintTimeLine(CDC& dc);							// ����EffectLine
	void PaintTimeLineTitle(CDC& dc);						// ����EffectLine����
	void PaintEffectLineRow(CDC& dc, GEFFECT_PAIR& gPair);	// ��Ⱦÿ����Ч��ʱ����
	void PaintInstLineRow(CDC& dc, GINSTANCE_PAIR& gPair);
	void PaintSoundLineRow(CDC& dc, GSOUND_PAIR& gPair);
	void PaintScaptLineRow(CDC& dc, GSCAPT_PAIR& gPair);
	void PaintIndicatorFrame(CDC& dc);
	void PaintEffectBar(CDC& dc);							// ������Ч����
	void PaintEffectBarTitle(CDC& dc);						// ������Ч��������
	void PaintWidgets(CDC& dc);
	void PaintSplitter(CDC& dc);							// ���Ʒָ���
	void PaintEffectBarRow(CDC& dc, GEFFECT_PAIR& gPair);	// ��Ⱦÿ����Ч������
	void PaintInstBarRow(CDC& dc, GINSTANCE_PAIR& gPair);
	void PaintScaptBarRow(CDC& dc, GSCAPT_PAIR& gPair);
	void PaintSoundBarRow(CDC& dc, GSOUND_PAIR& gPair);
	void PaintTimeLineRowBase(CDC& dc, int nRow, BOOL bSel, int nStartFrame, int nEndFrame, CBitmap& bmpTiles, CBitmap& bmpLine, LPCTSTR lpszText, int nTopMargin, int nBottomMargin, COLORREF clrTextSel = RGB(0, 0, 0), COLORREF clrText = RGB(0, 0, 0));
protected:

	// ��ͼ����
	void CreateDrawTools();							// ��ʼ����ͼ����
	void ReleaseDrawTools();								// �ͷŻ�ͼ����
	HBRUSH CreatePatternBrush(HINSTANCE hInst, UINT nID);	// ����Invertˢ��
	void DrawHorzDotLine(CDC *pDC, int y, int x1, int x2);
	void DrawVertDotLine(CDC *pDC, int x, int y1, int y2);
protected:
	int		m_nSplitterPos;				// �ָ��ߵ�λ��
	CSize	m_sizeFrame;				// ÿ֡�����ƾ��εĴ�С
	int		m_nTitleHeight;				// ����ĸ߶�
	int		m_nTotalFrames;				// ֡������
	COLORREF	m_clrEffectLineText;	// ��Чʱ�����ϵ�������ɫ
	COLORREF	m_clrEffectLineTextSel;	
	COLORREF	m_clrSoundLineText;		// ����ʱ�����ϵ�������ɫ
	COLORREF	m_clrSoundLineTextSel;

	BOOL m_bTrackingTimeLine;
public:
	// �¼�
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void ProcessMouseEvent(UINT nMessage, UINT nFlags, CPoint point);
private:

	// GDI Object
	CFont m_fntFrameNum;

	CPen m_penDarkGray;

	CBrush m_brWhite;
	CBrush m_brLightGray2;
	CBrush m_brDarkGray;

	CBrush m_brThickBorder;

	CImageList m_lstExpanded;
	CImageList m_lstTreeLine;
	CImageList m_lstInstanceIcon;

	CBitmap m_bmpTiles;
	CBitmap m_bmpSoundTotal;
	CBitmap m_bmpEffectTotal;
	CBitmap m_bmpSoundLine;
	CBitmap m_bmpEffectLine;

	CBitmap m_bmpSoundIcon;
	CImageList m_lstSoundList;
	CBitmap m_bmpEffectIcon;
public:
	// TRACK����
	enum
	{
		TRACK_VOID = 0,
		TRACK_MOVE = 1,
		TRACK_RESIZE_LEFT = 2,
		TRACK_RESIZE_RIGHT = 3
	};
	int GetTrackTypeBase(int nIndex, int nStartTime, int nLength, const CPoint& point);
	int GetTrackType(GEFFECT_PAIR& gPair, const CPoint& point);
	int GetTrackType(GINSTANCE_PAIR& gPair, const CPoint& point);
	int GetTrackType(GSOUND_PAIR& gPair, const CPoint& point);
	int GetTrackType(GSCAPT_PAIR& gPair, const CPoint& point);
	void TrackMoveEffect(GEFFECT_PAIR& gPair, const CPoint& point);
	BOOL TrackTimeLine(int nIndex, int& nStartTime, int& nLength, int nFrame, const CPoint& point, UINT nTrackType);
	void TrackSplitter(const CPoint& point);	// ����Splitter�¼�
	BOOL CanMoveEffect(GEFFECT_PAIR& gPair, int index);
	void RecalcLayout();				// �������²���
	void DrawMoveEffectDragRect(CDC* pDC, GEFFECT_PAIR& gPair, int nIndex, BOOL bRestore);
	void DrawSplitterDragRect(CDC *pDC, int nPos, BOOL bRestore);	// ����Splitter
	void DrawTimeLineDragRect(CDC* pDC, int nIndex, int nStartTime, int nLength, BOOL bRestore);

	enum AUTOSCROLLDIR
	{
		none, 
		left, 
		top, 
		right,
		bottom, 
	}	m_nAutoScrollDir;
	BOOL m_bAutoScrollUpdateWindow;
	BOOL m_bAutoScrollSpeed;
	BOOL m_bScrolled;
	void EnableAutoScroll(AUTOSCROLLDIR dir, BOOL bAutoScrollUpdateWindow);


public:
	// ѡ�����
	enum SEL_TYPE
	{
		SEL_VOID	= 0,
		SEL_SCAPT	= 1,
		SEL_SOUND	= 2,
		SEL_INST	= 3,
		SEL_EFFECT	= 4,
		SEL_MULINST	= 5	// Reversed
	};

	BOOL IsInstSelected(gldInstance* pInstance)
	{
		return m_nSelType == SEL_INST && m_pSelInstance == pInstance;
	}
	BOOL IsEffectSelected(gldInstance* pInstance, gldEffect* pEffect)
	{
		return m_nSelType == SEL_EFFECT && m_pSelInstance == pInstance && m_pSelEffect == pEffect;
	}
	BOOL IsSoundSelected(gldSceneSound* pSound)
	{
		return m_nSelType == SEL_SOUND && m_pSelSound == pSound;
	}
	BOOL IsScaptSelected()
	{
		return m_nSelType == SEL_SCAPT;
	}
	void SetInstSelection(gldInstance* pInstance, BOOL bNotify);
	void SetEffectSelection(gldInstance* pInstance, gldEffect* pEffect, BOOL bNotify);
	void SetScaptSelection(BOOL bNotify);
	void SetSoundSelection(gldSceneSound* pSound, BOOL bNotify);
	void SetVoidSelection(BOOL bNodify);

	SEL_TYPE GetSelType()
	{
		return m_nSelType;
	}
	gldInstance* GetSelInstance()
	{
		return m_pSelInstance;
	}
	gldEffect* GetSelEffect()
	{
		return m_pSelEffect;
	}
	gldSceneSound* GetSelSound()
	{
		return m_pSelSound;
	}
	BOOL SelBlockIncrement()
	{
		return m_bSelBlocked++;
	}
	BOOL SelBlockDecrement()
	{
		return --m_bSelBlocked;
	}
	void SetActive(BOOL bActive);		// ����Ϊ�
protected:
	void UpdateSelection();
	void EnsureSelectionVisible();
	void EnsureRowVisible(int nIndex);
	void EnsureFrameVisible(int nStartFrame, int nEndFrame);
	BOOL m_bActive;
	SEL_TYPE m_nSelType;
	BOOL m_bSelBlocked;
	gldInstance* m_pSelInstance;
	gldEffect* m_pSelEffect;
	gldSceneSound* m_pSelSound;

public:
	void ExpandEffects(gldInstance* pInstance);
	void ExpandSounds();
	void CollapseEffects(gldInstance* pInstance);
	void CollapseSounds();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

public:
	CToolTipCtrl m_tooltip;
	struct tagHOVER
	{
		SEL_TYPE	type;
		void*		ptr;
		tagHOVER()	{	type = SEL_VOID; ptr = NULL;	}
	}m_hover;
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

	afx_msg void OnTimeLinePlaySound();
	afx_msg void OnUpdateTimeLinePlaySound(CCmdUI* pCmdUI);
	afx_msg void OnTimeLineCopy();
	afx_msg void OnUpdateTimeLineCopy(CCmdUI* pCmdUI);
	afx_msg void OnTimeLineCut();
	afx_msg void OnUpdateTimeLineCut(CCmdUI* pCmdUI);
	afx_msg void OnTimeLinePaste();
	afx_msg void OnUpdateTimeLinePaste(CCmdUI* pCmdUI);
	afx_msg void OnTimeLineDelete();
	afx_msg void OnUpdateTimeLineDelete(CCmdUI* pCmdUI);

	afx_msg void OnTimeLineMoveUp();
	afx_msg void OnUpdateTimeLineMoveUp(CCmdUI* pCmdUI);
	afx_msg void OnTimeLineMoveDown();
	afx_msg void OnUpdateTimeLineMoveDown(CCmdUI* pCmdUI);

	afx_msg void OnTimeLineConfig();
	afx_msg void OnUpdateTimeLineConfig(CCmdUI* pCmdUI);
	afx_msg void OnTimeLineSoundInfo();

	afx_msg void OnTimeLineReplace();
	afx_msg void OnUpdateTimeLineReplace(CCmdUI* pCmdUI);
};