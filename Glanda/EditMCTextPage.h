#pragma once
#include <list>
#include "StaticShapePreview.h"
#include "Observer.h"

class gldCharacterKey;
class gldObj;
// CEditMCTextPage dialog

class CEditMCTextPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEditMCTextPage)

public:
	TCommandGroup* m_pCmd;
	CEditMCTextPage();
	virtual ~CEditMCTextPage();

	DECLARE_GLANDA_OBSERVER(CEditMCTextPage, ModifyCurrentScene2)

// Dialog Data
	enum { IDD = IDD_EDIT_MC_TEXT_PAGE };

protected:
	void HandleSelChange();
	void HandleTextModified();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CStaticShapePreview m_stcTextPreview;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeTextList();
	afx_msg void OnBnClickedBtnEditText();
	afx_msg void OnDestroy();
	void OnButtonTextModify();
};
