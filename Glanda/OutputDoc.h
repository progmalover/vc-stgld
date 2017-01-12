// ASDoc.h : interface of the COutputDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEDOC_H__B1B69ED1_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_)
#define AFX_SAMPLEDOC_H__B1B69ED1_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CCrystalTextBuffer.h"

class COutputTextBuffer;

class COutputDoc : public CDocument
{
protected: // create from serialization only
	DECLARE_DYNCREATE(COutputDoc)

// Attributes
public:
	COutputTextBuffer *m_pTextBuffer;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	COutputDoc();
	virtual ~COutputDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COutputDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class COutputTextBuffer : public CCrystalTextBuffer
{
private:
	COutputDoc *m_pOwnerDoc;
public:
	COutputTextBuffer(COutputDoc *pDoc) { m_pOwnerDoc = pDoc; };
	virtual void SetModified(BOOL bModified = TRUE)
		{ m_pOwnerDoc->SetModifiedFlag(bModified); };
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEDOC_H__B1B69ED1_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_)
