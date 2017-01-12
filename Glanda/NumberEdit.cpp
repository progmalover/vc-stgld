// NumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "NumberEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TYPE_INVALID	0
#define TYPE_INT		1
#define TYPE_FLOAT		2

extern UINT WM_SLIDERPOSCHANGED;
extern UINT WM_SLIDERENDTRACKING;

DEFINE_REGISTERED_MESSAGE(WM_NUMBEREDIT_ERROR)

double GetDlgItemFloat(HWND hDlg, int nID, BOOL *pTranslated)
{
	CString strText;
	CWnd::FromHandle(hDlg)->GetDlgItemText(nID, strText);
	TCHAR *p = NULL;
	double fValue = strtod(strText, &p);
	if (pTranslated)
		*pTranslated = (*p == '\0');
	return fValue;
}

void SetDlgItemFloat(HWND hDlg, int nID, double fValue, int nPrecision, BOOL bRemoveTrailZero)
{
	ASSERT(nPrecision > 0);
	CString strFormat;
	strFormat.Format("%%.%df", nPrecision);
	CString strText;
	strText.Format(strFormat, fValue);
	if (bRemoveTrailZero)
		RemoveFloatTrailZero(strText);
	SetDlgItemText(hDlg, nID, strText);
}

void RemoveFloatTrailZero(CString &strText)
{
	// must not be nnn. or .nnn
	int len = strText.GetLength();
	int nPos = strText.ReverseFind('.');
	if (nPos > 0 && nPos < len - 1)
	{
		int count = 0;
		for (int i = len - 1; i >= 2; i--)
		{
			if (strText[i] != '0')
				break;
			count++;
		}
		if (count > 0)
		{
			if (strText[len - count - 1] == '.')
				count++;
			strText = strText.Left(len - count);
		}
	}
}

// CNumberEdit

IMPLEMENT_DYNAMIC(CNumberEdit, CSmartEdit)
CNumberEdit::CNumberEdit() : 
	m_nType(TYPE_INVALID), 
	m_bAllowEmpty(FALSE), 

	m_nDefault(0), 
	m_nMin(0), 
	m_nMax(INT_MAX), 

	m_nPrecision(1), 
	m_fMin(0.0), 
	m_fMax(100.0), 
	m_fDefault(0.0)
{
}

CNumberEdit::~CNumberEdit()
{
}


BEGIN_MESSAGE_MAP(CNumberEdit, CSmartEdit)
	ON_REGISTERED_MESSAGE(WM_SLIDERPOSCHANGED, OnSliderPosChanged)
	ON_REGISTERED_MESSAGE(WM_SLIDERENDTRACKING, OnSliderEndTracking)
	ON_REGISTERED_MESSAGE(WM_NUMBEREDIT_ERROR, OnNumberEditError)
END_MESSAGE_MAP()



// CNumberEdit message handlers

BOOL CNumberEdit::ValidateText()
{
	HWND hDlg = ::GetParent(m_hWnd);

	CString strText;
	GetWindowText(strText);

	strText.Trim(' ');

	// check if is empty
	if (strText.IsEmpty())
	{
		if (m_bAllowEmpty)
			return TRUE;

		// if empty, use the default value
		if (m_nType == TYPE_INT)
		{
			::SetDlgItemInt(hDlg, GetDlgCtrlID(), m_nDefault, TRUE);
			if (m_bPrompt)
				ReportError(IDS_E_INTEGER_EMPTY2, m_nMin, m_nMax);
		}
		else
		{
			::SetDlgItemFloat(hDlg, GetDlgCtrlID(), m_fDefault, m_nPrecision, TRUE);
			if (m_bPrompt)
				ReportError(IDS_E_FLOAT_EMPTY2, m_fMin, m_fMax);
		}

		return FALSE;
	}

	// check for invalid chars
	BOOL bTrans;
	if (m_nType == TYPE_INT)
	{
		::GetDlgItemInt(hDlg, GetDlgCtrlID(), &bTrans, TRUE);
		if (!bTrans)
		{
			::SetDlgItemInt(hDlg, GetDlgCtrlID(), m_nDefault, TRUE);

			if (m_bPrompt)
				ReportError(IDS_E_NUMBER_INVALID1, strText);
		
			return FALSE;
		}
	}
	else
	{
		::GetDlgItemFloat(hDlg, GetDlgCtrlID(), &bTrans);
		if (!bTrans)
		{
			::SetDlgItemFloat(hDlg, GetDlgCtrlID(), m_fDefault, m_nPrecision, TRUE);

			if (m_bPrompt)
				ReportError(IDS_E_NUMBER_INVALID1, strText);
		
			return FALSE;
		}
	}

	// check if the value is in range
	if (m_nType == TYPE_INT)
	{
		int nValue = ::GetDlgItemInt(hDlg, GetDlgCtrlID(), &bTrans, TRUE);
		ASSERT(bTrans);
		if (nValue < m_nMin || nValue > m_nMax)
		{
			// use closet value
			if (nValue < m_nMin)
				::SetDlgItemInt(hDlg, GetDlgCtrlID(), m_nMin, TRUE);
			else
				::SetDlgItemInt(hDlg, GetDlgCtrlID(), m_nMax, TRUE);
			
			if (m_bPrompt)
				ReportError(IDS_E_INTEGER_OUT_OF_RANGE2, m_nMin, m_nMax);

			return FALSE;
		}
	}
	else
	{
		double fValue = ::GetDlgItemFloat(hDlg, GetDlgCtrlID(), &bTrans);
		ASSERT(bTrans);
		if (fValue < m_fMin || fValue > m_fMax)
		{
			// use closet value
			if (fValue < m_fMin)
				::SetDlgItemFloat(hDlg, GetDlgCtrlID(), m_fMin, m_nPrecision, TRUE);
			else
				::SetDlgItemFloat(hDlg, GetDlgCtrlID(), m_fMax, m_nPrecision, TRUE);
			
			if (m_bPrompt)
				ReportError(IDS_E_FLOAT_OUT_OF_RANGE2, m_fMin, m_fMax);

			return FALSE;
		}
	}

	return TRUE;
}

void CNumberEdit::SetRange(int nMin, int nMax, int nDefault, BOOL bAllowEmpty, BOOL bPrompt)
{
	m_nType = TYPE_INT;

	m_nMin = nMin;
	m_nMax = nMax;
	m_nDefault = nDefault;
	m_bAllowEmpty = bAllowEmpty;
	m_bPrompt = bPrompt;

	int nMaxAbs = max(abs(nMin), abs(nMax));
	int len = 1;
	while (true)
	{
		nMaxAbs /= 10;
		if (nMaxAbs == 0)
			break;
		len++;
	}

	LimitText(len + 1);	// signed
}

int CNumberEdit::GetValueInt()
{
	CString strText;
	GetWindowText(strText);
	strText = strText.Trim(" ");
	if(strText.IsEmpty())
		return m_nDefault;
	else
		return min(max(m_nMin, atoi(strText)), m_nMax);
}

LRESULT CNumberEdit::OnSliderPosChanged(WPARAM wp, LPARAM lp)
{
	int nPos = (int)wp;

	CString strNew;
	strNew.Format("%d", nPos);

	CString strOld;
	GetWindowText(strOld);
	if (strNew.Compare(strOld) != 0)
	{
		SetWindowText(strNew);
		UpdateWindow();
		m_bChanged = TRUE;
	}

	return 1;
}

LRESULT CNumberEdit::OnSliderEndTracking(WPARAM wp, LPARAM lp)
{
	if (m_bChanged)
		HandleModified();
	return 1;
}

void CNumberEdit::SetRange(int nPrecision, double fMin, double fMax, double fDefault, BOOL bAllowEmpty, BOOL bPrompt)
{
	m_nType = TYPE_FLOAT;

	m_nPrecision = nPrecision;
	m_fMin = fMin;
	m_fMax = fMax;
	m_fDefault = fDefault;
	m_bAllowEmpty = bAllowEmpty;
	m_bPrompt = bPrompt;

	int nMaxAbs = max(abs((int)fMin), abs((int)fMax));
	int len = 1;
	while (true)
	{
		nMaxAbs /= 10;
		if (nMaxAbs == 0)
			break;
		len++;
	}

	LimitText(len + 1 + nPrecision);
}

double CNumberEdit::GetValueFloat()
{
	BOOL bTrans;
	double fValue = GetDlgItemFloat(::GetParent(m_hWnd), GetDlgCtrlID(), &bTrans);
	if (bTrans)
		return fValue;		
	return m_fDefault;
}

void CNumberEdit::SaveDefaultValue()
{
	m_bChanged = FALSE;

	HWND hDlg = ::GetParent(m_hWnd);
	BOOL bTrans;
	if (m_nType == TYPE_INT)
	{
		m_nDefault = ::GetDlgItemInt(hDlg, GetDlgCtrlID(), &bTrans, TRUE);
		//TRACE1("m_nDefalue=%d\n", m_nDefault);
	}
	else
	{
		m_fDefault = ::GetDlgItemFloat(hDlg, GetDlgCtrlID(), &bTrans);
		//TRACE1("m_fDefalue=%f\n", m_fDefault);
	}
}

void CNumberEdit::ReportError(UINT fmt, ...)
{
	CString strFormat;
	if (strFormat.LoadString(fmt))
	{
		va_list args;
		va_start(args, fmt);
		m_strError.FormatV(strFormat, args);
		va_end (args);

		PostMessage(WM_NUMBEREDIT_ERROR, 0, 0L);
	}
}

LRESULT CNumberEdit::OnNumberEditError(WPARAM wp, LPARAM lp)
{
	if (!m_strError.IsEmpty())
		AfxMessageBox(m_strError);
	return 0;
}
