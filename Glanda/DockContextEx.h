#pragma once
#include "afxpriv.h"

class CDockContextEx :
	public CDockContext
{
public:
    CDockContextEx(CControlBar* pBar);

public:
	virtual void StartDrag(CPoint pt);
	void Move(CPoint pt);
	BOOL Track();
	void OnKey(int nChar, BOOL bDown);
	void UpdateState(BOOL* pFlag, BOOL bNewValue);
	DWORD CanDock();
	CDockBar* GetDockBar(DWORD dwOverDockStyle);
	DWORD CanDock(DWORD dwDockStyle, CDockBar** ppDockBar = NULL);
	void EndDrag();
};
