/************************************
  REVISION LOG ENTRY
  Revision By: Mihai Filimon
  Revised on 2/1/99 4:16:16 PM
  Comments: CuteTabCtrl.cpp : implementation file
 ************************************/

#include "stdafx.h"
#include "CuteTabCtrl.h"
#include "MemDC.h"
#include "Graphics.h"
#include "Resource.h"

#include "VisualStylesXP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_BUTTON_LEFT		1
#define IDC_BUTTON_RIGHT	2

/////////////////////////////////////////////////////////////////////////////
// CCuteTabCtrl::CTabItem class 

#define SNDMSGNOTIFY(message) if (CWnd* pParentWnd = GetParent())\
								if ( ::IsWindow( pParentWnd->m_hWnd ) )\
								{\
									NMHDR nmHdr;\
									nmHdr.hwndFrom = m_hWnd;\
									nmHdr.idFrom = ::GetDlgCtrlID(m_hWnd);\
									nmHdr.code = message;\
									::SendMessage(pParentWnd->m_hWnd, WM_NOTIFY, MAKEWPARAM(nmHdr.idFrom, NULL), LPARAM(&nmHdr));\
								}
#define SNDMSGKEYDOWN(_wVKey, _flags) if (CWnd* pParentWnd = GetParent())\
								if ( ::IsWindow( pParentWnd->m_hWnd ) )\
								{\
									TC_KEYDOWN tcKeyDown;\
									tcKeyDown.hdr.hwndFrom = m_hWnd;\
									tcKeyDown.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);\
									tcKeyDown.hdr.code = TCN_KEYDOWN;\
									tcKeyDown.wVKey = _wVKey;\
									tcKeyDown.flags = _flags;\
									::SendMessage(pParentWnd->m_hWnd, WM_NOTIFY, MAKEWPARAM(tcKeyDown.hdr.idFrom, NULL), LPARAM(&tcKeyDown));\
								}
// Function name	: CTabItem::CTabItem
// Description	    : Default construcotr
// Return type		: 
CTabItem::CTabItem(LPCTSTR lpszItemText, CCuteTabCtrl* pParentCtrl):
	m_strTitle(lpszItemText), 
	m_pParentCtrl(pParentCtrl)
{
	m_hIcon = NULL;
	m_nWidth = NULL;
	m_lParam = NULL;
	m_hWnd = NULL ;
}

// Function name	: CTabItem::NewItem
// Description	    : Called to create one new Item . Uses by AddItem functiopn
// Return type		: CTabItem* 
// Argument         : LPCTSTR lpszItemText
CTabItem* CTabItem::NewItem(LPCTSTR lpszItemText, CCuteTabCtrl* pParentCtrl)
{
	return new CTabItem(lpszItemText, pParentCtrl);
}

// Function name	: CTabItem::GetIdealWidth
// Description	    : Calc the ideal width of items.
// Return type		: void 
// Argument         : CDC * pDC
int CTabItem::GetIdealWidth(CDC * pDC)
{
	CFont *pOldFont = pDC->SelectObject((m_pParentCtrl->m_dwExStyle & TCXS_EX_BOLD_ACTIVE) ? &m_pParentCtrl->m_fntActive : &m_pParentCtrl->m_fntNormal);
	int nResult = pDC->GetTextExtent(m_strTitle).cx + m_pParentCtrl->m_nPaddingLeft + m_pParentCtrl->m_nPaddingRight;
	pDC->SelectObject(pOldFont);
	if (m_hIcon)
		nResult += 16;
	nResult = max(m_pParentCtrl->m_nPaddingLeft + m_pParentCtrl->m_nPaddingRight, nResult);
	return nResult;
}

// Function name	: CTabItem::Draw
// Description	    : Invalidate this item into pDC
// Return type		: void 
// Argument         : CDC * pDC
void CTabItem::Draw(CDC* pDC, int nIndex, int nActive, BOOL bFocused)
{
	DrawFrame(pDC, nIndex, nActive);
	DrawText(pDC, nIndex == nActive, nIndex == nActive && bFocused);
}

// Function name	: CTabItem::DrawIcon
// Description	    : 
// Return type		: void 
// Argument         : CDC* pDC
int CTabItem::DrawIcon(CDC* pDC, BOOL bActive)
{
	ASSERT(m_hIcon);

	CRect rect(GetItemRect());
	rect.DeflateRect(m_pParentCtrl->m_nPaddingLeft, 0, m_pParentCtrl->m_nPaddingRight, 0);

	if (m_pParentCtrl->GetStyle() & TCXS_UP)
		rect.top += m_pParentCtrl->m_nMarginTop;
	else
		rect.bottom -= m_pParentCtrl->m_nMarginTop;

	// If the icon cover the area, draw it
	if (rect.Width() > 16)
	{
		CRect r(rect);
		r.right = r.left + 16;
		r.DeflateRect(CSize(0, (rect.Height() - 16)/2));
		CDC dcMem; CBitmap bmp;
		CSize sIcon(32,32);
		if (dcMem.CreateCompatibleDC(pDC))
			if (bmp.CreateCompatibleBitmap(pDC, sIcon.cx, sIcon.cy))
			{
				CBitmap* pBmp = dcMem.SelectObject(&bmp);
				dcMem.FillRect(CRect(CPoint(0,0), sIcon), CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
				dcMem.DrawIcon(0,0,m_hIcon);
				pDC->StretchBlt(r.left,r.top, r.Width(), r.Height(), &dcMem, 0,0,sIcon.cx, sIcon.cy, SRCCOPY);
				dcMem.SelectObject(pBmp);
			}
		rect.left += r.Width() + m_pParentCtrl->m_nPaddingLeft / 2;
	}

	return rect.left;
}

// Function name	: CTabItem::DrawText
// Description	    : Draw the text here
// Return type		: void 
// Argument         : CDC* pDC
void CTabItem::DrawText(CDC* pDC, BOOL bActive, BOOL bFocused)
{
	CRect rect = GetItemRect();

	UINT nFormat = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
	nFormat |= (rect.Width() >= GetIdealWidth(pDC) ? DT_CENTER : DT_LEFT);

	rect.DeflateRect(m_pParentCtrl->m_nPaddingLeft, 0, m_pParentCtrl->m_nPaddingRight, 0);
	if (m_pParentCtrl->GetStyle() & TCXS_UP)
		rect.top += m_pParentCtrl->m_nMarginTop;
	else
		rect.bottom -= m_pParentCtrl->m_nMarginTop - 1;

	if (m_hIcon)
		rect.left = DrawIcon(pDC, bActive);
	pDC->DrawText(m_strTitle, &rect, nFormat);

	UNREFERENCED_PARAMETER(bFocused);
}

// Function name	: CTabItem::GetItemRect
// Description	    : Return the item rect
// Return type		: CRect 
CRect CTabItem::GetItemRect()
{
	ASSERT (m_pParentCtrl);
	CRect rect; m_pParentCtrl->GetClientRect(rect);

	int nHeight = m_pParentCtrl->GetItemHeight();
	if (m_pParentCtrl->GetStyle() & TCXS_UP)
		return CRect(m_nStart, rect.top, m_nStart + m_nWidth, rect.top + nHeight);
	else
		return CRect(m_nStart, rect.bottom - nHeight, m_nStart + m_nWidth, rect.bottom);
}

// Function name	: CTabItem::PtInItem
// Description	    : Return TRUE of point is in Item
// Return type		: BOOL 
// Argument         : CPoint point
BOOL CTabItem::PtInItem(CPoint point)
{
	return GetItemRect().PtInRect(point);
}

// Function name	: CTabItem::DrawFrame
// Description	    : Draw the frame.
// Return type		: void 
// Argument         : CDC * pDC
void CTabItem::DrawFrame(CDC * pDC, int nIndex, int nActive)
{
	CRect rect = GetItemRect();

	COLORREF crFace = ::GetSysColor(COLOR_3DFACE);
	COLORREF crHilight = ::GetSysColor(COLOR_3DHILIGHT);
	COLORREF crDkShadow = RGB(0, 0, 0);
	COLORREF crShadow = ::GetSysColor(COLOR_3DSHADOW);
	COLORREF crHighlightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

	CPen penFace(PS_SOLID, 1, crFace);
	CPen penHilight(PS_SOLID, 1, crHilight);
	CPen penDkShadow(PS_SOLID, 1, crDkShadow);
	CPen penLight(PS_SOLID, 1, ::GetSysColor(COLOR_3DLIGHT));
	CPen penShadow(PS_SOLID, 1, crShadow);
	CPen penHighlightText(PS_SOLID, 1, crHighlightText);
	CPen penWindow(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));

	CPen * pOldPen = NULL ;

	DWORD dwStyle = m_pParentCtrl->GetStyle();
	DWORD dwExStyle = m_pParentCtrl->GetExtendedStyle();

	BOOL bActive = (nIndex == nActive);

	if (dwStyle & TCXS_UP)	// TCXS_UP
	{
		if (dwExStyle & (TCXS_EX_VID6 | TCXS_EX_FLAT))
		{
			if (nIndex != nActive)
			{
				if (nIndex != nActive - 1)
				{
					// right
					pOldPen = pDC->SelectObject((dwExStyle & TCXS_EX_VID6) ? &penLight : &penHilight);
					pDC->MoveTo(rect.right, rect.bottom - 4);
					pDC->LineTo(rect.right, rect.top + 4);

					pDC->SelectObject(pOldPen);
				}
			}
			else
			{
				rect.top += m_pParentCtrl->m_nMarginTop;

				// background
				pDC->FillSolidRect(rect, crFace);
				DrawGradientRect(pDC, CRect(rect.left, rect.top, rect.right, rect.top + 8), Lighten(Lighten(::GetSysColor(COLOR_3DFACE))), ::GetSysColor(COLOR_3DFACE), FALSE);

				pDC->SelectObject(pOldPen);
			}
			if (m_pParentCtrl->m_bEnableDragDrop)
			{
				if (bActive)
				{
					pDC->SetPixel(rect.left + 3, rect.top + rect.Height() / 2 - 3, crHilight);
					pDC->SetPixel(rect.left + 3, rect.top + rect.Height() / 2, crHilight);
					pDC->SetPixel(rect.left + 3, rect.top + rect.Height() / 2 + 3, crHilight);

					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 - 2, crShadow);
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 + 1, crShadow);
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 + 4, crShadow);
				}
			}
		}
		else
		if (dwExStyle & TCXS_EX_PHOTOSHOP)
		{
			// fill background
			POINT pts1[] = 
			{
				{rect.left + 1, bActive ? rect.bottom + 1 : rect.bottom}, 
				{rect.left + 1, rect.top}, 
				{rect.right - rect.Height() / 2, rect.top}, 
				{rect.right + rect.Height() / 2 + (bActive? 2 : 0), rect.bottom + (bActive? 2 : 0)}
			};

			CRgn rgn;
			rgn.CreatePolygonRgn(pts1, 4, ALTERNATE);
			CBrush br;
			br.CreateSysColorBrush(COLOR_3DFACE);
			pDC->FillRgn(&rgn, &br);

			if (m_pParentCtrl->m_bEnableDragDrop)
			{
				if (bActive)
				{
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 - 3, crHilight);
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2, crHilight);
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 + 3, crHilight);
				}

				pDC->SetPixel(rect.left + 5, rect.top + rect.Height() / 2 - 2, crShadow);
				pDC->SetPixel(rect.left + 5, rect.top + rect.Height() / 2 + 1, crShadow);
				pDC->SetPixel(rect.left + 5, rect.top + rect.Height() / 2 + 4, crShadow);
			}

			// draw outer border
			POINT points[] = 
			{
				{rect.left, rect.bottom - 1}, 
				{rect.left, rect.top}, 
				{rect.right - rect.Height() / 2, rect.top}, 
				{rect.right + rect.Height() / 2, rect.bottom}
			};


			// left
			pOldPen = pDC->SelectObject(&penDkShadow);
			pDC->MoveTo(points[0]);
			pDC->LineTo(points[1]);

			// top
			pDC->LineTo(points[2]);

			// right
			pDC->LineTo(points[3]);

			pDC->SelectObject(pOldPen);

			// draw inner border
			if (bActive)
			{
				rect.left++;
				rect.top++;
				//rect.right--;
				rect.bottom++;

				POINT points[] = 
				{
					{rect.left, rect.bottom}, 
					{rect.left, rect.top}, 
					{rect.right - rect.Height() / 2, rect.top}, 
					{rect.right + rect.Height() / 2, rect.bottom}
				};
				
				// left
				pOldPen = pDC->SelectObject(&penHilight);
				pDC->MoveTo(points[0]);
				pDC->LineTo(points[1]);

				// top
				pDC->LineTo(points[2]);

				// right
				pDC->SelectObject(&penShadow);
				pDC->LineTo(points[3]);

				pDC->SelectObject(pOldPen);
			}
		}
		else
		if (m_pParentCtrl->m_dwExStyle & TCXS_EX_TRAPEZIA)
		{
			rect.top += m_pParentCtrl->m_nMarginTop;

			CRgn rgn;
			POINT points[6] = 
			{
				rect.left,	rect.bottom + 1,
				rect.left,	rect.top + 4,
				rect.left + 4,	rect.top,
				rect.right - 4,	rect.top,
				rect.right,	rect.top + 4,
				rect.right,	rect.bottom + 1,
			};

			if (bActive)
			{
				rgn.CreatePolygonRgn(points, sizeof(points) / sizeof(POINT), ALTERNATE);
				CBrush br;
				br.CreateSolidBrush(bActive ? crFace: ::Darken(crFace));
				pDC->FillRgn(&rgn, &br);
			}
			
			CPen *pOldPen = pDC->SelectObject(bActive ? &penDkShadow : &penShadow);

			points[0].y--;
			points[5].y--;
			pDC->MoveTo(points[0].x, points[0].y - 1);
			for (int i = 1; i < sizeof(points) / sizeof(POINT); i++)
				pDC->LineTo(points[i]);

			if (bActive || nIndex == nActive + 1)
			{
				pDC->SelectObject(bActive ? &penHilight : &penShadow);
			
				pDC->MoveTo(points[0].x + 1, points[0].y - 1);
				pDC->LineTo(points[1].x + 1, points[1].y);
				if (bActive)
				{
					pDC->LineTo(points[2].x, points[2].y + 1);
					pDC->LineTo(points[3].x - 1, points[2].y + 1);
				}
			}

			pDC->SelectObject(&pOldPen);
		}
		else
		if (m_pParentCtrl->m_dwExStyle & TCXS_EX_TRAPEZIA2)
		{
			rect.top += m_pParentCtrl->m_nMarginTop;

			const int nOverlap = 4;

			CRgn rgn;
			POINT points[] = 
			{
				rect.left, rect.bottom + 1,
				rect.left, rect.top, 
				rect.right + nOverlap - 5, rect.top,
				rect.right + nOverlap - 5 + 1, rect.top + 1,
				rect.right + nOverlap - 5 + 2, rect.top + 1,
				rect.right + nOverlap - 5 + 3, rect.top + 2,
				rect.right + nOverlap - 5 + 4, rect.top + 3,
				rect.right + nOverlap - 5 + 4, rect.top + 4,
				rect.right + nOverlap - 5 + 5, rect.top + 5,
				rect.right + nOverlap, rect.bottom + 1,
			};

			if (!bActive)
			{
				points[0].y -= 2;
				points[sizeof(points) / sizeof(POINT) - 1].y -= 2;
			}
			
			rgn.CreatePolygonRgn(points, sizeof(points) / sizeof(POINT), ALTERNATE);
			CBrush br;
			br.CreateSolidBrush(crFace);
			pDC->FillRgn(&rgn, &br);
			
			CPen *pOldPen = pDC->SelectObject(bActive ? &penDkShadow : &penShadow);

			if (bActive)
			{
				points[0].y--;
				points[sizeof(points) / sizeof(POINT) - 1].y--;
			}
			pDC->MoveTo(points[0].x, points[0].y - 1);
			for (int i = 1; i < sizeof(points) / sizeof(POINT); i++)
				pDC->LineTo(points[i]);

			if (bActive)
			{
				pDC->SelectObject(&penHilight);
				pDC->MoveTo(points[0].x + 1, points[0].y - 1);
				pDC->LineTo(points[1].x + 1, points[1].y + 1);
				pDC->LineTo(points[2].x + 1, points[2].y + 1);

				//pDC->SelectObject(&penShadow);
				//pDC->MoveTo(points[3].x + 1, points[3].y + 1);
				//pDC->LineTo(points[4].x + 1, points[4].y - 1);
			}

			pDC->SelectObject(&pOldPen);
		}
		else
		{
			if (nIndex != nActive)
				rect.top++;
			else
				rect.right++;

			// left
			pOldPen = pDC->SelectObject(&penHilight);
			pDC->MoveTo(rect.left, rect.bottom - 2);
			pDC->LineTo(rect.left, rect.top + 2);

			//topleft
			pDC->LineTo(rect.left + 2, rect.top);

			// top
			pDC->LineTo(rect.right - 2, rect.top);

			//topright
			pDC->SelectObject(&penDkShadow);
			pDC->MoveTo(rect.right - 2, rect.top + 1);
			pDC->LineTo(rect.right - 1, rect.top + 2);

			// right
			pDC->LineTo(rect.right - 1, rect.bottom - 2);

			if ((bActive))
			{
				pDC->SelectObject(&penFace);
				pDC->MoveTo(rect.right - 1, rect.bottom - 1);
				pDC->LineTo(rect.left, rect.bottom - 1);
			}

			pDC->SelectObject(pOldPen);
		}
	}
	else	// TCXS_DOWN
	{
		if (dwExStyle & (TCXS_EX_VID6 | TCXS_EX_FLAT))
		{
			if (nIndex != nActive)
			{
				if (nIndex != nActive - 1)
				{
					// right
					pOldPen = pDC->SelectObject((dwExStyle & TCXS_EX_VID6) ? &penLight : &penHighlightText);
					pDC->MoveTo(rect.right, rect.bottom - 4);
					pDC->LineTo(rect.right, rect.top + 4);

					pDC->SelectObject(pOldPen);
				}
			}
			else
			{
				rect.bottom -= m_pParentCtrl->m_nMarginTop;

				// background
				pDC->FillSolidRect(rect, crFace);

				pDC->SelectObject(pOldPen);
			}
			if (m_pParentCtrl->m_bEnableDragDrop)
			{
				if (bActive)
				{
					pDC->SetPixel(rect.left + 3, rect.top + rect.Height() / 2 - 3, crHilight);
					pDC->SetPixel(rect.left + 3, rect.top + rect.Height() / 2, crHilight);
					pDC->SetPixel(rect.left + 3, rect.top + rect.Height() / 2 + 3, crHilight);

					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 - 2, crShadow);
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 + 1, crShadow);
					pDC->SetPixel(rect.left + 4, rect.top + rect.Height() / 2 + 4, crShadow);
				}
			}
		}
		else
		if (dwExStyle & TCXS_EX_TRAPEZIA) // trapeziform
		{
			CRgn rgn;
			POINT points[4];
			points[0].x = rect.left - 4;	points[0].y = rect.top + 1;
			points[1].x = rect.left + 4;	points[1].y = rect.bottom - 1;
			points[2].x = rect.right - 4;	points[2].y = rect.bottom - 1;
			points[3].x = rect.right + 4;	points[3].y = rect.top;


			rgn.CreatePolygonRgn(points, 4, ALTERNATE);
			CBrush br;
			br.CreateSysColorBrush(bActive ? COLOR_WINDOW : COLOR_3DFACE);
			pDC->FillRgn(&rgn, &br);

			CPen *pOldPen = pDC->SelectObject(&penShadow);
			pDC->MoveTo(points[0].x, points[0].y);
			for (int i = 1; i < 4; i++)
				pDC->LineTo(points[i]);

			if (bActive)
			{
				pDC->SelectObject(&penWindow);
				pDC->MoveTo(points[0].x + 1, points[0].y - 1);
				pDC->LineTo(points[3].x, points[3].y);
			}

			pDC->SelectObject(pOldPen);

		}
		else
		{
			/*
			CRect r(rect);
			r.left++;
			r.right -= 2;

			if ((bActive))
			{
				pDC->FillSolidRect(r, GetSysColor(COLOR_3DFACE));
			}
			else
			{
				rect.bottom -= 2;
			}

			rect.top += 2;
			pOldPen = pDC->SelectObject(&penHilight);
			pDC->MoveTo(rect.left, !bActive ? rect.top : rect.top - 2);
			pDC->LineTo(rect.left, rect.bottom - 1);
			pDC->SelectObject(&penFace);
			pDC->MoveTo(rect.left + 1, rect.bottom - 2);
			pDC->LineTo(rect.left + 1, rect.bottom - 1);
			pDC->LineTo(rect.right - 2, rect.bottom - 1);
			pDC->MoveTo(rect.right - 3, rect.bottom - 2);
			pDC->LineTo(rect.right - 1, rect.bottom - 2);
			pDC->MoveTo(rect.right - 2, rect.bottom - 2);
			pDC->LineTo(rect.right - 2, !bActive ? rect.top - 1 : rect.top - 3);
			pDC->SelectObject(&penDkShadow);
			pDC->MoveTo(rect.left + 2, rect.bottom);
			pDC->LineTo(rect.right - 2, rect.bottom);
			pDC->MoveTo(rect.right - 2, rect.bottom - 1);
			pDC->LineTo(rect.right - 1, rect.bottom - 1);
			pDC->MoveTo(rect.right - 1, rect.bottom - 2);
			pDC->LineTo(rect.right - 1, rect.top - 1);
			pDC->SelectObject(pOldPen ) ;
			*/
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCuteTabCtrl class

// Function name	: CCuteTabCtrl::CCuteTabCtrl
// Description	    : default constrcutor
// Return type		: 
CCuteTabCtrl::CCuteTabCtrl()
: m_ptLButtonDown(0)
{
	m_nMarginLeft = 0;
	m_nMarginRight = 0;
	m_nMarginTop = 0;

	m_nPaddingLeft = 0;
	m_nPaddingRight = 0;

	m_nActiveItem = -1;
	m_dwStyle = 0;
	m_dwExStyle = 0;
	m_nButtonWidth = 16;
	m_nButtonHeight = 16;
	m_nButtonGap = 0;
	m_rcLeft.SetRect(0,0,m_nButtonWidth,m_nButtonHeight);
	m_rcRight.SetRect(0,0,m_nButtonWidth,m_nButtonHeight);
	m_nScrollStart = 0;
	m_nScrollEnd = 0;
	m_bShowScrollButton = FALSE;
	m_bEnableDragDrop = FALSE;
}

// Function name	: CCuteTabCtrl::~CCuteTabCtrl
// Description	    : virtual destructor
// Return type		: 
CCuteTabCtrl::~CCuteTabCtrl()
{
	int count = GetCount();
	for (int i = count - 1; i >= 0; i--)
		DeleteItem(i);
}


BEGIN_MESSAGE_MAP(CCuteTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CCuteTabCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_LEFT, OnButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, OnButtonRight)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCuteTabCtrl message handlers

BOOL CCuteTabCtrl::Create(CWnd* pParentWnd, CString &pTitle, UINT nID, DWORD dwStyle) 
{
	ASSERT_VALID(pParentWnd);   // must have a parent
	ASSERT(dwStyle & WS_CHILD);
	ASSERT(dwStyle & TCXS_MASK);

	if (CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS, AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), NULL),
		"",
		(dwStyle & ~TCXS_MASK) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CRect(0,0,0,0), 
		pParentWnd, 
		nID))
	{
		// only save CCuteTab's style
		m_dwStyle = dwStyle & TCXS_MASK;

		// conflicted styles
		ASSERT(!((m_dwExStyle & TCXS_EX_AUTOSIZE) && (m_dwExStyle & TCXS_EX_SCROLLABLE)));

		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		HFONT hFont = GetFontHandle();
		::GetObject(hFont, sizeof(lf), &lf);
		m_fntNormal.CreateFontIndirect(&lf);
		lf.lfWeight = FW_BOLD;
		m_fntActive.CreateFontIndirect(&lf);
		SetFont(&m_fntNormal);

		if (m_dwExStyle & TCXS_EX_SCROLLABLE)
		{
			if(!m_btnLeft.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_LEFT ))
			{
				TRACE0("Unable to create scroll left button\n");
				return FALSE;
			}
			m_btnLeft.m_iDirection = DIR_LEFT;

			if(!m_btnRight.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_RIGHT ))
			{
				TRACE0("Unable to create scroll right button\n");
				return FALSE;
			}
			m_btnRight.m_iDirection = DIR_RIGHT;
		}

		m_tooltip.Create(this, TTS_ALWAYSTIP);
		m_tooltip.Activate(TRUE);
		//m_tooltip.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		return TRUE;
	}
	return FALSE;
	UNREFERENCED_PARAMETER(pTitle);
}

void CCuteTabCtrl::PaintClient(CDC *pDC)
{
	CGlandaMemDC dcMem(pDC);
	dcMem.SetBkMode(TRANSPARENT);

	CRect rect; 
	GetClientRect(rect);


	PaintBackground(&dcMem, rect);

	// draw all tabs
	int count = GetCount();
	if (count > 0)
	{
		CFont *pFont = dcMem.SelectObject(&m_fntNormal);
		if (m_dwExStyle & (TCXS_EX_VID6))
			dcMem.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
		else
		if (m_dwExStyle & TCXS_EX_FLAT)
			dcMem.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
		else
			dcMem.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

		for (int i = count - 1; i >= 0; i--)
		{
			int nStart = m_arItems[i]->GetStart();
			if (nStart >= rect.left + m_nMarginLeft)
			{
				if (nStart < rect.right - m_nMarginRight/* || !m_bShowScrollButton*/)
				{
					if (i != m_nActiveItem)
						m_arItems[i]->Draw(&dcMem, i, m_nActiveItem, GetFocus() == this);
				}
				else
				{
					break;
				}
			}
		}

		if (m_nActiveItem >= 0)
		{
			if (GetExtendedStyle() & TCXS_EX_BOLD_ACTIVE)
				dcMem.SelectObject(&m_fntActive);
			dcMem.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			m_arItems[m_nActiveItem]->Draw(&dcMem, m_nActiveItem, m_nActiveItem, GetFocus() == this);
		}

		dcMem.SelectObject(pFont);

		/*
		if (m_bShowScrollButton)
		{
			for (i = 0 ; i < count; i++ )
			{
				// Draw a "fragmentized" item if needed. Added by Chen hao
				int nEnd = m_arItems[i]->GetEnd();
				if (nEnd > rect.right - m_nMarginRight)				
				{
					CRect rc = m_arItems[i]->GetItemRect();
					CRect rc2 = rc;
					rc2.left = rect.right - m_nButtonWidth * 2 - 10;
					if (m_dwStyle & TCXS_UP)
						rc2.bottom -= 2;
					else
						rc2.top += 2;
					dcMem.FillSolidRect(&rc2, GetSysColor(COLOR_3DFACE));

					CBitmap bmp;
					bmp.LoadBitmap(IDB_BROKEN_EDGE);
					DrawTransparent(
						&dcMem, 
						rc2.left, 
						rc2.top, 
						3, 
						rc2.Height(), 
						&bmp, 
						0, 
						0, 
						RGB(255, 0, 255));

					break;
				}
			}
		}
		*/
	}
}

// Function name	: CCuteTabCtrl::OnPaint
// Description	    : 
// Return type		: void 
void CCuteTabCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	PaintClient(&dc);
}


// Function name	: CCuteTabCtrl::AutoSize
// Description	    : Called from RecalLayout, for resize all items if need
// Return type		: void 
// Argument         : int nlWidth
// Argument         : int nAreaWidth: Client rect's width
void CCuteTabCtrl::AutoSize(int nLastEnd)
{
	ASSERT(m_dwExStyle & (TCXS_EX_AUTOSIZE | TCXS_EX_SCROLLABLE));

	CRect rc;
	GetClientRect(&rc);

	if (m_dwExStyle & TCXS_EX_SCROLLABLE)
	{
		if (m_bShowScrollButton)
		{
			int nItems = GetCount();
			ASSERT(nItems > 0);

			if (m_nScrollStart >= nItems)
				m_nScrollStart = nItems -1 ;

			int nStart = m_arItems[m_nScrollStart]->GetStart();
			int nEnd = rc.right - 2 * m_nButtonWidth;

			int nHeight = GetItemHeight();
			m_rcLeft.SetRect(nEnd, nHeight -  m_nButtonHeight, nEnd + m_nButtonWidth , nHeight);
			m_rcRight.SetRect(nEnd + m_nButtonWidth , nHeight - m_nButtonHeight, nEnd + 2 * m_nButtonWidth , nHeight);
			m_btnLeft.MoveWindow(m_rcLeft ) ;
			m_btnRight.MoveWindow(m_rcRight ) ;

			for (int i = 0 ; i < nItems ; i++)
				m_arItems[i]->SetStart(m_nMarginLeft + m_arItems[i]->GetStart() - nStart) ;

			m_btnLeft.ShowWindow(SW_SHOW) ;
			m_btnRight.ShowWindow(SW_SHOW);			
			if (m_nScrollStart == 0 )
				m_btnLeft.EnableWindow(FALSE);
			else 
				m_btnLeft.EnableWindow(TRUE);
							
			if ( m_arItems[nItems -1]->GetEnd()  <= nEnd ) 
				m_btnRight.EnableWindow(FALSE) ;
			else			
				m_btnRight.EnableWindow(TRUE) ;							
		}
		else
		{
			m_btnLeft.ShowWindow(SW_HIDE) ;
			m_btnRight.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (nLastEnd > rc.right - m_nMarginRight)
		{
			int nItems = GetCount();
			if (nItems > 0)
			{
				int nAreaWidth = max(0, rc.Width() - m_nMarginLeft - m_nMarginRight);
				int nDX = nAreaWidth / nItems;
				int n = m_nMarginLeft;
				int i = 0;
				for (; i < nItems - 1; i++)
				{
					m_arItems[i]->SetStart(n);
					m_arItems[i]->SetWidth(nDX);
					n += nDX;
				}
				m_arItems[i]->SetStart(n);
				m_arItems[i]->SetWidth(nAreaWidth - n);
			}
		}
	}
}

// Function name	: CCuteTabCtrl::RecalcLayout
// Description	    : Called after resize, after AddItem, or DeleteItem
// Return type		: void 
void CCuteTabCtrl::RecalcLayout()
{
	if (::IsWindow(m_tooltip.m_hWnd))
		for (int i = 0; i < m_tooltip.GetToolCount(); i++)
			m_tooltip.DelTool(this, i);

	int count = GetCount();
	if (count > 0)
	{
		CRect rect; 
		GetClientRect(rect);

		CDC* pDC = GetDC();

		int i = 0;
		int nStart;
		for ( i = 0, nStart = m_nMarginLeft; i < count; i++)
		{
			int cx = m_arItems[i]->GetIdealWidth(pDC);
			m_arItems[i]->SetStart(nStart);
			m_arItems[i]->SetWidth(cx);
			nStart += cx;
		}
		ReleaseDC(pDC);

		m_bShowScrollButton = FALSE;
		int nDelta = nStart - (rect.Width() - m_nMarginRight);
		if (m_dwExStyle & TCXS_EX_PHOTOSHOP)
			nDelta += GetItemHeight() / 2;
		if (nDelta > 0)
		{
			if (m_dwExStyle & TCXS_EX_AUTOSIZE)
			{
				AutoSize(nStart);
			}
			else 
			if (m_dwExStyle & TCXS_EX_SCROLLABLE)
			{
				m_bShowScrollButton = TRUE;
				AutoSize(nStart);
			}
			else
			{
				if (count >= 2)
				{
					int nOverlap = nDelta / (count - 1);
					if (nDelta % (count - 1) > 0)
						nOverlap++;

					for (int i = 1; i < count; i++)
						m_arItems[i]->SetStart(max(m_nMarginLeft + i * 3, m_arItems[i - 1]->GetStart() + m_arItems[i - 1]->GetWidth() - nOverlap));
				}
			}
		}

		if (::IsWindow(m_tooltip.m_hWnd))
		{
			int tips = m_tooltip.GetToolCount();
			if (tips > 0)
			{
				for (i = tips - 1; i >= 0; i--)
					m_tooltip.DelTool(this, 1);
				ASSERT(m_tooltip.GetToolCount() == 0);
			}

			if (nDelta > 0)
			{
				for (i = 0; i < count; i++)
				{
					CTabItem *pTab = m_arItems[i];
					m_tooltip.AddTool(this, pTab->GetToolTip(), &pTab->GetItemRect(), 1);
				}
			}
		}
	}
}

// Function name	: CCuteTabCtrl::OnSize
// Description	    : 
// Return type		: void 
// Argument         : UINT nType
// Argument         : int cx
// Argument         : int cy
void CCuteTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	RecalcLayout();
	InvalidateNew();
}

// Function name	: CCuteTabCtrl::GetCount
// Description	    : Return the number of items
// Return type		: int 
int CCuteTabCtrl::GetCount()
{
	return (int)m_arItems.GetSize();
}

// Function name	: CCuteTabCtrl::ItemSize
// Description	    : Return Item size of nIndex item, if this is positive
// Return type		: CSize 
// Argument         : int iIndex
CSize CCuteTabCtrl::ItemSize(int iIndex)
{
	if (iIndex >= 0)
		if (iIndex < GetCount())
			return CSize(m_arItems[iIndex]->GetWidth(), GetItemHeight());
	CSize sResult(0, GetItemHeight());
	for (int i = 0; i < GetCount(); i++)
		sResult.cx += m_arItems[i]->GetWidth();
	return sResult;
}

int CCuteTabCtrl::InsertItem(LPCTSTR lpszItemText, int index, LPARAM lParam, HICON hIcon)
{
	int nResult = -1;
	if (CTabItem* pItem = CTabItem::NewItem(lpszItemText, this))
	{
		pItem->SetToolTip(lpszItemText);

		int count = GetCount();
		if (index == 0 || index < count)
		{
			m_arItems.InsertAt(index, pItem);
			nResult = index;
		}
		else
		{
			m_arItems.Add(pItem);
			nResult = count;
		}

		pItem->m_hIcon = hIcon;
		pItem->m_lParam = lParam;

		//if (m_bShowScrollButton)
		//	OnButtonRight();

		if (m_nActiveItem == -1)
		{
			// SetActive() will call RecalcLayout() and InvalidateNew()
			SetActive(nResult, TRUE);
		}
		else
		{
			RecalcLayout();
			InvalidateNew();
		}
	}
	return nResult;
}

// Function name	: CCuteTabCtrl::AddItem
// Description	    : 
// Return type		: int 
// Argument         : LPCTSTR lpszItemText
// Argument         : HICON hIcon
int CCuteTabCtrl::AddItem(LPCTSTR lpszItemText, LPARAM lParam, HICON hIcon)
{
	return InsertItem(lpszItemText, GetCount(), lParam, hIcon);
}

// Function name	: CCuteTabCtrl::GetActiveItem
// Description	    : Return the active item
// Return type		: int 
int CCuteTabCtrl::GetActiveItem()
{
	return m_nActiveItem;
}

// Function name	: CCuteTabCtrl::DeleteItem
// Description	    : 
// Return type		: BOOL 
// Argument         : int nItem
BOOL CCuteTabCtrl::DeleteItem(int nItem)
{
	ASSERT(nItem >= 0 && nItem < GetCount());
	if (nItem >= 0 && nItem < GetCount())
	{
		int nActive = m_nActiveItem;
		if (nItem < m_nActiveItem && nItem > 0)	// 1 to m_nActiveItem - 1
			nActive--;

		SetActive(-1, FALSE);

		delete m_arItems[nItem];
		m_arItems.RemoveAt(nItem);

		int nCount = GetCount();
		if (nCount == 0)
		{
			nActive = -1;
		}
		else
		{
			if (nActive > nCount - 1)
				nActive = nCount - 1;
		}

		SetActive(nActive, TRUE);

		//if (::IsWindow(m_hWnd))
		//	RecalcLayout();
		//if (m_bShowScrollButton)
		//	OnButtonLeft() ;
		//InvalidateNew();

		return TRUE;
	}

	return FALSE;
}

// Function name	: CCuteTabCtrl::PreCreateWindow
// Description	    : 
// Return type		: BOOL 
// Argument         : CREATESTRUCT& cs
BOOL CCuteTabCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPCHILDREN;
	
	return CWnd::PreCreateWindow(cs);
}

// Function name	: CCuteTabCtrl::OnLButtonDown
// Description	    : First click will be the focus event
// Return type		: void 
// Argument         : UINT nFlags
// Argument         : CPoint point
void CCuteTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Activate the item, which is onto poitn
	int nResult = ItemFromPoint(point);
	BOOL bOnActiveItem = (nResult == m_nActiveItem);
	if (nResult != -1 /*&& m_arItems[nResult]->GetEnd() < nEnd*/ )
	{
		SetActive(nResult, TRUE);
		UpdateWindow();

		if (nResult >= 0)		
			SNDMSGNOTIFY( NM_CLICK );

		if (bOnActiveItem && m_bEnableDragDrop)
		{
			m_ptLButtonDown = point;
			SetCapture();
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CCuteTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseMove(nFlags, point);

	if (m_bEnableDragDrop)
	{
		if (GetCapture() == this)
		{
			ASSERT(m_nActiveItem >= 0);
			if (abs(point.x - m_ptLButtonDown.x) >= 3 || abs(point.y - m_ptLButtonDown.y) >= 3)
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				OnBeginDrag(point);
			}
		}
	}
}

void CCuteTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bEnableDragDrop)
		if (GetCapture() == this)
			ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CCuteTabCtrl::EnableDragDrop(BOOL bEnable)
{
	m_bEnableDragDrop = bEnable;
}

BOOL CCuteTabCtrl::IsDragDropEnabled()
{
	return m_bEnableDragDrop;
}

void CCuteTabCtrl::OnBeginDrag(CPoint point)
{
}

//
// PURPOSE:			Give parent ctrl a event handle
// PARAMETERS:		
// RETURN VALUE:	
// NOTE:			
//
void CCuteTabCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	// Activate the item, which is onto poitn
	int nResult = ItemFromPoint(point);
	if (nResult >= 0)		
		SNDMSGNOTIFY( NM_RCLICK );

	CWnd::OnRButtonDown(nFlags, point);
}

// Function name	: SetActive 
// Description	    : Set the new active item to be nActiveItem
// Return type		: void 
// Argument         : int nActiveItem
//                  : BOOL bNotify. Will not call OnActivePage() if bNotify==FALSE
// Note:            : RecalcLayout() and InvalidateNew () is called
BOOL CCuteTabCtrl::SetActive(int nActiveItem, BOOL bNotify)
{
	if (nActiveItem < GetCount())
	{
		if (bNotify)
		{
			SNDMSGNOTIFY( TCN_SELCHANGING );
			if (OnActivatePage(m_nActiveItem, nActiveItem))
			{
				m_nActiveItem = nActiveItem;
				RecalcLayout();
				InvalidateNew();
				SNDMSGNOTIFY( TCN_SELCHANGE );
				return TRUE;
			}
			else
			{
				// Can not change active item now
				return FALSE;
			}
		}
		else
		{
			m_nActiveItem = nActiveItem;
			RecalcLayout();
			InvalidateNew();
			return TRUE;
		}
	}

	return FALSE;
}

// Function name	: CCuteTabCtrl::ItemFromPoint
// Description	    : Return index of item from point
// Return type		: int 
// Argument         : CPoint point
int CCuteTabCtrl::ItemFromPoint(CPoint point)
{
	for (int i = GetCount() - 1; i >= 0; i--)
		if (m_arItems[i]->PtInItem(point))
			return i;
	return -1;
}

// Function name	: CCuteTabCtrl::OnEraseBkgnd
// Description	    : Dont erase the background, because I will do in WM_PAINT
// Return type		: BOOL 
// Argument         : CDC* pDC
BOOL CCuteTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;

	UNREFERENCED_PARAMETER(pDC);
}

// Function name	: CCuteTabCtrl::InvalidateNew
// Description	    : Call this, instead Invalidate to check the if this is a window
// Return type		: BOOL 
// Argument         : BOOL bErase
void CCuteTabCtrl::InvalidateNew(BOOL bErase)
{
	if (::IsWindow(m_hWnd))
	{
		CRect rc ;
		GetClientRect(rc) ;
		InvalidateRect(rc, bErase);
	}
}

// Function name	: CCuteTabCtrl::GetStyle
// Description	    : return the particular style
// Return type		: UINT 
DWORD CCuteTabCtrl::GetStyle()
{
	return m_dwStyle;
}

DWORD CCuteTabCtrl::GetExtendedStyle()
{
	return m_dwExStyle;
}

// Function name	: CCuteTabCtrl::GetItemHeight
// Description	    : return the item height of control
// Return type		: int 
int CCuteTabCtrl::GetItemHeight()
{
	if (m_dwExStyle & TCXS_EX_PHOTOSHOP)
		return 16;

	if (m_dwExStyle & TCXS_EX_FLAT)
		return 18;

	if (m_dwExStyle & (TCXS_EX_TRAPEZIA | TCXS_EX_TRAPEZIA2))
		return 17;

	return GetStyle() & TCXS_UP ? 20 : 18;
}

// Function name	: CCuteTabCtrl::OnActivatePage
// Description	    : Called when opertor = is called
// Return type		: BOOL. indicate whether to procceed or not 
// Argument         : int nItemOld
// Argument         : int nItemNew
BOOL CCuteTabCtrl::OnActivatePage(int nItemOld, int nItemNew)
{
	// do nothing here
	return TRUE;

	UNREFERENCED_PARAMETER(nItemOld);
	UNREFERENCED_PARAMETER(nItemNew);
}

// Function name	: Item
// Description	    : return the nIndex item
// Return type		: virtual CTabItem* 
// Argument         : int nIndex
CTabItem* CCuteTabCtrl::Item(int nIndex)
{
	if (nIndex >= 0)
		if (nIndex < GetCount())
			return m_arItems[nIndex];
	return NULL;
}

CString CTabItem::GetTitle()
{
	return m_strTitle;
}

void CTabItem::SetTitle(CString sTitle)
{
	m_strTitle = sTitle ;
}

void CCuteTabCtrl::OnButtonLeft()
{
	if (m_nScrollStart>0) 
	
	{
		m_nScrollStart --  ;
		RecalcLayout();
		InvalidateNew();
	}
}



void CCuteTabCtrl::OnButtonRight()
{
	CRect rc ;
	GetClientRect(rc);
	int nEnd = m_arItems[GetCount() -1]->GetEnd() ;
	int nClientEnd = rc.Width() - 2 * m_nButtonWidth - 2 * m_nButtonGap ;
	if ( (m_nScrollStart < (GetCount()-1) )
		&& (nEnd > nClientEnd ) )
	{
		m_nScrollStart ++  ;
		RecalcLayout();
		InvalidateNew();
	}

}

void CCuteTabCtrl::OnSysColorChange() 
{
	CWnd::OnSysColorChange();	                          
	InvalidateNew() ;                                   
}

void CCuteTabCtrl::SetExtendedStyle(DWORD dwStyle, BOOL bRepaint)
{
	m_dwExStyle = dwStyle;
	if (bRepaint)
	{
		Invalidate();
	}
}

void CTabItem::SetToolTip(LPCTSTR lpszToolTip)
{
	lstrcpyn(m_szToolTip, lpszToolTip, 80);
}

LPCTSTR CTabItem::GetToolTip()
{
	return m_szToolTip;
}

void CCuteTabCtrl::SetToolTip(int index, LPCTSTR lpszToolTip)
{
	ASSERT(index >= 0 && index < m_arItems.GetSize());
	m_arItems[index]->SetToolTip(lpszToolTip);
}

BOOL CCuteTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		m_tooltip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

HFONT CCuteTabCtrl::GetFontHandle(void)
{
	return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

void CCuteTabCtrl::SetMargin(int nMarginLeft, int nMarginRight, int nMarginTop)
{
	m_nMarginLeft = nMarginLeft;
	m_nMarginRight = nMarginRight;
	m_nMarginTop = nMarginTop;
}

void CCuteTabCtrl::SetPadding(int nPaddingLeft, int nPaddingRight)
{
	m_nPaddingLeft = nPaddingLeft;
	m_nPaddingRight = nPaddingRight;
}

void CCuteTabCtrl::SetItemText(int index, LPCTSTR lpszText)
{
	if (index >= 0)
	{
		m_arItems[index]->SetTitle(lpszText);
		RecalcLayout();
		InvalidateNew();
	}
}

void CCuteTabCtrl::PaintBackground(CDC *pDC, CRect &rect)
{
	CRect rcTabs = rect;
	if (GetStyle() & TCXS_UP)
		rcTabs.bottom = rcTabs.top + GetItemHeight();
	else
		rcTabs.top = rcTabs.bottom - GetItemHeight();

	if (m_dwExStyle & TCXS_EX_VID6)
	{
		pDC->FillSolidRect(&rcTabs, ::GetSysColor(COLOR_3DSHADOW));
	}
	else
	if (m_dwExStyle & TCXS_EX_FLAT)
	{
		//pDC->FillSolidRect(&rcTabs, ::GetSysColor(COLOR_3DSHADOW));

		COLORREF crStart, crEnd;
		crStart = crEnd = CLR_INVALID;

		/*
		HTHEME hTheme = NULL;
		if (g_xpStyle.IsAppThemed())
		{
			hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"Window");
			if (hTheme)
			{
				COLORREF crTitle = g_xpStyle.GetThemeSysColor(hTheme, COLOR_ACTIVECAPTION);
				crStart= crTitle;
				crEnd = Darken(Darken(crTitle));
				g_xpStyle.CloseThemeData(hTheme);
			}
		}
		*/

		// failed to get theme color
		if (crStart == CLR_INVALID)
		{
			crStart = ::GetSysColor(COLOR_3DSHADOW);
			crEnd = Darken(Darken(::GetSysColor(COLOR_3DSHADOW)));
		}

		DrawGradientRect(pDC, rcTabs, crStart, crEnd, FALSE);
	}
	else
	{
		pDC->FillSolidRect(&rcTabs, ::GetSysColor(COLOR_3DFACE));
	}

	CRect rcMain(rect);
	if (GetStyle() & TCXS_UP)
		rcMain.top += GetItemHeight();
	else
		rcMain.bottom -= GetItemHeight();

	pDC->FillSolidRect(&rcMain, ::GetSysColor(COLOR_3DFACE));

	if (m_dwExStyle & (TCXS_EX_PHOTOSHOP))
	{
		CPen penDkShadow(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));
		CPen *pOldPen = pDC->SelectObject(&penDkShadow);

		if (m_dwStyle & TCXS_UP)
		{
			pDC->MoveTo(rcMain.left, rcMain.top - 1);
			pDC->LineTo(rcMain.right, rcMain.top - 1);
		}
		else
		{
			pDC->MoveTo(rcMain.left, rcMain.bottom - 1);
			pDC->LineTo(rcMain.right, rcMain.bottom - 1);
		}

		CPen penHilight(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT));
		pDC->SelectObject(&penHilight);

		if (m_dwStyle & TCXS_UP)
		{
			pDC->MoveTo(rcMain.left, rcMain.top);
			pDC->LineTo(rcMain.right, rcMain.top);
		}
		else
		{
			pDC->MoveTo(rcMain.left, rcMain.bottom);
			pDC->LineTo(rcMain.right, rcMain.bottom);
		}

		pDC->SelectObject(pOldPen);
	}
	else
	if (m_dwExStyle & (TCXS_EX_TRAPEZIA | TCXS_EX_TRAPEZIA2))
	{
		CPen penShadow(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = pDC->SelectObject(&penShadow);

		if (m_dwStyle & TCXS_UP)
		{
			pDC->MoveTo(rcMain.left, rcMain.top);
			pDC->LineTo(rcMain.right, rcMain.top);
		}
		else
		{
			pDC->MoveTo(rcMain.left, rcMain.bottom);
			pDC->LineTo(rcMain.right, rcMain.bottom);
		}

		pDC->SelectObject(pOldPen);
	}
	
	if ((m_dwExStyle & TCXS_EX_NOBORDER) == 0)
	{
		pDC->Draw3dRect(&rcMain, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DDKSHADOW));
		rcMain.InflateRect(-1, -1);
		pDC->Draw3dRect(&rcMain, ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_3DSHADOW));
	}
}
