#pragma once
#include "gld_graph.h"
#include "EditGeneralPage.h"
#include "EditShapeLinePage.h"
#include "EditShapeFillPage.h"
#include "EditMatrixPage.h"
#include "EditCxformPage.h"

// CEditShapeSheet

class CEditShapeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEditShapeSheet)

	gldInstance* m_pInstance;

public:
	CEditShapeSheet(gldInstance* pInstance);
	virtual ~CEditShapeSheet();

	gld_shape GetEditShape();

	CEditGeneralPage m_editGeneralPage;
	CEditShapeLinePage m_editLinePage;
	CEditShapeFillPage m_editFillPage;
	CEditMatrixPage m_editMatrixPage;
	CEditCxformPage m_editCxformPage;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
};


