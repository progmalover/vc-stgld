// ResourceDirTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ResourceDirTreeCtrl.h"
#include "CategoryExplorerPage.h"
#include ".\resourcedirtreectrl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CResourceDirTreeCtrl

IMPLEMENT_DYNAMIC(CResourceDirTreeCtrl, CTreeCtrl)
CResourceDirTreeCtrl::CResourceDirTreeCtrl()
{
}

CResourceDirTreeCtrl::~CResourceDirTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CResourceDirTreeCtrl, CTreeCtrl)
	ON_WM_DELETEITEM()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnTvnDeleteitem)
END_MESSAGE_MAP()



// CResourceDirTreeCtrl message handlers
void CResourceDirTreeCtrl::SetResourceRootDir(LPCTSTR lpszRootDirectory)
{
	DeleteAllItems();

	if(lpszRootDirectory && _tcslen(lpszRootDirectory))
	{
		HTREEITEM hItem = InsertDirectory(lpszRootDirectory, TVI_ROOT, TVI_LAST);
		if (ItemHasChildren(hItem))
			Expand(hItem, TVE_EXPAND);
	}
}

void CResourceDirTreeCtrl::OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	CString* pString  = (CString*)lpDeleteItemStruct->itemData;
	delete pString;

	CTreeCtrl::OnDeleteItem(nIDCtl, lpDeleteItemStruct);
}

HTREEITEM CResourceDirTreeCtrl::InsertDirectory(LPCTSTR lpszPathName, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString strPathName(lpszPathName);
	int nIndex = strPathName.ReverseFind('\\');
	ASSERT(nIndex >= 0);

	HTREEITEM hNewItem = InsertItem(strPathName.Mid(nIndex + 1, strPathName.GetLength() - nIndex - 1), 0, 1, hParent, hInsertAfter);
	SetItemData(hNewItem, (DWORD_PTR)(new CString(lpszPathName)));

	BOOL bSubExist = FALSE;
	CFileFind FileFind;
	for(BOOL bWorking = FileFind.FindFile(strPathName + "\\*.*"); bWorking; )
	{
		bWorking = FileFind.FindNextFile();
		if(FileFind.IsDirectory() && !FileFind.IsDots())
		{
			bSubExist = TRUE;
			InsertDirectory(strPathName + "\\"  + FileFind.GetFileName(), hNewItem, TVI_LAST);
		}
	}
	//if(ItemHasChildren(hNewItem))
	//{
	//	Expand(hNewItem, TVE_EXPAND);
	//}

	//this->SetItemImage(hNewItem, 0, (bSubExist ? 1 : 0));

	return hNewItem;
}

int CResourceDirTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void CResourceDirTreeCtrl::LoadImageList()
{
	if (m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		if (bmp.LoadBitmap(IDB_RESOURCE_DIR_IMAGE_LIST))
		{
			m_imgList.Add(&bmp, RGB(255, 0, 255));
			SetImageList(&m_imgList, TVSIL_NORMAL);
		}
	}
}


HTREEITEM CResourceDirTreeCtrl::ItemFromDirectory(LPCTSTR lpszDirectory, HTREEITEM hParent)
{
	for(HTREEITEM hItem = GetChildItem(hParent); hItem; hItem = GetNextSiblingItem(hItem))
	{
		if(AfxComparePath(*(CString*)GetItemData(hItem), lpszDirectory))
		{
			return hItem;
		}
		HTREEITEM hNewItem = ItemFromDirectory(lpszDirectory, hItem);
		if(hNewItem)
		{
			return hNewItem;
		}
	}
	return NULL;
}

void CResourceDirTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// TODO: Add your control notification handler code here

	CString* pString  = (CString*)pNMTreeView->itemOld.lParam;
	delete pString;

	*pResult = 0;
}
