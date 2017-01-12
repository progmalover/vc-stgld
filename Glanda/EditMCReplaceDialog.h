#pragma once
#include "ResizableDialog.h"
#include "ShellTree.h"
#include "ResourceThumbListCtrl.h"
#include "HSplitter.h"
#include "ResourceExplorerWnd.h"


// CEditMCReplaceDialog dialog

class CEditMCReplaceDialog : public CResizableDialog, public CResourceThumbItemAction
{
//	DECLARE_DYNAMIC(CEditMCReplaceDialog)

public:
	CEditMCReplaceDialog(int nResourceType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditMCReplaceDialog();

// Dialog Data
	enum { IDD = IDD_EDIT_MC_REPLACE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_strProfileSection;
	CResourceThumbItem* m_pCurSelResource;
	CResourceExplorerWnd m_ExplorerWnd;
	int m_nResourceType;
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void SetProfileSection(LPCTSTR lpszSection)
	{
		m_strProfileSection = lpszSection;
		m_ExplorerWnd.SetProfileSection(lpszSection);
	}
	CResourceThumbItem* GetCurSelResource()
	{
		return m_pCurSelResource;
	}
	afx_msg void OnDestroy();
	virtual void OnResourceThumbAction(int nIndex, CResourceThumbListCtrl* pThumbListCtrl);
protected:
	virtual void OnOK();
};