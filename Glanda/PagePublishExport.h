#pragma once

#include "PropertyPageFixed.h"

// CPagePublishExport dialog

class CPagePublishExport : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishExport)

public:
	CPagePublishExport();
	virtual ~CPagePublishExport();

// Dialog Data
	enum { IDD = IDD_PUBLISH_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonBrowse();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardBack();
};
