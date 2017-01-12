#pragma once
#include "EditGeneralPage.h"
#include "EditMatrixPage.h"
#include "EditCxformPage.h"


// CEditTextSheet

class CEditTextSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEditTextSheet)
public:
	CEditTextSheet(gldInstance* pInstance);
	virtual ~CEditTextSheet();

protected:
	DECLARE_MESSAGE_MAP()
	gldInstance* m_pInstance;
	CEditGeneralPage m_editGeneralPage;
	CEditMatrixPage m_editMatrixPage;
	CEditCxformPage m_editCxformPage;
public:
	virtual INT_PTR DoModal();
};


