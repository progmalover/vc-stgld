#pragma once


// CResourceThumbListCtrl
#include "FlexListCtrl.h"
#include <Gdiplus.h>
#include <atlimage.h>
#include "glanda_i.h"
#include "ResourceThumbItem.h"
#include "Observer.h"


class CResourceThumbItem;
class gldSound;
class gldObj;

class CResourceThumbListCtrl;

template <class T>
class CItemActionImpl
{
public:
	virtual void OnResourceThumbAction(int nIndex, T* pThumbListCtrl) = 0;
};

template <class T>
class CItemActionDecl
{
protected:
	CItemActionImpl<T>* m_pActionObject;
public:
	CItemActionDecl()
		: m_pActionObject(NULL)
	{
	}
	void SetActionObject(CItemActionImpl<T>* pObject)
	{
		m_pActionObject = pObject;
	}
};

typedef CItemActionImpl<CResourceThumbListCtrl> CResourceThumbItemAction;

class CResourceThumbListCtrl
	: public CFlexListCtrl
	, public CItemActionDecl<CResourceThumbListCtrl>
{
protected:
	DECLARE_GLANDA_OBSERVER(CResourceThumbListCtrl, DeleteFile)
	DECLARE_GLANDA_OBSERVER(CResourceThumbListCtrl, MoveFile)
public:
	CResourceThumbListCtrl(int nResourceType);
	virtual ~CResourceThumbListCtrl();

	void SetResourceDir(LPCTSTR lpszResourceDir);						// 设置资源目录
	void SetLibraryItems();
	void ClearThumbBuffer();											// 清除缩略图的缓存
	void EnableDragResouurce(BOOL bEnable)
	{
		m_bEnableDragResource = bEnable;
	}

private:
//	DECLARE_DYNAMIC(CResourceThumbListCtrl)

	BOOL m_bEnableDragResource;
	int m_nResourceType;			// 资源类型

protected:
	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);
	virtual void OnDeleteItem(int index);
	virtual BOOL GetToolTipText(int index, CString &strText);

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnThumbPlaySound();
	afx_msg void OnThumbCopy();
	afx_msg void OnThumbDelete();
	afx_msg void OnThumbRename();
	afx_msg void OnThumbExplore();
	afx_msg void OnThumbProperties();
	afx_msg void OnThumbAddToScene();
	afx_msg void OnUpdateThumbAddToScene(CCmdUI* pCmdUI);
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
};

class CResourceThumbItemImportAction : public CResourceThumbItemAction
{
public:
	virtual void OnResourceThumbAction(int nIndex, CResourceThumbListCtrl* pThumbListCtrl);
};
