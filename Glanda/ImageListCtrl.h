#pragma once

#include "FlexListCtrl.h"

class gldImage;

// CImageListCtrl

class CImageListCtrl : public CFlexListCtrl
{
protected:
	class CItemData
	{
	public:
		CItemData()
		{
			pImage = NULL;
			hBitmap = NULL;
		}
		~CItemData()
		{
			if (hBitmap)
				::DeleteObject(hBitmap);
		}
		gldImage *pImage;
		HBITMAP hBitmap;
	};

public:
	CImageListCtrl();
	virtual ~CImageListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);
	virtual void OnDeleteItem(int index);
	virtual BOOL GetToolTipText(int index, CString &strText);
public:
	int InsertImage(int index, gldImage * pImage);
	int AddImage(gldImage *pImage);
	gldImage *GetImage(int index);
	int FindImage(gldImage *pImage);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


