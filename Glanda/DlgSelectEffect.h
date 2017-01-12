#pragma once

#include "ShockwaveFlash.h"
#include "ResizableDialog.h"
#include "FlatColorButton.h"
#include "FlatShapeButton.h"
#include "MoviePreview.h"

class gldEffectInfo;

class CDlgSelectEffect : public CResizableDialog
{
	//DECLARE_DYNCREATE(CDlgSelectEffect)

public:
	CDlgSelectEffect(gldEffectInfo *pInfo = NULL, IGLD_Parameters *pIParas = NULL, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectEffect();
// Overrides
	
// Dialog Data
	enum { IDD = IDD_SELECT_EFFECT };
	
	gldEffectInfo *GetSelEffectInfo() const { return m_pSelEffectInfo; }
	HRESULT GetEffectOptions(IGLD_Parameters **options) { return m_Options.CopyTo(options); }
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	CTreeCtrl m_treEffects;
	CShockwaveFlash m_Player;
	UINT_PTR m_PreviewTimer;
	BOOL m_Init;
	CComPtr<IGLD_Parameters> m_Options;	
	CMoviePreview m_Preview;	
	HTREEITEM m_hLastItem;
	gldEffectInfo *m_pSelEffectInfo;

	CFlatColorButton m_btnBackground;
	CFlatColorButton m_btnTextColor;
	CFlatShapeButton m_btnTextStyle[6];

	CString m_strPreviewFile;
	
	void LoadTextStyleButtons();
	void Refresh(BOOL bForce);

public:
	static LPCTSTR DEFINED_PARAS[];
	static LPCTSTR REG_KEY;

public:
	afx_msg void OnTvnSelchangingTreeEffects(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();

	afx_msg void OnBackgroundColorChanged();
	afx_msg void OnTextColorChanged();
	afx_msg void OnBnClickedTextStyle(UINT nID);

	void Preview(gldEffectInfo *pEffect);
	BOOL FillEffectTree();
	BOOL IsPreviewSync();
	void CreatePreviewMovie();
	void CreatePreviewMovieFromText();
	void CreatePreviewMovieFromSel();

	void Reposition();
	CRect GetVertSplitBarRect();
	void TrackVertSplit(CPoint point);
	void VertSplitTo(int x);
	
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedCheckPreviewSync();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOption();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
