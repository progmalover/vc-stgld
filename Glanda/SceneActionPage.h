#pragma once

#include "ResizablePage.h"
#include "DlgSceneAction.h"
#include "EventsListBox.h"

class TCommandGroup;

// CSceneActionPage dialog

class CSceneActionPage : public CResizablePage
{
public:
	CSceneActionPage();
	virtual ~CSceneActionPage();

// Dialog Data
	enum { IDD = IDD_SCENE_ACTION_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CEventsListBox m_list;
	int m_nCurSel;

	CString m_strActionEnter;
	CString m_strActionExit;

	void UpdateListBoxItems();

public:
	virtual BOOL OnInitDialog();
	CDlgSceneAction m_dlgAction;
	afx_msg void OnSelChangeEvents();
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();

	TCommandGroup *m_pCmd;
	afx_msg void OnBnClickedButtonClearAll();
};
