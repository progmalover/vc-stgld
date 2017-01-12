////////////////////////////////////////////////////////////////////////////
//	File:		CCrystalTextBuffer.h
//	Version:	1.0.0.0
//	Created:	29-Dec-1998
//
//	Author:		Stcherbatchenko Andrei
//	E-mail:		windfall@gmx.de
//
//	Interface of the CCrystalTextBuffer class, a part of Crystal Edit -
//	syntax coloring text editor.
//
//	You are free to use, distribute or modify this code
//	but LEAVE THIS HEADER INTACT.
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CCrystalTextView.h"

#ifndef __AFXTEMPL_H__
#pragma message("Include <afxtempl.h> in your stdafx.h to avoid this message")
#include <afxtempl.h>
#endif

#ifndef CRYSEDIT_CLASS_DECL
#define CRYSEDIT_CLASS_DECL
#endif
#define AFX_ZERO_INIT_OBJECT(base_class) \
	memset(((base_class*)this)+1, 0, sizeof(*this) - sizeof(class base_class));
enum LINEFLAGS
{
	LF_BOOKMARK_FIRST			= 0x00000001L,
	LF_EXECUTION				= 0x00010000L,
	LF_BREAKPOINT				= 0x00020000L,
	LF_COMPILATION_ERROR		= 0x00040000L,
	LF_INVALID_BREAKPOINT		= 0x00080000L
};
//#define LF_BOOKMARK(id)		(LF_BOOKMARK_FIRST << id)
#define LF_BOOKMARK			0x00000001L

enum CRLFSTYLE
{
	CRLF_STYLE_AUTOMATIC	= -1,
	CRLF_STYLE_DOS			= 0,
	CRLF_STYLE_UNIX			= 1,
	CRLF_STYLE_MAC			= 2,
};


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer class
//todo: document this
class CUpdateContext
{
public:
	virtual void RecalcPoint(CPoint &ptPoint) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer command target

class CRYSEDIT_CLASS_DECL CCrystalTextBuffer : public CCmdTarget
{
	DECLARE_DYNCREATE(CCrystalTextBuffer)

public:
	static BOOL m_bCreateBackupFile;
private:
	BOOL m_bInit;
	BOOL m_bReadOnly;
	BOOL m_bModified;
	int m_nCRLFMode;
	int m_nUndoBufSize;
	int FindLineWithFlag(DWORD dwFlag);

protected:
#pragma pack(push, 1)
	//	Nested class declarations
	struct SLineInfo
	{
		TCHAR	*m_pcLine;
		int		m_nLength, m_nMax;
		DWORD	m_dwFlags;

		SLineInfo() { memset(this, 0, sizeof(SLineInfo)); };
	};

	enum
	{
		UNDO_INSERT = 0x0001,
		UNDO_BEGINGROUP	= 0x0100,
	};

public:
	struct SUndoRecord
	{
		DWORD	m_dwFlags;
		CPoint	m_ptStartPos, m_ptEndPos;			//	Block of text participating
		TCHAR	*m_pcText;							//	Text participating

		SUndoRecord() { memset(this, 0, sizeof(SUndoRecord)); };
	};
#pragma pack(pop)

	class CInsertContext : public CUpdateContext
	{
	public:
		CPoint m_ptStart, m_ptEnd;
		virtual void RecalcPoint(CPoint &ptPoint);
	};

	class CDeleteContext : public CUpdateContext
	{
	public:
		CPoint m_ptStart, m_ptEnd;
		virtual void RecalcPoint(CPoint &ptPoint);
	};

	//	Lines of text
	CArray <SLineInfo, SLineInfo&> m_aLines;

	//	Undo
	CArray <SUndoRecord, SUndoRecord&> m_aUndoBuf;
	int m_nUndoPosition;
	int m_nSyncPosition;
	BOOL m_bUndoGroup, m_bUndoBeginGroup;
	BOOL m_bUndo;

	//	Connected views
	CList <CCrystalTextView *, CCrystalTextView *> m_lpViews;

	//	Helper methods
	void InsertLine(LPCTSTR pszLine, int nLength = -1, int nPosition = -1);
	void AppendLine(int nLineIndex, LPCTSTR pszChars, int nLength = -1);

	//	Implementation
	/////////////////////////////////////////////////////////////////////////
	// I changed these 2 functions to virtual because i want to overload
	// them, so i can refresh the widgets list in current file just in time
	virtual BOOL InternalInsertText(CCrystalTextView *pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar);
	virtual BOOL InternalDeleteText(CCrystalTextView *pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);
	void AddUndoRecord(BOOL bInsert, const CPoint &ptStartPos, const CPoint &ptEndPos, LPCTSTR pszText);

// Attributes
public:

// Operations
public:
	BOOL m_bEnableUndo;
	BOOL m_bModifiedAfterLastCharInput;
	//	Construction/destruction code
	CCrystalTextBuffer();
	~CCrystalTextBuffer();

	//	Basic functions
	BOOL InitNew(int nCrlfStyle = CRLF_STYLE_DOS, CString strDefault = _T(""));
	BOOL LoadFromFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
	BOOL SaveToFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC, BOOL bClearModifiedFlag = TRUE);
	void FreeAll();

	BOOL SaveUndoBuffer(CArchive &ar);
	BOOL LoadUndoBuffer(CArchive &ar);

	//	'Dirty' flag
	virtual void SetModified(BOOL bModified = TRUE);
	BOOL IsModified() const { return m_bModified; };

	//	Connect/disconnect views
	void AddView(CCrystalTextView *pView);
	void RemoveView(CCrystalTextView *pView);

	//	Text access functions
	int GetLineCount();
	int GetLineLength(int nLine);
	LPTSTR GetLineChars(int nLine);
	DWORD GetLineFlags(int nLine);
	int GetLineWithFlag(DWORD dwFlag);
	void SetLineFlag(int nLine, DWORD dwFlag, BOOL bSet, BOOL bRemoveFromPreviousLine = TRUE);
	void GetText(int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString &text, LPCTSTR pszCRLF = NULL);

	//	Attributes
	int GetCRLFMode();
	void SetCRLFMode(int nCRLFMode);
	BOOL GetReadOnly() const;
	void SetReadOnly(BOOL bReadOnly = TRUE);

	//	Text modification functions
	BOOL InsertText(CCrystalTextView *pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar);
	BOOL DeleteText(CCrystalTextView *pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);

	void EnableUndo(BOOL bEnable);

	BOOL CanUndo();
	BOOL CanRedo();
	BOOL Undo(CPoint &ptCursorPos);
	BOOL Redo(CPoint &ptCursorPos);

	//	Undo grouping
	void BeginUndoGroup(BOOL bMergeWithPrevious = FALSE);
	void FlushUndoGroup();

	void EmptyUndoBuffer();

	//	Notify all connected views about changes in text
	void UpdateViews(CCrystalTextView *pSource, CUpdateContext *pContext,
					DWORD dwUpdateFlags, int nLineIndex = -1);

	// add by sunli
	SUndoRecord *GetLastUndoRecord();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrystalTextBuffer)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCrystalTextBuffer)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
