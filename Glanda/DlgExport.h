#pragma once

#include "FileDialogEx.h"

// CDlgExport

class CDlgExport : public CFileDialogEx
{
	DECLARE_DYNAMIC(CDlgExport)

public:
	CDlgExport(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	CDlgExport(LPCTSTR lpszSection, BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR lpszFileName = NULL, CWnd* pParentWnd = NULL);
	virtual ~CDlgExport();
	BOOL m_bCompress;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCheckCompress();

protected:
	void RecalcLayout();
public:
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnFileNameOK();
};
