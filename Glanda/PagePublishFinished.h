#pragma once

#include "PropertyPageFixed.h"

// CPagePublishFinished dialog

class CPagePublishFinished : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishFinished)

public:
	CPagePublishFinished();
	virtual ~CPagePublishFinished();

// Dialog Data
	enum { IDD = IDD_PUBLISH_FINISHED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
};
