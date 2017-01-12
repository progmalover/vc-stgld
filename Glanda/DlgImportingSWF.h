#pragma once


// CDlgImportingSWF dialog

class CDlgImportingSWF : public CDialog
{
	DECLARE_DYNAMIC(CDlgImportingSWF)

public:
	CDlgImportingSWF(const CString &swfFileName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImportingSWF();

// Dialog Data
	enum { IDD = IDD_IMPORTING_SWF };

	inline CString GetExportPath() const { return m_sPath; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_sPath;
	BOOL m_bCanceled;
	CString m_sSWFPath;

	static BOOL WINAPI ImportSWFCallback(int rate, LPARAM lParam);

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
	virtual void OnCancel();

	LRESULT OnProgressMessage(WPARAM wParam, LPARAM lParam);
};
