// VSplitter.cpp: implementation of the CVSplitter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "VSplitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVSplitter::CVSplitter()
{

}

CVSplitter::~CVSplitter()
{

}

void CVSplitter::OnMouseMove(const CPoint &point)
{
	if (point.y != m_ptOrigin.y)
	{
		CopyRect(m_rcOrigin);
		TRACE2("%d - %d\n", point.y, m_ptOrigin.y);
		OffsetRect(0, point.y - m_ptOrigin.y);
		AdjustPosition();
		m_bMoved = TRUE;
	}
}

void CVSplitter::AdjustPosition()
{
	//TRACE1("%d -> ", top);
	int height = Height();
	top = min(m_nUpper - height, max(m_nLower, top));
	bottom = top + height;
	//TRACE3("(%d, %d) -> %d\n", m_nLower, m_nUpper, top);
}
