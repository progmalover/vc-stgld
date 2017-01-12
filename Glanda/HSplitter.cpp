#include "StdAfx.h"
#include ".\hsplitter.h"

CHSplitter::CHSplitter(void)
{
}

CHSplitter::~CHSplitter(void)
{
}

void CHSplitter::OnMouseMove(const CPoint &point)
{
	if (point.x != m_ptOrigin.x)
	{
		CopyRect(m_rcOrigin);
		TRACE2("%d - %d\n", point.x, m_ptOrigin.x);
		OffsetRect(point.x - m_ptOrigin.x, 0);
		AdjustPosition();
		m_bMoved = TRUE;
	}
}

void CHSplitter::AdjustPosition()
{
	TRACE1("%d -> ", left);
	int width = Width();
	left = min(m_nUpper - width, max(m_nLower, left));
	right = left + width;
	TRACE3("(%d, %d) -> %d\n", m_nLower, m_nUpper, left);
}
