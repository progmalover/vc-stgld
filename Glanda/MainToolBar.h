#pragma once

#include "ToolBarEx.h"
#include "ZoomComboBox.h"

// CMainToolBar

class CMainToolBar : public CToolBarEx
{
public:
	CMainToolBar();
	virtual ~CMainToolBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropDown(NMHDR *pNMHDR, LRESULT *pResult);
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	BOOL CreateZoomComboBox();

protected:
	CZoomComboBox m_cmbZoom;
	virtual void RecalcLayout();
	void RecalcZoomComboBoxPos();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
