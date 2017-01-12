// HierarchyPopup.cpp : implementation file
//

#include "stdafx.h"
#include "HierarchyPopup.h"
#include "SysParas.h"
#include ".\hierarchypopup.h"

// CHierarchyPopup

IMPLEMENT_DYNAMIC(CHierarchyPopup, CWnd)

// public interface
CHierarchyPopup::CHierarchyPopup()
{
	_item_height = DEFAULT_ITEM_HEIGHT;
	_top_item = 0;
	_cur_item = -1;
	_image_list = NULL;
	_cx_icon = 0;
	_cy_icon = 0;
	_timer = 0;
}

CHierarchyPopup::~CHierarchyPopup()
{
	Clear();
}

CImageList *CHierarchyPopup::SetImageList(CImageList *pIL)
{
	CImageList *il = _image_list;
	_image_list = pIL;
	if (pIL != NULL)
	{
		IMAGEINFO ii;
		memset(&ii, 0, sizeof(ii));
		pIL->GetImageInfo(0, &ii);
		_cx_icon = ii.rcImage.right - ii.rcImage.left;
		_cy_icon = ii.rcImage.bottom - ii.rcImage.top;
	}
	return il;
}

CImageList *CHierarchyPopup::GetImageList() const
{
	return _image_list;
}

int CHierarchyPopup::AddItem(const CString &text, int level /*= 0*/, int icon /*= -1*/, BOOL disabled /*= FALSE*/, LPARAM lParam /*= 0*/)
{
	ItemData *item = new ItemData(text, level, icon, disabled, lParam);
	ASSERT(item != NULL);
	_items.push_back(item);
	RefreshScrollBar();	
	return (int)_items.size() - 1;
}

void CHierarchyPopup::Clear()
{
	for (ItemIter ii = _items.begin(); ii != _items.end(); ++ii)
	{
		delete *ii;
	}
	_items.clear();
	RefreshScrollBar();
}

void CHierarchyPopup::RemoveItem(int index)
{
	if (index < 0 || index >= (int)_items.size())
	{	
		return;
	}
	delete _items[index];
	_items.erase(_items.begin() + index);
}

int CHierarchyPopup::GetCount() const
{
	return (int)_items.size();
}

int CHierarchyPopup::ItemFromPoint(const CPoint &point, BOOL disabled /*= FALSE*/) const
{
	CRect rect;
	GetClientRect(&rect);
	if (!rect.PtInRect(point))
	{
		return -1;
	}
	int item = point.y / _item_height + _top_item;
	if (item < (int)_items.size())
	{
		if (disabled)
		{
			return item;
		}
		else
		{
			return IsItemDisabled(item) ? -1 : item;
		}
	}
	else
	{
		return -1;
	}
}

CRect CHierarchyPopup::GetItemRect(int index) const
{
	CRect rect;
	GetClientRect(&rect);
	rect.top += (index - _top_item) * _item_height;
	rect.bottom = rect.top + _item_height;
	return rect;
}

void CHierarchyPopup::Popup(CWnd *owner, int x, int y, int cx, int cy)
{
	ASSERT(m_hWnd == NULL);

	if (owner == NULL)
	{
		owner = AfxGetMainWnd();
	}
	ASSERT(owner != NULL);

	LPCTSTR wnd_class = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW);
	VERIFY(CreateEx(WS_EX_TOPMOST, wnd_class, NULL, WS_BORDER | WS_POPUP
		, x, y, cx, cy, owner->m_hWnd, NULL, NULL));
	SetOwner(owner);
	InitScrollBar();	
	VERIFY(SetWindowPos(NULL, x, y, cx, cy, SWP_NOACTIVATE | SWP_SHOWWINDOW));
	VERIFY(SetHooks());	
}

void CHierarchyPopup::Close(int result, int code /*= 0*/)
{
	ASSERT(result == SET || result == CANCEL);
	ASSERT(m_hWnd != NULL);

	CWnd *owner = GetOwner();
	ASSERT(owner != NULL);

	Unhooks();
	DestroyWindow();

	if ((owner != NULL) && (result == SET))
	{
		owner->SendMessage(MSG_SELCHANGED, 0, code);
	}
}

void CHierarchyPopup::DrawItem(CDC *pDC, int item, const CRect &rect, UINT state /*= 0*/, BOOL indent /*= TRUE*/)
{
	ItemData *data = _items[item];
	ASSERT(data->_icon != SEPERATOR);
	CRect rect_item = rect;
	if (state & FOCUS) // item is focus
	{
		pDC->DrawFocusRect(rect_item);		
	}
	rect_item.DeflateRect(1, 1, 1, 1);
	if (state & SELECT) // item is selected
	{
		pDC->FillSolidRect(rect_item, SysParas.cl_hilight);
	}
	else
	{
		pDC->FillSolidRect(rect_item, SysParas.cl_window);
	}
	int n_indent = indent ? GetIndent(data->_level) : 0;
	int left = rect_item.left + n_indent;
	CRect rect_text(left + 2, rect_item.top
		, rect_item.right, rect_item.bottom);
	if (_image_list != NULL)
	{
		int icon_top = rect_item.top 
			+ (rect_item.Height() - _cy_icon) / 2;
		_image_list->Draw(pDC, data->_icon
			, CPoint(left, icon_top), ILD_TRANSPARENT);
		rect_text.left += _cx_icon;
	}
	if (IsItemDisabled(item))
	{
		pDC->SetTextColor(SysParas.cl_gray_text);
	}
	else
	{
		if (state & SELECT)
		{
			pDC->SetTextColor(SysParas.cl_hilight_text);
		}
		else
		{
			pDC->SetTextColor(SysParas.cl_text);
		}
	}
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(data->_text, rect_text, DT_SINGLELINE | DT_VCENTER);	
}

int CHierarchyPopup::NextItem(int item) const
{
	if (GetCount() == 0)
	{
		return -1;
	}
	if (item < -1) 
	{
		item = -1;
	}
	int next = item + 1;
	while (next < GetCount() && IsItemDisabled(next))
	{
		next++;
	}
	return next < GetCount() ? next : -1;
}

int CHierarchyPopup::PrevItem(int item) const
{
	if (GetCount() == 0)
	{
		return -1;
	}
	if (item > GetCount())
	{
		item = GetCount();
	}
	int prev = item - 1;
	while (prev > -1 && IsItemDisabled(prev))
	{
		prev--;
	}
	return prev > -1 ? prev : -1;
}

void CHierarchyPopup::SetCurSel(int item)
{	
	if (item == _cur_item || IsItemDisabled(item))
	{
		return;
	}

	if (IsVisible())
	{
		EnsureItemVisible(item);

		int old_item = _cur_item;
		_cur_item = item;
		CDC *pDC = GetDC();
		CFont *old_font = pDC->SelectObject(SysParas.font_default);
		if (old_item != -1)
		{
			DrawItem(pDC, old_item, GetItemRect(old_item), 0, TRUE);
		}
		if (item != -1)
		{
			DrawItem(pDC, item, GetItemRect(item), SELECT, TRUE);
		}
		pDC->SelectObject(old_font);
		ReleaseDC(pDC);
	}
	else
	{
		_cur_item = item;
	}
}

int CHierarchyPopup::GetItemHeight() const
{
	return _item_height;
}

/* this method is implement ugly */
int CHierarchyPopup::ItemFromChar(const CString &str, int item /*= -1*/) const
{
	for (int i = __max(0, item + 1); i < (int)_items.size(); i++)
	{
		if (IsItemDisabled(i))
		{
			ItemData *data = _items[i];
			int n_len = str.GetLength();
			if (data->_text.GetLength() >= n_len)
			{
				CString str_cmp = data->_text.Left(n_len);
				if (_tcsicmp(str_cmp, str) == 0)
				{
					return i;
				}
			}
		}		
	}
	return -1;
}

void CHierarchyPopup::SetItemData(int item, LPARAM data)
{
	ASSERT(item > -1 && item < GetCount());
	if (item > -1 && item < GetCount())
	{
		ItemData *p = _items[item];
		p->_lParam = data;
	}
}

LPARAM CHierarchyPopup::GetItemData(int item) const
{
	ASSERT(item > -1 && item < GetCount());
	if (item > -1 && item < GetCount())
	{
		return _items[item]->_lParam;
	}
	return 0;
}

// private implement
void CHierarchyPopup::InitScrollBar()
{
	ASSERT(m_hWnd != NULL);
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;	
	si.nMin = 0;	
	si.nPage = GetVisibleItems();
	si.nMax = (int)_items.size() - 1;
	si.nPos = _top_item;
	VERIFY(SetScrollInfo(SB_VERT, &si, FALSE));
}

void CHierarchyPopup::RefreshScrollBar()
{
	if (m_hWnd == NULL) return;

	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	VERIFY(GetScrollInfo(SB_VERT, &si));
	si.fMask = 0;
	int n_max = (int)_items.size() - 1;
	if (si.nMax != n_max)
	{
		si.nMax = n_max;
		si.fMask |= SIF_RANGE;
	}
	if (si.nPos > n_max)
	{
		si.nPos = n_max;
		si.fMask |= SIF_POS;
	}
	if (si.fMask != 0)
	{
		SetScrollInfo(SB_VERT, &si, TRUE);
	}
}

int CHierarchyPopup::GetIndent(int level)
{
	if (_cx_icon == 0)
	{
		return DEFAULT_INDENT * level;
	}
	return _cx_icon * level;
}

void CHierarchyPopup::DrawSeperator(CDC *pDC, int item, const CRect &rect)
{
	ItemData *data = _items[item];
	ASSERT(data->_icon == SEPERATOR);
	int n_indent = GetIndent(data->_level);
	int left = rect.left + n_indent;
	int center = (rect.top + rect.bottom) / 2;
	if (left < rect.right)
	{
		CPen sep_pen(PS_SOLID, 1, SysParas.cl_gray_text);
		CPen *old_pen = pDC->SelectObject(&sep_pen);
		if (data->_text.GetLength() == 0)
		{	
			pDC->MoveTo(left, center);
			pDC->LineTo(rect.right, center);
		}
		else
		{
			CSize size_text = pDC->GetTextExtent(data->_text);
			CRect rect_text = rect;
			rect_text.left = left + (rect.Width() - size_text.cx) / 2;
			rect_text.right = rect_text.left + size_text.cx;
			pDC->SetTextColor(SysParas.cl_gray_text);
			pDC->SetBkMode(TRANSPARENT);
			pDC->DrawText(data->_text, rect_text, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			if (left < rect_text.left)
			{
				pDC->MoveTo(left, center);
				pDC->LineTo(rect_text.left, center);
			}
			if (rect_text.right < rect.right)
			{
				pDC->MoveTo(rect_text.right, center);
				pDC->LineTo(rect.right, center);
			}
		}
		pDC->SelectObject(old_pen);
	}	
}

BOOL CHierarchyPopup::IsItemDisabled(int index) const
{
	if (index < 0 || index >= (int)_items.size()) 
	{
		return TRUE;
	}

	ItemData *data = _items[index];

	return (data->_icon == SEPERATOR) || (data->_state & DISABLED);
}

BOOL CHierarchyPopup::SetHooks()
{
	ASSERT(m_hWnd != NULL);
	ASSERT(_mouse_hook == NULL);
	HINSTANCE inst = AfxGetInstanceHandle();
	DWORD thread_id = GetCurrentThreadId();
	_mouse_hook = SetWindowsHookEx(
		WH_MOUSE, (HOOKPROC)MouseProc
		, inst, thread_id);		
	ASSERT(_mouse_hook != NULL);
	if (_mouse_hook == NULL) return FALSE;
	ASSERT(_wnd_hook == NULL);
	_wnd_hook = SetWindowsHookEx(
		WH_CALLWNDPROC, (HOOKPROC)CallWndProc
		, inst, thread_id);
	ASSERT(_wnd_hook != NULL);
	if (_wnd_hook == NULL)
	{
		UnhookWindowsHookEx(_mouse_hook);
		return FALSE;
	}	
	ASSERT(_kb_hook == NULL);
	_kb_hook = SetWindowsHookEx(
		WH_KEYBOARD, (HOOKPROC)KeyboardProc
		, inst, thread_id);
	ASSERT(_kb_hook != NULL);
	if (_kb_hook == NULL)
	{
		UnhookWindowsHookEx(_mouse_hook);
		UnhookWindowsHookEx(_wnd_hook);
		return FALSE;
	}	
	ASSERT(_this == NULL);
	_this = this;

	return TRUE;
}

void CHierarchyPopup::Unhooks()
{
	if (_mouse_hook != NULL)
	{
		UnhookWindowsHookEx(_mouse_hook);
		_mouse_hook = NULL;
	}
	if (_wnd_hook != NULL)
	{
		UnhookWindowsHookEx(_wnd_hook);
		_wnd_hook = NULL;
	}
	if (_kb_hook != NULL)
	{
		UnhookWindowsHookEx(_kb_hook);
		_kb_hook = NULL;
	}
	_this = NULL;
}

BOOL CHierarchyPopup::IsVisible() const
{
	return m_hWnd != NULL;
}

void CHierarchyPopup::EnsureItemVisible(int item)
{
	ASSERT(m_hWnd != NULL);
	CRect rect_item = GetItemRect(item);
	CRect rect_client;
	GetClientRect(&rect_client);
	if (rect_item.bottom > rect_client.bottom)
	{
		int n_page = GetVisibleItems();
		SetTopItem(item - (n_page - 1));
	}
	else if (rect_item.top < rect_client.top)
	{
		SetTopItem(item);
	}
}

void CHierarchyPopup::SetTopItem(int pos)
{
	ASSERT(m_hWnd != NULL);	
	int n_max = 0;
	int n_min = 0;
	GetScrollRange(SB_VERT, &n_min, &n_max);
	n_max = GetScrollLimit(SB_VERT);
	if (pos > n_max) pos = n_max;
	if (pos < n_min) pos = n_min;
	if (pos == _top_item)
	{
		return;
	}
	SetScrollPos(SB_VERT, pos, TRUE);
	_top_item = pos;
	Invalidate(FALSE);
}

int CHierarchyPopup::GetVisibleItems() const
{
	ASSERT(m_hWnd != NULL);
	CRect rect_client;
	GetClientRect(&rect_client);
	return rect_client.Height() / _item_height;
}

// hook function
HHOOK CHierarchyPopup::_mouse_hook = NULL;
HHOOK CHierarchyPopup::_wnd_hook = NULL;
HHOOK CHierarchyPopup::_kb_hook = NULL;
CHierarchyPopup *CHierarchyPopup::_this = NULL;

LRESULT CALLBACK CHierarchyPopup::MouseProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CallNextHookEx(_mouse_hook, nCode, wParam, lParam);
	ASSERT(_this != NULL);
	if (nCode == HC_ACTION)
	{
		MOUSEHOOKSTRUCT *m_h_struct = (MOUSEHOOKSTRUCT *)lParam;
		HWND hwnd_rcv = m_h_struct->hwnd;
		if (hwnd_rcv != _this->m_hWnd)
		{
			if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
			{
				POINT pt = m_h_struct->pt;
				_this->ScreenToClient(&pt);
				_this->SendMessage(
					WM_LBUTTONDOWN, 0
					, MAKELPARAM(pt.x, pt.y));
			}
			else if (wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN)
			{
				_this->Close(CANCEL);
			}
			else if (wParam == WM_MOUSEWHEEL)
			{
				_this->SendMessage(WM_MOUSEWHEEL, wParam, lParam);
			}
		}
	}
	return result;
}

LRESULT CALLBACK CHierarchyPopup::CallWndProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CallNextHookEx(_wnd_hook, nCode, wParam, lParam);
	ASSERT(_this != NULL);
	if (nCode == HC_ACTION)
	{
		CWPSTRUCT *cwp_struct = (CWPSTRUCT *)lParam;
		HWND hwnd = cwp_struct->hwnd;		
		CWnd *owner = _this->GetOwner();
		ASSERT(owner != NULL);
		if (hwnd == owner->m_hWnd)
		{
			UINT msg = cwp_struct->message;			
			if (msg == WM_KILLFOCUS)
			{
				_this->Close(CANCEL);
			}			
		}
	}
	return result;
}

LRESULT CALLBACK CHierarchyPopup::KeyboardProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CallNextHookEx(_kb_hook, nCode, wParam, lParam);
	if (nCode == HC_ACTION)
	{
		if (lParam & 0x40000000) // MSDN: 30 - Specifies the previous key state. The value is 1 if the key is down before the message is sent; it is 0 if the key is up.
		{
			_this->SendMessage(WM_KEYDOWN, wParam, lParam);
		}
	}
	return result;
}

BEGIN_MESSAGE_MAP(CHierarchyPopup, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_VSCROLL()	
	ON_WM_MOUSEMOVE()	
	ON_WM_MOUSEACTIVATE()	
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CHierarchyPopup message handlers
void CHierarchyPopup::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (_timer == 0)
	{
		UINT nElapse = 50;
		/*DWORD nRepeat = 0;
		if (SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &nRepeat, 0))
		{
			if (nRepeat > 30) 
			{
				nRepeat = 30;
			}
			nElapse = 1000 * (30 - nRepeat) / (30 - 2) + 30;
		}*/
		_timer = SetTimer(1, nElapse, NULL);
	}
}

void CHierarchyPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DOWN || nChar == VK_UP)
	{		
		int item = (nChar == VK_DOWN) 
			? NextItem(_cur_item) : PrevItem(_cur_item);
		if (item != -1)
		{
			CWnd *owner = GetOwner();
			if (owner != NULL)
			{
				owner->SendMessage(MSG_SELCHANGED, 0, item);
			}			
		}
		if (item != _cur_item)
		{
			SetCurSel(item);
		}
	}
	else if (nChar == VK_ESCAPE)
	{
		Close(CANCEL);
	}
	else if (nChar == VK_RETURN)
	{
		if (_cur_item != -1)
		{
			Close(SET, _cur_item);
		}
		else
		{
			MessageBeep(MB_ICONQUESTION);
		}
	}
}

void CHierarchyPopup::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	CDC dc_mem;
	if (!dc_mem.CreateCompatibleDC(&dc))
	{
		return;
	}
	CRect rect;
	GetClientRect(&rect);
	CBitmap bitmap;
	if (!bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
	{
		return;
	}
	CBitmap *old_bitmap = dc_mem.SelectObject(&bitmap);	
	dc_mem.FillSolidRect(rect, SysParas.cl_window);
	int y_max = rect.bottom;
	rect.bottom = rect.top + _item_height;
	CFont *old_font = dc_mem.SelectObject(SysParas.font_default);
	for (int i = _top_item; i < (int)_items.size() && rect.top < y_max; i++)
	{
		if (_items[i]->_icon == SEPERATOR)
		{
			DrawSeperator(&dc_mem, i, rect);
		}
		else
		{
			DrawItem(&dc_mem, i, rect, _cur_item == i ? SELECT : 0, TRUE);
		}
		rect.top = rect.bottom;
		rect.bottom += _item_height;
	}
	dc_mem.SelectObject(old_font);
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height()
		,&dc_mem, 0, 0, SRCCOPY);
	dc_mem.SelectObject(old_bitmap);
}

void CHierarchyPopup::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Get the minimum and maximum scroll-bar positions.
	int min_pos;
	int max_pos;

	GetScrollRange(SB_VERT, &min_pos, &max_pos);
	max_pos = GetScrollLimit(SB_VERT);
	// Get the current position of scroll box.
	int cur_pos = GetScrollPos(SB_VERT);

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		cur_pos = min_pos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		cur_pos = max_pos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		cur_pos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		cur_pos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
		{
			// Get the page size. 
			SCROLLINFO info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);
			cur_pos -= (int)info.nPage;
		}
		break;

	case SB_PAGERIGHT:      // Scroll one page right.
		{
			// Get the page size. 
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);
			cur_pos += (int)info.nPage;
		}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		cur_pos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		cur_pos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	SetTopItem(cur_pos);

  	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CHierarchyPopup::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect;
	GetClientRect(&rect);
	if (!rect.PtInRect(point))
	{
		return;
	}	
	SetCurSel(ItemFromPoint(point));
	
	CWnd::OnMouseMove(nFlags, point);
}

int CHierarchyPopup::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}

BOOL CHierarchyPopup::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int iDelta = zDelta / WHEEL_DELTA;
	int nTopItem = _top_item - iDelta;
	SetTopItem(nTopItem);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CHierarchyPopup::OnLButtonUp(UINT nFlags, CPoint point)
{	
	CRect rect_this;
	GetWindowRect(&rect_this);
	CPoint pt_src = point;
	ClientToScreen(&pt_src);	
	if (!rect_this.PtInRect(pt_src))
	{
		CWnd *owner = GetOwner();
		ASSERT(owner != NULL);
		CRect rect_owner;
		owner->GetWindowRect(&rect_owner);		
		if (!rect_owner.PtInRect(pt_src))
		{
			Close(CANCEL);
		}
	}
	else
	{
		int item = ItemFromPoint(point, FALSE);
		if (item != -1)
		{
			Close(SET, item);
		}
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CHierarchyPopup::OnDestroy()
{
	if (_timer != 0)
	{
		KillTimer(_timer);
		_timer = 0;
	}
	CWnd::OnDestroy();
}

void CHierarchyPopup::OnTimer(UINT nIDEvent)
{
	if ((GetKeyState(VK_LBUTTON) >> (sizeof(SHORT) - 1)) == 0)
	{
		ASSERT(_timer != 0);
		KillTimer(_timer);
		_timer = 0;
	}
	else
	{
		CPoint ptCur;
		if (GetCursorPos(&ptCur))
		{
			CRect rcWnd;
			GetWindowRect(&rcWnd);
			if (ptCur.y < rcWnd.top)
			{
				SetTopItem(_top_item - 1);
			}
			else if (ptCur.y > rcWnd.bottom)
			{
				SetTopItem(_top_item + 1);
			}
		}
	}
	CWnd::OnTimer(nIDEvent);
}
