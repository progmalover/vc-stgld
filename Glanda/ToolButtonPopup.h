#pragma once

#include <vector>

class CSubTool
{
public:
	CSubTool(UINT nID, UINT nIDBitmap, LPCTSTR lpszText)
	{
		m_nID = nID;
		m_nIDBitmap = nIDBitmap;
		m_strText = lpszText;
	}

	UINT m_nID;
	UINT m_nIDBitmap;
	CString m_strText;
};

typedef std::vector <CSubTool> TOOL_LIST;

// CToolButtonPopup frame

class CToolButtonPopup : public CWnd
{
	DECLARE_DYNCREATE(CToolButtonPopup)
public:
	CToolButtonPopup();           // protected constructor used by dynamic creation
	virtual ~CToolButtonPopup();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	int m_nCurrent;
	int m_nHover;
	BOOL m_bLButtonDown;

	int m_nMaxWidth;
	int GetItemHeight();
	void GetItemRect(int index, CRect &rc);
	void RecalcWindowSize(CSize &size);
	int HitTest(CPoint point, BOOL &bSep);

public:
	TOOL_LIST m_list;

	void SetCurrentTool(int index);
	int FindTool(UINT nID);
	int GetCurrentTool();
	void GetItemText(int index, CString &strText);

	void SetHover(int index);
	void AddTool(UINT nID, UINT nIDBitmap, LPCTSTR lpszText);
	void RemoveAll();
	afx_msg void OnPaint();

	BOOL Create();
	BOOL Track(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
