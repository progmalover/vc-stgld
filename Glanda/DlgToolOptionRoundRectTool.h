#pragma once

#include "DlgToolOption.h"
#include "staticspinbutton.h"
#include "numberedit.h"
#include "staticbitmap.h"

// CDlgToolOptionRoundRectTool dialog

class CDlgToolOptionRoundRectTool : public CDlgToolOption
{
public:
	CDlgToolOptionRoundRectTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToolOptionRoundRectTool();

	DECLARE_SINGLETON(CDlgToolOptionRoundRectTool)

// Dialog Data
	enum { IDD = IDD_TOOL_OPTION_ROUND_RECT_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStaticSpinButton m_btnRadius;
	CNumberEdit m_edtRadius;
	afx_msg void OnRadiusModified();
	CStaticBitmap m_stcRadius;

public:
	virtual void UpdateControls();
};
