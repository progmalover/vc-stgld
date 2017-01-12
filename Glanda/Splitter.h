// Splitter.h: interface for the CSplitter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTER_H__F4C53AB4_3848_4A89_9492_1855073B11CD__INCLUDED_)
#define AFX_SPLITTER_H__F4C53AB4_3848_4A89_9492_1855073B11CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSplitter : public CRect  
{
public:
	int GetSize();
	void SetSize(int nSize);
	virtual void AdjustPosition();
	void SetRange(int nLower, int nUpper);
	BOOL AttachRight(HWND hWnd);
	BOOL AttachLeft(HWND hWnd);
	BOOL Track(CWnd *pWnd, const CPoint &point);
	CSplitter();
	virtual ~CSplitter();

protected:
	int m_nSize;
	int m_nUpper;
	int m_nLower;
	CRect m_rcTracker;
	void DrawTrackerEnd(CDC *pDC);
	void DrawTracker(CDC *pDC);
	void DrawTrackerStart(CDC *pDC);
	HWND m_hWndRight;
	HWND m_hWndLeft;
	CPoint m_ptOrigin;	// Cussor pos when starting dragging
	CRect m_rcOrigin;	// Self rect when starting dragging
	BOOL m_bMoved;
	virtual void OnMouseMove(const CPoint &point);
};

#endif // !defined(AFX_SPLITTER_H__F4C53AB4_3848_4A89_9492_1855073B11CD__INCLUDED_)
