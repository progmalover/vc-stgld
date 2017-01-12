#pragma once

#include "ASDocBase.h"

class CASTextBuffer;
class CASParser;

class CASDoc : public CASDocBase
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CASDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CASDoc)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	CASDoc();
	virtual ~CASDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CASParser *m_pParser;
	BOOL m_bModifiedAfterParsing;

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CASDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CASTextBuffer : public CASTextBufferBase
{
public:
	CASTextBuffer(CASDoc *pDoc) : CASTextBufferBase(pDoc)
	{
	}
	virtual BOOL InternalInsertText(CCrystalTextView *pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar)
	{
		((CASDoc *)m_pOwnerDoc)->m_bModifiedAfterParsing = TRUE;
		return CASTextBufferBase::InternalInsertText(pSource, nLine, nPos, pszText, nEndLine, nEndChar);
	}
	virtual BOOL InternalDeleteText(CCrystalTextView *pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos)
	{
		((CASDoc *)m_pOwnerDoc)->m_bModifiedAfterParsing = TRUE;
		return CASTextBufferBase::InternalDeleteText(pSource, nStartLine, nStartPos, nEndLine, nEndPos);
	}
};
