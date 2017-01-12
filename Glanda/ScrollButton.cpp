// CJButton.cpp : implementation file
//
// Owner drawn button control.
//
// Copyright ?1998 Written by Kirk Stowell   
//		mailto:kstowel@sprynet.com
//		http://www.geocities.com/SiliconValley/Haven/8230
//
// This code may be used in compiled form in any way you desire. This  
// file may be redistributed unmodified by any means PROVIDING it is   
// not sold for profit without the authors written consent, and   
// providing that this notice and the authors name and all copyright   
// notices remains intact. If the source code in this file is used in   
// any  commercial application then a statement along the lines of   
// "Portions Copyright ?1998 Kirk Stowell" must be included in   
// the startup banner, "About" box or printed documentation. An email   
// letting me know that you are using it would be nice as well. That's   
// not much to ask considering the amount of work that went into this.  
//  
// This file is provided "as is" with no expressed or implied warranty.  
// The author accepts no liability for any damage/loss of business that  
// this product may cause.  
//
// ==========================================================================
// HISTORY:	
// ==========================================================================
//			1.00	17 Oct 1998	- Initial re-write and release.
// ==========================================================================
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScrollButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScrollButton

//##ModelId=3B2040AA0106
CScrollButton::CScrollButton()
{
}

//##ModelId=3B2040AA00FC
CScrollButton::~CScrollButton()
{
}

IMPLEMENT_DYNAMIC(CScrollButton, CButton)

BEGIN_MESSAGE_MAP(CScrollButton, CButton)
	//{{AFX_MSG_MAP(CScrollButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollButton message handlers

// Function name	: CScrollButton::DrawItem
// Description	    : if use image draw transparent bitmap else draw standard button
// Return type		: void 
// Argument         : LPDRAWITEMSTRUCT lpDrawItemStruct
//##ModelId=3B2040AA0103
void CScrollButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);	

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC) ;

	// copy the rect
	CRect rc = lpDrawItemStruct->rcItem;
	UINT nState;
	if (!(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		nState = m_iDirection == DIR_LEFT ? DFCS_SCROLLLEFT : DFCS_SCROLLRIGHT;
		if (lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			nState |= DFCS_INACTIVE;
		}
		pDC->DrawFrameControl(
			&rc, 
			DFC_SCROLL, 
			nState);
	}
	else
	{
		nState = (m_iDirection == DIR_LEFT ? DFCS_SCROLLLEFT : DFCS_SCROLLRIGHT) | DFCS_PUSHED | DFCS_FLAT;
		pDC->DrawFrameControl(
			&rc, 
			DFC_SCROLL, 
			nState);
	}

	ReleaseDC (pDC);
}

//##ModelId=3B2040AA00F9
BOOL CScrollButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE ;
	UNREFERENCED_PARAMETER(pDC);
}

//##ModelId=3B2040AA00EF
void CScrollButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));

	//CButton::OnLButtonDblClk(nFlags, point);
}
