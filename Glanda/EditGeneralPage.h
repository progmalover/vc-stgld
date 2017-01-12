#pragma once
#include "SmartEdit.h"
#include "Command.h"


class gldInstance;
// CEditGeneralPage dialog

class CEditGeneralPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditGeneralPage)

public:
	CEditGeneralPage(gldInstance* pInstance);   // standard constructor
	virtual ~CEditGeneralPage();

// Dialog Data
	enum { IDD = IDD_EDIT_GENERAL_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnEditInstanceName();
	gldInstance* m_pInstance;
	CSmartEdit m_editInstanceName;

	BOOL IsValidInstanceName(const CString& strName);

	DECLARE_MESSAGE_MAP()
public:
	TCommandGroup* m_pCmd;
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnBnClickedCheckMask();
	afx_msg void OnBnClickedButtonEditAction();
};
