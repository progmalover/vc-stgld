#include "stdafx.h"
#include ".\sysparas.h"

CSysParas::CSysParas(void)
{
	font_default = new CFont;
	Refresh();
}

CSysParas::~CSysParas(void)
{
	font_default->Detach();
	delete font_default;
}

void CSysParas::Refresh()
{
	cl_window = GetSysColor(COLOR_WINDOW);
	cl_text = GetSysColor(COLOR_WINDOWTEXT);
	cl_hilight_text = GetSysColor(COLOR_HIGHLIGHTTEXT);
	cl_hilight = GetSysColor(COLOR_HIGHLIGHT);
	cl_gray_text = GetSysColor(COLOR_GRAYTEXT);
	cx_border = GetSystemMetrics(SM_CXBORDER);
	cy_border = GetSystemMetrics(SM_CYBORDER);
	cx_icon = GetSystemMetrics(SM_CXICON);
	cy_icon = GetSystemMetrics(SM_CYICON);
	font_default->Attach((HFONT)GetStockObject(DEFAULT_GUI_FONT));
}

CSysParas SysParas;