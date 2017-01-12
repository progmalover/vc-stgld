// DlgSelectScene.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgSelectScene.h"
#include ".\dlgselectscene.h"
#include "GlandaDoc.h"
#include "gldScene2.h"
#include "SceneListCtrl.h"
#include "VisualStylesXP.h"

IMPLEMENT_DYNAMIC(CStaticScene, CStatic)

BEGIN_MESSAGE_MAP(CStaticScene, CStatic)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

CStaticScene::CStaticScene()
: m_pScene(NULL)
{
}

void CStaticScene::OnPaint()
{
	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	if (m_pScene != NULL)
	{
		rect.DeflateRect(2, 2, 2, 2);
		CSceneListCtrl::RenderThumb(dc, rect, m_pScene);
	}
	else
	{
		dc.FillSolidRect(rect, GetSysColor(COLOR_BTNFACE));
	}
}

void CStaticScene::OnNcPaint()
{
	Default();

	DWORD dwExStyle = GetExStyle();

	if ((dwExStyle & WS_EX_CLIENTEDGE) || (dwExStyle & WS_EX_STATICEDGE))
	{
		if (g_xpStyle.IsAppThemed())
		{
			HTHEME hTheme = g_xpStyle.OpenThemeData(m_hWnd, L"COMBOBOX");
			if (hTheme)
			{
				COLORREF crBorder;

				HRESULT hr = g_xpStyle.GetThemeColor(hTheme, 0, 0, TMT_BORDERCOLOR, &crBorder);
				g_xpStyle.CloseThemeData(hTheme);

				if (SUCCEEDED(hr))
				{
					CDC *pDC = GetWindowDC();
					if (pDC)
					{
						CRect rc;
						GetWindowRect(&rc);
						rc.OffsetRect(-rc.left, -rc.top);

						CBrush brBorder(crBorder);
						pDC->FrameRect(&rc, &brBorder);

						if (dwExStyle & WS_EX_CLIENTEDGE)
						{
							rc.InflateRect(-1, -1);

							CBrush brWindow(::GetSysColor(COLOR_WINDOW));
							pDC->FrameRect(&rc, &brWindow);
						}

						ReleaseDC(pDC);
					}
				}
			}
		}
	}
}
// CDlgSelectScene dialog

IMPLEMENT_DYNAMIC(CDlgSelectScene, CDialog)
CDlgSelectScene::CDlgSelectScene(CWnd* pParent /*=NULL*/, const CString &strCaption /*= ""*/)
	: CDialog(CDlgSelectScene::IDD, pParent)
	, m_strCaption(strCaption)
{
}

CDlgSelectScene::~CDlgSelectScene()
{
}

void CDlgSelectScene::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_SCENE, m_lstScene);
	DDX_Control(pDX, IDC_STATIC_THUMBNAIL, m_sttScene);
}


BEGIN_MESSAGE_MAP(CDlgSelectScene, CDialog)	
	ON_LBN_SELCHANGE(IDC_LIST_SCENE, OnLbnSelchangeListScene)
END_MESSAGE_MAP()


// CDlgSelectScene message handlers

BOOL CDlgSelectScene::OnInitDialog()
{
	CDialog::OnInitDialog();

	// set window caption
	if (m_strCaption.GetLength() > 0)
	{
		SetWindowText(m_strCaption);
	}

	// list scenes
	CString strTitle;
	int num = 1;
	gldMainMovie2 *mainMovie = _GetMainMovie2();
	for (GSCENE2_LIST::iterator its = mainMovie->m_sceneList.begin()
		; its != mainMovie->m_sceneList.end(); ++its)
	{
		gldScene2 *scene = *its;
		strTitle.Format("%d. %s", num++, scene->m_name.c_str());
		int item = m_lstScene.AddString(strTitle);
		m_lstScene.SetItemData(item, (DWORD_PTR)scene);
	}

	// set selected scene
	if (m_lstScene.GetCount() > 0)
	{
		m_lstScene.SetCurSel(0);
		m_sttScene.m_pScene = (gldScene2 *)m_lstScene.GetItemData(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectScene::OnLbnSelchangeListScene()
{
	// update scene thumbnail
	int sel = m_lstScene.GetCurSel();
	if (sel > -1)
	{
		m_sttScene.m_pScene = (gldScene2 *)m_lstScene.GetItemData(sel);
	}
	else
	{
		m_sttScene.m_pScene = NULL;
	}
	m_sttScene.Invalidate(FALSE);
}
