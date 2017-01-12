// SceneNameEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "SceneNameEdit.h"
#include ".\scenenameedit.h"


// CSceneNameEdit

CSceneNameEdit::CSceneNameEdit()
{
	m_nMaxWidth = 0;
}

CSceneNameEdit::~CSceneNameEdit()
{
}


BEGIN_MESSAGE_MAP(CSceneNameEdit, CEditEx)
END_MESSAGE_MAP()



// CSceneNameEdit message handlers

void CSceneNameEdit::SetMaxWidth(int nMaxWidth)
{
	m_nMaxWidth = nMaxWidth;
}

void CSceneNameEdit::RecalcWindowWidth()
{
	ASSERT((GetStyle() | WS_BORDER) != 0);

	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);

	CString str;
	GetWindowText(str);

	CClientDC dc(this);

	CFont *pOldFont = dc.SelectObject(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(pOldFont);

	// Add margins + borders
	DWORD dwMargins = GetMargins();
	size.cx += 4 /*borders */ + LOWORD(dwMargins) + HIWORD(dwMargins) + 2 /*to show caret better*/;
	if (size.cx > m_nMaxWidth)
		size.cx = m_nMaxWidth;

	CPoint point = rc.CenterPoint();
	rc.left = point.x - size.cx / 2;
	rc.right = point.x + size.cx / 2;

	SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
}
