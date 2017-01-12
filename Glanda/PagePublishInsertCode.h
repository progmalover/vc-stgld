#pragma once

#include "PropertyPageFixed.h"
#include "afxcmn.h"

// CPagePublishInsertCode dialog

class CPagePublishInsertCode : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishInsertCode)

public:
	CPagePublishInsertCode();
	virtual ~CPagePublishInsertCode();

// Dialog Data
	enum { IDD = IDD_PUBLISH_INSERT_CODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_edtFile;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonSave();
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();

protected:
	BOOL m_bChanged;
	BOOL m_bInserted;
	BOOL m_bSaved;
	BOOL DoSave();
public:
	afx_msg void OnEnChangeRicheditFile();
	afx_msg void OnBnClickedButtonPreview();
};
