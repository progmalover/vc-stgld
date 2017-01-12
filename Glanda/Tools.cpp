#include "Tools.h"
#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
	          公用模块
	收录下列函数:
	FileExist - 检查文件是否存在
	ExtractFileName - 从文件路径中解析出文件名
	DrawButton - 画按纽表面及边框
	Div - 按四舍五入原则相除两整数
	Lighten - 将颜色变亮或边暗
*/
BOOL FileExist(const CString &FileName)
{
	WIN32_FIND_DATA FindData;
	HANDLE handle;

	handle = FindFirstFile(FileName, &FindData);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	else
	{
		FindClose(handle);
		return TRUE;
	}
}

CString ExtractFileName(const CString &strPath)
{
	CString FileName(_T(""));
	int dotpos(-1), slashpos(0);
	for (int pos=strPath.GetLength()-1; pos>-1; pos--)
	{
		if ((strPath.GetAt(pos) == '.') && (dotpos == -1))
		{
			dotpos = pos;
		}
		else if (strPath.GetAt(pos) == '\\')
		{
			slashpos = pos;
			break;
		}
	}
	if (dotpos == -1) dotpos = strPath.GetLength() - 1;
	int FileNameLen = dotpos - slashpos - 1;
	if (FileNameLen > 0)
	{
		FileName = strPath.Mid(slashpos + 1, FileNameLen);
	}
	return FileName;
}

int Div(int x, int y)
{
	ASSERT(y != 0);
	float f = (float)x / (float)y;
	int z = (int)(f + 0.5);
	return z;
}

COLORREF Lighten(COLORREF cr, double scale)
{
	ASSERT(scale > 0);

	int r = GetRValue(cr);
	int g = GetGValue(cr);
	int b = GetBValue(cr);

	r = (int)(r * scale + 0.5);
	g = (int)(g * scale + 0.5);
	b = (int)(b * scale + 0.5);

	r = min(r, 255);
	g = min(r, 255);
	b = min(r, 255);

	return RGB(r, g, b);
}

void DrawButton(CDC *pdc, const CRect &rect, COLORREF cr, BOOL Pushed)
{
	ASSERT_VALID(pdc);

	CRect rc(rect);
	if (Pushed)
	{
		pdc->Draw3dRect(rc, Lighten(cr, 0.5), Lighten(cr, 1.5));
		rc.DeflateRect(1, 1, 1, 1);
		pdc->FillSolidRect(rc, cr);
	}
	else
	{
		pdc->Draw3dRect(rc, Lighten(cr, 1.5), Lighten(cr, 0.5));
		rc.DeflateRect(1, 1, 1, 1);
		pdc->FillSolidRect(rc, cr);
	}
}

