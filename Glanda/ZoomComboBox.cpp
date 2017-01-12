// ZoomComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "ZoomComboBox.h"
#include "my_app.h"

#include "DesignWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ZOOM_MIN	10
#define ZOOM_MAX	1000
/////////////////////////////////////////////////////////////////////////////
// CZoomComboBox

CZoomComboBox::CZoomComboBox()
{
	CSubjectManager::Instance()->GetSubject("ChangeMainMovie2")->Attach(this);
	CSubjectManager::Instance()->GetSubject("Zoom")->Attach(this);
}

CZoomComboBox::~CZoomComboBox()
{
}


BEGIN_MESSAGE_MAP(CZoomComboBox, CComboBox)
	//{{AFX_MSG_MAP(CZoomComboBox)
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelEndOK)
	ON_CONTROL_REFLECT(CBN_SELENDCANCEL, OnSelEndCancel)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomComboBox message handlers

void CZoomComboBox::OnSelEndOK()
{
	int index = GetCurSel();
	if (index == -1)
	{
		OnSelEndCancel();
		return;
	}

	CString strValue;
	GetLBText(index, strValue);

	strValue = strValue.Left(strValue.GetLength() - 1);
	int i = atoi(strValue);
	ASSERT(i >= 10 && i <= 1000);

	my_app.ZoomTo(i);
	
	CDesignWnd::Instance()->SetFocus();
}

void CZoomComboBox::OnSelEndCancel()
{
	SetWindowText(m_strOldValue);
}

void CZoomComboBox::OnSetFocus() 
{
	// TODO: Add your message handler code here

	if (!::IsWindow(m_wndEdit.m_hWnd))
		m_wndEdit.SubclassWindow(GetDlgItem(1001)->GetSafeHwnd());

	GetWindowText(m_strOldValue);

	//ASSERT(!m_strOldValue.IsEmpty());
}

BOOL CZoomComboBox::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	switch (HIWORD(wParam))
	{
	case EN_KEYRETURN:
		OnKeyReturn();
		break;

	case EN_KEYESCAPE:
		OnKeyEscape();
		break;
	}

	return CComboBox::OnCommand(wParam, lParam);
}

void CZoomComboBox::OnKeyReturn()
{
	CString strValue;
	GetWindowText(strValue);

	strValue.TrimLeft();
	strValue.TrimRight();
	int len = strValue.GetLength();
	if (len == 0)
	{
		SetWindowText(m_strOldValue);
		return;
	}

	if (strValue[len - 1] == '%')
		strValue = strValue.Left(len - 1);

	for (int j = 0; j < strValue.GetLength(); j++)
	{
		if (!(strValue[j] >= '0' && strValue[j] <= '9'))
		{
			MessageBeep(0);
			return;
		}
	}

	UINT i;
	VERIFY(sscanf(strValue, "%u", &i) == 1);
	i = max(ZOOM_MIN, min(ZOOM_MAX, i));
	strValue.Format("%d%%", i);

	m_strOldValue = strValue;
	SetWindowText(strValue);

	my_app.ZoomTo(i);

	CDesignWnd::Instance()->SetFocus();
}

void CZoomComboBox::OnKeyEscape()
{
	SetWindowText(m_strOldValue);
	m_wndEdit.SetSel(0, -1);

	CDesignWnd::Instance()->SetFocus();
}

void CZoomComboBox::SetZoom(int nPercent)
{
	
}

void CZoomComboBox::Update(void *pData)
{
	int nPercent = my_app.GetZoomPercent();
	CString strValue;
	strValue.Format("%d%%", nPercent);
	m_strOldValue = strValue;
	SetWindowText(strValue);
}
