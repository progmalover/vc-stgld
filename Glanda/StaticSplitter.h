#pragma once


// CStaticSplitter

class CStaticSplitter : public CStatic
{
	DECLARE_DYNAMIC(CStaticSplitter)

public:
	CStaticSplitter();
	virtual ~CStaticSplitter();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


