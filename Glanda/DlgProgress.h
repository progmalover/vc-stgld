#pragma once
#include "afxcmn.h"

#include "Observer.h"

// CDlgProgress dialog

class CDlgProgress : public CDialog, public CObserver
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt);   // standard constructor
	virtual ~CDlgProgress();

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CString m_strCaption;
	CString m_strPrompt;

	CProgressCtrl m_progress;
	HANDLE m_hEvent;
	bool *m_pbCanceled;
public:
	void WaitForInitialize();
	void SetCancelFlagPtr(bool *pbCancel);
	bool IsCanceled() {return *m_pbCanceled;}
	void End(UINT nIDResult);
	void SetCaption(LPCTSTR lpszText);
	void SetPrompt(LPCTSTR lpszPrompt);
	void SetProgress(int nProgress);
protected:
	virtual void OnCancel();
	virtual void OnOK();
	LRESULT OnProgressMessage(WPARAM wp, LPARAM lp);
private:
	void Update(void *pData);
};
