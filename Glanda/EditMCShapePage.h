#pragma once

#include "StaticShapePreview.h"
#include "Observer.h"


// CEditMCShapePage dialog

class CEditMCShapePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditMCShapePage)

public:
	CEditMCShapePage();
	virtual ~CEditMCShapePage();

	DECLARE_GLANDA_OBSERVER(CEditMCShapePage, ModifyCurrentScene2)

// Dialog Data
	enum { IDD = IDD_EDIT_MC_SHAPE_PAGE };

protected:
	void HandleSelChange();
	void HandleShapeModified();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CStaticShapePreview m_stcShapePreview;

	DECLARE_MESSAGE_MAP()
public:
	TCommandGroup* m_pCmd;
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeShapeList();
	afx_msg void OnBnClickedBtnEditShape();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnReplaceShape();
};
