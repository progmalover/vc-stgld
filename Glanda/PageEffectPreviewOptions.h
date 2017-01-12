#pragma once
#include "GLD_PropertyPage.h"


// CPageEffectPreviewOptions : Property page dialog

class CPageEffectPreviewOptions : public CGLD_PropertyPage
{
	DECLARE_DYNCREATE(CPageEffectPreviewOptions)
	DECLARE_OLECREATE_EX(CPageEffectPreviewOptions)

// Constructors
public:
	CPageEffectPreviewOptions();

// Dialog Data
	enum { IDD = IDD_PAGE_EFFECT_PREVIEW_OPTIONS };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	virtual BOOL OnApply();

	BOOL m_bHasSel;
	BOOL m_bUseSel;

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedCheckUseSelected();
};
