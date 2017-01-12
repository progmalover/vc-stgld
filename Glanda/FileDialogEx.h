#pragma once


// CFileDialogEx

class CFileDialogEx : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDialogEx)

public:
	CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	CFileDialogEx(LPCTSTR lpszSection, BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR lpszFileName = NULL, CWnd* pParentWnd = NULL);
	virtual ~CFileDialogEx();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CString m_strSection;
	CString m_strPath;
protected:
	virtual BOOL OnFileNameOK();
};


