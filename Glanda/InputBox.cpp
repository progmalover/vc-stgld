// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "InputBox.h"
#include ".\inputbox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// CInputBox dialog

IMPLEMENT_DYNAMIC(CInputBox, CDialog)

CInputBox::CInputBox(CWnd* pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt, LPCTSTR lpszInitText, INPUTBOX_VALIDATE_PROC pValidateProc, void *pData)
	: CDialog(CInputBox::IDD, pParent)
	, m_strCaption(lpszCaption)
	, m_strPrompt(lpszPrompt)
	, m_strText(lpszInitText)
	, m_pfnValidateProc(pValidateProc)
	, m_pData(pData)
{
}

CInputBox::CInputBox(CWnd* pParent, UINT nCaptionID, UINT nPromptID, LPCTSTR lpszInitText, INPUTBOX_VALIDATE_PROC pValidateProc, void *pData)
: CDialog(CInputBox::IDD, pParent)	
	, m_pfnValidateProc(pValidateProc)
	, m_pData(pData)
	, m_strText(lpszInitText)
{
	m_strCaption.LoadString(nCaptionID);
	m_strPrompt.LoadString(nPromptID);	
}

CInputBox::~CInputBox()
{
}

void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_strText);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_stcPrompt);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_edtText);
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
END_MESSAGE_MAP()


// CInputBox message handlers

BOOL CInputBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowText(m_strCaption);
	m_stcPrompt.SetWindowText(m_strPrompt);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInputBox::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	UpdateData(TRUE);

	if (m_pfnValidateProc && !m_pfnValidateProc(m_pData, m_strText))
		return;

	CDialog::OnOK();
}

BOOL InputBox(LPCTSTR lpszCaption, LPCTSTR lpszPrompt, LPCTSTR lpszInitText, CString &strResult, INPUTBOX_VALIDATE_PROC pValidateProc, void *pData)
{
	CInputBox dlg(NULL, lpszCaption, lpszPrompt, lpszInitText, pValidateProc, pData);
	if (dlg.DoModal() == IDOK)
	{
		strResult = dlg.m_strText;
		return TRUE;
	}
	return FALSE;
}

BOOL InputBox(CWnd *pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt, LPCTSTR lpszInitText, LPTSTR lpszResult, UINT nBufferSize, INPUTBOX_VALIDATE_PROC pValidateProc, void *pData)
{
	CInputBox dlg(pParent, lpszCaption, lpszPrompt, lpszInitText, pValidateProc, pData);
	if (dlg.DoModal() == IDOK)
	{
		_tcsncpy(lpszResult, dlg.m_strText, nBufferSize);
		lpszResult[nBufferSize - 1] = '\0';
		return TRUE;
	}
	return FALSE;
}
