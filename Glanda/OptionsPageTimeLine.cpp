// OptionsPageTimeLine.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "OptionsPageTimeLine.h"

#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COptionsPageTimeLine dialog

IMPLEMENT_DYNAMIC(COptionsPageTimeLine, CPropertyPage)
COptionsPageTimeLine::COptionsPageTimeLine()
	: CPropertyPage(COptionsPageTimeLine::IDD)
{
}

COptionsPageTimeLine::~COptionsPageTimeLine()
{
}

void COptionsPageTimeLine::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_SHOW_NAME, COptions::Instance()->m_bShowTimelineObjName);
	DDX_Check(pDX, IDC_CHECK_SHOW_TOOLTIP, COptions::Instance()->m_bShowTimelineToolTip);
	DDX_Check(pDX, IDC_CHECK_SHOW_ALL_ELEMENTS, COptions::Instance()->m_bShowAllElementsInTimeline);
}


BEGIN_MESSAGE_MAP(COptionsPageTimeLine, CPropertyPage)
END_MESSAGE_MAP()


// COptionsPageTimeLine message handlers
