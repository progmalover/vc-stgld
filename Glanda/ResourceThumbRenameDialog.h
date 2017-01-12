#pragma once
class CResourceThumbItem;

// CResourceThumbRenameDialog dialog

class CResourceThumbRenameDialog : public CDialog
{
	DECLARE_DYNAMIC(CResourceThumbRenameDialog)

	CResourceThumbItem* m_pThumbItem;
public:
	CResourceThumbRenameDialog(CResourceThumbItem* pThumbItem, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResourceThumbRenameDialog();

// Dialog Data
	enum { IDD = IDD_RESOURCE_THUMB_RENAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
