#pragma once

class gldSound;
class TCommandGroup;
// CEditMCSoundPage dialog

class CEditMCSoundPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditMCSoundPage)

public:
	TCommandGroup* m_pCmd;
	CEditMCSoundPage();
	virtual ~CEditMCSoundPage();

// Dialog Data
	enum { IDD = IDD_EDIT_MC_SOUND_PAGE };

protected:
	void HandleSelChange();
	void HandleSoundModified(bool bRemove = false);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeSoundList();
	afx_msg void OnBnClickedBtnReplaceSound();
	afx_msg void OnBnClickedBtnPlaySound();
	afx_msg void OnBnClickedBtnRemoveSound();
};
