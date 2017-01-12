// DHTMLMenuView.cpp : implementation of the CGlandaView class
//

#include "stdafx.h"
#include "Glanda.h"

#include "GlandaDoc.h"
#include "GlandaView.h"

#include "DesignWnd.h"

#include "gldDataKeeper.h"
#include "SWFSpriteTag.h"
#include "SWFFrame.h"
#include "SWFCharacter.h"

#include "gldSWFEngine.h"
#include "gldColor.h"
#include "gldMovieClip.h"

#include "gld_graph.h"
#include "TransAdaptor.h"

#include <map>

#include "my_app.h"

#include "RuleWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MARGIN	0

// CGlandaView

IMPLEMENT_DYNCREATE(CGlandaView, CView)
BEGIN_MESSAGE_MAP(CGlandaView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SIZE()

	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_RULERS, OnViewRulers)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RULERS, OnUpdateViewRulers)
END_MESSAGE_MAP()

// CGlandaView construction/destruction

CGlandaView::CGlandaView() : 
	m_bInit(false)
{
	// TODO: add construction code here
}

CGlandaView::~CGlandaView()
{
	CRuleWnd::ReleaseInstance();
}

BOOL CGlandaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPCHILDREN;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	static TCHAR strClassName[] = "SothinkViewClass";
	WNDCLASS wndclass;
	wndclass.style = CS_DBLCLKS;//|CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc = ::DefWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = AfxGetInstanceHandle();
	wndclass.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = strClassName;

	if (AfxRegisterClass(&wndclass))
		cs.lpszClass = strClassName;

	return CView::PreCreateWindow(cs);
}

// CGlandaView drawing

void CGlandaView::OnDraw(CDC* pDC)
{
	CGlandaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here

#if (MARGIN != 0)
	CRect rc;
	GetClientRect(&rc);

	if (::GetFocus() == CDesignWnd::Instance()->m_hWnd)
	{
		CBrush br;
		br.CreateSysColorBrush(COLOR_HIGHLIGHT);

		rc.InflateRect(-1, -1);
		pDC->FrameRect(&rc, &br);
	}
#endif
}

// CGlandaView printing

BOOL CGlandaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGlandaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGlandaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CGlandaView diagnostics

#ifdef _DEBUG
void CGlandaView::AssertValid() const
{
	CView::AssertValid();
}

void CGlandaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGlandaDoc* CGlandaView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGlandaDoc)));
	return (CGlandaDoc*)m_pDocument;
}
#endif //_DEBUG


// CGlandaView message handlers

#include "DeferWindowPos.h"
#include ".\GlandaView.h"

void CGlandaView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	RepositionChildren();
}

void CGlandaView::RepositionChildren()
{
	if (::IsWindow(m_hWnd))
	{
		CRect rc;
		GetClientRect(&rc);
		int cx = rc.Width();
		int cy = rc.Height();
		if (CRuleWnd::Instance()->ShowRule())
		{
			int rs = CRuleWnd::Instance()->GetRuleSize();

			CDeferWindowPos dwp;
			dwp.BeginDeferWindowPos();
			dwp.DeferWindowPos(CRuleWnd::Instance()->m_hWnd, 0, 0, 0, cx, cy, SWP_NOZORDER);
			dwp.DeferWindowPos(CDesignWnd::Instance()->m_hWnd, wndTop.m_hWnd,
				rs, rs, cx - rs, cy - rs, 0);
			dwp.EndDeferWindowPos();
		}
		else
		{
			CDeferWindowPos dwp;
			dwp.BeginDeferWindowPos();
			dwp.DeferWindowPos(CRuleWnd::Instance()->m_hWnd, 0, 0, 0, cx, cy, SWP_NOZORDER);
			dwp.DeferWindowPos(CDesignWnd::Instance()->m_hWnd, wndTop.m_hWnd,
				0, 0, cx, cy, 0);
			dwp.EndDeferWindowPos();
		}
	}
}

void CGlandaView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	if (!m_bInit)
	{
		//CSnapFloatingWnd::Instance()->AddWnd(this, -2, TRUE);
		m_bInit = true;
	}

	SetFocus();
}

void CGlandaView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	CDesignWnd::Instance()->SetFocus();
}

int CGlandaView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	CRect rc;
	GetClientRect(&rc);
	
	CRuleWnd::Instance()->Create(NULL, NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, rc, this, 1);
	CDesignWnd::Instance()->Create(WS_CHILD | WS_VISIBLE, rc, this, 2);
	CDesignWnd::Instance()->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	CRuleWnd::Instance()->SetSibling(CDesignWnd::Instance());

	return 0;
}

BOOL CGlandaView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	//if (CDesignWnd::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CGlandaView::OnViewRulers()
{
	// TODO: Add your command handler code here
	my_app.ShowAuxLine(!my_app.ShowAuxLine());
	CRuleWnd::Instance()->ShowRule(my_app.ShowAuxLine());

	RepositionChildren();
}

void CGlandaView::OnUpdateViewRulers(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(my_app.ShowAuxLine() ? 1 : 0);
}

void CGlandaView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class

	UNUSED(pActivateView);
	//CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
