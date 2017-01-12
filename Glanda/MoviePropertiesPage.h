#pragma once

#include "numberedit.h"
#include "flatcolorbutton.h"
#include "staticspinbutton.h"
#include "FlatBitmapButton.h"
#include "HistoryComboBox.h"

class TCommand;

// CMoviePropertiesPage dialog

class CMoviePropertiesPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMoviePropertiesPage)

public:
	CMoviePropertiesPage();
	virtual ~CMoviePropertiesPage();

// Dialog Data
	enum { IDD = IDD_MOVIE_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CNumberEdit m_edtFrameRate;
	CNumberEdit m_edtWidth;
	CNumberEdit m_edtHeight;

	CFlatColorButton m_btnBackground;
	CStaticSpinButton m_spnFrameRate;
	CStaticSpinButton m_spnWidth;
	CStaticSpinButton m_spnHeight;

	virtual BOOL OnInitDialog();

	TCommand *m_pCmd;
protected:
	virtual BOOL OnApply();
	virtual void OnOK();
	void UpdateStatistics();
	afx_msg void OnFrameRateModified();
};
