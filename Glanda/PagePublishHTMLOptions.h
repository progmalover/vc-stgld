#pragma once

#include "PropertyPageFixed.h"

// CPagePublishHTMLOptions dialog

class CPagePublishHTMLOptions : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishHTMLOptions)

public:
	CPagePublishHTMLOptions();
	virtual ~CPagePublishHTMLOptions();

// Dialog Data
	enum { IDD = IDD_PUBLISH_HTML_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CComboBox m_cmbQuality;
	CComboBox m_cmbWmode;
	CComboBox m_cmbScale;
	CComboBox m_cmbFlashAlign;
	CComboBox m_cmbHTMLAlign;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();

public:
	void GenerateHTML();
	CString m_strHTML;
	virtual LRESULT OnWizardBack();
	virtual BOOL OnInitDialog();
};
