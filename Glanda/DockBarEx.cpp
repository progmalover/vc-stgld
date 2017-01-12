// DockBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "DockBarEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CDockBarEx

IMPLEMENT_DYNAMIC(CDockBarEx, CDockBar)
CDockBarEx::CDockBarEx()
{
}

CDockBarEx::~CDockBarEx()
{
}


BEGIN_MESSAGE_MAP(CDockBarEx, CDockBar)
END_MESSAGE_MAP()



// CDockBarEx message handlers

CSize CDockBarEx::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	// Copied form MFC, but closed "Wrapper" feature for horz dockbars

	ASSERT_VALID(this);

	CSize sizeFixed = CControlBar::CalcFixedLayout(bStretch, bHorz);

	// get max size
	CSize sizeMax;
	if (!m_rectLayout.IsRectEmpty())
	{
		CRect rect = m_rectLayout;
		CalcInsideRect(rect, bHorz);
		sizeMax = rect.Size();
	}
	else
	{
		CRect rectFrame;
		CFrameWnd* pFrame = GetParentFrame();
		pFrame->GetClientRect(&rectFrame);
		sizeMax = rectFrame.Size();
	}

	// prepare for layout
	AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = m_bLayoutQuery ?
		NULL : ::BeginDeferWindowPos(m_arrBars.GetSize());
	CPoint pt(-afxData.cxBorder2, -afxData.cyBorder2);
	int nWidth = 0;

	BOOL bWrapped = FALSE;

	// layout all the control bars
	for (int nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		void* pVoid = m_arrBars[nPos];

		if (pBar != NULL)
		{
			if (pBar->IsVisible())
			{
				// get ideal rect for bar
				DWORD dwMode = 0;
				if ((pBar->m_dwStyle & CBRS_SIZE_DYNAMIC) &&
					(pBar->m_dwStyle & CBRS_FLOATING))
					dwMode |= LM_HORZ | LM_MRUWIDTH;
				else if (pBar->m_dwStyle & CBRS_ORIENT_HORZ)
					dwMode |= LM_HORZ | LM_HORZDOCK;
				else
					dwMode |=  LM_VERTDOCK;

				CSize sizeBar = pBar->CalcDynamicLayout(-1, dwMode);

				CRect rect(pt, sizeBar);

				// get current rect for bar
				CRect rectBar;
				pBar->GetWindowRect(&rectBar);
				ScreenToClient(&rectBar);

				if (bHorz)
				{
					// Offset Calculated Rect out to Actual
					if (rectBar.left > rect.left && !m_bFloating)
						rect.OffsetRect(rectBar.left - rect.left, 0);

					// If ControlBar goes off the right, then right justify
					if (rect.right > sizeMax.cx && !m_bFloating)
					{
						int x = rect.Width() - afxData.cxBorder2;
						x = max(sizeMax.cx - x, pt.x);
						rect.OffsetRect(x - rect.left, 0);
					}

					// If ControlBar has been wrapped, then left justify
					/*
					if (bWrapped)
					{
						bWrapped = FALSE;
						rect.OffsetRect(-(rect.left + afxData.cxBorder2), 0);
					}
					// If ControlBar is completely invisible, then wrap it
					else if ((rect.left >= (sizeMax.cx - afxData.cxBorder2)) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}
					*/
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING))
							{
								pBar->m_pDockContext->m_rectMRUDockPos = rect;
							}
							AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
						}
						pt.x = rect.left + sizeBar.cx - afxData.cxBorder2;
						nWidth = max(nWidth, sizeBar.cy);
					}
				}
				else
				{
					// Offset Calculated Rect out to Actual
					if (rectBar.top > rect.top && !m_bFloating)
						rect.OffsetRect(0, rectBar.top - rect.top);

					// If ControlBar goes off the bottom, then bottom justify
					if (rect.bottom > sizeMax.cy && !m_bFloating)
					{
						int y = rect.Height() - afxData.cyBorder2;
						y = max(sizeMax.cy - y, pt.y);
						rect.OffsetRect(0, y - rect.top);
					}

					// If ControlBar has been wrapped, then top justify
					/*
					if (bWrapped)
					{
						bWrapped = FALSE;
						rect.OffsetRect(0, -(rect.top + afxData.cyBorder2));
					}
					// If ControlBar is completely invisible, then wrap it
					else if ((rect.top >= (sizeMax.cy - afxData.cyBorder2)) &&
						(nPos > 0) && (m_arrBars[nPos - 1] != NULL))
					{
						m_arrBars.InsertAt(nPos, (CObject*)NULL);
						pBar = NULL; pVoid = NULL;
						bWrapped = TRUE;
					}
					*/
					if (!bWrapped)
					{
						if (rect != rectBar)
						{
							if (!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING))
							{
								pBar->m_pDockContext->m_rectMRUDockPos = rect;
							}
							AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
						}
						pt.y = rect.top + sizeBar.cy - afxData.cyBorder2;
						nWidth = max(nWidth, sizeBar.cx);
					}
				}
			}
			if (!bWrapped)
			{
				// handle any delay/show hide for the bar
				pBar->RecalcDelayShow(&layout);
			}
		}
		if (pBar == NULL && pVoid == NULL && nWidth != 0)
		{
			// end of row because pBar == NULL
			if (bHorz)
			{
				pt.y += nWidth - afxData.cyBorder2;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.x = -afxData.cxBorder2;
			}
			else
			{
				pt.x += nWidth - afxData.cxBorder2;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.y = -afxData.cyBorder2;
			}
			nWidth = 0;
		}
	}
	if (!m_bLayoutQuery)
	{
		// move and resize all the windows at once!
		if (layout.hDWP != NULL)
			::EndDeferWindowPos(layout.hDWP);
		else
			TRACE0("Warning: DeferWindowPos failed - low system resources.\n");
	}

	// adjust size for borders on the dock bar itself
	CRect rect;
	rect.SetRectEmpty();
	CalcInsideRect(rect, bHorz);

	if ((!bStretch || !bHorz) && sizeFixed.cx != 0)
		sizeFixed.cx += -rect.right + rect.left;
	if ((!bStretch || bHorz) && sizeFixed.cy != 0)
		sizeFixed.cy += -rect.bottom + rect.top;

	return sizeFixed;
}
