// SampleView.cpp : implementation of the CASView class
//

#include "stdafx.h"
#include "Resource.h"

#include "ASDoc.h"
#include ".\ASView.h"

#include "TipWnd.h"
#include "MemberListBox.h"

#include "SWFProxy.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldCharacter.h"
#include "gldAction.h"

#include "my_app.h"

#include "CmdUpdateObj.h"
#include "editcmd.h"

#include "gldDataKeeper.h"
#include "help.h"
#include "filepath.h"
#include "Options.h"

#include "Regexx.h"
using namespace regexx;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//class CCmdChangeAction2 : public CCmdUpdateObjUniId
//{
//public:
//	CCmdChangeAction2(void *pActionContainer, gldAction **ppAction, ActionSegmentType nActionType, LPCTSTR lpszSourceCode) : CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
//	{
//		_M_Desc_ID = IDS_CMD_CHANGEACTION;
//
//		m_pActionContainer = pActionContainer;
//		m_ppAction = ppAction;
//		m_nActionType = nActionType;
//
//		if (*m_ppAction)
//			m_useCompiledActionBin = (*m_ppAction)->m_useCompiledActionBin;
//		else
//			m_useCompiledActionBin = true;
//
//		if (*ppAction)
//			m_sCodeOld = (*ppAction)->GetSourceCode();
//		if (lpszSourceCode != NULL)
//			m_sCodeNew = lpszSourceCode;
//	}
//
//	~CCmdChangeAction2()
//	{
//
//	}
//
//private:
//	bool Execute()
//	{
//		BOOL bRedraw = FALSE;
//		ChangeActionType cat = catUpdate;
//		gldAction *pAction = *m_ppAction;
//		if (!m_sCodeNew.empty())
//		{
//			if (*m_ppAction == NULL)
//			{
//				*m_ppAction = new gldAction();
//				(*m_ppAction)->m_type = m_nActionType;
//
//				bRedraw = TRUE;
//				pAction = *m_ppAction;
//				cat = catInsert;
//			}
//			
//			(*m_ppAction)->SetSourceCode(m_sCodeNew.c_str());
//		}
//		else
//		{
//			if (*m_ppAction != NULL)
//			{
//				delete *m_ppAction;
//				*m_ppAction = NULL;
//
//				bRedraw = TRUE;
//				cat = catRemove;
//			}
//		}
//
//		if (*m_ppAction != NULL)
//			(*m_ppAction)->m_useCompiledActionBin = true;
//
//		UpdateUniId();
//
//		CSubjectChangeAction *pSubject = (CSubjectChangeAction *)CSubjectManager::Instance()->GetSubject("ChangeAction2");
//		pSubject->Assign(m_pActionContainer, pAction, m_nActionType, cat);
//		pSubject->Notify(pSubject);
//
//		if (bRedraw)
//			CGlandaTimeLine::Instance()->Notify(TL_REDRAW, NULL);
//
//		return TCommand::Execute();
//	}
//
//	bool Unexecute()
//	{
//		BOOL bRedraw = FALSE;
//		ChangeActionType cat = catUpdate;
//		gldAction *pAction = *m_ppAction;
//		if (!m_sCodeOld.empty())
//		{
//			if (*m_ppAction == NULL)
//			{
//				*m_ppAction = new gldAction();
//				(*m_ppAction)->m_type = m_nActionType;
//
//				bRedraw = TRUE;
//				cat = catInsert;
//				pAction = *m_ppAction;
//			}
//			(*m_ppAction)->SetSourceCode(m_sCodeOld.c_str());
//		}
//		else
//		{
//			if (*m_ppAction != NULL)
//			{
//				delete *m_ppAction;
//				*m_ppAction = NULL;
//
//				bRedraw = TRUE;
//				cat = catRemove;
//			}
//		}
//
//		if (*m_ppAction != NULL)
//			(*m_ppAction)->m_useCompiledActionBin = m_useCompiledActionBin;
//
//		UpdateUniId();
//
//		CSubjectChangeAction *pSubject = (CSubjectChangeAction *)CSubjectManager::Instance()->GetSubject("ChangeAction2");
//		pSubject->Assign(m_pActionContainer, pAction, m_nActionType, cat);
//		pSubject->Notify(pSubject);
//
//		if (bRedraw)
//			CGlandaTimeLine::Instance()->Notify(TL_REDRAW, NULL);
//
//		return TCommand::Unexecute();
//	}
//
//	void *m_pActionContainer;
//	gldAction **m_ppAction;
//	ActionSegmentType m_nActionType;
//	bool m_useCompiledActionBin;
//	std::string m_sCodeOld;
//	std::string m_sCodeNew;
//};
//
//class CCmdMakeActionEditable : public CCmdUpdateObjUniId
//{
//public:
//	CCmdMakeActionEditable(gldAction *pAction) : CCmdUpdateObjUniId(CSWFProxy::GetCurObj())
//	{
//		_M_Desc_ID = IDS_CMD_CHANGEACTION;
//
//		m_pAction = pAction;
//	
//		ASSERT(!m_pAction->m_useCompiledActionBin);
//		ASSERT(m_pAction->m_useActionScript20);
//	}
//
//	~CCmdMakeActionEditable()
//	{
//
//	}
//
//private:
//	bool Execute()
//	{
//		ASSERT(!m_pAction->m_useCompiledActionBin);
//		ASSERT(m_pAction->m_useActionScript20);
//
//		m_pAction->m_useCompiledActionBin = true;
//		m_pAction->m_useActionScript20 = false;
//
//		UpdateUniId();
//
//		return TCommand::Execute();
//	}
//
//	bool Unexecute()
//	{
//		ASSERT(m_pAction->m_useCompiledActionBin);
//		ASSERT(!m_pAction->m_useActionScript20);
//
//		m_pAction->m_useCompiledActionBin = false;
//		m_pAction->m_useActionScript20 = true;
//
//		UpdateUniId();
//
//		return TCommand::Unexecute();
//	}
//
//	gldAction *m_pAction;
//};


//void LeaveActionScriptEdit()
//{
//	CWnd *pWnd = CWnd::GetFocus();
//	CASView *pView = CPropAction2::Instance()->GetActiveView();
//	if (pWnd && pWnd == pView)
//		pView->Submit();
//}

#define IsTipVisible() (::IsWindow(CTipWnd::Instance()->m_hWnd) && CTipWnd::Instance()->IsWindowVisible())

// find var declaration in current context
CVarInfo *CASView::FindVar(LPCTSTR lpszVar, BOOL &bDeleteVar)
{
	CVarInfo *pVar = NULL;

	// find in the Global class
	CClassInfo *pClass = g_ASParser.FindClass("Global");
	ASSERT(pClass);
	if (pClass)
		pVar = pClass->FindVar(lpszVar);
	
	// find in MovieClip, because everything belongs to its parent MovieClip.
	// for example, _parent is reference to MovieClip::_parent
	if (!pVar)
	{
		CClassInfo *pClass = g_ASParser.FindClass("MovieClip");
		ASSERT(pClass);
		if (pClass)
			pVar = pClass->FindVar(lpszVar);
	}

	if (!pVar)
	{
		if (_tcsicmp(lpszVar, "this") == 0)
		{
			CClassInfo *pClass = g_ASParser.FindClass("MovieClip");
			ASSERT(pClass);
			if (pClass)
			{
				pVar = new CVarInfo("this", "MovieClip");
				bDeleteVar = TRUE;
			}
		}
	}

	// find local variables in current script block
	if (!pVar)
	{
		CASDoc *pDoc = GetDocument();
		if (pDoc->m_bModifiedAfterParsing)
		{
			CString strText;
			GetText(strText);
			pDoc->m_pParser->ParseBuffer(strText);
			pDoc->m_bModifiedAfterParsing = FALSE;
		}
		pVar = pDoc->m_pParser->FindVar(lpszVar);
	}

	bDeleteVar = FALSE;

	// find instance name
	if (!pVar)
	{
		pVar = FindInstance(lpszVar);
		if (!pVar)
			return NULL;
		bDeleteVar = TRUE;
	}

	return pVar;
}

// find named instance in curren timeline
CVarInfo *CASView::FindInstance(LPCTSTR lpszName)
{
	gldMovieClip *pMC = _GetCurMovieClip();
	if (pMC)
	{
		for (GLAYER_LIST_IT itLayer = pMC->m_layerList.begin(); itLayer != pMC->m_layerList.end(); itLayer++)
		{
			gldLayer *pLayer = *itLayer;
			for (GFRAMECLIP_LIST_IT itFC = pLayer->m_frameClipList.begin(); itFC != pLayer->m_frameClipList.end(); itFC++)
			{
				gldFrameClip *pFC = *itFC;
				for (GCHARACTERKEY_LIST_IT itCK = pFC->m_characterKeyList.begin(); itCK != pFC->m_characterKeyList.end(); itCK++)
				{
					gldCharacterKey *pCK = *itCK;
					if (_tcsicmp(pCK->m_name.c_str(), lpszName) == 0)
					{
						TRACE1("%s\n", pCK->m_name.c_str());
						CVarInfo *pVar = new CVarInfo(lpszName, pCK->GetObj()->GetGObjType() == gobjButton ? "Button" : "MovieClip");
						return pVar;
					}
				}
			}
		}
	}
	return NULL;
}

#define IDT_QUICKINFO	1

/////////////////////////////////////////////////////////////////////////////
// CASView

IMPLEMENT_DYNCREATE(CASView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CASView, CCrystalEditView)
	//{{AFX_MSG_MAP(CASView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
	ON_WM_ENABLE()
	ON_WM_PAINT()

	ON_COMMAND(ID_EDIT_LIST_MEMBERS, OnEditListMembers)
	ON_COMMAND(ID_EDIT_PARAMETER_INFO, OnEditParameterInfo)

	ON_REGISTERED_MESSAGE(WM_MEMBERLISTBOX, OnMemberListMessage)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_EDIT_REFERENCE, OnEditReference)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_EDIT_COMPLETE_WORD, OnEditCompleteWord)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CASView construction/destruction

CASView::CASView()
{
	// TODO: add construction code here

	m_hAccel = NULL;

	m_bSelMargin = COptions::Instance()->m_bShowSelMargin;
	m_bShowLineNumber = COptions::Instance()->m_bShowLineNumbers;
	m_bAutoIndent = COptions::Instance()->m_bAutoIndent;

	m_nActionType = actionSegNone;
	m_pActionContainer = NULL;

	m_bMemberListVisible = FALSE;

	m_bSubmiting = FALSE;
}

CASView::~CASView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CASView diagnostics

#ifdef _DEBUG
void CASView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CASView::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

CASDoc* CASView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CASDoc)));
	return (CASDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CASView message handlers

void CASView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CRect rc;

	GetClientRect(rc);
	ClientToScreen(&rc);

	if (rc.PtInRect(point))
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_ASVIEW))
		{
			CMenu *pPopup = menu.GetSubMenu(0);
			if (pPopup)
				pPopup->TrackPopupMenu(TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
}

UINT CASView::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	return DLGC_WANTALLKEYS;
	//return CCrystalEditView::OnGetDlgCode();
}

BOOL CASView::OnCharBefore(UINT nChar)
{
	if (m_bMemberListVisible && !IsVarChar((char)nChar))
		HideMemberList(TRUE);
	return TRUE;
}

void CASView::OnCharAfter(UINT nChar)
{
	if (m_bMemberListVisible && IsVarChar((char)nChar))
	{
		m_ptAutoCompleteEnd.x++;

		CString strAutoCompleteText;
		CPoint ptCursor = GetCursorPos();
		LocateTextBuffer()->GetText(m_ptAutoCompleteStart.y, m_ptAutoCompleteStart.x, ptCursor.y, ptCursor.x, strAutoCompleteText);
		CMemberListBox::Instance()->FindStringEx(strAutoCompleteText);
	}

	switch (nChar)
	{
		case '.':
			if (COptions::Instance()->m_bAutoListMembers)
				ShowMemberList();
			break;

		case '(':
			if (COptions::Instance()->m_bAutoShowParameterInfo)
				ShowParameterInfo();
			break;
	}
}

class CExpElement  
{
public:
	int m_nType;
	CString m_strText;
	int m_nEndPos;
	int m_nStartPos;
	CExpElement();
	virtual ~CExpElement();

};

void ConverBackslashToSlash(LPTSTR pszLine)
{
	TCHAR *p = pszLine;
	while (true)
	{
		p = strchr(p, '\\');
		if (p == NULL)	// no more '\\' found
			break;

		p++;
		if (*p == '\0')	// end of string
			break;

		if (*p == '\\')	// 2 continuous '\\' found
		{
			*(p - 1) = '/';
			*p = '/';
		}
	}
}

BOOL IsPosInString(LPCTSTR lpszLine, int pos)
{
	int len = (int)strlen(lpszLine);
	int count = 0;
	int xStart = 0;
	LPCTSTR p = lpszLine;
	while (p < lpszLine + len && p < lpszLine + pos)
	{
		p = strchr(p, '\"');
		if (!p || p >= lpszLine + pos)
			break;
		if (p >= lpszLine && *p != '\\')
			count++;
		p++;
	}

	return count % 2 == 1;
}

BOOL IsContinuousLine(LPCTSTR lpszLine, int nLength)
{
	for (int i = nLength - 1; i >= 0; i--)
	{
		if (lpszLine[i] == ' ' || lpszLine[i] == '\t')
			continue;

		if (lpszLine[i] == ';' && !IsPosInString(lpszLine, i))
			return FALSE;

		return TRUE;
	}
	
	return TRUE;
}

BOOL CASView::ShowParameterInfo()
{
	CPoint point = GetCursorPos();

	int nLineIndex = point.y;
	ASSERT(nLineIndex <= GetLineCount());
	int nLineLength = GetLineLength(nLineIndex);
	ASSERT(point.x <= nLineLength);

	int nStartLine = point.y;
	int nTotalLength = nLineLength;

	// try to get multiple lines if possible
	for (int i = point.y - 1; i >= 0; i--)
	{
		nLineLength = GetLineLength(i);
		if (!::IsContinuousLine(GetLineChars(i), nLineLength))
			break;
		nStartLine = i;
		nTotalLength += nLineLength + 1;
	}

	TCHAR *pszLine = (TCHAR *)alloca(sizeof(TCHAR) * (nTotalLength + 1));
	if (!pszLine)
		return FALSE;
	memset(pszLine, 0, (nTotalLength + 1) * sizeof(TCHAR));

	TCHAR *p = pszLine;
	for (int j = nStartLine; j <= point.y; j++)
	{
		nLineLength = GetLineLength(j);
		memcpy(p, GetLineChars(j), nLineLength);
		p += nLineLength;

		// not the last line, add a seperator
		if (j < point.y)
			*p++ = ' ';

		//if (nStartLine > point.y)		// do this if it's a multi-line express
		//	if (j < point.y)			// must do for the last line
		//		while (*--p != ';')
		//			;
	}

	//TRACE1("pszLine=%s\n", pszLine);

	// replace "\\" with "//", make it easy to find string position.
	ConverBackslashToSlash(pszLine);


	point.x += (int)_tcslen(pszLine) - GetLineLength(point.y);
	int nLineStartPos = 0;

	int xStart = point.x;

	// condition:
	// func("xxx|
	// if caret in a string, must get out of it
	if (IsPosInString(pszLine, xStart))
	{
		while (xStart > 0)
		{
			xStart--;
			if (pszLine[xStart] == '\"')
				break;
		}
		if (xStart == 0)
			return FALSE;
	}


	BOOL nInBracket = 1;

	// look for '('
	while (xStart > 0)
	{
		// skip '...' and "..."
		if (pszLine[xStart - 1] == '\"')
		{
			xStart--;
			while (xStart > 0)
			{
				xStart--;
				if (pszLine[xStart] == '\"')
					break;
			}
			continue;
		}
		else
		if (pszLine[xStart - 1] == ')')
		{
			nInBracket++;
			xStart--;
			continue;
		}
		else
		{
			xStart--;
			if (pszLine[xStart] == '(' || pszLine[xStart] == ' ' || pszLine[xStart] == '\t')
			{
				if (pszLine[xStart] == ' ' || pszLine[xStart] == '\t')
				{
					int xStartTest = xStart;
					// bypass all other blanks
					while (xStartTest > 0 && (pszLine[xStartTest - 1] == ' ' || pszLine[xStartTest - 1] == '\t'))
						xStartTest--;

					// check if it's a "function like" statement ahead
					if (!(xStartTest > 0 && IsVarChar(pszLine[xStartTest - 1])))
						continue;

					BOOL bContinue = FALSE;
					while (xStartTest > 0)
					{
						if (pszLine[xStartTest - 1] != ' ' && pszLine[xStartTest - 1] != '\t')
						{
							bContinue = TRUE;
							break;
						}
						xStartTest--;
					}
					if (bContinue)
						continue;
				}
				else
				{
					// bybass all spaces between func/array name and the barects
					// Ex: func    (...)
					while (xStart > 0 && (pszLine[xStart - 1] == ' ' || pszLine[xStart - 1] == '\t'))
						xStart--;
				}

				nInBracket--;
				// nInBracket == 0: func(((i+j))|
				// nInBracket <  0: func((i+j|
				if (nInBracket <= 0)
				{
					int xStartFunc = xStart;
					// seek the beginning of the function
					while (xStartFunc > 0 && IsVarChar(pszLine[xStartFunc - 1]))
						xStartFunc--;

					if (xStartFunc < xStart)
					{
						CString strFunc;
						strFunc.Append(pszLine + xStartFunc, xStart - xStartFunc);
						TRACE1("Func: %s\n", (LPCTSTR)strFunc);

						CString strVar;
						
						int xStartVar = xStartFunc;
						if (xStartVar > 0 && pszLine[xStartVar - 1] == '.')
						{
							xStartVar--;
							while (xStartVar > 0 && IsVarChar(pszLine[xStartVar - 1]))
								xStartVar--;
							
							//if (xStartVar == 0 || pszLine[xStartVar - 1] != '.')
							//{
								strVar.Append(pszLine + xStartVar, xStartFunc - xStartVar - 1);
								TRACE1("Var: %s\n", (LPCTSTR)strVar);
							//}
						}

						CASDoc *pDoc = GetDocument();
						if (pDoc->m_bModifiedAfterParsing)
						{
							CString strText;
							GetText(strText);
							pDoc->m_pParser->ParseBuffer(strText);
							pDoc->m_bModifiedAfterParsing = FALSE;
						}

						CFuncInfo *pFunc = NULL;
						if (strVar.IsEmpty())
						{
							// find in Global class
							CClassInfo *pClass = g_ASParser.FindClass("Global");
							if (pClass)
								pFunc = pClass->FindFunc(strFunc);

							// find in MovieClip (current time), for example, getDepth()
							if (pFunc == NULL)
							{
								pClass = g_ASParser.FindClass("MovieClip");
								if (pClass)
									pFunc = pClass->FindFunc(strFunc);
							}

							// find local function
							if (pFunc == NULL)
								pFunc = pDoc->m_pParser->FindFunc(strFunc);
						}
						else
						{
							BOOL bDeleteVar = FALSE;
							CVarInfo *pVar = FindVar(strVar, bDeleteVar);

							if (pVar)
							{
								if (!pVar->m_strType.IsEmpty())
								{
									CClassInfo *pClass = g_ASParser.FindClass(pVar->m_strType);
									if (pClass)
										pFunc = pClass->FindFunc(strFunc);
								}

								if (bDeleteVar)
									delete pVar;
							}
						}

						if (pFunc)
						{
							// calculate current parameter
							int nCurrent = 0;
							if (pFunc->m_aParams.size() > 0)
							{
								while (pszLine[xStart] == ' ' || pszLine[xStart] == '\t')
									xStart++;
								if (pszLine[xStart] == '(')
									xStart++;

								nInBracket = 0;
								while (xStart < point.x)
								{
									// string
									if (pszLine[xStart] == '\"')
									{
										// bypass current '\"'
										xStart++;
										while (xStart < point.x)
										{
											if (pszLine[xStart] == '\"')
											{
												if (pszLine[xStart - 1] != '\\')
												{
													xStart++;
													break;
												}
											}
											xStart++;
										}

										continue;
									}

									// nested function
									if (pszLine[xStart] == '(')
									{
										// bypass current '('
										xStart++;
										nInBracket++;
										continue;
									}

									if (nInBracket > 0)
									{
										if (pszLine[xStart] == ')')
										{
											// bypass current ')'
											xStart++;
											nInBracket--;
											continue;
										}
									}
									else
									{
										if (pszLine[xStart] == ',')
											nCurrent++;
									}

									xStart++;
								}
							}


							// calculate tip position
							CTipWnd *pTip = CTipWnd::Instance();

							CRect rc;
							pTip->GetWindowRect(&rc);

							int nLineHeight = GetLineHeight();
							CPoint ptCursor = GetCursorPos();
							CPoint ptCursorClient = TextToClient(ptCursor);
							ClientToScreen(&ptCursorClient);

							if (!IsTipVisible() || rc.top != ptCursorClient.y + nLineHeight + 1 /*scrolled?*/)
								pTip->SetWindowPos(0, ptCursorClient.x, ptCursorClient.y + nLineHeight + 1, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

							pTip->SetFont(GetFont());
							pTip->SetParameterInfo(pFunc, nCurrent);

							if (!pTip->IsWindowVisible())
								pTip->ShowWindow(SW_SHOWNOACTIVATE);
						}

						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

void CASView::WordFromPos(CPoint point, CString &strWord)
{
	strWord.Empty();
	int nLine = point.y;
	int nLength = GetLineLength(nLine);
	if (nLength > 0)
	{
		TCHAR *pszLine = (TCHAR *)alloca(sizeof(TCHAR) * (nLength + 1));
		if (!pszLine)
			return;
		memcpy(pszLine, GetLineChars(nLine), nLength);
		pszLine[nLength] = '\0';

		LPTSTR p = pszLine + point.x;
		LPTSTR p1 = p, p2 = p;

		while (IsVarChar(*p2))
			p2++;

		while (p1 > pszLine && IsVarChar(*(p1 - 1)))
			p1--;

		if (p1 != p2)
		{
			LPTSTR pszWord = strWord.GetBufferSetLength(p2 - p1);
			memcpy(pszWord, p1, p2 - p1);
		}
	}
}

BOOL CASView::ShowHelp()
{
	CString strWord;
	CPoint point = GetCursorPos();

	WordFromPos(point, strWord);

	HH_AKLINK link;
	link.cbStruct = sizeof(HH_AKLINK);
	link.fReserved = FALSE;
	link.pszKeywords = (LPCTSTR)strWord;
	link.pszUrl = NULL;
	link.pszMsgText = NULL;
	link.pszMsgTitle = NULL;
	link.pszWindow = NULL;
	link.fIndexOnFail = FALSE;

	CString strHelpFile = GetModuleFilePath();
	strHelpFile += "\\Glanda.chm";
	ShowHelpFile("as\\index.htm");
	::HtmlHelp(NULL, strHelpFile, HH_KEYWORD_LOOKUP, (DWORD)&link);

	return TRUE;
}

BOOL CASView::ShowMemberList(BOOL bAutoComplete)
{
	CPoint point = GetCursorPos();

	int nLine = point.y;
	int nLength = point.x;

	TCHAR *pszLine = (TCHAR *)alloca(sizeof(TCHAR) * (nLength + 1));
	if (!pszLine)
		return FALSE;
	memcpy(pszLine, GetLineChars(nLine), nLength);
	pszLine[nLength] = '\0';

	ConverBackslashToSlash(pszLine);
	if (IsPosInString(pszLine, nLength))
		return FALSE;

	TCHAR *pEnd = pszLine + nLength - 1;
	TCHAR *p = pEnd;
	TCHAR *pszClass = NULL;
	TCHAR *pszMember = NULL;

	BOOL bDotFound = FALSE;
	while (p >= pszLine)
	{
		if (IsVarChar(*p) || (!bDotFound && *p == '.'))
		{
			p--;
			if (*p == '.')
				bDotFound = TRUE;
			continue;
		}
		break;
	}

	//if (p == pEnd)
	//{
	//	// List global functions/classes?
	//	return FALSE;
	//}

	TCHAR *pStart = p + 1;

	int nStart;
	CClassInfo *pClass = NULL;
	CClassInfo *pClass1 = NULL;

	Regexx re;
	int ret = re.exec(pStart, "([a-zA-Z_][a-zA-Z0-9_]*)\\.([a-zA-Z_][a-zA-Z0-9_]*|)$");
	if (ret != 0)
	{
		TRACE2("%s.%s\n", re.match[0].atom[0].str().c_str(), re.match[0].atom[1].str().c_str());

		nStart = pStart - pszLine + re.match[0].atom[1].start();

		std::string sVar = re.match[0].atom[0].str();

		BOOL bDeleteVar = FALSE;
		CVarInfo *pVar = FindVar(sVar.c_str(), bDeleteVar);

		if (pVar == NULL || pVar->m_strType.IsEmpty())
		{
			if (bDeleteVar)
				delete pVar;
			return FALSE;
		}

		pClass = g_ASParser.FindClass(pVar->m_strType);
		
		if (bDeleteVar)
			delete pVar;
	}
	else
	{
		if (*pStart == '.')
			return FALSE;

		// no '.' found. Global/MovieClip function or variable.
		nStart = pStart - pszLine;
		pClass = g_ASParser.FindClass("Global");
		pClass1 = g_ASParser.FindClass("MovieClip");
	}

	if (!pClass)
		return FALSE;
    
	CMemberListBox *pList = CMemberListBox::Instance();

	int nLineHeight = GetLineHeight();
	int nItemHeight = pList->SetItemHeight(nLineHeight);
	pList->ResetContent();

	for (FUNC_LIST::iterator it = pClass->m_aFuncs.begin(); it != pClass->m_aFuncs.end(); it++)
		pList->AddItem((*it).second->m_strName, 1);

	for (VAR_LIST::iterator it = pClass->m_aVars.begin(); it != pClass->m_aVars.end(); it++)
		pList->AddItem((*it).second->m_strName, 0);

	if (pClass1)
	{
		for (FUNC_LIST::iterator it = pClass1->m_aFuncs.begin(); it != pClass1->m_aFuncs.end(); it++)
			pList->AddItem((*it).second->m_strName, 1);

		for (VAR_LIST::iterator it = pClass1->m_aVars.begin(); it != pClass1->m_aVars.end(); it++)
			pList->AddItem((*it).second->m_strName, 0);
	}

	int count = pList->GetCount();
	if (count > 0)
	{
		// Get proper position of props-list
		CPoint ptEnd = GetCursorPos();
		CPoint ptStart(max(0, nStart), ptEnd.y);
		EnsureVisible(ptStart);

		m_ptAutoCompleteStart = ptStart;
		m_ptAutoCompleteEnd = ptEnd;

		CString strAutoCompleteText;
		LocateTextBuffer()->GetText(ptStart.y, ptStart.x, ptEnd.y, ptEnd.x, strAutoCompleteText);

		if (bAutoComplete)
		{
			int index = pList->FindString(-1, strAutoCompleteText);
			if (index >= 0)
			{
				int index2 = pList->FindString(index, strAutoCompleteText);
				if (index2 == -1 || index2 == index)	// only one match
				{
					pList->FindStringEx(strAutoCompleteText);
					HideMemberList(TRUE);

					return TRUE;
				}
			}
		}

		CPoint ptStartClient = TextToClient(ptStart);
		ClientToScreen(&ptStartClient);

		// force to use current view's font
		pList->SetFont(GetFont());

		int nWidth = pList->GetMaxItemWidth() + 10;	// 10 is additional space
		if (pList->GetStyle() & WS_VSCROLL)
			nWidth += GetSystemMetrics(SM_CXVSCROLL);
		if (nWidth < 72)
			nWidth = 72;
		int nHeight = nItemHeight * min(count, 10);

		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		CRect rcList(
			ptStartClient.x - 2 - 16 - 4, 
			ptStartClient.y + nLineHeight + 1, 
			ptStartClient.x - 2 - 16 - 4 + nWidth, 
			ptStartClient.y + nLineHeight + 1 + nHeight);

		// Do not cover the parameter info window, 
		// or will cause dislay problem
		if (rcList.bottom > cy || IsTipVisible())
			rcList.OffsetRect(0, -(nLineHeight + 1 + nHeight + pList->GetTrayWnd().GetPadding() * 2 + 2 * 2));

		if (rcList.left < 0)
			rcList.OffsetRect(-rcList.left, 0);

		if (rcList.right > cx)
			rcList.OffsetRect(-(rcList.right - cx), 0);

		// must call SetWindowPos() before calling FindStringEx(), 
		// or EnsureVisible() will not work (can not get count of visible items).
		pList->SetWindowPos(0, 0, 0, rcList.Width(), rcList.Height(), SWP_NOZORDER | SWP_NOMOVE);
		pList->GetTrayWnd().SetWindowPos(&wndTopMost, rcList.left, rcList.top, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE);

		pList->SetOwner(this);

		int index = pList->FindStringEx(strAutoCompleteText);
		if (index >= 0)
			pList->CenterItem(index);

		m_bMemberListVisible = TRUE;

		return TRUE;
	}

	return FALSE;
}

void CASView::OnKillFocus(CWnd* pNewWnd)
{
	CCrystalEditView::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	if (IsTipVisible())
		CTipWnd::Instance()->ShowWindow(SW_HIDE);

	if (m_bMemberListVisible)
		HideMemberList(FALSE);

	//Submit();
}

void CASView::UpdateCaret()
{
	CCrystalEditView::UpdateCaret();

	if (IsTipVisible())
		if (!ShowParameterInfo())
			CTipWnd::Instance()->ShowWindow(SW_HIDE);
}

int CASView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCrystalEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	ModifyStyle(0, WS_CLIPSIBLINGS | WS_TABSTOP);
	ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_STATICEDGE, SWP_FRAMECHANGED);

	CTipWnd *pTip = CTipWnd::Instance();
	if (!::IsWindow(pTip->m_hWnd))
		pTip->Create();

	CMemberListBox *pList = CMemberListBox::Instance();
	if (!::IsWindow(pList->m_hWnd))
		pList->Create();

	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ASVIEW));

	m_tooltip.Create(this, TTS_ALWAYSTIP);
	m_tooltip.SetDelayTime(TTDT_INITIAL, 0);
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, 1000 * 30);
	m_tooltip.Activate(TRUE);

	return 0;
}

void CASView::Submit()
{
}

void CASView::OnEnable(BOOL bEnable)
{
	CCrystalEditView::OnEnable(bEnable);

	// TODO: Add your message handler code here

	Invalidate(FALSE);
}

void CASView::OnPaint()
{
	CCrystalEditView::OnPaint();
}

LRESULT CASView::OnMemberListMessage(WPARAM wp, LPARAM lp)
{
	switch (wp)
	{
	case LBM_SELCHANGE:
		HideMemberList(TRUE);
		break;
	case LBM_SELCANCEL:
		HideMemberList(FALSE);
		break;
	default:
		ASSERT(FALSE);
	}

	return TRUE;
}

void CASView::HideMemberList(BOOL bUpdate)
{
	CMemberListBox *pList = CMemberListBox::Instance();

	if (bUpdate)
	{
		int index = pList->GetCurSel();
		if (index >= 0)
		{
			CString strText;
			pList->GetText(index, strText);
			ASSERT(!strText.IsEmpty());


			m_pTextBuffer->BeginUndoGroup();

			ASSERT(m_ptAutoCompleteEnd.x >= m_ptAutoCompleteStart.x);
			if (m_ptAutoCompleteEnd.x > m_ptAutoCompleteStart.x)
			{
				LPCTSTR pszLine = GetLineChars(m_ptAutoCompleteEnd.y);
				int len = GetLineLength(m_ptAutoCompleteEnd.y);
				LPCTSTR p = pszLine + m_ptAutoCompleteEnd.x;
				while (p < pszLine + len && IsVarChar(*p))
					p++;
				m_pTextBuffer->DeleteText(this, m_ptAutoCompleteStart.y, m_ptAutoCompleteStart.x, m_ptAutoCompleteEnd.y, p - pszLine);
			}

			int nEndLine, nEndChar;
			m_pTextBuffer->InsertText(this, m_ptAutoCompleteStart.y, m_ptAutoCompleteStart.x, strText, nEndLine, nEndChar);
			
			CPoint ptCursorPos(nEndChar, nEndLine);
			SetCursorPosSimply(ptCursorPos);

			m_pTextBuffer->FlushUndoGroup();
		}
	}

	pList->GetParent()->ShowWindow(SW_HIDE);

	m_bMemberListVisible = FALSE;
}

BOOL CASView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message >= WM_KEYDOWN && pMsg->message <= WM_KEYLAST)
	{
		if(m_bMemberListVisible)
		{
			CMemberListBox *pList = CMemberListBox::Instance();

			// because the base class uses VK_LEFT, VK_UP etc. keys as accelerators,
			// i must handle these keys first.

			BOOL bHandled = FALSE;

			if (pMsg->message == WM_SYSKEYDOWN)
			{
				HideMemberList(FALSE);
				bHandled = TRUE;
			}

			if (!bHandled && pMsg->message == WM_KEYDOWN)
			{
				bHandled = TRUE;
				CPoint point;
				switch (pMsg->wParam)
				{
				case VK_RETURN:
				case VK_TAB:
					HideMemberList(TRUE);
					return TRUE;
				case VK_SPACE:
					HideMemberList(TRUE);
					break;
				case VK_ESCAPE:
					HideMemberList(FALSE);
					return TRUE;
				case VK_BACK:
					point = GetCursorPos();
					ASSERT(point.y == m_ptAutoCompleteStart.y);
					{
						SendMessage(WM_COMMAND, ID_EDIT_DELETE_BACK, 0L);

						CPoint ptCursor = GetCursorPos();
						ASSERT(ptCursor.y != point.y || point.x >= ptCursor.x /*equal at (0, 0)*/);
						if (ptCursor.y != point.y || ptCursor.x < m_ptAutoCompleteStart.x)
						{
							HideMemberList(FALSE);
						}
						else
						{
							m_ptAutoCompleteEnd.x -= (point.x - ptCursor.x);
							ASSERT(m_ptAutoCompleteEnd.x >= ptCursor.x);

							CString strAutoCompleteText;
							LocateTextBuffer()->GetText(m_ptAutoCompleteStart.y, m_ptAutoCompleteStart.x, ptCursor.y, ptCursor.x, strAutoCompleteText);
							pList->FindStringEx(strAutoCompleteText);
						}

						return TRUE;
					}
					break;
				case VK_LEFT:
					point = GetCursorPos();
					if (point.x == 0 || point.x - 1 < m_ptAutoCompleteStart.x)
						HideMemberList(FALSE);
					break;
				case VK_RIGHT:
					point = GetCursorPos();
					if (point.x == GetLineLength(m_ptAutoCompleteStart.y) - 1 || point.x + 1 > m_ptAutoCompleteEnd.x)
						HideMemberList(FALSE);
					break;
				case VK_UP:
				case VK_DOWN:
				case VK_HOME:
				case VK_END:
				case VK_PRIOR:
				case VK_NEXT:
					return pList->PostMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
				default:
					bHandled = FALSE;
				}
			}

			// process current view's accelerators
			if (!bHandled)
			{
				int nAccels = CopyAcceleratorTable(m_hAccel, NULL, 0);
				WORD wCmd;
				if (IsAccelerator(m_hAccel, nAccels, pMsg, &wCmd))
				{
					HideMemberList(FALSE);
					bHandled = TRUE;
				}
			}

			// process bass class's accelerators
			if (!bHandled)
			{
				int nAccels = CopyAcceleratorTable(CCrystalEditView::m_hAccel, NULL, 0);
				WORD wCmd;
				if (IsAccelerator(CCrystalEditView::m_hAccel, nAccels, pMsg, &wCmd))
				{
					HideMemberList(FALSE);
					bHandled = TRUE;
				}
			}

			// process CMainFrame's accelerators
			if (!bHandled)
			{
				CFrameWnd *pFrame = (CFrameWnd *)AfxGetMainWnd();

				int nAccels = CopyAcceleratorTable(pFrame->m_hAccelTable, NULL, 0);
				WORD wCmd;
				if (IsAccelerator(pFrame->m_hAccelTable, nAccels, pMsg, &wCmd))
				{
					HideMemberList(FALSE);
					bHandled = TRUE;
				}
			}
		}

		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}

	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		m_tooltip.RelayEvent(pMsg);

	return CCrystalEditView::PreTranslateMessage(pMsg);
}

void CASView::OnEditListMembers()
{
	if (GetFocus() != this && IsWindowEnabled())
		SetFocus();

	if (!ShowMemberList())
		MessageBeep(MB_ICONWARNING);
}

void CASView::OnEditParameterInfo()
{
	if (GetFocus() != this && IsWindowEnabled())
		SetFocus();

	if (!ShowParameterInfo())
		MessageBeep(MB_ICONWARNING);
}

int CASView::GetIndent(CString &strIndent, int nChar, int x, int y)
{
	if (y > 0)
	{	
		int indent = 0;
		if (nChar == VK_RETURN)
		{
			LPCTSTR lpszLine = GetLineChars(y - 1);
			int len = GetLineLength(y - 1);
			for (int i = 0; i < len; i++)
			{
				if (lpszLine[i] == ' ' || lpszLine[i] == '\t')
					indent++;
				else
					break;
			}

			if (indent > 0)
			{
				TCHAR *pszBuf = strIndent.GetBufferSetLength(indent);
				memcpy(pszBuf, lpszLine, sizeof(TCHAR) * indent);
			}

			if (len > 0)
			{
				LPCTSTR p = lpszLine + len - 1;
				while (p > lpszLine && *p == ' ' || *p == '\t')
					p--;
				if (*p == '{')
				{
					strIndent += '\t';
					indent++;
				}
			}

			return indent;
		}
		else
		if (nChar == '}')
		{
			LPCTSTR lpszLine = GetLineChars(y);
			BOOL bFirstChar = TRUE;
			for (int i = x - 1; i >= 0; i--)
			{
				if (lpszLine[i] != ' ' && lpszLine[i] != '\t')
				{
					bFirstChar = FALSE;
					break;
				}
			}

			if (bFirstChar)
			{
				int nMatches = 0;
				for (int i = y - 1; i >= 0; i--)
				{
					lpszLine = GetLineChars(i);
					int len = GetLineLength(i);
					for (int j = len - 1; j >= 0; j--)
					{
						if (lpszLine[j] == '{')
						{
							int nColor = GetColorIndex(CPoint(j, i));
							if (nColor != COLORINDEX_STRING && nColor != COLORINDEX_COMMENT)
							{
								nMatches++;
								if (nMatches == 1)
								{
									for (int k = 0; k < len; k++)
									{
										if (lpszLine[k] == ' ' || lpszLine[k] == '\t')
											indent++;
										else
											break;
									}

									if (indent > 0)
									{
										TCHAR *pszBuf = strIndent.GetBufferSetLength(indent);
										memcpy(pszBuf, lpszLine, sizeof(TCHAR) * indent);
									}

									return indent;
								}
							}
						}
						else
						if (lpszLine[j] == '}')
						{
							int nColor = GetColorIndex(CPoint(j, i));
							if (nColor != COLORINDEX_STRING && nColor != COLORINDEX_COMMENT)
								nMatches--;
						}
					}
				}
			}
		}
	}

	return x;
}

int CASView::GetColorIndex(const CPoint &point)
{
	int nLineIndex = point.y;
	ASSERT(nLineIndex <= GetLineCount());
	int nLength = GetLineLength(nLineIndex);
	ASSERT(point.x <= nLength);

	DWORD dwCookie = GetParseCookie(nLineIndex - 1);
	TEXTBLOCK *pBuf = (TEXTBLOCK *) _alloca(sizeof(TEXTBLOCK) * nLength * 3);
	int nBlocks = 0;
	VERIFY(ParseLine(dwCookie, nLineIndex, pBuf, nBlocks) != (DWORD) -1);

	for (int i = nBlocks - 1; i >= 0; i --)
	{
		ASSERT(pBuf[i].m_nCharPos >= 0 && pBuf[i].m_nCharPos <= nLength);
		if ((i == nBlocks - 1 && pBuf[i].m_nCharPos < point.x /*&& point.x < nLength*/) ||
			(pBuf[i].m_nCharPos < point.x && pBuf[i + 1].m_nCharPos > point.x))
		{
			int nColorIndex = pBuf[i].m_nColorIndex;
			return nColorIndex;
		}
	}
	
	return -1;
}

BOOL CASView::QueryEditable()
{
	return CASViewBase::QueryEditable();
}

void CASView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CASViewBase::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here

	UpdateMenuCmdUI(this, pPopupMenu, nIndex, bSysMenu);
}

void CASView::OnEditReference()
{
	// TODO: Add your command handler code here

	ShowHelp();
}

BOOL CASView::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	// prevent CMainFrame show the help again
	return TRUE;

	//return CASViewBase::OnHelpInfo(pHelpInfo);
}

void CASView::OnEditCompleteWord()
{
	// TODO: Add your command handler code here

	if (GetFocus() != this && IsWindowEnabled())
		SetFocus();

	if (!ShowMemberList(TRUE))
		MessageBeep(MB_ICONWARNING);
}

void CASView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	KillTimer(IDT_QUICKINFO);
	SetTimer(IDT_QUICKINFO, 250, NULL);

	CASViewBase::OnMouseMove(nFlags, point);
}

void CASView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == IDT_QUICKINFO)
	{
		KillTimer(IDT_QUICKINFO);
		ShowQuickInfo();
	}

	CASViewBase::OnTimer(nIDEvent);
}

BOOL CASView::ShowQuickInfo()
{
	/*
	CString strWord;
	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);
	CPoint point2 = ClientToText(point);

	WordFromPos(point2, strWord);

	if (!strWord.IsEmpty())
	{
		if (strWord != m_strWordLast)
		{
			m_tooltip.Pop();
			m_tooltip.DelTool(this, 1);

			m_strWordLast = strWord;


			int nHeight = GetLineHeight();

			AFX_OLDTOOLINFO ti;
			memset(&ti, 0, sizeof(ti));
			ti.cbSize = sizeof(ti);
			ti.hinst = 0;
			ti.hwnd = m_hWnd;
			ti.uFlags = 0;
			ti.lpszText = (LPSTR)(LPCTSTR)strWord;
			ti.uId = 1;

			//ti.rect.left = point.x;
			//ti.rect.top = point.y + nHeight;
			//ti.rect.right = point.x + nHeight;
			//ti.rect.bottom = point.y + nHeight + nHeight;

			GetClientRect(&ti.rect);

			m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);

			m_tooltip.Activate(TRUE);

			TRACE1("%s\n", (LPCTSTR)strWord);
		}

		return TRUE;
	}
	else
	{
		m_strWordLast.Empty();
		m_tooltip.Pop();
		m_tooltip.DelTool(this, 1);
	}
	*/

	return FALSE;
}

BOOL CASView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bMemberListVisible)
	{
		CMemberListBox::Instance()->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y));
		return TRUE;
	}
	else
	{
		return CASViewBase::OnMouseWheel(nFlags, zDelta, pt);
	}
}
