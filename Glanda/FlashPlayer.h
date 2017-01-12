#if !defined(AFX_PLAYER_H__D36EBB4E_FA82_4EB3_881C_7EFCB63C9AAD__INCLUDED_)
#define AFX_PLAYER_H__D36EBB4E_FA82_4EB3_881C_7EFCB63C9AAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Player.h : header file
//
#include <map>
using namespace std;

#include "ShockwaveFlash.h"

typedef map<HWND, WNDPROC> MapHwndToProc;

/////////////////////////////////////////////////////////////////////////////
// CFlashPlayer window

class CFlashPlayer : public CShockwaveFlash
{
// Construction
public:
	CFlashPlayer();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlashPlayer)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetMovie(LPCTSTR lpszNewValue);
	int GetZoom();
	void ShowAll();
	void Zoomout();
	void Zoomin();
	BOOL Subclass();
	virtual ~CFlashPlayer();

	// Generated message map functions
protected:
	static BOOL m_bTracking;
	int m_zoom;
	static MapHwndToProc m_mapHandle;
	static LRESULT CALLBACK PlayerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void ShowContextMenu(int x, int y);
	//{{AFX_MSG(CFlashPlayer)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
public:
	afx_msg UINT OnGetDlgCode();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYER_H__D36EBB4E_FA82_4EB3_881C_7EFCB63C9AAD__INCLUDED_)
