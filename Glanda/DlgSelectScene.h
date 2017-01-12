#pragma once


// CDlgSelectScene dialog

class gldScene2;

class CStaticScene : public CStatic
{
	DECLARE_DYNAMIC(CStaticScene)

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnNcPaint();

public:
	CStaticScene();

	gldScene2 *m_pScene;
};

class CDlgSelectScene : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectScene)

public:
	CDlgSelectScene(CWnd* pParent = NULL, const CString &strCaption = "");   // standard constructor
	virtual ~CDlgSelectScene();

// Dialog Data
	enum { IDD = IDD_SELECT_SCENE };

	CListBox m_lstScene;
	CStaticScene m_sttScene;
	CString m_strCaption;

	inline gldScene2 *GetSelScene() const { return m_sttScene.m_pScene; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	
	afx_msg void OnLbnSelchangeListScene();
};
