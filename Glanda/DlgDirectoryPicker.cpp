// DlgDirectoryPicker.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgDirectoryPicker.h"
#include "Utils.h"
#include "DlgNewFolder.h"
#include "filepath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDirectoryPicker dialog


CDlgDirectoryPicker::CDlgDirectoryPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDirectoryPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDirectoryPicker)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bPopulating = FALSE;
}


void CDlgDirectoryPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDirectoryPicker)
	DDX_Control(pDX, IDC_TREE, m_wndTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDirectoryPicker, CDialog)
	//{{AFX_MSG_MAP(CDlgDirectoryPicker)
	ON_BN_CLICKED(IDC_BUTTON_NEW_FOLDER, OnNewFolder)
	ON_BN_CLICKED(IDC_BUTTON_LOCATE, OnLocate)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelChangedTree)
	ON_EN_CHANGE(IDC_EDIT_PATH, OnChangeEditPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDirectoryPicker message handlers

void CDlgDirectoryPicker::OnNewFolder() 
{
	// TODO: Add your control notification handler code here
	
	CDlgNewFolder dlg;

	if (!m_wndTree.GetSelectedFolderPath(dlg.m_strCurrentFolder))
	{
		ASSERT(FALSE);
		return;
	}

	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.m_strCurrentFolder;
		if (strPath.Right(1) != "\\")
			strPath += "\\";
		strPath += dlg.m_strNewFolder;
		if (!CreateDirectory(strPath, NULL))
		{
			ReportLastError(IDS_E_CREATE_DIRECTORY1);
			return;
		}

		HTREEITEM hItem = m_wndTree.Locate(strPath);
		ASSERT(hItem);
	}
}

void CDlgDirectoryPicker::OnLocate() 
{
	// TODO: Add your control notification handler code here
	
	CString strPath;
	GetDlgItemText(IDC_EDIT_PATH, strPath);
	m_wndTree.Locate(strPath);
}

void CDlgDirectoryPicker::OnOK() 
{
	// TODO: Add extra validation here
	
	GetDlgItemText(IDC_EDIT_PATH, m_strPath);
	m_strPath.TrimLeft();
	m_strPath.TrimRight();
	if (!PathIsRoot(m_strPath) && !PathIsDirectory(m_strPath))
		if (!PrepareDirectory(m_strPath, TRUE))
			return;

	CDialog::OnOK();
}

void CDlgDirectoryPicker::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	m_strPath.Empty();

	CDialog::OnCancel();
}

BOOL CDlgDirectoryPicker::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	// fill tree
	m_wndTree.EnableImages();

	m_bPopulating = TRUE;
	m_wndTree.PopulateTree();
	m_bPopulating = FALSE;

	// Set initialize path
	HTREEITEM hItem = NULL;
	if (m_strPath.IsEmpty())
		m_wndTree.GetSelectedFolderPath(m_strPath);
	m_wndTree.Locate(m_strPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgDirectoryPicker::SetPath(LPCTSTR lpszPath)
{
	m_strPath = lpszPath;
	if (::IsWindow(m_hWnd))
		return m_wndTree.Locate(lpszPath) != NULL;
	return TRUE;
}

CString CDlgDirectoryPicker::GetPath()
{
	return m_strPath;
}

void CDlgDirectoryPicker::OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	// do not handle this event while populating the tree.
	// this can avoid changing m_strPath
	if (!m_bPopulating)
	{
		BOOL bEnable = FALSE;

		if (m_wndTree.GetSelectedFolderPath(m_strPath))
		{
			if (!m_strPath.IsEmpty())
			{
				bEnable = TRUE;
				SetDlgItemText(IDC_EDIT_PATH, m_strPath);
			}
		}

		GetDlgItem(IDOK)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_NEW_FOLDER)->EnableWindow(bEnable);
	}

	*pResult = 0;
}

void CDlgDirectoryPicker::OnChangeEditPath() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	GetDlgItemText(IDC_EDIT_PATH, m_strPath);
	m_strPath.TrimLeft();
	m_strPath.TrimRight();
	GetDlgItem(IDOK)->EnableWindow(IsValidPath(m_strPath));
}
