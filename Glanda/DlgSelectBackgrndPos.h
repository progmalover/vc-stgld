#pragma once


// CDlgSelectBackgrndPos dialog

class CDlgSelectBackgrndPos : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectBackgrndPos)

public:
	CDlgSelectBackgrndPos(int selected = 0, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectBackgrndPos();

// Dialog Data
	enum { IDD = IDD_SELECT_BACKGROUND_POS };

	inline int GetSelected() const { return m_selected; }

protected:
	CComboBox m_cmbPos;
	int m_selected;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
