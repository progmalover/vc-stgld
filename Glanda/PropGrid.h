#pragma once

#include "numberedit.h"
#include "flatcolorbutton.h"

// CPropGrid dialog

class CPropGrid : public CDialog
{
	DECLARE_DYNAMIC(CPropGrid)

public:
	CPropGrid(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropGrid();

// Dialog Data
	enum { IDD = IDD_EDIT_GRID };

protected:
	CNumberEdit m_edtXDist;
	CNumberEdit m_edtYDist;	
	CFlatColorButton m_btnColor;

	void UpdateControls();
	void HandleChange();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnEditModifyXDist();
	afx_msg void OnEditModifyYDist();
	afx_msg void OnColorChanged();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedChkshowgrid();
	afx_msg void OnBnClickedChksnaptogrid();
};
