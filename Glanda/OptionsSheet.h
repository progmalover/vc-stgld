#pragma once

#include "OptionsPageGeneral.h"
#include "OptionsPageDesign.h"
#include "OptionsPageTimeLine.h"
#include "OptionsPageAS.h"
#include "OptionsPagePreview.h"

// COptionsSheet

class COptionsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsSheet)

public:
	COptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsSheet();

protected:
	DECLARE_MESSAGE_MAP()

	void RecalcLayout();
	static int m_nActivePage;

public:

	COptionsPageGeneral m_pageGeneral;
	COptionsPageDesign m_pageDesign;
	COptionsPageTimeLine m_pageTimeLine;
	COptionsPageAS m_pageAS;
	COptionsPagePreview m_pagePreview;
	CButton m_btnReset;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReset();
};
