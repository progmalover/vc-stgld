#pragma once
#include "splitter.h"

class CHSplitter :
	public CSplitter
{
public:
	CHSplitter(void);
	virtual ~CHSplitter(void);
	virtual void AdjustPosition();
protected:
	virtual void OnMouseMove(const CPoint &point);
};
