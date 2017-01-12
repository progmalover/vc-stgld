#pragma once

#include "PropertyPageFixed.h"
#include "historycombobox.h"

// CPagePublishSelectHTML dialog

class CPagePublishSelectHTML : public CPropertyPageFixed
{
	DECLARE_DYNAMIC(CPagePublishSelectHTML)

public:
	CPagePublishSelectHTML();
	virtual ~CPagePublishSelectHTML();

// Dialog Data
	enum { IDD = IDD_PUBLISH_SELECT_HTML };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowse();
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	CHistoryComboBox m_cmbFile;
	CString m_strFile;
	CString m_strFileContent;
	UINT m_nFormat;
};
