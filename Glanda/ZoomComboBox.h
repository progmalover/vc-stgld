#if !defined(AFX_ZOOMCOMBOBOX_H__0C079AB3_71AE_11D5_9A21_0080C82BC2DE__INCLUDED_)
#define AFX_ZOOMCOMBOBOX_H__0C079AB3_71AE_11D5_9A21_0080C82BC2DE__INCLUDED_

#include "EditEx.h"
#include "Observer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZoomComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CZoomComboBox window

class CZoomComboBox : public CComboBox, public CObserver
{
// Construction
public:
	CZoomComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoomComboBox)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZoomComboBox();

	// Generated message map functions
protected:
	void OnKeyEscape();
	void OnKeyReturn();
	CEditEx m_wndEdit;
	CString m_strOldValue;
	//{{AFX_MSG(CZoomComboBox)
	afx_msg void OnSetFocus();
	//}}AFX_MSG
	afx_msg void OnSelEndOK();
	afx_msg void OnSelEndCancel();
	DECLARE_MESSAGE_MAP()

public:
	void SetZoom(int nPercent);

private:
	void Update(void *pData);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOOMCOMBOBOX_H__0C079AB3_71AE_11D5_9A21_0080C82BC2DE__INCLUDED_)
