#pragma once

#include "ResizablePage.h"
#include "DlgInstanceAction.h"
#include "EventsListBox.h"

#include <vector>

class TCommandGroup;

// CInstanceActionPage dialog

class CInstanceActionPage : public CResizablePage
{
	struct EVENT
	{
		CString strEvent;
		CString strAction;
	};

	typedef std::vector<EVENT> EVENT_LIST;

public:
	CInstanceActionPage();
	virtual ~CInstanceActionPage();

// Dialog Data
	enum { IDD = IDD_INSTANCE_ACTION_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	EVENT_LIST m_eventList;
	CEventsListBox m_list;
	int m_nCurSel;

	BOOL FindEvent(LPCTSTR lpszAction, LPCTSTR lpszEvent, CString &strAction);
	void ParseAction();
	void UpdateListBoxItems();

public:
	virtual BOOL OnInitDialog();
	CDlgInstanceAction m_dlgAction;
	afx_msg void OnSelChangeEvents();
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();

	TCommandGroup *m_pCmd;
	afx_msg void OnBnClickedButtonClearAll();
};
