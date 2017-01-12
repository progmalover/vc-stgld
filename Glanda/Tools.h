#pragma once
/*
	          公用模块
	收录下列函数:
	ExtractFileName - 从文件路径中解析出文件名
	DrawButton - 画按纽表面及边框
	Div - 按四舍五入原则相除两整数
	Lighten - 将颜色变亮或边暗
*/
BOOL FileExist(const CString &FileName);
CString ExtractFileName(const CString &strPath);
void DrawButton(CDC *pdc, const CRect &rect, COLORREF cr, BOOL Pushed);
COLORREF Lighten(COLORREF cr, double scale = 0.8);
int Div(int x, int y);
