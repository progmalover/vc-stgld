// DlgToolOptionTextTool.cpp : implementation file
//

#include "stdafx.h"
#include "DlgToolOptionTextTool.h"
#include "my_app.h"
#include "CategoryWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CDlgToolOptionTextTool dialog

IMPLEMENT_SINGLETON(CDlgToolOptionTextTool)
CDlgToolOptionTextTool::CDlgToolOptionTextTool(CWnd *pParentWnd)
: CDlgTextTool(CDlgToolOptionTextTool::IDD, pParentWnd)
{
}

CDlgToolOptionTextTool::~CDlgToolOptionTextTool()
{
}


BEGIN_MESSAGE_MAP(CDlgToolOptionTextTool, CDlgTextTool)
END_MESSAGE_MAP()


// CDlgToolOptionTextTool message handlers

IMPLEMENT_OBSERVER(CDlgToolOptionTextTool, TextInplaceEdit)
{
	if ((int)pData == 1)
		CCategoryWnd::Instance()->SetActivePage(INDEX_TOOLS_PAGE);
	HandleSelChange();
}

IMPLEMENT_OBSERVER(CDlgToolOptionTextTool, TextSelChange)
{
	HandleSelChange();
}

SETextToolEx* CDlgToolOptionTextTool::GetTextTool()
{
	return (SETextToolEx *)my_app.Tools()[IDT_TEXTTOOLEX - IDT_FIRST];
}

void CDlgToolOptionTextTool::OnTextChanged()
{
}