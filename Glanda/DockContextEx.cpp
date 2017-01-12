#include "StdAfx.h"
#include "dockcontextex.h"
#include "wingdi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern void AFXAPI _AfxAdjustRectangle(CRect& rect, CPoint pt);

#define HORZF(dw) (dw & CBRS_ORIENT_HORZ)
#define VERTF(dw) (dw & CBRS_ORIENT_VERT)

#define DOCK_SENSITIVITY	15

CDockContextEx::CDockContextEx(CControlBar* pBar) : CDockContext(pBar)
{
	// base class only init left and right.
	m_rectMRUDockPos.SetRectEmpty();
}

void CDockContextEx::StartDrag(CPoint pt)
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));

	ASSERT_VALID(m_pBar);
	m_bDragging = TRUE;

	InitLoop();

	// GetWindowRect returns screen coordinates(not mirrored),
	// so if the desktop is mirrored then turn off mirroring
	// for the desktop dc so that we get correct focus rect drawn.
	// This layout change should be remembered, just in case ...

	if (m_pDC->GetLayout() & 1L)
		m_pDC->SetLayout(0L);

	if (m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
	{
		// get true bar size (including borders)
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(0, LM_VERTDOCK);
		CSize sizeFloat = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);

		m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectDragVert = CRect(rect.TopLeft(), sizeVert);

		// calculate frame dragging rectangle
		m_rectFrameDragHorz = CRect(rect.TopLeft(), sizeFloat);
		m_rectFrameDragVert = CRect(rect.TopLeft(), sizeFloat);

		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);

		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	else if (m_pBar->m_dwStyle & CBRS_SIZE_FIXED)
	{
		// get true bar size (including borders)
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(-1, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(-1, LM_VERTDOCK);

		// calculate frame dragging rectangle
		m_rectFrameDragHorz = m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectFrameDragVert = m_rectDragVert = CRect(rect.TopLeft(), sizeVert);

		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	else
	{
		// get true bar size (including borders)
		CRect rect;
		m_pBar->GetWindowRect(rect);
		m_ptLast = pt;
		BOOL bHorz = HORZF(m_dwStyle);
		DWORD dwMode = !bHorz ? (LM_HORZ | LM_HORZDOCK) : LM_VERTDOCK;
		CSize size = m_pBar->CalcDynamicLayout(-1, dwMode);

		// calculate inverted dragging rect
		if (bHorz)
		{
			m_rectDragHorz = rect;
			m_rectDragVert = CRect(CPoint(pt.x - rect.Height()/2, rect.top), size);
		}
		else // vertical orientation
		{
			m_rectDragVert = rect;
			m_rectDragHorz = CRect(CPoint(rect.left, pt.y - rect.Width()/2), size);
		}

		// calculate frame dragging rectangle
		m_rectFrameDragHorz = m_rectDragHorz;
		m_rectFrameDragVert = m_rectDragVert;

		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}

	// adjust rectangles so that point is inside
	_AfxAdjustRectangle(m_rectDragHorz, pt);
	_AfxAdjustRectangle(m_rectDragVert, pt);
	_AfxAdjustRectangle(m_rectFrameDragHorz, pt);
	_AfxAdjustRectangle(m_rectFrameDragVert, pt);

	// initialize tracking state and enter tracking loop
	m_dwOverDockStyle = CanDock();
	//Move(pt);   // call it here to handle special keys
	Track();
}

void CDockContextEx::Move(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;

	// offset all drag rects to new position
	m_rectDragHorz.OffsetRect(ptOffset);
	m_rectFrameDragHorz.OffsetRect(ptOffset);
	m_rectDragVert.OffsetRect(ptOffset);
	m_rectFrameDragVert.OffsetRect(ptOffset);
	m_ptLast = pt;

	// if control key is down don't dock
	m_dwOverDockStyle = m_bForceFrame ? 0 : CanDock();

	// update feedback
	DrawFocusRect();
}

BOOL CDockContextEx::Track()
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// set capture to the window which received this message
	m_pBar->SetCapture();
	ASSERT(m_pBar == CWnd::GetCapture());

	// get messages until capture lost or cancelled/accepted
	while (CWnd::GetCapture() == m_pBar)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		switch (msg.message)
		{
		case WM_LBUTTONUP:
			if (m_bDragging)
				EndDrag();
			else
				EndResize();
			return TRUE;
		case WM_MOUSEMOVE:
			if (m_bDragging)
				Move(msg.pt);
			else
				Stretch(msg.pt);
			break;
		case WM_KEYUP:
			if (m_bDragging)
				OnKey((int)msg.wParam, FALSE);
			break;
		case WM_KEYDOWN:
			if (m_bDragging && (msg.wParam == VK_CONTROL || msg.wParam == VK_SHIFT))
			{
				OnKey((int)msg.wParam, TRUE);
			}
			else
			{
				// cancel on any other key, or will cause redrawing problems.
				CancelLoop();
				return FALSE;
			}
			break;
		case WM_RBUTTONDOWN:
			CancelLoop();
			return FALSE;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	CancelLoop();

	return FALSE;
}

void CDockContextEx::OnKey(int nChar, BOOL bDown)
{
	if (nChar == VK_CONTROL)
		UpdateState(&m_bForceFrame, bDown);
	if (nChar == VK_SHIFT)
		UpdateState(&m_bFlip, bDown);
}

void CDockContextEx::UpdateState(BOOL* pFlag, BOOL bNewValue)
{
	if (*pFlag != bNewValue)
	{
		*pFlag = bNewValue;
		m_bFlip = (HORZF(m_dwDockStyle) && VERTF(m_dwDockStyle) && m_bFlip); // shift key
		m_dwOverDockStyle = (m_bForceFrame) ? 0 : CanDock();
		DrawFocusRect();
	}
}

DWORD CDockContextEx::CanDock()
{
	BOOL bStyleHorz;
	DWORD dwDock = 0; // Dock Canidate
	DWORD dwCurr = 0; // Current Orientation

	// let's check for something in our current orientation first
	// then if the shift key is not forcing our orientation then
	// check for horizontal or vertical orientations as long
	// as we are close enough
	ASSERT(m_dwStyle != 0);

	bStyleHorz = HORZF(m_dwStyle);
	bStyleHorz = m_bFlip ? !bStyleHorz : bStyleHorz;

	if (bStyleHorz && HORZF(m_dwDockStyle))
		dwDock = CanDock(m_dwDockStyle & ~CBRS_ORIENT_VERT);
	else if (VERTF(m_dwDockStyle))
		dwDock = CanDock(m_dwDockStyle & ~CBRS_ORIENT_HORZ);

	if (!m_bFlip)
	{
		if (dwDock == 0 && HORZF(m_dwDockStyle))
		{
			dwCurr = CanDock(m_dwDockStyle & ~CBRS_ORIENT_VERT);
			dwDock = CanDock(m_dwDockStyle & ~CBRS_ORIENT_VERT);
			dwDock = (dwDock == dwCurr) ? dwDock : 0;
		}
		if (dwDock == 0 && VERTF(m_dwDockStyle))
		{
			dwCurr = CanDock(m_dwDockStyle & ~CBRS_ORIENT_HORZ);
			dwDock = CanDock(m_dwDockStyle & ~CBRS_ORIENT_HORZ);
			dwDock = (dwDock == dwCurr) ? dwDock : 0;
		}
	}

	return dwDock;
}

CDockBar* CDockContextEx::GetDockBar(DWORD dwOverDockStyle)
{
	DWORD dw = 0;
	CDockBar* pBar;
	if (HORZF(dwOverDockStyle))
	{
		dw = CanDock(dwOverDockStyle & ~CBRS_ORIENT_VERT, &pBar);
		ASSERT(dw != 0);
		ASSERT(pBar != NULL);
		return pBar;
	}
	if (VERTF(dwOverDockStyle))
	{
		dw = CanDock(dwOverDockStyle & ~CBRS_ORIENT_HORZ, &pBar);
		ASSERT(dw != 0);
		ASSERT(pBar != NULL);
		return pBar;
	}
	return NULL;
}


DWORD CDockContextEx::CanDock(DWORD dwDockStyle, CDockBar** ppDockBar)
{
	// dwDockStyle -- allowable styles of bar
	// don't allow to dock to floating unless multi is specified
	dwDockStyle &= CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI;

	CPoint point;
	GetCursorPos(&point);
	CRect rect(point.x - DOCK_SENSITIVITY,  point.y - DOCK_SENSITIVITY, point.x + DOCK_SENSITIVITY, point.y + DOCK_SENSITIVITY);

	if (ppDockBar != NULL)
		*ppDockBar = NULL;
	POSITION pos = m_pDockSite->m_listControlBars.GetHeadPosition();
	while (pos != NULL)
	{
		CDockBar* pDockBar = (CDockBar*)m_pDockSite->m_listControlBars.GetNext(pos);
		if (pDockBar->IsDockBar() && pDockBar->IsWindowVisible() &&
			(pDockBar->m_dwStyle & dwDockStyle & CBRS_ALIGN_ANY) &&
			(!pDockBar->m_bFloating ||
				(dwDockStyle & pDockBar->m_dwStyle & CBRS_FLOAT_MULTI)))
		{
			CRect rectBar;
			pDockBar->GetWindowRect(&rectBar);
			if (rectBar.Width() == 0)
				rectBar.right++;
			if (rectBar.Height() == 0)
				rectBar.bottom++;
			if (rectBar.IntersectRect(rectBar, rect))
			{
				if (ppDockBar != NULL)
					*ppDockBar = pDockBar;
				return pDockBar->m_dwStyle & dwDockStyle;
			}
		}
	}
	return 0;
}

void CDockContextEx::EndDrag()
{
	CancelLoop();

	if (m_dwOverDockStyle != 0)
	{
		CDockBar* pDockBar = GetDockBar(m_dwOverDockStyle);
		ASSERT(pDockBar != NULL);

		CRect rect = (m_dwOverDockStyle & CBRS_ORIENT_VERT) ?
			m_rectDragVert : m_rectDragHorz;

		UINT uID = _AfxGetDlgCtrlID(pDockBar->m_hWnd);
		if (uID >= AFX_IDW_DOCKBAR_TOP &&
			uID <= AFX_IDW_DOCKBAR_BOTTOM)
		{
			m_uMRUDockID = uID;
			m_rectMRUDockPos = rect;
			pDockBar->ScreenToClient(&m_rectMRUDockPos);
		}

		// dock it at the specified position, RecalcLayout will snap
		m_pDockSite->DockControlBar(m_pBar, pDockBar, &rect);
		m_pDockSite->RecalcLayout();
	}
	else if ((m_dwStyle & CBRS_SIZE_DYNAMIC) || (HORZF(m_dwStyle) && !m_bFlip) ||
			(VERTF(m_dwStyle) && m_bFlip))
	{
		m_dwMRUFloatStyle = CBRS_ALIGN_TOP | (m_dwDockStyle & CBRS_FLOAT_MULTI);
		m_ptMRUFloatPos = m_rectFrameDragHorz.TopLeft();
		m_pDockSite->FloatControlBar(m_pBar, m_ptMRUFloatPos, m_dwMRUFloatStyle);
	}
	else // vertical float
	{
		m_dwMRUFloatStyle = CBRS_ALIGN_LEFT | (m_dwDockStyle & CBRS_FLOAT_MULTI);
		m_ptMRUFloatPos = m_rectFrameDragVert.TopLeft();
		m_pDockSite->FloatControlBar(m_pBar, m_ptMRUFloatPos, m_dwMRUFloatStyle);
	}
}
