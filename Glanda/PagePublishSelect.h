#pragma once

#include "PropertyPageFixed.h"

#define PUBLISH_EXPORT_MOVIE	0
#define PUBLISH_INSERT_CODE		1
#define PUBLISH_VIEW_CODE		2

// CPagePublishSelect dialog

class CPagePublishSelect : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishSelect)

public:
	CPagePublishSelect();
	virtual ~CPagePublishSelect();

// Dialog Data
	enum { IDD = IDD_PUBLISH_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();

	BOOL m_nChoice;
	virtual BOOL OnInitDialog();
};
