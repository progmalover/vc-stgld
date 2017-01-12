#pragma once

#include <vector>
// CHierarchyPopup

class CHierarchyPopup : public CWnd
{
	DECLARE_DYNAMIC(CHierarchyPopup)

public:
	enum {SEPERATOR = -2};
	enum {SET, CANCEL};
	enum {MSG_SELCHANGED = WM_APP + 1};
	enum {SELECT = 0x01, FOCUS = 0x02, DISABLED = 0x04};

public:
	CHierarchyPopup();
	virtual ~CHierarchyPopup();

	CImageList *SetImageList(CImageList *pIL);
	CImageList *GetImageList() const;
	int AddItem(const CString &text, int level = 0, int icon = -1, BOOL disabled = FALSE, LPARAM lParam = 0);	
	void Clear();
	void RemoveItem(int index);
	int GetCount() const;
	int ItemFromPoint(const CPoint &point, BOOL disabled = FALSE) const;
	CRect GetItemRect(int index) const;
	void Popup(CWnd *owner, int x, int y, int cx, int cy);
	void Close(int result, int code = 0);
	void DrawItem(CDC *pDC, int item, const CRect &rect, UINT state = 0, BOOL indent = TRUE);	
	int NextItem(int item) const;
	int PrevItem(int item) const;
	void SetCurSel(int item);
	int GetItemHeight() const;
	int ItemFromChar(const CString &str, int item = -1) const;
	void SetItemData(int item, LPARAM data);
	LPARAM GetItemData(int item) const;

private:
	struct ItemData
	{
		CString _text;
		int _level;
		int _icon;
		LPARAM _lParam;
		UINT _state;
		ItemData(const CString &text, int level = 0, int icon = -1, BOOL disabled = FALSE, LPARAM lParam = 0)
			: _text(text), _level(level), _icon(icon), _state(disabled ? DISABLED : 0), _lParam(lParam)
		{
		}
	};
	typedef std::vector<ItemData *> Items;
	typedef Items::iterator ItemIter;

	enum {DEFAULT_ITEM_HEIGHT = 16};
	enum {ID_HIERARCHYPOPUP = 1001};
	enum {DEFAULT_INDENT = 12};

private:
	Items _items;
	int _item_height;
	int _top_item;
	int _cur_item;
	CImageList *_image_list;
	int _cx_icon;
	int _cy_icon;	
	UINT _timer;

private:
	void EnsureItemVisible(int item);
	void RefreshScrollBar();
	int GetIndent(int level);
	void DrawSeperator(CDC *pDC, int item, const CRect &rect);	
	BOOL IsItemDisabled(int index) const;	
	void InitScrollBar();
	BOOL SetHooks();
	void Unhooks();
	BOOL IsVisible() const;
	void SetTopItem(int pos);
	int GetVisibleItems() const;

private:
	static HHOOK _mouse_hook;
	static HHOOK _wnd_hook;
	static HHOOK _kb_hook;
	static CHierarchyPopup *_this;
	static LRESULT CALLBACK MouseProc(UINT nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CallWndProc(UINT nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK KeyboardProc(UINT nCode, WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);	
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
};


