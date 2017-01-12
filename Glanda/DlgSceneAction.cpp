// SceneActionExitPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgSceneAction.h"

#include "GlandaDoc.h"
#include "my_app.h"

#include "gldAction.h"

#include "ASView.h"
#include "ASDoc.h"

#include "Regexx.h"

using namespace regexx;

// CDlgSceneAction dialog

CDlgSceneAction::CDlgSceneAction()
	: CResizableToolTipDialog(CDlgSceneAction::IDD)
{
	m_pstrAction = NULL;
	m_pView = NULL;
}

CDlgSceneAction::~CDlgSceneAction()
{
}

void CDlgSceneAction::DoDataExchange(CDataExchange* pDX)
{
	CResizableToolTipDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_URL, m_cmbURL);
	DDX_Control(pDX, IDC_COMBO_TARGET, m_cmbTarget);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_cmbScenes);
	DDX_Text(pDX, IDC_COMBO_URL, m_strURL);
	DDX_Text(pDX, IDC_COMBO_TARGET, m_strTarget);
	DDX_CBStringExact(pDX, IDC_COMBO_SCENES, m_strScene);

	DDX_Control(pDX, IDC_BUTTON_CHECK_SYNTAX, m_btnCheckSyntax);
	DDX_Control(pDX, IDC_BUTTON_MEMBER_LIST, m_btnMemberList);
	DDX_Control(pDX, IDC_BUTTON_PARAMETER_INFO, m_btnParameterInfo);
	DDX_Control(pDX, IDC_BUTTON_AUTO_COMPLETE, m_btnAutoComplete);

	if (pDX->m_bSaveAndValidate)
	{
		if (IsDlgButtonChecked(IDC_RADIO_GET_URL) == BST_CHECKED && m_strURL.IsEmpty())
		{
			AfxMessageBox(IDS_NEED_URL);
			pDX->Fail();
		}

		if (IsDlgButtonChecked(IDC_RADIO_GOTO_SCENE) == BST_CHECKED && m_strScene.IsEmpty())
		{
			AfxMessageBox(IDS_NEED_SELECT_SCENE);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgSceneAction, CResizableToolTipDialog)
	ON_BN_CLICKED(IDC_RADIO_NONE, OnBnClickedRadioNextScene)
	ON_BN_CLICKED(IDC_RADIO_GET_URL, OnBnClickedRadioGetUrl)
	ON_BN_CLICKED(IDC_RADIO_GOTO_SCENE, OnBnClickedRadioGotoScene)
	ON_BN_CLICKED(IDC_RADIO_STOP, OnBnClickedRadioStop)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, OnBnClickedRadioCustom)

	ON_BN_CLICKED(IDC_BUTTON_MEMBER_LIST, OnBnClickedButtonMemberList)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_INFO, OnBnClickedButtonParameterInfo)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_COMPLETE, OnBnClickedButtonAutoComplete)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_SYNTAX, OnBnClickedButtonCheckSyntax)
END_MESSAGE_MAP()


// CDlgSceneAction message handlers

void CDlgSceneAction::ParseAction()
{
	CheckDlgButton(IDC_RADIO_NONE, BST_UNCHECKED);
	CheckDlgButton(IDC_RADIO_GET_URL, BST_UNCHECKED);
	CheckDlgButton(IDC_RADIO_GOTO_SCENE, BST_UNCHECKED);
	CheckDlgButton(IDC_RADIO_STOP, BST_UNCHECKED);
	CheckDlgButton(IDC_RADIO_CUSTOM, BST_UNCHECKED);

	m_strURL.Empty();
	m_strTarget.Empty();
	m_strScene.Empty();
	m_cmbScenes.SetCurSel(-1);

	CCrystalTextBuffer *pBuf = m_pView->LocateTextBuffer();			
	int nLines = pBuf->GetLineCount();
	if (nLines > 0)
	{
		int len = pBuf->GetLineLength(nLines - 1);
		if (nLines > 1 || len > 0)
			pBuf->DeleteText(m_pView, 0, 0, nLines - 1, len);
	}

	UINT nRadio = IDC_RADIO_NONE;
	if (!m_pstrAction->IsEmpty())
	{
		Regexx re;

		LPCTSTR lpszAction = (LPCTSTR)*m_pstrAction;

		// _root.getURL
		if (re.exec(lpszAction, "^[ \t\r\n]*_root\\.getURL\\(\"(.*)\",[ \t]*\"(.*)\"\\);[ \t\r\n]*$", Regexx::nocase | Regexx::dotall) != 0)
		{
			nRadio = IDC_RADIO_GET_URL;
			m_strURL = re.match[0].atom[0].str().c_str();
			m_strTarget = re.match[0].atom[1].str().c_str();
		}
		else	// _root.gotoAndPlay
		if (re.exec(lpszAction, "^[ \t\r\n]*_root\\.gotoAndPlay[ \t]*\\(\"(.*)\"\\);[ \t\r\n]*$", Regexx::nocase | Regexx::dotall) != 0)
		{
			nRadio = IDC_RADIO_GOTO_SCENE;
			m_strScene = re.match[0].atom[0].str().c_str();
		}
		else	// _root.stop
		if (re.exec(lpszAction, "^[ \t\r\n]*_root\\.stop[ \t]*\\(\\);[ \t\r\n]*$", Regexx::nocase | Regexx::dotall) != 0)
		{
			nRadio = IDC_RADIO_STOP;
		}
		else	// custom
		{
			nRadio = IDC_RADIO_CUSTOM;
			m_pView->InsertText(lpszAction);
			m_pView->SetCursorPosSimply(CPoint(0, 0));
			m_pView->LocateTextBuffer()->EmptyUndoBuffer();
		}
	}

	CheckDlgButton(nRadio, BST_CHECKED);

	UpdateData(FALSE);

	UpdateControls();
}

BOOL CDlgSceneAction::OnInitDialog()
{
	CResizableToolTipDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_btnCheckSyntax.LoadBitmap(IDB_BUTTON_CHECK_SYNTAX, 1);
	m_btnMemberList.LoadBitmap(IDB_BUTTON_MEMBER_LIST, 1);
	m_btnParameterInfo.LoadBitmap(IDB_BUTTON_PARAMETER_INFO, 1);
	m_btnAutoComplete.LoadBitmap(IDB_BUTTON_AUTO_COMPLETE, 1);

	CCreateContext ctx;
	ctx.m_pCurrentDoc = new CASDoc;
	ctx.m_pNewViewClass = RUNTIME_CLASS(CASView);
	m_pView = (CASView *) ((CFrameWnd*)this)->CreateView(&ctx);
	ASSERT(m_pView);
	if (m_pView)
	{
		CString strAction;
		strAction.LoadString(IDS_KEY_ACTION);
		m_pView->SetWindowText(strAction);

		CCrystalTextBuffer *pBuf = m_pView->LocateTextBuffer();
		pBuf->InitNew();

		m_pView->OnInitialUpdate();

		m_pView->ModifyStyle(0, WS_TABSTOP);

		CRect rc;
		GetDlgItem(IDC_STATIC_ACTIONSCRIPT)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_pView->MoveWindow(&rc);
	}

	AddAnchor(IDC_COMBO_URL, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_TARGET, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_COMBO_TARGET, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_COMBO_SCENES, TOP_LEFT, TOP_RIGHT);
	AddAnchor(m_pView->m_hWnd, TOP_LEFT, BOTTOM_RIGHT);

	m_cmbURL.Init("getURL");
	m_cmbURL.Load();

	FillComboBox((CComboBox *)GetDlgItem(IDC_COMBO_TARGET), IDS_LIST_TARGET);

	gldMainMovie2 *pMovie = _GetMainMovie2();
	for (GSCENE2_LIST::iterator it = pMovie->m_sceneList.begin(); it != pMovie->m_sceneList.end(); it++)
	{
		gldScene2 *pScene = *it;
		m_cmbScenes.AddString(pScene->m_name.c_str());
	}

	AddToolTip(IDC_BUTTON_CHECK_SYNTAX);
	AddToolTip(IDC_BUTTON_MEMBER_LIST);
	AddToolTip(IDC_BUTTON_PARAMETER_INFO);
	AddToolTip(IDC_BUTTON_AUTO_COMPLETE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSceneAction::UpdateControls()
{
	BOOL bComboURL = FALSE;
	BOOL bComboScenes = FALSE;
	BOOL bEditActionScript = FALSE;

	if (IsDlgButtonChecked(IDC_RADIO_GET_URL) == BST_CHECKED)
	{
		bComboURL = TRUE;
	}
	else if (IsDlgButtonChecked(IDC_RADIO_GOTO_SCENE) == BST_CHECKED)
	{
		bComboScenes = TRUE;
	}
	else if (IsDlgButtonChecked(IDC_RADIO_CUSTOM) == BST_CHECKED)
	{
		bEditActionScript = TRUE;
	}

	GetDlgItem(IDC_COMBO_URL)->EnableWindow(bComboURL);
	GetDlgItem(IDC_COMBO_TARGET)->EnableWindow(bComboURL);
	GetDlgItem(IDC_COMBO_SCENES)->EnableWindow(bComboScenes);
	m_pView->EnableWindow(bEditActionScript);

	m_btnCheckSyntax.ShowWindow(bEditActionScript ? SW_SHOW : SW_HIDE);
	m_btnMemberList.ShowWindow(bEditActionScript ? SW_SHOW : SW_HIDE);
	m_btnParameterInfo.ShowWindow(bEditActionScript ? SW_SHOW : SW_HIDE);
	m_btnAutoComplete.ShowWindow(bEditActionScript ? SW_SHOW : SW_HIDE);
}

void CDlgSceneAction::OnBnClickedRadioNextScene()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}


void CDlgSceneAction::OnBnClickedRadioGetUrl()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}

void CDlgSceneAction::OnBnClickedRadioGotoScene()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}

void CDlgSceneAction::OnBnClickedRadioStop()
{
	// TODO: Add your control notification handler code here

	UpdateControls();
}

void CDlgSceneAction::OnBnClickedRadioCustom()
{
	// TODO: Add your control notification handler code here

	UpdateControls();

	m_pView->SetFocus();
}

BOOL CDlgSceneAction::UpdateAction()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!UpdateData(TRUE))
		return FALSE;

	CString &strAction = *m_pstrAction;
	if (IsDlgButtonChecked(IDC_RADIO_NONE) == BST_CHECKED)
	{
		strAction.Empty();
	}
	else if (IsDlgButtonChecked(IDC_RADIO_GET_URL) == BST_CHECKED)
	{
		if (m_strURL.Trim().IsEmpty())
		{
			AfxMessageBox(IDS_NEED_URL);
			return FALSE;
		}

		m_cmbURL.AddCurrentText(TRUE);

		strAction.Format("_root.getURL(\"%s\", \"%s\");", (LPCTSTR)m_strURL, (LPCTSTR)m_strTarget);
		m_cmbURL.Save();
	}
	else if (IsDlgButtonChecked(IDC_RADIO_GOTO_SCENE) == BST_CHECKED)
	{
		if (m_strScene.Trim().IsEmpty())
		{
			AfxMessageBox(IDS_NEED_SELECT_SCENE);
			return FALSE;
		}

		strAction.Format("_root.gotoAndPlay(\"%s\");", (LPCTSTR)m_strScene);
	}
	else if (IsDlgButtonChecked(IDC_RADIO_STOP) == BST_CHECKED)
	{
		strAction = "_root.stop();";
	}
	else if (IsDlgButtonChecked(IDC_RADIO_CUSTOM) == BST_CHECKED)
	{
		m_pView->GetText(strAction);
		if (!CheckSyntax(strAction))
			return FALSE;
	}

	return TRUE;
}

BOOL CDlgSceneAction::CheckSyntax(LPCTSTR lpszCode)
{
	unsigned char *pBinCode = NULL;
	int nSize = 0;

	// do not add \r\n so the compiler can indicate the error at the correct line
	CString strError;
	if (CompileActionScript(actionSegFrame, FALSE, lpszCode, &pBinCode, &nSize, strError))
	{
		if (pBinCode != NULL)
			delete pBinCode;
		return TRUE;
	}
	else
	{
		AfxMessageBox(strError, MB_ICONWARNING | MB_OK);
		return FALSE;
	}
}

void CDlgSceneAction::OnBnClickedButtonMemberList()
{
	// TODO: Add your control notification handler code here

	m_pView->SendMessage(WM_COMMAND, ID_EDIT_LIST_MEMBERS);
}

void CDlgSceneAction::OnBnClickedButtonParameterInfo()
{
	// TODO: Add your control notification handler code here

	m_pView->SendMessage(WM_COMMAND, ID_EDIT_PARAMETER_INFO);
}

void CDlgSceneAction::OnBnClickedButtonAutoComplete()
{
	// TODO: Add your control notification handler code here

	m_pView->SendMessage(WM_COMMAND, ID_EDIT_COMPLETE_WORD);
}

void CDlgSceneAction::OnBnClickedButtonCheckSyntax()
{
	// TODO: Add your control notification handler code here

	CString strText;
	m_pView->GetText(strText);

	if (strText.IsEmpty())
	{
		AfxMessageBox(IDS_AS_NOTHING_TO_DO);
		return;
	}

	if (CheckSyntax(strText))
		AfxMessageBox(IDS_CHECK_SYNTAX_NO_ERROR, MB_ICONINFORMATION | MB_OK);
}
