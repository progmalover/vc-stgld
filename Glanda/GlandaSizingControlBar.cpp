// GlandaSizingControlBar.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "GlandaSizingControlBar.h"
#include "Graphics.h"

#define TITLE_HEIGHT 13

// CGlandaSizingControlBar

IMPLEMENT_DYNAMIC(CGlandaSizingControlBar, CSizingControlBarEx)
CGlandaSizingControlBar::CGlandaSizingControlBar()
{
	m_bShowTitle = TRUE;
}

CGlandaSizingControlBar::~CGlandaSizingControlBar()
{
}


BEGIN_MESSAGE_MAP(CGlandaSizingControlBar, CSizingControlBarEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CGlandaSizingControlBar message handlers

void CGlandaSizingControlBar::GetTitleRect(CRect *pRect)
{
	if (!HasTitle())
	{
		pRect->SetRectEmpty();
	}
	else
	{
		GetClientRectExcludeBorder(pRect);

		if (IsHorzDocked())
			pRect->right = pRect->left + TITLE_HEIGHT;
		else			
			pRect->bottom = pRect->top + TITLE_HEIGHT;
	}
}

void CGlandaSizingControlBar::PaintTitle(CDC &dc)
{
	CRect rcTitle;
	GetTitleRect(&rcTitle);

	//if (m_bActive)
	//	dc.FillSolidRect(&rcTitle, ::GetSysColor(COLOR_3DSHADOW));

	//dc.Draw3dRect(&rcTitle, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));

	//int nBkModeOld = dc.SetBkMode(TRANSPARENT);
	
	//::DrawGradientRect(&dc, rcTitle, 
	//	::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DSHADOW), IsHorzDocked());

	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
	CPen *pOldPen = dc.SelectObject(&pen);

	CString strText;
	GetWindowText(strText);

	//int nBkModeOld = dc.SetBkMode(TRANSPARENT);
	//int nTextColorOld = dc.SetTextColor(::GetSysColor(m_bActive ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
	
	if (IsHorzDocked())
	{
		int x = (rcTitle.right - rcTitle.left) / 2;

		for (int i = rcTitle.bottom - 2; i >= rcTitle.top + 14; i -= 2)
		{
			dc.MoveTo(x - 1, i);
			dc.LineTo(x + 2, i);
		}
	}
	else
	{
		/////////////////////////////////////////////
		// style 1
		int y = (rcTitle.bottom - rcTitle.top) / 2;
		CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = dc.SelectObject(&pen);
		for (int i = rcTitle.left + 2; i <= rcTitle.right - 18; i += 2)
		{
			dc.MoveTo(i, y - 1);
			dc.LineTo(i, y + 2);
		}

		//////////////////////////////////////////
		// style 2
		//int x = rcTitle.left + 3;
		//for (int i = rcTitle.top + 2; i <= rcTitle.bottom - 2; i += 2)
		//{
		//	dc.MoveTo(x - 1, i);
		//	dc.LineTo(x + 2, i);
		//}

		//CRect rcText = rcTitle;
		//rcText.left += 8;
		//rcText.right -= 16;

		//CFont *pOldFont = dc.SelectObject(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
		//dc.DrawText(strText, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
		//dc.SelectObject(pOldFont);

		////////////////////////////////////////////////////
		// style 3
		//CBrush brDark(::GetSysColor(COLOR_3DDKSHADOW));
		//CBrush brLight(::GetSysColor(COLOR_3DHILIGHT));

		//CPoint point = rcTitle.CenterPoint();
		//CRect rc(point.x - 1, point.y - 1, point.x + 1, point.y + 1);
		//for (int i = 2; ; i += 4)
		//{
		//	// left
		//	CRect rcTemp = rc;
		//	rcTemp.left -= i;
		//	rcTemp.right -= i;
		//	if (rcTemp.left - 4 <= rcTitle.left)
		//		break;
		//	dc.FillRect(&rcTemp, &brDark);

		//	rcTemp.OffsetRect(1, 1);
		//	dc.FillRect(&rcTemp, &brLight);

		//	// right
		//	rcTemp = rc;
		//	rcTemp.left += i;
		//	rcTemp.right += i;
		//	if (rcTemp.right + 24 >= rcTitle.right)
		//		continue;
		//	dc.FillRect(&rcTemp, &brDark);

		//	rcTemp.OffsetRect(1, 1);
		//	dc.FillRect(&rcTemp, &brLight);
		//}
	}

	dc.SelectObject(pOldPen);

	//dc.SetBkMode(nBkModeOld);
	//dc.SetTextColor(nTextColorOld);
}

void CGlandaSizingControlBar::RecalcLayout()
{
	if (::IsWindow(m_hWnd))
	{
		const int MARGIN = 3;
		if (HasTitle())
		{
			CRect rc;
			GetTitleRect(&rc);

			int w = m_btnClose.GetWidth();
			int h = m_btnClose.GetHeight();
			if (IsHorzDocked())
			{
				m_btnClose.MoveWindow(rc.left, rc.top, w, h);
			}
			else
			{
				m_btnClose.MoveWindow(rc.right - (w + MARGIN), rc.top, w, h);
			}
		}

		m_btnClose.ShowWindow(HasTitle() && m_bShowCloseButton ? SW_SHOW : SW_HIDE);

		Invalidate();
	}
}

int CGlandaSizingControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBarEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_btnClose.ModifyFBStyle(0, FBS_NOXPSTYLE | FBS_TRANSPARENT | FBS_NOBORDER);

	return 0;
}

void CGlandaSizingControlBar::LoadCloseButtonBitmap(BOOL bActive)
{
	m_btnClose.LoadBitmap(IDB_BUTTON_CLOSE2, 3);
}
