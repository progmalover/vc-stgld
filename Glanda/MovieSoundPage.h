#pragma once

#include "numberedit.h"
#include "flatcolorbutton.h"
#include "staticspinbutton.h"
#include "FlatBitmapButton.h"
#include "HistoryComboBox.h"

class TCommandGroup;

// CMovieSoundPage dialog

class CMovieSoundPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMovieSoundPage)

	CToolTipCtrl m_tooltip;
public:
	CMovieSoundPage();
	virtual ~CMovieSoundPage();

// Dialog Data
	enum { IDD = IDD_MOVIE_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CNumberEdit m_edtLoop;

	CFlatBitmapButton m_btnBrowse;
	CFlatBitmapButton m_btnPlay;
	CStaticSpinButton m_spnLoop;

	CComboBox m_cmbScenes;
	CString m_strSoundStartScene;
	BOOL m_bSoundLoop;
	int m_nSoundLoopTimes;

	virtual BOOL OnInitDialog();
	TCommandGroup* m_pCmd;
	TCommandGroup* m_pCmdAddObj;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonPlaySound();
	afx_msg void OnBnClickedCheckLoop();
	virtual BOOL OnApply();
	afx_msg void OnCbnSelchangeComboSoundList();
};
