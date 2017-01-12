#if !defined(AFX_CUSTOMDRAWLISTCTRL_H__321C7451_6927_11D5_9A17_0080C82BC2DE__INCLUDED_)
#define AFX_CUSTOMDRAWLISTCTRL_H__321C7451_6927_11D5_9A17_0080C82BC2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomDrawListCtrl.h : header file
//

#include "afxtempl.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawListCtrl window

class CCustomDrawListCtrl : public CListCtrl
{
public:
	class CItemData
	{
	public:
		CItemData()
		{
			index = -1;
			pBmp = NULL;
			dwUserData = 0;
		}
		~CItemData()
		{
			if (pBmp)
				delete pBmp;
		}
		int index;
		CString strText;
		CBitmap *pBmp;
		DWORD_PTR dwUserData;
	};

// Construction
public:
	CCustomDrawListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomDrawListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetHover();
	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem) const;
	DWORD_PTR GetUserData(int nItem);
	BOOL SetUserData(int nItem, DWORD_PTR dwData);
	void SetItemText(int index, LPCTSTR lpszText);
	CString GetItemText(int index);
	void RegisterDropTarget(HWND hWnd);
	virtual BOOL Load();
	void SetItemSize(const CSize &size);
	void ClearCache(int nItem);
	void RedrawItem(int nItem, BOOL bCreateCache);
	virtual ~CCustomDrawListCtrl();

	// Generated message map functions
protected:
	BOOL m_bShowLabel;
	virtual BOOL CanDrag(int index);
	virtual void OnDrop(const CPoint &point);
	BOOL PtInDropTarget(const CPoint point);
	CPoint m_ptDrag;
	HWND m_hDropTarget;
	void DrawDragRect(const CPoint &point);
	BOOL m_bDragging;
	int m_nDragItem;
	int InsertItem(int index, LPCTSTR lpszText, DWORD dwData);
	virtual BOOL LoadAllItems() = 0;
	virtual void DrawImage(CDC *pDC, int nItem, const CRect &rc) = 0;
	CSize m_sizeItem;
	CImageList m_imgList;
	//{{AFX_MSG(CCustomDrawListCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	int m_nHover;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMDRAWLISTCTRL_H__321C7451_6927_11D5_9A17_0080C82BC2DE__INCLUDED_)
