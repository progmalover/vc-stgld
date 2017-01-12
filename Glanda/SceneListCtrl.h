#pragma once

#include "FlexListCtrl.h"
#include "Observer.h"
#include "SceneNameEdit.h"

class gldScene2;

// CSceneListCtrl

class CSceneListCtrl : public CFlexListCtrl
{
	struct _ItemData
	{
		gldScene2* m_pScene;
		CBitmap* m_pBitmap;
		_ItemData(gldScene2* pScene)
			: m_pScene(pScene)
			, m_pBitmap(NULL)
		{
		}
		~_ItemData()
		{
			ClearBuffer();
		}
		void ClearBuffer()
		{
			if(m_pBitmap)
			{
				delete m_pBitmap;
				m_pBitmap = NULL;
			}
		}
	};
public:
	CSceneListCtrl();
	virtual ~CSceneListCtrl();

	DECLARE_SINGLETON(CSceneListCtrl)
protected:
	DECLARE_MESSAGE_MAP()

protected:
	DECLARE_GLANDA_OBSERVER(CSceneListCtrl, ChangeMainMovie2)
	DECLARE_GLANDA_OBSERVER(CSceneListCtrl, ChangeMovieProperties)
	DECLARE_GLANDA_OBSERVER(CSceneListCtrl, ChangeCurrentScene2)
	DECLARE_GLANDA_OBSERVER(CSceneListCtrl, ModifyCurrentScene2)

public:
	void RecalcItemSize();
	int InsertScene(int index, gldScene2 *pScene);
	int FindScenePos(gldScene2* pScene);
	gldScene2* GetScene(int nIndex);
	gldScene2* GetSelectedScene(void);

	int GetLabelHeight();
	void GetLabelRect(int index, RECT *pLabelRect, BOOL bExact);


	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	static void RenderThumb(CDC& dc, CRect& rc, gldScene2* pScene);
protected:
	// 自动生成场景的名称
	void AutoGenerateSceneName(CString& strName);
	void AutoGenerateSceneName(std::string& strName);
	virtual void OnDeleteItem(int index);

	int m_nLabelHeight;

	// 绘制场景的缩略图
	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);	
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	// 将索引为nOldIndex的场景移动到索引nNewIndex
	bool MoveScene(int nNewIndex, int nOldIndex);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

protected:
	enum AUTOSCROLLDIR
	{
		none, 
		top, 
		bottom, 
	}m_nAutoScrollDir;

	CSceneNameEdit m_edit;
	int m_nLButtonDownOnSelected;
	int m_nEditName;
	int m_nDelayEditName;
	BOOL m_bInEndEditName;

	void BeginEditName(int index);
	void EndEditName(BOOL bSubmit);

	void BeginDelayEditName(int index);
	void CancelDelayEditName();

	BOOL GetNearestItem(int y, int &index, BOOL &bBefore);
	void DrawInsertionMarker(CDC *pDC, int index, BOOL bBefore);
	BOOL TrackMoveScene(const CPoint point, int &nIndex, BOOL &bBefore, BOOL &bCopy);
	void EnableAutoScroll(AUTOSCROLLDIR dir);
	BOOL m_bScrolled;

	HACCEL m_hAccel;


	virtual BOOL GetToolTipText(int index, CString &strText);

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSceneAdd();
	afx_msg void OnSceneInsert();
	afx_msg void OnSceneRename();
	afx_msg void OnNameEditKillFocus();
	afx_msg void OnNameEditChange();
	afx_msg void OnNameEditKeyEnter();
	afx_msg void OnNameEditKeyEscape();
	afx_msg void OnSceneDupliate();
	afx_msg void OnUpdateSceneDupliate(CCmdUI *pCmdUI);
};
