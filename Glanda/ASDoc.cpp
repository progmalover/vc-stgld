// SampleDoc.cpp : implementation of the CASDoc class
//

#include "stdafx.h"
#include "Resource.h"

#include "ASDoc.h"
#include "CodeTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CASDoc

IMPLEMENT_DYNCREATE(CASDoc, CASDocBase)

BEGIN_MESSAGE_MAP(CASDoc, CASDocBase)
	//{{AFX_MSG_MAP(CASDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CASDoc construction/destruction

CASDoc::CASDoc() : CASDocBase(new CASTextBuffer(this))
{
	// TODO: add one-time construction code here

	m_pParser = new CASParser();
	m_bModifiedAfterParsing = TRUE;
}

CASDoc::~CASDoc()
{
	delete m_pParser;
}

/////////////////////////////////////////////////////////////////////////////
// CASDoc serialization

/////////////////////////////////////////////////////////////////////////////
// CASDoc diagnostics

#ifdef _DEBUG
void CASDoc::AssertValid() const
{
	CASDocBase::AssertValid();
}

void CASDoc::Dump(CDumpContext& dc) const
{
	CASDocBase::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CASDoc commands

