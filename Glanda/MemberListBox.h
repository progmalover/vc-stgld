// MemberListBox.h : header file
//

#pragma once

// This class handles an owner-draw list box.  The list box in question
// handles color bars instead of text.  It shows all of the required
// features for an owner-draw list box. See the on-line help for CListBox 
// for other ways you can manipulate the contents of an owner-draw list
// box.
//
// You can actually handle just about anything you want in an owner-draw 
// list box as long as you override functions CompareItem, DrawItem, and 
// MeasureItem.  Strings are easiest and don't require all the overrides.  
// Other examples: bitmaps, text in varying fonts, text in a non-standard 
// font, etc.
//
// TODO:
//	1.	Create an owner-draw list box in your resources to use with
//		this class.  You will need to have the following features set
//		on the Styles page for the list box:
//		- Owner-Draw: Variable
//		- Has Strings: Unchecked
//		- Sort: Checked (to show off owner-draw sorting)
//
//	2.	Subclass your owner-draw list box to associate it with this
//		class.  The following statement placed in OnInitDialog() for 
//		the dialog you used this list box in should work fine:
//			m_myDraw.SubclassDlgItem(IDC_MYDRAW, this);
//		where:
//			m_myDraw is an instance of the owner-draw list box class
//			IDC_MYDRAW is the ID associated with your list box resource
//
//	3.	Call m_myDraw.LoadList() to initialize the contents of the owner-draw
//		list box.  Do this immediately after you subclass the list box
//		control.
//
//	4.	The list box implemented here is single selection for simplicity.  
//		If you want	something different (like multiple or extended selection),
//		you will need to do the following:
//		- change the Selections type on the Styles page for the list box 
//			appropriately
//		- use the multiple-selection operations functions provided for 
//			CListBox to manipulate the selections (see the on-line help 
//			for CListBox for more details on these functions)

#include "TrayWnd.h"
#include "Utils.h"

extern UINT WM_MEMBERLISTBOX;

#define LBM_SELCHANGE			1
#define LBM_SELCANCEL			2

/////////////////////////////////////////////////////////////////////////////
// CMemberListBox window

class CMemberListBox : public CListBox
{
// Construction
public:
	CMemberListBox();

	DECLARE_SINGLETON(CMemberListBox)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemberListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void CenterItem(int index);
	int FindStringEx(LPCTSTR lpszText);
	static HHOOK m_hHook;
	LRESULT SendOwnerMessage(WPARAM wParam);
	int SetCurSelEx(int nSel);
	void EndTrackLButton();
	int SetItemHeight(int nHeight);
	int GetMaxItemWidth();
	int AddItem(LPCTSTR lpszItem, int nImage);
	virtual ~CMemberListBox();
	BOOL Create();
	void RelayEvent(MSG *pMsg);
	CTrayWnd &GetTrayWnd() {return m_wndTray;}

	// Generated message map functions
protected:
	CTrayWnd m_wndTray;
	BOOL m_bListBoxSmoothScrolling;
	void EnsureVisible(int index);
	int m_nCurFocus;
	void RedrawItem(int index, BOOL bImage);
	int SetCurFocus(int nCurFocus);
	CImageList m_imgList;
	static CMemberListBox * m_pListBox;
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wp, LPARAM lp);
	BOOL PtInWindow(CPoint &point);
	BOOL m_bTrackingLButton;
	void StartTrackLButton();
	int m_nItemHeight;
	BOOL m_bUseImage;
	//{{AFX_MSG(CMemberListBox)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
