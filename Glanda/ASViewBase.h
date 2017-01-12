#pragma once

#include "CCrystalEditView.h"

class CASDocBase;

class CASViewBase : public CCrystalEditView
{
protected: // create from serialization only
	CASViewBase();
	DECLARE_DYNCREATE(CASViewBase)

// Attributes
public:
	CASDocBase* GetDocument();

	virtual CCrystalTextBuffer *LocateTextBuffer();

protected:
	virtual DWORD ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);
	BOOL IsKeyword(LPCTSTR pszChars, int nLength);
	BOOL IsNumber(LPCTSTR pszChars, int nLength);;
	BOOL IsVarChar(char c);


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CASViewBase)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CASViewBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void GetText(CString &strText);

// Generated message map functions
protected:
	//{{AFX_MSG(CASViewBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SampleView.cpp
inline CASDocBase* CASViewBase::GetDocument()
   { return (CASDocBase*)m_pDocument; }
#endif
