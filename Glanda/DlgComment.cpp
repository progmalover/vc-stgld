// DlgComment.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgComment.h"


// CDlgComment dialog

IMPLEMENT_DYNAMIC(CDlgComment, CDialog)
CDlgComment::CDlgComment(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComment::IDD, pParent)
	, m_strComment(_T(""))
{
}

CDlgComment::~CDlgComment()
{
}

void CDlgComment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);
	DDV_MaxChars(pDX, m_strComment, 256);
}


BEGIN_MESSAGE_MAP(CDlgComment, CDialog)
END_MESSAGE_MAP()


// CDlgComment message handlers
