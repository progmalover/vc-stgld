#pragma once
#include "EditShapeLinePage.h"
#include "EditShapeFillPage.h"

class gldObj;
class TCommandGroup;

// CEditMCShapeSheet

class CEditMCShapeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEditMCShapeSheet)

public:
	CEditMCShapeSheet(gldObj* pObj);
	virtual ~CEditMCShapeSheet();
	TCommandGroup* m_pCmd;

protected:
	gldObj* m_pObj;
	CEditShapeLinePage m_editLinePage;
	CEditShapeFillPage m_editFillPage;

	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
};


