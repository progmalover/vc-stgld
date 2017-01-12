#pragma once


// CDlgApplyTemplate dialog
#include "ListTemplate.h"
#include "shockwaveflash.h"
#include "ResizableDialog.h"

class CDlgApplyTemplate : public CResizableDialog
{	
public:
	CDlgApplyTemplate(CListTemplate::FilterTemplate filter, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgApplyTemplate();

// Dialog Data
	enum { IDD = IDD_APPLY_TEMPLATE };

	const CString &GetTemplatePath() const { return m_strTemplatePath; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CListTemplate m_lstTemplate;
	CListTemplate::FilterTemplate m_Filter;
	CShockwaveFlash m_Player;
	CString m_strTemplatePath;
	CString m_strPreviewFile;

private:
	void UpdateControls();

public:
	virtual BOOL OnInitDialog();
	void Reposition();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLbnSelchangeListTemplate();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBtnRename();
	afx_msg void OnBnClickedBtnDelete();
};
