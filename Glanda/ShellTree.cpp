// ShellTree.cpp : implementation file
//

#include "stdafx.h"
#include "ShellTree.h"
#include "Resource.h"
#include "atlconv.h"
#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "shlwapi")

BOOL GetShortcutTarget(LPCTSTR szShortcut, CString &strTarget)
{
    IShellLink* psl;
    HRESULT hr;

    // Create instance for shell link
    hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &psl);
    if (SUCCEEDED(hr))
    {
        // Get a pointer to the persist file interface
        IPersistFile* ppf;
        hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
        if (SUCCEEDED(hr))
        {
            // Make sure it's ANSI
            WORD wsz[MAX_PATH + 1];
            ::MultiByteToWideChar(CP_ACP, 0, szShortcut, -1, (LPWSTR)&wsz, MAX_PATH);

            // Load shortcut
            hr = ppf->Load((LPCOLESTR)&wsz, STGM_READ);
            if (SUCCEEDED(hr)) {
				WIN32_FIND_DATA wfd;
				// find the path from that
				HRESULT hr = psl->GetPath(strTarget.GetBuffer(_MAX_PATH + 1), 
								MAX_PATH,
								&wfd, 
								SLGP_UNCPRIORITY);
				strTarget.ReleaseBuffer();
            }
            ppf->Release();
        }
        psl->Release();
    }

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
// CShellTree
//
// This source is part of CShellTree - Selom Ofori
// 
// Version: 1.02 (any previously unversioned copies are older/inferior
//
// This code is free for all to use. Mutatilate it as much as you want
// See MFCENUM sample from microsoft

CShellTree::CShellTree()
{
}

CShellTree::~CShellTree()
{
}


BEGIN_MESSAGE_MAP(CShellTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CShellTree)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShellTree::LPITEMDATA CShellTree::AllocItemData()
{
	LPITEMDATA pData = (LPITEMDATA)CoTaskMemAlloc(sizeof(ITEMDATA));
	if (pData != NULL)
		memset(pData, 0, sizeof(ITEMDATA));
	return pData;
}

void CShellTree::FreeItemData(LPITEMDATA pData)
{
	if (pData != NULL)	
		CoTaskMemFree(pData);	
}

/****************************************************************************
*
*    FUNCTION: PopulateTree()
*
*    PURPOSE:  Processes the File.Fill/RefreshTree command
*
****************************************************************************/
void CShellTree::PopulateTree() 
{

    LPSHELLFOLDER lpsf=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;
   
    // Get a pointer to the desktop folder.
    hr=SHGetDesktopFolder(&lpsf);

    if (SUCCEEDED(hr))
    {
       // Initialize the tree view to be empty.
		if (GetCount() > 0)
		{
		   SaveSelection();
		   DeleteAllItems();
		}
	   
	   LPITEMIDLIST pidlDesktop = NULL;
	   hr = SHGetSpecialFolderLocation(::GetParent(m_hWnd), CSIDL_DESKTOP, &pidlDesktop);

	   SHFILEINFO sfi;
	   SHGetFileInfo((LPCTSTR)pidlDesktop, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	   CoTaskMemFree(pidlDesktop);
	   
	   LPITEMDATA lptvid = AllocItemData();
	   if (!lptvid)
			return;
	   
	   lptvid->lpi = NULL;
	   lptvid->lpifq = NULL;
	   lptvid->lpsfParent = NULL;

	   TVINSERTSTRUCTA tvis;
	   tvis.hParent = TVI_ROOT;
	   tvis.hInsertAfter = TVI_LAST;
	   tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | LVIF_PARAM;
	   tvis.item.pszText = sfi.szDisplayName;
	   tvis.item.iImage = sfi.iIcon;
	   tvis.item.iSelectedImage = sfi.iIcon;
	   tvis.item.cChildren = 1;
	   tvis.item.lParam = (LPARAM)lptvid;

	   HTREEITEM hDesktop = InsertItem(&tvis);

	   Expand(hDesktop, TVE_EXPAND);

	   // Fill in the tree view from the root.
       //FillTreeView(lpsf, NULL, hDesktop);

       //TunnelFillTree(lpsf, NULL, TVI_ROOT);
       // Release the folder pointer.
       lpsf->Release();
    }

    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;

    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);

    InitSelection();
}

/****************************************************************************
*
*	FUNCTION:	PopulateTree()
*
*	PURPOSE:	Processes the File.Fill/RefreshTree command
*				This overload  has the ability to open from a
*				special folderlocation like SHBrowseForFolder()
*
*	WARNING:	TunnelTree() will not work if you use a special
*				folderlocation
*
****************************************************************************/
void CShellTree::PopulateTree(int nFolder) 
{

	LPSHELLFOLDER lpsf=NULL,lpsf2=NULL;
    LPITEMIDLIST  lpi=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;
   
    // Get a pointer to the desktop folder.
	hr=SHGetDesktopFolder(&lpsf);

    if (SUCCEEDED(hr))
    {
       // Initialize the tree view to be empty.
		DeleteAllItems();

		if (!SUCCEEDED(SHGetSpecialFolderLocation( 
				m_hWnd, nFolder, &lpi))) 
		{ 
			lpi=NULL;
			FillTreeView(lpsf,NULL,TVI_ROOT);
		}
		else
		{
			hr=lpsf->BindToObject(lpi,
                0, IID_IShellFolder,(LPVOID *)&lpsf2);

			if(SUCCEEDED(hr))
			{
				// Fill in the tree view from the root.
				FillTreeView(lpsf2, lpi, TVI_ROOT);
				lpsf2->Release();
			}
			else
				FillTreeView(lpsf,NULL,TVI_ROOT);
		}

		// Release the folder pointer.
		lpsf->Release();
	}
    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;

    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);
    
	HTREEITEM hItem;
	hItem = GetRootItem();
	Expand(hItem,TVE_EXPAND);
	Select(GetRootItem(),TVGN_CARET);
}

ULONG CShellTree::GetItemAttrs(LPSHELLFOLDER lpsf, LPITEMIDLIST pidl)
{
	ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM | SFGAO_FOLDER;

	lpsf->GetAttributesOf(1, (LPCITEMIDLIST *)&pidl, &ulAttrs);

	if ((ulAttrs & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR)) && (ulAttrs & SFGAO_FOLDER))
		return ulAttrs;
	else
		return 0;
}

/****************************************************************************
*
*  FUNCTION: FillTreeView( LPSHELLFOLDER lpsf,
*                          LPITEMIDLIST  lpifq,
*                          HTREEITEM     hParent)
*
*  PURPOSE: Fills a branch of the TreeView control.  Given the
*           shell folder, enumerate the subitems of this folder,
*           and add the appropriate items to the tree.
*
*  PARAMETERS:
*    lpsf         - Pointer to shell folder that we want to enumerate items 
*    lpifq        - Fully qualified item id list to the item that we are enumerating
*                   items for.  In other words, this is the PIDL to the item
*                   identified by the lpsf parameter.
*    hParent      - Parent node
*
*  COMMENTS:
*    This function enumerates the items in the folder identifed by lpsf.
*    Note that since we are filling the left hand pane, we will only add
*    items that are folders and/or have sub-folders.  We *could* put all
*    items in here if we wanted, but that's not the intent.
*
****************************************************************************/
void CShellTree::FillTreeView(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM     hParent)
{
    TV_ITEM tvi;						          // TreeView Item.
    TV_INSERTSTRUCT tvins;                        // TreeView Insert Struct.
    HTREEITEM hPrev					= NULL;           // Previous Item Added.
    LPENUMIDLIST lpe				= NULL;
    LPITEMIDLIST lpi				= NULL;
	LPITEMIDLIST lpifqThisItem		= NULL;
    LPITEMDATA lptvid				= NULL;
    ULONG ulFetched;
    HRESULT hr;
    TCHAR szBuff[_MAX_PATH];
    HWND hwnd = ::GetParent(m_hWnd);

    // Get the IEnumIDList object for the given folder.
    hr=lpsf->EnumObjects(hwnd, SHCONTF_INCLUDEHIDDEN | SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &lpe);

    if (SUCCEEDED(hr))
    {
        // Enumerate throught the list of folder and non-folder objects.
        while (S_OK==lpe->Next(1, &lpi, &ulFetched))
        {
			//Create a fully qualified path to the current item
            //The SH* shell api's take a fully qualified path pidl,
            //(see GetIcon above where I call SHGetFileInfo) whereas the
            //interface methods take a relative path pidl.
            //ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM | SFGAO_FOLDER;

            // Determine what type of object we have.
            //lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);

            //if (ulAttrs & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR) && ulAttrs & SFGAO_FOLDER)
			ULONG ulAttrs = GetItemAttrs(lpsf, lpi);
			if (ulAttrs != 0)
            {
               //We need this next if statement so that we don't add things like
               //the MSN to our tree.  MSN is not a folder, but according to the
               //shell it has subfolders.
               //if (ulAttrs & SFGAO_FILESYSANCESTOR)
               {
                  tvi.mask= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

                  if (ulAttrs & SFGAO_HASSUBFOLDER)
                  {
                     //This item has sub-folders, so let's put the + in the TreeView.
                     //The first time the user clicks on the item, we'll populate the
                     //sub-folders.
                     tvi.cChildren=1;
                     tvi.mask |= TVIF_CHILDREN;
                  }
                    
                  //OK, let's get some memory for our ITEMDATA struct
                  lptvid = AllocItemData();
                  if (!lptvid)
                     goto Done;  // Error - could not allocate memory.

                  //Now get the friendly name that we'll put in the treeview.
                  if (!GetName(lpsf, lpi, SHGDN_NORMAL, szBuff))
                     goto Done; // Error - could not get friendly name.

                  tvi.pszText    = szBuff;
                  tvi.cchTextMax = MAX_PATH;

                  lpifqThisItem=ConcatPidls(lpifq, lpi);
  
					//ulAttrs = SFGAO_FILESYSTEM;
					//lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);
					//TRACE1("%d: ", ulAttrs & (SFGAO_FILESYSTEM));
					//if (ulAttrs & (SFGAO_FILESYSTEM))
					//{
					//	TCHAR szPath[_MAX_PATH];
					//	SHGetPathFromIDList(lpifqThisItem, szPath);
					//	TRACE1("%s", szPath);
					//}
					//TRACE0("\n");

                  //Now, make a copy of the ITEMIDLIST
                  lptvid->lpi=CopyITEMID(lpi);

                  GetNormalAndSelectedIcons(lpifqThisItem, &tvi);

                  lptvid->lpsfParent=lpsf;    //Store the parent folders SF
                  lpsf->AddRef();

                  lptvid->lpifq=ConcatPidls(lpifq, lpi);

                  tvi.lParam = (LPARAM)lptvid;

                  // Populate the TreeVeiw Insert Struct
                  // The item is the one filled above.
                  // Insert it after the last item inserted at this level.
                  // And indicate this is a root entry.
                  tvins.item         = tvi;
                  tvins.hInsertAfter = hPrev;
                  tvins.hParent      = hParent;

                  // Add the item to the tree
                  hPrev = InsertItem(&tvins);

				  TRACE1("Insert item: %s\n", szBuff);
               }
               // Free this items task allocator.
			   CoTaskMemFree(lpifqThisItem);               
               lpifqThisItem=0;
            }
			
            CoTaskMemFree(lpi);  //Free the pidl that the shell gave us.
            lpi=0;
        }
    }

Done:
 
    if (lpe)  
        lpe->Release();

    //The following 2 if statements will only be TRUE if we got here on an
    //error condition from the "goto" statement.  Otherwise, we free this memory
    //at the end of the while loop above.
    if (lpi)
        CoTaskMemFree(lpi);
    if (lpifqThisItem) 
        CoTaskMemFree(lpifqThisItem);     
}


/****************************************************************************
*
*    FUNCTION: GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
*
*    PURPOSE:  Gets the index for the normal and selected icons for the current item.
*
*    PARAMETERS:
*    lpifq    - Fully qualified item id list for current item.
*    lptvitem - Pointer to treeview item we are about to add to the tree.
*
****************************************************************************/
void CShellTree::GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,
                               LPTV_ITEM lptvitem)
{
   //Note that we don't check the return value here because if GetIcon()
   //fails, then we're in big trouble...

   lptvitem->iImage = GetItemIcon(lpifq, SHGFI_PIDL | 
                              SHGFI_SYSICONINDEX | 
                              SHGFI_SMALLICON);
   
   lptvitem->iSelectedImage = GetItemIcon(lpifq, SHGFI_PIDL | 
                                      SHGFI_SYSICONINDEX | 
                                      SHGFI_SMALLICON |
                                      SHGFI_OPENICON);
   
   return;
}



/****************************************************************************
*
*    FUNCTION: TreeViewCompareProc(LPARAM, LPARAM, LPARAM)
*
*    PURPOSE:  Callback routine for sorting the tree 
*
****************************************************************************/
int CALLBACK CShellTree::TreeViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM lparamSort)
{
    LPITEMDATA lptvid1=(LPITEMDATA)lparam1;
    LPITEMDATA lptvid2=(LPITEMDATA)lparam2;
    HRESULT   hr;

    hr = lptvid1->lpsfParent->CompareIDs(0,lptvid1->lpi,lptvid2->lpi);

    if (FAILED(hr))
       return 0;

    return (short)SCODE_CODE(GetScode(hr));
}




/////////////////////////////////////////////////////////////////////////////
// CShellTree message handlers

/****************************************************************************
*
*	FUNCTION:	GetContextMenu(NMHDR* pNMHDR, LRESULT* pResult) 
*
*	PURPOSE:	Diplays a popup menu for the folder selected. Pass the
*				parameters from Rclick() to this function.
*
*	MESSAGEMAP:	NM_RCLICK;
*
****************************************************************************/
void CShellTree::GetContextMenu(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POINT			pt;
	LPITEMDATA	lptvid;  //Long pointer to TreeView item data
	static char		szBuff[MAX_PATH];
	TV_HITTESTINFO	tvhti;
	TV_ITEM			tvi;

	// TODO: Add your control notification handler code here
	::GetCursorPos((LPPOINT)&pt);
	ScreenToClient(&pt);
	tvhti.pt=pt;
    HitTest(&tvhti);
    SelectItem(tvhti.hItem);
	if (tvhti.flags & (TVHT_ONITEMLABEL|TVHT_ONITEMICON))
	{
		ClientToScreen(&pt);
		tvi.mask=TVIF_PARAM;
		tvi.hItem=tvhti.hItem;
		
        if (!GetItem(&tvi)){
			return;
		}
		
		lptvid=(LPITEMDATA)tvi.lParam;
		
		DoTheMenuThing(::GetParent(m_hWnd),
		lptvid->lpsfParent, lptvid->lpi, &pt);
	}	
	
	*pResult = 0;
}

/****************************************************************************
*
*	FUNCTION:	OnFolderSelected(NMHDR* pNMHDR, LRESULT* pResult, CString &strFolderPath) 
*
*	PURPOSE:	Call this function if for example you want to put the path of the folder
*				selected inside a combobox or an edit window. You would pass the
*				parameters from OnSelChanged() to this function along with a CString object
*				that will hold the folder path. If the path is not
*				in the filesystem(eg MyComputer) it returns false.
*
*	MESSAGEMAP:	TVN_SELCHANGED
*
****************************************************************************/
BOOL CShellTree::OnFolderSelected(NMHDR* pNMHDR, LRESULT* pResult, CString &strFolderPath) 
{
	// TODO: Add your control notification handler code here
	LPITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	static char		szBuff[MAX_PATH];
	HRESULT			hr;
	BOOL			bRet=false;
	TV_SORTCB		tvscb;
	HTREEITEM		hItem=NULL;

	if((hItem = GetSelectedItem()))
	{
		lptvid=(LPITEMDATA)GetItemData(hItem);

		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
					 0,IID_IShellFolder,(LPVOID *)&lpsf2);

			if (SUCCEEDED(hr))
				{
					ULONG ulAttrs = SFGAO_FILESYSTEM;

					// Determine what type of object we have.
					lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

					if (ulAttrs & (SFGAO_FILESYSTEM))
					{
						if(SHGetPathFromIDList(lptvid->lpifq,szBuff)){
							strFolderPath = szBuff;
							bRet = true;
						}
					}
					//non standard from here(NEW CODE)
					NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
					if ((pnmtv->itemNew.cChildren == 1) && !(pnmtv->itemNew.state & TVIS_EXPANDEDONCE)){
						FillTreeView(lpsf2,lptvid->lpifq,pnmtv->itemNew.hItem);

						tvscb.hParent     = pnmtv->itemNew.hItem;
						tvscb.lParam      = 0;
						tvscb.lpfnCompare = TreeViewCompareProc;
						SortChildrenCB(&tvscb);
						
						pnmtv->itemNew.state |= TVIS_EXPANDEDONCE;
						pnmtv->itemNew.stateMask |= TVIS_EXPANDEDONCE;
						pnmtv->itemNew.mask |= TVIF_STATE;
						SetItem(&pnmtv->itemNew);
					}
				}

		}
		if(lpsf2)
			lpsf2->Release();
		
	}	
	*pResult = 0;
	return bRet;
}

void CShellTree::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPITEMDATA lptvid=NULL;
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//Let's free the memory for the TreeView item data...		
	lptvid=(LPITEMDATA)pNMTreeView->itemOld.lParam;
	if (lptvid->lpsfParent)
		lptvid->lpsfParent->Release();
	CoTaskMemFree(lptvid->lpi);  
	CoTaskMemFree(lptvid->lpifq);  
	
	FreeItemData(lptvid);

	*pResult = 0;
}

/****************************************************************************
*
*	FUNCTION:	EnableImages()
*
*	PURPOSE:	Obtains a handle to the system image list and attaches it
*				to the tree control. DO NOT DELETE the imagelist
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
void CShellTree::EnableImages()
{
	// Get the handle to the system image list, for our icons
    HIMAGELIST  hImageList;
    SHFILEINFO    sfi;

    hImageList = (HIMAGELIST)SHGetFileInfo((LPCSTR)"C:\\", 
                                           0,
                                           &sfi, 
                                           sizeof(SHFILEINFO), 
                                           SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    // Attach ImageList to TreeView
    if (hImageList)
        ::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM) TVSIL_NORMAL,
            (LPARAM)hImageList);
}

/****************************************************************************
*
*	FUNCTION:	GetSelectedFolderPath(CString &strFolderPath)
*
*	PURPOSE:	Retrieves the path of the currently selected string.
*				Pass a CString object that will hold the folder path. 
*				If the path is not in the filesystem(eg MyComputer) 
*				or none is selected it returns false.
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
BOOL CShellTree::GetSelectedFolderPath(CString &strFolderPath)
{
	LPITEMDATA		lptvid;
	LPSHELLFOLDER	lpsf2 = NULL;
	TCHAR			szBuff[_MAX_PATH];
	HTREEITEM		hItem = NULL;
	HRESULT			hr;
	BOOL			bRet = FALSE;

	if((hItem = GetSelectedItem()))
	{
		lptvid=(LPITEMDATA)GetItemData(hItem);

		if (lptvid)
		{
			if (lptvid->lpsfParent)
			{
				if (lptvid->lpi)
				{
					hr=lptvid->lpsfParent->BindToObject(lptvid->lpi, 0, IID_IShellFolder, (LPVOID *)&lpsf2);

					if (SUCCEEDED(hr))
					{
						ULONG ulAttrs = SFGAO_FILESYSTEM | SFGAO_LINK;

						// Determine what type of object we have.
						lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);

						TRACE("%08x, %08x, %08x\n", ulAttrs, SFGAO_FILESYSTEM, SFGAO_LINK);

						if (ulAttrs & (SFGAO_FILESYSTEM))
						{
							if(SHGetPathFromIDList(lptvid->lpifq, szBuff))
							{
								if (ulAttrs & SFGAO_LINK)
								{
									bRet = GetShortcutTarget(szBuff, strFolderPath);
								}
								else
								{
									strFolderPath = szBuff;
									bRet = true;
								}
							}
						}
					}
				}
			}
			else	// it's the root: desktop
			{
				LPITEMIDLIST pidl = NULL;
				if (SUCCEEDED(SHGetSpecialFolderLocation(m_hWnd, CSIDL_DESKTOP, &pidl)))
				{
					if(SHGetPathFromIDList(pidl, szBuff))
					{
						strFolderPath = szBuff;
						bRet = true;
					}

					CoTaskMemFree(pidl);					
				}
			}
		}
		if(lpsf2)
			lpsf2->Release();
	}
	return bRet;
}

/****************************************************************************
*
*	FUNCTION:	GetParentShellFolder(HTREEITEM folderNode)
*
*	PURPOSE:	Retrieves the pointer to the ISHELLFOLDER interface
*				of the tree node passed as the paramter.
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
LPSHELLFOLDER CShellTree::GetParentShellFolder(HTREEITEM folderNode)
{
	LPITEMDATA	lptvid;  //Long pointer to TreeView item data

	lptvid=(LPITEMDATA)GetItemData(folderNode);
	if(lptvid)
		return lptvid->lpsfParent;
	else
		return NULL;
}

/****************************************************************************
*
*	FUNCTION:	GetRelativeIDLIST(HTREEITEM folderNode)
*
*	PURPOSE:	Retrieves the Pointer to an ITEMIDLIST structure that
*				identifies the subfolder relative to its parent folder.
*				see GetParentShellFolder();
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
LPITEMIDLIST CShellTree::GetRelativeIDLIST(HTREEITEM folderNode)
{
	LPITEMDATA	lptvid;  //Long pointer to TreeView item data

	lptvid=(LPITEMDATA)GetItemData(folderNode);
	if(lptvid)
		return lptvid->lpifq;
	else
		return NULL;
}

/****************************************************************************
*
*	FUNCTION:	GetFullyQualifiedIDLIST(HTREEITEM folderNode)
*
*	PURPOSE:	Retrieves the Retrieves the Pointer to an ITEMIDLIST
*				structure that identifies the subfolder relative to the
*				desktop. This is a fully qualified Item Identifier
*
*	MESSAGEMAP:	NONE
*
****************************************************************************/
LPITEMIDLIST CShellTree::GetFullyQualifiedID(HTREEITEM folderNode)
{
	LPITEMDATA	lptvid;  //Long pointer to TreeView item data

	lptvid=(LPITEMDATA)GetItemData(folderNode);
	if(lptvid)
		return lptvid->lpifq;
	else
		return NULL;
}

/****************************************************************************
*
*	FUNCTION:	SearchTree(	HTREEITEM treeNode,
*							CString strSearchName )
*
*	PURPOSE:	Too crude to explain, just use it
*
*	WARNING:	Only works if you use the default PopulateTree()
*				Not guaranteed to work on any future or existing
*				version of windows. Use with caution. Pretty much
*				ok if you're using on local drives
*
****************************************************************************/
bool CShellTree::SearchTree(HTREEITEM treeNode,
							CString strSearchName,
							FindAttribs attr)
{
	LPITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];
	char	ext[_MAX_EXT];
	bool	bRet=false;
	HRESULT	hr;
	CString strCompare;

	TRACE1("Search: %s\n", GetItemText(treeNode));

	strSearchName.MakeUpper();
	treeNode = GetChildItem(treeNode);
	while(treeNode && bRet==false)
	{
		lptvid=(LPITEMDATA)GetItemData(treeNode);
		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
					 0,IID_IShellFolder,(LPVOID *)&lpsf2);
			if (SUCCEEDED(hr))
			{
				ULONG ulAttrs = SFGAO_FILESYSTEM;
				lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);
				if (ulAttrs & (SFGAO_FILESYSTEM))
				{
					if(SHGetPathFromIDList(lptvid->lpifq, strCompare.GetBuffer(MAX_PATH)))
					{
						switch(attr)
						{
						case type_drive:
							_splitpath(strCompare, drive, dir, fname, ext);
							strCompare = drive;
							break;
						case type_folder:
							strCompare = GetItemText(treeNode);
							break;
						}
						strCompare.MakeUpper();
						if(strCompare == strSearchName)
						{
							EnsureVisible(treeNode);
							SelectItem(treeNode);
							bRet=true;
						}
					}
				}
				lpsf2->Release();
			}
		}
		treeNode = GetNextSiblingItem(treeNode);
	}
	return bRet;
}

HTREEITEM CShellTree::LocatePath(LPCTSTR lpszPath)
{
	LPSHELLFOLDER lpsf = NULL;
	LPITEMIDLIST pidlFull = NULL;
	if (SHGetDesktopFolder(&lpsf) != NOERROR)
		return NULL;

	HTREEITEM hRoot = NULL;

	USES_CONVERSION;

	LPITEMIDLIST pidl = NULL;
	ULONG uEaten = 0;
	DWORD dwAttributes = 0;
	if (lpsf->ParseDisplayName(m_hWnd, NULL, A2OLE(lpszPath), &uEaten, &pidl, &dwAttributes) == NOERROR)
	{
		hRoot = LocateITEMIDLIST(pidl);
		CoTaskMemFree(pidl);		
	}
	
	ULONG cRef = lpsf->Release();
	TRACE1("lpsf->Release() = %d\n", cRef);

	return hRoot;
}

HTREEITEM CShellTree::Locate(LPITEMIDLIST pidl)
{
	CWaitCursor wc;

	SetRedraw(FALSE);

	HTREEITEM hItem = LocateITEMIDLIST(pidl);
	if (hItem)
	{
		SelectItem(hItem);
		EnsureVisible(hItem);
	}
	
	SetRedraw(TRUE);
	return hItem;
}

HTREEITEM CShellTree::Locate(LPCTSTR lpszPath)
{
	CWaitCursor wc;

	SetRedraw(FALSE);
	HTREEITEM hItem = LocatePath(lpszPath);
	
	if (hItem)
	{
		SelectItem(hItem);
		EnsureVisible(hItem);
	}
	
	SetRedraw(TRUE);
	return hItem;
}

void CShellTree::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
    if ((pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE || GetChildItem(pNMTreeView->itemNew.hItem)))
         return;

	CWaitCursor wc;

	TRACE1("OnItemExpanding(): %s\n", GetItemText(pNMTreeView->itemNew.hItem));

	LPITEMDATA   lptvid;  //Long pointer to TreeView item data
	HRESULT        hr;
	LPSHELLFOLDER  lpsf = NULL;
	static char    szBuff[MAX_PATH];
	TV_SORTCB      tvscb;

    lptvid = (LPITEMDATA)pNMTreeView->itemNew.lParam;
    if (lptvid)
	{
        //hr=lptvid->lpsfParent->BindToObject(lptvid->lpi, 0, IID_IShellFolder,(LPVOID *)&lpsf);
		if (lptvid->lpsfParent)
			hr = lptvid->lpsfParent->BindToObject(lptvid->lpi, 0, IID_IShellFolder,(LPVOID *)&lpsf);
		else
			hr = SHGetDesktopFolder(&lpsf);

        if (SUCCEEDED(hr))
        {
			FillTreeView(lpsf, lptvid->lpifq, pNMTreeView->itemNew.hItem);
		
			lpsf->Release();

			tvscb.hParent     = pNMTreeView->itemNew.hItem;
			tvscb.lParam      = 0;
			tvscb.lpfnCompare = TreeViewCompareProc;

			SortChildrenCB(&tvscb /*, FALSE*/);
		}
		else
		{
			TVITEM tvi;
			tvi.hItem = pNMTreeView->itemNew.hItem;
			tvi.mask = TVIF_CHILDREN;
			tvi.cChildren = 0;
			SetItem(&tvi);
		}
    }	
	
	*pResult = 0;
}

HTREEITEM CShellTree::LocateITEMIDLIST(LPITEMIDLIST pidl)
{
	ASSERT (pidl);	

	HTREEITEM hRoot = GetRootItem();
	ASSERT(hRoot);

	BOOL bFound = FALSE;
	BOOL bFoundInTree = FALSE;

	HRESULT hr;

	LPSHELLFOLDER lpsf = NULL;
	hr = SHGetDesktopFolder(&lpsf);
    if (hr != NOERROR)
		return NULL;

	LPITEMIDLIST pidlFull = NULL;
	hr = SHGetSpecialFolderLocation(::GetParent(m_hWnd), CSIDL_DESKTOP, &pidlFull);

    if (hr != NOERROR)
		return NULL;


	// check if it's just the desktop folder
	if (lpsf->CompareIDs(0, pidl, pidlFull) == 0)
	{
		bFound = TRUE;
		bFoundInTree = TRUE;
	}
	else
	{
		ULONG uEaten = 0;
		DWORD dwAttributes = 0;

		LPITEMIDLIST pidlNext = pidl;

		int indent = 0;
		while (pidlNext && pidlNext->mkid.cb > 0)
		{
			bFound = FALSE;

			// only compare the first element in the chain
			UINT cb = pidlNext->mkid.cb;

			// look up child pidl in current lpsf
			LPENUMIDLIST pe = NULL;
			if (lpsf->EnumObjects(m_hWnd, SHCONTF_INCLUDEHIDDEN | SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pe) != NOERROR)
			{
				ASSERT(FALSE);
				break;
			}

			LPITEMIDLIST pidlSub = NULL;
			UINT cbSub = 0;
			ULONG ulFetched = 0;
			while (pe->Next(1, &pidlSub, &ulFetched) == NOERROR)
			{
				cbSub = pidlSub->mkid.cb;

				LPITEMIDLIST pidlTemp = (LPITEMIDLIST)CoTaskMemAlloc(cb + 2);
				memset(pidlTemp, 0, cb + 2);
				memcpy(pidlTemp, pidlNext, cb);
				if (lpsf->CompareIDs(0, pidlSub, pidlTemp) == 0)
				{
					CoTaskMemFree(pidlTemp);
					bFound = TRUE;
					break;
				}

				CoTaskMemFree(pidlTemp);
				
				// if not match, release pidlSub here
				CoTaskMemFree(pidlSub);				
				pidlSub = NULL;
			}
			
			pe->Release();
			pe = NULL;

			if (!bFound)
				break;

			TRACE1("Search under item: %s\n", GetItemText(hRoot));
			Expand(hRoot, TVE_EXPAND);

			bFoundInTree = FALSE;
			HTREEITEM hItem = GetChildItem(hRoot);
			while (hItem)
			{
				LPITEMDATA pid = (LPITEMDATA)GetItemData(hItem);
				ASSERT(pid);

				/*
				if (memcmp(pid->lpi->mkid.abID, pidlSub->mkid.abID, pidlSub->mkid.cb - sizeof(pidlSub->mkid.cb)) == 0)
				{
					bFoundInTree = TRUE;
					for (int i = 0; i < indent; i++)
						TRACE0(" ");
					TRACE("Found this pidlSub in tree!\n");
					break;
				}
				*/
				//LPITEMIDLIST pidlTemp = (LPITEMIDLIST)lpMalloc->Alloc(cbSub + 2);
				//memset(pidlTemp, 0, cbSub + 2);
				//memcpy(pidlTemp, pid->lpi, cbSub);
				//if (lpsf->CompareIDs(0, pidlSub, pidlTemp) == 0)
				if (lpsf->CompareIDs(0, pidlSub, pid->lpi) == 0)
				{
					bFoundInTree = TRUE;
					//lpMalloc->Free(pidlTemp);
					for (int i = 0; i < indent; i++)
						TRACE0(" ");
					TRACE("Found this pidlSub in tree!\n");
					break;
				}
				//lpMalloc->Free(pidlTemp);

				hItem = GetNextSiblingItem(hItem);
			}

			LPITEMIDLIST pidlFullOld = pidlFull;
			pidlFull = ConcatPidls(pidlFullOld, pidlSub);
			CoTaskMemFree(pidlFullOld);			

			if (!bFoundInTree)
			{
				TRACE1("Not found under %s, should insert an item.\n", GetItemText(hRoot));
				ULONG uAttrs = GetItemAttrs(lpsf, pidlSub);
				if (uAttrs != 0) // show item
				{
					LPITEMDATA lptvid = AllocItemData();
					if (lptvid)
					{
						TVINSERTSTRUCTA tvis;

						SHFILEINFO sfi;
						SHGetFileInfo((LPCTSTR)pidlFull, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

						lptvid->lpi = CopyITEMID(pidlSub);
						ASSERT(lpsf->CompareIDs(0, lptvid->lpi, pidlSub) == 0);
						lptvid->lpifq = CopyITEMID(pidlFull);
						ASSERT(memcmp(lptvid->lpifq, pidlFull, GetSize(pidlFull)) == 0);
						lptvid->lpsfParent = lpsf;
						lpsf->AddRef();

						TCHAR szPath[_MAX_PATH];
						SHGetPathFromIDList(lptvid->lpifq, szPath);
						TRACE1("FullPath: %s\n", szPath);

						tvis.hParent = hRoot;
						tvis.hInsertAfter = TVI_LAST;
						tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | LVIF_PARAM;
						tvis.item.pszText = sfi.szDisplayName;
						tvis.item.iImage = sfi.iIcon;
						tvis.item.iSelectedImage = sfi.iIcon;
						tvis.item.lParam = (LPARAM)lptvid;

						if (uAttrs & SFGAO_HASSUBFOLDER)
						{
							tvis.item.cChildren = 1;
							tvis.item.mask |= TVIF_CHILDREN;
						}

						hItem = InsertItem(&tvis);

						TV_SORTCB tvscb;
						tvscb.hParent = hRoot;
						tvscb.lParam = 0;
						tvscb.lpfnCompare = TreeViewCompareProc;
						SortChildrenCB(&tvscb);

						bFoundInTree = TRUE;
					}
				}
			}

			hRoot = hItem;

			// get current IShellFolder *
			LPSHELLFOLDER lpsfNew = NULL;
			hr = lpsf->BindToObject(pidlSub, 0, IID_IShellFolder, (LPVOID *)&lpsfNew);
			//ASSERT(lpsfNew);
			ULONG cRef = lpsf->Release();
			TRACE1("lpsf->Release() = %d\n", cRef);
			lpsf = lpsfNew;
			
			ASSERT(pidlSub);
			CoTaskMemFree(pidlSub);
			
			pidlNext = Next(pidlNext);

			indent++;

			if ((!bFoundInTree) || (lpsf == NULL)) // item is not folder or error occured
				break;
		}
	}

	if (pidlFull)
		CoTaskMemFree(pidlFull);		

	if (lpsf)
	{
		ULONG cRef = lpsf->Release();
		TRACE1("lpsf->Release() = %d\n", cRef);
	}

	return bFound && bFoundInTree ? hRoot: NULL;
}

void CShellTree::InitSelection()
{
	SelectItem(GetRootItem());
}

void CShellTree::SaveSelection()
{

}
