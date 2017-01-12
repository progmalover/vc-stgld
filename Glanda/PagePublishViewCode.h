#pragma once

#include "PropertyPageFixed.h"

// CPagePublishViewCode dialog

class CPagePublishViewCode : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishViewCode)

public:
	CPagePublishViewCode();
	virtual ~CPagePublishViewCode();

// Dialog Data
	enum { IDD = IDD_PUBLISH_VIEW_CODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCopy();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	CRichEditCtrl m_edtCode;
	CString m_strCode;
	virtual BOOL OnInitDialog();
	int m_nFormat;
};
