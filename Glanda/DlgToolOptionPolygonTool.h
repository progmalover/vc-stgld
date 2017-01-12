#pragma once

#include "DlgToolOption.h"
#include "staticspinbutton.h"
#include "numberedit.h"
#include "staticbitmap.h"

// CDlgToolOptionPolygonTool dialog

class CDlgToolOptionPolygonTool : public CDlgToolOption
{
public:
	CDlgToolOptionPolygonTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToolOptionPolygonTool();

	DECLARE_SINGLETON(CDlgToolOptionPolygonTool)

// Dialog Data
	enum { IDD = IDD_TOOL_OPTION_POLYGON_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	CStaticSpinButton m_btnPoints;
	CNumberEdit m_edtPoints;
	afx_msg void OnPointsModified();
	CStaticBitmap m_stcPoints;

public:
	virtual void UpdateControls();
};
