#pragma once


// CImportImageDialog dialog

class CImportImageDialog : public CDialog
{
	DECLARE_DYNAMIC(CImportImageDialog)

public:
	CImportImageDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportImageDialog();

// Dialog Data
	enum { IDD = IDD_IMPORT_IMAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	BOOL	m_bShow;
	BOOL	m_bTiled;
	BOOL	m_bBackground;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedRadioNormal();
	afx_msg void OnBnClickedRadioBackground();
	void UpdateControls(BOOL bSave);
	virtual void OnOK();
};
