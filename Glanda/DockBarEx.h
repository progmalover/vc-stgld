#pragma once


// CDockBarEx

class CDockBarEx : public CDockBar
{
	DECLARE_DYNAMIC(CDockBarEx)

public:
	CDockBarEx();
	virtual ~CDockBarEx();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
};
