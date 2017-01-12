#ifndef _MEMDC_H
#define _MEMDC_H
//////////////////////////////////////////////////
// CGlandaMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//           25 feb 98 - fixed minor assertion bug
//
// This class implements a memory Device Context

//##ModelId=3B204087030C
class CGlandaMemDC : public CDC
{
public:

    // constructor sets up the memory DC
	//##ModelId=3B2040870372
    CGlandaMemDC(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if (m_bMemDC)    // Create a Memory DC
        {
            pDC->GetClipBox(&m_rect);
            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
#ifdef _DEBUG
			//if (m_rect.Width() > 0 )
			//{
				//TRACE("m_rect l, r, t, b : %d,%d,%d,%d \n",m_rect.left, m_rect.right, m_rect.top, m_rect.bottom) ;				
			//}
#endif
            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);			
			IntersectClipRect(m_rect) ;
			
        }
        else        // Make a copy of the relevent parts of the current DC for printing
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
    
    // Destructor copies the contents of the mem DC to the original DC
	//##ModelId=3B2040870371
    ~CGlandaMemDC()
    {
        if (m_bMemDC) 
        {    
			//SetViewportOrg(0,0) ;
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);
						  //this, 0, 0, SRCCOPY);

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
        } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
	//##ModelId=3B2040870370
    CGlandaMemDC* operator->() {return this;}
        
    // Allow usage as a pointer
	//##ModelId=3B2040870366
    operator CGlandaMemDC*() {return this;}

private:
	//##ModelId=3B204087035E
    CBitmap  m_bitmap;      // Offscreen bitmap
	//##ModelId=3B204087034A
    CBitmap* m_pOldBitmap;  // bitmap originally found in CGlandaMemDC
	//##ModelId=3B2040870336
    CDC*     m_pDC;         // Saves CDC passed in constructor
	//##ModelId=3B204087032A
    CRect    m_rect;        // Rectangle of drawing area.
	//##ModelId=3B2040870321
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.

};

#endif

