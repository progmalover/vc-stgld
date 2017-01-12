#pragma once
class CResourceThumbItem;


// CResourceThumbFilePropertiesDialog dialog

class CResourceThumbFilePropertiesDialog : public CPropertyPage
{
	DECLARE_DYNAMIC(CResourceThumbFilePropertiesDialog)

	CResourceThumbItem* m_pThumbItem;
	CToolTipCtrl m_tooltip;
public:
	CResourceThumbFilePropertiesDialog(CResourceThumbItem* pThumbItem, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResourceThumbFilePropertiesDialog();

// Dialog Data
	enum { IDD = IDD_RESOURCE_THUMB_FILE_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnApply();
public:
	afx_msg void OnBnClickedButtonComment();
};
