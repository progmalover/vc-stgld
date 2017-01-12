#pragma once
#include "afxwin.h"

typedef bool (*INPUTBOX_VALIDATE_PROC)(void *pData, LPCTSTR lpszText) ;

// CInputBox dialog

class CInputBox : public CDialog
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CWnd* pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt, LPCTSTR lpszInitText, INPUTBOX_VALIDATE_PROC pValidateProc, void *pData);   // standard constructor
	CInputBox(CWnd* pParent, UINT nCaptionID, UINT nPromptID, LPCTSTR lpszInitText, INPUTBOX_VALIDATE_PROC pValidateProc, void *pData);
	virtual ~CInputBox();

// Dialog Data
	enum { IDD = IDD_INPUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CString m_strCaption;
	CString m_strPrompt;
	INPUTBOX_VALIDATE_PROC m_pfnValidateProc;
	void *m_pData;

protected:
	virtual void OnOK();
public:
	CString m_strText;
	virtual BOOL OnInitDialog();
	CStatic m_stcPrompt;
	CEdit m_edtText;
};

BOOL InputBox(LPCTSTR lpszCaption, LPCTSTR lpszPrompt, LPCTSTR lpszInitText, CString &strResult, INPUTBOX_VALIDATE_PROC pValidateProc = NULL, void *pData = NULL);
BOOL InputBox(CWnd *pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt, LPCTSTR lpszInitText, LPTSTR lpszResult, UINT nBufferSize, INPUTBOX_VALIDATE_PROC pValidateProc = NULL, void *pData = NULL);
