#pragma once

#include "ResizablePage.h"

class TCommandGroup;

class gldScene2;

// CScenePropertiesPage dialog

class CScenePropertiesPage : public CResizablePage
{
public:
	CScenePropertiesPage();
	virtual ~CScenePropertiesPage();

// Dialog Data
	enum { IDD = IDD_SCENE_PROPERTIES_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckSpecifyFrames();

public:
	virtual BOOL OnApply();
	TCommandGroup *m_pCmd;

protected:
	CString m_strName;
	int m_nFrames;
	BOOL m_bInit;

protected:
	virtual BOOL OnInitDialog();
	BOOL ValidateData();
	virtual BOOL OnKillActive();
	void UpdateControls();
	gldScene2 *m_pScene;
	void UpdateStatistics();
	void OnTotalFramesChanged();
};
