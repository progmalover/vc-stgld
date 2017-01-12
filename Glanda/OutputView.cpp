// SampleView.cpp : implementation of the COutputView class
//

#include "stdafx.h"
#include "Resource.h"

#include "OutputDoc.h"
#include "OutputView.h"

#include "ASView.h"

//#include "SWFProxy.h"

//#include "gldDataKeeper.h"
//#include "gldLibrary.h"
//#include "gldSprite.h"
//#include "gldButton.h"
//#include "gldLayer.h"
//#include "gldFrameClip.h"
//#include "gldCharacter.h"
//
//#include "DesignWnd.h"
//#include "my_app.h"
//
//#include "Regexx.h"
//using namespace regexx;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputView

IMPLEMENT_DYNCREATE(COutputView, CCrystalEditView)

BEGIN_MESSAGE_MAP(COutputView, CCrystalEditView)
	//{{AFX_MSG_MAP(COutputView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, OnUpdateEditClearAll)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

DWORD COutputView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// COutputView construction/destruction

COutputView::COutputView()
{
	// TODO: add construction code here

	m_bSelMargin = FALSE;
	m_bShowLineNumber = FALSE;
}

COutputView::~COutputView()
{
}

BOOL COutputView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalEditView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// COutputView diagnostics

#ifdef _DEBUG
void COutputView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void COutputView::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

COutputDoc* COutputView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COutputDoc)));
	return (COutputDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputView message handlers

CCrystalTextBuffer *COutputView::LocateTextBuffer()
{
	return GetDocument()->m_pTextBuffer;
}

void COutputView::OnInitialUpdate() 
{
	CCrystalEditView::OnInitialUpdate();

	/*
	Japanese:
	font face = "‚l‚r ƒSƒVƒbƒN"
	font size = 9

	English
	font face = "Courier New"
	font size = 9
	*/

	CString strFont, strFontSize;
	strFont.LoadString(IDS_FIXED_FONT);
	strFontSize.LoadString(IDS_FIXED_FONT_SIZE);
	int nFontSize = atoi(strFontSize);
	SetFont(strFont, nFontSize);
}

void COutputView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CRect rc;

	GetClientRect(rc);
	ClientToScreen(&rc);

	if (rc.PtInRect(point))
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_OUTPUTVIEW))
		{
			CMenu *pPopup = menu.GetSubMenu(0);
			if (pPopup)
				pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
}

UINT COutputView::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	return DLGC_WANTALLKEYS;
	//return CCrystalEditView::OnGetDlgCode();
}

int COutputView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCrystalEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void COutputView::AppendLog(LPCTSTR fmt, ...)
{
	CString strBuf;

	va_list args;
	va_start(args, fmt);
	strBuf.FormatV(fmt, args);
	va_end (args);

	CCrystalTextBuffer *pBuf = LocateTextBuffer();
	int nLines = pBuf->GetLineCount();
	int nEndLine, nEndChar;

	pBuf->SetReadOnly(FALSE);
	pBuf->InsertText(this, nLines - 1, pBuf->GetLineLength(nLines - 1), strBuf, nEndLine, nEndChar);
	pBuf->SetReadOnly(TRUE);

	CPoint point = GetCursorPos();
	if (point.y == nLines - 1)
		SetCursorPosSimply(CPoint(nEndChar, nEndLine));
}

void COutputView::ClearLog()
{
	CCrystalTextBuffer *pBuf = LocateTextBuffer();

	int nLines = pBuf->GetLineCount();
	if (nLines > 0)
	{
		int len = pBuf->GetLineLength(nLines - 1);
		if (nLines > 1 || len > 0)
		{
			pBuf->SetReadOnly(FALSE);
			pBuf->DeleteText(this, 0, 0, nLines - 1, len);
			SetCursorPosSimply(CPoint(0, 0));
			pBuf->SetReadOnly(TRUE);
		}
	}
}

void COutputView::OnEditClearAll()
{
	// TODO: Add your command handler code here

	ClearLog();
}

void COutputView::OnUpdateEditClearAll(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	CCrystalTextBuffer *pBuf = LocateTextBuffer();
	pCmdUI->Enable(pBuf->GetLineCount() > 1 || (pBuf->GetLineCount() == 0 && pBuf->GetLineLength(0) > 0));
}

void COutputView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CCrystalEditView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here

	UpdateMenuCmdUI(this, pPopupMenu, nIndex, bSysMenu);
}
