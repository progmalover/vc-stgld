#include "StdAfx.h"
#include ".\deferwindowpos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDeferWindowPos::CDeferWindowPos(void)
{
}

CDeferWindowPos::~CDeferWindowPos(void)
{
}

void CDeferWindowPos::BeginDeferWindowPos()
{
	void(0);
}

void CDeferWindowPos::DeferWindowPos(HWND hWnd, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags)
{
	DEFER_WND_POS_INFO info;
	info.hWnd = hWnd;
	info.hWndInsertAfter = hWndInsertAfter;
	info.x= x;
	info.y = y;
	info.cx = cx;
	info.cy = cy;
	info.uFlags = uFlags;
	m_list.push_back(info);
}

void CDeferWindowPos::DeferWindowPos(HWND hWnd, HWND hWndInsertAfter, const CRect &rc, UINT uFlags)
{
	this->DeferWindowPos(hWnd, hWndInsertAfter, rc.left, rc.top, rc.Width(), rc.Height(), uFlags);
}

BOOL CDeferWindowPos::EndDeferWindowPos()
{
	if (m_list.size() > 0)
	{
		HDWP hDWP = ::BeginDeferWindowPos((int)m_list.size());
		if (hDWP)
		{
			for (INFO_LIST::iterator it = m_list.begin(); it != m_list.end(); it++)
			{
				DEFER_WND_POS_INFO info = *it;
				if (!::DeferWindowPos(hDWP, info.hWnd, info.hWndInsertAfter, info.x, info.y, info.cx, info.cy, info.uFlags))
					return FALSE;
			}
			return ::EndDeferWindowPos(hDWP);
		}
	}
	return FALSE;
}
