#pragma once

#include "CCrystalTextBuffer.h"

class CASTextBufferBase;

class CASDocBase : public CDocument
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CASDocBase)

// Attributes
public:
	CCrystalTextBuffer *m_pTextBuffer;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CASDocBase)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	CASDocBase(CCrystalTextBuffer *pTextBuffer = NULL);
	virtual ~CASDocBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CASDocBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CASTextBufferBase : public CCrystalTextBuffer
{
protected:
	CDocument *m_pOwnerDoc;
public:
	CASTextBufferBase(CASDocBase *pDoc)
	{
		m_pOwnerDoc = pDoc;
	};
	virtual void SetModified(BOOL bModified = TRUE)
	{
		TRACE1("CASTextBufferBase::SetModified(%d)\n", bModified);
		m_pOwnerDoc->SetModifiedFlag(bModified);
	};
};

