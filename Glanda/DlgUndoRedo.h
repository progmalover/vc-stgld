#pragma once

#include "UndoListBox.h"

// CDlgUndo dialog

class CDlgUndo : public CDialog
{
	DECLARE_DYNAMIC(CDlgUndo)

public:
	CDlgUndo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUndo();

// Dialog Data
	enum { IDD = IDD_UNDO };

	DECLARE_SINGLETON(CDlgUndo)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnFloatStatus(WPARAM wParam, LPARAM);
	afx_msg void OnKillFocusList();

	BOOL m_bUndo;
	CUndoListBox m_list;
	void LoadList(BOOL bUndo);
	BOOL TrackMessage();
	void UpdateStatus(int nCurSel);
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStaticStatus();
};
