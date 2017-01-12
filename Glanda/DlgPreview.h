#pragma once

#include "ResizableDialog.h"
#include "flashplayer.h"

// CDlgPreview dialog

class CDlgPreview : public CResizableDialog
{
public:
	CDlgPreview(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPreview();

	DECLARE_SINGLETON(CDlgPreview)

// Dialog Data
	enum { IDD = IDD_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFile;
	CFlashPlayer m_player;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	HICON m_hIcon;
public:
	DECLARE_EVENTSINK_MAP()
	void OnReadyStateChangePlayer(long newState);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPreviewEscape();
	void FSCommandPlayer(LPCTSTR command, LPCTSTR args);
protected:
	virtual void OnCancel();
	BOOL m_bFullScreen;
	CRect m_rcNormal;
	BOOL m_bOutputShown;
	HACCEL m_hAccel;
	void ShowCaptionAndBorder(BOOL bShow);
	CPoint m_ptPreviewPos;
public:
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	void ResetWindowPos();
	void SaveWindowPos();
	void LoadWindowPos();
};
