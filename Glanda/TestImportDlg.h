// TestImportDlg.h : header file
//

#pragma once

// CDlgTestImport dialog
class CDlgTestImport : public CDialog
{
// Construction
public:
	CDlgTestImport(CWnd* pParent = NULL);	// standard constructor
	~CDlgTestImport();

	DECLARE_SINGLETON(CDlgTestImport)

// Dialog Data
	enum { IDD = IDD_TESTIMPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	CCheckListBox m_list;
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonBrowseSwfPath();

public:
	int m_nChecked;
	int m_nImportFailed;
	int m_nExportFailed;

	BOOL m_bRunning;
	BOOL m_bStop;
	void RunTest();
	void AddFiles(LPCTSTR lpszPath);

public:
	CString m_strPath;
	BOOL m_bIncludeSubFolders;
	BOOL m_bTestChangeFrame;
	BOOL m_bTestInplaceEdit;
	afx_msg void OnBnClickedButtonSelectall();
	afx_msg void OnBnClickedButtonDeselectall();
	int m_nLoop;
};
