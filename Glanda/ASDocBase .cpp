// SampleDoc.cpp : implementation of the CASDocBase class
//

#include "stdafx.h"
#include "Resource.h"
#include "ASDocBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CASDocBase

IMPLEMENT_DYNCREATE(CASDocBase, CDocument)

BEGIN_MESSAGE_MAP(CASDocBase, CDocument)
	//{{AFX_MSG_MAP(CASDocBase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CASDocBase construction/destruction

CASDocBase::CASDocBase(CCrystalTextBuffer *pTextBuffer)
{
	// TODO: add one-time construction code here

	if (pTextBuffer == NULL)
		pTextBuffer = new CASTextBufferBase(this);
	m_pTextBuffer = pTextBuffer;
}

CASDocBase::~CASDocBase()
{
	if (m_pTextBuffer)
	{
		m_pTextBuffer->FreeAll();
		delete m_pTextBuffer;
	}
}

BOOL CASDocBase::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_pTextBuffer->InitNew();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CASDocBase serialization

void CASDocBase::Serialize(CArchive& ar)
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
// CASDocBase diagnostics

#ifdef _DEBUG
void CASDocBase::AssertValid() const
{
	CDocument::AssertValid();
}

void CASDocBase::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CASDocBase commands

void CASDocBase::DeleteContents() 
{
	CDocument::DeleteContents();

	m_pTextBuffer->FreeAll();
}

BOOL CASDocBase::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	return m_pTextBuffer->LoadFromFile(lpszPathName);
}

BOOL CASDocBase::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_pTextBuffer->SaveToFile(lpszPathName);

	return TRUE;	//	Note - we didn't call inherited member!
}
