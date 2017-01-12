// PropertyPageFixed.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyPageFixed.h"

extern AUX_DATA afxData;

// CPropertyPageFixed dialog

IMPLEMENT_DYNAMIC(CPropertyPageFixed, CPropertyPage)
CPropertyPageFixed::CPropertyPageFixed(UINT nIDD, UINT nIDCaption)
	: CPropertyPage(nIDD, nIDCaption)
{
}

CPropertyPageFixed::~CPropertyPageFixed()
{
}

void CPropertyPageFixed::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

void CPropertyPageFixed::Construct(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle)
{
	if (_AfxGetComCtlVersion() < MAKELONG(80, 5))
	{
		nIDHeaderTitle = 0;
		nIDHeaderSubTitle = 0;
	}

	CPropertyPage::Construct(nIDTemplate, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle);
}

BEGIN_MESSAGE_MAP(CPropertyPageFixed, CPropertyPage)
END_MESSAGE_MAP()


// CPropertyPageFixed message handlers
