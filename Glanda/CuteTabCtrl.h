/************************************
  REVISION LOG ENTRY
  Revision By: Mihai Filimon
  Revised on 2/1/99 4:16:26 PM
  Comments: // CuteTabCtrl.h : header file
 ************************************/

#if !defined(AFX_CUTETABCTRL_H__7A55DF62_B8FC_11D2_8766_0040055C08D9__INCLUDED_)
#define AFX_CUTETABCTRL_H__7A55DF62_B8FC_11D2_8766_0040055C08D9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CuteTabCtrl.h : header file
//

#include <afxtempl.h>
#include "ScrollButton.h"
#include "atltypes.h"

/////////////////////////////////////////////////////////////////////////////
// CCuteTabCtrl window
#define TCXS_UP					1
#define TCXS_DOWN				2
#define TCXS_MASK				3

// Extended styles added by Chen hao
#define TCXS_EX_AUTOSIZE		0x00000001L << 0
#define TCXS_EX_SCROLLABLE		0x00000001L << 1
#define TCXS_EX_VID6			0x00000001L << 2
#define TCXS_EX_TRAPEZIA		0x00000001L << 3
#define TCXS_EX_TRAPEZIA2		0x00000001L << 4
#define TCXS_EX_PHOTOSHOP		0x00000001L << 5
#define TCXS_EX_FLAT			0x00000001L << 6
#define TCXS_EX_NOBORDER		0x00000001L << 7
#define TCXS_EX_BOLD_ACTIVE		0x00000001L << 16

class CTabItem
{
	friend class CCuteTabCtrl;
	public:
			LPCTSTR GetToolTip();
			void SetToolTip(LPCTSTR lpszToolTip);
			void SetTitle(CString sTitle);
			CString GetTitle();
			LPARAM m_lParam;
			virtual CRect GetItemRect();
			static CTabItem* NewItem(LPCTSTR lpszItemText, CCuteTabCtrl* pParentCtrl);
			CTabItem(LPCTSTR lpszItemText, CCuteTabCtrl* pParentCtrl);
			inline int GetWidth() { return m_nWidth; };
			inline int GetStart() { return m_nStart; };
			inline int GetEnd() {return m_nStart + m_nWidth; };
			HWND m_hWnd;
	protected:
			TCHAR m_szToolTip[80];
			virtual BOOL PtInItem(CPoint point);
			virtual void DrawText(CDC* pDC, BOOL bActive, BOOL bFocused);
			virtual int DrawIcon(CDC* pDC, BOOL bActive);
			virtual void DrawFrame(CDC * pDC, int nIndex, int nActive);
			inline void SetWidth(int nWidth) { m_nWidth = nWidth; };
			inline void SetStart(int nStart) { m_nStart = nStart; };
			virtual int GetIdealWidth(CDC* pDC);
			virtual void Draw(CDC* pDC, int nIndex, int nActive, BOOL bFocused);
		// Atributtes
			CCuteTabCtrl* m_pParentCtrl;
			HICON m_hIcon;
			int m_nWidth;
			int m_nStart;
			CString m_strTitle;
};

class CCuteTabCtrl : public CWnd
{
public:

		CArray<CTabItem*, CTabItem*> m_arItems;
// Construction
public:
		CCuteTabCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCuteTabCtrl)
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	int InsertItem(LPCTSTR lpszItemText, int index, LPARAM lParam, HICON hIcon);
		DWORD GetExtendedStyle();
		DWORD GetStyle();
		void SetToolTip(int index, LPCTSTR lpszToolTip);
		void SetExtendedStyle(DWORD dwStyle, BOOL bRepaint = FALSE);
		BOOL SetActive(int nActiveItem, BOOL bNotify);
		void InvalidateNew(BOOL bErase = TRUE);
		virtual CTabItem* Item(int nIndex);
		virtual int GetItemHeight();
		virtual int GetActiveItem();
		virtual int GetCount();
		virtual CSize ItemSize(int iIndex = -1);
		virtual BOOL DeleteItem(int nItem);
		virtual int AddItem(LPCTSTR lpszItemText, LPARAM lParam = NULL, HICON hIcon = NULL);

	    virtual BOOL Create(CWnd* pParentWnd, CString &pTitle, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP  );

		virtual ~CCuteTabCtrl();

	// Generated message map functions
protected:
	CToolTipCtrl m_tooltip;
	DWORD m_dwStyle;
	DWORD m_dwExStyle;

	virtual BOOL OnActivatePage(int nItemOld, int nItemNew);
	virtual void OnBeginDrag(CPoint point);
	HACCEL m_hAccel;
	virtual int ItemFromPoint(CPoint point);
	virtual void AutoSize(int nLastEnd);
	int m_nActiveItem;
	virtual void RecalcLayout();
	int m_nDY;
	CFont m_fntNormal;
	CFont m_fntActive;
	CScrollButton m_btnLeft;
	CScrollButton m_btnRight;
	int m_nScrollEnd;
	int m_nButtonWidth;
	int m_nButtonHeight;
	int m_nButtonGap;
	CRect m_rcLeft;
	CRect m_rcRight;
	int m_nScrollStart;
	BOOL m_bShowScrollButton;
	BOOL m_bEnableDragDrop;

	int m_nMarginLeft;
	int m_nMarginRight;
	int m_nMarginTop;

	int m_nPaddingLeft;
	int m_nPaddingRight;

	void PaintClient(CDC *pDC);
	void PaintBackground(CDC *pDC, CRect &rect);


	//{{AFX_MSG(CCuteTabCtrl)
		afx_msg void OnPaint();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnButtonLeft();
		afx_msg void OnButtonRight();
		afx_msg void OnSysColorChange();
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetItemText(int index, LPCTSTR lpszText);
	void SetPadding(int nPaddingLeft, int nPaddingRight);
	void SetMargin(int nMarginLeft, int nMarginRight, int nMarginTop);
	void EnableDragDrop(BOOL bEnable);
	BOOL IsDragDropEnabled();
	virtual HFONT GetFontHandle(void);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CPoint m_ptLButtonDown;

	friend class CTabItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTETABCTRL_H__7A55DF62_B8FC_11D2_8766_0040055C08D9__INCLUDED_)
