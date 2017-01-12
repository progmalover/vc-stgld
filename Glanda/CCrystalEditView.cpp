////////////////////////////////////////////////////////////////////////////
//	File:		CCrystalEditView.cpp
//	Version:	1.0.0.0
//	Created:	29-Dec-1998
//
//	Author:		Stcherbatchenko Andrei
//	E-mail:		windfall@gmx.de
//
//	Implementation of the CCrystalEditView class, a part of the Crystal Edit -
//	syntax coloring text editor.
//
//	You are free to use, distribute or modify this code
//	but LEAVE THIS HEADER INTACT.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editcmd.h"
#include "editreg.h"
#include "CCrystalEditView.h"
#include "CCrystalTextBuffer.h"
#include "CEditReplaceDlg.h"
#include "resource.h"
#include <imm.h>

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif
#ifndef __AFXOLE_H__
#pragma message("Include <afxole.h> in your stdafx.h to avoid this message")
#include <afxole.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRAG_BORDER_X		5
#define DRAG_BORDER_Y		5

/////////////////////////////////////////////////////////////////////////////
// CEditDropTargetImpl class declaration

class CEditDropTargetImpl : public COleDropTarget
{
private:
	CCrystalEditView *m_pOwner;
public:
	CEditDropTargetImpl(CCrystalEditView *pOwner) { m_pOwner = pOwner; };

	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
};


/////////////////////////////////////////////////////////////////////////////
// CCrystalEditView

IMPLEMENT_DYNCREATE(CCrystalEditView, CCrystalTextView)

CCrystalEditView::CCrystalEditView()
{
	AFX_ZERO_INIT_OBJECT(CCrystalTextView);
	m_bImeSelChange = TRUE;
	m_bIme = FALSE;
}

CCrystalEditView::~CCrystalEditView()
{
}


BEGIN_MESSAGE_MAP(CCrystalEditView, CCrystalTextView)
	//{{AFX_MSG_MAP(CCrystalEditView)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_DELETE_BACK, OnEditDeleteBack)
	ON_COMMAND(ID_EDIT_UNTAB, OnEditUntab)
	ON_COMMAND(ID_EDIT_TAB, OnEditTab)
	ON_COMMAND(ID_EDIT_SWITCH_OVRMODE, OnEditSwitchOvrmode)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SWITCH_OVRMODE, OnUpdateEditSwitchOvrmode)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	//ON_COMMAND(ID_EDIT_REDOALL, OnEditRedoall)
	//ON_UPDATE_COMMAND_UI(ID_EDIT_REDOALL, OnUpdateEditRedo)
	//ON_COMMAND(ID_EDIT_UNDOALL, OnEditUndoall)
	//ON_UPDATE_COMMAND_UI(ID_EDIT_UNDOALL, OnUpdateEditUndo)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_EDIT_INDICATOR_READ, OnUpdateIndicatorRead)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INDICATOR_COL, OnUpdateIndicatorCol)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINECOL, OnUpdateIndicatorPosition)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_CRLF, OnUpdateIndicatorCRLF)
	ON_MESSAGE(WM_IME_COMPOSITION, OnImeComposition)
	ON_MESSAGE(WM_IME_NOTIFY, OnImeNotify)
	ON_MESSAGE(WM_IME_ENDCOMPOSITION, OnImeEndComposition)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCrystalEditView message handlers

BOOL CCrystalEditView::QueryEditable()
{
	if (m_pTextBuffer == NULL)
		return FALSE;
	return ! m_pTextBuffer->GetReadOnly();
}

void CCrystalEditView::OnEditPaste() 
{
	Paste();
}

void CCrystalEditView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TextInClipboard());
}

void CCrystalEditView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsSelection());
}

void CCrystalEditView::OnEditCut() 
{
	Cut();
}

BOOL CCrystalEditView::DeleteCurrentSelection()
{
	if (IsSelection())
	{
		CPoint ptSelStart, ptSelEnd;
		GetSelection(ptSelStart, ptSelEnd);

		CPoint ptCursorPos = ptSelStart;
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);

		m_pTextBuffer->DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x);
		return TRUE;
	}
	return FALSE;
}

void CCrystalEditView::Paste()
{
	if (! QueryEditable())
		return;
	if (m_pTextBuffer == NULL)
		return;

	m_pTextBuffer->BeginUndoGroup();

	DeleteCurrentSelection();

	CString text;
	if (GetFromClipboard(text))
	{
		CPoint ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		int x, y;
		m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, text, y, x);
		ptCursorPos.x = x;
		ptCursorPos.y = y;
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);
	}

	m_pTextBuffer->FlushUndoGroup();
}

void CCrystalEditView::Cut()
{
	if (! QueryEditable())
		return;
	if (m_pTextBuffer == NULL)
		return;
	if (! IsSelection())
		return;

	CPoint ptSelStart, ptSelEnd;
	GetSelection(ptSelStart, ptSelEnd);
	CString text;
	GetText(ptSelStart, ptSelEnd, text);
	PutToClipboard(text);

	CPoint ptCursorPos = ptSelStart;
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	SetAnchor(ptCursorPos);
	SetSelection(ptCursorPos, ptCursorPos);
	SetCursorPos(ptCursorPos);
	EnsureVisible(ptCursorPos);

	m_pTextBuffer->DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x);
}

void CCrystalEditView::OnEditDelete() 
{
	if (! QueryEditable() || m_pTextBuffer == NULL)
		return;

	CPoint ptSelStart, ptSelEnd;
	GetSelection(ptSelStart, ptSelEnd);
	if (ptSelStart == ptSelEnd)
	{
		int nLength = GetLineLength(ptSelEnd.y);
		if (ptSelEnd.x == GetLineLength(ptSelEnd.y))
		{
			if (ptSelEnd.y == GetLineCount() - 1)
				return;
			ptSelEnd.y ++;
			ptSelEnd.x = 0;
		}
		else
		{
			LPCTSTR pszChars = GetLineChars(ptSelEnd.y);
			if (ptSelEnd.x < nLength - 1 && __ismbslead(pszChars, ptSelEnd.x))
			{
				ptSelEnd.x += 2;
			}
			else
			{
				ptSelEnd.x ++;
			}
		}
	}

	CPoint ptCursorPos = ptSelStart;
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	SetAnchor(ptCursorPos);
	SetSelection(ptCursorPos, ptCursorPos);
	SetCursorPos(ptCursorPos);
	EnsureVisible(ptCursorPos);

	m_pTextBuffer->DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x);
}

int CCrystalEditView::GetIndent(CString &strIndent, int nChar, int x, int y)
{
	return 0;
}

void CCrystalEditView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CCrystalTextView::OnChar(nChar, nRepCnt, nFlags);

	if (m_pTextBuffer == NULL || !QueryEditable())
	{
		MessageBeep(0);
		return;
	}

	if ((::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
			(::GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0)
		return;

	if (nChar == VK_RETURN)
	{
		//
		// Modified by Chenhao 01-2-7 上午 11:24:10
		// Purpose: Add auto indent support
		// 
		CPoint ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);

		if (m_bOvrMode)
		{
			//CPoint ptCursorPos = GetCursorPos();
			//ASSERT_VALIDTEXTPOS(ptCursorPos);
			if (ptCursorPos.y < GetLineCount() - 1)
			{
				ptCursorPos.x = 0;
				ptCursorPos.y ++;
				if (m_bAutoIndent)
				{
					int indent = 0;
					int len = GetLineLength(ptCursorPos.y);
					LPCTSTR p = GetLineChars(ptCursorPos.y);
					LPCTSTR p1 = p;
					while (p - p1 < len && (*p == '\t' || *p == ' '))
					{
						indent++;
						p++;
					}
					ptCursorPos.x += indent;
				}

				ASSERT_VALIDTEXTPOS(ptCursorPos);
				SetSelection(ptCursorPos, ptCursorPos);
				SetAnchor(ptCursorPos);
				SetCursorPos(ptCursorPos);
				EnsureVisible(ptCursorPos);
				return;
			}
		}

		m_pTextBuffer->BeginUndoGroup();

		DeleteCurrentSelection();

		ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		const static TCHAR pszText[3] = _T("\r\n");

		int x, y;
		m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszText, y, x);

		if (m_bAutoIndent)
		{
			int count = 0;
			int len = GetLineLength(y);
			LPCTSTR p = GetLineChars(y);
			while (count < len && *p == ' ' || *p == '\t')
			{
				count++;
				p++;
			}
			if (count > 0)
				m_pTextBuffer->DeleteText(this, y, 0, y, count);

			CString strIndent;
			if (GetIndent(strIndent, nChar, x, y) > 0)
				m_pTextBuffer->InsertText(this, y, x, strIndent, y, x);
		}

		ptCursorPos.x = x;
		ptCursorPos.y = y;

		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetAnchor(ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);

		m_pTextBuffer->FlushUndoGroup();
		return;
	}

	if (nChar > 31)
	{
		if (OnCharBefore(nChar))
		{
			CPoint ptSelStart, ptSelEnd;
			GetSelection(ptSelStart, ptSelEnd);

			//  合并undo的条件：
			// 1、和上一次的字符类型相同
			// 2、输入点没变
			// 3、上次输入后没有通过其他方式修改文件(如Copy&Paste, undo/redo等)
			BOOL b1 = (isalnum(nChar) != 0);
			BOOL b2 = (isalnum(m_nLastInputChar) != 0);
			m_pTextBuffer->BeginUndoGroup(b1 == b2 && 
				GetCursorPos() == m_ptLastInputPos &&
				!m_pTextBuffer->m_bModifiedAfterLastCharInput);

			CPoint ptCursorPos;
			if (ptSelStart != ptSelEnd)
			{
				ptCursorPos = ptSelStart;
				DeleteCurrentSelection();
			}
			else
			{
				ptCursorPos = GetCursorPos();
				if (m_bOvrMode)
				{
					int nLength = GetLineLength(ptCursorPos.y);
					if (ptCursorPos.x < nLength)
					{
						LPCTSTR pszChars = GetLineChars(ptCursorPos.y);
						if (ptCursorPos.x < nLength - 1 && __ismbslead(pszChars, ptSelEnd.x))
							m_pTextBuffer->DeleteText(this, ptCursorPos.y, ptCursorPos.x, ptCursorPos.y, ptCursorPos.x + 2);
						else
							m_pTextBuffer->DeleteText(this, ptCursorPos.y, ptCursorPos.x, ptCursorPos.y, ptCursorPos.x + 1);
					}
				}
			}

			ASSERT_VALIDTEXTPOS(ptCursorPos);

			int x = ptCursorPos.x;
			int y = ptCursorPos.y;

			CString strIndent;
			int indent = GetIndent(strIndent, nChar, x, y);
			if (indent != x)
			{
				if (x > 0)
					if (m_pTextBuffer->DeleteText(this, y, 0, y, x))
						x = 0;
				if (!strIndent.IsEmpty())
					m_pTextBuffer->InsertText(this, y, x, strIndent, y, x);
			}

			char pszText[2];
			pszText[0] = (char) nChar;
			pszText[1] = 0;

			m_pTextBuffer->InsertText(this, y, x, pszText, y, x);

			ptCursorPos.x = x;
			ptCursorPos.y = y;
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetAnchor(ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);

			m_ptLastInputPos = ptCursorPos;
			m_nLastInputChar = nChar;
			m_pTextBuffer->m_bModifiedAfterLastCharInput = FALSE;

			m_pTextBuffer->FlushUndoGroup();

			// 
			// Modified by Chenhao 01-2-8 下午 01:42:18
			// To notify auto-completion handle
			OnCharAfter(nChar);
		}
	}
}

void CCrystalEditView::OnEditDeleteBack() 
{
	if (IsSelection())
	{
		OnEditDelete();
		return;
	}

	if (! QueryEditable() || m_pTextBuffer == NULL)
		return;

	CPoint ptCursorPos = GetCursorPos();
	CPoint ptCurrentCursorPos = ptCursorPos;
	if (ptCursorPos.x == 0)
	{
		if (ptCursorPos.y == 0)
			return;
		ptCursorPos.y --;
		ptCursorPos.x = GetLineLength(ptCursorPos.y);
	}
	else
	{
		LPCTSTR pszChars = GetLineChars(ptCursorPos.y);
		if (ptCursorPos.x > 1 && __ismbstrail(pszChars, ptCursorPos.x - 1))
			ptCursorPos.x -= 2;
		else
			ptCursorPos.x --;
	}

	ASSERT_VALIDTEXTPOS(ptCursorPos);
	SetAnchor(ptCursorPos);
	SetSelection(ptCursorPos, ptCursorPos);
	SetCursorPos(ptCursorPos);
	EnsureVisible(ptCursorPos);

	m_pTextBuffer->DeleteText(this, ptCursorPos.y, ptCursorPos.x, ptCurrentCursorPos.y, ptCurrentCursorPos.x);
}

void CCrystalEditView::OnEditTab() 
{
	if (! QueryEditable() || m_pTextBuffer == NULL)
		return;

	BOOL bTabify = FALSE;
	CPoint ptSelStart, ptSelEnd;
	if (IsSelection())
	{
		GetSelection(ptSelStart, ptSelEnd);
		bTabify = ptSelStart.y != ptSelEnd.y;
	}

	//TCHAR pszText[] = _T("\t");
	TCHAR *pszText = NULL;
	if (!m_bTabToSpace)
	{
		pszText = _T("\t");
	}
	else
	{
		pszText = (TCHAR *)_alloca((GetTabSize() + 1) * sizeof(TCHAR));
		if (!pszText)
			return;
		memset(pszText, (TCHAR)' ', GetTabSize() * sizeof(TCHAR));
		pszText[GetTabSize()] = (TCHAR)'\0';
	}

	if (bTabify)
	{
		m_pTextBuffer->BeginUndoGroup();

		int nStartLine = ptSelStart.y;
		int nEndLine = ptSelEnd.y;
		ptSelStart.x = 0;
		if (ptSelEnd.x > 0)
		{
			if (ptSelEnd.y == GetLineCount() - 1)
			{
				ptSelEnd.x = GetLineLength(ptSelEnd.y);
			}
			else
			{
				ptSelEnd.x = 0;
				ptSelEnd.y ++;
			}
		}
		else
			nEndLine --;
		SetSelection(ptSelStart, ptSelEnd);
		SetCursorPos(ptSelEnd);
		EnsureVisible(ptSelEnd);

		//	Shift selection to right
		m_bHorzScrollBarLocked = TRUE;
		for (int l = nStartLine; l <= nEndLine; l ++)
		{
			int x, y;
			m_pTextBuffer->InsertText(this, l, 0, pszText, y, x);
		}
		m_bHorzScrollBarLocked = FALSE;
		RecalcHorzScrollBar();

		m_pTextBuffer->FlushUndoGroup();
		return;
	}

	if (m_bOvrMode)
	{
		CPoint ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);

		int nLineLength = GetLineLength(ptCursorPos.y);
		LPCTSTR pszLineChars = GetLineChars(ptCursorPos.y);
		if (ptCursorPos.x < nLineLength)
		{
			int nTabSize = GetTabSize();
			int nChars = nTabSize - CalculateActualOffset(ptCursorPos.y, ptCursorPos.x) % nTabSize;
			ASSERT(nChars > 0 && nChars <= nTabSize);

			while (nChars > 0)
			{
				if (ptCursorPos.x == nLineLength)
					break;
				if (pszLineChars[ptCursorPos.x] == _T('\t'))
				{
					ptCursorPos.x ++;
					break;
				}
				ptCursorPos.x ++;
				nChars --;
			}
			// Handle MCBS
			if (ptCursorPos.x < nLineLength && __ismbstrail(pszLineChars, ptCursorPos.x))
			{
				ptCursorPos.x ++;
			}

			ASSERT(ptCursorPos.x <= nLineLength);
			ASSERT_VALIDTEXTPOS(ptCursorPos);

			SetSelection(ptCursorPos, ptCursorPos);
			SetAnchor(ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);
			return;
		}
	}

	m_pTextBuffer->BeginUndoGroup();

	DeleteCurrentSelection();

	CPoint ptCursorPos = GetCursorPos();
	ASSERT_VALIDTEXTPOS(ptCursorPos);

	int x, y;
	m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszText, y, x);
	ptCursorPos.x = x;
	ptCursorPos.y = y;
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	SetSelection(ptCursorPos, ptCursorPos);
	SetAnchor(ptCursorPos);
	SetCursorPos(ptCursorPos);
	EnsureVisible(ptCursorPos);

	m_pTextBuffer->FlushUndoGroup();
}

void CCrystalEditView::OnEditUntab() 
{
	if (! QueryEditable() || m_pTextBuffer == NULL)
		return;

	BOOL bTabify = FALSE;
	CPoint ptSelStart, ptSelEnd;
	if (IsSelection())
	{
		GetSelection(ptSelStart, ptSelEnd);
		bTabify = ptSelStart.y != ptSelEnd.y;
	}

	if (bTabify)
	{
		m_pTextBuffer->BeginUndoGroup();

		CPoint ptSelStart, ptSelEnd;
		GetSelection(ptSelStart, ptSelEnd);
		int nStartLine = ptSelStart.y;
		int nEndLine = ptSelEnd.y;
		ptSelStart.x = 0;
		if (ptSelEnd.x > 0)
		{
			if (ptSelEnd.y == GetLineCount() - 1)
			{
				ptSelEnd.x = GetLineLength(ptSelEnd.y);
			}
			else
			{
				ptSelEnd.x = 0;
				ptSelEnd.y ++;
			}
		}
		else
			nEndLine --;
		SetSelection(ptSelStart, ptSelEnd);
		SetCursorPos(ptSelEnd);
		EnsureVisible(ptSelEnd);

		//	Shift selection to left
		m_bHorzScrollBarLocked = TRUE;
		for (int l = nStartLine; l <= nEndLine; l ++)
		{
			int nLength = GetLineLength(l);
			if (nLength > 0)
			{
				LPCTSTR pszChars = GetLineChars(l);
				int nPos = 0, nOffset = 0;
				while (nPos < nLength)
				{
					if (pszChars[nPos] == _T(' '))
					{
						nPos ++;
						if (++ nOffset >= GetTabSize())
							break;
					}
					else
					{
						if (pszChars[nPos] == _T('\t'))
							nPos ++;
						break;
					}
				}

				if (nPos > 0)
					m_pTextBuffer->DeleteText(this, l, 0, l, nPos);
			}
		}
		m_bHorzScrollBarLocked = FALSE;
		RecalcHorzScrollBar();

		m_pTextBuffer->FlushUndoGroup();
	}
	else
	{
		CPoint ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		if (ptCursorPos.x > 0)
		{
			int nTabSize = GetTabSize();
			int nOffset = CalculateActualOffset(ptCursorPos.y, ptCursorPos.x);
			int nNewOffset = nOffset / nTabSize * nTabSize;
			if (nOffset == nNewOffset && nNewOffset > 0)
				nNewOffset -= nTabSize;
			ASSERT(nNewOffset >= 0);

			LPCTSTR pszChars = GetLineChars(ptCursorPos.y);
			int nCurrentOffset = 0;
			int I = 0;
			while (nCurrentOffset < nNewOffset)
			{
				if (pszChars[I] == _T('\t'))
					nCurrentOffset = nCurrentOffset / nTabSize * nTabSize + nTabSize;
				else
					nCurrentOffset ++;
				I ++;
			}

			ASSERT(nCurrentOffset == nNewOffset);

			ptCursorPos.x = I;
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetAnchor(ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);
		}
	}
}

void CCrystalEditView::OnUpdateIndicatorCol(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CCrystalEditView::OnUpdateIndicatorOvr(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bOvrMode);
}

void CCrystalEditView::OnUpdateIndicatorRead(CCmdUI* pCmdUI)
{
	if (m_pTextBuffer == NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(m_pTextBuffer->GetReadOnly());
}

void CCrystalEditView::OnUpdateIndicatorPosition(CCmdUI* pCmdUI)
{
	CCrystalTextView::OnUpdateIndicatorPosition(pCmdUI);
}

void CCrystalEditView::OnUpdateIndicatorCRLF(CCmdUI* pCmdUI)
{
	CCrystalTextView::OnUpdateIndicatorCRLF(pCmdUI);
}

void CCrystalEditView::OnEditSwitchOvrmode() 
{
	m_bOvrMode = ! m_bOvrMode;
}

void CCrystalEditView::OnUpdateEditSwitchOvrmode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bOvrMode ? 1 : 0);
}

DROPEFFECT CEditDropTargetImpl::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if (!pDataObject->IsDataAvailable(CF_TEXT))
	{
		m_pOwner->HideDropIndicator();
		return DROPEFFECT_NONE;
	}
	m_pOwner->ShowDropIndicator(point);
	if (dwKeyState & MK_CONTROL)
		return DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
	UNREFERENCED_PARAMETER(pWnd);
}

void CEditDropTargetImpl::OnDragLeave(CWnd* pWnd)
{
	m_pOwner->HideDropIndicator();
	UNREFERENCED_PARAMETER(pWnd);
}

DROPEFFECT CEditDropTargetImpl::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if (!pDataObject->IsDataAvailable(CF_TEXT))
	{
		m_pOwner->HideDropIndicator();
		return DROPEFFECT_NONE;
	}
	m_pOwner->ShowDropIndicator(point);
	if (dwKeyState & MK_CONTROL)
		return DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
	UNREFERENCED_PARAMETER(pWnd);
}

BOOL CEditDropTargetImpl::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	m_pOwner->HideDropIndicator();
	if (!pDataObject->IsDataAvailable(CF_TEXT))
		return FALSE;

	return m_pOwner->DoDropText(pDataObject, point);
	UNREFERENCED_PARAMETER(pWnd);
	UNREFERENCED_PARAMETER(dropEffect);
}

DROPEFFECT CEditDropTargetImpl::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point)
{
	ASSERT(m_pOwner == pWnd);
	m_pOwner->DoDragScroll(point);

	if (dwKeyState & MK_CONTROL)
		return DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
}

void CCrystalEditView::DoDragScroll(const CPoint &point)
{
	CRect rcClientRect;
	GetClientRect(rcClientRect);
	if (point.y < rcClientRect.top + DRAG_BORDER_Y)
	{
		HideDropIndicator();
		ScrollUp();
		UpdateWindow();
		ShowDropIndicator(point);
		return;
	}
	if (point.y >= rcClientRect.bottom - DRAG_BORDER_Y)
	{
		HideDropIndicator();
		ScrollDown();
		UpdateWindow();
		ShowDropIndicator(point);
		return;
	}
	if (point.x < rcClientRect.left + GetMarginWidth() + DRAG_BORDER_X)
	{
		HideDropIndicator();
		ScrollLeft();
		UpdateWindow();
		ShowDropIndicator(point);
		return;
	}
	if (point.x >= rcClientRect.right - DRAG_BORDER_X)
	{
		HideDropIndicator();
		ScrollRight();
		UpdateWindow();
		ShowDropIndicator(point);
		return;
	}
}

BOOL CCrystalEditView::DoDropText(COleDataObject *pDataObject, const CPoint &ptClient)
{
	if (!pDataObject->IsDataAvailable(CF_TEXT))
	{
		return FALSE;
	}

	CPoint ptDropPos = ClientToText(ptClient);
	if (IsDraggingText() && IsInsideSelection(ptDropPos))
	{
		SetAnchor(ptDropPos);
		SetSelection(ptDropPos, ptDropPos);
		SetCursorPos(ptDropPos);
		EnsureVisible(ptDropPos);
		return FALSE;
	}

	HGLOBAL hData;
	if (pDataObject->IsDataAvailable(CF_TEXT) && QueryEditable())
	{
		// Important! or will cause strange problems if the derived class 
		// asume the widnow must have focus while editing.
		SetFocus();

		hData = pDataObject->GetGlobalData(CF_TEXT);
		if (hData == NULL)
			return FALSE;
		LPSTR pszText = (LPSTR) ::GlobalLock(hData);
		if (pszText == NULL)
			return FALSE;

		int x, y;

		//
		// Modified by chenhao, 1/18/2000
		//
		//m_pTextBuffer->InsertText(this, ptDropPos.y, ptDropPos.x, A2T(pszText), y, x);
		if (!m_pTextBuffer->InsertText(this, ptDropPos.y, ptDropPos.x, pszText, y, x))
			return FALSE;

		CPoint ptCurPos(x, y);
		ASSERT_VALIDTEXTPOS(ptCurPos);
		SetAnchor(ptDropPos);
		SetSelection(ptDropPos, ptCurPos);
		SetCursorPos(ptCurPos);
		EnsureVisible(ptCurPos);

		::GlobalUnlock(hData);
	}
	/*
	else
	{
		ASSERT(pDataObject->IsDataAvailable(g_cfPagelet));

		hData = pDataObject->GetGlobalData(g_cfPagelet);
		if (hData == NULL)
			return FALSE;

		SetCursorPos(ptDropPos);
		
		CMDIChildWnd *pFrame = (CMDIChildWnd *)GetParent();
		ASSERT_VALID(pFrame);
		pFrame->MDIActivate();
	}
	*/

	return TRUE;
}

int CCrystalEditView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCrystalTextView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ASSERT(m_pDropTarget == NULL);
	m_pDropTarget = new CEditDropTargetImpl(this);
	if (! m_pDropTarget->Register(this))
	{
		TRACE0("Warning: Unable to register drop target for CCrystalEditView.\n");
		delete m_pDropTarget;
		m_pDropTarget = NULL;
	}

	return 0;
}

void CCrystalEditView::OnDestroy() 
{
	if (m_pDropTarget != NULL)
	{
		m_pDropTarget->Revoke();
		delete m_pDropTarget;
		m_pDropTarget = NULL;
	}

	CCrystalTextView::OnDestroy();
}

void CCrystalEditView::ShowDropIndicator(const CPoint &point)
{
	if (! m_bDropPosVisible)
	{
		HideCursor();
		m_ptSavedCaretPos = GetCursorPos();
		m_bDropPosVisible = TRUE;
		::CreateCaret(m_hWnd, (HBITMAP) 1, 2, GetLineHeight());
	}
	m_ptDropPos = ClientToText(point);
	if (m_ptDropPos.x >= m_nOffsetChar)
	{
		SetCaretPos(TextToClient(m_ptDropPos));
		ShowCaret();
	}
	else
	{
		HideCaret();
	}
}

void CCrystalEditView::HideDropIndicator()
{
	if (m_bDropPosVisible)
	{
		SetCursorPos(m_ptSavedCaretPos);
		ShowCursor();
		m_bDropPosVisible = FALSE;
	}
}

DROPEFFECT CCrystalEditView::GetDropEffect()
{
	CCrystalTextBuffer *pBuf = LocateTextBuffer();
	if (pBuf && pBuf->GetReadOnly())
		return DROPEFFECT_COPY;
	return DROPEFFECT_COPY | DROPEFFECT_MOVE;
}

void CCrystalEditView::OnDropSource(DROPEFFECT de)
{
	if (! IsDraggingText())
		return;

	ASSERT_VALIDTEXTPOS(m_ptDraggedTextBegin);
	ASSERT_VALIDTEXTPOS(m_ptDraggedTextEnd);

	//
	// "&& QueryEditable()" is added by chenhao, 1/19/2000
	//
	if (de == DROPEFFECT_MOVE && QueryEditable())
	{
		m_pTextBuffer->DeleteText(this, m_ptDraggedTextBegin.y, m_ptDraggedTextBegin.x, m_ptDraggedTextEnd.y, m_ptDraggedTextEnd.x);

		// Added by Chen Hao, 2004-03-15
		// Bug fixed: if both drag source & drop target are current view, 
		// the caret pos will be incorrect.
		CPoint ptSelStart, ptSelEnd;
		GetSelection(ptSelStart, ptSelEnd);
		if (ptSelStart != ptSelEnd)
		{
			CPoint ptCursorPos = ptSelEnd;
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetAnchor(ptCursorPos);
			//SetSelection(ptCursorPos, ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);
		}
	}
}

void CCrystalEditView::UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex /*= -1*/)
{
	CCrystalTextView::UpdateView(pSource, pContext, dwFlags, nLineIndex);

	if (m_bSelectionPushed && pContext != NULL)
	{
		pContext->RecalcPoint(m_ptSavedSelStart);
		pContext->RecalcPoint(m_ptSavedSelEnd);
		ASSERT_VALIDTEXTPOS(m_ptSavedSelStart);
		ASSERT_VALIDTEXTPOS(m_ptSavedSelEnd);
	}
}

void CCrystalEditView::OnEditReplace() 
{
	if (! QueryEditable())
		return;

	CWinApp *pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	CEditReplaceDlg dlg(this);

	//	Take search parameters from registry
	dlg.m_bMatchCase = pApp->GetProfileInt(REG_REPLACE_SUBKEY, REG_MATCH_CASE, FALSE);
	dlg.m_bWholeWord = pApp->GetProfileInt(REG_REPLACE_SUBKEY, REG_WHOLE_WORD, FALSE);
	dlg.m_sText = pApp->GetProfileString(REG_REPLACE_SUBKEY, REG_FIND_WHAT, _T(""));
	dlg.m_sNewText = pApp->GetProfileString(REG_REPLACE_SUBKEY, REG_REPLACE_WITH, _T(""));

	if (IsSelection())
	{
		GetSelection(m_ptSavedSelStart, m_ptSavedSelEnd);
		m_bSelectionPushed = TRUE;

		dlg.m_nScope = 0;	//	Replace in current selection
		dlg.m_ptCurrentPos = m_ptSavedSelStart;
		dlg.m_bEnableScopeSelection = TRUE;
		dlg.m_ptBlockBegin = m_ptSavedSelStart;
		dlg.m_ptBlockEnd = m_ptSavedSelEnd;
	}
	else
	{
		dlg.m_nScope = 1;	//	Replace in whole text
		dlg.m_ptCurrentPos = GetCursorPos();
		dlg.m_bEnableScopeSelection = FALSE;
	}

	//	Execute Replace dialog
	BOOL bShowInactiveSelection = m_bShowInactiveSelection;
	m_bShowInactiveSelection = TRUE;
	dlg.DoModal();
	m_bShowInactiveSelection = bShowInactiveSelection;

	//	Restore selection
	if (m_bSelectionPushed)
	{
		SetSelection(m_ptSavedSelStart, m_ptSavedSelEnd);
		m_bSelectionPushed = FALSE;
	}

	//	Save search parameters to registry
	pApp->WriteProfileInt(REG_REPLACE_SUBKEY, REG_MATCH_CASE, dlg.m_bMatchCase);
	pApp->WriteProfileInt(REG_REPLACE_SUBKEY, REG_WHOLE_WORD, dlg.m_bWholeWord);
	pApp->WriteProfileString(REG_REPLACE_SUBKEY, REG_FIND_WHAT, dlg.m_sText);
	pApp->WriteProfileString(REG_REPLACE_SUBKEY, REG_REPLACE_WITH, dlg.m_sNewText);
}

BOOL CCrystalEditView::ReplaceSelection(LPCTSTR pszNewText)
{
	ASSERT(pszNewText != NULL);

	if (! IsSelection())
		return FALSE;


	m_pTextBuffer->BeginUndoGroup();

	DeleteCurrentSelection();

	CPoint ptCursorPos = GetCursorPos();
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	int x = ptCursorPos.x;
	int y = ptCursorPos.y;
	if (_tcslen(pszNewText) > 0)
		m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszNewText, y, x);
	CPoint ptEndOfBlock = CPoint(x, y);
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	ASSERT_VALIDTEXTPOS(ptEndOfBlock);
	SetAnchor(ptEndOfBlock);
	SetSelection(ptCursorPos, ptEndOfBlock);
	SetCursorPos(ptEndOfBlock);
	EnsureVisible(ptEndOfBlock);

	m_pTextBuffer->FlushUndoGroup();

	return TRUE;
}

void CCrystalEditView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pTextBuffer != NULL && m_pTextBuffer->CanUndo());
}

void CCrystalEditView::OnEditUndo() 
{
	if (m_pTextBuffer != NULL && m_pTextBuffer->CanUndo())
	{
		CPoint ptCursorPos;
		if (m_pTextBuffer->Undo(ptCursorPos))
		{
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetAnchor(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);
		}
	}
}

void CCrystalEditView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pTextBuffer != NULL && m_pTextBuffer->CanRedo());
}

void CCrystalEditView::OnEditRedo() 
{
	if (m_pTextBuffer != NULL && m_pTextBuffer->CanRedo())
	{
		CPoint ptCursorPos;
		if (m_pTextBuffer->Redo(ptCursorPos))
		{
			ASSERT_VALIDTEXTPOS(ptCursorPos);
			SetAnchor(ptCursorPos);
			SetSelection(ptCursorPos, ptCursorPos);
			SetCursorPos(ptCursorPos);
			EnsureVisible(ptCursorPos);
		}
	}
}


void CCrystalEditView::OnEditRedoall() 
{
	// TODO: Add your command handler code here
	while (m_pTextBuffer != NULL && m_pTextBuffer->CanRedo())
		OnEditRedo() ;
	
}



void CCrystalEditView::OnEditUndoall() 
{
	// TODO: Add your command handler code here
	while (m_pTextBuffer != NULL && m_pTextBuffer->CanUndo())
		OnEditUndo() ;	
}

//
// Edit by Chenhao 01-1-12 上午 10:12:21
// Buf fixed: the author forgets to call SetSelection()
//
BOOL CCrystalEditView::InsertText(LPCTSTR pszNewText)
{
	ASSERT(pszNewText != NULL);

	if (!pszNewText)
		return FALSE;

	if (strlen(pszNewText) == 0)
		return TRUE;

	//m_pTextBuffer->BeginUndoGroup();

	BOOL bSelection = IsSelection();

	DeleteCurrentSelection();

	CPoint ptCursorPos = GetCursorPos();
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	int x, y;
	if (! m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszNewText, y, x) )
		return FALSE ;
	CPoint ptEndOfBlock = CPoint(x, y);
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	ASSERT_VALIDTEXTPOS(ptEndOfBlock);
	SetAnchor(ptEndOfBlock);
	if (bSelection)
		SetSelection(ptCursorPos, ptEndOfBlock);
	else
		SetSelection(ptEndOfBlock, ptEndOfBlock);
	SetCursorPos(ptEndOfBlock);
	EnsureVisible(ptEndOfBlock);

	//m_pTextBuffer->FlushUndoGroup();

	return TRUE;
}

CString CCrystalEditView::GetSelText()
{
	if (! IsSelection())
		return "";

	CPoint ptStart,ptEnd;
	GetSelection(ptStart,ptEnd);
	ASSERT_VALIDTEXTPOS(ptStart);
	ASSERT_VALIDTEXTPOS(ptEnd);

	CString strSelText;
	GetText(ptStart,ptEnd,strSelText);

	return strSelText;
}

BOOL CCrystalEditView::OnCharBefore(UINT nChar)
{
	return TRUE;
}

void CCrystalEditView::OnCharAfter(UINT nChar)
{
	SetCompositionWindow();
}


LRESULT CCrystalEditView::OnImeComposition(WPARAM wp, LPARAM lp)
{
	HKL hkl = GetKeyboardLayout(0);

	if (LOWORD(hkl) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
	{
		SetCompositionWindow();

		if (GCS_RESULTSTR & lp)
		{
			HIMC imc = ImmGetContext(GetSafeHwnd());
			char temp[1024];
			memset(temp, 0, 1024);
			int sz = ImmGetCompositionString(imc, GCS_RESULTSTR, temp, 1024);
		//	if (sz)
			{
				m_bImeSelChange = FALSE;

				if (!m_bIme)
				{
					IMEReplaceText(temp);
				}
				else
				{
					IMEReplaceText(temp);
					m_pTextBuffer->FlushUndoGroup();
				}

				CPoint ptSelStart, ptSelEnd;
				GetSelection(ptSelStart, ptSelEnd);
				SetSelection(ptSelEnd, ptSelEnd);

				m_bImeSelChange = TRUE;
			}


			m_bIme = FALSE;
			ImmReleaseContext(GetSafeHwnd(), imc);
			return 0;
		}
		else if (GCS_COMPSTR & lp)
		{
			HIMC imc = ImmGetContext(GetSafeHwnd());
			char temp[1024];
			memset(temp, 0, 1024);
			int sz = ImmGetCompositionString(imc, GCS_COMPSTR, temp, 1024);

		//	if (sz)
			{
				m_bImeSelChange = FALSE;
				CPoint ptSelStart, ptSelEnd;
				CPoint ptSelStart2, ptSelEnd2;
				GetSelection(ptSelStart, ptSelEnd);

				if (!m_bIme)
				{
					m_pTextBuffer->BeginUndoGroup();
					IMEReplaceText(temp);
				}
				else
				{
					IMEReplaceText(temp);
				}

				GetSelection(ptSelStart2, ptSelEnd2);
				SetSelection(ptSelStart, ptSelEnd2);
				m_bImeSelChange = TRUE;
			}

			m_bIme = TRUE;
			ImmReleaseContext(GetSafeHwnd(), imc);

			return 0;
		}
	}
	else
	{
		SetCompositionWindow();
	}

	return DefWindowProc(WM_IME_COMPOSITION, wp, lp);
}

void CCrystalEditView::SetCompositionWindow()
{
	COMPOSITIONFORM cf;
	memset(&cf, 0, sizeof(cf));
	cf.dwStyle = CFS_POINT | CFS_FORCE_POSITION;

	CPoint ptCursor = GetCursorPos();
	CPoint ptCursorClient = TextToClient(ptCursor);

	cf.ptCurrentPos.x = ptCursorClient.x;
	cf.ptCurrentPos.y = ptCursorClient.y;
	ImmSetCompositionWindow(ImmGetContext(m_hWnd), &cf);

	LOGFONT lf;
	GetFont(lf);
	ImmSetCompositionFont(ImmGetContext(m_hWnd), &lf);
}

LRESULT CCrystalEditView::OnImeNotify(WPARAM wp, LPARAM lp)
{
	return DefWindowProc(WM_IME_NOTIFY, wp, lp);
}

LRESULT CCrystalEditView::OnImeEndComposition(WPARAM wp, LPARAM lp)
{
	return DefWindowProc(WM_IME_ENDCOMPOSITION, wp, lp);
}

void CCrystalEditView::MyEndIme()
{
//	HKL kl = GetKeyboardLayout(0);
//	DWORD n = ImmGetProperty(kl, IGP_SENTENCE);
//	if (!n)
	{
		HIMC imc = ImmGetContext(GetSafeHwnd());
		char temp[1024];
		memset(temp, 0, 1024);
		int sz = ImmGetCompositionString(imc, GCS_RESULTSTR, temp, 1024);
		if (sz)
		{
			if (IsSelection())
			{
				ReplaceSelection(temp);

				CPoint pts, pte;
				GetSelection(pts, pte);
				SetSelection(pte, pte);
			}
			else
			{
				InsertText(temp);
			}
		}
	}
}

void CCrystalEditView::OnSelectChange()
{
	if (m_bImeSelChange)
	{
		HIMC imc = ImmGetContext(GetSafeHwnd());
		if (imc) ImmNotifyIME(imc, NI_COMPOSITIONSTR, CPS_COMPLETE, true);
		ImmReleaseContext(GetSafeHwnd(), imc);
		if (m_bIme)
		{
			m_pTextBuffer->FlushUndoGroup();
			m_bIme = FALSE;
		}
	}
}

void CCrystalEditView::IMEReplaceText(LPCTSTR pszNewText)
{
	if (strlen(pszNewText) == 0)
	{
		CPoint ptSelStart, ptSelEnd;
		GetSelection(ptSelStart, ptSelEnd);
		if (ptSelStart == ptSelEnd)
		{
			int nLength = GetLineLength(ptSelEnd.y);
			if (ptSelEnd.x == GetLineLength(ptSelEnd.y))
			{
				if (ptSelEnd.y == GetLineCount() - 1)
					return;
				ptSelEnd.y ++;
				ptSelEnd.x = 0;
			}
			else
			{
				LPCTSTR pszChars = GetLineChars(ptSelEnd.y);
				if (ptSelEnd.x < nLength - 1 && __ismbslead(pszChars, ptSelEnd.x))
				{
					ptSelEnd.x += 2;
				}
				else
				{
					ptSelEnd.x ++;
				}
			}
		}

		CPoint ptCursorPos = ptSelStart;
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);

		m_pTextBuffer->DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x);
		return;
	}

	if (IsSelection())
	{
		// replace
		DeleteCurrentSelection();

		CPoint ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		int x = ptCursorPos.x;
		int y = ptCursorPos.y;
		if (_tcslen(pszNewText) > 0)
			m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszNewText, y, x);
		CPoint ptEndOfBlock = CPoint(x, y);
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		ASSERT_VALIDTEXTPOS(ptEndOfBlock);
		SetAnchor(ptEndOfBlock);
		SetSelection(ptCursorPos, ptEndOfBlock);
		SetCursorPos(ptEndOfBlock);
		EnsureVisible(ptEndOfBlock);
	}
	else
	{
		BOOL bSelection = IsSelection();

		DeleteCurrentSelection();

		CPoint ptCursorPos = GetCursorPos();
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		int x, y;
		if (! m_pTextBuffer->InsertText(this, ptCursorPos.y, ptCursorPos.x, pszNewText, y, x) )
			return;
		CPoint ptEndOfBlock = CPoint(x, y);
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		ASSERT_VALIDTEXTPOS(ptEndOfBlock);
		SetAnchor(ptEndOfBlock);
		if (bSelection)
			SetSelection(ptCursorPos, ptEndOfBlock);
		else
			SetSelection(ptEndOfBlock, ptEndOfBlock);
		SetCursorPos(ptEndOfBlock);
		EnsureVisible(ptEndOfBlock);
	}
}