#pragma once

#include <list>
class CDeferWindowPos
{
public:
	CDeferWindowPos(void);
	virtual ~CDeferWindowPos(void);

	void BeginDeferWindowPos();
	void DeferWindowPos(HWND hWnd, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags);
	void DeferWindowPos(HWND hWnd, HWND hWndInsertAfter, const CRect &rc, UINT uFlags);
	BOOL EndDeferWindowPos();

private:
	struct DEFER_WND_POS_INFO
	{
		HWND hWnd;
		HWND hWndInsertAfter;
		int x;
		int y;
		int cx;
		int cy;
		UINT uFlags;
	};
	typedef std::list<DEFER_WND_POS_INFO> INFO_LIST;
	INFO_LIST m_list;
	bool m_bInit;
};
