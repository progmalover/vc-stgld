// SampleDoc.cpp : implementation of the COutputDoc class
//

#include "stdafx.h"
#include "Resource.h"

#include "OutputDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputDoc

IMPLEMENT_DYNCREATE(COutputDoc, CDocument)

BEGIN_MESSAGE_MAP(COutputDoc, CDocument)
	//{{AFX_MSG_MAP(COutputDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputDoc construction/destruction

COutputDoc::COutputDoc()
{
	// TODO: add one-time construction code here

	m_pTextBuffer = new COutputTextBuffer(this);
}

COutputDoc::~COutputDoc()
{
	if (m_pTextBuffer)
	{
		m_pTextBuffer->FreeAll();
		delete m_pTextBuffer;
	}
}

BOOL COutputDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_pTextBuffer->InitNew();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// COutputDoc serialization

void COutputDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// COutputDoc diagnostics

#ifdef _DEBUG
void COutputDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COutputDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputDoc commands

void COutputDoc::DeleteContents() 
{
	CDocument::DeleteContents();

	m_pTextBuffer->FreeAll();
}

BOOL COutputDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	return m_pTextBuffer->LoadFromFile(lpszPathName);
}

BOOL COutputDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_pTextBuffer->SaveToFile(lpszPathName);

	return TRUE;	//	Note - we didn't call inherited member!
}
