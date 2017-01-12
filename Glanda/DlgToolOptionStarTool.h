#pragma once

#include "DlgToolOption.h"
#include "staticspinbutton.h"
#include "numberedit.h"
#include "staticbitmap.h"

// CDlgToolOptionStarTool dialog

class CDlgToolOptionStarTool : public CDlgToolOption
{
public:
	CDlgToolOptionStarTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToolOptionStarTool();

	DECLARE_SINGLETON(CDlgToolOptionStarTool)

// Dialog Data
	enum { IDD = IDD_TOOL_OPTION_STAR_TOOL };

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
