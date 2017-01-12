#pragma once

class CSysParas
{
public:
	CSysParas(void);
	~CSysParas(void);

	void Refresh();

	COLORREF cl_window;
	COLORREF cl_text;
	COLORREF cl_hilight_text;
	COLORREF cl_hilight;
	COLORREF cl_gray_text;
	int cx_border;
	int cy_border;
	int cx_icon;
	int cy_icon;
	CFont *font_default;
};

extern CSysParas SysParas;