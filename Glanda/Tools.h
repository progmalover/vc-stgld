#pragma once
/*
	          ����ģ��
	��¼���к���:
	ExtractFileName - ���ļ�·���н������ļ���
	DrawButton - ����Ŧ���漰�߿�
	Div - ����������ԭ�����������
	Lighten - ����ɫ������߰�
*/
BOOL FileExist(const CString &FileName);
CString ExtractFileName(const CString &strPath);
void DrawButton(CDC *pdc, const CRect &rect, COLORREF cr, BOOL Pushed);
COLORREF Lighten(COLORREF cr, double scale = 0.8);
int Div(int x, int y);
