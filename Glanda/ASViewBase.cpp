// SampleView.cpp : implementation of the CASViewBase class
//

#include "stdafx.h"
#include "Resource.h"

#include "ASDocBase.h"
#include "ASViewBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CASViewBase

IMPLEMENT_DYNCREATE(CASViewBase, CCrystalEditView)

BEGIN_MESSAGE_MAP(CASViewBase, CCrystalEditView)
	//{{AFX_MSG_MAP(CASViewBase)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

//	C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR g_szKeywords[] =
{
	("__proto__"),
	("_accProps"),
	("_alpha"),
	("_currentframe"),
	("_droptarget"),
	("_focusrect"),
	("_framesloaded"),
	("_global"),
	("_height"),
	("_highquality"),
	("_level0"),
	("_level1"),
	("_level2"),
	("_level3"),
	("_level4"),
	("_level5"),
	("_level6"),
	("_level7"),
	("_level8"),
	("_level9"),
	("_level10"),
	("_lockroot"),
	("_name"),
	("_parent"),
	("_quality"),
	("_root"),
	("_rotation"),
	("_soundbuftime"),
	("_target"),
	("_totalframes"),
	("_url"),
	("_visible"),
	("_width"),
	("_x"),
	("_xmouse"),
	("_xscale"),
	("_y"),
	("_ymouse"),
	("_yscale"),
	("abs"),
	("Accessibility"),
	("acos"),
	("activityLevel"),
	("add"),
	("addListener"),
	("addPage"),
	("addProperty"),
	("addRequestHeader"),
	("align"),
	("allowDomain"),
	("allowInsecureDomain"),
	("and"),
	("appendChild"),
	("apply"),
	("Arguments"),
	("Array"),
	("asfunction"),
	("asin"),
	("atan"),
	("atan2"),
	("attachAudio"),
	("attachMovie"),
	("attachSound"),
	("attachVideo"),
	("attributes"),
	("autosize"),
	("avHardwareDisable"),
	("background"),
	("backgroundColor"),
	("BACKSPACE"),
	("bandwidth"),
	("beginFill"),
	("beginGradientFill"),
	("blockIndent"),
	("bold"),
	("Boolean"),
	("border"),
	("borderColor"),
	("bottomScroll"),
	("break"),
	("bufferLength"),
	("bufferTime"),
	("builtInItems"),
	("bullet"),
	("Button"),
	("bytesLoaded"),
	("bytesTotal"),
	("call"),
	("callee"),
	("caller"),
	("Camera"),
	("capabilities"),
	("CAPSLOCK"),
	("caption"),
	("case"),
	("catch"),
	("ceil"),
	("charAt"),
	("charCodeAt"),
	("childNodes"),
	("chr"),
	("class"),
	("clear"),
	("clearInterval"),
	("cloneNode"),
	("close"),
	("Color"),
	("concat"),
	("connect"),
	("condenseWhite"),
	("constructor"),
	("contentType"),
	("ContextMenu"),
	("ContextMenuItem"),
	("continue"),
	("CONTROL"),
	("copy"),
	("cos"),
	("createElement"),
	("createEmptyMovieClip"),
	("createTextField"),
	("createTextNode"),
	("currentFps"),
	("curveTo"),
	("CustomActions"),
	("customItems"),
	("data"),
	("Date"),
	("deblocking"),
	("default"),
	("delete"),
	("DELETEKEY"),
	("do"),
	("docTypeDecl"),
	("domain"),
	("DOWN"),
	("duplicateMovieClip"),
	("duration"),
	("dynamic"),
	("E"),
	("else"),
	("else"),
	("embedFonts"),
	("enabled"),
	("END"),
	("endFill"),
	("ENTER"),
	("eq"),
	("Error"),
	("ESCAPE"),
	("escape"),
	("eval"),
	("exactSettings"),
	("exp"),
	("extends"),
	("false"),
	("finally"),
	("findText"),
	("firstChild"),
	("floor"),
	("flush"),
	("focusEnabled"),
	("font"),
	("for"),
	("fps"),
	("fromCharCode"),
	("fscommand"),
	("function"),
	("gain"),
	("ge"),
	("get"),
	("getAscii"),
	("getBeginIndex"),
	("getBounds"),
	("getBytesLoaded"),
	("getBytesTotal"),
	("getCaretIndex"),
	("getCode"),
	("getCount"),
	("getDate"),
	("getDay"),
	("getDepth"),
	("getEndIndex"),
	("getFocus"),
	("getFontList"),
	("getFullYear"),
	("getHours"),
	("getInstanceAtDepth"),
	("getLocal"),
	("getMilliseconds"),
	("getMinutes"),
	("getMonth"),
	("getNewTextFormat"),
	("getNextHighestDepth"),
	("getPan"),
	("getProgress"),
	("getProperty"),
	("getRGB"),
	("getSeconds"),
	("getSelected"),
	("getSelectedText"),
	("getSize"),
	("getStyle"),
	("getStyleNames"),
	("getSWFVersion"),
	("getText"),
	("getTextExtent"),
	("getTextFormat"),
	("getTextSnapshot"),
	("getTime"),
	("getTimer"),
	("getTimezoneOffset"),
	("getTransform"),
	("getURL"),
	("getUTCDate"),
	("getUTCDay"),
	("getUTCFullYear"),
	("getUTCHours"),
	("getUTCMilliseconds"),
	("getUTCMinutes"),
	("getUTCMonth"),
	("getUTCSeconds"),
	("getVersion"),
	("getVolume"),
	("getYear"),
	("globalToLocal"),
	("goto"),
	("gotoAndPlay"),
	("gotoAndStop"),
	("gt"),
	("hasAccessibility"),
	("hasAudio"),
	("hasAudioEncoder"),
	("hasChildNodes"),
	("hasEmbeddedVideo"),
	("hasMP3"),
	("hasPrinting"),
	("hasScreenBroadcast"),
	("hasScreenPlayback"),
	("hasStreamingAudio"),
	("hasStreamingVideo"),
	("hasVideoEncoder"),
	("height"),
	("hide"),
	("hideBuiltInItems"),
	("hitArea"),
	("hitTest"),
	("hitTestTextNearPos"),
	("HOME"),
	("hscroll"),
	("html"),
	("htmlText"),
	("ID3"),
	("if"),
	("ifFrameLoaded"),
	("ignoreWhite"),
	("implements"),
	("import"),
	("in"),
	("indent"),
	("index"),
	("indexOf"),
	("Infinity"),
	("-Infinity"),
	("INSERT"),
	("insertBefore"),
	("install"),
	("instanceof"),
	("int"),
	("interface"),
	("isActive"),
	("isDebugger"),
	("isDown"),
	("isFinite"),
	("isNaN"),
	("isToggled"),
	("italic"),
	("join"),
	("Key"),
	("language"),
	("lastChild"),
	("lastIndexOf"),
	("le"),
	("leading"),
	("LEFT"),
	("leftMargin"),
	("length"),
	("lineStyle"),
	("lineTo"),
	("list"),
	("LN10"),
	("LN2"),
	("load"),
	("loadClip"),
	("loaded"),
	("loadMovie"),
	("loadMovieNum"),
	("loadSound"),
	("loadVariables"),
	("loadVariablesNum"),
	("LoadVars"),
	("LocalConnection"),
	("localFileReadDisable"),
	("localToGlobal"),
	("log"),
	("LOG10E"),
	("LOG2E"),
	("lt"),
	("manufacturer"),
	("Math"),
	("max"),
	("MAX_VALUE"),
	("maxChars"),
	("maxhscroll"),
	("maxscroll"),
	("mbchr"),
	("mblength"),
	("mbord"),
	("mbsubstring"),
	("menu"),
	("message"),
	("Microphone"),
	("min"),
	("MIN_VALUE"),
	("MMExecute"),
	("motionLevel"),
	("motionTimeOut"),
	("Mouse"),
	("mouseWheelEnabled"),
	("moveTo"),
	("MovieClip"),
	("MovieClipLoader"),
	("multiline"),
	("muted"),
	("name"),
	("names"),
	("NaN"),
	("ne"),
	("NEGATIVE_INFINITY"),
	("NetConnection"),
	("NetStream"),
	("new"),
	("newline"),
	("nextFrame"),
	("nextScene"),
	("nextSibling"),
	("nodeName"),
	("nodeType"),
	("nodeValue"),
	("not"),
	("null"),
	("Number"),
	("Object"),
	("on"),
	("onActivity"),
	("onChanged"),
	("onClipEvent"),
	("onClose"),
	("onConnect"),
	("onData"),
	("onDragOut"),
	("onDragOver"),
	("onEnterFrame"),
	("onID3"),
	("onKeyDown"),
	("onKeyUp"),
	("onKillFocus"),
	("onLoad"),
	("onLoadComplete"),
	("onLoadError"),
	("onLoadInit"),
	("onLoadProgress"),
	("onLoadStart"),
	("onMouseDown"),
	("onMouseMove"),
	("onMouseUp"),
	("onMouseWheel"),
	("onPress"),
	("onRelease"),
	("onReleaseOutisde"),
	("onResize"),
	("onRollOut"),
	("onRollOver"),
	("onScroller"),
	("onSelect"),
	("onSetFocus"),
	("onSoundComplete"),
	("onStatus"),
	("onUnload"),
	("onUpdate"),
	("onXML"),
	("or"),
	("ord"),
	("os"),
	("parentNode"),
	("parseCSS"),
	("parseFloat"),
	("parseInt"),
	("parseXML"),
	("password"),
	("pause"),
	("PGDN"),
	("PGUP"),
	("PI"),
	("pixelAspectRatio"),
	("play"),
	("playerType"),
	("pop"),
	("position"),
	("POSITIVE_INFINITY"),
	("pow"),
	("prevFrame"),
	("previousSibling"),
	("prevScene"),
	("print"),
	("printAsBitmap"),
	("printAsBitmapNum"),
	("PrintJob"),
	("printNum"),
	("private"),
	("prototype"),
	("public"),
	("push"),
	("quality"),
	("random"),
	("rate"),
	("registerClass"),
	("removeListener"),
	("removeMovieClip"),
	("removeNode"),
	("removeTextField"),
	("replaceSel"),
	("replaceText"),
	("resolutionX"),
	("resolutionY"),
	("restrict"),
	("return"),
	("reverse"),
	("RIGHT"),
	("rightMargin"),
	("round"),
	("scaleMode"),
	("screenColor"),
	("screenDPI"),
	("screenResolutionX"),
	("screenResolutionY"),
	("scroll"),
	("seek"),
	("selectable"),
	("Selection"),
	("send"),
	("sendAndLoad"),
	("separatorBefore"),
	("serverString"),
	("set"),
	("set"),
	("setBufferTime"),
	("setClipboard"),
	("setDate"),
	("setFocus"),
	("setFullYear"),
	("setGain"),
	("setHours"),
	("setInterval"),
	("setMask"),
	("setMilliseconds"),
	("setMinutes"),
	("setMode"),
	("setMonth"),
	("setMotionLevel"),
	("setNewTextFormat"),
	("setPan"),
	("setProperty"),
	("setQuality"),
	("setRate"),
	("setRGB"),
	("setSeconds"),
	("setSelectColor"),
	("setSelected"),
	("setSelection"),
	("setSilenceLevel"),
	("setStyle"),
	("setTextFormat"),
	("setTime"),
	("setTransform"),
	("setUseEchoSuppression"),
	("setUTCDate"),
	("setUTCFullYear"),
	("setUTCHours"),
	("setUTCMilliseconds"),
	("setUTCMinutes"),
	("setUTCMonth"),
	("setUTCSeconds"),
	("setVolume"),
	("setYear"),
	("SharedObject"),
	("SHIFT"),
	("shift"),
	("show"),
	("showMenu"),
	("showSettings"),
	("silenceLevel"),
	("silenceTimeout"),
	("sin"),
	("size"),
	("slice"),
	("smoothing"),
	("sort"),
	("sortOn"),
	("Sound"),
	("SPACE"),
	("splice"),
	("split"),
	("sqrt"),
	("SQRT1_2"),
	("SQRT2"),
	("Stage"),
	("start"),
	("startDrag"),
	("static"),
	("status"),
	("stop"),
	("stopAllSounds"),
	("stopDrag"),
	("String"),
	("StyleSheet"),
	("styleSheet"),
	("substr"),
	("substring"),
	("super"),
	("swapDepths"),
	("switch"),
	("System"),
	("TAB"),
	("tabChildren"),
	("tabEnabled"),
	("tabIndex"),
	("tabStops"),
	("tan"),
	("target"),
	("targetPath"),
	("tellTarget"),
	("text"),
	("textColor"),
	("TextField"),
	("TextFormat"),
	("textHeight"),
	("TextSnapshot"),
	("textWidth"),
	("this"),
	("throw"),
	("time"),
	("toggleHighQuality"),
	("toLowerCase"),
	("toString"),
	("toUpperCase"),
	("trace"),
	("trackAsMenu"),
	("true"),
	("try"),
	("type"),
	("typeof"),
	("undefined"),
	("underline"),
	("unescape"),
	("uninstall"),
	("unloadClip"),
	("unloadMovie"),
	("unLoadMovieNum"),
	("unshift"),
	("unwatch"),
	("UP"),
	("updateAfterEvent"),
	("updateProperties"),
	("url"),
	("useCodePage"),
	("useEchoSuppression"),
	("useHandCursor"),
	("UTC"),
	("valueOf"),
	("var"),
	("variable"),
	("version"),
	("Video"),
	("visible"),
	("void"),
	("watch"),
	("while"),
	("width"),
	("with"),
	("wordwrap"),
	("XML"),
	("xmlDecl"),
	("XMLNode"),
	("XMLSocket"),
	(NULL)
};

BOOL CASViewBase::IsKeyword(LPCTSTR pszChars, int nLength)
{
	for (int L = 0; g_szKeywords[L] != NULL; L ++)
	{
		if (strncmp(g_szKeywords[L], pszChars, nLength) == 0
				&& g_szKeywords[L][nLength] == 0)
			return TRUE;
	}
	return FALSE;
}

BOOL CASViewBase::IsNumber(LPCTSTR pszChars, int nLength)
{
	if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
	{
		for (int I = 2; I < nLength; I ++)
		{
			if (isdigit(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
										(pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (! isdigit(pszChars[0]))
		return FALSE;
	for (int I = 1; I < nLength; I ++)
	{
		if (! isdigit(pszChars[I]) && pszChars[I] != '+' &&
			pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
			pszChars[I] != 'E')
			return FALSE;
	}
	return TRUE;
}

BOOL CASViewBase::IsVarChar(char c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');
}

#define DEFINE_BLOCK(pos, colorindex)										\
ASSERT((pos) >= 0 && (pos) <= nLength);										\
if (pBuf != NULL)															\
{																			\
	if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){	\
	pBuf[nActualItems].m_nCharPos = (pos);									\
	pBuf[nActualItems].m_nColorIndex = (colorindex);						\
	nActualItems ++;}														\
}

#define COOKIE_PREPROCESSOR		(1L << 1)		// #
#define COOKIE_STRING			(1L << 2)		// " "
#define COOKIE_CHAR				(1L << 3)		// ' '
#define COOKIE_EXT_COMMENT		(1L << 4)		// /* */
#define COOKIE_COMMENT			(1L << 5)		// //

DWORD CASViewBase::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	//if (!m_bEnableColoring)
	//	return 0;

	int nLength = GetLineLength(nLineIndex);
	if (nLength <= 0)
		return dwCookie & COOKIE_EXT_COMMENT;

	LPCTSTR pszChars = GetLineChars(nLineIndex);
	BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
	BOOL bRedefineBlock = TRUE;
	BOOL bDecIndex = FALSE;
	int nIdentBegin = -1;
	int I = 0;
	for ( I = 0;; I++)
	{
		if (bRedefineBlock)
		{
			int nPos = I;
			if (bDecIndex)
				nPos --;
			if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else
			if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else
			if (dwCookie & COOKIE_PREPROCESSOR)
			{
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
			}
			else
			{
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			bRedefineBlock = FALSE;
			bDecIndex = FALSE;
		}

		if (I == nLength)
			break;

		if (dwCookie & COOKIE_COMMENT)
		{
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//
		// Block end test
		//
		//	String end: "
		if (dwCookie & COOKIE_STRING)
		{
			// BUG: "abc\\"def"
			if (pszChars[I] == '"')
			{
				BOOL bEndString = (I == 0);
				if (!bEndString)
				{
					int count = 0;
					int J = I - 1;
					while (J >= 0 && pszChars[J] == '\\')
					{
						count++;
						J--;
					}
					bEndString = (count % 2 == 0);
				}

				if (bEndString)
				{
					dwCookie &= ~COOKIE_STRING;
					bRedefineBlock = TRUE;
				}
			}
			continue;
		}

		//	Char constant '..'
		if (dwCookie & COOKIE_CHAR)
		{
			// BUG: 'abc\\'def'
			if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_CHAR;
				bRedefineBlock = TRUE;
			}
			continue;
		}


		//	Extended comment /*....*/
		if (dwCookie & COOKIE_EXT_COMMENT)
		{
			if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '*')
			{
				dwCookie &= ~COOKIE_EXT_COMMENT;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	Preprocessor directive #....
		if (dwCookie & COOKIE_PREPROCESSOR)
		{
			if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
			{
				DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_EXT_COMMENT;
			}
			continue;
		}

		// Normal text
		if (pszChars[I] == '"')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;
			continue;
		}
		if (pszChars[I] == '\'')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_CHAR;
			continue;
		}
		if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_EXT_COMMENT;
			continue;
		}
		
		if (bFirstChar)
		{
			if (pszChars[I] == '#')
			{
				DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
				dwCookie |= COOKIE_PREPROCESSOR;
				continue;
			}
			if (pszChars[I] != ' ' && pszChars[I] != '\t')
				bFirstChar = FALSE;
		}

		if (pBuf == NULL)
			continue;	//	We don't need to extract keywords,
						//	for faster parsing skip the rest of loop


		if (__isalnum(pszChars[I])	|| 
			pszChars[I] == '_'		|| 
			pszChars[I] == '-'		|| 
			pszChars[I] == '!'		|| 
			pszChars[I] == '@'		|| 
			pszChars[I] == '#'		|| 
			pszChars[I] == '%')
		{
			if (nIdentBegin == -1)
				nIdentBegin = I;
		}
		else
		{
			if (nIdentBegin >= 0)
			{
				if (IsKeyword(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else
				if (IsNumber(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				}
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsKeyword(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else
		if (IsNumber(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
		}
	}

	if (pszChars[nLength - 1] != '\\')
		dwCookie &= COOKIE_EXT_COMMENT;

	return dwCookie;
}

/////////////////////////////////////////////////////////////////////////////
// CASViewBase construction/destruction

CASViewBase::CASViewBase()
{
	// TODO: add construction code here
}

CASViewBase::~CASViewBase()
{
}

/////////////////////////////////////////////////////////////////////////////
// CASViewBase diagnostics

#ifdef _DEBUG
void CASViewBase::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CASViewBase::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

CASDocBase* CASViewBase::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CASDocBase)));
	return (CASDocBase*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CASViewBase message handlers

CCrystalTextBuffer *CASViewBase::LocateTextBuffer()
{
	return GetDocument()->m_pTextBuffer;
}

void CASViewBase::OnInitialUpdate() 
{
	CCrystalEditView::OnInitialUpdate();

	/*
	Japanese:
	font face = "‚l‚r ƒSƒVƒbƒN"
	font size = 9

	English
	font face = "Courier New"
	font size = 9
	*/

	CString strFont, strFontSize;
	strFont.LoadString(IDS_FIXED_FONT);
	strFontSize.LoadString(IDS_FIXED_FONT_SIZE);
	int nFontSize = atoi(strFontSize);
	SetFont(strFont, nFontSize);
}

void CASViewBase::GetText(CString &strText)
{
	strText.Empty();

	CCrystalTextBuffer *pBuf = LocateTextBuffer();
	
	int nLines = pBuf->GetLineCount();
	if (nLines > 0)
	{
		int len = pBuf->GetLineLength(nLines - 1);
		if (nLines > 1 || len > 0)
			pBuf->GetText(0, 0, nLines - 1, len, strText);
	}
}
