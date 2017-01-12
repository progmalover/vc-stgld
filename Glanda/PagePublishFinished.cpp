// PagePublishFinished.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "PagePublishFinished.h"
#include ".\pagepublishfinished.h"


// CPagePublishFinished dialog

IMPLEMENT_DYNAMIC(CPagePublishFinished, CPropertyPageFixed)
CPagePublishFinished::CPagePublishFinished()
	: CPropertyPageFixed(CPagePublishFinished::IDD)
{
	Construct(IDD, IDS_PUBLISH, IDS_PUBLISH_FINISHED, 0);
}

CPagePublishFinished::~CPagePublishFinished()
{
}

void CPagePublishFinished::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageFixed::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPagePublishFinished, CPropertyPageFixed)
END_MESSAGE_MAP()


// CPagePublishFinished message handlers

BOOL CPagePublishFinished::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_FINISH);
	CancelToClose();

	return CPropertyPageFixed::OnSetActive();
}
