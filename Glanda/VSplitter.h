// VSplitter.h: interface for the CVSplitter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSPLITTER_H__684DE617_A854_4E8A_ABE7_9899C4742239__INCLUDED_)
#define AFX_VSPLITTER_H__684DE617_A854_4E8A_ABE7_9899C4742239__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Splitter.h"

class CVSplitter : public CSplitter  
{
public:
	virtual void AdjustPosition();
	CVSplitter();
	virtual ~CVSplitter();

protected:
	virtual void OnMouseMove(const CPoint &point);
};

#endif // !defined(AFX_VSPLITTER_H__684DE617_A854_4E8A_ABE7_9899C4742239__INCLUDED_)
