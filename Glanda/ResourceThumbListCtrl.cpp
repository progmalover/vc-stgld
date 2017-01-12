// ResourceThumbListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "ResourceThumbListCtrl.h"
#include "filePath.h"
#include "DesignWnd.h"
#include <atlwin.h>
#include <afxole.h>
#include "GlandaDoc.h"
#include "glanda_i.h"
#include "I2C.h"
#include "TransAdaptor.h"
#include "gld_draw.h"
#include "DesignWnd.h"
#include "my_app.h"
#include "gldSound.h"
#include "gldSWFEngine.h"
#include "Graphics.h"
#include "gldButton.h"
#include "CmdAddSound.h"
#include "CmdAddShape.h"
#include "ResourceThumbFilePropertiesDialog.h"
#include "ResourceThumbObjPropertiesDialog.h"
#include "ResourceThumbRenameDialog.h"
#include <GdiPlus.h>
#include "ResourceThumbItem.h"
#include "ProfileManager.h"
#include "PlaySoundDialog.h"
#include ".\resourcethumblistctrl.h"
using namespace Gdiplus;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CResourceThumbListCtrl

//IMPLEMENT_DYNAMIC(CResourceThumbListCtrl, CFlexListCtrl)
CResourceThumbListCtrl::CResourceThumbListCtrl(int nResourceType)
: m_nResourceType(nResourceType)
, m_bEnableDragResource(FALSE)
{
	SetMargin(5, 5, 5, 5);
	SetItemSpace(5, 5);
	SetBkColor(COLOR_WINDOW, TRUE);
}

CResourceThumbListCtrl::~CResourceThumbListCtrl()
{
	CSubjectManager::Instance()->GetSubject("DeleteFile")->Detach(&m_xCObserverDeleteFile);
	CSubjectManager::Instance()->GetSubject("MoveFile")->Detach(&m_xCObserverMoveFile);
}


BEGIN_MESSAGE_MAP(CResourceThumbListCtrl, CFlexListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_COMMAND(ID_THUMB_PLAY_SOUND, OnThumbPlaySound)
	ON_COMMAND(ID_THUMB_COPY, OnThumbCopy)
	ON_COMMAND(ID_THUMB_DELETE, OnThumbDelete)
	ON_COMMAND(ID_THUMB_RENAME, OnThumbRename)
	ON_COMMAND(ID_THUMB_EXPLORE, OnThumbExplore)
	ON_COMMAND(ID_THUMB_PROPERTIES, OnThumbProperties)
	ON_COMMAND(ID_THUMB_ADD_TO_SCENE, OnThumbAddToScene)
	ON_UPDATE_COMMAND_UI(ID_THUMB_ADD_TO_SCENE, OnUpdateThumbAddToScene)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()


// CResourceThumbListCtrl message handlers

void CResourceThumbListCtrl::SetResourceDir(LPCTSTR lpszResourceDir)
{
	DeleteAllItems();

	if(lpszResourceDir && _tcslen(lpszResourceDir))
	{
		CString strResourceDir(lpszResourceDir);

		if(PathFileExists(lpszResourceDir))
		{
			CFileFind FileFind;
			int nItemCount = 0;
			for(BOOL bWorking = FileFind.FindFile(strResourceDir + "\\*.*"); bWorking; )
			{
				bWorking = FileFind.FindNextFile();
				if(!FileFind.IsDirectory())
				{
					int nResourceType = CResourceThumbItem::GetResourceType(FileFind.GetFileName());
					if(m_nResourceType & nResourceType)
					{
						CResourceThumbItem* pThumbItem = new CResourceThumbItem(strResourceDir + '\\' + FileFind.GetFileName());
						InsertItem(nItemCount++, (DWORD_PTR)pThumbItem);
					}
				}
			}
		}
	}
}

void CResourceThumbListCtrl::SetLibraryItems()
{
	DeleteAllItems();

	int nItemCount = 0;
	for(CObjectMap::_ObjectMap::iterator iter = _GetObjectMap()->m_Map.begin(); iter != _GetObjectMap()->m_Map.end(); ++iter)
	{
		gldObj* pObj = iter->first;
		int nResourceType = CResourceThumbItem::GetResourceType(pObj);
		if(m_nResourceType & nResourceType)
		{
			CResourceThumbItem* pThumbItem = new CResourceThumbItem(pObj);
			InsertItem(nItemCount++, (DWORD_PTR)pThumbItem);
		}
	}
}

void CResourceThumbListCtrl::OnDeleteItem(int index)
{
	delete (CResourceThumbItem*)GetItemData(index);
}

void CResourceThumbListCtrl::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
	// 画名称
	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(index);
	CString strName;
	if(pThumbItem->m_bStockResource)
	{
		strName = pThumbItem->m_pObj->m_name.c_str();
		if(strName.IsEmpty())
		{
			if(pThumbItem->m_pObj->IsGObjInstanceOf(gobjImage))
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_IMAGE_OBJ);
			}
			else if(pThumbItem->m_pObj->IsGObjInstanceOf(gobjShape))
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_SHAPE_OBJ);
			}
			else if(pThumbItem->m_pObj->IsGObjInstanceOf(gobjButton))
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_BUTTON_OBJ);
			}
			else if(pThumbItem->m_pObj->IsGObjInstanceOf(gobjSound))
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_SOUND_OBJ);
			}
			else
			{
				strName = " ";
			}
		}
	}
	else
	{
		strName = FileStripPath(pThumbItem->m_strPathName);
	}

	pDC->FillSolidRect(pRect, GetSysColor(COLOR_WINDOW));
	CFont Font;
	Font.Attach(GetStockObject(DEFAULT_GUI_FONT));
	CFont* pOldFont = pDC->SelectObject(&Font);

	CRect rcText(pRect);
	rcText.top = rcText.bottom - pDC->GetTextExtent(strName).cy - 2;
	if(GetCurSel()==index)
	{
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(GetSysColor(GetFocus()==this ? COLOR_HIGHLIGHT : COLOR_BTNFACE));
		pDC->SetTextColor(GetSysColor(GetFocus()==this ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
	}
	else
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	}

	pDC->DrawText(strName, CRect(&rcText), DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
	pDC->SelectObject(pOldFont);
	Font.Detach();
    
	//画边框
	CRect rcThumb(pRect);
	rcThumb.bottom = rcText.top;
	int nBorderThick = 2;
	if(GetCurSel()==index)
	{
		CBrush Brush(GetSysColor(GetFocus()==this ? COLOR_HIGHLIGHT : COLOR_3DFACE));
		for(int i = 0; i < nBorderThick; ++i)
		{
			pDC->FrameRect(&rcThumb, &Brush);
			rcThumb.DeflateRect(1,1);
		}
	}
	else
	{
		CBrush Brush(GetSysColor(COLOR_3DFACE));
		rcThumb.DeflateRect(nBorderThick-1, nBorderThick-1);
		pDC->FrameRect(&rcThumb, &Brush);
		rcThumb.DeflateRect(1,1);
	}
 	rcThumb.DeflateRect(2, 2);

	pThumbItem->Render(pDC, rcThumb);
}

void CResourceThumbListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(CDesignWnd::Instance()->CanDraw(false))
	{
		int index = ItemFromPoint(point);
		if(index>=0)
		{
			if(m_pActionObject)
			{
				m_pActionObject->OnResourceThumbAction(index, this);
			}
		}
	}

	CFlexListCtrl::OnLButtonDblClk(nFlags, point);
}

void CResourceThumbListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CFlexListCtrl::OnKillFocus(pNewWnd);

	if(GetCurSel()>=0)
	{
		RedrawItem(GetCurSel());
	}

}

void CResourceThumbListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CFlexListCtrl::OnSetFocus(pOldWnd);

	if(GetCurSel()>=0)
	{
		RedrawItem(GetCurSel());
	}
}

void CResourceThumbListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CFlexListCtrl::OnLButtonDown(nFlags, point);

	int index = GetCurSel();

	if(index >= 0 && m_bEnableDragResource)
	{
		CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(index);
		if(pThumbItem->m_bStockResource)
		{
			return ;
		}

		SetFocus();
		SetCapture();

		MSG msg;
		while (GetMessage(&msg, 0, 0, 0))
		{
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
				{
					CPoint pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
					if(max(pt.x, point.x) - min(pt.x, point.x)>GetSystemMetrics(SM_CXDRAG) || max(pt.y, point.y) - min(pt.y, point.y)>GetSystemMetrics(SM_CYDRAG))
					{
						ReleaseCapture();
						CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(index);
						HGLOBAL hData = ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DROPFILES) + pThumbItem->m_strPathName.GetLength() + 2);
						if (hData != NULL)
						{
							DROPFILES *pDrop = (DROPFILES *)::GlobalLock(hData);
							if (pDrop)
							{
								pDrop->pFiles = sizeof(DROPFILES);
								TCHAR *pszFile = (TCHAR *)pDrop + sizeof(DROPFILES);
								_tcscpy(pszFile, (LPCTSTR)pThumbItem->m_strPathName);

								::GlobalUnlock(hData);

								COleDataSource ds;
								ds.CacheGlobalData(CF_HDROP, hData);
								DROPEFFECT de = ds.DoDragDrop(DROPEFFECT_COPY);
							}
						}
						return ;
					}
				}
				continue;

			case WM_LBUTTONUP:
				{
					ReleaseCapture();
					return;
				}
				break;

				// 过滤右键及滚轮事件
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
				continue;

				// 系统消息退出
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				ReleaseCapture();
				break;

			default:
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			// 若失去捕获,则退出
			if (GetCapture() != this)
				break;
		}
	}
}

void CResourceThumbListCtrl::ClearThumbBuffer()
{
	for(int i = 0, j = GetItemCount(); i<j; ++i)
	{
		CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(i);
		pThumbItem->ReleaseBuffer();
	}
}


BOOL CResourceThumbListCtrl::GetToolTipText(int index, CString &strText)
{
	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(index);

	CString strName;
	int nResourceType = pThumbItem->GetResourceType();
	if(pThumbItem->m_bStockResource)
	{
		strName = pThumbItem->m_pObj->m_name.c_str();
	}
	else
	{
		strName = FileStripPath(pThumbItem->m_strPathName);
	}

	if(pThumbItem->IsValidResource()==FALSE)
	{
		strText = strName;
		return strText.IsEmpty() == FALSE;
	}

	switch(nResourceType)
	{
	case RESOURCE_IMAGES:
		{
			if(strName.IsEmpty())
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_IMAGE_OBJ);
			}

			CString strDimensions;
			strDimensions.Format("%d x %d", pThumbItem->GetDimensions().cx, pThumbItem->GetDimensions().cy);
			CString s;
			s.LoadString(IDS_THUMB_TIP_IMAGE);
			strText.Format(s, strName, (LPCTSTR)strDimensions, round((float)pThumbItem->GetDataLen()/1024));
		}
		break;
	case RESOURCE_SHAPES:
		{
			if(strName.IsEmpty())
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_SHAPE_OBJ);
			}

			CString strDimensions;
			strDimensions.Format("%d x %d", pThumbItem->GetDimensions().cx, pThumbItem->GetDimensions().cy);
			CString s;
			s.LoadString(IDS_THUMB_TIP_SHAPE);
			strText.Format(s, strName, (LPCTSTR)strDimensions, round((float)pThumbItem->GetDataLen()/1024));
		}
		break;
	case RESOURCE_BUTTONS:
		{
			if(strName.IsEmpty())
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_BUTTON_OBJ);
			}

			CString strDimensions;
			strDimensions.Format("%d x %d", pThumbItem->GetDimensions().cx, pThumbItem->GetDimensions().cy);
			CString s;
			s.LoadString(IDS_THUMB_TIP_BUTTON);
			strText.Format(s, strName, (LPCTSTR)strDimensions, round((float)pThumbItem->GetDataLen()/1024));

			CString strDesc = pThumbItem->GetDesc();
			if (!strDesc.IsEmpty())
				strText += "\n\n" + strDesc;
		}
		break;

	case RESOURCE_MOVIECLIP:
		{
			if(strName.IsEmpty())
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_MOVIECLIP_OBJ);
			}

			CString strDimensions;
			strDimensions.Format("%d x %d", pThumbItem->GetDimensions().cx, pThumbItem->GetDimensions().cy);
			CString s;
			s.LoadString(IDS_THUMB_TIP_BUTTON);
			strText.Format(s, strName, (LPCTSTR)strDimensions, round((float)pThumbItem->GetDataLen()/1024));

			CString strDesc = pThumbItem->GetDesc();
			if (!strDesc.IsEmpty())
				strText += "\n\n" + strDesc;
		}
		break;

	case RESOURCE_SOUNDS:
		{
			if(strName.IsEmpty())
			{
				strName.LoadString(IDS_THUMB_TIP_UNNAMED_SOUND_OBJ);
			}

			double dDuration = pThumbItem->GetDuration();
			int hours	= int(dDuration) / 3600;
			int minutes = int(dDuration) % 3600 / 60;
			int seconds = int(dDuration) % 60;
			CString strDuration;
			strDuration.Format("%d:%02d:%02d", hours, minutes, seconds);

			CString s;
			s.LoadString(IDS_THUMB_TIP_SOUND);
			strText.Format(s, strName, (LPCTSTR)strDuration, round((float)pThumbItem->GetDataLen()/1024));
		}
		break;
	default:
		strText = strName;
		break;
	}

	return strText.IsEmpty() == FALSE;
}


void CResourceThumbListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CPoint pt(point);
	ScreenToClient(&pt);

	int index = ItemFromPoint(pt);

	if(index >= 0)
	{
		CResourceThumbItem* pThumbItem= (CResourceThumbItem*)GetItemData(index);

		CMenu menu;
		menu.LoadMenu(IDR_POPUP_RESOURCE_THUMB_ITEM);

		CMenu* pPopup = menu.GetSubMenu(0);
		if(pPopup)
		{
			const UINT ITEM_EXPLORE		= 8;
			const UINT ITEM_DELETE		= 5;
			const UINT ITEM_COPY		= 4;
			const UINT ITEM_PLAYSOUND	= 2;
			const UINT ITEM_ADDTOSCENE	= 0;

			// 出现以下错误是因为您修改了菜单资源IDR_POPUP_RESOURCE_THUMB_ITEM, 请重新设置以上的参数
			ASSERT(pPopup->GetMenuItemID(ITEM_EXPLORE)==ID_THUMB_EXPLORE);
			ASSERT(pPopup->GetMenuItemID(ITEM_DELETE)==ID_THUMB_DELETE);
			ASSERT(pPopup->GetMenuItemID(ITEM_COPY)==ID_THUMB_COPY);
			ASSERT(pPopup->GetMenuItemID(ITEM_PLAYSOUND)==ID_THUMB_PLAY_SOUND);
			ASSERT(pPopup->GetMenuItemID(ITEM_ADDTOSCENE)==ID_THUMB_ADD_TO_SCENE);
			if(pThumbItem->m_bStockResource)
			{
				pPopup->DeleteMenu(ITEM_EXPLORE + 1, MF_BYPOSITION);
				pPopup->DeleteMenu(ITEM_EXPLORE, MF_BYPOSITION);
				pPopup->DeleteMenu(ITEM_DELETE, MF_BYPOSITION);
				pPopup->DeleteMenu(ITEM_COPY, MF_BYPOSITION);
			}

			if(pThumbItem->GetResourceType() != RESOURCE_SOUNDS)
			{
				pPopup->DeleteMenu(ITEM_PLAYSOUND + 1, MF_BYPOSITION);
				pPopup->DeleteMenu(ITEM_PLAYSOUND, MF_BYPOSITION);
			}

			if(m_bEnableDragResource == FALSE)
			{
				pPopup->DeleteMenu(ITEM_ADDTOSCENE + 1, MF_BYPOSITION);
				pPopup->DeleteMenu(ITEM_ADDTOSCENE, MF_BYPOSITION);
			}

			pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
}

void CResourceThumbItemImportAction::OnResourceThumbAction(int nIndex, CResourceThumbListCtrl* pThumbListCtrl)
{
	CResourceThumbItem* pThumbItem = ((CResourceThumbItem*)(pThumbListCtrl->GetItemData(nIndex)));
	if(pThumbItem->m_bStockResource)
	{
		return ;
	}

	_LOG1("ImportResource:\t%s", (LPCTSTR)pThumbItem->m_strPathName);

	CGlandaDoc *pDoc = _GetCurDocument();
	CSize size = my_app.GetViewSize();
	CPoint point2(size.cx / 2, size.cy / 2);

	switch(pThumbItem->GetResourceType())
	{
	case RESOURCE_IMAGES:
		pDoc->ImportImage(pThumbItem->m_strPathName, point2, TRUE);
		break;
	case RESOURCE_SHAPES:
		{
			CString strExt = ::PathFindExtension(pThumbItem->m_strPathName);
			if(strExt.CompareNoCase(".gls")==0)
			{
				pDoc->ImportGLS(pThumbItem->m_strPathName, point2);
			}
			else if(strExt.CompareNoCase(".wmf")==0
				|| strExt.CompareNoCase(".emf")==0
				|| strExt.CompareNoCase(".svg")==0
				|| strExt.CompareNoCase(".ai")==0)
			{
				pDoc->ImportVectorFormat(pThumbItem->m_strPathName, point2);
			}
		}
		break;

	case RESOURCE_BUTTONS:
	case RESOURCE_MOVIECLIP:
		pDoc->ImportGLC(pThumbItem->m_strPathName, point2);
		break;

	case RESOURCE_SOUNDS:
		pDoc->ImportSound(pThumbItem->m_strPathName);
		break;
	default:
		ASSERT(0);
		break;
	}
}

int CResourceThumbListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFlexListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_tooltip.SetMaxTipWidth(256);

	return 0;
}

void CResourceThumbListCtrl::OnUpdateThumbAddToScene(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}

void CResourceThumbListCtrl::OnThumbAddToScene()
{
	ASSERT(m_pActionObject);

	if(_GetCurScene2())
	{
		int index = GetCurSel();
		m_pActionObject->OnResourceThumbAction(index, this);
	}
}

void CResourceThumbListCtrl::OnThumbPlaySound()
{
	int index = GetCurSel();

	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(index);
	if(pThumbItem->m_bStockResource)
	{
		gldSound* pSound = (gldSound*)pThumbItem->m_pObj;
		CPlaySoundDialog dlg(pSound);
		dlg.DoModal();
	}
	else
	{
		_LOG1("PlaySound:\t%s", (LPCTSTR)pThumbItem->m_strPathName);
		if(::PathFileExists(pThumbItem->m_strPathName))
		{
			CAutoPtr<CWaitCursor> wc(new CWaitCursor());
			gldSound sound;
			if(sound.ReadFile(pThumbItem->m_strPathName))
			{
				wc.Free();
				sound.m_name = ::FileStripPath(pThumbItem->m_strPathName);
				CPlaySoundDialog dlg(&sound);
				dlg.DoModal();
			}
			else
			{
				wc.Free();
				AfxMessageBox(IDS_ERROR_IMPORT, MB_OK|MB_ICONERROR);
			}
		}
	}
}

void CResourceThumbListCtrl::OnThumbCopy()
{
	int nCurSel = GetCurSel();
	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(nCurSel);

	ASSERT(pThumbItem->m_bStockResource==FALSE);

	CString strPath = pThumbItem->m_strPathName;

	if (::OpenClipboard(m_hWnd))
	{
		if (::EmptyClipboard())
		{
			HANDLE hMem = ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DROPFILES) + strPath.GetLength() + 2);
			if (hMem)
			{
				DROPFILES *pDrop = (DROPFILES *)::GlobalLock(hMem);
				if (pDrop)
				{
					pDrop->pFiles = sizeof(DROPFILES);
					TCHAR *pszFile = (TCHAR *)pDrop + sizeof(DROPFILES);
					_tcscpy(pszFile, strPath);

					::GlobalUnlock(hMem);

					::SetClipboardData(CF_HDROP, hMem);
				}
			}
		}

		::CloseClipboard();
	}
	else
	{
		ReportLastError();
	}
}

void CResourceThumbListCtrl::OnThumbDelete()
{
	int nCurSel = GetCurSel();

	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(nCurSel);

	ASSERT(pThumbItem->m_bStockResource == FALSE);

	if(::PathFileExists(pThumbItem->m_strPathName))
	{
		if(::DeleteFile(pThumbItem->m_strPathName))
		{
			CString strPathName(pThumbItem->m_strPathName);
			CSubjectManager::Instance()->GetSubject("DeleteFile")->Notify((void*)&strPathName);
		}
	}
}

void CResourceThumbListCtrl::OnThumbRename()
{
	int nCurSel = GetCurSel();

	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(nCurSel);
	CResourceThumbRenameDialog dlg(pThumbItem);
	dlg.DoModal();
}

void CResourceThumbListCtrl::OnThumbExplore()
{
	int nCurSel = GetCurSel();

	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(nCurSel);

	ASSERT(pThumbItem->m_bStockResource==FALSE);

	CString strParameters;
	strParameters.Format("/n,/select,%s", pThumbItem->m_strPathName);
	ShellExecute(m_hWnd, "open", "explorer.exe", strParameters, NULL, SW_SHOWNORMAL);
}

void CResourceThumbListCtrl::OnThumbProperties()
{
	int nCurSel = GetCurSel();

	CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(nCurSel);
	CPropertySheet sheet("Properties");
	sheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	if(pThumbItem->m_bStockResource)
	{
		CResourceThumbObjPropertiesDialog page(pThumbItem);
		sheet.AddPage(&page);
		sheet.DoModal();
	}
	else
	{
		CResourceThumbFilePropertiesDialog page(pThumbItem);
		sheet.AddPage(&page);
		sheet.DoModal();
	}
}

IMPLEMENT_OBSERVER(CResourceThumbListCtrl, DeleteFile)
{
	CString& strPathName = *(CString*)pData;
	for(int i = GetItemCount() - 1; i >= 0; --i)
	{
		CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(i);
		if(pThumbItem->m_bStockResource==FALSE)
		{
			if(AfxComparePath(pThumbItem->m_strPathName, strPathName))
			{
				DeleteItem(i);
				break;
			}
		}
	}
}

IMPLEMENT_OBSERVER(CResourceThumbListCtrl, MoveFile)
{
	int nCurSel = GetCurSel();
	CString* strPathName = (CString*)pData;
	for(int i = GetItemCount() - 1; i >= 0; --i)
	{
		CResourceThumbItem* pThumbItem = (CResourceThumbItem*)GetItemData(i);
		if(pThumbItem->m_bStockResource==FALSE)
		{
			if(AfxComparePath(pThumbItem->m_strPathName, strPathName[0]))
			{
				DeleteItem(i);
				if(CResourceThumbItem::GetResourceType(strPathName[1]) & m_nResourceType)
				{
					CResourceThumbItem* pThumbItem = new CResourceThumbItem(strPathName[1]);
					InsertItem(i, (DWORD_PTR)pThumbItem);
					if(i==nCurSel && GetCurSel() != i)
					{
						SelectItem(i);
					}
				}
				break;
			}
		}
	}
}

void CResourceThumbListCtrl::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFlexListCtrl::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	UpdateMenuCmdUI(this, pPopupMenu, nIndex, bSysMenu);
}
