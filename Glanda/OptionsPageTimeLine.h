#pragma once


// COptionsPageTimeLine dialog

class COptionsPageTimeLine : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPageTimeLine)

public:
	COptionsPageTimeLine();
	virtual ~COptionsPageTimeLine();

// Dialog Data
	enum { IDD = IDD_OPTIONS_TIMELINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
