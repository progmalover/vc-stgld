#pragma once

#include <list>
class CCategoryPage;

// CCategoryTabCtrl

class CCategoryTabCtrl : public CTabCtrl
{
	CFont m_fontNormal;
	int m_nActivePage;
	DECLARE_DYNAMIC(CCategoryTabCtrl)

public:
	CCategoryTabCtrl();
	virtual ~CCategoryTabCtrl();
	void AddPage(LPCTSTR lpszItem, CCategoryPage* pPage);
	void AddPage(UINT nIDItem, CCategoryPage* pPage);

protected:
	void RecalcLayout();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);

	void UpdateSelectionChange();
	void SetActivePage(int nActivePage);
	int GetActivePage(){return m_nActivePage;}
	CCategoryPage* GetPage(int nPage);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


