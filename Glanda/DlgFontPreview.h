#pragma once
#include "statictext.h"


// CDlgFontPreview dialog

class CDlgFontPreview : public CDialog
{
	DECLARE_DYNAMIC(CDlgFontPreview)

public:
	CDlgFontPreview(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFontPreview();

// Dialog Data
	enum { IDD = IDD_FONT_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStaticText m_stcFont;
	void Preview(int x, int y, LPCTSTR lpszFont, int nSize, BOOL bBold, BOOL bItalic);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
