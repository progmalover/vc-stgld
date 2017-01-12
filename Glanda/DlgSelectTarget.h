#pragma once

#include "ResizableDialog.h"
#include "TargetTree.h"

// CDlgSelectTarget dialog

class CDlgSelectTarget : public CResizableDialog
{
public:
	CDlgSelectTarget(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectTarget();

// Dialog Data
	enum { IDD = IDD_SELECT_TARGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTargetTree m_tree;
	CString m_strTarget;
	BOOL m_bShowNamedInstancesOnly;
	int m_nPathType;
	void CalcTargetPath(HTREEITEM hItem);

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckShowNamedOnly();
	afx_msg void OnBnClickedRadioAbsolute();
	afx_msg void OnBnClickedRadioRelative();
};
