#pragma once

#include "Resource.h"
#include "tool.h"
#include "GGraphics.h"
#include "gld_draft_dc.h"
#include "ShapeDraw.h"
#include <list>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <fstream>
#include <assert.h>
#include "NotifyTransceiver.h"
#include "se_const.h"
#include <afxtempl.h>
#include "Observer.h"
#include "gldLibrary.h"
#include "gldDataKeeper.h"
#include "gldFont.h"
#include "gldTextGlyphRecord.h"
#include "gldText.h"
#include "gldFrameClip.h"
#include "gldLayer.h"
#include "SWFProxy.h"
#include "DrawHelper.h"
#include "gld_func.h"
#include "BinStream.h"
#include "clipboard.h"
#include "Graphics.h"
#include "gldText2_uneditable.h"
#include <imm.h>
#include <set>

#include "CharConvert.h"

#ifndef BOOL2bool
#define BOOL2bool(v) (v == TRUE ? true : false)
#endif

#ifndef bool2BOOL
#define bool2BOOL(v) (v == true ? TRUE : FALSE)
#endif

typedef std::map <std::string, std::string>	STRING_MAP;
typedef std::set <std::string> STRING_SET;

const int nHandleSize = 20 * 10;

#define DEFINE_TECOMMAND(name) class CTECommand_##name : public CTECommand { \
	public: virtual ~CTECommand_##name() {}; \
	std::string GetClassName() { return std::string("##name"); }
#define DEFINE_TECOMMANDGROUP(name) class CTECommand_##name : public CTECommandGroup {
#define END_TECOMMAND() };
#define COMMAND_CLSNAME(name) CTECommand_##name

// 文本块格式眮E?
#define TF_FONTFACE  0x1
#define TF_FONTSIZE  0x2
#define TF_FONTCOLOR 0x4
#define TF_BOLD      0x8
#define TF_ITALIC    0x10
#define TF_SPACING   0x20
#define TF_LOCATE    0x40
#define TF_LINK      0x80
#define TF_TARGET    0x100
#define TF_ALL       0xffffffff

// 段落格式眮E?
#define PF_ALIGNTYPE   0x1
#define PF_SPACING     0x2
#define PF_MARGINLEFT  0x4
#define PF_MARGINRIGHT 0x8
#define PF_INDENT      0x10
#define PF_ALL         0xffffffff

// 文本属性眮E?
#define TP_TYPE        0x1
#define TP_BORDER      0x2
#define TP_CANSELECT   0x4
#define TP_BINDVAR     0x8
#define TP_CANEDIT     0x10
#define TP_MAXLENGTH   0x20
#define TP_MULTITYPE   0x40
#define TP_ALL         0xffffffff

// 文本工具能力
#define TTCAPS_FONTFACE    0x1
#define TTCAPS_FONTSIZE    0x2
#define TTCAPS_FONTCOLOR   0x4
#define TTCAPS_BOLD        0x8
#define TTCAPS_ITALIC      0x10
#define TTCAPS_LOCATE      0x20
#define TTCAPS_CHARSPACING 0x40
#define TTCAPS_LINK        0x80
#define TTCAPS_TARGET      0x100
#define TTCAPS_LINESPACING 0x200
#define TTCAPS_INDENT      0x400
#define TTCAPS_MARGINLEFT  0x800
#define TTCAPS_MARGINRIGHT 0x1000
#define TTCAPS_ALIGN       0x2000
#define TTCAPS_BORDER      0x4000
#define TTCAPS_CANSELECT   0x8000
#define TTCAPS_BINDVAR     0x10000
#define TTCAPS_CANEDIT     0x20000
#define TTCAPS_MAXLENGTH   0x40000
#define TTCAPS_MULTITYPE   0x80000
#define TTCAPS_ALL         0xffffffff
#define STD_FONT_HEIGHT	   1020

typedef struct
{
	DWORD dwCaps;    // 是否允喧彖置的信息
	DWORD dwAllowAS; // 是否允喧铨用到选择区?E
}TTCAPS;

// 文本类型
enum gldText2_TEXTTYPE
{
	TextType_Static,
	TextType_Dynamic,
	TextType_HTML,
};

class CTextEditBase;

#ifndef BORDER_SIZE
#define BORDER_SIZE (2 * 20)
#endif

#ifndef BORDER_HEIGHT
#define BORDER_HEIGHT (2 * 20)
#endif

inline BOOL IsSameColor(COLORREF cr1, COLORREF cr2);
BOOL MakeDiffColor(COLORREF fr, COLORREF bg);

// ######################################################################################### //

// 格式
class CTextFormat
{
public:
	enum emCharLocate
	{
		CL_NONE,
		CL_SUPERSCRIPT,
		CL_SUBSCRIPT,
	};

	PROPERTY(std::string, FontFace); // 字?E
	PROPERTY(int, FontSize);         // 字体大小
	PROPERTY(COLORREF, FontColor);   // 字体颜色
	PROPERTY(BOOL, Bold);            // 粗?E
	PROPERTY(BOOL, Italic);          // 斜?E
	PROPERTY(int, Spacing);          // 字间緛E
	PROPERTY(emCharLocate, Locate);  // 字符位置
	PROPERTY(std::string, Link);     // 链接
	PROPERTY(std::string, Target);   // 链接目眮E

	CTextFormat();
	CTextFormat(const char *lpszFontFace, int nFontSize, COLORREF crColor, 
		BOOL bBold, BOOL bItalic, int nSpacing, emCharLocate Locate, std::string strLink, std::string strTarget);
	CTextFormat(CTextFormat &TextFormat);
	CTextFormat &operator =(CTextFormat &TextFormat);
	BOOL operator ==(CTextFormat &TextFormat);
	void Write(oBinStream &out);
	void Read(iBinStream &in);
	DWORD Compare(CTextFormat &TextFormat, DWORD dwFlag = TF_ALL);
	DWORD Compare(CTextFormat &TextFormat, DWORD dwFlag1, DWORD dwFlag2);
	void ChangeFormat(CTextFormat &TextFormat, DWORD dwFlag = TF_ALL);
	BOOL IsSame(CTextFormat &TextFormat);
};

class CParagraph;

// 文本縼E
class CTextBlock
{
public:
	CTextFormat  TextFormat;        // 文本格式
	std::wstring strText;
	int          nWidth;            // 文本窥胰
	int          nAscent;           // 字体Ascent
	int          nHeight;           // 文本高度
	std::vector <int> CharWidth;    // 每个字的窥胰

private:
	CParagraph  *pParentParagraph; // 所属的段聛E

public:
	CTextBlock(CTextFormat &TextFormat, const wchar_t *pText = L"")
	{
		this->TextFormat = TextFormat;
		strText = pText;
		pParentParagraph = NULL;
	}

	int GetLength();
	void Break(int nIndex, CTextBlock **pBefore, CTextBlock **pAfter);
	void CalcWidth();
	void CalcWidth2();
	void CalcHeight();
	int GetCharWidth(int nIndex);
	int GetWidth() { return nWidth; }
	void SetText(const wchar_t *pText) { strText = pText; CalcWidth(); }
	void SetParent(CParagraph *pParent);
	int GetHeight() { return nHeight; }
	void ChangeFormat(CTextFormat &TextFormat, DWORD dwFlag = TF_ALL);
	int TryChangeFormat(CTextFormat &TextFormat, DWORD dwFlag = TF_ALL);
	void Write(oBinStream &out);
	void Read(iBinStream &in);
	CTextBlock *GetSub(int nStart, int nEnd);
};

class CParagraph;
typedef std::list <CTextBlock *> TextBlocks;
typedef std::vector <CParagraph *> Paragraphs;

// ######################################################################################### //

class CCharIterator
{
private:
	CParagraph *pParagraph;
	TextBlocks::iterator TextBlockIt;
	int        nInBlockIndex;

public:
	CCharIterator(CParagraph *para, int pos);
	virtual ~CCharIterator();
	CCharIterator &operator =(CCharIterator &it);
	bool operator ==(CCharIterator &it);
	bool operator !=(CCharIterator &it);
	CCharIterator &operator ++(int);
	CCharIterator &operator --(int);
	wchar_t operator *();
	POINT GetXY();
	bool IsError();

	friend class CCharIterator;
};

// ######################################################################################### //

class CParaFormat;

// 段聛E

class CParagraph
{
public:
	enum emAlignType // 对?E绞?
	{
		AlignLeft,
		AlignCenter,
		AlignRight,
	};

private:
	CTextEditBase *pEditBase;            // 所属的文本编辑工具
	BOOL bStiffRet;                      // 是否是硬回车
	int  nHeight;                        // 段落的高度
	int  nBaseline;                      // 基线
	int  nDrawY;                         // 绘制的Y坐眮E
	emAlignType AlignType;               // 靠?E嘈?
	PROPERTY(int, Spacing);              // 行緛E
	PROPERTY(int, MarginLeft);           // 左边緛E
	PROPERTY(int, MarginRight);          // 右边緛E
	PROPERTY_ONLYGET(int, Indent);       // 行首缩絹E

	// 整历椅聛E
	BOOL Arrange(int *nProcLine, int &nUpdate);

public:
	TextBlocks m_TextBlockList;

	CParagraph(CTextEditBase *pEditBase, BOOL bStiffRet = TRUE, emAlignType at = AlignLeft, int nLineSpacing = 0,
		int nMarginLeft = 0, int nMarginRight = 0, int nIndent = 0);
	virtual ~CParagraph();
	int GetHeight() { return nHeight; }
	int GetDrawY()  { return nDrawY; }
	BOOL IsStiffRet()  { return bStiffRet; }
	void SetStiffRet(BOOL bStiffRet) { this->bStiffRet = bStiffRet; }
	CTextEditBase *GetEditBase() { return pEditBase; }
	void SetEditBase(CTextEditBase *pb) { pEditBase = pb; }
	int GetBaseLine() { return nBaseline; }
	CParagraph::emAlignType GetAlignType() { return AlignType; }
	int GetPageWidth();
	int GetPageWidth2();
	int GetPageWidth3();
	int GenMinRight(int n);
	void SetIndent(int w) { m_Indent = bStiffRet ? w : 0; }
	void DebugShowWidths();

	// 优化文本块链眮E
	void Optimiz();

	// 获取段落内得文本块总数
	int GetTextBlockCount();

	// 获取段落内得第一个文本縼E
	CTextBlock *GetFirstTextBlock();

	// 获取段落内得煮一个文本縼E
	CTextBlock *GetLastTextBlock();

	// 是否是该段第一个文本縼E
	BOOL IsFirstTextBlock(CTextBlock *pTextBlock);

	// 是否是该段煮一个文本縼E
	BOOL IsLastTextBlock(CTextBlock *pTextBlock);

	// 根据列号返回所属得文本縼E
	TextBlocks::iterator BlockFromIndex(int nIndex, int *nInBlockIndex = NULL, int *nRetIndex = NULL);
	TextBlocks::iterator BlockFromIndex2(int nIndex, int *nInBlockIndex = NULL, int *nRetIndex = NULL);

	// 在当前惆面插?E桓鑫谋究丒
	void InsertToFirst(CTextBlock *pTextBlock);

	// 在当前段落末尾插?E桓鑫谋究丒
	void Append(CTextBlock *pTextBlock);

	// 在当前位置插?E欢挝谋?
	void Insert(int nIndex, wchar_t *pText, BOOL bReDraw = TRUE);
	void Insert2(int nIndex, wchar_t *pText);

	// 在当前位置插?E桓鑫谋究丒
	void Insert(int nIndex, CTextBlock *pTextBlock, BOOL bReDraw = TRUE);
	void Insert2(int nIndex, CTextBlock *pTextBlock);

	// 获取一个文本块绘制的X坐眮E
	int GetDrawX(CTextBlock *pTextBlock);

	// 获取制定位置字符绘制的X坐眮E
	int GetDrawX(int nIndex);

	// 获取当前段落绘制的X坐眮E
	int GetDrawX();

	// 计算段落的高度
	void CalcHeight();

	// 计算绘制的Y坐眮E
	void CalcDrawY();

	// 计算当前行和所有下面的行绘制的Y坐眮E
	void CalcDownY();

	// 获取段落占用的窥胰
	int GetWidth();

	// 整历椅聛E
	// 功能：如果这个段落超出了页面的范围，则把超出的部分截断成一个新的鳃棕车段聛E
	// 如果下一段落是个鳃棕车段落，并且这个段落还有多余的空间，则把下面的能移上来的文本都移上来
	BOOL Arrange(int *nProcLine = NULL);

	int ColumnFromTextBlockPtr(CTextBlock *pTextBlock);

	// 重画这一行
	void ReDraw(BOOL bReDraw = TRUE);

	// 重画这一行和下面所有的行
	void ReDrawDown(BOOL bReDraw = TRUE);

	// 获取该行的文本总数
	int GetLength();

	// 删除这一行
	void Delete(BOOL bReDraw = TRUE);
	void DeleteNoDraw();

	void AutoUpdate(BOOL bReDraw = TRUE);

	// 获取指定X坐眮E诘淖址丒
	int ColumnFromPixelX(int x);

	// 设置对?E绞?
	void SetAlignTypeType(emAlignType at, BOOL bReDraw = TRUE);

	// 序列化函数
	void Write(oBinStream &out);
	void Read(iBinStream &in);

	// 获取能调整的臁窥胰
	int GetAdjustMaxWidth();

	// 寻找一个字符，并且返回位置
	// nDirection = -1(向前) or 1(向簛E
	CTextBlock *FindChar(CTextBlock *pSBlock, int &nIndex, wchar_t c, int nDirection = 1);

	CParagraph *Clone(CTextEditBase *pb);
	std::wstring GetText();

	void GetParaFormat(CParaFormat &fmt);
	static BOOL IsRetChar(wchar_t c);
	CTextBlock *FindRetChar(CTextBlock *pSBlock, int &nIndex, int nDirection = 1);

	CCharIterator Begin();
	CCharIterator End();
};

class CParaFormat
{
public:
	CParaFormat()
	{
	}

	CParaFormat(CParagraph::emAlignType AlignType, int nSpacing, int nMarginLeft,
		int nMarginRight, int nIndent)
	{
		SetAlignType(AlignType);
		SetSpacing(nSpacing);
		SetMarginLeft(nMarginLeft);
		SetMarginRight(nMarginRight);
		SetIndent(nIndent);
	}

	PROPERTY(CParagraph::emAlignType, AlignType); // 靠?E嘈?
	PROPERTY(int, Spacing);                       // 行緛E
	PROPERTY(int, MarginLeft);                   // 左边緛E
	PROPERTY(int, MarginRight);                  // 右边緛E
	PROPERTY(int, Indent);                       // 行首缩絹E
	DWORD Compare(CParaFormat &ParaFormat, DWORD dwFlag1, DWORD dwFlag2);
};

// ######################################################################################### //

class CTextEditBase;

class CTECursor
{
private:
	int x, y;    // 光眮E允镜淖丒
	int width;   // 光眮E目?
	int height;  // 光眮E母叨?
	HWND hwnd;   // 附属的窗口
	UINT_PTR nID;
	BOOL bInstalled;
	BOOL bShow;
	CTextEditBase *pEditBase;

	static std::map <UINT_PTR, CTECursor *> Cursors; // 定时器ID对应的光眮E丒

public:
	CTECursor();
	void Install(HWND hwnd, CTextEditBase *pEB, int x, int y, int w, int h);
	void Uninstall();
	void Move(int x, int y);
	void Move2(int x, int y);
	void SetHeight(int h);
	void SetWidth(int w);
	void OnTimer();
	void ReDraw();
	int GetCursorX() { return x; };
	int GetCursorY() { return y; };
	int GetHeight() { return height; };
	int GetWidth() { return width; };
	void Show(BOOL bShow) { this->bShow = bShow; }

	static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
};

// Commands ################################################################################ //

enum CmdType
{
	CmdType_Normal,
	CmdType_Group,
};

class CProtoCommand
{
public:
	CProtoCommand()
	{
	}

	virtual ~CProtoCommand()
	{
	};

	virtual bool CanMix()
	{
		return false;
	}

	virtual void Mix(CProtoCommand *pcmd)
	{
	}

	virtual void Execute() = 0;
	virtual void UnExecute() = 0;
	virtual enum CmdType GetType() = 0;
};

class CCommand : public CProtoCommand
{
public:
	enum CmdType GetType() { return CmdType_Normal; }
};

class CCommandGroup : public CCommand
{
private:
	std::vector <CCommand *> cmds;

public:
	~CCommandGroup()
	{
		for (size_t i = 0; i < cmds.size(); i++)
		{
			delete cmds[i];
		}
	}

	void AddCommand(CCommand *pCmd)
	{
		cmds.push_back(pCmd);
		pCmd->Execute();
	}

	void AddCommandToFirst(CCommand *pCmd)
	{
		cmds.insert(cmds.begin(), pCmd);
		pCmd->Execute();
	}

	void AddCommandNoExec(CCommand *pCmd)
	{
		cmds.push_back(pCmd);
	}

	virtual void Execute()
	{
		for (size_t i = 0; i < cmds.size(); i++)
		{
			cmds[i]->Execute();
		}
	}

	virtual void UnExecute()
	{
		for (int i = (int)cmds.size() - 1; i >= 0; i--)
		{
			cmds[i]->UnExecute();
		}
	}

	enum CmdType GetType() { return CmdType_Group; }
};

typedef std::stack <CCommand *> stackCommand;

// Command调用者
class CInvoker
{
private:
	stackCommand stackUndo;
	stackCommand stackRedo;
	bool         bModified;

public:
	bool IsModified()
	{
		return bModified;
	}

	CInvoker()
	{
		bModified = true;
	}

	virtual ~CInvoker()
	{
		ClearCommands();
	}

	void PushCommand(CCommand *p)
	{
		stackUndo.push(p);

		while(!stackRedo.empty())
		{
			delete stackRedo.top();
			stackRedo.pop();
		}

		bModified = true;
	}

	void PopUndoCommand()
	{
		if (stackUndo.size())
		{
			delete stackUndo.top();
			stackUndo.pop();
		}
	}

	void ExecCommand(CCommand *p)
	{
		stackUndo.push(p);

		while(!stackRedo.empty())
		{
			delete stackRedo.top();
			stackRedo.pop();
		}

		bModified = true;

		p->Execute();
	}

	void ExecAndGroup(CCommand *p)
	{
		if (stackUndo.size() == 0)
		{
			ExecCommand(p);
			return;
		}
		else
		{
			CCommand *pPrev = stackUndo.top();

			if (pPrev->GetType() == CmdType_Normal)
			{
				CCommandGroup *pNewGroup = new CCommandGroup;

				pNewGroup->AddCommand(p);
				pNewGroup->AddCommandNoExec(pPrev);

				stackUndo.pop();
				PushCommand(pNewGroup);
			}
			else if (pPrev->GetType() == CmdType_Group)
			{
				CCommandGroup *pGroup = (CCommandGroup *)pPrev;
				
				pGroup->AddCommandToFirst(p);
			}
			else
			{
				assert(false);
			}
		}
	}

	void ExecAndMerge(CCommand *p)
	{
		if (stackUndo.size() == 0)
		{
			ExecCommand(p);
			return;
		}
		else
		{
			CCommand *pPrev = stackUndo.top();

			if (pPrev->GetType() == CmdType_Normal)
			{
				CCommandGroup *pNewGroup = new CCommandGroup;

				pNewGroup->AddCommandNoExec(pPrev);
				pNewGroup->AddCommand(p);

				stackUndo.pop();
				PushCommand(pNewGroup);
			}
			else if (pPrev->GetType() == CmdType_Group)
			{
				CCommandGroup *pGroup = (CCommandGroup *)pPrev;

				pGroup->AddCommand(p);
			}
			else
			{
				assert(false);
			}
		}
	}

	void Undo()
	{
		if (CanUndo())
		{
			stackUndo.top()->UnExecute();
			stackRedo.push(stackUndo.top());
			stackUndo.pop();
		}
	}

	void Redo()
	{
		if (CanRedo())
		{
			stackRedo.top()->Execute();
			stackUndo.push(stackRedo.top());
			stackRedo.pop();
		}
	}

	bool CanRedo()
	{
		return stackRedo.size() > 0;
	}

	bool CanUndo()
	{
		return stackUndo.size() > 0;
	}

	void ClearCommands()
	{
		bModified = false;

		while(!stackUndo.empty())
		{
			delete stackUndo.top();
			stackUndo.pop();
		}

		while(!stackRedo.empty())
		{
			delete stackRedo.top();
			stackRedo.pop();
		}
	}
};

class CTECommand : public CCommand
{
protected:
	CTextEditBase *pTEB;
	POINT ptOldSelStart, ptOldSelEnd;
	POINT ptOldCursor;

public:
	CTECommand(CTextEditBase *base)
	{
		pTEB = base;
		BackupPos();
	}

	virtual ~CTECommand()
	{
	}

	void BackupPos();
	void RestorePos(BOOL bUpdate = TRUE);
};

class CTEInvoker : public CInvoker
{
public:
	void DoReplace(const wchar_t *str);
	void DoChangeFormat(CTextFormat &TextFormat, DWORD dwFlag = TF_ALL);
};

// Replace Cmd
DEFINE_TECOMMAND(Replace)
	private:
		oBinStream data;
		std::wstring strReplace;
		POINT ptEnd;

	public:
		COMMAND_CLSNAME(Replace)(CTextEditBase *base, const wchar_t *str);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// ChangeFormat Cmd
DEFINE_TECOMMAND(ChangeFmt)
	private:
		oBinStream data;
		DWORD dwFlag;
		CTextFormat fmt;
		POINT ptSNew, ptENew;

	public:
		COMMAND_CLSNAME(ChangeFmt)(CTextEditBase *base, DWORD dwFlag, CTextFormat fmt);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// Return Cmd
DEFINE_TECOMMAND(Return)
	private:
		int nLine;

	public:
		COMMAND_CLSNAME(Return)(CTextEditBase *base);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// ReverseReturn Cmd
DEFINE_TECOMMAND(ReverseReturn)
	private:
		int nRetCurX, nRetCurY;

	public:
		COMMAND_CLSNAME(ReverseReturn)(CTextEditBase *base);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// Paste Cmd
DEFINE_TECOMMAND(Paste)
	private:
		iBinStream data;
		POINT ptEnd;
		int nFormat;

	public:
		COMMAND_CLSNAME(Paste)(CTextEditBase *base);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// MarginLeft Cmd
DEFINE_TECOMMAND(MarginLeft)
	private:
		std::vector <int> OldMarginLeft;
		std::vector <int> OldMarginRight;
		std::vector <int> OldIndent;
		int nNewLeft;
		POINT ptSNew, ptENew;

	public:
		COMMAND_CLSNAME(MarginLeft)(CTextEditBase *base, int nMarginLeft);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// MarginRight Cmd
DEFINE_TECOMMAND(MarginRight)
	private:
		std::vector <int> OldMarginLeft;
		std::vector <int> OldMarginRight;
		std::vector <int> OldIndent;
		int nNewRight;
		POINT ptSNew, ptENew;

	public:
		COMMAND_CLSNAME(MarginRight)(CTextEditBase *base, int nMarginRight);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// Indent Cmd
DEFINE_TECOMMAND(Indent)
	private:
		std::vector <int> OldMarginLeft;
		std::vector <int> OldMarginRight;
		std::vector <int> OldIndent;
		int nNewIndent;
		POINT ptSNew, ptENew;

	public:
		COMMAND_CLSNAME(Indent)(CTextEditBase *base, int nIndent);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// Align
DEFINE_TECOMMAND(Align)
	private:
		std::vector <CParagraph::emAlignType> OldAlign;
		CParagraph::emAlignType NewAlign;
		POINT ptSNew, ptENew;

	public:
		COMMAND_CLSNAME(Align)(CTextEditBase *base, CParagraph::emAlignType Align);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// LineSpacing
DEFINE_TECOMMAND(LineSpacing)
	private:
		std::vector <int> OldSpacing;
		int NewSpacing;
		POINT ptSNew, ptENew;

	public:
		COMMAND_CLSNAME(LineSpacing)(CTextEditBase *base, int nSpacing);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()


// ######################################################################################### //

class CTextEditBase : public CTEInvoker // 派生CTEInvoker支持撤?E重做功能
{
public:
	Paragraphs m_ParagraphList;           // 段落列眮E

	CTextBlock *m_pCurTextBlock;          // 当前编辑的文本縼E
	int        m_nColumn;                 // 当前光眮E诘牧?
	int        m_nSubColumn;              // 当前光眮E谖谋究槟诘牧?
	CParagraph *m_pCurParagraph;          // 当前编辑的段聛E
	int        m_nRow;                    // 当前光眮E诘男?
	RECT       m_rtRegion;                // 编辑区域的位置和大小
	CTextFormat m_DefTextFormat;          // 当前输?E奈谋靖袷?
	CParaFormat m_DefParaFormat;          // 当前输?E?系亩温涓袷?
	CTextFormat m_DefTextFormat1;         // 默认的文本格式
	CParaFormat m_DefParaFormat1;         // 默认的段落格式
	POINT      ptSelStart, ptSelEnd;      // 选择区?E
	TMatrix    teMatrix;
	BOOL       bInsertMode;               // 插?EＪ?

	CTECursor  Cursor;                    // 光眮E
	BOOL       bEdit;                     // 是否正在编辑状态
	POINT      ptSelPos;
	int        nTabStop;

	BOOL       bUpdate;
	int        nLockUpdate;
	int        nLockDraw;
	int        nLockSendUpdate;

public:
	void Key_Return();
	void Key_Char(UINT nChar, BOOL bUnicode = FALSE);
	void Key_Backspace();
	void Key_Arrow(UINT nChar, UINT nFlags = 0, BOOL bSel = TRUE);
	void Key_Del();
	void Key_Ins();

private:
	void WriteAnsiCharToBinStream(std::string &out, std::wstring wstr);

public:
	void Return();
	void UpdateLine(CParagraph *pParagraph, BOOL bReDraw = TRUE);
	void UpdateLine(int nStartY, int nEndY, BOOL bReDraw = TRUE);
	void UpdateRange(POINT ptStart, POINT ptEnd, BOOL bReDraw = TRUE);
	void UpdateSelectRange(BOOL bReDraw = TRUE);
	int RowFromParagraphPtr(CParagraph *pParagraph);
	int GetFirstIndex();
	int GetLastIndex();
	void AdjustLocate(long &nColumn, long &nRow);
	int GetHeight();
	// 给出一个行区域，求出区域内的所有段聛E
	void GetParagraphs(int &nStart, int &nEnd);
	CParagraph *GetHeadLine(int nLine);

public:
	CTextEditBase();
	virtual ~CTextEditBase();
	void StartEdit();
	void EndEdit();
	void MoveCursor(int nColumn, int nRow, BOOL bNoReposCursor = FALSE); // 移动光眮E
	void MoveCursor(BOOL bNoReposCursor = FALSE);
	void MoveToLast();  // 移动光眮E阶竺丒
	void MoveToFirst(); // 移动光眮E阶懊丒
	void CursorToFirst();                   // 移动到所有文本惆脕E
	void CursorToLast();                    // 移动到所有文本煮脕E
	BOOL CursorAtFirst();
	BOOL CursorAtLast();
	void GetCharRect(int nColumn, int nRow, RECT &rt);
	void PixelToTextCrood(const POINT &point, int &nRow, int &nColumn);
	void GetSel(POINT &ptStart, POINT &ptEnd);  // 取得选中区?E
	void SetSel(POINT ptStart, POINT ptEnd, BOOL bReDraw = TRUE); // 设置选中区?E
	BOOL InSel(int nColumn, int nRow);          // 判断一个文本坐眮E欠裨谘≡袂蚰?
	void SelectAll();                           // 选择全部
	void SelectWord(int nColumn, int nRow);             // 选择一个词
	BOOL SelectIsEmpty();                       // 选择是否为空
	void ClearSelect(BOOL bReDraw = TRUE);      // 清除选择区?E
	DWORD GetFormat(CTextFormat &TextFormat); // 获取当前光眮E恢玫奈谋靖袷?
	DWORD GetParaFormat(CParaFormat &ParaFormat);
	void LockUpdate()   { nLockUpdate++; }
	void UnlockUpdate() { nLockUpdate--; }
	void LockDraw()     { nLockDraw++;   }
	void UnlockDraw()   { nLockDraw--;   }
	void LockSendUpdate() { nLockSendUpdate++; }
	void UnlockSendUpdate()
	{
		nLockSendUpdate--; 

		if (nLockSendUpdate == 0)
		{
			int x1, y1, x2, y2;
			OnReposCursor(Cursor.GetCursorX(), Cursor.GetCursorY(), x1, y1, x2, y2, TRUE);
		}
	}
	std::wstring GetText();
	BOOL IsEmpty();
	int InsertRawData(char *pData, size_t size);
	int InsertText(const char *pszText);
	void CopyToStream(oBinStream &out);
	void CopyTextToAnsiString(std::string &out);
	void SelToCursor(BOOL bReDraw = TRUE);
	BOOL CanPaste();
	virtual void OnReposCursor(int x, int y, int &x1, int &y1, int &x2, int &y2, BOOL bReposView) = 0;
	//void MoveFBtoLB(CParagraph *para1, CParagraph *para2);

public: // 原始的编辑脕E丒
	void Replace(const wchar_t *str, BOOL bReDraw = TRUE); // 替换选择区域内的文本
	void ChangeFormat(CTextFormat &TextFormat, DWORD dwFlag = TF_ALL); // 竵E难≡袂虻奈谋靖袷?
	void SetAlignType(CParagraph::emAlignType type); // 设置当前光眮E恢玫亩温涞亩云丒绞?
	void SetLineSpacing(int nSpacing);             // 设置行緛E
	void SetFontFace(const char *pFontFace);
	void SetFontSize(int nSize);
	void SetFontColor(COLORREF crFont);
	void SetFontBold(BOOL bFlag);
	void SetFontItalic(BOOL bFlag);
	void SetCharSpacing(int nCharSpacing);
	void SetCharLocate(CTextFormat::emCharLocate Locate);
	void SetLink(std::string strLink);
	void SetTarget(std::string strTarget);
	void SetMarginLeft(int nLeft);
	void SetMarginRight(int nRight);
	void SetIndent(int nIndent);
	void ChangeRegion(RECT rt);
	void ChangeRegion(RECT rt, RECT oldrt);
	void Copy();
	void Paste();
	void Delete();
	void Cut();

protected:
	void TE_KeyDown(UINT nChar, UINT nRepCount, UINT nFlags);
	void TE_Char(UINT nChar, UINT nRepCount, UINT nFlags);
	BOOL TE_LButtonDown(UINT nFlags, const POINT &point);
	void TE_BeginDrag(UINT nFlags, const POINT &point);
	void TE_DragOver(UINT nFlags, const POINT &point);
	void TE_DragDrop(UINT nFlags, const POINT &point);
	void TE_CancelDrag(UINT nFlags, const POINT & point);
	void TE_LButtonDbClick(UINT nFlags, const POINT &point);
	int GetAllMinWidth();
	BOOL CanInsertChar(wchar_t c);

	virtual HWND GetHwnd() = 0;
	virtual int GetCharWidth(wchar_t c1, wchar_t c2, CTextFormat &TextFormat) = 0;
	virtual void GetFontHeight(CTextFormat &TextFormat, int &nHeight, int &nAscent) = 0;
	virtual void DrawChar(int x, int y, int w, int h, int ascent, int baseline, int lh, wchar_t c, CTextFormat &TextFormat, POINT *pLocate, BOOL bReDraw) = 0;
	virtual void FillSolidRect(int x, int y, int w, int h, COLORREF color, BOOL bReDraw) = 0;
	virtual void UpdateAll() = 0;
	virtual void ReDraw(gld_rect *pRect = NULL) = 0;
	virtual void RestoreBackground(RECT &rt) = 0;
	virtual void OnSelChanged() = 0;
	virtual void OnFormatChange() = 0;
	virtual void OnBeforeSelChange() = 0;

	friend class CParagraph;
	friend class CTextBlock;
	friend class COMMAND_CLSNAME(Replace);
	friend class COMMAND_CLSNAME(ChangeFmt);
	friend class COMMAND_CLSNAME(Return);
	friend class COMMAND_CLSNAME(Paste);
	friend class COMMAND_CLSNAME(MarginLeft);
	friend class COMMAND_CLSNAME(MarginRight);
	friend class COMMAND_CLSNAME(Indent);
	friend class COMMAND_CLSNAME(Align);
	friend class COMMAND_CLSNAME(LineSpacing);
	friend class COMMAND_CLSNAME(ChangeRegion);
	friend class COMMAND_CLSNAME(ReverseReturn);
};

// ######################################################################################### //

class gldText2 : public gldText
{
public:
	RECT rtRegion;
	Paragraphs m_ParagraphList;
	BOOL bNoConvert;
	STRING_MAP rpl_fontmap;
	
private:
	class CConvertFromHTML
	{
	public:
		int nBold;
		int nItalic;
		int nUnderLine;
		DEFINE_HTMLSTYLE(FONT_TAG, Font);
		DEFINE_HTMLSTYLE(ANCHOR_TAG, Anchor);

	public:
		CConvertFromHTML(gldText2 *pGldText2);
	};

	class CConvertToHTML
	{
	public:
		CConvertToHTML(gldText2 *pGldText2);
	};

	void ConvertFromStatic();
	void ConvertFromSimpleDynamic();
	void ConvertFromHTML();

	std::string ReplaceFontFace(const std::string fontface);

public:
	gldText2();
	virtual ~gldText2();
	void Clear();
	void ClearAll();
	virtual void WriteToBinStream(oBinStream &os);
	virtual void ReadFromBinStream(iBinStream &is);
	virtual gldObj *Clone();
	void BuildEditInfo();
	void BuildRecord();
	void SubstituteFonts(STRING_MAP &fontmap);
	void GetUsedFonts(STRING_SET &fontList);
};

enum emMultilineType
{
	Text_Singleline,
	Text_Multiline,
	Text_MultilineNoWrap,
	Text_Password,
	Text_Unknown,
};

typedef struct
{
	gldText2_TEXTTYPE TextType;
	BOOL              bBorder;
	BOOL              bCanSelect;
	BOOL              bCanEdit;
	std::string       strBindVar;
	int               nMaxLength;
	enum emMultilineType MultilineType;
}TEXT_PROPERTY;

template <class _IClass>
class TTextToolExTmpl : public TToolTmpl<_IClass>, public CTextEditBase, public CObserver
{
protected:
	// 显示用数据
	POINT    m_ptStart;
	POINT    m_ptCurrent;
	HPEN     m_hPen, m_hOldPen;
	TDraftDC m_dc;
	gldText2  *pgldText;
	BOOL     bResize;
	RECT     rtOldRegion;
	POINT    ptOldCur, ptOldSelStart, ptOldSelEnd;

	gld_shape	m_TShape;
	gld_layer	m_TLayer;
	BOOL		m_bIme; // 正在用IME输入
	BOOL		m_bImeSelChange;

	CTraitInstance::GInstance *pEditInstance;
	
	BOOL     IsResize() {return bResize;}

	TEXT_PROPERTY TextProperty, TextProperty1;

	////////////////////////////////////////////////////////
	typedef TToolTmpl<_IClass>	Base_type;

public:
	BOOL     IsActive() {return bEdit;}

	TTextToolExTmpl(void) : Base_type()
	{
		bEdit = FALSE;
		bResize = FALSE;

		ReadFormatFromReg();

		nLockSendUpdate = 0;
		m_bIme = FALSE;
		m_bImeSelChange = TRUE;

		TNotifyTransceiver::Attach(SEN_REDRAW, this, OnNotify);
		CSubjectManager::Instance()->GetSubject("Select")->Attach(this);
	}

	TTextToolExTmpl(_IClass *pIClass) :Base_type(pIClass)
	{
		bEdit = FALSE;
		bResize = FALSE;

		m_bIme = FALSE;
		m_bImeSelChange = TRUE;

		ReadFormatFromReg();

		nLockSendUpdate = 0;

		TNotifyTransceiver::Attach(SEN_REDRAW, this, OnNotify);
		CSubjectManager::Instance()->GetSubject("Select")->Attach(this);
	}

	~TTextToolExTmpl(void)
	{
		WriteFormatToReg();

		TNotifyTransceiver::Detach(SEN_REDRAW, this);
		CSubjectManager::Instance()->GetSubject("Select")->Detach(this);
	}

	static void OnNotify(void *sender, void *receiver, int nNotifyID, LPARAM lParam)
	{
		TTextToolExTmpl<_IClass> *pEditBase = (TTextToolExTmpl<_IClass> *)receiver;

		switch(nNotifyID)
		{
		case SEN_REDRAW:
			if (pEditBase->IsActive())
			{
				pEditBase->ReDraw();
			}
			break;
		}
	}

	virtual void Enter();
	virtual void Leave();

private:
	void SetCompositionWindow();

protected:
	virtual void OnSetFocus(HWND hOldWnd);
	virtual void OnKillFocus(HWND hNewWnd);
	virtual void OnLButtonDbClick(UINT nFlags, const POINT &point);
	virtual void OnLButtonDown(UINT nFlags, const POINT &point);
	virtual void OnBeginDrag(UINT nFlags, const POINT &point);
	virtual void OnDragOver(UINT nFlags, const POINT &point);
	virtual void OnDragDrop(UINT nFlags, const POINT &point);
	virtual void OnCancelDrag(UINT nFlags, const POINT & point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCount, UINT nFlags);
	virtual void OnChar(UINT nChar, UINT nRepCount, UINT nFlags);
	virtual void OnDraw(HDC hdc);
	virtual bool OnSetCursor(UINT nFlags, const POINT &point);
	virtual void OnSelChanged();
	virtual void OnBeforeSelChange();
	virtual LRESULT OnImeComposition(WPARAM wParam, LPARAM lParam);
	virtual void OnImeChar(WPARAM wParam, LPARAM lParam);
	virtual void OnImeNotify(WPARAM wParam, LPARAM lParam);

	void PrepareDC();
	void UnprepareDC();
	void DrawEffect();
	void MyStartEdit();
	void MyEndEdit();
	void MyCancelEdit();
	
	void HideTText();
	void ShowTText();

	void DrawBorder(HDC hdc);
	void DrawSelect(HDC hdc);
	void DrawLink(HDC hdc);
	CTextBlock *FirstTB();
	CParagraph *FirstLine();
	void AdjustRegion();
	emMultilineType GetMultilineType(gldText2 *pText);
	void ChangeMultilineType(gldText2 *pText, emMultilineType mt);

public:
	void SetActive(BOOL act = FALSE)
	{
		bEdit = act;
	}

	void ConvertToGldText(gldText2 &text);
	void ConvertFromGldText(gldText2 &text/*, BOOL bImport = FALSE*/);
	void SubstituteFonts(STRING_MAP &fontmap, gldText2 &text);
	void StartEditText(CTraitInstance::GInstance *pCh, const POINT *pt = NULL);
	TTCAPS GetTextToolCaps();
	void ChangeTextType(gldText2_TEXTTYPE type);
	void SetBorder(BOOL b);
	void SetCanSelect(BOOL s);
	void SetBindVar(const char *pszVariable);
	void SetCanEdit(BOOL bCanEdit);
	void SetMaxLength(unsigned short nLength);
	void SetMultilineType(emMultilineType mt);

	DWORD GetTextProperty(TEXT_PROPERTY &prop);
	void WriteFormatToReg();
	void ReadFormatFromReg();
	void RepaintDesignWnd();
	BOOL CheckBound();

	virtual int Capabilities()
	{
		return TC_ADDSHAPE | TC_EDITTEXT;
	}

	virtual HWND GetHwnd();
	virtual int GetCharWidth(wchar_t c1, wchar_t c2, CTextFormat &TextFormat);
	virtual void GetFontHeight(CTextFormat &TextFormat, int &nHeight, int &nAscent);
	virtual void DrawChar(int x, int y, int w, int h, int ascent, int baseline, int lh, wchar_t c, CTextFormat &TextFormat, POINT *pLocate = NULL, BOOL bReDraw = TRUE);
	virtual void FillSolidRect(int x, int y, int w, int h, COLORREF color, BOOL bReDraw = TRUE);
	virtual void UpdateAll();
	virtual void ReDraw(gld_rect *pRect = NULL);
	virtual void RestoreBackground(RECT &rt);
	virtual void OnFormatChange();
	virtual void OnReposCursor(int x, int y, int &x1, int &y1, int &x2, int &y2, BOOL bReposView);
	virtual void Update(void *pData)
	{
		if (nLockSendUpdate == 0)
		{
			CSubjectManager::Instance()->GetSubject("TextSelChange")->Notify((void *)0);
		}
	}

public:
	DWORD GetFormat(CTextFormat &TextFormat);
	DWORD GetParaFormat(CParaFormat &ParaFormat);

	// 工具的格式设置脕E丒
	// 如果当前在编辑状态，则调用EditBase的相同脕E丒
	// 否则，设置选择的文本格式
	// 如果选择为空，则这是默认格式
	void SetAlignType(CParagraph::emAlignType type);
	void SetLineSpacing(int nSpacing);
	void SetFontFace(const char *pFontFace);
	void SetFontSize(int nSize);
	void SetFontColor(COLORREF crFont);
	void SetFontBold(BOOL bFlag);
	void SetFontItalic(BOOL bFlag);
	void SetCharSpacing(int nCharSpacing);
	void SetCharLocate(CTextFormat::emCharLocate Locate);
	void SetLink(std::string strLink);
	void SetTarget(std::string strTarget);
	void SetMarginLeft(int nLeft);
	void SetMarginRight(int nRight);
	void SetIndent(int nIndent);

	friend class COMMAND_CLSNAME(ChangeTextType);
	friend class COMMAND_CLSNAME(SetBorder);
	friend class COMMAND_CLSNAME(CanSelect);
	friend class COMMAND_CLSNAME(SetBindVar);
	friend class COMMAND_CLSNAME(CanEdit);
	friend class COMMAND_CLSNAME(SetMaxLength);
	friend class COMMAND_CLSNAME(SetMultilineType);
};

class ITextTool;

// 竵E奈谋纠嘈偷腃ommand，只能是TextTool使用
DEFINE_TECOMMAND(ChangeTextType)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		oBinStream data;
		gldText2_TEXTTYPE newtype, oldtype;

	public:
		COMMAND_CLSNAME(ChangeTextType)(CTextEditBase *base, gldText2_TEXTTYPE type);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// 竵E氖欠窨裳≡袷粜缘腃ommand，只能是TextTool使用
DEFINE_TECOMMAND(CanSelect)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		BOOL oldcansel, newcansel;

	public:
		COMMAND_CLSNAME(CanSelect)(CTextEditBase *base, BOOL cansel);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// 竵E奈谋臼欠裼斜呖虻腃ommand，只能是TextTool使用
DEFINE_TECOMMAND(SetBorder)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		BOOL oldborder, newborder;

	public:
		COMMAND_CLSNAME(SetBorder)(CTextEditBase *base, BOOL border);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// 竵E奈谋臼欠癜蠖ū淞康腃ommand，只能是TextTool使用
DEFINE_TECOMMAND(SetBindVar)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		std::string strOldBind, strNewBind;

	public:
		COMMAND_CLSNAME(SetBindVar)(CTextEditBase *base, const char *newBind);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// 竵E奈谋臼欠窨杀嗉?腃ommand，只能是TextTool使用
DEFINE_TECOMMAND(CanEdit)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		BOOL oldSel;
		BOOL oldEdit, newEdit;

	public:
		COMMAND_CLSNAME(CanEdit)(CTextEditBase *base, BOOL edit);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// 设置动态文本畜长度的Command，只能是TextTool使用
DEFINE_TECOMMAND(SetMaxLength)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		unsigned short nOldLength, nNewLength;

	public:
		COMMAND_CLSNAME(SetMaxLength)(CTextEditBase *base, unsigned short nLength);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

// 设置动态文本畜长度的Command，只能是TextTool使用
DEFINE_TECOMMAND(SetMultilineType)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
		emMultilineType OldType, NewType;

	public:
		COMMAND_CLSNAME(SetMultilineType)(CTextEditBase *base, emMultilineType mt);
		virtual void Execute();
		virtual void UnExecute();
END_TECOMMAND()

DEFINE_TECOMMAND(ChangeRegion)
	private:
		TTextToolExTmpl<ITextTool> *pEditTool;
	public:
		RECT rtOld, rtNew;

	public:
		COMMAND_CLSNAME(ChangeRegion)(CTextEditBase *base, RECT rt);
		virtual void Execute();
		virtual void UnExecute();
		void ChangeRegion(RECT rt);
END_TECOMMAND()

template <class _IClass>
void TTextToolExTmpl<_IClass>::StartEditText(CTraitInstance::GInstance *pCh, const POINT *pt)
{
	if (IsActive())
	{
		MyEndEdit();
	}

	if (pCh)
	{
		gldObj *pObj = CTraitInstance(pCh);
		if (pObj->GetGObjType() == gobjText)
		{
			pEditInstance = pCh;

			gldText2 *pText = (gldText2 *)pObj;
			ConvertFromGldText(*pText);
			pgldText = pText;

			teMatrix.m_e11 = pCh->m_matrix.m_e11;
			teMatrix.m_e12 = pCh->m_matrix.m_e12;
			teMatrix.m_e21 = pCh->m_matrix.m_e21;
			teMatrix.m_e22 = pCh->m_matrix.m_e22;
			teMatrix.m_dx = pCh->m_matrix.m_x;
			teMatrix.m_dy = pCh->m_matrix.m_y;

			m_TShape = CTraitShape(pCh);
			m_TLayer = CTraitLayer(pCh);
			ASSERT(m_TShape.validate() && m_TLayer.validate());

			HideTText();

			MyStartEdit();
			if (pt != NULL)
			{   
				TE_LButtonDown(0, *pt);
			}
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnLButtonDown(UINT nFlags, const POINT &point)
{
	int x = point.x;
	int y = point.y;

	_M_matrix.Invert().Transform(x, y);

	x  = TWIPS_TO_PIXEL(x);
	y  = TWIPS_TO_PIXEL(y);

	if (!bEdit)
	{
		bResize = FALSE;

		gld_shape gs = m_pIClass->PtOnText(point.x, point.y);
		if (gs.validate())
		{
			CTraitInstance::GInstance *pCh = CTraitInstance(gs);
			StartEditText(pCh, &point);
		}
		else if (m_pIClass->CanAddText())
		{
			pgldText = NULL;
			Track(x, y);
		}
	}
	else
	{
		// 编辑状态
		POINT pt = point;
		int cx = m_rtRegion.right, cy = m_rtRegion.bottom;
		int px = pt.x, py = pt.y;

		teMatrix.Transform(cx, cy);
		_M_matrix.Invert().Transform(cx, cy);
		_M_matrix.Invert().Transform(px, py);

		if (px >= cx - nHandleSize / 2 &&
			py >= cy - nHandleSize / 2 &&
			px <= cx + nHandleSize / 2 &&
			py <= cy + nHandleSize / 2)
		{
			bResize = TRUE;
			Track(x, y);
		}
		else
		{
			if (!TE_LButtonDown(nFlags, point))
			{
				MyEndEdit();
			}
			else
			{
				Track(x, y);
			}
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnBeginDrag(UINT nFlags, const POINT &point)
{
	if (!bEdit)
	{
		teMatrix = TMatrix();

		m_ptStart = point;
		m_ptCurrent = point;

		DrawEffect();
	}
	else
	{
		if (IsResize())
		{
			rtOldRegion = m_rtRegion;
			ptOldCur.x = m_nColumn;
			ptOldCur.y = m_nRow;
			ptOldSelStart = ptSelStart;
			ptOldSelEnd = ptSelEnd;

			m_ptStart.x = m_rtRegion.left;
			m_ptStart.y = m_rtRegion.top;
			m_ptCurrent.x = m_rtRegion.right;
			m_ptCurrent.y = m_rtRegion.bottom;
			DrawEffect();
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnDragOver(UINT nFlags, const POINT &point)
{
	if (!bEdit)
	{
		DrawEffect();
		int nFontHeight, nAscent;

		GetFontHeight(m_DefTextFormat1, nFontHeight, nAscent);
		m_ptCurrent.x = max(point.x, m_ptStart.x + BORDER_SIZE * 2 + 
			(m_DefParaFormat1.GetMarginRight() + abs(m_DefParaFormat1.GetMarginLeft() + 
			m_DefParaFormat1.GetIndent())));
		m_ptCurrent.y = max(m_ptStart.y + nFontHeight + BORDER_HEIGHT * 2, point.y);

		DrawEffect();
	}
	else
	{
		if (IsResize())
		{
			POINT newcur, pt = point;
			teMatrix.Invert().Transform(pt.x, pt.y);

			int nTotalHeight = 0, nMaxWidth = 0;

			Paragraphs::iterator it = m_ParagraphList.begin();
			while(it != m_ParagraphList.end())
			{
				int nMaxHeight = 0;

				nMaxWidth = max(nMaxWidth, (*it)->GetMarginRight() + 
					abs((*it)->GetMarginLeft() + (*it)->GetIndent()));

				do
				{
					nMaxHeight = max(nMaxHeight, (*it)->GetHeight());

					it++;

					if (it == m_ParagraphList.end())
					{
						break;
					}
				}while(!(*it)->IsStiffRet());

				nTotalHeight += nMaxHeight;
			}

			newcur.x = max(m_ptStart.x + nMaxWidth + GetAllMinWidth() + BORDER_SIZE * 2, pt.x);
			m_rtRegion.right = m_rtRegion.left + (newcur.x - m_ptStart.x);

			for (size_t i = 0; i < m_ParagraphList.size(); i++)
			{
				CParagraph *pParagraph = m_ParagraphList[i];

				int nProcLine = 0, nUpdate = 0;
				pParagraph->Arrange(&nProcLine, nUpdate);
			}

			Paragraphs::iterator itp = m_ParagraphList.begin();
			int nHeight = 0;

			while(itp != m_ParagraphList.end())
			{
				nHeight += (*itp)->GetHeight();
				itp++;
			}

			newcur.y = max(m_ptStart.y + nHeight + BORDER_HEIGHT * 2, pt.y);

			if (memcmp(&m_ptCurrent, &newcur, sizeof(POINT)) != 0)
			{
				DrawEffect();
				m_ptCurrent = newcur;
				DrawEffect();
			}
		}
		else
		{
			TE_DragOver(nFlags, point);
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::AdjustRegion()
{
	RECT rtNew;
	TMatrix mat;

/*	rtNew.left = -(m_rtRegion.right - m_rtRegion.left) / 2;
	rtNew.top = - (m_rtRegion.bottom - m_rtRegion.top) / 2;
	rtNew.right = (m_rtRegion.right - m_rtRegion.left) / 2;
	rtNew.bottom = (m_rtRegion.bottom - m_rtRegion.top) / 2;
	*/

	mat.m_dx = (m_rtRegion.left + m_rtRegion.right) / 2;
	mat.m_dy = (m_rtRegion.top + m_rtRegion.bottom) / 2;

	rtNew = m_rtRegion;
	OffsetRect(&rtNew, -mat.m_dx, -mat.m_dy);

	m_rtRegion = rtNew;
	teMatrix = teMatrix * mat;
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnDragDrop(UINT nFlags, const POINT &point)
{
	if (!bEdit)
	{
		DrawEffect();

		m_rtRegion.left = m_ptStart.x;
		m_rtRegion.top = m_ptStart.y;
		m_rtRegion.right = m_ptCurrent.x;
		m_rtRegion.bottom = m_ptCurrent.y;

		if (m_rtRegion.left > m_rtRegion.right)
		{
			swap(m_rtRegion.left, m_rtRegion.right);
		}

		if (m_rtRegion.top > m_rtRegion.bottom)
		{
			swap(m_rtRegion.top, m_rtRegion.bottom);
		}

		MyStartEdit();
	}
	else
	{
		if (IsResize())
		{
			DrawEffect();

			RECT rt;
			rt.left = m_ptStart.x;
			rt.top = m_ptStart.y;
			rt.right = m_ptCurrent.x;
			rt.bottom = m_ptCurrent.y;
			ChangeRegion(rt, rtOldRegion);

			m_pIClass->Redraw();
			m_pIClass->Repaint();

			bResize = FALSE;
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnCancelDrag(UINT nFlags, const POINT & point)
{
	DrawEffect();

	if (bResize)
	{
		m_rtRegion = rtOldRegion;

		for (size_t i = 0; i < m_ParagraphList.size(); i++)
		{
			CParagraph *pParagraph = m_ParagraphList[i];

			int nProcLine = 0, nUpdate = 0;
			pParagraph->Arrange(&nProcLine, nUpdate);
		}

		MoveCursor(ptOldCur.x, ptOldCur.y);
		ptSelStart = ptOldSelStart;
		ptSelEnd = ptOldSelEnd;

		bResize = FALSE;
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnKeyDown(UINT nChar, UINT nRepCount, UINT nFlags)
{
	TE_KeyDown(nChar, nRepCount, nFlags);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnChar(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (IsActive())
	{
		if (nChar == VK_ESCAPE)
		{
			MyCancelEdit();
		}
		else
		{
			TE_Char(nChar, nRepCount, nFlags);
			SetCompositionWindow();
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnImeChar(WPARAM wParam, LPARAM lParam)
{
	if (IsActive())
	{
//		TE_Char(nChar, nRepCount, nFlags);
		SetCompositionWindow();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::PrepareDC()
{
	m_dc.Matrix(teMatrix * _M_matrix.Invert());
	m_dc.Hdc(::GetDC(_M_hwnd));

	m_hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	m_hOldPen = m_dc.SelectPen(m_hPen);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::UnprepareDC()
{
	m_dc.SelectPen(m_hOldPen);
	DeleteObject(m_hPen);
	::ReleaseDC(_M_hwnd, m_dc.Hdc());
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::DrawEffect()
{
	PrepareDC();

	int nOldROP = m_dc.SetROP2(R2_NOTXORPEN);
	m_dc.MoveTo(m_ptStart.x, m_ptStart.y, TDraftDC::_trans_tag());
	m_dc.LineTo(m_ptCurrent.x, m_ptStart.y, TDraftDC::_trans_tag());
	m_dc.LineTo(m_ptCurrent.x, m_ptCurrent.y, TDraftDC::_trans_tag());
	m_dc.LineTo(m_ptStart.x, m_ptCurrent.y, TDraftDC::_trans_tag());
	m_dc.LineTo(m_ptStart.x, m_ptStart.y, TDraftDC::_trans_tag());

	m_dc.SetROP2(nOldROP);

	UnprepareDC();
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::HideTText()
{	
	CGuardDrawOnce	xDraw;

	m_TLayer.remove(m_TShape);

	gld_shape_sel sel = m_pIClass->CurSel();
	sel.unselect(m_TShape);	
 	m_TShape.release(true);
	if (sel.count() != m_pIClass->CurSel().count())
	{
		m_pIClass->DoSelect(sel);
	}
	else
	{
		m_pIClass->Repaint();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ShowTText()
{
	CTransAdaptor::RebuildCurrentScene();
	m_pIClass->Redraw(gld_rect(m_rtRegion.left, m_rtRegion.top, m_rtRegion.right, m_rtRegion.bottom));
	m_pIClass->Repaint();
	
	m_TShape = gld_shape();
	m_TLayer = gld_layer();

	gld_shape_sel sel;
	ASSERT(pEditInstance != NULL);
	gld_shape shape = CTraitShape(pEditInstance);
	ASSERT(shape.validate());
	sel.select(shape);
	m_pIClass->DoSelect(sel);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::RepaintDesignWnd()
{
	m_pIClass->Repaint();
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::MyStartEdit()
{
	m_pIClass->DoSelect(gld_shape_sel());

	// 初始化GDI环境
	bEdit = TRUE;

	if (pgldText == NULL)
	{
		m_DefTextFormat = m_DefTextFormat1;
		m_DefParaFormat = m_DefParaFormat1;

		TextProperty = TextProperty1;
	}
	else
	{
		m_DefTextFormat = (*(pgldText->m_ParagraphList[0]->m_TextBlockList.begin()))->TextFormat;
		pgldText->m_ParagraphList[0]->GetParaFormat(m_DefParaFormat);
	}

	StartEdit();

//	CSubjectManager::Instance()->GetSubject("TextInplaceEdit")->Notify(pgldText != NULL ? pgldText : NULL);
	CSubjectManager::Instance()->GetSubject("TextInplaceEdit")->Notify((void *)1);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::MyEndEdit()
{
	OnBeforeSelChange();

	bEdit = FALSE;

	LockSendUpdate();

	m_pIClass->LeaveEdit();

	// 铁赜文本
	if (pgldText == NULL)
	{
		if ((TextProperty.TextType != TextType_Static) || !IsEmpty())
		{
			AdjustRegion();

			gldText2 *ptext = new gldText2;
			ptext->rtRegion = m_rtRegion;
			ConvertToGldText(*ptext);

			gldMatrix text_mat;
			TMatrix Mat = teMatrix;

			text_mat.m_e11 = Mat.m_e11;
			text_mat.m_e12 = Mat.m_e12;
			text_mat.m_e21 = Mat.m_e21;
			text_mat.m_e22 = Mat.m_e22;
			text_mat.m_x   = Mat.m_dx;
			text_mat.m_y   = Mat.m_dy;

			ptext->m_matrix = text_mat;

			m_pIClass->AddText(ptext);
		}
	}
	else
	{
		if (IsModified())
		{
			gldText2 *pNewText = new gldText2;
			oBinStream out;
			iBinStream in;

			pgldText->WriteToBinStream(out);
			U8 *p = new U8[out.Size()];

			out.WriteToMemory(p);
			in.ReadFromMemory(out.Size(), p);
			pNewText->ReadFromBinStream(in);

			delete p;

			ConvertToGldText(*pNewText);
			m_pIClass->ModifyText(pgldText, pNewText);
		}
		else
		{
			if (pEditInstance != NULL)
			{
				ShowTText();
			}
		}
	}

	UnlockSendUpdate();

	pgldText = NULL;

	CSubjectManager::Instance()->GetSubject("TextInplaceEdit")->Notify((void *)0);

	EndEdit();

	m_pIClass->Redraw();
	m_pIClass->Repaint();	
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::MyCancelEdit()
{
	bEdit = FALSE;

	CSubjectManager::Instance()->GetSubject("TextInplaceEdit")->Notify((void *)0);

	EndEdit();

	if (pgldText != NULL)
	{
		ShowTText();
	}
	
	m_pIClass->Redraw();
	m_pIClass->Repaint();
}

template <class _IClass>
int TTextToolExTmpl<_IClass>::GetCharWidth(wchar_t c1, wchar_t c2, CTextFormat &TextFormat)
{
	//MY_USES_CONVERSION;

	gldFont *font = gldDataKeeper::Instance()->m_objLib->GetFont(TextFormat.GetFontFace().c_str(), 
		BOOL2bool(TextFormat.GetBold()), BOOL2bool(TextFormat.GetItalic()));
	double ratio = TextFormat.GetFontSize() / (double)STD_FONT_HEIGHT;

	wchar_t str[] = {c1, 0};
	char *ansi_str = MY_W2A(str);
	int len = (int)strlen(ansi_str);
	SIZE sz;

	if (*ansi_str != '\t')
	{
		GetTextExtentPoint(font->GetDefaultHDC(), ansi_str, len, &sz);
	}
	else
	{
		std::string strtab;
		strtab.resize(nTabStop + 1);

		for (int i = 0; i < nTabStop; i++)
		{
			strtab += " ";
		}

		GetTextExtentPoint(font->GetDefaultHDC(), strtab.c_str(), nTabStop, &sz);
	}

	sz.cx = int(sz.cx * ratio);

	// kerning
	int nKerning = 0;

	if (c2 != 0)
	{
		nKerning = int((double)font->GetKerning(c1, c2) * ratio);
	}

	if (TextFormat.GetLocate() != CTextFormat::CL_NONE)
	{
		sz.cx /= 2;
		nKerning /= 2;
	}

	return sz.cx + nKerning;
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::GetFontHeight(CTextFormat &TextFormat, int &nHeight, int &nAscent)
{
	gldFont *font = gldDataKeeper::Instance()->m_objLib->GetFont(TextFormat.GetFontFace().c_str(), 
		BOOL2bool(TextFormat.GetBold()), BOOL2bool(TextFormat.GetItalic()));
    double ratio = TextFormat.GetFontSize() / 1024.0;

	nHeight = int(font->GetDefaultHeight() * ratio);
	nAscent = int(font->GetDefaultAscent() * ratio);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::DrawChar(int x, int y, int w, int h, int ascent, int baseline, int lh, wchar_t c, CTextFormat &TextFormat, POINT *pLocate, BOOL bReDraw/* = TRUE*/)
{
	//MY_USES_CONVERSION;

	gldFont *font = gldDataKeeper::Instance()->m_objLib->GetFont(TextFormat.GetFontFace().c_str(), 
		BOOL2bool(TextFormat.GetBold()), BOOL2bool(TextFormat.GetItalic()));
	gldText text;
	COLORREF fontcolor = MakeDiffColor(TextFormat.GetFontColor(), RGB(255, 255, 255));
	gldColor color(GetRValue(fontcolor), 
		GetGValue(fontcolor), 
		GetBValue(fontcolor));

	int nDisplayX = m_rtRegion.left + x;
	int nDisplayY = m_rtRegion.top + y;
/*
	if (pLocate != NULL)
	{
		if (InSel(pLocate->x, pLocate->y))
		{
			// 在选择范围内，绘制选择背景
//			FillSolidRect(x, y, w, lh, GetSysColor(COLOR_HIGHLIGHT), FALSE);

			// 设置选择文本颜色
			COLORREF hlText = GetSysColor(COLOR_HIGHLIGHTTEXT);
			color = gldColor(GetRValue(hlText), 
				GetGValue(hlText), 
				GetBValue(hlText));
		}
	}
*/
	wchar_t wc[] = {c, 0};
	char *ansi = MY_W2A(wc);
	int nFontSize = TextFormat.GetFontSize();

	if (TextFormat.GetLocate() != CTextFormat::CL_NONE)
	{
		nFontSize /= 2;
	}

	if (strlen(ansi) == 2)
	{
		WORD c1 = (unsigned char)ansi[0], c2 = (unsigned char)ansi[1];
		c1 <<= 8;
		c1 |= c2;
		text.AppendChar(c1, font, color, nDisplayX, (baseline - ascent) + (nDisplayY + ascent) -
			(TextFormat.GetLocate() == CTextFormat::CL_SUPERSCRIPT ? TextFormat.GetFontSize() / 2 : 0), nFontSize, 0);
	}
	else
	{
		if (!(*ansi == '\t' || *ansi == ' '))
		{
			text.AppendChar(ansi[0], font, color, nDisplayX, (baseline - ascent) + (nDisplayY + ascent) -
				(TextFormat.GetLocate() == CTextFormat::CL_SUPERSCRIPT ? TextFormat.GetFontSize() / 2 : 0), nFontSize, 0);
		}
	}

	static TShapeDraw ShapeDraw;
	TGraphicsDevice *pGD = m_pIClass->GetGraphDev();
	pGD->SetMatrix(teMatrix * _M_matrix.Invert());

	// 绘制文本
	ShapeDraw.Draw(&text, pGD);

	// 画链接下划线
	if (TextFormat.GetLink().length())
	{
		TLineStyleTable LineStyle;

		pGD->SetFillStyleTable(NULL);
		pGD->SetLineStyleTable(NULL);
		
		int ls = LineStyle.Add(new TLineStyle(1, TColor(color.red, color.green, color.green, 255))); // 设置下划线颜色
		pGD->SetLineStyle(ls);
		pGD->MoveTo(nDisplayX, nDisplayY + lh - 20);
		pGD->LineTo(nDisplayX + w, nDisplayY + lh - 20);
		pGD->SetLineStyleTable(&LineStyle);
		pGD->Render();
	}

	// 竵E碌狡聊?
	if (bReDraw)
	{
		UpdateAll();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::Enter()
{
	gld_shape_sel	sel;

	m_pIClass->DoSelect(sel);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::Leave()
{
	if (bEdit)
	{
		MyEndEdit();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::FillSolidRect(int x, int y, int w, int h, COLORREF color, BOOL bReDraw/* = TRUE*/)
{
	TGraphicsDevice *pGD = m_pIClass->GetGraphDev();
	pGD->SetMatrix(teMatrix * _M_matrix.Invert());

	int x1 = m_rtRegion.left + x, y1 = m_rtRegion.top + y;
	int x2 = m_rtRegion.left + x + w - 1, y2 = m_rtRegion.top + y + h - 1;

	TColor fcolor(GetRValue(color), GetGValue(color), GetBValue(color), 255);

	TFillStyleTable fst;
	int fsindex = fst.Add(new TSolidFillStyle(fcolor));

	pGD->SetFillStyle0(fsindex);
	pGD->MoveTo(x1, y1);
	pGD->LineTo(x2, y1);
	pGD->LineTo(x2, y2);
	pGD->LineTo(x1, y2);
	pGD->LineTo(x1, y1);
	pGD->SetFillStyleTable(&fst);
	pGD->Render();

	if (bReDraw)
	{
		UpdateAll();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::UpdateAll()
{
	if (nLockUpdate == 0)
	{
		if (CheckBound())
		{
			TGraphicsDevice *pGD = m_pIClass->GetGraphDev();
			HDC hDC = GetDC(_M_hwnd);

			if (_M_hwnd != NULL)
			{
				int w, h;

				pGD->GetSize(w, h);

				HDC hMemDC = CreateCompatibleDC(hDC);
				HBITMAP hMemBitmap = CreateCompatibleBitmap(hDC, w, h), hOldBitmap;
				hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);

				pGD->Display(hMemDC, 0, 0);
				DrawBorder(hMemDC);
				DrawLink(hMemDC);
				DrawSelect(hMemDC);

				BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
				SelectObject(hMemDC, hOldBitmap);
				DeleteObject(hMemBitmap);
				DeleteDC(hMemDC);
			}

			ReleaseDC(_M_hwnd, hDC);
		}
	}
}

template <class _IClass>
HWND TTextToolExTmpl<_IClass>::GetHwnd()
{
	return _M_hwnd;
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ReDraw(gld_rect *pRect)
{
	if (nLockDraw == 0)
	{
		TGraphicsDevice *pGD = m_pIClass->GetGraphDev();
		GGRECT oldClipRect = pGD->GetClipRect();

		pGD->SetMatrix(teMatrix * _M_matrix.Invert());

		int x1 = m_rtRegion.left - 20, y1 = m_rtRegion.top - 20;
		int x2 = m_rtRegion.right + 20, y2 = m_rtRegion.bottom + 20;

		TColor fcolor(255, 255, 255, 255);

		TFillStyleTable fst;
		int fsindex = fst.Add(new TSolidFillStyle(fcolor));
 
		pGD->SetFillStyle0(fsindex);
		pGD->MoveTo(x1, y1);
		pGD->LineTo(x2, y1);
		pGD->LineTo(x2, y2);
		pGD->LineTo(x1, y2);
		pGD->LineTo(x1, y1);

		pGD->SetFillStyleTable(&fst);
		pGD->Render();

		m_ParagraphList[0]->ReDrawDown(FALSE);
		UpdateAll();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::RestoreBackground(RECT &rt)
{
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::DrawBorder(HDC hdc)
{
	// 画边縼E
	int x1 = m_rtRegion.left - 20, y1 = m_rtRegion.top - 20;
	int x2 = m_rtRegion.right + 20, y2 = m_rtRegion.bottom + 20;

	TDraftDC dc(hdc);

	HPEN pen = CreatePen(PS_SOLID, 1, 0), oldpen;
	oldpen = dc.SelectPen(pen);

	dc.Matrix(teMatrix * _M_matrix.Invert());

	dc.MoveTo(x1, y1, TDraftDC::_trans_tag());
	dc.LineTo(x2, y1, TDraftDC::_trans_tag());
	dc.LineTo(x2, y2, TDraftDC::_trans_tag());
	dc.LineTo(x1, y2, TDraftDC::_trans_tag());
	dc.LineTo(x1, y1, TDraftDC::_trans_tag());

	// Handle
	HBRUSH hbr, oldhbr;

	hbr = CreateSolidBrush(RGB(255, 255, 255));
	oldhbr = dc.SelectBrush(hbr);

	int cx = x2, cy = y2;
	TMatrix mat = teMatrix * _M_matrix.Invert();
	mat.Transform(cx, cy);
	dc.Ellipse(cx - nHandleSize / 2,
		cy - nHandleSize / 2,
		cx + nHandleSize / 2,
		cy + nHandleSize / 2,
		TDraftDC::_no_trans_tag());

	dc.SelectBrush(oldhbr);
	DeleteObject(hbr);

	dc.SelectPen(oldpen);
	DeleteObject(pen);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::DrawSelect(HDC hdc)
{
	if (SelectIsEmpty())
	{
		return;
	}

	int x1 = m_rtRegion.left/* + BORDER_SIZE*/, y1 = m_rtRegion.top/* + BORDER_SIZE*/;
	int x2 = m_rtRegion.right/* - BORDER_SIZE*/, y2 = m_rtRegion.bottom/* - BORDER_SIZE*/;
	POINT lt, rt, lb, rb;
	std::vector <POINT> points;
	int i;

	lt.x = x1, lt.y = y1;
	rt.x = x2, rt.y = y1;
	lb.x = x1, lb.y = y2;
	rb.x = x2, rb.y = y2;

	for (i = ptSelStart.y; i <= ptSelEnd.y; i++)
	{
		CParagraph *pPara = m_ParagraphList[i];

		if (i == ptSelStart.y)
		{
			POINT pt = lt;

			pt.x += pPara->GetDrawX(ptSelStart.x);
			pt.y += pPara->GetDrawY();
			points.push_back(pt);

			pt.y += pPara->GetHeight();
			points.push_back(pt);
		}
		else
		{
			POINT pt = lt;

			pt.x += pPara->GetDrawX(pPara->GetFirstTextBlock());
			pt.y += pPara->GetDrawY();
			points.push_back(pt);
			pt.y += pPara->GetHeight();
			points.push_back(pt);
		}
	}

	for (i = ptSelEnd.y; i >= ptSelStart.y; i--)
	{
		CParagraph *pPara = m_ParagraphList[i];

		if (i == ptSelEnd.y)
		{
			POINT pt = lt, pt1;
			int dx = pPara->GetDrawX(ptSelEnd.x);

			if (dx == BORDER_SIZE)
			{
				int max_w = 0;

				TextBlocks::iterator it = pPara->m_TextBlockList.begin();
				while(it != pPara->m_TextBlockList.end())
				{
					CTextBlock *tb = *it;

					for (int j = 0; j < tb->GetLength(); j++)
					{
						max_w = max(max_w, GetCharWidth(tb->strText[j], 0, tb->TextFormat));
					}

					it++;
				}

				pt.x += max_w; 
			}
			else
			{
				pt.x += dx;
			}

			pt.y += pPara->GetDrawY();

			pt1 = pt;
			pt1.y += pPara->GetHeight();

			points.push_back(pt1);
			points.push_back(pt);
		}
		else
		{
			POINT pt = lt, pt1;
			int dx = (pPara->GetLength() == 0 ? 
				pPara->GetDrawX(pPara->GetFirstTextBlock()) + GetCharWidth(L' ', 0, pPara->GetLastTextBlock()->TextFormat) :
			pPara->GetDrawX(pPara->GetLastTextBlock()) + pPara->GetLastTextBlock()->GetWidth());

			if (dx == BORDER_SIZE)
			{
				int max_w = 0;

				TextBlocks::iterator it = pPara->m_TextBlockList.begin();
				while(it != pPara->m_TextBlockList.end())
				{
					CTextBlock *tb = *it;

					for (int j = 0; j < tb->GetLength(); j++)
					{
						max_w = max(max_w, GetCharWidth(tb->strText[j], 0, tb->TextFormat));
					}

					it++;
				}

				pt.x += max_w; 
			}
			else
			{
				pt.x += dx;
			}

			pt.y += pPara->GetDrawY();
			pt1 = pt;
			pt1.y += pPara->GetHeight();
			points.push_back(pt1);
			points.push_back(pt);
		}
	}
/*
	for (size_t j = 0, k = points.size() / 2; j < points.size() / 2; j++, k++)
	{
		if (points[j].x > points[k].x)
		{
			swap(points[j].x, points[k].x);
		}
	}
*/
	if (points.size() > 0)
	{
		points.push_back(points[0]);

		TMatrix mat = teMatrix * _M_matrix.Invert() * TMatrix(0.05f, 0, 0, 0.05f, 0, 0);

		for (size_t j = 0; j < points.size(); j++)
		{
			POINT temp = points[j];
			mat.Transform(temp.x, temp.y);
			points[j] = temp;
		}

		int polycount = (int)points.size();
		HRGN hRgn = CreatePolyPolygonRgn(&points[0], &polycount, 1, ALTERNATE);
		InvertRgn(hdc, hRgn);
		DeleteObject(hRgn);
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::DrawLink(HDC hdc)
{
}
	
template <class _IClass>
BOOL TTextToolExTmpl<_IClass>::CheckBound()
{
	Paragraphs::iterator i = m_ParagraphList.begin();
	RECT rtNew = m_rtRegion;
	int nHeight = 0;

	while(i != m_ParagraphList.end())
	{
		nHeight += (*i)->GetHeight();
		i++;
	}

	if (nHeight > abs(m_rtRegion.bottom - m_rtRegion.top - BORDER_HEIGHT))
	{
		// 超出了
		// 重新调整编辑区域大小
		rtNew.bottom = m_rtRegion.top + nHeight + BORDER_HEIGHT * 2;
		ExecAndGroup(new COMMAND_CLSNAME(ChangeRegion)(this, rtNew));
		return FALSE;
	}

	return TRUE;
}

static bool getGlyphRecords_ltPos(gldText2 &text, int &x, int &y)
{
	if (text.m_glyphRecords.empty())
	{
		return false;
	}

	bool finish = false;
	x = y = 2147483647;
	int curx = 0, cury = 0;

	for(list<gldTextGlyphRecord*>::iterator it = text.m_glyphRecords.begin(); it != text.m_glyphRecords.end(); it++)
	{
		gldTextGlyphRecord *_rec = (*it);

		if (_rec != NULL)
		{
			int cx = (short)_rec->m_xOffset;
			int cy = (short)_rec->m_yOffset;

			curx = cx;
			cury = cy;

			if (curx < x)
			{
				x = curx;
			}

			if (cury < y)
			{
				y = cury;
			}

			finish = true;
		}
	}

	return finish;
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ConvertToGldText(gldText2 &text)
{
/*	bool adjust = false;
	int ltx, lty;

	if (TextProperty.TextType == TextType_Static && getGlyphRecords_ltPos(text, ltx, lty))
	{
		adjust = true;
	}
*/
	// 清除
	text.ClearAll();

	if (TextProperty.TextType == TextType_Static)
	{
		text.m_textType = gtextStatic;
	}
	else if (TextProperty.TextType == TextType_Dynamic)
	{
		text.m_textType = gtextDynamic;
		text.m_dynaInfo.m_flag &= ~seditTextFlagsHTML;	
	}
	else if (TextProperty.TextType == TextType_HTML)
	{
		text.m_textType = gtextDynamic;
		text.m_dynaInfo.m_flag |= seditTextFlagsHTML;
	}

	text.m_dynaInfo.SetHasBorderFlag(BOOL2bool(TextProperty.bBorder));
	text.m_dynaInfo.m_variableName = CA2W(TextProperty.strBindVar.c_str());
	text.m_dynaInfo.SetIsReadOnlyFlag(!BOOL2bool(TextProperty.bCanEdit));
	text.m_dynaInfo.SetCanSelectFlag(TextProperty.bCanEdit ? true : BOOL2bool(TextProperty.bCanSelect));
	text.m_dynaInfo.SetHasMaxLengthFlag(TextProperty.nMaxLength > 0);
	text.m_dynaInfo.m_maxLength = TextProperty.nMaxLength;

	ChangeMultilineType(&text, TextProperty.MultilineType);

	// 复制编辑信息
	text.rtRegion = m_rtRegion;
	text.m_bounds = m_rtRegion;
	Paragraphs::iterator it = m_ParagraphList.begin();
	while(it != m_ParagraphList.end())
	{
		text.m_ParagraphList.push_back((*it)->Clone(this));
		it++;
	}

	text.BuildRecord();
	text.bNoConvert = FALSE;
/*
	if (adjust)
	{
		int cx, cy;

		if (getGlyphRecords_ltPos(text, cx, cy))
		{
			gldTextGlyphRecord *_rec = *text.m_glyphRecords.begin();
			int offsetx, offsety;
			offsetx = cx - (short)ltx;
			offsety = cy - (short)lty;

			text.rtRegion.left -= offsetx;
			text.rtRegion.top -= offsety;
			text.rtRegion.right -= offsetx;
			text.rtRegion.bottom -= offsety;
			text.m_bounds = text.rtRegion;

			text.BuildRecord();
		}
	}
	*/
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SubstituteFonts(STRING_MAP &fontmap, gldText2 &text)
{
	text.rpl_fontmap = fontmap;
	text.Clear();
	ConvertFromGldText(text);
	text.rpl_fontmap.clear();
	ConvertToGldText(text);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ConvertFromGldText(gldText2 &text/*, BOOL bImport*/)
{
	//MY_USES_CONVERSION;

	if (text.m_textType == gtextStatic)
	{
		TextProperty.TextType = TextType_Static;
	}
	else if (text.m_textType == gtextDynamic)
	{
		if (text.m_dynaInfo.IsHTML())
		{
			TextProperty.TextType = TextType_HTML;
		}
		else
		{
			TextProperty.TextType = TextType_Dynamic;
		}
	}

	// 铁赜新的
	text.BuildEditInfo();

	TextProperty.bBorder = bool2BOOL(text.m_dynaInfo.HasBorder());
	TextProperty.bCanSelect = bool2BOOL(text.m_dynaInfo.CanSelect());
	TextProperty.bCanEdit = !bool2BOOL(text.m_dynaInfo.IsReadOnly());
	TextProperty.strBindVar = CW2A(text.m_dynaInfo.m_variableName.c_str());
	TextProperty.MultilineType = GetMultilineType(&text);
	TextProperty.nMaxLength = text.m_dynaInfo.m_maxLength;

	Paragraphs::iterator ip = text.m_ParagraphList.begin();
	while(ip != text.m_ParagraphList.end())
	{
		CParagraph *pPara = (*ip)->Clone(this);

		pPara->SetEditBase(this);
		m_ParagraphList.push_back(pPara);

		pPara->CalcDrawY();
		ip++;
	}

	// 调整编辑框位置
	if (text.m_textType == gtextStatic && text.bNoConvert)
	{
		if (m_ParagraphList.size() > 0)
		{
			CParagraph *pBasePara = *m_ParagraphList.begin();

			int nOffsetX = 0, nOffsetY = 0;

			nOffsetX = pBasePara->GetDrawX();
			nOffsetY = BORDER_HEIGHT + pBasePara->GetBaseLine();

			if (text.m_glyphRecords.size() > 0)
			{
				gldTextGlyphRecord *pBaseRecord = *text.m_glyphRecords.begin();
				gldMatrix &mx = text.m_textMatrix;
				long x = mx.GetX((short)pBaseRecord->m_xOffset, (short)pBaseRecord->m_yOffset);
				long y = mx.GetY((short)pBaseRecord->m_xOffset, (short)pBaseRecord->m_yOffset);				
				text.m_bounds.left = x - nOffsetX;
				text.m_bounds.top = y - nOffsetY;			
			}
		}
	}

	memset(&ptSelStart, 0, sizeof(ptSelStart));
	memset(&ptSelEnd, 0, sizeof(ptSelEnd));

	// 求编辑框大小
	if (text.bNoConvert)
	{
		RECT rtt;

		rtt.left = text.m_bounds.left;
		rtt.top = text.m_bounds.top;

		int nMaxWidth = 0, nMaxHeight = 0;

		Paragraphs::iterator it_para = m_ParagraphList.begin();
		while(it_para != m_ParagraphList.end())
		{
			CParagraph *pPara = *it_para;
			TextBlocks::iterator it_tb = pPara->m_TextBlockList.begin();
			int w = 0;

			while(it_tb != pPara->m_TextBlockList.end())
			{
				w += (*it_tb)->GetWidth();
				it_tb++;
			}

			nMaxWidth = max(nMaxWidth, w);
			nMaxHeight += pPara->GetHeight();

			it_para++;
		}

		rtt.right = rtt.left + nMaxWidth + BORDER_SIZE * 2;
		rtt.bottom = rtt.top + nMaxHeight + BORDER_HEIGHT * 2;

		if (TextProperty.TextType == TextType_Static)
		{
			m_rtRegion = rtt;
		}
		else
		{
			rtt.right += (text.m_dynaInfo.m_leftMargin + text.m_dynaInfo.m_rightMargin);

			m_rtRegion = text.m_bounds;

			/*
			if (text.m_bounds.Width() < (rtt.right - rtt.left + 1))
			{
				// 因为字体变动原因，导致文本框大小不够
				int nMaxWidth = 0;

				Paragraphs::iterator it = m_ParagraphList.begin();

				while(it != m_ParagraphList.end())
				{
					CParagraph *para = *it;
					nMaxWidth = max(nMaxWidth, para->GetMarginRight() + 
						abs(para->GetMarginLeft() + para->GetIndent()));
					it++;
				}

				int w = GetAllMinWidth() + nMaxWidth + BORDER_SIZE * 2; //rtt.right - rtt.left + 1;

				m_rtRegion.right = m_rtRegion.left + w;
			}
*/
			if (text.m_bounds.Height() < (rtt.bottom - rtt.top + 1))
			{
				// 文本框高度不够
				int h = (rtt.bottom - rtt.top + 1);

				m_rtRegion.bottom = m_rtRegion.top + h;
			}
		}

		text.bNoConvert = FALSE;
	}
	else
	{
		m_rtRegion = text.m_bounds;
	}

//	CRect aj_rt = m_rtRegion;
//	text.m_textMatrix.invert().TransformRect(aj_rt);
//	m_rtRegion = aj_rt;

	/////////////////////// 整纴E
	for (size_t i = 0; i < m_ParagraphList.size(); i++)
	{
		m_ParagraphList[i]->CalcHeight();
		m_ParagraphList[i]->Arrange();
	}

	if (!CheckBound())
	{
		PopUndoCommand();
	}
}

template <class _IClass>
bool TTextToolExTmpl<_IClass>::OnSetCursor(UINT nFlags, const POINT &point)
{
	if (!m_pIClass->CanAddText())
	{
		::SetCursor(m_pIClass->StockCursor(IDC_G_STOP));
		return true;
	}

	POINT pt = point;

	if (IsActive())
	{
		// 判断是否在Handle上
		int cx = m_rtRegion.right, cy = m_rtRegion.bottom;
		int px = pt.x, py = pt.y;

		teMatrix.Transform(cx, cy);
		_M_matrix.Invert().Transform(cx, cy);
		_M_matrix.Invert().Transform(px, py);

		if (px >= cx - nHandleSize / 2 &&
			py >= cy - nHandleSize / 2 &&
			px <= cx + nHandleSize / 2 &&
			py <= cy + nHandleSize / 2)
		{
			// 设置右下角光眮E
			int lbx = m_rtRegion.left, lby = m_rtRegion.bottom,
				rtx = m_rtRegion.right, rty = m_rtRegion.top,
				rbx = m_rtRegion.right, rby = m_rtRegion.bottom;

			teMatrix.Transform(lbx, lby);
			teMatrix.Transform(rtx, rty);
			teMatrix.Transform(rbx, rby);

			::SetCursor(m_pIClass->StockCursor(
				TSECursor::GetStretchCursor(rbx, rby, lbx, lby, rtx, rty)));
			return true;
		}

		// 判断是否在文本框纴E丒
		teMatrix.Invert().Transform(pt.x, pt.y);

		if (pt.x >= m_rtRegion.left && pt.y >= m_rtRegion.top &&
			pt.x <= m_rtRegion.right && pt.y <= m_rtRegion.bottom)
		{
			::SetCursor(m_pIClass->StockCursor(IDC_G_IBEAM));
			return true;
		}
	}
	else
	{
		gld_shape gs = m_pIClass->PtOnText(pt.x, pt.y);
		if (gs.validate())
		{
			::SetCursor(m_pIClass->StockCursor(IDC_G_IBEAM));
			return true;
		}
		else
		{
			::SetCursor(m_pIClass->StockCursor(IDC_G_CROSS));
			return true;
		}
	}

	return false;
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnSelChanged()
{
	if (IsActive())
	{
		if (nLockSendUpdate == 0)
		{
			CSubjectManager::Instance()->GetSubject("TextSelChange")->Notify((void *)0);
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnFormatChange()
{
	OnSelChanged();
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnDraw(HDC hdc)
{
	if (IsActive())
	{
		DrawBorder(hdc);
		DrawSelect(hdc);
	}
}

#define ENTER_TEXT(t) \
{ \
	gld_shape gs = t; \
	if (gs.validate()) \
	{ \
		CTraitInstance::GInstance *pCh = CTraitInstance(gs); \
		if (pCh) \
		{ \
			gldObj *pObj = CTraitInstance(gs); \
			if (pObj->GetGObjType() == gobjText) \
			{ \
				gldText2 *pText = (gldText2 *)pObj; \
				LockUpdate(); \
				LockDraw(); \
				ConvertFromGldText(*pText); \
				SelectAll();

#define LEAVE_TEXT() \
				CTextEditBase::EndEdit(); \
				UnlockDraw(); \
				UnlockUpdate(); \
			} \
		} \
	} \
}



template <class _IClass>
DWORD TTextToolExTmpl<_IClass>::GetFormat(CTextFormat &TextFormat)
{
	if (IsActive())
	{
		return CTextEditBase::GetFormat(TextFormat);
	}
	else
	{
		gld_shape_sel &sel = m_pIClass->CurSel();
		gld_shape_sel::iterator i = sel.begin();
		DWORD dwFlag = 0;
		BOOL bFirst = TRUE, bHaveText = FALSE;

		if (sel.count() > 0)
		{
			while(i != sel.end())
			{
				ENTER_TEXT(*i)

				bHaveText = TRUE;

				if (!bFirst)
				{
					CTextFormat fmt;
					DWORD dwFlag1 = CTextEditBase::GetFormat(fmt);
					dwFlag = TextFormat.Compare(fmt, dwFlag1, dwFlag);
				}
				else
				{
					dwFlag = CTextEditBase::GetFormat(TextFormat);
					bFirst = FALSE;
				}

				LEAVE_TEXT()
				i++;
			}

			if (bHaveText) return dwFlag;
		}

		TextFormat = m_DefTextFormat1;
		return TF_ALL;
	}
}

template <class _IClass>
DWORD TTextToolExTmpl<_IClass>::GetParaFormat(CParaFormat &ParaFormat)
{
	if (IsActive())
	{
		return CTextEditBase::GetParaFormat(ParaFormat);
	}
	else
	{
		gld_shape_sel &sel = m_pIClass->CurSel();
		gld_shape_sel::iterator i = sel.begin();
		DWORD dwFlag = 0;
		BOOL bFirst = TRUE, bHaveText = FALSE;

		if (sel.count() > 0)
		{
			while(i != sel.end())
			{
				ENTER_TEXT(*i)

				bHaveText = TRUE;

				if (!bFirst)
				{
					CParaFormat fmt;
					DWORD dwFlag1 = CTextEditBase::GetParaFormat(fmt);
					dwFlag = ParaFormat.Compare(fmt, dwFlag1, dwFlag);
				}
				else
				{
					dwFlag = CTextEditBase::GetParaFormat(ParaFormat);
					bFirst = FALSE;
				}

				LEAVE_TEXT()
				i++;
			}

			if (bHaveText) return dwFlag;
		}

		// 返回默认的段落格式
		ParaFormat = m_DefParaFormat1;
		return PF_ALL;
	}
}

#define BEGIN_MODIFY \
		gld_shape_sel sel = m_pIClass->CurSel(); \
		if (!sel.empty()) \
		{ \
			GOBJECT_LIST objList; \
			for (gld_shape_sel::iterator iSel = sel.begin(); iSel != sel.end(); iSel++) \
			{ \
				gldObj *pObj = CTraitInstance(*iSel); \
				objList.push_back(pObj); \
			} \
			for (GOBJECT_LIST::iterator iObj = objList.begin(); iObj != objList.end(); iObj++) \
			{ \
				gldObj *pObj = *iObj; \
				if (pObj->GetGObjType() == gobjText) \
				{ \
					gldText2 *pText = (gldText2 *)pObj; \
					LockUpdate(); \
					LockDraw(); \
					ConvertFromGldText(*pText); \
					LockSendUpdate(); \
					SelectAll(); \
					bEdit = TRUE;

#define END_MODIFY \
					CheckBound(); \
					gldText2 *pNewText = new gldText2; \
					oBinStream out; \
					iBinStream in; \
					pText->WriteToBinStream(out); \
					U8 *p = new U8[out.Size()]; \
					U32 _size = out.Size(); \
					out.WriteToMemory(p); \
					in.ReadFromMemory(out.Size(), p); \
					pNewText->ReadFromBinStream(in); \
					delete p; \
					ConvertToGldText(*pNewText); \
					CTextEditBase::EndEdit(); \
					bEdit = FALSE; \
					m_pIClass->ModifyText(pText, pNewText); \
					UnlockSendUpdate(); \
					UnlockDraw(); \
					UnlockUpdate(); \
				} \
			} \
			m_pIClass->Repaint(); \
		} \

#define APPLY_AS_RULE(r) \
	LockDraw(); \
	LockSendUpdate(); \
	LockUpdate(); \
	if (!(GetTextToolCaps().dwAllowAS & r)) \
	{ \
		CTextEditBase::SelectAll(); \
	}

#define APPLY_AS_RULE_END(r) \
	if (!(GetTextToolCaps().dwAllowAS & r)) \
	{ \
		CTextEditBase::SelToCursor(); \
	} \
	UnlockDraw(); \
	UnlockSendUpdate(); \
	UnlockUpdate(); \
	ReDraw();

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetAlignType(CParagraph::emAlignType type)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetAlignType(type);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_ALIGN);
		CTextEditBase::SetAlignType(type);
		APPLY_AS_RULE_END(TTCAPS_ALIGN);
		UnlockSendUpdate();
	}

	m_DefParaFormat1.SetAlignType(type);
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetLineSpacing(int nSpacing)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetLineSpacing(nSpacing);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_LINESPACING);
		CTextEditBase::SetLineSpacing(nSpacing);
		APPLY_AS_RULE_END(TTCAPS_LINESPACING);
		UnlockSendUpdate();
	}

	m_DefParaFormat1.SetSpacing(nSpacing);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetFontFace(const char *pFontFace)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetFontFace(pFontFace);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_FONTFACE);
		CTextEditBase::SetFontFace(pFontFace);
		APPLY_AS_RULE_END(TTCAPS_FONTFACE);
		UnlockSendUpdate();
	}

	m_DefTextFormat1.SetFontFace(pFontFace);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetFontSize(int nSize)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetFontSize(nSize);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_FONTSIZE);
		CTextEditBase::SetFontSize(nSize);
		APPLY_AS_RULE_END(TTCAPS_FONTSIZE);
		UnlockSendUpdate();
	}

	m_DefTextFormat1.SetFontSize(nSize);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetFontColor(COLORREF crFont)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetFontColor(crFont);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_FONTCOLOR);
		CTextEditBase::SetFontColor(crFont);
		APPLY_AS_RULE_END(TTCAPS_FONTCOLOR);
		UnlockSendUpdate();
	}

	m_DefTextFormat1.SetFontColor(crFont);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetFontBold(BOOL bFlag)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetFontBold(bFlag);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_BOLD);
		CTextEditBase::SetFontBold(bFlag);
		APPLY_AS_RULE_END(TTCAPS_BOLD);
		UnlockSendUpdate();
	}

	m_DefTextFormat1.SetBold(bFlag);
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetFontItalic(BOOL bFlag)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetFontItalic(bFlag);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_ITALIC);
		CTextEditBase::SetFontItalic(bFlag);
		APPLY_AS_RULE_END(TTCAPS_ITALIC);
		UnlockSendUpdate();
	}

	m_DefTextFormat1.SetItalic(bFlag);
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetCharSpacing(int nCharSpacing)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetCharSpacing(nCharSpacing);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_CHARSPACING);
		CTextEditBase::SetCharSpacing(nCharSpacing);
		APPLY_AS_RULE_END(TTCAPS_CHARSPACING);
		UnlockSendUpdate();
	}

	m_DefTextFormat1.SetSpacing(nCharSpacing);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetCharLocate(CTextFormat::emCharLocate Locate)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetCharLocate(Locate);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_LOCATE);
		CTextEditBase::SetCharLocate(Locate);
		APPLY_AS_RULE_END(TTCAPS_LOCATE);
		UnlockSendUpdate();
	}

//	m_DefTextFormat1.SetLocate(Locate);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetLink(std::string strLink)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetLink(strLink);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_LINK);
		CTextEditBase::SetLink(strLink);
		APPLY_AS_RULE_END(TTCAPS_LINK);
		UnlockSendUpdate();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetTarget(std::string strTarget)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetTarget(strTarget);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_TARGET);
		CTextEditBase::SetTarget(strTarget);
		APPLY_AS_RULE_END(TTCAPS_TARGET);
		UnlockSendUpdate();
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetMarginLeft(int nLeft)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetMarginLeft(nLeft);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_MARGINLEFT);
		CTextEditBase::SetMarginLeft(nLeft);
		APPLY_AS_RULE_END(TTCAPS_MARGINLEFT);
		UnlockSendUpdate();
	}

	m_DefParaFormat1.SetMarginLeft(nLeft);
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetMarginRight(int nRight)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetMarginRight(nRight);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_MARGINRIGHT);
		CTextEditBase::SetMarginRight(nRight);
		APPLY_AS_RULE_END(TTCAPS_MARGINRIGHT);
		UnlockSendUpdate();
	}

	m_DefParaFormat1.SetMarginRight(nRight);
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetIndent(int nIndent)
{
	if (!IsActive())
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			CTextEditBase::SetIndent(nIndent);
			END_MODIFY
		}
	}
	else
	{
		LockSendUpdate();
		APPLY_AS_RULE(TTCAPS_INDENT);
		CTextEditBase::SetIndent(nIndent);
		APPLY_AS_RULE_END(TTCAPS_INDENT);
		UnlockSendUpdate();
	}

	m_DefParaFormat1.SetIndent(nIndent);
	Update(NULL);
}

// 功能：获取文本工具能力。
// 返回值：根据TTCAPS.dwCaps来判断当前能改动哪些文本属性。
template <class _IClass>
TTCAPS TTextToolExTmpl<_IClass>::GetTextToolCaps()
{
	static TTCAPS NullCaps = 
	{
		0,
		0,
	};

	static TTCAPS AllCaps =
	{
		TTCAPS_ALL,
		TTCAPS_ALL,
	};

	static TTCAPS StaticCaps =
	{
		TTCAPS_ALL & ~TTCAPS_BORDER & ~TTCAPS_CANSELECT &
			~TTCAPS_LINK & ~TTCAPS_TARGET & ~TTCAPS_CANEDIT & ~TTCAPS_BINDVAR & ~TTCAPS_MAXLENGTH & ~TTCAPS_MULTITYPE,
		TTCAPS_ALL,
	};

	static TTCAPS DynamicCaps = 
	{
		TTCAPS_FONTFACE | TTCAPS_FONTSIZE | TTCAPS_FONTCOLOR | TTCAPS_BOLD | TTCAPS_ITALIC | 
			TTCAPS_LINESPACING | TTCAPS_INDENT | TTCAPS_MARGINLEFT | TTCAPS_MARGINRIGHT | 
			TTCAPS_ALIGN | TTCAPS_BORDER | TTCAPS_CANSELECT | TTCAPS_BINDVAR | TTCAPS_CANEDIT | TTCAPS_MAXLENGTH | TTCAPS_MULTITYPE,
		0,
	};

	static TTCAPS HTMLCaps = 
	{
		TTCAPS_FONTFACE | TTCAPS_FONTSIZE | TTCAPS_FONTCOLOR | TTCAPS_BOLD | TTCAPS_ITALIC | TTCAPS_LINK | 
			TTCAPS_TARGET | TTCAPS_LINESPACING | TTCAPS_INDENT | TTCAPS_MARGINLEFT | TTCAPS_MARGINRIGHT |
			TTCAPS_ALIGN | TTCAPS_BORDER | TTCAPS_CANSELECT | TTCAPS_BINDVAR | TTCAPS_CANEDIT | TTCAPS_MAXLENGTH | TTCAPS_MULTITYPE,
		TTCAPS_FONTFACE | TTCAPS_FONTSIZE | TTCAPS_FONTCOLOR | TTCAPS_BOLD | TTCAPS_ITALIC | TTCAPS_LINK | TTCAPS_TARGET,
	};

	if (IsActive())
	{
		TTCAPS caps = NullCaps;

		switch(TextProperty.TextType)
		{
		case TextType_Static:
			caps = StaticCaps;
			break;
		case TextType_Dynamic:
			caps = DynamicCaps;
			break;
		case TextType_HTML:
			caps = HTMLCaps;
			break;
		}

		if (SelectIsEmpty() || TextProperty.bCanEdit)
		{
			caps.dwCaps &= (~TTCAPS_LINK & ~TTCAPS_TARGET);
		}

		if (TextProperty.bCanEdit)
		{
			caps.dwCaps &= ~TTCAPS_CANSELECT;
		}
		else
		{
			caps.dwCaps &= ~TTCAPS_MAXLENGTH;
		}

		return caps;
	}
	else
	{
		gld_shape_sel sel = m_pIClass->CurSel();
		BOOL bHaveText = FALSE;
		TTCAPS ttcaps = 
		{
			0xffffffff,
			0xffffffff,
		};

		if (!sel.empty())
		{
			for (gld_shape_sel::iterator i = sel.begin(); i != sel.end(); i++)
			{
				gld_shape gs = *i;
				if (gs.validate())
				{
					CTraitInstance::GInstance *pCh = CTraitInstance(gs);
					if (pCh)
					{
						gldObj *pObj = CTraitInstance(gs);
						if (pObj->GetGObjType() == gobjText)
						{
							bHaveText = TRUE;

							gldText2 *pText = (gldText2 *)pObj;
							gldText2_TEXTTYPE ttype;

							if (pText->m_textType == gtextStatic)
							{
								ttype = TextType_Static;
							}
							else if (pText->m_textType == gtextDynamic)
							{
								if (pText->m_dynaInfo.IsHTML())
								{
									ttype = TextType_HTML;
								}
								else
								{
									ttype = TextType_Dynamic;
								}
							}

							switch(ttype)
							{
							case TextType_Static:
								ttcaps.dwCaps &= StaticCaps.dwCaps;
								ttcaps.dwAllowAS &= StaticCaps.dwAllowAS;
								break;
							case TextType_Dynamic:
								ttcaps.dwCaps &= DynamicCaps.dwCaps;
								ttcaps.dwAllowAS &= DynamicCaps.dwAllowAS;
								break;
							case TextType_HTML:
								ttcaps.dwCaps &= HTMLCaps.dwCaps;
								ttcaps.dwAllowAS &= HTMLCaps.dwAllowAS;
								break;
							}

							if (ttype == TextType_Dynamic || ttype == TextType_HTML)
							{
								if (!pText->m_dynaInfo.IsReadOnly())
								{
									ttcaps.dwCaps &= (~TTCAPS_LINK & ~TTCAPS_TARGET);
									ttcaps.dwCaps &= ~TTCAPS_CANSELECT;
								}

								if (!pText->m_dynaInfo.CanSelect() ||
									pText->m_dynaInfo.IsReadOnly())
								{
									ttcaps.dwCaps &= ~TTCAPS_MAXLENGTH;
								}
							}
						}
					}
				}
			}

			if (bHaveText) return ttcaps;
		}

		TTCAPS caps = NullCaps;

		switch(TextProperty1.TextType)
		{
		case TextType_Static:
			caps = StaticCaps;
			break;
		case TextType_Dynamic:
			caps = DynamicCaps;
			break;
		case TextType_HTML:
			caps = HTMLCaps;
			break;
		}

		caps.dwCaps &= (~TTCAPS_LINK & ~TTCAPS_TARGET);

		return caps;
	}

	return NullCaps;
}

// 功能：获取文本属性，如果当前为编辑状态，返回正在被编辑的文本的属性，
//       否则，返回被选中的文本的属性。（用这个函数来获取文本类型，Static,Dynamic or HTML)
// 返回值：眮E窘峁怪心男┦萦行А?
template <class _IClass>
DWORD TTextToolExTmpl<_IClass>::GetTextProperty(TEXT_PROPERTY &prop)
{
	if (IsActive())
	{
		if (TextProperty.TextType != TextType_Static)
		{
			DWORD mask = TP_ALL;
			prop = TextProperty;

			if (!TextProperty.bCanSelect || !TextProperty.bCanEdit)
			{
				mask &= ~TP_MAXLENGTH;
			}

			return mask;
		}
		else
		{
			prop = TextProperty;
			return TP_ALL & ~TP_BORDER & ~TP_CANSELECT & ~TP_BINDVAR & ~TP_CANEDIT & 
				~TP_MAXLENGTH & ~TP_MULTITYPE;
		}
	}
	else
	{
		gld_shape_sel &sel = m_pIClass->CurSel();
		DWORD dwFlags = TP_ALL;
		BOOL bFirst = TRUE, bHaveText = FALSE;

		if (!sel.empty())
		{
			for (gld_shape_sel::iterator i = sel.begin(); i != sel.end(); i++)
			{
				gld_shape gs = *i;
				if (gs.validate())
				{
					CTraitInstance::GInstance *pCh = CTraitInstance(gs);
					if (pCh)
					{
						gldObj *pObj = CTraitInstance(gs);
						if (pObj->GetGObjType() == gobjText)
						{
							gldText2 *pText = (gldText2 *)pObj;

							bHaveText = TRUE;

							// 取文本属性
							TEXT_PROPERTY tp;

							if (pText->m_textType == gtextStatic)
							{
								tp.TextType = TextType_Static;
							}
							else if (pText->m_textType == gtextDynamic)
							{
								if (pText->m_dynaInfo.IsHTML())
								{
									tp.TextType = TextType_HTML;
								}
								else
								{
									tp.TextType = TextType_Dynamic;
								}
							}

							tp.bBorder    = FALSE;
							tp.bCanSelect = FALSE;
							tp.bCanEdit   = FALSE;
							tp.nMaxLength = 0;
							tp.MultilineType = Text_Multiline;
							tp.strBindVar = "";

							if (tp.TextType != TextType_Static)
							{
								tp.bBorder = bool2BOOL(pText->m_dynaInfo.HasBorder());
								tp.bCanSelect = bool2BOOL(pText->m_dynaInfo.CanSelect());
								tp.strBindVar = CW2A(pText->m_dynaInfo.m_variableName.c_str());
								tp.bCanEdit = !bool2BOOL(pText->m_dynaInfo.IsReadOnly());
								tp.nMaxLength = pText->m_dynaInfo.m_maxLength;
								tp.MultilineType = GetMultilineType(pText);
							}

							// 比较
							if (bFirst)
							{
								prop.TextType = tp.TextType;
								prop.bBorder  = tp.bBorder;
								prop.bCanSelect = tp.bCanSelect;
								prop.strBindVar = tp.strBindVar;
								prop.bCanEdit   = tp.bCanEdit;
								prop.nMaxLength = tp.nMaxLength;
								prop.MultilineType = tp.MultilineType;

								bFirst = FALSE;
							}
							else
							{
								if (prop.TextType != tp.TextType)
								{
									dwFlags &= ~TP_TYPE;
								}

								if (prop.bBorder != tp.bBorder)
								{
									dwFlags &= ~TP_BORDER;
								}

								if (prop.bCanSelect != tp.bCanSelect)
								{
									dwFlags &= ~TP_CANSELECT;
								}

								if (stricmp(prop.strBindVar.c_str(), tp.strBindVar.c_str()) != 0)
								{
									dwFlags &= ~TP_BINDVAR;
								}

								if (prop.bCanEdit != tp.bCanEdit)
								{
									dwFlags &= ~TP_CANEDIT;
								}

								if (prop.nMaxLength != tp.nMaxLength)
								{
									dwFlags &= ~TP_MAXLENGTH;
								}

								if (prop.MultilineType != tp.MultilineType)
								{
									dwFlags &= ~TP_MULTITYPE;
								}
							}

							if (!prop.bCanSelect || !prop.bCanEdit)
							{
								dwFlags &= ~TP_MAXLENGTH;
							}
						}
					}
				}
			}

			if (bHaveText) return dwFlags;
		}

		prop = TextProperty1;
		return TP_ALL;
	}
}

template <class _IClass>
CTextBlock *TTextToolExTmpl<_IClass>::FirstTB()
{
	return *(m_ParagraphList[0]->m_TextBlockList.begin());
}

template <class _IClass>
CParagraph *TTextToolExTmpl<_IClass>::FirstLine()
{
	return m_ParagraphList[0];
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ChangeTextType(gldText2_TEXTTYPE type)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(ChangeTextType)(this, type));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY
			LockUpdate();
			LockDraw();

			ExecCommand(new COMMAND_CLSNAME(ChangeTextType)(this, type));

			if (type != TextType_Static)
			{
				pText->m_bounds.SetRect(m_rtRegion.left, 
					m_rtRegion.top, m_rtRegion.right, m_rtRegion.bottom);
			}

			UnlockDraw();
			UnlockUpdate();
			END_MODIFY
		}
	}

	TextProperty1.TextType = type;

#define GETFLAG(v, f) ((v & f) == f)

	TTCAPS caps = GetTextToolCaps();

	// &&&&&&&&&&&&& 取消部分属性 &&&&&&&&&&&&&&&&&&&&&&&&&&
	// 取消字间緛E
	if (!GETFLAG(caps.dwCaps, TTCAPS_CHARSPACING))
	{
		m_DefTextFormat1.SetSpacing(0);
	}

	// 取消上眮E卤丒
	if (!GETFLAG(caps.dwCaps, TTCAPS_LOCATE))
	{
		m_DefTextFormat1.SetLocate(CTextFormat::CL_NONE);
	}

#undef GETFLAG

	Update(NULL);
}

#define BEGIN_MODIFY1 \
	CCharacterKeyList sel = m_pIClass->CurSel(); \
	if (!sel.empty()) \
	{ \
		for (CCharacterKeyList::iterator i = sel.begin(); i != sel.end(); i++) \
		{ \
			if ((*i)->GetObj()->GetGObjType() == gobjText) \
			{ \
				gldText2 *pText = (gldText2 *)((*i)->GetObj()); \
				gldText2 *pNewText = new gldText2; \
				oBinStream out; \
				iBinStream in; \
				pText->WriteToBinStream(out); \
				U8 *p = new U8[out.Size()]; \
				U32 _size = out.Size(); \
				out.WriteToMemory(p); \
				in.ReadFromMemory(out.Size(), p); \
				pNewText->ReadFromBinStream(in);

#define END_MODIFY1 \
	LockSendUpdate(); \
	m_pIClass->ModifyText(pText, pNewText); \
	UnlockSendUpdate(); \
	delete p; }}}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetBorder(BOOL b)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(SetBorder)(this, b));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY1
			pNewText->m_dynaInfo.SetHasBorderFlag(BOOL2bool(b));
			END_MODIFY1
		}
	}

	TextProperty1.bBorder = b;
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetCanSelect(BOOL s)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(CanSelect)(this, s));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY1
			pNewText->m_dynaInfo.SetCanSelectFlag(BOOL2bool(s));
			END_MODIFY1
		}
	}

	TextProperty1.bCanSelect = s;
	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetBindVar(const char *pszVariable)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(SetBindVar)(this, pszVariable));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY1
				pNewText->m_dynaInfo.m_variableName = CA2W(pszVariable);
			END_MODIFY1
		}
	}

	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetCanEdit(BOOL bCanEdit)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(CanEdit)(this, bCanEdit));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY1
				if (bCanEdit)
				{
					pNewText->m_dynaInfo.SetIsReadOnlyFlag(!BOOL2bool(bCanEdit));
					pNewText->m_dynaInfo.SetCanSelectFlag(TRUE);
				}
				else
				{
					pNewText->m_dynaInfo.SetIsReadOnlyFlag(!BOOL2bool(bCanEdit));
				}
			END_MODIFY1
		}
	}

	if (bCanEdit)
	{
		TextProperty1.bCanEdit = bCanEdit;
		TextProperty1.bCanSelect = TRUE;
	}
	else
	{
		TextProperty1.bCanEdit = bCanEdit;
	}

	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetMaxLength(unsigned short nLength)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(SetMaxLength)(this, nLength));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY1
				pNewText->m_dynaInfo.SetHasMaxLengthFlag(nLength > 0);
				pNewText->m_dynaInfo.m_maxLength = nLength;
			END_MODIFY1
		}
	}

	TextProperty1.nMaxLength = nLength;

	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ChangeMultilineType(gldText2 *pText, emMultilineType mt)
{
	switch(mt)
	{
	case Text_Singleline:
		pText->m_dynaInfo.SetHasWordWarpFlag(false);
		pText->m_dynaInfo.SetIsPasswordFlag(false);
		pText->m_dynaInfo.SetIsMultilineFlag(false);
		break;
	case Text_Multiline:
		pText->m_dynaInfo.SetHasWordWarpFlag(true);
		pText->m_dynaInfo.SetIsPasswordFlag(false);
		pText->m_dynaInfo.SetIsMultilineFlag(true);
		break;
	case Text_MultilineNoWrap:
		pText->m_dynaInfo.SetHasWordWarpFlag(false);
		pText->m_dynaInfo.SetIsPasswordFlag(false);
		pText->m_dynaInfo.SetIsMultilineFlag(true);
		break;
	case Text_Password:
		pText->m_dynaInfo.SetHasWordWarpFlag(false);
		pText->m_dynaInfo.SetIsPasswordFlag(true);
		pText->m_dynaInfo.SetIsMultilineFlag(false);
		break;
	}
}

template <class _IClass>
emMultilineType TTextToolExTmpl<_IClass>::GetMultilineType(gldText2 *pText)
{
	if (!pText->m_dynaInfo.HasWordWarp() &&
		!pText->m_dynaInfo.IsMultiline() &&
		!pText->m_dynaInfo.IsPassword())
	{
		return Text_Singleline;
	}

	if (pText->m_dynaInfo.HasWordWarp() &&
		pText->m_dynaInfo.IsMultiline() &&
		!pText->m_dynaInfo.IsPassword())
	{
		return Text_Multiline;
	}

	if (!pText->m_dynaInfo.HasWordWarp() &&
		pText->m_dynaInfo.IsMultiline() &&
		!pText->m_dynaInfo.IsPassword())
	{
		return Text_MultilineNoWrap;
	}

	if (!pText->m_dynaInfo.HasWordWarp() &&
		!pText->m_dynaInfo.IsMultiline() &&
		pText->m_dynaInfo.IsPassword())
	{
		return Text_Password;
	}

	return Text_Unknown;
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetMultilineType(emMultilineType mt)
{
	if (IsActive())
	{
		ExecCommand(new COMMAND_CLSNAME(SetMultilineType)(this, mt));
	}
	else
	{
		if (m_pIClass->CurSel().count() > 0)
		{
			BEGIN_MODIFY1
				ChangeMultilineType(pNewText, mt);
			END_MODIFY1
		}
	}

	TextProperty1.MultilineType = mt;

	Update(NULL);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::WriteFormatToReg()
{
	// text property
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "TextType", (int)TextProperty1.TextType);
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Border", (int)TextProperty1.bBorder);
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "CanSelect", (int)TextProperty1.bCanSelect);
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "CanEdit", (int)TextProperty1.bCanEdit);
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "MaxLength", (int)TextProperty1.nMaxLength);
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "MultilineType", (int)TextProperty1.MultilineType);

	// text format
	AfxGetApp()->WriteProfileString("Tools\\TextEx", "Font face", m_DefTextFormat1.GetFontFace().c_str());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Font size", m_DefTextFormat1.GetFontSize() / 20);
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Font color", (int)m_DefTextFormat1.GetFontColor());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Bold", (int)m_DefTextFormat1.GetBold());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Italic", (int)m_DefTextFormat1.GetItalic());
//	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Locate", (int)m_DefTextFormat1.GetLocate());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Char Spacing", m_DefTextFormat1.GetSpacing());

	// para format
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Indent", m_DefParaFormat1.GetIndent());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "LeftMargin", m_DefParaFormat1.GetMarginLeft());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "RightMargin", m_DefParaFormat1.GetMarginRight());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Align", (int)m_DefParaFormat1.GetAlignType());
	AfxGetApp()->WriteProfileInt("Tools\\TextEx", "Spacing", m_DefParaFormat1.GetSpacing());
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::ReadFormatFromReg()
{
	// text property
	TextProperty1.TextType = (gldText2_TEXTTYPE)AfxGetApp()->GetProfileInt("Tools\\TextEx", "TextType", (int)TextType_Static);
	TextProperty1.bBorder = AfxGetApp()->GetProfileInt("Tools\\TextEx", "Border", 0);
	TextProperty1.bCanSelect = AfxGetApp()->GetProfileInt("Tools\\TextEx", "CanSelect", 0);
	TextProperty1.bCanEdit = AfxGetApp()->GetProfileInt("Tools\\TextEx", "CanEdit", 0);
	TextProperty1.nMaxLength = AfxGetApp()->GetProfileInt("Tools\\TextEx", "MaxLength", 0);
	TextProperty1.MultilineType = (emMultilineType)AfxGetApp()->GetProfileInt("Tools\\TextEx", "MultilineType", (int)Text_Multiline);

	if (TextProperty1.TextType != TextType_Static &&
		TextProperty1.TextType != TextType_Dynamic &&
		TextProperty1.TextType != TextType_HTML)
	{
		TextProperty1.TextType = TextType_Static;
	}

	if (TextProperty1.MultilineType != Text_Singleline &&
		TextProperty1.MultilineType != Text_Multiline &&
		TextProperty1.MultilineType != Text_MultilineNoWrap &&
		TextProperty1.MultilineType != Text_Password)
	{
		TextProperty1.MultilineType = Text_Singleline;
	}

	if (TextProperty1.nMaxLength < 0 || TextProperty1.nMaxLength > 100)
	{
		TextProperty1.nMaxLength = 0;
	}

	// text format
	CString strDefFontFace;
	strDefFontFace.LoadString(IDS_TEXT_TOOL_FONT);

	m_DefTextFormat1.SetFontFace(AfxGetApp()->GetProfileString("Tools\\TextEx", "Font face", strDefFontFace).GetBuffer());
	m_DefTextFormat1.SetFontSize(AfxGetApp()->GetProfileInt("Tools\\TextEx", "Font size", 24) * 20);
	m_DefTextFormat1.SetFontColor(AfxGetApp()->GetProfileInt("Tools\\TextEx", "Font color", 0));
	m_DefTextFormat1.SetBold(AfxGetApp()->GetProfileInt("Tools\\TextEx", "Bold", 0));
	m_DefTextFormat1.SetItalic(AfxGetApp()->GetProfileInt("Tools\\TextEx", "Italic", 0));
//	m_DefTextFormat1.SetLocate((CTextFormat::emCharLocate)AfxGetApp()->GetProfileInt("Tools\\TextEx", "Locate", (int)CTextFormat::CL_NONE));
	m_DefTextFormat1.SetSpacing(AfxGetApp()->GetProfileInt("Tools\\TextEx", "Char Spacing", 0));

	if (TextProperty1.TextType == TextType_Dynamic || 
		TextProperty1.TextType == TextType_HTML)
	{
		m_DefTextFormat1.SetSpacing(0);
		m_DefTextFormat1.SetLocate(CTextFormat::CL_NONE);

		if (!TextProperty1.bCanEdit || !TextProperty1.bCanSelect)
		{
			TextProperty1.nMaxLength = 0;
		}
	}

	if (m_DefTextFormat1.GetFontSize() < 0 || m_DefTextFormat1.GetFontSize() > 2048)
	{
		m_DefTextFormat1.SetFontSize(8);
	}

	if (m_DefTextFormat1.GetSpacing() < -60 || m_DefTextFormat1.GetSpacing() > 60)
	{
		m_DefTextFormat1.SetSpacing(0);
	}

	// para format
	m_DefParaFormat1.SetIndent((short)AfxGetApp()->GetProfileInt("Tools\\TextEx", "Indent", 0));
	m_DefParaFormat1.SetMarginLeft((short)AfxGetApp()->GetProfileInt("Tools\\TextEx", "LeftMargin", 0));
	m_DefParaFormat1.SetMarginRight((short)AfxGetApp()->GetProfileInt("Tools\\TextEx", "RightMargin", 0));
	m_DefParaFormat1.SetAlignType((CParagraph::emAlignType)AfxGetApp()->GetProfileInt("Tools\\TextEx", "Align", (int)CParagraph::AlignLeft));
	m_DefParaFormat1.SetSpacing(AfxGetApp()->GetProfileInt("Tools\\TextEx", "Spacing", 0));

	if (m_DefParaFormat1.GetIndent() < -200 || m_DefParaFormat1.GetIndent() > 200)
	{
		m_DefParaFormat1.SetIndent(0);
	}

	if (m_DefParaFormat1.GetMarginLeft() < 0 || m_DefParaFormat1.GetMarginLeft() > 200)
	{
		m_DefParaFormat1.SetMarginLeft(0);
	}

	if (m_DefParaFormat1.GetMarginRight() < 0 || m_DefParaFormat1.GetMarginRight() > 200)
	{
		m_DefParaFormat1.SetMarginRight(0);
	}

	if (m_DefParaFormat1.GetAlignType() != CParagraph::AlignLeft &&
		m_DefParaFormat1.GetAlignType() != CParagraph::AlignCenter &&
		m_DefParaFormat1.GetAlignType() != CParagraph::AlignRight)
	{
		m_DefParaFormat1.SetAlignType(CParagraph::AlignLeft);
	}

	if (m_DefParaFormat1.GetSpacing() < -360 || m_DefParaFormat1.GetSpacing() > 360)
	{
		m_DefParaFormat1.SetSpacing(0);
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnReposCursor(int x, int y, int &x1, int &y1, int &x2, int &y2, BOOL bReposView)
{
	TMatrix mat = TMatrix(1, 0, 0, 1, m_rtRegion.left, m_rtRegion.top) * 
		teMatrix * _M_matrix.Invert() *
		TMatrix(0.05f, 0, 0, 0.05f, 0, 0);
	int width = Cursor.GetWidth(), height = Cursor.GetHeight();

	int dx = x, dy = y, dx2 = x, dy2 = y + height;

	mat.Transform(dx, dy);
	mat.Transform(dx2, dy2);

	x1 = dx, y1 = dy;
	x2 = dx2, y2 = dy2;

	if (bReposView && nLockUpdate == 0)
	{
		CSize szView = m_pIClass->GetViewSize();
		int sx = 0, sy = 0;

		if (x1 < 20 || x2 < 20)
		{
			sx = min(x1, x2) - 50;
		}

		if (y1 < 0 || y2 < 0)
		{
			sy = min(y1, y2) - 50;
		}

		if (x1 >= szView.cx - 20 || x2 >= szView.cx - 20)
		{
			sx = max(x1 - szView.cx, x2 - szView.cx) + 50;
		}

		if (y2 >= szView.cy - 20 || y2 >= szView.cy - 20)
		{
			sy = max(y1 - szView.cy, y2 - szView.cy) + 50;
		}

		if (sx != 0 || sy != 0)
		{
			m_pIClass->Scroll(sx, sy);
		}
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnSetFocus(HWND hOldWnd)
{
	if (IsActive())
	{
		UpdateAll();
		Cursor.Show(TRUE);
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnKillFocus(HWND hNewWnd)
{
	if (IsActive())
	{
		UpdateAll();
		Cursor.Show(FALSE);
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnLButtonDbClick(UINT nFlags, const POINT &point)
{
	if (bEdit)
	{
		TE_LButtonDbClick(nFlags, point);
	}
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::SetCompositionWindow()
{
	int x = Cursor.GetCursorX(), y = Cursor.GetCursorY();
	TMatrix mat = TMatrix(1, 0, 0, 1, m_rtRegion.left, m_rtRegion.top) * 
		teMatrix * _M_matrix.Invert() *
		TMatrix(0.05f, 0, 0, 0.05f, 0, 0);
	mat.Transform(x, y);

	COMPOSITIONFORM cf;
	memset(&cf, 0, sizeof(cf));
	cf.dwStyle = CFS_FORCE_POSITION | CFS_POINT;

	HKL hkl = GetKeyboardLayout(0);
	HIMC imc = ImmGetContext(_M_hwnd);

	if (LOWORD(hkl) != MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
	{
		cf.ptCurrentPos.x = x;
		cf.ptCurrentPos.y = y - 1;

		ImmSetCompositionWindow(imc, &cf);

		// set font
		LOGFONT font;

		font.lfHeight = -m_DefTextFormat.GetFontSize() / 20;
		font.lfWidth = 0;
		font.lfEscapement = 0;
		font.lfOrientation = 0;
		font.lfWeight = m_DefTextFormat.GetBold() ? FW_BOLD : 0;
		font.lfItalic = m_DefTextFormat.GetItalic() ? 1 : 0;
		font.lfUnderline = 0;
		font.lfStrikeOut = 0;
		font.lfCharSet = DEFAULT_CHARSET;
		font.lfOutPrecision = 0;
		font.lfClipPrecision = 0;
		font.lfQuality = 0;
		font.lfPitchAndFamily = 0;
		strcpy(font.lfFaceName, m_DefTextFormat.GetFontFace().c_str());

		ImmSetCompositionFont(imc, &font);
	}
	else
	{
		cf.ptCurrentPos.x = -100;
		cf.ptCurrentPos.y = -100;

		HIMC imc = ImmGetContext(_M_hwnd);
		ImmSetCompositionWindow(imc, &cf);
	}

	ImmReleaseContext(_M_hwnd, imc);
}

template <class _IClass>
LRESULT TTextToolExTmpl<_IClass>::OnImeComposition(WPARAM wParam, LPARAM lParam)
{
	HKL hkl = GetKeyboardLayout(0);

	if (LOWORD(hkl) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
	{
		if (bEdit)
		{
			SetCompositionWindow();

			if (GCS_RESULTSTR & lParam)
			{
				HIMC imc = ImmGetContext(_M_hwnd);
				char temp[1024];
				memset(temp, 0, 1024);
				int sz = ImmGetCompositionString(imc, GCS_RESULTSTR, temp, 1024);
//				if (sz)
				{
					m_bImeSelChange = FALSE;

					if (!m_bIme)
					{
						DoReplace(MY_A2W(temp));
					}
					else
					{
						ExecAndMerge(new COMMAND_CLSNAME(Replace)((CTextEditBase *)this, MY_A2W(temp)));
					}

					m_bImeSelChange = TRUE;
				}

				m_bIme = FALSE;
				ImmReleaseContext(_M_hwnd, imc);
				return 0;
			}
			else if (GCS_COMPSTR & lParam)
			{
				HIMC imc = ImmGetContext(_M_hwnd);
				char temp[1024];
				memset(temp, 0, 1024);
				int sz = ImmGetCompositionString(imc, GCS_COMPSTR, temp, 1024);

					m_bImeSelChange = FALSE;
					//POINT ptSelStart, ptSelEnd;
					POINT ptSelStart2, ptSelEnd2;
					//GetSel(ptSelStart, ptSelEnd);

					if (!m_bIme)
					{
						DoReplace(MY_A2W(temp));
					}
					else
					{
						ExecAndMerge(new COMMAND_CLSNAME(Replace)((CTextEditBase *)this, MY_A2W(temp)));
					}

					GetSel(ptSelStart2, ptSelEnd2);
					ptSelStart2.x--;
					SetSel(ptSelStart2, ptSelEnd2);
					m_bImeSelChange = TRUE;

				m_bIme = TRUE;
				ImmReleaseContext(_M_hwnd, imc);

				return 0;
			}
		}
	}
	else
	{
		SetCompositionWindow();
	}

	return DefWindowProc(_M_hwnd, WM_IME_COMPOSITION, wParam, lParam);
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnImeNotify(WPARAM wParam, LPARAM lParam)
{
}

template <class _IClass>
void TTextToolExTmpl<_IClass>::OnBeforeSelChange()
{
	if (m_bImeSelChange)
	{
		HIMC imc = ImmGetContext(_M_hwnd);
		if (imc) ImmNotifyIME(imc, NI_COMPOSITIONSTR, CPS_COMPLETE, true);
		ImmReleaseContext(_M_hwnd, imc);
	}
}
#undef DEFINE_HTMLSTYLEYLE