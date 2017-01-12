// Splitter.cpp: implementation of the CSplitter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "Splitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSplitter::CSplitter()
{
	m_nSize = 5;
	m_nLower = 0;
	m_nUpper = 5;
	m_hWndLeft = NULL;
	m_hWndRight = NULL;
	SetRectEmpty();
	m_bMoved = FALSE;
}

CSplitter::~CSplitter()
{

}

BOOL CSplitter::Track(CWnd *pWnd, const CPoint &point)
{
	ASSERT(::GetCapture() == NULL);

	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());
	pWnd->UpdateWindow();

    BOOL bDragShowContent = FALSE;
    //::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bDragShowContent, 0);

	m_ptOrigin = point;
	m_rcOrigin = *this;
	TRACE1("*** Track(): top=%d\n", top);

	CDC *pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);

	CRect rcSave = *this;
	CPoint ptNew;
	m_bMoved = FALSE;

	CBrush* pBrush = CDC::GetHalftoneBrush();
	CBrush* pBrushOld = pDC->SelectObject(pBrush);
	
	if (!bDragShowContent)
		DrawTrackerStart(pDC);

	while (true)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			ptNew.x = (int)(short)LOWORD(msg.lParam);
			ptNew.y = (int)(short)HIWORD(msg.lParam);
			OnMouseMove(ptNew);

			if (!bDragShowContent)
			{
				DrawTracker(pDC);
			}
			else
			{
				CRect rcClient;
				pWnd->GetClientRect(&rcClient);
				pWnd->SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rcClient.Width(), rcClient.Height()));
			}

			break;

		case WM_LBUTTONUP:
			goto _exit;

		// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;

		case WM_RBUTTONDOWN:
			CRect::CopyRect(rcSave);
			goto _exit;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

_exit:
	if (!bDragShowContent)
		DrawTrackerEnd(pDC);

	pDC->SelectObject(pBrushOld);
	pWnd->ReleaseDC(pDC);

	ReleaseCapture();

	return m_bMoved;
}

void CSplitter::OnMouseMove(const CPoint &point)
{
	m_bMoved = TRUE;
}

BOOL CSplitter::AttachLeft(HWND hWnd)
{
	ASSERT(!m_hWndLeft);
	m_hWndLeft = hWnd;
	return TRUE;
}

BOOL CSplitter::AttachRight(HWND hWnd)
{
	ASSERT(!m_hWndRight);
	m_hWndRight = hWnd;
	return TRUE;
}

void CSplitter::DrawTrackerStart(CDC *pDC)
{
	pDC->PatBlt(left, top, Width(), Height(), PATINVERT);
	m_rcTracker = *this;
}

void CSplitter::DrawTracker(CDC *pDC)
{
	if (m_rcTracker != *this)
	{
		pDC->PatBlt(m_rcTracker.left, m_rcTracker.top, m_rcTracker.Width(), m_rcTracker.Height(), PATINVERT);
		pDC->PatBlt(left, top, Width(), Height(), PATINVERT);
		m_rcTracker = *this;
	}
}

void CSplitter::DrawTrackerEnd(CDC *pDC)
{
	pDC->PatBlt(m_rcTracker.left, m_rcTracker.top, m_rcTracker.Width(), m_rcTracker.Height(), PATINVERT);
}

void CSplitter::SetRange(int nLower, int nUpper)
{
	m_nLower = nLower;
	m_nUpper = max(nLower + m_nSize, nUpper);
	AdjustPosition();
}

void CSplitter::AdjustPosition()
{

}

void CSplitter::SetSize(int nSize)
{
	m_nSize = nSize;
	m_nUpper = max(m_nLower + nSize, m_nUpper);
}

int CSplitter::GetSize()
{
	return m_nSize;
}
