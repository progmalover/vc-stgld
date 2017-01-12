#pragma once


// CStaticBitmap

class CStaticBitmap : public CStatic
{
	DECLARE_DYNAMIC(CStaticBitmap)

public:
	CStaticBitmap();
	virtual ~CStaticBitmap();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
};


