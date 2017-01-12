#pragma once

#include "ResizableDialog.h"
#include "TemplateListCtrl.h"

// CDlgTemplates dialog

class CDlgTemplates : public CResizableDialog
{
public:
	CDlgTemplates(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTemplates();

// Dialog Data
	enum { IDD = IDD_TEMPLATES };

protected:
	CTemplateListCtrl m_list;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL LoadTemplates();
	CLSID m_clsid;
protected:
	virtual void OnOK();
public:
	afx_msg void OnDestroy();
	afx_msg void OnDblclkList();
};
