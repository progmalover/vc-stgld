#pragma once

#include "GGraphics.h"
#include <vector>

#define LGF_CURRENTHANDLECHANGED	0x1000	// index of current handle changed
#define LGF_HANDLEPOSCHANGED		0x1001	// pos of current handle changed

// CStaticLinearGradientFill

class CGradientColor
{
public:
	CGradientColor() : color(0, 0, 0, 0)
	{
		ratio = 0;
		current = false;
		visible = true;
	}
	~CGradientColor()
	{
	}

	TColor color;
	BYTE ratio;
	bool current;
	bool visible;
};

typedef std::vector <CGradientColor *> gradient_color_list;

class CStaticLinearGradientFill : public CStatic
{
	DECLARE_DYNAMIC(CStaticLinearGradientFill)

public:
	CStaticLinearGradientFill();
	virtual ~CStaticLinearGradientFill();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
protected:
	TGraphicsDevice m_device;
	gradient_color_list m_list;
	int HitTest(CPoint point);
	int AddHandle(int nPos);
	void RemoveHandle(int index);
	BYTE PosToRatio(CRect &rcClient, int nPos);
	int RatioToPos(CRect &rcClient, BYTE ratio);
	void DrawHandle(CDC &dc, CGradientColor *pgc);
	CRect GetSampleRect(CRect &rcClient);
	int ClientToSample(CRect &rcClient, int nPos);
	int SampleToClient(CRect &rcClient, int nPos);
	void Track(int index, CPoint point);
	void SetCurrentHandle(int index);
public:
	int GetMargin();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void GetFillStyle(TGradientFillStyle *pFillStyle);
	int GetCurrentHandle();
	void SetFillStyle(TGradientFillStyle *pFillStyle);
	TColor GetCurrentColor();
	void SetCurrentColor(TColor &color);

protected:
	virtual void PreSubclassWindow();
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	inline const gradient_color_list& GetGradientColors(){return m_list;}
	afx_msg void OnEnable(BOOL bEnable);
};


