#pragma once

#include <vector>

// CSlidingPanelGroup
class CSlidingPanel;

class CSlidingPanelGroup : public CWnd
{

	DECLARE_DYNAMIC(CSlidingPanelGroup)
protected:
	enum {PANEL_SPACING = 1};

	std::vector<CSlidingPanel*> m_Panels;
	int m_nActivePanel;
	CImageList *m_pImageList;
public:
	CSlidingPanelGroup();
	virtual ~CSlidingPanelGroup();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void RecalcLayout();
	void GetPanelRect(CRect& rect, int nPanel);

	void AddPanel(CSlidingPanel* pPanel);
	CSlidingPanel* GetPanel(int nPanel);
	void SetActivePanel(int nActivePanel);
	void SetActivePanel(CSlidingPanel* pPanel);
	int GetActivePanel()	{return m_nActivePanel;}

	void LoadActivePanel(LPCTSTR lpszSection);
	void SaveActivePanel(LPCTSTR lpszSection);

	static int GetMinHeight(int nSize);

	CImageList *SetImageList(CImageList *pImageList);
	CImageList *GetImageList();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


