#pragma once


// COwnerDrawListBox

class COwnerDrawListBox : public CListBox
{
public:
	COwnerDrawListBox();
	virtual ~COwnerDrawListBox();

	class CItemData
	{
	public:
		CItemData()
		{
			m_nImage = -1;
			m_bSysColor = TRUE;
			m_crText = COLOR_WINDOWTEXT;
			m_bBold = FALSE;
			m_data = NULL;
		}
		COLORREF GetTextColor()
		{
			return m_bSysColor ? ::GetSysColor((int)m_crText) : m_crText;
		}
		int m_nImage;
		BOOL m_bSysColor;
		COLORREF m_crText;
		BOOL m_bBold;
		DWORD_PTR m_data;
	};

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL LoadImages(UINT nIDBitmap, int cx, COLORREF crMask);
	BOOL LoadImages(UINT nIDBitmap, int nFlags, int cx, int cy, COLORREF crMask);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void SetItemColor(int index, BOOL bSysColor, COLORREF color);
	void SetItemBold(int index, BOOL bBold);
	void SetItemImage(int index, int nImage);
	int GetItemImage(int index);
	void RedrawItem(int index);
	DWORD_PTR SetItemUserData(int index, DWORD_PTR dwData);
	DWORD_PTR GetItemUserData(int index);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	CImageList m_imgList;
};
