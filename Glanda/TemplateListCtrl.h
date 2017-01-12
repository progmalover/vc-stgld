#pragma once

#include "FlexListCtrl.h"
#include <vector>

// CTemplateListCtrl

class gldTemplateInfo
{
public:
	gldTemplateInfo(REFCLSID _clsid)
	{
		clsid = _clsid;
		hBitmap = NULL;
		dwPriority = (DWORD)-1;
		bSelected = FALSE;
	}
	~gldTemplateInfo()
	{
		if (hBitmap)
			::DeleteObject(hBitmap);
	}

	BOOL LoadTemplateInfo();

public:
	CString strName;
	CString strDescription;
	HBITMAP hBitmap;
	CLSID clsid;
	DWORD dwPriority;
	BOOL bSelected;
};

typedef std::vector<gldTemplateInfo *> GTEMPLATEINFO_LIST;

class CTemplateListCtrl : public CFlexListCtrl
{
public:
	CTemplateListCtrl();
	virtual ~CTemplateListCtrl();
	enum
	{
		IMAGE_WIDTH = 100, 
		IMAGE_HEIGHT = 100, 
		IMAGE_TEXT_SPACING = 10, 
		IMAGE_BORDER = 1,
		TEXT_PADDING = 1
	};

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);
	virtual void OnDeleteItem(int index);
	virtual BOOL GetToolTipText(int index, CString &strText);
public:
	void CalcItemSize();
	int AddTemplate(gldTemplateInfo *pInfo);
	REFCLSID GetTemplate(int index);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
