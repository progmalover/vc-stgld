// DlgMovieProperties.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "MoviePropertiesPage.h"

#include "gldMainMovie2.h"
#include "my_app.h"
#include "GlandaDoc.h"
#include "gldMovieSound.h"
#include "gldSound.h"

// CMoviePropertiesPage dialog

IMPLEMENT_DYNAMIC(CMoviePropertiesPage, CPropertyPage)
CMoviePropertiesPage::CMoviePropertiesPage()
	: CPropertyPage(CMoviePropertiesPage::IDD)
{
	m_pCmd = NULL;
}

CMoviePropertiesPage::~CMoviePropertiesPage()
{
}

void CMoviePropertiesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_BACKGROUND, m_btnBackground);
	DDX_Control(pDX, IDC_EDIT_FRAME_RATE, m_edtFrameRate);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edtWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtHeight);

	DDX_Control(pDX, IDC_STATIC_SPIN_FRAME_RATE, m_spnFrameRate);
	DDX_Control(pDX, IDC_STATIC_SPIN_WIDTH, m_spnWidth);
	DDX_Control(pDX, IDC_STATIC_SPIN_HEIGHT, m_spnHeight);
}


BEGIN_MESSAGE_MAP(CMoviePropertiesPage, CPropertyPage)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_FRAME_RATE, OnFrameRateModified)
END_MESSAGE_MAP()


// CMoviePropertiesPage message handlers

BOOL CMoviePropertiesPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	m_edtFrameRate.SetRange(2, 0.01, 120, 12, FALSE, TRUE);
	m_spnFrameRate.SetRange(1, 120);
	m_spnFrameRate.SetBuddyWindow(m_edtFrameRate);

	m_edtWidth.SetRange(1, 2880, 400, FALSE, TRUE);
	m_spnWidth.SetRange(1, 2880);
	m_spnWidth.SetBuddyWindow(m_edtWidth);

	m_edtHeight.SetRange(1, 2880, 300, FALSE, TRUE);
	m_spnHeight.SetRange(1, 2880);
	m_spnHeight.SetBuddyWindow(m_edtHeight);

	gldMainMovie2 *pMovie = _GetMainMovie2();
	SetDlgItemFloat(m_hWnd, IDC_EDIT_FRAME_RATE, pMovie->m_frameRate, 2, TRUE);
	SetDlgItemInt(IDC_EDIT_WIDTH, pMovie->m_width, FALSE);
	SetDlgItemInt(IDC_EDIT_HEIGHT, pMovie->m_height, FALSE);
	m_btnBackground.SetColor(pMovie->m_color & 0x00ffffff, 255);

	UpdateStatistics();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMoviePropertiesPage::OnApply()
{
	if (!UpdateData(TRUE))
		return FALSE;

	COLORREF color = m_btnBackground.GetColor();
	float fFrameRate = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_FRAME_RATE, NULL);
	int nWidth= GetDlgItemInt(IDC_EDIT_WIDTH, NULL, FALSE);
	int nHeight = GetDlgItemInt(IDC_EDIT_HEIGHT, NULL, FALSE);	

	if (m_pCmd)
		delete m_pCmd;
	m_pCmd = new CCmdChangeMovieProperties(fFrameRate, nWidth, nHeight, color);

	return CPropertyPage::OnApply();
}

void CMoviePropertiesPage::OnOK()
{
	// TODO: Add your control notification handler code here

	CPropertyPage::OnOK();

	if (IsDlgButtonChecked(IDC_CHECK_SAVE_AS_DEFAULT) == BST_CHECKED)
	{
		float fFrameRate = (float)GetDlgItemFloat(m_hWnd, IDC_EDIT_FRAME_RATE, NULL);
		int nWidth= GetDlgItemInt(IDC_EDIT_WIDTH, NULL, FALSE);
		int nHeight = GetDlgItemInt(IDC_EDIT_HEIGHT, NULL, FALSE);	
		COLORREF color = m_btnBackground.GetColor();

		TCHAR szBuf[_CVTBUFSIZE];
		_gcvt(fFrameRate, 32, szBuf);

		CWinApp *pApp = AfxGetApp();

		pApp->WriteProfileString("Movie", "Frame Rate", szBuf);
		pApp->WriteProfileInt("Movie", "Width", nWidth);
		pApp->WriteProfileInt("Movie", "Height", nHeight);
		pApp->WriteProfileInt("Movie", "Background", color);
	}
}

void CMoviePropertiesPage::UpdateStatistics()
{
	gldMainMovie2 *pMovie = _GetMainMovie2();

	int nScenes = 0;
	int nFrames = 0;
	float fTime = 0.0;
	int nObjects = 0;
	int nSounds = 0;

	for (GSCENE2_LIST::iterator it = pMovie->m_sceneList.begin(); it != pMovie->m_sceneList.end(); it++)
	{
		nScenes++;

		gldScene2 *pScene = *it;
		nFrames += pScene->GetMaxTime(FALSE);

		for (GINSTANCE_LIST::iterator it = pScene->m_instanceList.begin(); it != pScene->m_instanceList.end(); it++)
		{
			nObjects++;
		}
		for (GSCENESOUND_LIST::iterator it = pScene->m_soundList.begin(); it != pScene->m_soundList.end(); it++)
		{
			nSounds++;
		}
	}

	if (pMovie->m_pMovieSound)
		nSounds++;

	SetDlgItemInt(IDC_STATIC_SCENES, nScenes);
	SetDlgItemInt(IDC_STATIC_FRAMES, nFrames);
	SetDlgItemInt(IDC_STATIC_OBJECTS, nObjects);
	SetDlgItemInt(IDC_STATIC_SOUNDS, nSounds);

	float fFrameRate = (float)m_edtFrameRate.GetValueFloat();
	fTime = (float)nFrames / fFrameRate;
	SetDlgItemFloat(m_hWnd, IDC_STATIC_TIME, fTime, 2, TRUE);
}

void CMoviePropertiesPage::OnFrameRateModified()
{
	double fFrameRate = (double)m_edtFrameRate.GetValueFloat();
	if (fFrameRate >= 0.01 && fFrameRate <= 120.00)
	{
		UpdateStatistics();
	}
}
