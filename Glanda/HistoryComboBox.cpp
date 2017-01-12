// HistoryComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "HistoryComboBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CHistoryComboBox

CHistoryComboBox::CHistoryComboBox()
: m_nMaxListSize(16)
, m_strKey("")
{
	EnableAutoCompletion(FALSE);
}

CHistoryComboBox::~CHistoryComboBox()
{
}


BEGIN_MESSAGE_MAP(CHistoryComboBox, CAutoCompletionComboBox)
END_MESSAGE_MAP()



// CHistoryComboBox message handlers

void CHistoryComboBox::Init(LPCTSTR lpszKey, UINT nMaxListSize)
{
	m_strKey = lpszKey;
	m_nMaxListSize = nMaxListSize;

	ASSERT(!m_strKey.IsEmpty());
}

BOOL CHistoryComboBox::ClearHistory()
{
	ASSERT(!m_strKey.IsEmpty());

	CString strKey;
	strKey.Format("Software\\%s\\%s\\", AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszProfileName);
	return (AfxGetApp()->DelRegTree(HKEY_CURRENT_USER, strKey + m_strKey) == ERROR_SUCCESS);
}

int CHistoryComboBox::Save()
{
	ASSERT(!m_strKey.IsEmpty());

	ClearHistory();

	int count = GetCount();
	int nWritten = 0;
	for (int i = 0; i < count; i++)
	{
		CString strText;
		GetLBText(i, strText);
		CString strEntry;
		strEntry.Format("#%d", i);
		if (AfxGetApp()->WriteProfileString(m_strKey, strEntry, strText))
			nWritten++;
	}
	AfxGetApp()->WriteProfileInt(m_strKey, "Count", nWritten);

	return nWritten;
}

int CHistoryComboBox::Load()
{
	ASSERT(!m_strKey.IsEmpty());

	UINT count = (UINT)AfxGetApp()->GetProfileInt(m_strKey, "Count", 0);
	int nRead = 0;
	for (UINT i = 0; i < count && i < m_nMaxListSize; i++)
	{
		CString strEntry;
		strEntry.Format("#%d", i);
		CString strText = AfxGetApp()->GetProfileString(m_strKey, strEntry, NULL);
		if (!strText.IsEmpty())
		{
			AddString(strText);
			nRead++;
		}
	}
	return nRead;
}

void CHistoryComboBox::AddCurrentText(BOOL bCaseSensitive)
{
	CString strText;
	GetWindowText(strText);

	for (int i = 0; i < GetCount(); i++)
	{
		CString strLBText;
		GetLBText(i, strLBText);
		BOOL ret = bCaseSensitive ? _tcscmp(strText, strLBText) : _tcsicmp(strText, strLBText);
		if (ret == 0)
		{
			DeleteString(i);
			break;
		}
	}

	if (!strText.IsEmpty())
	{
		InsertString(0, strText);
		SetCurSel(0);
	}

	while (true)
	{
		int count = GetCount();
		if ((UINT)count <= m_nMaxListSize)
			break;
		DeleteString(count - 1);
	}
}
