// DlgMovieProperties.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "MovieSoundPage.h"

#include "gldMainMovie2.h"
#include "my_app.h"
#include "GlandaDoc.h"
#include "gldMovieSound.h"
#include "CmdModifyMovieSound.h"
#include "gldSound.h"
#include "CmdAddShape.h"
#include "FileDialogEx.h"
#include "FilePath.h"
#include "gldMovieSound.h"
#include "GlandaDoc.h"
#include "PlaySoundDialog.h"

// CMovieSoundPage dialog

IMPLEMENT_DYNAMIC(CMovieSoundPage, CPropertyPage)
CMovieSoundPage::CMovieSoundPage()
	: CPropertyPage(CMovieSoundPage::IDD)
{
	m_pCmd			= NULL;
	m_pCmdAddObj	= NULL;
}

CMovieSoundPage::~CMovieSoundPage()
{
}

void CMovieSoundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_LOOP, m_edtLoop);
	DDX_Control(pDX, IDC_STATIC_SPIN_LOOP, m_spnLoop);

	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_BUTTON_PLAY_SOUND, m_btnPlay);

	DDX_Control(pDX, IDC_COMBO_SCENES, m_cmbScenes);

	DDX_CBStringExact(pDX, IDC_COMBO_SCENES, m_strSoundStartScene);
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bSoundLoop);
	DDX_Text(pDX, IDC_EDIT_LOOP, m_nSoundLoopTimes);
}


BEGIN_MESSAGE_MAP(CMovieSoundPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_SOUND, OnBnClickedButtonPlaySound)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheckLoop)
	ON_CBN_SELCHANGE(IDC_COMBO_SOUND_LIST, OnCbnSelchangeComboSoundList)
END_MESSAGE_MAP()


// CMovieSoundPage message handlers

BOOL CMovieSoundPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	gldMainMovie2 *pMovie = _GetMainMovie2();

	m_btnBrowse.LoadBitmap(IDB_BUTTON_BROWSE);
	m_btnPlay.LoadBitmap(IDB_BUTTON_PLAY);

	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	CString str;
	GetDlgItemText(IDC_BUTTON_BROWSE, str);
	m_tooltip.AddTool(&m_btnBrowse, str);
	str.LoadString(IDS_BTN_PLAY_SOUND);
	m_tooltip.AddTool(&m_btnPlay, str);

	CComboBox* pCmbSoundList = (CComboBox*)GetDlgItem(IDC_COMBO_SOUND_LIST);
	CString strMovieSoundNone, strMovieSoundUnnamed;
	strMovieSoundNone.LoadString(IDS_MOVIE_SOUND_NONE);
	strMovieSoundUnnamed.LoadString(IDS_MOVIE_SOUND_UNNAMED);
	int nItem = pCmbSoundList->AddString(strMovieSoundNone);
	pCmbSoundList->SetItemData(nItem, NULL);
	for(CObjectMap::_ObjectMap::iterator iter = _GetObjectMap()->m_Map.begin(); iter != _GetObjectMap()->m_Map.end(); ++iter)
	{
		gldObj* pObj = iter->first;
		if(pObj->IsGObjInstanceOf(gobjSound))
		{
			gldSound* pSound = (gldSound*)pObj;
			if(pSound->m_name.size())
			{
				nItem = pCmbSoundList->AddString(pSound->m_name.c_str());
			}
			else
			{
				nItem = pCmbSoundList->AddString(strMovieSoundUnnamed);
			}
			pCmbSoundList->SetItemData(nItem, (DWORD_PTR)pSound);
		}
	}

	if(pMovie->m_pMovieSound)
	{
		for(int i =0, j = pCmbSoundList->GetCount(); i < j; ++i)
		{
			if((gldSound*)pCmbSoundList->GetItemData(i)==pMovie->m_pMovieSound->m_sound->m_soundObj)
			{
				pCmbSoundList->SetCurSel(i);
				break;
			}
		}
	}
	else
	{
		pCmbSoundList->SetCurSel(0);
	}
	OnCbnSelchangeComboSoundList();

	for (GSCENE2_LIST::iterator it = pMovie->m_sceneList.begin(); it != pMovie->m_sceneList.end(); it++)
	{
		gldScene2 *pScene = *it;
		m_cmbScenes.AddString(pScene->m_name.c_str());
	}

	if(pMovie->m_pMovieSound)
	{
		m_strSoundStartScene = pMovie->m_pMovieSound->m_startScene.c_str();
		m_bSoundLoop = pMovie->m_pMovieSound->m_sound->m_loopFlag ? TRUE : FALSE;
		m_nSoundLoopTimes = min(100, max(1, pMovie->m_pMovieSound->m_sound->m_loopTimes));
	}
	else
	{
		m_strSoundStartScene.Empty();
		m_bSoundLoop = FALSE;
		m_nSoundLoopTimes = 1;
	}

	m_edtLoop.SetRange(1, 100, 1, FALSE, TRUE);
	m_spnLoop.SetRange(1, 100);
	m_spnLoop.SetBuddyWindow(m_edtLoop);

	UpdateData(FALSE);

	OnBnClickedCheckLoop();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMovieSoundPage::OnBnClickedButtonPlaySound()
{
	CComboBox* pCmbSoundList = (CComboBox*)GetDlgItem(IDC_COMBO_SOUND_LIST);
	int nCurSel = pCmbSoundList->GetCurSel();
	gldSound* pSound = (gldSound*)pCmbSoundList->GetItemData(nCurSel);

	if(pSound)
	{
		CPlaySoundDialog dlg(pSound);
		dlg.DoModal();
	}
}

void CMovieSoundPage::OnBnClickedButtonBrowse()
{
	CString strTitle, strFilter;
	strTitle.LoadString(IDS_SELECT_SOUND_TITLE);
	strFilter.LoadString(IDS_FILTER_SOUND_ALL);
	CFileDialogEx dlg(strTitle, TRUE, NULL, strFilter);
	dlg.m_pOFN->lpstrTitle = strTitle;
	if(dlg.DoModal()==IDOK)
	{
		CWaitCursor wc;

		gldSound* pSound = new gldSound();
		if(!pSound->ReadFile(dlg.GetPathName()))
		{
			delete pSound;
			AfxMessageBox(IDS_ERROR_IMPORT, MB_OK|MB_ICONERROR, NULL);
			return ;
		}

		pSound->m_name = FileStripPath(dlg.GetPathName());
		_GetObjectMap()->UpdateUniId(pSound);

		if(m_pCmdAddObj == NULL)
		{
			m_pCmdAddObj = new TCommandGroup(IDS_CMD_CHANGEMOVIEPROPERTIES);
		}

		m_pCmdAddObj->Do(new CCmdAddObj(pSound));

		CComboBox* pCmdSoundList = (CComboBox*)GetDlgItem(IDC_COMBO_SOUND_LIST);
		int nItem = pCmdSoundList->AddString(pSound->m_name.c_str());
		pCmdSoundList->SetItemData(nItem, (DWORD_PTR)pSound);
		pCmdSoundList->SetCurSel(nItem);
		OnCbnSelchangeComboSoundList();
	}
}

BOOL CMovieSoundPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		m_tooltip.RelayEvent(pMsg);

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CMovieSoundPage::OnBnClickedCheckLoop()
{
	BOOL bLoop = (IsDlgButtonChecked(IDC_CHECK_LOOP) == BST_CHECKED);
	m_edtLoop.EnableWindow(bLoop);
	m_spnLoop.EnableWindow(bLoop);
}

BOOL CMovieSoundPage::OnApply()
{
	if (!UpdateData(TRUE))
	{
		return FALSE;
	}

	if(m_pCmd)
	{
		m_pCmd->Unexecute();
		delete m_pCmd;
		m_pCmd = NULL;
	}

	CComboBox* pCmbSoundList = (CComboBox*)GetDlgItem(IDC_COMBO_SOUND_LIST);
	int nCurSel = pCmbSoundList->GetCurSel();
	gldSound* pSound = (gldSound*)pCmbSoundList->GetItemData(nCurSel);
	gldMovieSound* pOldMovieSound = _GetMainMovie2()->m_pMovieSound;
	gldSound* pOldSound = pOldMovieSound ? pOldMovieSound->m_sound->m_soundObj : NULL;

	if(pOldSound != pSound)
	{
		m_pCmd = new TCommandGroup(IDS_CMD_CHANGEMOVIEPROPERTIES);
		if(pSound)
		{
			gldMovieSound* pMovieSound = new gldMovieSound();
			pMovieSound->m_sound->m_soundObj = pSound;
			m_pCmd->Do(new CCmdModifyMovieSound(pMovieSound));
			m_pCmd->Do(new CCmdModifyMovieSoundProperties(pMovieSound, m_strSoundStartScene, m_bSoundLoop != FALSE, m_nSoundLoopTimes));
		}
		else
		{
			m_pCmd->Do(new CCmdModifyMovieSound(NULL));
		}
	}
	else if(pOldMovieSound)
	{
		m_pCmd = new TCommandGroup(IDS_CMD_CHANGEMOVIEPROPERTIES);
		m_pCmd->Do(new CCmdModifyMovieSoundProperties(pOldMovieSound, m_strSoundStartScene, m_bSoundLoop != FALSE, m_nSoundLoopTimes));
	}

	return CPropertyPage::OnApply();
}

void CMovieSoundPage::OnCbnSelchangeComboSoundList()
{
	CComboBox* pCmbSoundList = (CComboBox*)GetDlgItem(IDC_COMBO_SOUND_LIST);
	int nCurSel = pCmbSoundList->GetCurSel();
	gldSound* pSound = (gldSound*)pCmbSoundList->GetItemData(nCurSel);
	m_btnPlay.EnableWindow(pSound ? TRUE : FALSE);
}