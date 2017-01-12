// ScenePropertiesPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include ".\scenepropertiespage.h"

#include "my_app.h"
#include "GlandaDoc.h"

#include "NumberEdit.h"

// CScenePropertiesPage dialog

CScenePropertiesPage::CScenePropertiesPage()
	: CResizablePage(CScenePropertiesPage::IDD)
{
	m_pCmd = NULL;

	m_pScene = _GetCurScene2();
	m_strName = m_pScene->m_name.c_str();
	m_nFrames = m_pScene->GetMaxTime(FALSE);

	m_bInit = FALSE;
}

CScenePropertiesPage::~CScenePropertiesPage()
{
}

void CScenePropertiesPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_FRAMES, m_nFrames);

	DDV_MaxChars(pDX, m_strName, gldScene2::MAX_SCENE_NAME);
	DDV_MinMaxUInt(pDX, m_nFrames, gldScene2::MIN_SCENE_TIME, gldScene2::MAX_SCENE_TIME);
}

BEGIN_MESSAGE_MAP(CScenePropertiesPage, CResizablePage)
	ON_BN_CLICKED(IDC_CHECK_SPECIFY_FRAMES, OnBnClickedCheckSpecifyFrames)
	ON_EN_CHANGE(IDC_EDIT_FRAMES, OnTotalFramesChanged)
END_MESSAGE_MAP()


// CScenePropertiesPage message handlers

BOOL CScenePropertiesPage::OnInitDialog()
{
	CResizablePage::OnInitDialog();

	// TODO:  Add extra initialization here

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_FRAMES))->SetRange32(gldScene2::MIN_SCENE_TIME, gldScene2::MAX_SCENE_TIME);

	if (m_pScene->m_specifiedFrames >= 0)
		CheckDlgButton(IDC_CHECK_SPECIFY_FRAMES, BST_CHECKED);

	UpdateStatistics();
	UpdateControls();

	AddAnchor(IDC_STATIC_STATISTICS, TOP_CENTER, TOP_CENTER);

	AddAnchor(IDC_STATIC_S1, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_S11, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_FRAME_START, TOP_CENTER, TOP_CENTER);

	AddAnchor(IDC_STATIC_S2, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_S21, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_FRAME_COUNT, TOP_CENTER, TOP_CENTER);

	AddAnchor(IDC_STATIC_S3, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_S31, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_TIME_START, TOP_CENTER, TOP_CENTER);

	AddAnchor(IDC_STATIC_S4, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_S41, TOP_CENTER, TOP_CENTER);
	AddAnchor(IDC_STATIC_TIME_LENGTH, TOP_CENTER, TOP_CENTER);

	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CScenePropertiesPage::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!ValidateData())
		return FALSE;

	if (m_pCmd)
		delete m_pCmd;
	m_pCmd = new TCommandGroup(IDS_CMD_CHANGESCENEPROPERTIES);
	m_pCmd->Do(new CCmdChangeSceneName(m_pScene, m_strName));
	m_pCmd->Do(new CCmdChangeSceneLength(m_pScene, (IsDlgButtonChecked(IDC_CHECK_SPECIFY_FRAMES) == BST_CHECKED) ? m_nFrames : -1));

	return CResizablePage::OnApply();
}

BOOL CScenePropertiesPage::ValidateData()
{
	if (!UpdateData(TRUE))
		return FALSE;

	m_strName.Trim();
	if (m_strName.IsEmpty())
	{
		AfxMessageBox(IDS_NEED_SCENE_NAME);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));
		return FALSE;
	}

	gldMainMovie2 *pMainMovie = _GetMainMovie2();
	for (GSCENE2_LIST::iterator it = pMainMovie->m_sceneList.begin(); it != pMainMovie->m_sceneList.end(); it++)
	{
		gldScene2 *pScene = *it;
		if (pScene != m_pScene && m_strName.CompareNoCase(pScene->m_name.c_str()) == 0)
		{
			AfxMessageBox(IDS_SCENE_NAME_EXISTS);
			GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CScenePropertiesPage::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class

	return ValidateData();
}

void CScenePropertiesPage::OnBnClickedCheckSpecifyFrames()
{
	UpdateControls();
}

void CScenePropertiesPage::UpdateControls()
{
	BOOL bEnable = (IsDlgButtonChecked(IDC_CHECK_SPECIFY_FRAMES) == BST_CHECKED);
	GetDlgItem(IDC_EDIT_FRAMES)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_FRAMES)->EnableWindow(bEnable);

	if (!bEnable)
		SetDlgItemInt(IDC_EDIT_FRAMES, m_pScene->GetMaxTime(TRUE));
}

void CScenePropertiesPage::UpdateStatistics()
{
	gldMainMovie2 *pMovie = _GetMainMovie2();

	int nStartFrame = pMovie->GetSceneStartTime(m_pScene);
	int nFrames = -1;
	if (IsDlgButtonChecked(IDC_CHECK_SPECIFY_FRAMES) == BST_CHECKED)
		nFrames = GetDlgItemInt(IDC_EDIT_FRAMES, NULL, FALSE);
	else
		nFrames = m_pScene->GetMaxTime(TRUE);

	SetDlgItemInt(IDC_STATIC_FRAME_START, nStartFrame + 1);
	SetDlgItemInt(IDC_STATIC_FRAME_COUNT, nFrames);

	double fStartTime = nStartFrame / pMovie->m_frameRate;
	double fTime = nFrames / pMovie->m_frameRate;
	SetDlgItemFloat(m_hWnd, IDC_STATIC_TIME_START, fStartTime, 2, TRUE);
	SetDlgItemFloat(m_hWnd, IDC_STATIC_TIME_LENGTH, fTime, 2, TRUE);
}

void CScenePropertiesPage::OnTotalFramesChanged()
{
	if (m_bInit)
	{
		BOOL bTrans;
		int nFrames = GetDlgItemInt(IDC_EDIT_FRAMES, &bTrans, FALSE);
		if (bTrans && nFrames >= gldScene2::MIN_SCENE_TIME && nFrames <= gldScene2::MAX_SCENE_TIME)
		{
			UpdateStatistics();
		}
	}
}
