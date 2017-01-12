#pragma once


// COwnerDrawComboBox

class COwnerDrawComboBox : public CComboBox
{
public:
	COwnerDrawComboBox();
	virtual ~COwnerDrawComboBox();

	class CItemData
	{
	public:
		CItemData()
		{
			m_bSysColor = TRUE;
			m_crText = COLOR_WINDOWTEXT;
			m_bBold = FALSE;
		}
		COLORREF GetTextColor()
		{
			return m_bSysColor ? ::GetSysColor((int)m_crText) : m_crText;
		}
		BOOL m_bSysColor;
		COLORREF m_crText;
		BOOL m_bBold;
	};

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void SetItemColor(int index, BOOL bSysColor, COLORREF color);
	void SetItemBold(int index, BOOL bBold);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
protected:
	virtual void PreSubclassWindow();
};


