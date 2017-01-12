#pragma once


// CPropertyPageFixed dialog

class CPropertyPageFixed : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyPageFixed)

public:
	CPropertyPageFixed(UINT nIDD, UINT nIDCaption = 0);
	virtual ~CPropertyPageFixed();

// Dialog Data
	enum { IDD = 0 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void Construct(UINT nIDTemplate, UINT nIDCaption, UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);

	DECLARE_MESSAGE_MAP()
};
