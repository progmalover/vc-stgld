#pragma once
class CResourceThumbItem;

// CResourceThumbObjPropertiesDialog dialog

class CResourceThumbObjPropertiesDialog : public CPropertyPage
{
	DECLARE_DYNAMIC(CResourceThumbObjPropertiesDialog)

	CResourceThumbItem* m_pThumbItem;
public:
	CResourceThumbObjPropertiesDialog(CResourceThumbItem* pThumbItem, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResourceThumbObjPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_RESOURCE_THUMB_OBJ_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
protected:
	virtual BOOL OnApply();
};
