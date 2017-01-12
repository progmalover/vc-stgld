#pragma once

#include "resizabledialog.h"
#include "FlatBitmapButton.h"
#include "NewMenu.h"

#include <vector>

class gldObj;
class gldMoviClip;

// CDlgEffectPanel dialog

class CDlgEffectPanel : public CResizableDialog
{
public:
	CDlgEffectPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEffectPanel();

// Dialog Data
	enum { IDD = IDD_EFFECT_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedBtnAddEffect();
	afx_msg void OnBnClickedBtnAddSound();
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnBnClickedBtnEffectMoveUp();
	afx_msg void OnBnClickedBtnEffectMoveDown();
	afx_msg void OnBnClickedBtnEffectSettings();

	afx_msg void OnUpdateBtnAddEffect(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnAddSound(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnRemove(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnEffectMoveUp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnEffectMoveDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnEffectSettings(CCmdUI *pCmdUI);

protected:
	virtual void OnOK();
	virtual void OnCancel();

	CFlatBitmapButton m_btnAddEffect;
	CFlatBitmapButton m_btnAddSound;
	CFlatBitmapButton m_btnDel;
	CFlatBitmapButton m_btnConfig;
	CFlatBitmapButton m_btnUp;
	CFlatBitmapButton m_btnDown;

	CNewMenu m_menuEffect;
	CNewMenu m_menuEffectEnter;
	CNewMenu m_menuEffectEmphasize;
	CNewMenu m_menuEffectExit;
	CNewMenu m_menuMotionPath;

	CImageList m_imgList;

public:
	CRect m_rcInitial;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnAddEffect(UINT nID);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSelectEffect();
};
