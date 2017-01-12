// DlgSelectTarget.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "DlgSelectTarget.h"
#include "gldDataKeeper.h"
#include "gldCharacter.h"
#include <list>



// CDlgSelectTarget dialog

CDlgSelectTarget::CDlgSelectTarget(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgSelectTarget::IDD, pParent)
	, m_strTarget(_T(""))
{
	m_bShowNamedInstancesOnly = AfxGetApp()->GetProfileInt("Insert Target Path", "Show Named Instances Only", FALSE);
	m_nPathType = AfxGetApp()->GetProfileInt("Insert Target Path", "Path Type", 1 /* absolute */);
}

CDlgSelectTarget::~CDlgSelectTarget()
{
	AfxGetApp()->WriteProfileInt("Insert Target Path", "Show Named Instances Only", m_bShowNamedInstancesOnly);
	AfxGetApp()->WriteProfileInt("Insert Target Path", "Path Type", m_nPathType);
}

void CDlgSelectTarget::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Text(pDX, IDC_EDIT_TARGET, m_strTarget);
	DDX_Radio(pDX, IDC_RADIO_RELATIVE, m_nPathType);
	DDX_Check(pDX, IDC_CHECK_SHOW_NAMED_ONLY, m_bShowNamedInstancesOnly);
}


BEGIN_MESSAGE_MAP(CDlgSelectTarget, CResizableDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE, OnTvnSelchangingTree)
	ON_BN_CLICKED(IDC_CHECK_SHOW_NAMED_ONLY, OnBnClickedCheckShowNamedOnly)
	ON_BN_CLICKED(IDC_RADIO_ABSOLUTE, OnBnClickedRadioAbsolute)
	ON_BN_CLICKED(IDC_RADIO_RELATIVE, OnBnClickedRadioRelative)
END_MESSAGE_MAP()


// CDlgSelectTarget message handlers

BOOL CDlgSelectTarget::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	AddAnchor(IDC_EDIT_TARGET, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDOK, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_TREE, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_STATIC_PATH_TYPE, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_RADIO_RELATIVE, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_RADIO_ABSOLUTE, BOTTOM_LEFT, BOTTOM_LEFT);

	EnableSaveRestore("Insert Target Path");

	m_tree.m_bShowNamedInstancesOnly = m_bShowNamedInstancesOnly;

	m_tree.PopulateTree(TVI_ROOT, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectTarget::OnTvnSelchangingTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	// TODO: Add your control notification handler code here

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	gldCharacterKey *pCK = (gldCharacterKey *)m_tree.GetItemData(hItem);
	if (pCK != NULL)	// not the _root
	{
		if (pCK->m_name.empty())
		{
			AfxMessageBox(IDS_PROMPT_TARGET_REQUIRE_INSTANCE_NAME);
			*pResult = 1;
			return;
		}

		while (hItem = m_tree.GetParentItem(hItem))
		{
			gldCharacterKey *pCK = (gldCharacterKey *)m_tree.GetItemData(hItem);
			if (pCK && pCK->m_name.empty())
			{
				AfxMessageBox(IDS_PROMPT_PATH_REQUIRE_INSTANCE_NAME);
				*pResult = 1;
				return;
			}
		}
	}

	*pResult = 0;
}

void CDlgSelectTarget::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// TODO: Add your control notification handler code here

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CalcTargetPath(hItem);

	pNMTreeView->itemOld.hItem;

	*pResult = 0;
}

typedef std::list<HTREEITEM> ITEM_LIST;
void CDlgSelectTarget::CalcTargetPath(HTREEITEM hItem)
{
	CString strTarget;
	if (m_nPathType == 0)	// relative
	{
		ITEM_LIST items;
		while (hItem)
		{
			items.push_back(hItem);
			hItem = m_tree.GetParentItem(hItem);
		}

		hItem = m_tree.m_hItemCurrentMovieClip;
		while (hItem)
		{
			if (strTarget.IsEmpty())
				strTarget = "this";
			else
				strTarget += "._parent";
			for (ITEM_LIST::reverse_iterator it = items.rbegin(); it != items.rend(); it++)
			{
				if (hItem == *it)	// found same parent item
				{
					for (it++; it != items.rend(); it++)
					{
						strTarget += "." + m_tree.GetItemText(*it);
					}
					goto _found;
				}
			}
			hItem = m_tree.GetParentItem(hItem);
		}

		ASSERT(FALSE);
_found:
		;
	}
	else
	{
		while (hItem)
		{
			HTREEITEM hParentItem = m_tree.GetParentItem(hItem);
			CString strNode = hParentItem == NULL ? "_root" : m_tree.GetItemText(hItem);
			if (!strTarget.IsEmpty())
				strTarget = "." + strTarget;
			strTarget = strNode + strTarget;
			hItem = hParentItem;
		}
	}

	SetDlgItemText(IDC_EDIT_TARGET, strTarget);
}

void CDlgSelectTarget::OnBnClickedCheckShowNamedOnly()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	m_tree.m_bShowNamedInstancesOnly = m_bShowNamedInstancesOnly;
	m_tree.PopulateTree(TVI_ROOT, NULL);
}

void CDlgSelectTarget::OnBnClickedRadioAbsolute()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	HTREEITEM hItem = m_tree.GetSelectedItem();
	if (hItem)
		CalcTargetPath(hItem);
}

void CDlgSelectTarget::OnBnClickedRadioRelative()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	HTREEITEM hItem = m_tree.GetSelectedItem();
	if (hItem)
		CalcTargetPath(hItem);
}
