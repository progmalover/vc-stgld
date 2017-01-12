#include "stdafx.h"
#include "TextToolEx.h"
#include "itexttool.h"

#include "gldTransAdaptor.h"

#include "Global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SWAPBR(cr) RGB(GetBValue(cr), GetGValue(cr), GetRValue(cr))

///////////////////////////////////////////////////////////////////////////////////////////////////////////

gldText2::gldText2()
{
	bNoConvert = TRUE;
}

gldText2::~gldText2()
{
	Clear();
}

void gldText2::Clear()
{
	for (size_t i = 0; i < m_ParagraphList.size(); i++)
	{
		delete m_ParagraphList[i];
	}
	m_ParagraphList.clear();
}

void gldText2::ClearAll()
{
	ClearGlyphRecords();
	Clear();
    if (m_data != NULL)
    {
        delete m_data;
        m_data = NULL;
        m_dataLen = 0;
    }
}

void gldText2::WriteToBinStream(oBinStream &os)
{
	gldText::WriteToBinStream(os);

	os << rtRegion.left;
	os << rtRegion.right;
	os << rtRegion.top;
	os << rtRegion.bottom;

	// 写所有行
	int linecount = (int)m_ParagraphList.size();

	os << linecount;

	for (int i = 0; i < (int)m_ParagraphList.size(); i++)
	{
		m_ParagraphList[i]->Write(os);
	}
}

void gldText2::ReadFromBinStream(iBinStream &is)
{
	gldText::ReadFromBinStream(is);

	is >> rtRegion.left;
	is >> rtRegion.right;
	is >> rtRegion.top;
	is >> rtRegion.bottom;

	// 读所有行
	int linecount;

	is >> linecount;
	for (int i = 0; i < linecount; i++)
	{
		CParagraph *pPara = new CParagraph(NULL);
		pPara->Read(is);
		m_ParagraphList.push_back(pPara);
	}
}

gldObj *gldText2::Clone()
{
	gldObj *pObj = new gldText2;

	if (pObj == NULL)
	{
		return NULL;
	}

	oBinStream out;
	WriteToBinStream(out);
	
	iBinStream in;
	in.ReadFromMemory(out.Size(), out.Memory());

	pObj->ReadFromBinStream(in);
	return pObj;
}

void gldText2::ConvertFromStatic()
{
	//MY_USES_CONVERSION;
	// 从Record转换
	std::list <gldTextGlyphRecord *>::iterator it = m_glyphRecords.begin();
	int yOffset = 0;
	int nLines = 0;
	BOOL bFirst = TRUE;
	CTextFormat defFmt("Times New Roman", 12, RGB(0, 0, 0), FALSE, FALSE, 0, CTextFormat::CL_NONE, std::string(), std::string());
	CParagraph *pPara = new CParagraph(NULL, TRUE, CParagraph::AlignLeft, 0);

	m_ParagraphList.push_back(pPara);

	if (m_glyphRecords.size() == 0)
	{
		pPara->Append(new CTextBlock(defFmt));
	}

	while(it != m_glyphRecords.end())
	{
		gldTextGlyphRecord *pRec = *it;

		CTextFormat tb;

		tb.SetFontFace(ReplaceFontFace(pRec->m_font->GetFontName()));
		tb.SetFontSize(pRec->m_fontHeight);
		tb.SetFontColor(RGBA(pRec->m_color.red,
			pRec->m_color.green, 
			pRec->m_color.blue,
			pRec->m_color.alpha));
		tb.SetBold(pRec->m_font->IsBold());
		tb.SetItalic(pRec->m_font->IsItalic());
		tb.SetSpacing(0);
		tb.SetLocate(CTextFormat::CL_NONE);

		if (yOffset != pRec->m_yOffset)
		{
			if (nLines > 0)
			{
				// 新建一个文本行
				pPara = new CParagraph(NULL, TRUE, CParagraph::AlignLeft, 0);
				m_ParagraphList.push_back(pPara);
				pPara->Append(new CTextBlock(defFmt));
			}

			yOffset = pRec->m_yOffset;
			nLines++;
		}

		CTextBlock *ptb;
		ptb = new CTextBlock(tb, L"");
		ptb->SetParent(pPara);

		if ((U8)(pRec->m_code >> 8) != 0)
		{
			char str[3] = {(char)((U8)(pRec->m_code >> 8)), (char)((U8)pRec->m_code), 0};
			ptb->SetText(MY_A2W(str));
		}
		else
		{
			char str[2] = {(char)((U8)pRec->m_code), 0};
			ptb->SetText(MY_A2W(str));
		}

		pPara->Append(ptb);

		ptb->CalcHeight();

		it++;
	}	
}

void gldText2::ConvertFromSimpleDynamic()
{
	//MY_USES_CONVERSION;
	CTextFormat tf;

	tf.SetFontFace(ReplaceFontFace(m_dynaInfo.m_font->GetFontName()));
	tf.SetFontSize(m_dynaInfo.m_fontHeight);
	tf.SetFontColor(RGBA(m_dynaInfo.m_textColor.red, 
		m_dynaInfo.m_textColor.green,
		m_dynaInfo.m_textColor.blue,
		m_dynaInfo.m_textColor.alpha));
	tf.SetBold(m_dynaInfo.m_font->IsBold());
	tf.SetItalic(m_dynaInfo.m_font->IsItalic());
	tf.SetSpacing(0);
	tf.SetLocate(CTextFormat::CL_NONE);

	CParagraph::emAlignType at;

	switch(m_dynaInfo.m_align)
	{
	case 0: // left
		at = CParagraph::AlignLeft;
		break;
	case 1: // right
		at = CParagraph::AlignRight;
		break;
	case 2: // center
		at = CParagraph::AlignCenter;
		break;
	default:
		at = CParagraph::AlignLeft;
	}

	CParagraph *pPara = new CParagraph(NULL, TRUE, at, 0, (short)m_dynaInfo.m_leftMargin,
		(short)m_dynaInfo.m_rightMargin, (short)m_dynaInfo.m_indent);
	std::string str;
	const char *p = CW2A(m_dynaInfo.m_initialString.c_str());

	m_ParagraphList.push_back(pPara);
	pPara->Append(new CTextBlock(tf));

	while(*p != '\0')
	{
		if (*p == 0x0d)
		{
			CTextBlock *tb = new CTextBlock(tf);
			tb->SetText(MY_A2W(str.c_str()));
			str.clear();
			pPara->Append(tb);

			pPara = new CParagraph(NULL, TRUE, at, 0, m_dynaInfo.m_leftMargin,
				m_dynaInfo.m_rightMargin, m_dynaInfo.m_indent);
			m_ParagraphList.push_back(pPara);
			pPara->Append(new CTextBlock(tf));
		}
		else
		{
			str += *p;
		}

		p++;
	}

	if (str.size() > 0)
	{
		CTextBlock *tb = new CTextBlock(tf);
		tb->SetText(MY_A2W(str.c_str()));
		str.clear();
		pPara->Append(tb);
	}
}

void gldText2::ConvertFromHTML()
{
	CConvertFromHTML(this);
}

void gldText2::BuildEditInfo()
{
	if (m_ParagraphList.size() > 0)
	{
		return;
	}

	if (m_textType == gtextStatic)
	{
		// 是静态文本
		ConvertFromStatic();
	}
	else if (m_textType == gtextDynamic)
	{
		// 是动态文本
		if (!m_dynaInfo.IsHTML())
		{
			ConvertFromSimpleDynamic();
		}
		else
		{
			// 转换HTML文本
			ConvertFromHTML();
		}
	}

	Paragraphs::iterator ip = m_ParagraphList.begin();
	while(ip != m_ParagraphList.end())
	{
		(*ip)->Optimiz();
		ip++;
	}
}

void gldText2::BuildRecord()
{
	//MY_USES_CONVERSION;
	Paragraphs::iterator ip = m_ParagraphList.begin();

	// 清除
	ClearGlyphRecords();

	// 转换编辑信息到Record
	int l = rtRegion.left, t = rtRegion.top;

	int nDisplayX;
	int nDisplayY = t + BORDER_HEIGHT;

	while(ip != m_ParagraphList.end())
	{
		CParagraph *pParagraph = *ip;
		TextBlocks::iterator it = pParagraph->m_TextBlockList.begin();

		nDisplayX = l + pParagraph->GetDrawX();

		while(it != pParagraph->m_TextBlockList.end())
		{
			CTextBlock *pTextBlock = *it;
			CTextFormat &TextFormat = pTextBlock->TextFormat;
			gldColor color(GetRValue(TextFormat.GetFontColor()), 
				GetGValue(TextFormat.GetFontColor()), 
				GetBValue(TextFormat.GetFontColor()));

			gldFont *font = gldDataKeeper::Instance()->m_objLib->GetFont(TextFormat.GetFontFace().c_str(), 
				BOOL2bool(TextFormat.GetBold()), BOOL2bool(TextFormat.GetItalic()));

			for (int i = 0; i < pTextBlock->GetLength(); i++)
			{
				if (pTextBlock->strText[i] != L'\t')
				{
					int nFontSize = TextFormat.GetFontSize();

					if (TextFormat.GetLocate() != CTextFormat::CL_NONE)
					{
						nFontSize /= 2;
					}

					wchar_t _temp[2] = {pTextBlock->strText[i], 0};
					LPSTR pszA = MY_W2A(_temp);
					U16 _code = _mbsnextc((const unsigned char *)pszA);

					AppendChar(_code, font, color, nDisplayX, (pParagraph->GetBaseLine() - pTextBlock->nAscent) +
						(nDisplayY + pTextBlock->nAscent) -
						(TextFormat.GetLocate() == CTextFormat::CL_SUPERSCRIPT ? TextFormat.GetFontSize() / 2 : 0), nFontSize, 0);
				}

				nDisplayX += pTextBlock->GetCharWidth(i);
			}

			it++;
		}

		nDisplayY += pParagraph->GetHeight();
		ip++;
	}

	// 转换动态文本
	if (m_textType == gtextDynamic)
	{
		CParaFormat pf;

		m_ParagraphList[0]->GetParaFormat(pf);

		// 设置边距，缩进
		m_dynaInfo.m_indent      = pf.GetIndent();
		m_dynaInfo.m_leftMargin  = pf.GetMarginLeft();
		m_dynaInfo.m_rightMargin = pf.GetMarginRight();
		m_dynaInfo.m_leading     = pf.GetSpacing();

		// 设置字体
		CTextFormat tf = (*(m_ParagraphList[0]->m_TextBlockList.begin()))->TextFormat;

		// 设置字体
		m_dynaInfo.m_font = gldDataKeeper::Instance()->m_objLib->GetFont(tf.GetFontFace().c_str(), 
			BOOL2bool(tf.GetBold()), BOOL2bool(tf.GetItalic()));

		if (!m_dynaInfo.IsHTML())
		{
			m_dynaInfo.m_fontHeight = tf.GetFontSize();

			// 设置字体颜色
			m_dynaInfo.m_textColor = gldColor(GetRValue(tf.GetFontColor()), 
				GetGValue(tf.GetFontColor()), 
				GetBValue(tf.GetFontColor()));

			// 设置对齐方式
			switch(pf.GetAlignType())
			{
			case CParagraph::AlignLeft:
				m_dynaInfo.m_align = 0;
				break;
			case CParagraph::AlignCenter:
				m_dynaInfo.m_align = 2;
				break;
			case CParagraph::AlignRight:
				m_dynaInfo.m_align = 1;
				break;
			}

			m_dynaInfo.m_initialString.clear();
			Paragraphs::iterator it = m_ParagraphList.begin();
			while(TRUE)
			{
				m_dynaInfo.m_initialString += (*it)->GetText();
				Paragraphs::iterator it1 = it;
				it1++;

				if (it1 == m_ParagraphList.end())
				{
					break;
				}

				if ((*it1)->IsStiffRet())
				{
					m_dynaInfo.m_initialString += 0x0d;
				}

				it++;
			}

			m_dynaInfo.SetHasTextColorFlag();
			m_dynaInfo.SetHasFontFlag();
			m_dynaInfo.SetIsHTMLFlag(false);
		}
		else
		{
			CConvertToHTML tohtml(this);
			m_dynaInfo.SetIsHTMLFlag();
			m_dynaInfo.SetHasTextColorFlag(false);
			m_dynaInfo.SetHasFontFlag(true);
			m_dynaInfo.SetHasLayoutFlag(false);
		}

		m_dynaInfo.SetHasTextFlag();
		m_dynaInfo.SetHasLayoutFlag(true);
	}
}

std::string gldText2::ReplaceFontFace(const std::string fontface)
{
	for (STRING_MAP::iterator it = rpl_fontmap.begin(); it != rpl_fontmap.end(); it++)
	{
		if (stricmp(fontface.c_str(), it->first.c_str()) == 0)
		{
			return it->second.c_str();
		}

	}

	return fontface;
}

void gldText2::GetUsedFonts(STRING_SET &fontList)
{
	for (Paragraphs::iterator it = m_ParagraphList.begin(); it != m_ParagraphList.end(); it++)
	{
		CParagraph *para = *it;
		TextBlocks::iterator it_tb = para->m_TextBlockList.begin();

		while(it_tb != para->m_TextBlockList.end())
		{
			CTextBlock *tb = *it_tb;
			fontList.insert(tb->TextFormat.GetFontFace());
			it_tb++;
		}
	}
}

gldText2::CConvertFromHTML::CConvertFromHTML(gldText2 *pGldText2)
{
	//MY_USES_CONVERSION;

	// 开始转换
	CPasteHTML Paste(pGldText2->m_dynaInfo.m_initialString.c_str());
	CPasteHTML::SEG seg;
	CTextFormat defFmt("Times New Roman", 256, RGB(0, 0, 0), FALSE, FALSE, 0, CTextFormat::CL_NONE, "", "");

	FONT_TAG defFont;
	ANCHOR_TAG defAnchor;
	nBold = nItalic = nUnderLine = 0;

	strncpy(CW2A(defFont.strFontFace), "Times New Roman", 256);
	defFont.nFontSize   = 12;
	defFont.FontColor   = RGB(0, 0, 0);

	defAnchor.strHref[0] = NULL;
	defAnchor.strTarget[0] = NULL;

	if (pGldText2->m_dynaInfo.HasFont())
	{
		defFmt.SetFontFace(pGldText2->ReplaceFontFace(pGldText2->m_dynaInfo.m_font->GetFontName()));
		defFmt.SetBold(pGldText2->m_dynaInfo.m_font->IsBold());
		defFmt.SetItalic(pGldText2->m_dynaInfo.m_font->IsItalic());
		defFmt.SetFontSize(pGldText2->m_dynaInfo.m_fontHeight);

		strncpy(CW2A(defFont.strFontFace), pGldText2->m_dynaInfo.m_font->GetFontName().c_str(), 256);

		if (pGldText2->m_dynaInfo.m_font->IsBold())
		{
			nBold++;
		}

		if (pGldText2->m_dynaInfo.m_font->IsItalic())
		{
			nItalic++;
		}

		defFont.nFontSize = pGldText2->m_dynaInfo.m_fontHeight / 20;
	}

	if (pGldText2->m_dynaInfo.HasTextColor())
	{
		defFmt.SetFontColor(RGB(pGldText2->m_dynaInfo.m_textColor.red, 
			pGldText2->m_dynaInfo.m_textColor.green,
			pGldText2->m_dynaInfo.m_textColor.blue));

		defFont.FontColor = defFmt.GetFontColor();
	}

	// 设置默认样式

	PushFont(defFont);
	PushAnchor(defAnchor);

	while(Paste.GetSeg(seg))
	{
		if (seg.type == CPasteHTML::htmlTAG)
		{
			// 字体标签
			if (std::string(CW2A(seg.strText.c_str())) == "FONT")
			{
				FONT_TAG font;

				wcscpy(font.strFontFace, TopFont().strFontFace);
				font.nFontSize = TopFont().nFontSize;
				font.FontColor = TopFont().FontColor;

				if (seg.GetAttribute(CA2W("FACE")).length())
				{
					wcsncpy(font.strFontFace, seg.GetAttribute(CA2W("FACE")).c_str(), 256);
				}

				if (seg.GetAttribute(CA2W("SIZE")).length())
				{
					font.nFontSize = _wtoi(seg.GetAttribute(CA2W("SIZE")).c_str());
				}

				if (seg.GetAttribute(CA2W("COLOR")).length())
				{
					font.FontColor = CPasteHTML::StringToColor(seg.GetAttribute(CA2W("COLOR")));
				}

				PushFont(font);
			}
			else if (seg.strText == std::wstring(CA2W("/FONT")))
			{
				PopFont();
			}
			// 字体加粗
			else if (seg.strText ==  std::wstring(CA2W("B")))
			{
				nBold++;
			}
			else if (seg.strText ==  std::wstring(CA2W("/B")))
			{
				nBold--;
			}
			// 字体倾斜
			else if (seg.strText == std::wstring( CA2W("I")))
			{
				nItalic++;
			}
			else if (seg.strText ==  std::wstring(CA2W("/I")))
			{
				nItalic--;
			}
			// 下划线
			else if (seg.strText == std::wstring( CA2W("U")))
			{
				nUnderLine++;
			}
			else if (seg.strText ==  std::wstring(CA2W("/U")))
			{
				nUnderLine--;
			}
			// 链接
			else if (seg.strText ==  std::wstring(CA2W("A")))
			{
				ANCHOR_TAG anchor;

				wcsncpy(anchor.strHref, seg.GetAttribute(CA2W("HREF")).c_str(), 256);
				wcsncpy(anchor.strTarget, seg.GetAttribute(CA2W("TARGET")).c_str(), 256);

				PushAnchor(anchor);
			}
			else if (seg.strText ==  std::wstring(CA2W("/A")))
			{
				PopAnchor();
			}
			// 段落
			else if (seg.strText ==  std::wstring(CA2W("P")))
			{
				CParagraph::emAlignType at;

				if (seg.GetAttribute(CA2W("ALIGN")).length())
				{
					if (seg.GetAttribute(CA2W("ALIGN")) == std::wstring(CA2W("LEFT")))
					{
						at = CParagraph::AlignLeft;
					}
					else if (seg.GetAttribute(CA2W("ALIGN")) == std::wstring(CA2W("CENTER")))
					{
						at = CParagraph::AlignCenter;
					}
					else if (seg.GetAttribute(CA2W("ALIGN")) == std::wstring(CA2W("RIGHT")))
					{
						at = CParagraph::AlignRight;
					}
					else
					{
						at = CParagraph::AlignLeft;
					}
				}
				else
				{
					if (pGldText2->m_ParagraphList.size() > 0)
					{
						at = pGldText2->m_ParagraphList[pGldText2->m_ParagraphList.size() - 1]->GetAlignType();
					}
					else
					{
						at = CParagraph::AlignLeft;
					}
				}

				CParagraph *pPara = new CParagraph(NULL, TRUE, at, 0, (short)pGldText2->m_dynaInfo.m_leftMargin,
					(short)pGldText2->m_dynaInfo.m_rightMargin, (short)pGldText2->m_dynaInfo.m_indent);
				pGldText2->m_ParagraphList.push_back(pPara);
				pPara->Append(new CTextBlock(defFmt));
			}
			else if (seg.strText == std::wstring(CA2W("/P")))
			{
				// ...
			}
			else if (seg.strText == std::wstring(CA2W("SBR")) || seg.strText == std::wstring(CA2W("SBR/")))
			{
				//pGldText2->m_dynaInfo.SetHasWordWarpFlag(true);

				if (pGldText2->m_ParagraphList.size() == 0)
				{
					CParagraph *pPara = new CParagraph(NULL, TRUE, CParagraph::AlignLeft, 0, (short)pGldText2->m_dynaInfo.m_leftMargin,
						(short)pGldText2->m_dynaInfo.m_rightMargin, (short)pGldText2->m_dynaInfo.m_indent);
					pGldText2->m_ParagraphList.push_back(pPara);
					pPara->Append(new CTextBlock(defFmt));
				}
				else
				{
					CParagraph::emAlignType at = (*pGldText2->m_ParagraphList.rbegin())->GetAlignType();
					CParagraph *pPara = new CParagraph(NULL, TRUE, at, 0, (short)pGldText2->m_dynaInfo.m_leftMargin,
						(short)pGldText2->m_dynaInfo.m_rightMargin, (short)pGldText2->m_dynaInfo.m_indent);
					pGldText2->m_ParagraphList.push_back(pPara);
					pPara->Append(new CTextBlock(defFmt));
				}
			}
		}
		else if (seg.type == CPasteHTML::htmlTEXT)
		{
		//	assert(pGldText2->m_ParagraphList.size() > 0);
			if (pGldText2->m_ParagraphList.size() == 0)
			{
				CParagraph *pPara = new CParagraph(NULL, TRUE, CParagraph::AlignLeft, 0, (short)pGldText2->m_dynaInfo.m_leftMargin,
					(short)pGldText2->m_dynaInfo.m_rightMargin, (short)pGldText2->m_dynaInfo.m_indent);
				pGldText2->m_ParagraphList.push_back(pPara);
				pPara->Append(new CTextBlock(defFmt));
			}

			CTextFormat tf;

			tf.SetFontFace(pGldText2->ReplaceFontFace(std::string(CW2A(TopFont().strFontFace))));
			tf.SetFontSize(TopFont().nFontSize * 20);
			tf.SetFontColor(TopFont().FontColor);
			tf.SetBold(nBold > 0);
			tf.SetItalic(nItalic > 0);
			// tf.SetUnderLine(nUnderLine > 0); 下划线好像还不支持
			tf.SetSpacing(0);
			tf.SetLocate(CTextFormat::CL_NONE);
			tf.SetLink(std::string(CW2A(TopAnchor().strHref)));
			tf.SetTarget(std::string(CW2A(TopAnchor().strTarget)));

			defFmt = tf;

			CTextBlock *ptb = new CTextBlock(tf, seg.strText.c_str());
			pGldText2->m_ParagraphList[pGldText2->m_ParagraphList.size() - 1]->Append(ptb);
		}
	}

	if (pGldText2->m_ParagraphList.size() == 0)
	{
		CParagraph *pPara = new CParagraph(NULL, TRUE, CParagraph::AlignLeft, 0, (short)pGldText2->m_dynaInfo.m_leftMargin,
			(short)pGldText2->m_dynaInfo.m_rightMargin, (short)pGldText2->m_dynaInfo.m_indent);
		pGldText2->m_ParagraphList.push_back(pPara);
		pPara->Append(new CTextBlock(defFmt));
	}
}

gldText2::CConvertToHTML::CConvertToHTML(gldText2 *pGldText2)
{
	//MY_USES_CONVERSION;

	Paragraphs::iterator it_para = pGldText2->m_ParagraphList.begin();
	CGenHTML gen;

	while(it_para != pGldText2->m_ParagraphList.end())
	{
		BOOL bFirstStiff = TRUE;
		CParagraph *pPara = *it_para;
		std::string strAlign;

		// 生成一个段落
		switch(pPara->GetAlignType())
		{
		case CParagraph::AlignLeft:
			strAlign = "LEFT";
			break;
		case CParagraph::AlignCenter:
			strAlign = "CENTER";
			break;
		case CParagraph::AlignRight:
			strAlign = "RIGHT";
			break;
		default:
			assert(FALSE);
		}

		gen.Para(CA2W(strAlign.c_str()));

		while(it_para != pGldText2->m_ParagraphList.end())
		{
			pPara = *it_para;

            if (pPara->IsStiffRet())
			{
				if (bFirstStiff)
				{
					bFirstStiff = FALSE;
				}
				else
				{
					break;
				}
			}

			// 生成每行的HTML
			TextBlocks::iterator it_tb = pPara->m_TextBlockList.begin();

			while(it_tb != pPara->m_TextBlockList.end())
			{
				CTextBlock *ptb = *it_tb;
				CTextFormat *ptf = &ptb->TextFormat;

				gen.Font(CA2W(ptf->GetFontFace().c_str()), ptf->GetFontSize() / 20, SWAPBR(ptf->GetFontColor()));
				if (ptf->GetBold()) gen.Bold();
				if (ptf->GetItalic()) gen.Italic();
				if (ptf->GetLink().length())
				{
					gen.Anchor(CA2W(ptf->GetLink().c_str()), CA2W(ptf->GetTarget().c_str()));
				}

				gen.Text(ptb->strText.c_str());

				if (ptf->GetLink().length())
				{
					gen.AnchorEnd();
				}
				if (ptf->GetItalic()) gen.ItalicEnd();
				if (ptf->GetBold()) gen.BoldEnd();

				it_tb++;
			}

			// </FONT>
			while(gen.stackFont.size()) gen.FontEnd();

			it_para++;
// If the it_para reach m_ParagraphList.end(), the assign will cause unpredictable error.
// Move this assign to the beginning of the while loop.
//			pPara = *it_para;
// Modified by Tang Hongbo.
		}

		gen.ParaEnd();
	}

	pGldText2->m_dynaInfo.m_initialString = gen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SHIFT_DOWN() (GetKeyState(VK_SHIFT) & (1 << (sizeof(short int) * 8 - 1)))
#define CTRL_DOWN() (GetKeyState(VK_CONTROL) & (1 << (sizeof(short int) * 8 - 1)))
#define ALT_DOWN() (GetKeyState(VK_MENU) & (1 << (sizeof(short int) * 8 - 1)))

CTextFormat::CTextFormat()
{
}

CTextFormat::CTextFormat(const char *lpszFontFace, int nFontSize, COLORREF crColor,
						 BOOL bBold, BOOL bItalic, int nSpacing, emCharLocate Locate,
						 std::string strLink, std::string strTarget)
{
	SetFontFace(lpszFontFace);
	SetFontSize(nFontSize);
	SetFontColor(crColor);
	SetBold(bBold);
	SetItalic(bItalic);
	SetSpacing(nSpacing);
	SetLocate(Locate);
	SetLink(strLink);
	SetTarget(strTarget);
}

CTextFormat::CTextFormat(CTextFormat &TextFormat)
{
	*this = TextFormat;
}

CTextFormat &CTextFormat::operator =(CTextFormat &TextFormat)
{
	SetFontFace(TextFormat.GetFontFace());
	SetFontSize(TextFormat.GetFontSize());
	SetFontColor(TextFormat.GetFontColor());
	SetBold(TextFormat.GetBold());
	SetItalic(TextFormat.GetItalic());
	SetSpacing(TextFormat.GetSpacing());
	SetLocate(TextFormat.GetLocate());
	SetLink(TextFormat.GetLink());
	SetTarget(TextFormat.GetTarget());

	return *this;
}

BOOL CTextFormat::operator ==(CTextFormat &TextFormat)
{
	return (GetFontFace() == TextFormat.GetFontFace() &&
		GetFontSize() == TextFormat.GetFontSize() &&
		GetFontColor() == TextFormat.GetFontColor() &&
		GetBold() == TextFormat.GetBold() &&
		GetItalic() == TextFormat.GetItalic() &&
		GetSpacing() == TextFormat.GetSpacing() &&
		GetLocate() == TextFormat.GetLocate() &&
		GetLink() == TextFormat.GetLink() &&
		GetTarget() == TextFormat.GetTarget());
}

DWORD CTextFormat::Compare(CTextFormat &TextFormat, DWORD dwFlag/* = TF_ALL*/)
{
	if (dwFlag & TF_FONTFACE)
	{
		if (GetFontFace() != TextFormat.GetFontFace())
		{
			dwFlag &= ~TF_FONTFACE;
		}
	}

	if (dwFlag & TF_FONTSIZE)
	{
		if (GetFontSize() != TextFormat.GetFontSize())
		{
			dwFlag &= ~TF_FONTSIZE;
		}
	}

	if (dwFlag & TF_FONTCOLOR)
	{
		if (GetFontColor() != TextFormat.GetFontColor())
		{
			dwFlag &= ~TF_FONTCOLOR;
		}
	}

	if (dwFlag & TF_BOLD)
	{
		if (GetBold() != TextFormat.GetBold())
		{
			dwFlag &= ~TF_BOLD;
		}
	}

	if (dwFlag & TF_ITALIC)
	{
		if (GetItalic() != TextFormat.GetItalic())
		{
			dwFlag &= ~TF_ITALIC;
		}
	}

	if (dwFlag & TF_SPACING)
	{
		if (GetSpacing() != TextFormat.GetSpacing())
		{
			dwFlag &= ~TF_SPACING;
		}
	}

	if (dwFlag & TF_LOCATE)
	{
		if (GetLocate() != TextFormat.GetLocate())
		{
			dwFlag &= ~TF_LOCATE;
		}
	}

	if (dwFlag & TF_LINK)
	{
		if (GetLink() != TextFormat.GetLink())
		{
			dwFlag &= ~TF_LINK;
		}
	}

	if (dwFlag & TF_TARGET)
	{
		if (GetTarget() != TextFormat.GetTarget())
		{
			dwFlag &= ~TF_TARGET;
		}
	}

	return dwFlag;
}

DWORD CTextFormat::Compare(CTextFormat &TextFormat, DWORD dwFlag1/* = TF_ALL*/, DWORD dwFlag2/* = TF_ALL*/)
{
	if (dwFlag2 & TF_FONTFACE)
	{
		if (dwFlag1 & TF_FONTFACE)
		{
			if (GetFontFace() != TextFormat.GetFontFace())
			{
				dwFlag2 &= ~TF_FONTFACE;
			}
		}
		else
		{
			dwFlag2 &= ~TF_FONTFACE;
		}
	}

	if (dwFlag2 & TF_FONTSIZE)
	{
		if (dwFlag1 & TF_FONTSIZE)
		{
			if (GetFontSize() != TextFormat.GetFontSize())
			{
				dwFlag2 &= ~TF_FONTSIZE;
			}
		}
		else
		{
			dwFlag2 &= ~TF_FONTSIZE;
		}
	}

	if (dwFlag2 & TF_FONTCOLOR)
	{
		if (dwFlag1 & TF_FONTCOLOR)
		{
			if (GetFontColor() != TextFormat.GetFontColor())
			{
				dwFlag2 &= ~TF_FONTCOLOR;
			}
		}
		else
		{
			dwFlag2 &= ~TF_FONTCOLOR;
		}
	}

	if (dwFlag2 & TF_BOLD)
	{
		if (dwFlag1 & TF_BOLD)
		{
			if (GetBold() != TextFormat.GetBold())
			{
				dwFlag2 &= ~TF_BOLD;
			}
		}
		else
		{
			dwFlag2 &= ~TF_BOLD;
		}
	}

	if (dwFlag2 & TF_ITALIC)
	{
		if (dwFlag1 & TF_ITALIC)
		{
			if (GetItalic() != TextFormat.GetItalic())
			{
				dwFlag2 &= ~TF_ITALIC;
			}
		}
		else
		{
			dwFlag2 &= ~TF_ITALIC;
		}
	}

	if (dwFlag2 & TF_SPACING)
	{
		if (dwFlag1 & TF_SPACING)
		{
			if (GetSpacing() != TextFormat.GetSpacing())
			{
				dwFlag2 &= ~TF_SPACING;
			}
		}
		else
		{
			dwFlag2 &= ~TF_SPACING;
		}
	}

	if (dwFlag2 & TF_LOCATE)
	{
		if (dwFlag1 & TF_LOCATE)
		{
			if (GetLocate() != TextFormat.GetLocate())
			{
				dwFlag2 &= ~TF_LOCATE;
			}
		}
		else
		{
			dwFlag2 &= ~TF_LOCATE;
		}
	}

	if (dwFlag2 & TF_LINK)
	{
		if (dwFlag1 & TF_LINK)
		{
			if (GetLink() != TextFormat.GetLink())
			{
				dwFlag2 &= ~TF_LINK;
			}
		}
		else
		{
			dwFlag2 &= ~TF_LINK;
		}
	}

	if (dwFlag2 & TF_TARGET)
	{
		if (dwFlag1 & TF_LINK)
		{
			if (GetTarget() != TextFormat.GetTarget())
			{
				dwFlag2 &= ~TF_TARGET;
			}
		}
		else
		{
			dwFlag2 &= ~TF_TARGET;
		}
	}

	return dwFlag2;
}

void CTextFormat::ChangeFormat(CTextFormat &TextFormat, DWORD dwFlag/* = TF_ALL*/)
{
	if (dwFlag & TF_FONTFACE)
	{
		SetFontFace(TextFormat.GetFontFace());
	}

	if (dwFlag & TF_FONTSIZE)
	{
		SetFontSize(TextFormat.GetFontSize());
	}

	if (dwFlag & TF_FONTCOLOR)
	{
		SetFontColor(TextFormat.GetFontColor());
	}

	if (dwFlag & TF_BOLD)
	{
		SetBold(TextFormat.GetBold());
	}

	if (dwFlag & TF_ITALIC)
	{
		SetItalic(TextFormat.GetItalic());
	}

	if (dwFlag & TF_SPACING)
	{
		SetSpacing(TextFormat.GetSpacing());
	}

	if (dwFlag & TF_LOCATE)
	{
		SetLocate(TextFormat.GetLocate());
	}

	if (dwFlag & TF_LINK)
	{
		SetLink(TextFormat.GetLink());
	}

	if (dwFlag & TF_TARGET)
	{
		if (GetLink().length())
		{
			// 只有设置链接地址后才能设置链接目标
			SetTarget(TextFormat.GetTarget());
		}
	}
}

void CTextFormat::Write(oBinStream &out)
{
	int t;

	out << m_FontFace;
	out << m_FontSize;
	out << m_FontColor;
	out << m_Bold;
	out << m_Italic;
	out << m_Spacing;
	t = (int)m_Locate;
	out << t;
	out << m_Link;
	out << m_Target;
}

void CTextFormat::Read(iBinStream &in)
{
	int t;

	in >> m_FontFace;
	in >> m_FontSize;
	in >> m_FontColor;
	in >> m_Bold;
	in >> m_Italic;
	in >> m_Spacing;
	in >> t;
	m_Locate = (CTextFormat::emCharLocate)t;
	in >> m_Link;
	in >> m_Target;
}

BOOL CTextFormat::IsSame(CTextFormat &TextFormat)
{
	return (GetBold() == TextFormat.GetBold() &&
		GetItalic() == TextFormat.GetItalic() &&
		GetFontSize() == TextFormat.GetFontSize() &&
		GetFontFace() == TextFormat.GetFontFace() &&
		GetLocate() == TextFormat.GetLocate());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int CTextBlock::GetLength()
{
	return (int)strText.length();
}

void CTextBlock::Break(int nIndex, CTextBlock **pBefore, CTextBlock **pAfter)
{
	*pBefore = *pAfter = NULL;

	std::wstring strLeft, strRight;

	strLeft.assign(strText.begin(), strText.begin() + nIndex);

	if (strRight.length() + nIndex <  strText.length())
	{
		strRight.assign(strText.begin() + nIndex, strText.end());
	}

	if (!strLeft.empty())
	{
		*pBefore = new CTextBlock(TextFormat, strLeft.c_str());
	}

	if (!strRight.empty())
	{
		*pAfter = new CTextBlock(TextFormat, strRight.c_str());
	}
}

void CTextBlock::SetParent(CParagraph *pParent)
{
	pParentParagraph = pParent;
	CalcWidth();
	CalcHeight();
}

void CTextBlock::CalcWidth()
{
	if (pParentParagraph != NULL)
	{
		if (pParentParagraph->GetEditBase() != NULL)
		{
			nWidth = 0;
			CharWidth.clear();

			TextBlocks::iterator it = find(pParentParagraph->m_TextBlockList.begin(),
				pParentParagraph->m_TextBlockList.end(), this), it_next, it_prev;

			assert(it != pParentParagraph->m_TextBlockList.end());

			it_next = it;
			it_next++;

			while(it_next != pParentParagraph->m_TextBlockList.end())
			{
				if ((*it_next)->GetLength())
				{
					break;
				}

				it_next++;
			}

			if (it != pParentParagraph->m_TextBlockList.begin())
			{
				it_prev = it;
				it_prev--;

				while(it_prev != pParentParagraph->m_TextBlockList.begin())
				{
					if ((*it_prev)->GetLength())
					{
						break;
					}

					it_prev--;
				}

				if ((*it_prev)->GetLength() == 0)
				{
					it_prev = pParentParagraph->m_TextBlockList.end();
				}
			}
			else
			{
				it_prev = pParentParagraph->m_TextBlockList.end();
			}

			CTextBlock *pTBNext, *pTBPrev;

			for (size_t i = 0; i < strText.length(); i++)
			{
				if (i == 0 && it_prev != pParentParagraph->m_TextBlockList.end())
				{
					pTBPrev = *it_prev;

					if (pTBPrev->GetLength() > 0)
					{
						if (pTBPrev->TextFormat.IsSame(TextFormat))
						{
							int w = pParentParagraph->GetEditBase()->GetCharWidth(pTBPrev->strText[pTBPrev->GetLength() - 1],
								strText[i], TextFormat) + TextFormat.GetSpacing();
							if (w < 0) w = 0;

							pTBPrev->CharWidth[pTBPrev->GetLength() - 1] = w;
							pTBPrev->CalcWidth2();
						}
						else
						{
							int w = pParentParagraph->GetEditBase()->GetCharWidth(pTBPrev->strText[pTBPrev->GetLength() - 1],
								0, pTBPrev->TextFormat) + pTBPrev->TextFormat.GetSpacing();
							if (w < 0) w = 0;

							pTBPrev->CharWidth[pTBPrev->GetLength() - 1] = w;
							pTBPrev->CalcWidth2();
						}
					}
				}
				else if (i == strText.length() - 1 && it_next != pParentParagraph->m_TextBlockList.end())
				{
					pTBNext = *it_next;

					if (pTBNext->GetLength() > 0)
					{
						if (pTBNext->TextFormat.IsSame(TextFormat))
						{
							int w = pParentParagraph->GetEditBase()->GetCharWidth(strText[i],
								pTBNext->strText[0], TextFormat) + TextFormat.GetSpacing();
							CharWidth.push_back(w);
							if (w < 0) w = 0;
							nWidth += w;
							continue;
						}
						else
						{
							int w = pParentParagraph->GetEditBase()->GetCharWidth(strText[i],
								0, TextFormat) + TextFormat.GetSpacing();
							CharWidth.push_back(w);
							if (w < 0) w = 0;
							nWidth += w;
							continue;
						}
					}
				}

				int w = pParentParagraph->GetEditBase()->GetCharWidth(strText[i],
					(i + 1 < (int)strText.length()) ? strText[i + 1] : 0, TextFormat) + TextFormat.GetSpacing();
				if (w < 0) w = 0;
				CharWidth.push_back(w);
				nWidth += w;
			}
		}
	}
}

void CTextBlock::CalcWidth2()
{
	nWidth = 0;

	for (size_t i = 0; i < CharWidth.size(); i++)
	{
		nWidth += CharWidth[i];
	}
}

void CTextBlock::CalcHeight()
{
	if (pParentParagraph != NULL)
	{
		if (pParentParagraph->GetEditBase() != NULL)
		{
			pParentParagraph->GetEditBase()->GetFontHeight(TextFormat, nHeight, nAscent);
		}
	}
}

int CTextBlock::GetCharWidth(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)CharWidth.size())
	{
		return CharWidth[nIndex];
	}
	else
	{
		return 0;
	}
}

void CTextBlock::ChangeFormat(CTextFormat &TextFormat, DWORD dwFlag/* = TF_ALL*/)
{
	this->TextFormat.ChangeFormat(TextFormat, dwFlag);

	CalcWidth();
	CalcHeight();
}

int CTextBlock::TryChangeFormat(CTextFormat &TextFormat, DWORD dwFlag/* = TF_ALL*/)
{
	CTextFormat t = this->TextFormat;

	if (dwFlag & TF_FONTFACE)
	{
		t.SetFontFace(TextFormat.GetFontFace());
	}

	if (dwFlag & TF_FONTSIZE)
	{
		t.SetFontSize(TextFormat.GetFontSize());
	}

	if (dwFlag & TF_FONTCOLOR)
	{
		t.SetFontColor(TextFormat.GetFontColor());
	}

	if (dwFlag & TF_BOLD)
	{
		t.SetBold(TextFormat.GetBold());
	}

	if (dwFlag & TF_ITALIC)
	{
		t.SetItalic(TextFormat.GetItalic());
	}

	if (dwFlag & TF_SPACING)
	{
		t.SetSpacing(TextFormat.GetSpacing());
	}

	if (dwFlag & TF_LOCATE)
	{
		t.SetLocate(TextFormat.GetLocate());
	}

	if (dwFlag & TF_LINK)
	{
		t.SetLink(TextFormat.GetLink());
	}

	if (dwFlag & TF_TARGET)
	{
		t.SetTarget(TextFormat.GetTarget());
	}

	int nMaxWidth = -1;
	for (size_t i = 0; i < strText.length(); i++)
	{
		nMaxWidth = max(nMaxWidth, pParentParagraph->GetEditBase()->GetCharWidth(strText[i], 0, t));
	}

	return nMaxWidth;
}

void CTextBlock::Write(oBinStream &out)
{
	//MY_USES_CONVERSION;

	char *t = MY_W2A(strText.c_str());
	std::string str = t;

	TextFormat.Write(out);
	out << str;
}

void CTextBlock::Read(iBinStream &in)
{
	//MY_USES_CONVERSION;

	std::string str;

	TextFormat.Read(in);
	in >> str;
	strText = MY_A2W(str.c_str());
}

CTextBlock *CTextBlock::GetSub(int nStart, int nEnd)
{
	//MY_USES_CONVERSION;

	 if (nStart < 0)
	 {
		 nStart = 0;
	 }

	 if (nEnd > GetLength())
	 {
		 nEnd = GetLength();
	 }

	 if (nEnd == nStart)
	 {
		 return NULL;
	 }

	 std::wstring str;
	 str.assign(strText.begin() + nStart, strText.begin() + nEnd);

	 return new CTextBlock(TextFormat, str.c_str());
}

// ######################################################################################### //

CParagraph::CParagraph(CTextEditBase *pEditBase, BOOL bStiffRet/* = TRUE*/, emAlignType at/* = AlignLeft*/, int nLineSpacing/* = 0*/,
					   int nMarginLeft, int nMarginRight, int nIndent)
{
	this->pEditBase = pEditBase;
	this->bStiffRet = bStiffRet;
	this->AlignType = at;
	SetSpacing(nLineSpacing);
	SetMarginLeft(nMarginLeft);
	SetMarginRight(nMarginRight);
	SetIndent(nIndent);
}

CParagraph::~CParagraph()
{
	TextBlocks::iterator i = m_TextBlockList.begin();

	while(i != m_TextBlockList.end())
	{
		delete *i;
		i++;
	}
}

// 获取段落内得文本块总数
int CParagraph::GetTextBlockCount()
{
	return (int)m_TextBlockList.size(); 
}

// 获取段落内得第一个文本块
CTextBlock *CParagraph::GetFirstTextBlock()
{
	ASSERT(m_TextBlockList.size() > 0);
	return *m_TextBlockList.begin();
}

// 获取段落内得最后一个文本块
CTextBlock *CParagraph::GetLastTextBlock()
{
	ASSERT(m_TextBlockList.size() > 0);
	TextBlocks::iterator it = m_TextBlockList.end();
	it--;
	return *it;
}

// 是否是该段第一个文本块
BOOL CParagraph::IsFirstTextBlock(CTextBlock *pTextBlock)
{
	return *m_TextBlockList.begin() == pTextBlock;
}

// 是否是该段最后一个文本块
BOOL CParagraph::IsLastTextBlock(CTextBlock *pTextBlock)
{
	return *(--m_TextBlockList.end()) == pTextBlock;
}

// 根据列号返回所属得文本块
TextBlocks::iterator CParagraph::BlockFromIndex(int nIndex, int *nInBlockIndex/* = NULL*/, int *nRetIndex/* = NULL*/)
{
	ASSERT(m_TextBlockList.size() > 0);

	int nCount = 0;
	TextBlocks::iterator i;

	for (i = m_TextBlockList.begin(); i != m_TextBlockList.end(); i++)
	{
		nCount += (*i)->GetLength();
		if (nIndex <= nCount)
		{
			TextBlocks::iterator j = i;

			j++;
			if (j != m_TextBlockList.end() &&
				nIndex - (nCount - (*i)->GetLength()) == (*i)->GetLength())
			{
				if ((*j)->GetLength() == 0)
				{
					do
					{
						TextBlocks::iterator k = j;
						k++;

						if (k == m_TextBlockList.end())
						{
							break;
						}

						if ((*k)->GetLength())
						{
							break;
						}

						j++;
					}while(TRUE);

					if (nInBlockIndex != NULL)
					{
						*nInBlockIndex = 0;
					}

					if (nRetIndex != NULL)
					{
						*nRetIndex = nIndex;
					}

					return j;
				}
			}

			// 在这个块内
			if (nInBlockIndex != NULL)
			{
				*nInBlockIndex = nIndex - (nCount - (*i)->GetLength());
			}

			if (nRetIndex != NULL)
			{
				*nRetIndex = nIndex;
			}

			return i;
		}
	}

	// 返回最后一个块
	i = m_TextBlockList.end();
	i--;

	if (nInBlockIndex != NULL)
	{
		*nInBlockIndex = (*i)->GetLength();
	}

	if (nRetIndex != NULL)
	{
		*nRetIndex = nCount;
	}

	return i;
}

// 根据列号返回所属得文本块
TextBlocks::iterator CParagraph::BlockFromIndex2(int nIndex, int *nInBlockIndex/* = NULL*/, int *nRetIndex/* = NULL*/)
{
	ASSERT(m_TextBlockList.size() > 0);

	int nCount = 0;
	TextBlocks::iterator i;

	for (i = m_TextBlockList.begin(); i != m_TextBlockList.end(); i++)
	{
		nCount += (*i)->GetLength();
		if (nIndex < nCount)
		{
			// 在这个块内
			if (nInBlockIndex != NULL)
			{
				*nInBlockIndex = nIndex - (nCount - (*i)->GetLength());
			}

			if (nRetIndex != NULL)
			{
				*nRetIndex = nIndex;
			}

			return i;
		}
	}

	// 返回最后一个块
	i = m_TextBlockList.end();
	i--;

	if (nInBlockIndex != NULL)
	{
		*nInBlockIndex = (*i)->GetLength();
	}

	if (nRetIndex != NULL)
	{
		*nRetIndex = nCount;
	}

	return i;
}

// 在当前最前面插入一个文本块
void CParagraph::InsertToFirst(CTextBlock *pTextBlock)
{
	m_TextBlockList.insert(m_TextBlockList.begin(), pTextBlock);
	pTextBlock->SetParent(this);
	CalcHeight();
}

// 在当前段落末尾插入一个文本块
void CParagraph::Append(CTextBlock *pTextBlock)
{
	m_TextBlockList.push_back(pTextBlock);
	pTextBlock->SetParent(this);
	CalcHeight();
}

// 在当前位置插入一段文本
void CParagraph::Insert(int nIndex, wchar_t *pText, BOOL bReDraw/* = TRUE*/)
{
	TextBlocks::iterator i = BlockFromIndex(nIndex);

	CTextBlock TextBlock((*i)->TextFormat, pText);
	Insert(nIndex, &TextBlock, bReDraw);
}

// 在当前位置插入一个文本块
void CParagraph::Insert(int nIndex, CTextBlock *pTextBlock, BOOL bReDraw/* = TRUE*/)
{
	int nInBlockIndex;
	TextBlocks::iterator i;
	CTextBlock *pInsertTextBlock;

	i = BlockFromIndex(nIndex, &nInBlockIndex);
	pInsertTextBlock = *i;

	if (pInsertTextBlock->TextFormat == pTextBlock->TextFormat)
	{
		// 如果格式是一样的
		// 直接插入字符串
		pInsertTextBlock->strText.insert(nInBlockIndex, pTextBlock->strText.c_str());

		for (size_t j = 0; j < pTextBlock->strText.length(); j++)
		{
			pInsertTextBlock->CharWidth.insert(pInsertTextBlock->CharWidth.begin() + nInBlockIndex + j, 0);
		}
		pInsertTextBlock->CalcWidth();
	}
	else
	{
		// 断开文本块
		CTextBlock *pBefore, *pAfter;

		pInsertTextBlock->Break(nInBlockIndex, &pBefore, &pAfter);

		if (pBefore != NULL)
		{
			m_TextBlockList.insert(i, pBefore);
			pBefore->SetParent(this);
		}

		if (pAfter != NULL)
		{
			i++;
			m_TextBlockList.insert(i, pAfter);
			pAfter->SetParent(this);
		}

		// 替换中间的文本
		pInsertTextBlock->TextFormat = pTextBlock->TextFormat;
		pInsertTextBlock->strText = pTextBlock->strText;

		pInsertTextBlock->CalcWidth();
		pInsertTextBlock->CalcHeight();
	}

	Optimiz();
	CalcHeight();

	if (!Arrange())
	{
		ReDraw(bReDraw);
	}
}

// 在当前位置插入一个文本块，但是不整理
void CParagraph::Insert2(int nIndex, wchar_t *pText)
{
	TextBlocks::iterator i = BlockFromIndex(nIndex);

	CTextBlock TextBlock((*i)->TextFormat, pText);
	Insert2(nIndex, &TextBlock);
}

void CParagraph::Insert2(int nIndex, CTextBlock *pTextBlock)
{
	int nInBlockIndex;
	TextBlocks::iterator i;
	CTextBlock *pInsertTextBlock;

	i = BlockFromIndex(nIndex, &nInBlockIndex);
	pInsertTextBlock = *i;

	if (pInsertTextBlock->TextFormat == pTextBlock->TextFormat)
	{
		// 如果格式是一样的
		// 直接插入字符串
		pInsertTextBlock->strText.insert(nInBlockIndex, pTextBlock->strText.c_str());
		pInsertTextBlock->CalcWidth();
	}
	else
	{
		// 断开文本块
		CTextBlock *pBefore, *pAfter;

		pInsertTextBlock->Break(nInBlockIndex, &pBefore, &pAfter);

		if (pBefore != NULL)
		{
			m_TextBlockList.insert(i, pBefore);
			pBefore->SetParent(this);
		}

		if (pAfter != NULL)
		{
			i++;
			m_TextBlockList.insert(i, pAfter);
			pAfter->SetParent(this);
		}

		// 替换中间的文本
		pInsertTextBlock->TextFormat = pTextBlock->TextFormat;
		pInsertTextBlock->strText = pTextBlock->strText;

		pInsertTextBlock->CalcWidth();
		pInsertTextBlock->CalcHeight();
	}

	Optimiz();
	CalcHeight();
}

// 获取一个文本块绘制的X坐标
int CParagraph::GetDrawX(CTextBlock *pTextBlock)
{
	int nDrawX = GetDrawX();
	BOOL bFound = FALSE;

	TextBlocks::iterator i = m_TextBlockList.begin();
	while(i != m_TextBlockList.end())
	{
		if (*i == pTextBlock)
		{
			bFound = TRUE;
			break;
		}

		nDrawX += (*i)->GetWidth();
		i++;
	}

	ASSERT(bFound);
	return nDrawX;
}

// 获取制定位置字符绘制的X坐标
int CParagraph::GetDrawX(int nIndex)
{
	int nInBlockIndex, nDrawX;
	TextBlocks::iterator i;
	CTextBlock *pTextBlock;

	i = BlockFromIndex(nIndex, &nInBlockIndex);
	pTextBlock = *i;

	nDrawX = GetDrawX(pTextBlock);

	for (int i = 0; i < nInBlockIndex; i++)
	{
		nDrawX += pTextBlock->GetCharWidth(i);
	}

	return nDrawX;
}

// 计算段落的高度
void CParagraph::CalcHeight()
{
	nHeight = 0;
	nBaseline = 0;
	TextBlocks::iterator i = m_TextBlockList.begin();

	while(i != m_TextBlockList.end())
	{
		CTextBlock *pTextBlock = *i;

		nHeight = max(nHeight, pTextBlock->GetHeight() + GetSpacing());
		nBaseline = max(nBaseline, pTextBlock->nAscent);

		i++;
	}
}

// 计算绘制的Y坐标
void CParagraph::CalcDrawY()
{
	int nRow = pEditBase->RowFromParagraphPtr(this);
	nDrawY = BORDER_HEIGHT;

	for (int i = 0; i < nRow; i++)
	{
		nDrawY += pEditBase->m_ParagraphList[i]->GetHeight();
	}
}

// 计算当前行和所有下面的行绘制的Y坐标
void CParagraph::CalcDownY()
{
	for (int i = pEditBase->RowFromParagraphPtr(this);
		i < (int)pEditBase->m_ParagraphList.size(); i++)
	{
		pEditBase->m_ParagraphList[i]->CalcDrawY();
	}
}

// 获取段落占用的宽度
int CParagraph::GetWidth()
{
	int nWidth = 0;

	TextBlocks::iterator i = m_TextBlockList.begin();

	while(i != m_TextBlockList.end())
	{
		CTextBlock *pTextBlock = *i;

		nWidth += pTextBlock->GetWidth();
		i++;
	}

	return nWidth;
}

void CParagraph::AutoUpdate(BOOL bReDraw/* = TRUE*/)
{
	int nOldHeight = GetHeight();

	CalcDrawY();
	CalcHeight();
	if (nOldHeight > GetHeight())
	{
		CalcDownY();
		ReDrawDown(bReDraw);

		// 清空下面的区域
		int nPosY = 0;
		Paragraphs::iterator i = pEditBase->m_ParagraphList.begin();

		while(i != pEditBase->m_ParagraphList.end())
		{
			nPosY += (*i)->GetHeight();
			i++;
		}

		pEditBase->FillSolidRect(0, nPosY, abs(pEditBase->m_rtRegion.right - pEditBase->m_rtRegion.left) + 1,
			nOldHeight - GetHeight(), RGB(255, 255, 255), TRUE);
	}
	else if (nOldHeight < GetHeight())
	{
		CalcDownY();
		ReDrawDown(bReDraw);
	}

	ReDraw(bReDraw);
}

// 整理段落
// 功能：如果这个段落超出了页面的范围，则把超出的部分截断成一个新的软回车段落
// 如果下一段落是个软回车段落，并且这个段落还有多余的空间，则把下面的能移上来的文本都移上来
BOOL CParagraph::Arrange(int *nProcLine/* = NULL*/)
{
	int nUpdate = 0;
	BOOL bModify;
	int nRow = pEditBase->RowFromParagraphPtr(this);
	CTextEditBase *pb = pEditBase;

	nRow = pb->RowFromParagraphPtr(pb->GetHeadLine(nRow));

	bModify = pb->m_ParagraphList[nRow]->Arrange(nProcLine, nUpdate);
	nRow++;

	while(nRow < (int)pb->m_ParagraphList.size())
	{
		CParagraph *p = pb->m_ParagraphList[nRow];

		if (p->IsStiffRet())
		{
			break;
		}

		if (p->Arrange(nProcLine, nUpdate))
		{
			bModify = TRUE;
		}

		nRow++;
	}

	if (bModify)
	{
		pb->ReDraw();
	}

	pb->MoveCursor();
	return bModify;
}

BOOL CParagraph::IsRetChar(wchar_t c)
{
	return c == L' ' || c == L'-';
}

CTextBlock *CParagraph::FindRetChar(CTextBlock *pSBlock, int &nIndex, int nDirection/* = 1*/)
{
	ASSERT(nDirection == 1 || nDirection == -1);

	TextBlocks::iterator i = m_TextBlockList.begin();

	while(*i != pSBlock) i++;

	if (nDirection == 1)
	{
		// 向前找
		while(i != m_TextBlockList.end())
		{
			CTextBlock *p = *i;

			for (; nIndex < p->GetLength(); nIndex++)
			{
				if (IsRetChar(p->strText[nIndex]))
				{
					return p;
				}
			}
			
			nIndex = 0;
			i++;
		}
	}
	else
	{
		// 向后找
		do
		{
			CTextBlock *p = *i;

			for (; nIndex >= 0; nIndex--)
			{
				if (IsRetChar(p->strText[nIndex]))
				{
					return p;
				}
			}

			if (i == m_TextBlockList.begin())
			{
				break;
			}

			i--;
			nIndex = (*i)->GetLength() - 1;
		}while(true);
	}

	return NULL;
}

BOOL CParagraph::Arrange(int *nProcLine, int &nUpdate)
{
	int nPageWidth;
	BOOL bModify = FALSE;
	int nOldLength = GetLength();

	nUpdate++;
	Optimiz();

_retry_arrange:

	nPageWidth = GetPageWidth2();
	int cWidth = GetWidth();

	if (nPageWidth < 0)
	{
		return TRUE;
	}

	TRACE(MY_W2A(GetText().c_str()));

	if (GetWidth() > nPageWidth)
	{
		static int count = 0;

		count++;
		if (count == 2)
		{
			TRACE("第二次");
		}

		TRACE("文本换行\n");
		DebugShowWidths();

		// 超出页面范围了
		int nWidth = 0;
		TextBlocks::iterator i = m_TextBlockList.begin();
		CTextBlock *pOverTB = NULL;
		BOOL bUpdateDown = FALSE;

		while(i != m_TextBlockList.end())
		{
			CTextBlock *pTextBlock = *i;

			nWidth += pTextBlock->GetWidth();

			if (nWidth > nPageWidth)
			{
				// 就是这个文本块超出了
				pOverTB = pTextBlock;
				break;
			}

			i++;
		}

		ASSERT(pOverTB != NULL);

		// 寻找超出的文本块的断开点
		int nOverPos = 0, jj = 0;
		nWidth -= pOverTB->GetWidth();
		while(TRUE)
		{
			int nCharWidth = pOverTB->GetCharWidth(jj);

			if (nCharWidth > GetPageWidth3())
			{
				RECT rtNew = pEditBase->m_rtRegion;
				rtNew.right = GenMinRight(nCharWidth);
				GetEditBase()->ExecAndGroup(new COMMAND_CLSNAME(ChangeRegion)(GetEditBase(), rtNew));
				goto _retry_arrange;
			}

			nWidth += nCharWidth;
			if (nWidth > nPageWidth)
			{
				break;
			}

			nOverPos++, jj++;
		}

		// Word-warp
		int nSpace = nOverPos;
		CTextBlock *pSpace = FindRetChar(pOverTB, nSpace, -1);

		if (pSpace != NULL)
		{
			if (pSpace != *m_TextBlockList.begin() ||
				nSpace != 0)
			{
				int cl = ColumnFromTextBlockPtr(pSpace);
				cl += nSpace + 1;

				if (cl == GetLength())
				{
					pOverTB = pSpace;
					nOverPos = nSpace;  
				}
				else
				{
					pOverTB = *BlockFromIndex(cl, &nOverPos);
				}

				TRACE("单词位置换行.\n");
			}
		}

		// 把超出的部分转移到下一行
		CParagraph *pNext;
		int nRow = pEditBase->RowFromParagraphPtr(this);

		if (nRow == (int)pEditBase->m_ParagraphList.size() - 1)
		{
			// 是最后一行，则新建一行
			CParagraph *pParagraph = new CParagraph(pEditBase, FALSE, AlignType, GetSpacing(),
				GetMarginLeft(), GetMarginRight(), GetIndent());

			pEditBase->m_ParagraphList.push_back(pParagraph);

			pParagraph->CalcDrawY();
			pNext = pParagraph;

			if (nProcLine != NULL)
			{
				(*nProcLine)++;
			}
		}
		else
		{
			pNext = pEditBase->m_ParagraphList[nRow + 1];

			if (pNext->IsStiffRet())
			{
				// 下面是一个硬回车行，则在下面插入一个新行
				CParagraph *pParagraph = new CParagraph(pEditBase, FALSE, AlignType, GetSpacing(),
					GetMarginLeft(), GetMarginRight(), GetIndent());

				pEditBase->m_ParagraphList.insert(pEditBase->m_ParagraphList.begin() + nRow + 1, pParagraph);

				bUpdateDown = TRUE;
				pNext = pParagraph;

				if (nProcLine != NULL)
				{
					(*nProcLine)++;
				}

				if (pEditBase->m_nRow > nRow)
				{
					pEditBase->m_nRow++;
				}

				if (pEditBase->ptSelStart.y > nRow)
				{
					pEditBase->ptSelStart.y++;
				}

				if (pEditBase->ptSelEnd.y > nRow)
				{
					pEditBase->ptSelEnd.y++;
				}
			}
		}

		// 添加文本块
		TextBlocks::iterator j = m_TextBlockList.end();
		j--;

		while(pOverTB != *j)
		{
			TextBlocks::iterator k = j;

			pNext->InsertToFirst(*j);
			j--;
			m_TextBlockList.erase(k);
		}

		CTextBlock *pBefore, *pAfter;

		pOverTB->Break(nOverPos, &pBefore, &pAfter);

		if (pBefore != NULL)
		{
			pOverTB->SetText(pBefore->strText.c_str());
			delete pBefore;
		}
		else
		{
			pOverTB->SetText(L"");
		}

		if (pAfter != NULL)
		{
			pNext->InsertToFirst(pAfter);
		}

		// Debug
		DebugShowWidths();
		pNext->DebugShowWidths();
		TRACE(MY_W2A(GetText().c_str()));
		TRACE("\n");
		TRACE(MY_W2A(pNext->GetText().c_str()));
		TRACE("\n");
		TRACE("%d\n", GetWidth());
		TRACE("%d\n", pNext->GetWidth());

		// 调整光标位置
		if (pEditBase->m_nRow == nRow)
		{
			int nRetPos = ColumnFromTextBlockPtr(pOverTB) + nOverPos;

			if (pEditBase->m_nColumn >= nRetPos)
			{
				pEditBase->m_nColumn -= nRetPos;
				pEditBase->m_nRow++;
			}
		}
		else if (pEditBase->m_nRow == nRow + 1)
		{
			pEditBase->m_nColumn += (nOldLength - ColumnFromTextBlockPtr(pOverTB) - nOverPos);
		}

		// 调整选择位置
		if (pEditBase->ptSelStart.y == nRow)
		{
			int nRetPos = ColumnFromTextBlockPtr(pOverTB) + nOverPos;

			if (pEditBase->ptSelStart.x >= nRetPos)
			{
				pEditBase->ptSelStart.x -= nRetPos;
				pEditBase->ptSelStart.y++;
			}
		}
		else if (pEditBase->ptSelStart.y == nRow + 1)
		{
			pEditBase->ptSelStart.x += (nOldLength - ColumnFromTextBlockPtr(pOverTB) - nOverPos);
		}

		if (pEditBase->ptSelEnd.y == nRow)
		{
			int nRetPos = ColumnFromTextBlockPtr(pOverTB) + nOverPos;

			if (pEditBase->ptSelEnd.x >= nRetPos)
			{
				pEditBase->ptSelEnd.x -= nRetPos;
				pEditBase->ptSelEnd.y++;
			}
		}
		else if (pEditBase->ptSelEnd.y == nRow + 1)
		{
			pEditBase->ptSelEnd.x += (nOldLength - ColumnFromTextBlockPtr(pOverTB) - nOverPos);
		}

		// 重新计算行高
		Optimiz();
		CalcDownY();
		CalcHeight();
		pNext->CalcHeight();

		// 整理新加的行
		while(bModify = Arrange(nProcLine, nUpdate));

		if (nRow + 1 < (int)pEditBase->m_ParagraphList.size())
		{
			pNext = pEditBase->m_ParagraphList[nRow + 1];
//			bModify = pNext->Arrange(nProcLine, nUpdate);
		}

		if (bUpdateDown)
		{
			pNext->CalcHeight();
			pNext->CalcDrawY();
		}

		bModify = TRUE;
	}
	else
	{
		int nWidth = GetWidth();
		int nLine = pEditBase->RowFromParagraphPtr(this);
		CParagraph *pNextLine;
		CTextBlock *pTextBlock;

		if (nLine == (int)pEditBase->m_ParagraphList.size() - 1)
		{
			// 下面没有行了
			nUpdate--;
			return FALSE;
		}

		pNextLine = pEditBase->m_ParagraphList[nLine + 1];

		if (pNextLine->IsStiffRet())
		{
			// 下面是一个硬回车行
			nUpdate--;
			return FALSE;
		}

		CTextBlock *_pLastTB = GetLastTextBlock();
		int ck, od = nWidth;
		wchar_t lc, fc;
		int w1, w2;
		int nBreakPos = 0;

		TextBlocks::iterator i = pNextLine->m_TextBlockList.begin();
		while(i != pNextLine->m_TextBlockList.end())
		{
			wchar_t _LastChar;
			CTextBlock *_pFirstTB = *i;
			nBreakPos = 0;

			for (int j = 0; j < _pFirstTB->GetLength(); j++)
			{
				if (j == 0)
				{
					// 准备测试第一个字符
					// begin
					fc = _pFirstTB->strText[0];

					if (_pLastTB->TextFormat.IsSame(_pFirstTB->TextFormat))
					{
						// 格式差不多,把nWidth加上kerning
						lc = _pLastTB->strText[_pLastTB->GetLength() - 1];
						w1 = GetEditBase()->GetCharWidth(lc, 0, _pLastTB->TextFormat);
						w2 = GetEditBase()->GetCharWidth(lc, fc, _pLastTB->TextFormat);

						nWidth += w2 - w1;
						ck = w2 - w1;
					}
					// end

					_LastChar = fc;
				}
				else
				{
					fc = _pFirstTB->strText[j];
					w1 = GetEditBase()->GetCharWidth(_LastChar, 0, _pFirstTB->TextFormat);
					w2 = GetEditBase()->GetCharWidth(_LastChar, fc, _pFirstTB->TextFormat);

					nWidth += w2 - w1;
					nWidth += w1;
				}

				nWidth += GetEditBase()->GetCharWidth(fc, 0, _pFirstTB->TextFormat) + _pFirstTB->TextFormat.GetSpacing();

				if (nWidth > nPageWidth)
				{
					// 超出了
					break;
				}

				nBreakPos++;
			}

			if (nWidth > nPageWidth)
			{
				// 超出了
				break;
			}

			_pLastTB = *i;
			i++;
		}

		pTextBlock = *i;

		if (i == pNextLine->m_TextBlockList.end())
		{
			// 先从行列表中移出第二行
			pEditBase->m_ParagraphList.erase(pEditBase->m_ParagraphList.begin() + nLine + 1);

			// 把下一行的文本全部移上去
			TextBlocks::iterator k = pNextLine->m_TextBlockList.begin();
			while(k != pNextLine->m_TextBlockList.end())
			{
				TextBlocks::iterator kk = k;
				k++;
				Append(*kk);
				pNextLine->m_TextBlockList.erase(kk);
			}

			// 删除下面一行
			delete pNextLine;//*(pEditBase->m_ParagraphList.begin() + nLine + 1);

			// 调整光标位置
			if (pEditBase->m_nRow == nLine + 1)
			{
				int nRetPos = pEditBase->m_nColumn;

				pEditBase->m_nColumn = nOldLength + nRetPos;
				pEditBase->m_nRow--;
				//pEditBase->MoveCursor(nOldLength + nRetPos, pEditBase->m_nRow - 1);
			}
			else if (pEditBase->m_nRow > nLine + 1)
			{
				pEditBase->m_nRow--;
				//pEditBase->MoveCursor(pEditBase->m_nColumn, pEditBase->m_nRow - 1);
			}

			// 调整选择位置
			if (pEditBase->ptSelStart.y == nLine + 1)
			{
				int nRetPos = pEditBase->ptSelStart.x;

				pEditBase->ptSelStart.x = nOldLength + nRetPos;
				pEditBase->ptSelStart.y--;
			}
			else if (pEditBase->ptSelStart.y > nLine + 1)
			{
				pEditBase->ptSelStart.y--;
			}

			if (pEditBase->ptSelEnd.y == nLine + 1)
			{
				int nRetPos = pEditBase->ptSelEnd.x;

				pEditBase->ptSelEnd.x = nOldLength + nRetPos;
				pEditBase->ptSelEnd.y--;
			}
			else if (pEditBase->ptSelEnd.y > nLine + 1)
			{
				pEditBase->ptSelEnd.y--;
			}
		}
		else
		{
			/*
			pTextBlock = *i;
			nWidth -= pTextBlock->GetWidth();
			int nBreakPos = 0;

			for (int j = 0; j < pTextBlock->GetLength(); j++)
			{
				if (nWidth + pTextBlock->GetCharWidth(j) >= nPageWidth)
				{
					break;
				}
				nBreakPos++;
				nWidth += pTextBlock->GetCharWidth(j);
			}
*/
			if (nBreakPos == 0 && i == pNextLine->m_TextBlockList.begin())
			{
				// 一个都不能移
				nUpdate--;
				return FALSE;
			}

			TRACE("文本往上移\n");

			// Word-warp
			int nSpace = nBreakPos;
			CTextBlock *pSpace = pNextLine->FindRetChar(pTextBlock, nSpace, -1);

			if (pSpace != NULL)
			{
				if (pSpace != *pNextLine->m_TextBlockList.begin() || nSpace != 0)
				{
					int cl = pNextLine->ColumnFromTextBlockPtr(pSpace);
					cl += nSpace + 1;

					pTextBlock = *pNextLine->BlockFromIndex(cl, &nBreakPos);
//					pTextBlock = pSpace;
//					nBreakPos = nSpace;
					TRACE("单词移上去.\n");
				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				TextBlocks::iterator it = m_TextBlockList.end();
				it--;
				CTextBlock *pLast = *it;
				if (IsRetChar(pLast->strText[pLast->strText.length() - 1]))
				{
					return FALSE;
				}
			}

			int nBreakPos2 = pNextLine->ColumnFromTextBlockPtr(pTextBlock) + nBreakPos;

			// 移动前面的
			TextBlocks::iterator k = pNextLine->m_TextBlockList.begin();
			while(k != i)
			{
				TextBlocks::iterator kk = k;
				k++;
				Append(*kk);
				pNextLine->m_TextBlockList.erase(kk);
			}

			// 移动到上一行
			CTextBlock *pBefore, *pAfter;

			pTextBlock->Break(nBreakPos, &pBefore, &pAfter);

			if (pAfter != NULL)
			{
				pTextBlock->SetText(pAfter->strText.c_str());
				delete pAfter;
			}
			else
			{
				pTextBlock->SetText(L"");
			}

			if (pBefore != NULL)
			{
				Append(pBefore);
				pBefore->SetParent(this);
			}

			// 调整光标位置
			if (pEditBase->m_nRow == nLine + 1)
			{
				if (pEditBase->m_nColumn < nBreakPos2)
				{
					pEditBase->m_nColumn = nOldLength/* + nBreakPos2*/;
					pEditBase->m_nRow--;
				}
				else
				{
					pEditBase->m_nColumn -= nBreakPos2;
				}
			}

			// 调整选择位置
			if (pEditBase->ptSelStart.y == nLine + 1)
			{
				if (pEditBase->ptSelStart.x < nBreakPos2)
				{
					pEditBase->ptSelStart.x = nOldLength/* + nBreakPos2*/;
					pEditBase->ptSelStart.y--;
				}
				else
				{
					pEditBase->ptSelStart.x -= nBreakPos2;
				}
			}

			if (pEditBase->ptSelEnd.y == nLine + 1)
			{
				if (pEditBase->ptSelEnd.x < nBreakPos2)
				{
					pEditBase->ptSelEnd.x = nOldLength/* + nBreakPos2*/;
					pEditBase->ptSelEnd.y--;
				}
				else
				{
					pEditBase->ptSelEnd.x -= nBreakPos2;
				}
			}
		}

		// 重画
		Optimiz();
		CalcDownY();
		CalcHeight();
/*
		// 如果下面的行成了空行而且光标不在这一行，则删除下面的行
		if (pNextLine->GetLength() <= 0 && pEditBase->m_nRow != nLine + 1)
		{
			pNextLine->DeleteNoDraw();

			if (nProcLine != NULL)
			{
				(*nProcLine)--;
			}
		}
		else
		{
			pNextLine->CalcHeight();
		}
*/
		while(bModify = Arrange(nProcLine, nUpdate));

		if (nLine + 1 < (int)pEditBase->m_ParagraphList.size())
		{
			pNextLine = pEditBase->m_ParagraphList[nLine + 1];
		}
		else
		{
			nUpdate--;
			return TRUE;
		}

		bModify = TRUE;
	}

	nUpdate--;
	if (nUpdate == 0)
	{
		if (bModify)
		{
//			pEditBase->ReDraw();
//			TRACE("ReDraw.\n");
		}
	}

	Optimiz();
	pEditBase->MoveCursor(TRUE);
	return bModify;
}

// 优化文本块链表
void CParagraph::Optimiz()
{
	TextBlocks::iterator i = m_TextBlockList.begin(), k, j;
	int nOptimiz = 0;
	CTextFormat TF;

	while(i != m_TextBlockList.end())
	{
		CTextBlock *pTextBlock = *i, *pTextBlock2;

		if (pTextBlock->GetLength() == 0 && m_TextBlockList.size() > 1)
		{
			// 删除空的文本块
			TF = pTextBlock->TextFormat;

			k = i;
			i++;
			delete pTextBlock;
			m_TextBlockList.erase(k);

			nOptimiz++;
			continue;
		}

		j = i;
		j++;
		if (j == m_TextBlockList.end())
		{
			break;
		}

		pTextBlock2 = *j;

		if (pTextBlock->TextFormat == pTextBlock2->TextFormat)
		{
			// 相同的格式
			std::wstring strNew = pTextBlock->strText + pTextBlock2->strText;

			delete pTextBlock2;
			m_TextBlockList.erase(j);

			pTextBlock->SetText(strNew.c_str());
			pTextBlock->CalcWidth();

			nOptimiz++;

			continue;
		}

		i++;
	}

	if (m_TextBlockList.size() == 0)
	{
		// 文本块列表为空
		// 插入一个新文本块上去
		CTextBlock *pNewTextBlock = new CTextBlock(TF, L"");
		InsertToFirst(pNewTextBlock);
	}

//	pEditBase->MoveCursor(pEditBase->m_nColumn, pEditBase->m_nRow);
}

int CParagraph::ColumnFromTextBlockPtr(CTextBlock *pTextBlock)
{
	int nColumn = 0;

	TextBlocks::iterator i = m_TextBlockList.begin();

	while(i != m_TextBlockList.end())
	{
		if (pTextBlock == *i)
		{
			break;
		}

		nColumn += (*i)->GetLength();
		i++;
	}

	return nColumn;
}

void CParagraph::ReDraw(BOOL bReDraw/* = TRUE*/)
{
	pEditBase->UpdateLine(this, bReDraw);
}

void CParagraph::ReDrawDown(BOOL bReDraw/* = TRUE*/)
{
	for (int i = pEditBase->RowFromParagraphPtr(this);
		i < (int)pEditBase->m_ParagraphList.size(); i++)
	{
		pEditBase->m_ParagraphList[i]->ReDraw(bReDraw);
	}
}

int CParagraph::GetLength()
{
	int nLength = 0;

	TextBlocks::iterator i = m_TextBlockList.begin();

	while(i != m_TextBlockList.end())
	{
		nLength += (*i)->GetLength();
		i++;
	}

	return nLength;
}

void CParagraph::Delete(BOOL bReDraw/* = TRUE*/)
{
	int nRow = pEditBase->RowFromParagraphPtr(this);
	Paragraphs::iterator i = pEditBase->m_ParagraphList.begin() + nRow;

	// 删除行
	pEditBase->m_ParagraphList.erase(i);

	if (i != pEditBase->m_ParagraphList.end())
	{
		(*i)->CalcDownY();
		(*i)->ReDrawDown(bReDraw);
	}

	// 调整光标位置
	if (nRow == pEditBase->m_nRow)
	{
		pEditBase->MoveCursor();
	}

	// 清空下面的区域
	int nPosY = 0;
	i = pEditBase->m_ParagraphList.begin();

	while(i != pEditBase->m_ParagraphList.end())
	{
		nPosY += (*i)->GetHeight();
		i++;
	}

	pEditBase->FillSolidRect(0, nPosY, abs(pEditBase->m_rtRegion.right - pEditBase->m_rtRegion.left) + 1,
		GetHeight(), RGB(255, 255, 255), bReDraw);

	delete this;
}

void CParagraph::DeleteNoDraw()
{
	int nRow = pEditBase->RowFromParagraphPtr(this);
	Paragraphs::iterator i = pEditBase->m_ParagraphList.begin() + nRow;

	// 删除行
	pEditBase->m_ParagraphList.erase(i);

	if (i != pEditBase->m_ParagraphList.end())
	{
		(*i)->CalcDownY();
	}

	// 调整光标位置
	if (nRow == pEditBase->m_nRow)
	{
		pEditBase->MoveCursor();
	}

	delete this;
}

int CParagraph::ColumnFromPixelX(int x)
{
	TextBlocks::iterator i = m_TextBlockList.begin();
	int nDrawX = 0, nColumn = 0;

	while(i != m_TextBlockList.end())
	{
		CTextBlock *pTextBlock = *i;

		nDrawX += pTextBlock->GetWidth();
		nColumn += pTextBlock->GetLength();

		if (nDrawX >= x)
		{
			nDrawX -= pTextBlock->GetWidth();
			nColumn -= pTextBlock->GetLength();

			for (int j = 0; j < pTextBlock->GetLength(); j++)
			{
				nDrawX += pTextBlock->GetCharWidth(j);
				nColumn++;

				if (nDrawX >= x)
				{
					return nColumn;
				}
			}

			break;
		}

		i++;
	}

	// 没有找到坐标>X的文本块
	return GetLength();
}

int CParagraph::GetDrawX()
{
	int nDrawX;
	int nWidth = GetWidth();

	switch(GetAlignType())
	{
	case CParagraph::AlignLeft:
		nDrawX = GetMarginLeft() + GetIndent();
		break;
	case CParagraph::AlignCenter:
		nDrawX = GetPageWidth() / 2 - nWidth / 2 + (GetMarginLeft() + GetIndent());
		break;
	case CParagraph::AlignRight:
		nDrawX = (abs(pEditBase->m_rtRegion.right - pEditBase->m_rtRegion.left) + 1) - nWidth - GetMarginRight() - BORDER_SIZE * 2;
		break;
	default:
		ASSERT(FALSE);
	}

	return nDrawX + BORDER_SIZE;
}

void CParagraph::SetAlignTypeType(emAlignType at, BOOL bReDraw/* = TRUE*/)
{
	AlignType = at;
}

void CParagraph::Write(oBinStream &out)
{
	int t;

	out << bStiffRet;
	out << nHeight;
	out << nBaseline;
	out << nDrawY;
	t = (int)AlignType;
	out << t;
	out << m_Spacing;
	out << m_MarginLeft;
	out << m_MarginRight;
	out << m_Indent;

	TextBlocks::iterator i = m_TextBlockList.begin();
	t = (int)m_TextBlockList.size();

	out << t;
	while(i != m_TextBlockList.end())
	{
		(*i)->Write(out);
		i++;
	}
}

void CParagraph::Read(iBinStream &in)
{
	int t;

	in >> bStiffRet;
	in >> nHeight;
	in >> nBaseline;
	in >> nDrawY;
	in >> t;
	AlignType = (CParagraph::emAlignType)t;
	in >> m_Spacing;
	in >> m_MarginLeft;
	in >> m_MarginRight;
	in >> m_Indent;

	in >> t;
	for (int i = 0; i < t; i++)
	{
		CTextBlock *ptb = new CTextBlock(CTextFormat());
		ptb->Read(in);
		m_TextBlockList.push_back(ptb);
		ptb->SetParent(this);
	}
}

int CParagraph::GetPageWidth()
{
	return abs((pEditBase->m_rtRegion.left + GetMarginLeft() + GetIndent()) - 
		(pEditBase->m_rtRegion.right - GetMarginRight())) + 1;
}

int CParagraph::GetPageWidth2()
{
	if (IsStiffRet())
	{
		return (pEditBase->m_rtRegion.right - pEditBase->m_rtRegion.left + 1) - 
			(GetMarginLeft() + GetIndent() + GetMarginRight() + BORDER_SIZE * 2);
	}
	else
	{
		return (pEditBase->m_rtRegion.right - pEditBase->m_rtRegion.left + 1) - 
			(GetMarginLeft() + GetMarginRight() + BORDER_SIZE * 2);
	}
}

int CParagraph::GetPageWidth3()
{
	return (pEditBase->m_rtRegion.right - pEditBase->m_rtRegion.left + 1) - 
		(GetMarginLeft() + GetMarginRight() + BORDER_SIZE * 2);
}

int CParagraph::GenMinRight(int n)
{
	if (IsStiffRet())
	{
		return pEditBase->m_rtRegion.left + n + GetMarginLeft() +
			GetMarginRight() + GetIndent() + BORDER_SIZE * 2 + 1;
	}
	else
	{
		return pEditBase->m_rtRegion.left + n + GetMarginLeft() +
			GetMarginRight() + BORDER_SIZE * 2 + 1;
	}
}

int CParagraph::GetAdjustMaxWidth()
{
	TextBlocks::iterator i = m_TextBlockList.begin();
	int nMax = -1;

	while(i != m_TextBlockList.end())
	{
		for (size_t j = 0; j < (*i)->strText.length(); j++)
		{
			//nMax = max(nMax, pEditBase->GetCharWidth((*i)->strText[j], 0, (*i)->TextFormat));
			nMax = max(nMax, (*i)->CharWidth[j]);
		}

		i++;
	}

	return nMax;
}

CTextBlock *CParagraph::FindChar(CTextBlock *pSBlock, int &nIndex, wchar_t c, int nDirection/* = 1*/)
{
	ASSERT(nDirection == 1 || nDirection == -1);

	TextBlocks::iterator i = m_TextBlockList.begin();

	while(*i != pSBlock) i++;

	if (nDirection == 1)
	{
		// 向前找
		while(i != m_TextBlockList.end())
		{
			CTextBlock *p = *i;

			for (; nIndex < p->GetLength(); nIndex++)
			{
				if (p->strText[nIndex] == c)
				{
					return p;
				}
			}
			
			nIndex = 0;
			i++;
		}
	}
	else
	{
		// 向后找
		do
		{
			CTextBlock *p = *i;

			for (; nIndex >= 0; nIndex--)
			{
				if (p->strText[nIndex] == c)
				{
					return p;
				}
			}

			if (i == m_TextBlockList.begin())
			{
				break;
			}

			i--;
			nIndex = (*i)->GetLength() - 1;
		}while(true);
	}

	return NULL;
}

CParagraph *CParagraph::Clone(CTextEditBase *pb)
{
	oBinStream out;
	iBinStream in;

	Write(out);
	U8 *p = new U8 [out.Size()];

	out.WriteToMemory(p);
	in.ReadFromMemory(out.Size(), p);
	CParagraph *pNewPara = new CParagraph(pb);
	pNewPara->Read(in);

	delete p;

	TextBlocks::iterator it = pNewPara->m_TextBlockList.begin();
	while(it != pNewPara->m_TextBlockList.end())
	{
		(*it)->CalcWidth();
		(*it)->CalcHeight();
		it++;
	}
	pNewPara->CalcHeight();

	return pNewPara;
}

std::wstring CParagraph::GetText()
{
	std::wstring str;

	TextBlocks::iterator i = m_TextBlockList.begin();
	while(i != m_TextBlockList.end())
	{
		str += (*i)->strText;
		i++;
	}

	return str;
}

void CParagraph::GetParaFormat(CParaFormat &fmt)
{
	fmt.SetAlignType(GetAlignType());
	fmt.SetIndent(GetIndent());
	fmt.SetMarginLeft(GetMarginLeft());
	fmt.SetMarginRight(GetMarginRight());
	fmt.SetSpacing(GetSpacing());
}

void CParagraph::DebugShowWidths()
{
#ifdef _DEBUG
	TRACE("\n");
	TextBlocks::iterator it_t;

	it_t = m_TextBlockList.begin();
	while(it_t != m_TextBlockList.end())
	{
		CTextBlock *pb = *it_t;

		for (int i = 0; i < pb->GetLength(); i++)
		{
			wchar_t str[2] = {pb->strText[i], 0};
			TRACE("%s:%d,", MY_W2A(str), pb->GetCharWidth(i));
		}

		it_t++;
	}
	TRACE("\n");
#endif
}

// ######################################################################################### //

DWORD CParaFormat::Compare(CParaFormat &ParaFormat, DWORD dwFlag1, DWORD dwFlag2)
{
	if (dwFlag2 & PF_ALIGNTYPE)
	{
		if (dwFlag1 & PF_ALIGNTYPE)
		{
			if (GetAlignType() != ParaFormat.GetAlignType())
			{
				dwFlag2 &= ~PF_ALIGNTYPE;
			}
		}
		else
		{
			dwFlag2 &= ~PF_ALIGNTYPE;
		}
	}

	if (dwFlag2 & PF_SPACING)
	{
		if (dwFlag1 & PF_SPACING)
		{
			if (GetSpacing() != ParaFormat.GetSpacing())
			{
				dwFlag2 &= ~PF_SPACING;
			}
		}
		else
		{
			dwFlag2 &= ~PF_SPACING;
		}
	}

	if (dwFlag2 & PF_MARGINLEFT)
	{
		if (dwFlag1 & PF_MARGINLEFT)
		{
			if (GetMarginLeft() != ParaFormat.GetMarginLeft())
			{
				dwFlag2 &= ~PF_MARGINLEFT;
			}
		}
		else
		{
			dwFlag2 &= ~PF_MARGINLEFT;
		}
	}

	if (dwFlag2 & PF_MARGINRIGHT)
	{
		if (dwFlag1 & PF_MARGINRIGHT)
		{
			if (GetMarginRight() != ParaFormat.GetMarginRight())
			{
				dwFlag2 &= ~PF_MARGINRIGHT;
			}
		}
		else
		{
			dwFlag2 &= ~PF_MARGINRIGHT;
		}
	}

	if (dwFlag2 & PF_INDENT)
	{
		if (dwFlag1 & PF_INDENT)
		{
			if (GetIndent() != ParaFormat.GetIndent())
			{
				dwFlag2 &= ~PF_INDENT;
			}
		}
		else
		{
			dwFlag2 &= ~PF_INDENT;
		}
	}

	return dwFlag2;
}

// ######################################################################################### //

std::map <UINT_PTR, CTECursor *> CTECursor::Cursors; // 定时器ID对应的光标类

CTECursor::CTECursor()
{
	bInstalled = FALSE;
	pEditBase = NULL;
}

void CTECursor::Install(HWND hwnd, CTextEditBase *pEB, int x, int y, int w, int h)
{
	if (bInstalled)
	{
		return;
	}

	this->hwnd = hwnd;
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
	this->pEditBase = pEB;

	nID = SetTimer(NULL, 0, GetCaretBlinkTime(), CTECursor::TimerProc);
	CTECursor::Cursors.insert(std::map <UINT_PTR, CTECursor *>::value_type(nID, this));
	bInstalled = TRUE;
	bShow = TRUE;
}

void CTECursor::Uninstall()
{
	if (!bInstalled)
	{
		return;
	}

	std::map <UINT_PTR, CTECursor *>::iterator i = CTECursor::Cursors.find(nID);

	if (i != CTECursor::Cursors.end())
	{
		KillTimer(NULL, (*i).first);
		CTECursor::Cursors.erase(i);
	}

	bInstalled = FALSE;
}

void CTECursor::OnTimer()
{
	if (bShow && pEditBase != NULL)
	{
		LOGBRUSH logBrush;

		logBrush.lbColor = RGB(255, 255, 255);
		logBrush.lbHatch = 0;
		logBrush.lbStyle = BS_SOLID;

		HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_BEVEL, width, &logBrush, 0, NULL),
			hOldPen;
		HDC hDC = GetDC(hwnd);
		int nOldROP = SetROP2(hDC, R2_XORPEN);

		hOldPen = (HPEN)SelectObject(hDC, hPen);

		int dx, dy, dx2, dy2;

		pEditBase->OnReposCursor(x, y, dx, dy, dx2, dy2, FALSE);

		MoveToEx(hDC, dx, dy, NULL);
		LineTo(hDC, dx2, dy2);

		SetROP2(hDC, nOldROP);
		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
		ReleaseDC(hwnd, hDC);
	}
}

void CALLBACK CTECursor::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (hwnd == NULL)
	{
		std::map <UINT_PTR, CTECursor *>::iterator i = CTECursor::Cursors.find(idEvent);

		if (i != CTECursor::Cursors.end())
		{
			(*i).second->OnTimer();
		}
	}
}

void CTECursor::Move(int x, int y)
{
	this->x = x;
	this->y = y;

	int dx, dy, dx2, dy2;
	
	 if (pEditBase != NULL)
	 {
		 pEditBase->OnReposCursor(x, y, dx, dy, dx2, dy2, TRUE);
	 }
}

void CTECursor::Move2(int x, int y)
{
	this->x = x;
	this->y = y;

	int dx, dy, dx2, dy2;

	if (pEditBase) pEditBase->OnReposCursor(x, y, dx, dy, dx2, dy2, FALSE);
}

void CTECursor::SetHeight(int h)
{
	this->height = h;
}

void CTECursor::SetWidth(int w)
{
	this->width = w;
}

void CTECursor::ReDraw()
{
	OnTimer();
}

// ######################################################################################### //

CTextEditBase::CTextEditBase() : m_DefTextFormat1("Times New Roman", 
												  24 * 20, RGB(0, 0, 0),
												  FALSE, FALSE, 
												  0, CTextFormat::CL_NONE, 
												  std::string(),
												  std::string()),
	m_DefParaFormat1(CParagraph::AlignLeft, 0, 0, 0, 0), bUpdate(TRUE)
{
	nTabStop = 4;
	bInsertMode = TRUE;
}

CTextEditBase::~CTextEditBase()
{
	EndEdit();
}

void CTextEditBase::StartEdit()
{
	nLockUpdate = 0;
	nLockDraw = 0;

	ptSelStart.x = ptSelStart.y = 0;
	ptSelEnd = ptSelStart;

	// 初始化光标显示服务
	Cursor.Install(GetHwnd(), this, 0, 0, 2, 0);

	// 初始化编辑环境
	// 1. 设置光标到第1行第1列
	if (m_ParagraphList.empty())
	{
		// 没有任何数据
		// 则新建一个空的段落和新的文本块
		CParagraph *pParagraph = new CParagraph(this, TRUE, m_DefParaFormat.GetAlignType(),
			m_DefParaFormat.GetSpacing(), 0, 0, 
			0);
		CTextBlock *pTextBlock = new CTextBlock(m_DefTextFormat);

		m_ParagraphList.push_back(pParagraph);

		pParagraph->Append(pTextBlock);
		pParagraph->CalcDrawY();

		// 设置默认段落格式
		SetIndent(m_DefParaFormat.GetIndent());
		SetMarginLeft(m_DefParaFormat.GetMarginLeft());
		SetMarginRight(m_DefParaFormat.GetMarginRight());
	}

	ClearCommands();
	ReDraw();

	// 设置初始光标位置
	MoveCursor(0, 0);
}

void CTextEditBase::EndEdit()
{
	// 释放Commands
	ClearCommands();

	// 释放编辑数据
	Paragraphs::iterator i = m_ParagraphList.begin();

	while(i != m_ParagraphList.end())
	{
		delete *i;
		i++;
	}

	m_ParagraphList.clear();

	// 删除显示光标
	Cursor.Uninstall();
}

int CTextEditBase::RowFromParagraphPtr(CParagraph *pParagraph)
{
	for (int i = 0; i < (int)m_ParagraphList.size(); i++)
	{
		if (m_ParagraphList[i] == pParagraph)
		{
			return i;
		}
	}

	return -1;
}

void CTextEditBase::TE_KeyDown(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (bEdit)
	{
		switch(nChar)
		{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
			Key_Arrow(nChar, nFlags);
			Cursor.ReDraw();
			break;
		default:
			break;
		}
	}
}

void CTextEditBase::Key_Ins()
{
	bInsertMode = !bInsertMode;
	MoveCursor();
	UpdateAll();
}

void CTextEditBase::TE_Char(UINT nChar, UINT nRepCount, UINT nFlags)
{
	if (bEdit)
	{
		switch(nChar)
		{
		case VK_RETURN:
			Key_Return();
			break;
		default:
			Key_Char(nChar);
			break;
		}
	}
}

void CTextEditBase::Key_Arrow(UINT nChar, UINT nFlags/* = 0*/, BOOL bSel/* = TRUE*/)
{
	POINT ptOld = {m_nColumn, m_nRow};

	switch(nChar)
	{
	case VK_LEFT:
		{
			if (!SelectIsEmpty() && !SHIFT_DOWN())
			{
				ClearSelect();
				return;
			}
			else
			{
				if (CursorAtFirst())
				{
					// 已经在最前面了
					if (m_nRow - 1 >= 0)
					{
						MoveCursor(m_nColumn, m_nRow - 1);
						MoveToLast();
					}
				}
				else
				{
					MoveCursor(m_nColumn - 1, m_nRow);
				}

				UpdateAll();
			}
		}
		break;
	case VK_RIGHT:
		{
			if (!SelectIsEmpty() && !SHIFT_DOWN())
			{
				ClearSelect();
				return;
			}
			else
			{
				if (CursorAtLast())
				{
					// 已经在最后面了
					if (m_nRow + 1 < (int)m_ParagraphList.size())
					{
						if (!m_ParagraphList[m_nRow + 1]->IsStiffRet() && m_nColumn == m_pCurParagraph->GetLength())
						{
							// 如果下一行是软回车行，并且光标在行的最末尾
							MoveCursor(1, m_nRow + 1);
						}
						else
						{
							MoveCursor(0, m_nRow + 1);
						}
					}
				}
				else
				{
					MoveCursor(m_nColumn + 1, m_nRow);
				}

				UpdateAll();
			}
		}
		break;
	case VK_UP:
		if (m_nRow > 0)
		{
			CParagraph *pPrev = m_ParagraphList[m_nRow - 1];

			MoveCursor(pPrev->ColumnFromPixelX(Cursor.GetCursorX()), m_nRow - 1);
		}
		UpdateAll();
		break;
	case VK_DOWN:
		if (m_nRow < (int)m_ParagraphList.size() - 1)
		{
			CParagraph *pNext = m_ParagraphList[m_nRow + 1];

			MoveCursor(pNext->ColumnFromPixelX(Cursor.GetCursorX()), m_nRow + 1);
		}
		UpdateAll();
		break;
	case VK_HOME:
		MoveToFirst();
		UpdateAll();
		break;
	case VK_END:
		MoveToLast();
		UpdateAll();
		break;
	}

	POINT pt = {m_nColumn, m_nRow};

	if (SHIFT_DOWN() && bSel)
	{
		if (SelectIsEmpty())
		{
			ptSelPos = ptOld;
			SetSel(ptSelPos, pt);
		}
		else
		{
			SetSel(ptSelPos, pt);
		}
	}
	else
	{
		// 设置选择区域到光标位置
		SelToCursor();
	}

	Cursor.ReDraw();
}

void CTextEditBase::MoveCursor(int nColumn, int nRow, BOOL bNoReposCursor)
{
	if (nRow < 0)
	{
		nRow = 0;
	}

	if (nRow > (int)m_ParagraphList.size() - 1)
	{
		nRow = (int)m_ParagraphList.size() - 1;
	}

	m_nRow = nRow;
	m_pCurParagraph = m_ParagraphList[nRow];

	TextBlocks::iterator i;
	int nInBlockIndex, nIndex;

	i = m_pCurParagraph->BlockFromIndex(nColumn, &nInBlockIndex, &nIndex);
	m_nColumn = nIndex;
	m_nSubColumn = nInBlockIndex;
	m_pCurTextBlock = *i;

	// 取文本格式
	if (i == m_pCurParagraph->m_TextBlockList.begin() && m_nColumn == 0 &&
			!m_pCurParagraph->IsStiffRet())
	{
		CParagraph *pPrev = m_ParagraphList[nRow - 1];
		TextBlocks::iterator it = pPrev->m_TextBlockList.end();
		it--;
		m_DefTextFormat = (*it)->TextFormat;
	}
	else
	{
		m_DefTextFormat = m_pCurTextBlock->TextFormat;
	}

	// 移动显示的光标
	//if (!bNoReposCursor)
	{
		RECT rtPos;

		GetCharRect(m_nColumn, m_nRow, rtPos);
		bNoReposCursor ? Cursor.Move2(rtPos.left, rtPos.top) : Cursor.Move(rtPos.left, rtPos.top);
		Cursor.SetHeight(rtPos.bottom - rtPos.top + 1 - m_pCurParagraph->GetSpacing());

		if (rtPos.left == rtPos.right)
		{
			Cursor.SetWidth(2);
		}
		else
		{
			if (bInsertMode)
			{
				Cursor.SetWidth(2);
			}
			else
			{
				Cursor.SetWidth((rtPos.right - rtPos.left + 1) / 20);
			}
		}
	}
}

void CTextEditBase::MoveCursor(BOOL bNoReposCursor)
{
	MoveCursor(m_nColumn, m_nRow, bNoReposCursor);
}

void CTextEditBase::CursorToFirst()
{
	POINT ptOld = {m_nColumn, m_nRow};

	UpdateAll();
	MoveCursor(0, 0);

	POINT pt = {m_nColumn, m_nRow};

	if (SHIFT_DOWN())
	{
		if (SelectIsEmpty())
		{
			ptSelPos = ptOld;
			SetSel(ptSelPos, pt);
		}
		else
		{
			SetSel(ptSelPos, pt);
		}
	}
	else
	{
		SelToCursor();
	}
}

void CTextEditBase::CursorToLast()
{
	POINT ptOld = {m_nColumn, m_nRow};

	int nRow = (int)m_ParagraphList.size();
	int nColumn = m_ParagraphList[m_ParagraphList.size() - 1]->GetLength();

	UpdateAll();
	MoveCursor(nColumn, nRow);

	POINT pt = {m_nColumn, m_nRow};

	if (SHIFT_DOWN())
	{
		if (SelectIsEmpty())
		{
			ptSelPos = ptOld;
			SetSel(ptSelPos, pt);
		}
		else
		{
			SetSel(ptSelPos, pt);
		}
	}
	else
	{
		SelToCursor();
	}
}

void CTextEditBase::MoveToLast()
{
	MoveCursor(/*GetLastIndex()*/m_pCurParagraph->GetLength(), m_nRow);
}

void CTextEditBase::MoveToFirst()
{
	MoveCursor(GetFirstIndex(), m_nRow);
}

int CTextEditBase::GetFirstIndex()
{
	return 0;
}

int CTextEditBase::GetLastIndex()
{
	int nRow = RowFromParagraphPtr(m_pCurParagraph);

	if (nRow == (int)m_ParagraphList.size() - 1)
	{
		return m_pCurParagraph->GetLength();
	}
	else
	{
		if (m_ParagraphList[nRow + 1]->IsStiffRet())
		{
			return m_pCurParagraph->GetLength();
		}
		else
		{
			if ((*m_ParagraphList[nRow + 1]->m_TextBlockList.begin())->strText[0] != L' ')
			{
				return m_pCurParagraph->GetLength() - 1;
			}
			else
			{
				return m_pCurParagraph->GetLength();
			}
		}
	}
}

BOOL CTextEditBase::CursorAtFirst()
{
	return m_nColumn == GetFirstIndex();
}

BOOL CTextEditBase::CursorAtLast()
{
	return m_nColumn == GetLastIndex();
}

BOOL CTextEditBase::CanInsertChar(wchar_t c)
{
/*	if (c == L' ')
	{
		return m_ParagraphList[m_nRow]->GetWidth() + GetCharWidth(c, 0, m_DefTextFormat) < 
			m_rtRegion.right - m_rtRegion.left;
	}
	else*/
	{
		return c != L'\t';
	}
}

void CTextEditBase::Return()
{
	if (CursorAtFirst() && !m_pCurParagraph->IsStiffRet())
	{
		// 如果当前光标在行首，并且当前行是一个软回车行，则把当前
		// 转换为硬回车行
		m_pCurParagraph->SetStiffRet(TRUE);
	}
	else
	{
		int nInBlockIndex;
		TextBlocks::iterator i, j, k;
		CTextBlock *pInsertTextBlock;
		CTextBlock *pBefore, *pAfter;
		CParagraph *pNext;

		i = m_pCurParagraph->BlockFromIndex(m_nColumn, &nInBlockIndex, &m_nSubColumn);
		j = i;
		pInsertTextBlock = *i;

		// 断开文本块
		pInsertTextBlock->Break(m_nSubColumn, &pBefore, &pAfter);

		// 修改被断开的文本块的字符串
		if (pBefore != NULL)
		{
			pInsertTextBlock->SetText(pBefore->strText.c_str());
			delete pBefore;
		}
		else
		{
			pInsertTextBlock->SetText(L"");
		}

		// 创建新段落
		CParagraph *pParagraph = new CParagraph(this, TRUE, m_pCurParagraph->GetAlignType(),
			m_pCurParagraph->GetSpacing(),
			m_pCurParagraph->GetMarginLeft(),
			m_pCurParagraph->GetMarginRight(),
			GetHeadLine(m_nRow)->GetIndent());
		CTextBlock *pTextBlock = new CTextBlock(pInsertTextBlock->TextFormat);

		m_ParagraphList.insert(m_ParagraphList.begin() + m_nRow + 1, pParagraph);
		pParagraph->Append(pTextBlock);

		pNext = pParagraph;

		// 转移后面的文本块
		j++;
		while(j != m_pCurParagraph->m_TextBlockList.end())
		{
			k = j;
			pNext->Append(*j);
			j++;
			m_pCurParagraph->m_TextBlockList.erase(k);
		}

		if (pAfter != NULL)
		{
			// 添加后半部分文本块
			pNext->InsertToFirst(pAfter);
		}

		m_pCurParagraph->CalcHeight();
		pNext->CalcHeight();
		pNext->CalcDownY();
		pNext->Arrange();

		MoveCursor(0, m_nRow + 1);
		ptSelStart.x = m_nColumn;
		ptSelStart.y = m_nRow;
		ptSelEnd = ptSelStart;
	}
}

void CTextEditBase::Key_Return()
{
	CCommandGroup *pCmd = new CCommandGroup;

	pCmd->AddCommand(new COMMAND_CLSNAME(Replace)(this, L""));
	pCmd->AddCommand(new COMMAND_CLSNAME(Return)(this));

	PushCommand(pCmd);
}

void CTextEditBase::Key_Char(UINT nChar, BOOL bUnicode/* = FALSE*/)
{
/*	if (CTRL_DOWN() || ALT_DOWN())
	{
		return;
	}
*/
	if ((::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
		(::GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0)
	{
		return;
	}

	if (bUnicode)
	{
		wchar_t chInsert[] = {nChar, 0};

		if (!CanInsertChar(nChar))
		{
			return;
		}

		if (!bInsertMode && SelectIsEmpty())
		{
			ptSelEnd.x = ptSelStart.x + 1;
			AdjustLocate(ptSelEnd.x, ptSelEnd.y);
		}

		DoReplace(chInsert);
		SelToCursor();
	}
	else
	{
		if (nChar <= 31)
		{
			return;
		}

		//MY_USES_CONVERSION;

		static int c_prev = -1;

		if ((_ismbblead(nChar) != 0) || c_prev != -1)
		{
			if (!bInsertMode && SelectIsEmpty())
			{
				ptSelEnd.x = ptSelStart.x + 1;
				AdjustLocate(ptSelEnd.x, ptSelEnd.y);
			}
		}

		if (c_prev == -1)
		{
			if (_ismbblead(nChar) != 0)
			{
				c_prev = nChar;
			}
			else
			{
				char ansi[] = {nChar, 0};
				wchar_t *_uc;

				try
				{
					_uc = MY_A2W(ansi);
					char *_p = MY_W2A(_uc);
				}catch(...)
				{
					return;
				}

				if (CanInsertChar(*_uc))
				{
					DoReplace(_uc);
				}
				SelToCursor();
			}
		}
		else
		{
			char ansi[] = {c_prev, nChar, 0};
			c_prev = -1;

			wchar_t *_uc;

			try
			{
				_uc = MY_A2W(ansi);
			}catch(...)
			{
				return;
			}

			if (CanInsertChar(*_uc))
			{
				DoReplace(_uc);
			}
			SelToCursor();
		}
	}
}

void CTextEditBase::Key_Backspace()
{
	if (!SelectIsEmpty())
	{
		DoReplace(L"");
	}
	else
	{
		SelToCursor();

		int nInBlockIndex;
		TextBlocks::iterator i;
		CTextBlock *pTextBlock;

		i = m_pCurParagraph->BlockFromIndex(m_nColumn, &nInBlockIndex);
		pTextBlock = *i;

		if (m_pCurParagraph->IsFirstTextBlock(pTextBlock) && nInBlockIndex == 0)
		{
			// 如果光标在最前面，并且当前是个软回车行，则移动光标到上一行的末尾，在删除一次
			// 如果当前是硬回车行，则先把当前行变成软回车行，整理上一行，然后再删除一次
			// 如果没有上一行，则什么都不做
			int nPrevLine = RowFromParagraphPtr(m_pCurParagraph) - 1;

			if (nPrevLine >= 0)
			{
				CParagraph *pPrev = m_ParagraphList[nPrevLine];

				if (!m_pCurParagraph->IsStiffRet())
				{
					MoveCursor(pPrev->GetLength(), nPrevLine);
					Key_Backspace();
				}
				else
				{
					ExecCommand(new COMMAND_CLSNAME(ReverseReturn)(this));
				}
			}
		}
		else
		{
			ptSelStart.x--;
			DoReplace(L"");
			return;
			m_pCurParagraph->Optimiz();
			if (m_pCurParagraph->Arrange())
			{
				// 画这一行
				m_pCurParagraph->ReDrawDown(FALSE);
			}
			else
			{
				m_pCurParagraph->ReDraw(FALSE);
			}

			m_pCurParagraph->AutoUpdate(FALSE);
			UpdateAll();
		}
	}
}

void CTextEditBase::Key_Del()
{
	if (!SelectIsEmpty())
	{
		DoReplace(L"");
	}
	else
	{
		if (m_nRow == (int)m_ParagraphList.size() - 1 &&
			m_nColumn == m_ParagraphList[m_nRow]->GetLength())
		{
			return;
		}

		if (CursorAtLast())
		{
			// 已经在最后面了
			if (m_nRow + 1 < (int)m_ParagraphList.size())
			{
				if (!m_ParagraphList[m_nRow + 1]->IsStiffRet() && m_nColumn == m_pCurParagraph->GetLength())
				{
					// 如果下一行是软回车行，并且光标在行的最末尾
					MoveCursor(1, m_nRow + 1);
				}
				else
				{
					MoveCursor(0, m_nRow + 1);
				}
			}
		}
		else
		{
			MoveCursor(m_nColumn + 1, m_nRow);
		}

		Key_Backspace();
	}
}

void CTextEditBase::UpdateLine(CParagraph *pParagraph, BOOL bReDraw/* = TRUE*/)
{
	if (nLockDraw != 0)
	{
		return;
	}

	TextBlocks::iterator i;
	int nDrawX = pParagraph->GetDrawX(), nDrawY = pParagraph->GetDrawY();
	int xx, yy = RowFromParagraphPtr(pParagraph);

	// 擦除行
	FillSolidRect(0, nDrawY, abs(m_rtRegion.right - m_rtRegion.left) + 1, pParagraph->GetHeight(), RGB(255, 255, 255), FALSE);
/*
	// 画选择区域
	if (!SelectIsEmpty())
	{
		i = pParagraph->m_TextBlockList.begin();
		xx = 0;
		int nSelX1, nSelX2, dx = nDrawX;
		BOOL bEnterSel = FALSE, bSelValid = FALSE;

		while(i != pParagraph->m_TextBlockList.end())
		{ 
			CTextBlock *pTextBlock = *i;

			int nLength = pTextBlock->GetLength();

			for (int j = 0; j < nLength; j++)
			{
				if (bEnterSel)
				{
					if (!InSel(xx, yy))
					{
						bEnterSel = FALSE;
						nSelX2 = dx;
					}
				}
				else
				{
					if (InSel(xx, yy))
					{
						bEnterSel = TRUE;
						bSelValid = TRUE;
						nSelX1 = dx;
					}
				}

				dx += pTextBlock->GetCharWidth(j);
				xx++;
			}

			i++;
		}

		if (bEnterSel && bSelValid)
		{
			nSelX2 = dx;
		}

		if (bSelValid)
		{
			FillSolidRect(nSelX1, nDrawY, (nSelX2 - nSelX1) + 1, pParagraph->GetHeight(), GetSysColor(COLOR_HIGHLIGHT), FALSE);
		}
	}
*/
	// 写文本到画布上
	xx = 0;
	i = pParagraph->m_TextBlockList.begin();

	while(i != pParagraph->m_TextBlockList.end())
	{ 
		CTextBlock *pTextBlock = *i;

		int nLength = pTextBlock->GetLength();
		int nSubDrawX = nDrawX;

		for (int j = 0; j < nLength; j++)
		{
			POINT ptLocate = {xx, yy};
			DrawChar(nSubDrawX, nDrawY, pTextBlock->GetCharWidth(j), pTextBlock->GetHeight(), 
				pTextBlock->nAscent, pParagraph->GetBaseLine(), pParagraph->GetHeight(),
				pTextBlock->strText[j], pTextBlock->TextFormat, &ptLocate, FALSE);
			int nCWidth = pTextBlock->GetCharWidth(j);

			nSubDrawX += nCWidth;
			xx++;
		}

		nDrawX += pTextBlock->GetWidth();
		i++;
	}

	if (bReDraw)
	{
		UpdateAll();
	}
}

void CTextEditBase::GetCharRect(int nColumn, int nRow, RECT &rt)
{
	int x, y, w, h;

	nRow = min(nRow, (int)m_ParagraphList.size());
	CParagraph *pLine = m_ParagraphList[nRow];

	y = pLine->GetDrawY();

	nColumn = min(pLine->GetLength(), nColumn);
	x = pLine->GetDrawX(nColumn);

	CTextBlock *pTextBlock;
	int nInBlockIndex;
	TextBlocks::iterator it = pLine->BlockFromIndex2(nColumn, &nInBlockIndex);
	pTextBlock = *it;

	if (nInBlockIndex < pTextBlock->GetLength())
	{
		w = pTextBlock->GetCharWidth(nInBlockIndex);
	}
	else
	{
		w = 0;
	}

	h = pLine->GetHeight();

	rt.left = x;
	rt.top = y;
	rt.right = x + w;
	rt.bottom = y + h;
}

BOOL CTextEditBase::TE_LButtonDown(UINT nFlags, const POINT &point)
{
	POINT pt = point;

	teMatrix.Invert().Transform(pt.x, pt.y);

	if (PtInRect(&m_rtRegion, pt))
	{
		pt.x -= m_rtRegion.left;
		pt.y -= m_rtRegion.top;
		int nRow, nColumn;

		PixelToTextCrood(pt, nRow, nColumn);

		if (!SHIFT_DOWN())
		{
			if (nRow == m_nRow && m_nColumn == nColumn && SelectIsEmpty())
			{
				// 光标没有移动
				ptSelPos.x = nColumn;
				ptSelPos.y = nRow;
				return TRUE;
			}

			UpdateAll();
			MoveCursor(nColumn, nRow);

			ptSelPos.x = nColumn;
			ptSelPos.y = nRow;
			SetSel(ptSelPos, ptSelPos);
		}
		else
		{
			UpdateAll();
			MoveCursor(nColumn, nRow);

			POINT ptEnd = {nColumn, nRow};
			SetSel(ptSelStart, ptEnd);
		}

		return TRUE;
	}

	return FALSE;
}

void CTextEditBase::TE_LButtonDbClick(UINT nFlags, const POINT &point)
{
	POINT pt = point;

	teMatrix.Invert().Transform(pt.x, pt.y);

	if (PtInRect(&m_rtRegion, pt))
	{
		pt.x -= m_rtRegion.left;
		pt.y -= m_rtRegion.top;
		int nRow, nColumn;

		PixelToTextCrood(pt, nRow, nColumn);
		SelectWord(nColumn, nRow);
	}
}

void CTextEditBase::TE_BeginDrag(UINT nFlags, const POINT &point)
{
}

void CTextEditBase::TE_DragOver(UINT nFlags, const POINT &point)
{
	POINT pt = point;

	teMatrix.Invert().Transform(pt.x, pt.y);

	pt.x -= m_rtRegion.left;
	pt.y -= m_rtRegion.top;

	int nRow, nColumn;

	PixelToTextCrood(pt, nRow, nColumn);
	MoveCursor(nColumn, nRow);

	POINT ptsel = {nColumn, nRow};
	SetSel(ptSelPos, ptsel);
}

void CTextEditBase::TE_DragDrop(UINT nFlags, const POINT &point)
{
}

void CTextEditBase::TE_CancelDrag(UINT nFlags, const POINT & point)
{
}

void CTextEditBase::PixelToTextCrood(const POINT &point, int &nRow, int &nColumn)
{
	nRow = nColumn = 0;

	// 计算行坐标
	if (point.y < 0)
	{
		nRow = 0;
	}
	else
	{
		int nTopY = 0;

		Paragraphs::iterator i = m_ParagraphList.begin();

		while(i != m_ParagraphList.end())
		{
			int nHeight = (*i)->GetHeight();

			if (point.y >= nTopY && point.y < nTopY + nHeight)
			{
				// 就是这一行
				break;
			}

			nTopY += nHeight;
			i++;
			nRow++;
		}

		if (i == m_ParagraphList.end())
		{
			// 没有探测到，则设置为最后一行
			nRow = (int)m_ParagraphList.size() - 1;
		}
	}

	// 计算列坐标
	CParagraph *pParagraph = m_ParagraphList[nRow];
	int nLeftX = pParagraph->GetDrawX();
	CTextBlock *pTextBlock;
	TextBlocks::iterator j = pParagraph->m_TextBlockList.begin();

	if (point.x < nLeftX)
	{
		nColumn = 0;
		return;
	}

	while(j != pParagraph->m_TextBlockList.end())
	{
		pTextBlock = *j;

		if (point.x >= nLeftX && point.x < nLeftX + pTextBlock->GetWidth())
		{
			// 就在这个块内
			int nSubX = nLeftX;

			for (int k = 0; k < pTextBlock->GetLength(); k++)
			{
				if (point.x >= nSubX && point.x < nSubX + pTextBlock->GetCharWidth(k))
				{
					// 就是这个字
					break;
				}

				nColumn++;
				nSubX += pTextBlock->GetCharWidth(k);
			}

			break;
		}

		nLeftX += pTextBlock->GetWidth();
		nColumn += pTextBlock->GetLength();
		j++;
	}

	if (j == pParagraph->m_TextBlockList.end())
	{
		// 没有探测到，设置为最后一列
		nColumn = pParagraph->GetLength();
	}
}

void CTextEditBase::GetSel(POINT &ptStart, POINT &ptEnd)
{
	ptStart = ptSelStart;
	ptEnd = ptSelEnd;
}

void CTextEditBase::AdjustLocate(long &nColumn, long &nRow)
{
	CParagraph *pParagraph;

	if (nRow < 0)
	{
		nRow = 0;
	}

	if (nRow >= (int)m_ParagraphList.size())
	{
/*		nRow = (int)m_ParagraphList.size() - 1;

		if (nRow - 1 >= 0)
		{
			nColumn = m_ParagraphList[nRow - 1]->GetLength();
		}
		*/
		nRow = (int)m_ParagraphList.size() - 1;
		nColumn = m_ParagraphList[nRow]->GetLength();
		return;
	}

	pParagraph = m_ParagraphList[nRow];

	if (nColumn < 0)
	{
		nColumn = 0;
	}

	if (nColumn > pParagraph->GetLength())
	{
		nColumn = pParagraph->GetLength();
	}
}

void CTextEditBase::SetSel(POINT ptStart, POINT ptEnd, BOOL bReDraw/* = TRUE*/)
{
	OnBeforeSelChange();

	AdjustLocate(ptStart.x, ptStart.y);
	AdjustLocate(ptEnd.x, ptEnd.y);

	if (ptStart.y > ptEnd.y)
	{
		swap(ptStart.x, ptEnd.x);
		swap(ptStart.y, ptEnd.y);
	}

	if (ptStart.y == ptEnd.y)
	{
		if (ptStart.x > ptEnd.x)
		{
			swap(ptStart.x, ptEnd.x);
		}
	}

	ptSelStart = ptStart;
	ptSelEnd = ptEnd;

	UpdateAll();
	OnSelChanged();
}

void CTextEditBase::SelToCursor(BOOL bReDraw/* = TRUE*/)
{
	POINT s, e;

	s.x = m_nColumn;
	s.y = m_nRow;

	e = s;
	SetSel(s, e, bReDraw);
}

BOOL CTextEditBase::InSel(int nColumn, int nRow)
{
	if (nRow >= ptSelStart.y && nRow <= ptSelEnd.y)
	{
		CParagraph *pParagraph = m_ParagraphList[nRow];
		int s = 0, e = pParagraph->GetLength();

		if (nRow == ptSelStart.y)
		{
			s = ptSelStart.x;
		}

		if (nRow == ptSelEnd.y)
		{
			e = ptSelEnd.x;
		}

		return nColumn >= s && nColumn < e;
	}

	return FALSE;
}

void CTextEditBase::UpdateLine(int nStartY, int nEndY, BOOL bReDraw/* = TRUE*/)
{
	if (nStartY < 0)
	{
		nStartY = 0;
	}

	if (nStartY >= (int)m_ParagraphList.size())
	{
		nStartY = (int)m_ParagraphList.size() - 1;
	}

	if (nEndY < 0)
	{
		nEndY = 0;
	}

	if (nEndY >= (int)m_ParagraphList.size())
	{
		nEndY = (int)m_ParagraphList.size() - 1;
	}

	for (int i = nStartY; i <= nEndY; i++)
	{
		CParagraph *pParagraph = m_ParagraphList[i];

		UpdateLine(pParagraph, FALSE);
	}

	if (bReDraw)
	{
		UpdateAll();
	}
}

void CTextEditBase::SelectAll()
{
	POINT pts = {0, 0}, 
		pte = {m_ParagraphList[m_ParagraphList.size() - 1]->GetLength(), (int)m_ParagraphList.size() - 1};

	CursorToLast();
	SetSel(pts, pte);
}

void CTextEditBase::SelectWord(int nColumn, int nRow)
{
	try
	{
		CCharIterator it_end = m_ParagraphList[nRow]->End(),
			it_begin = m_ParagraphList[nRow]->Begin();
		CCharIterator it1(m_ParagraphList[nRow], nColumn);
		CCharIterator it2(m_ParagraphList[nRow], nColumn);

		if (it1.IsError())
		{
			return;
		}

		while(it1 != it_end)
		{
			wchar_t ch = *it1;

			if (ch == L' ' || ch == L'\t')
			{
				it1--;
				break;
			}

			it1++;
		}

		if (*it_end == L' ' || *it_end == L'\t')
		{
			it1--;
		}

		while(it2 != it_begin)
		{
			wchar_t ch = *it2;

			if (ch == L' ' || ch == L'\t')
			{
				it2++;
				break;
			}

			it2--;
		}

		if (*it_begin == L' ' || *it_begin == L'\t')
		{
			it2++;
		}

		POINT start = it2.GetXY(), end = it1.GetXY();
		start.x--;
		SetSel(start, end);
		MoveCursor(end.x, end.y);
	}
	catch(...)
	{
	}
}

BOOL CTextEditBase::SelectIsEmpty()
{
	return (ptSelStart.x == ptSelEnd.x && ptSelStart.y == ptSelEnd.y);
}

void CTextEditBase::UpdateSelectRange(BOOL bReDraw/* = TRUE*/)
{
	UpdateLine(ptSelStart.y, ptSelEnd.y, bReDraw);
}

void CTextEditBase::ClearSelect(BOOL bReDraw/* = TRUE*/)
{
	if (memcmp(&ptSelStart, &ptSelEnd, sizeof(POINT)) == 0)
	{
		return;
	}

	int y = ptSelEnd.y;
	ptSelEnd = ptSelStart;
	UpdateLine(ptSelStart.y, y, bReDraw);
	OnSelChanged();
}

void CTextEditBase::Replace(const wchar_t *str, BOOL bReDraw/* = TRUE*/)
{
	// 测试文本框宽度够不够
	CParaFormat pf;
	std::wstring tstr = str;
	int nMaxWidth = -1;

	for (size_t kk = 0; kk < tstr.length(); kk++)
	{
		nMaxWidth = max(nMaxWidth, GetCharWidth(tstr[kk], 0, m_DefTextFormat) + m_DefTextFormat.GetSpacing());
	}

	int nAllMinWidth = 0x7fffffff;
	CParagraph *pMinPara = NULL;

	for (int i = ptSelStart.y; i < ptSelEnd.y; i++)
	{
		if (m_ParagraphList[i]->GetPageWidth2() < nAllMinWidth)
		{
			nAllMinWidth = m_ParagraphList[i]->GetPageWidth2();
			pMinPara = m_ParagraphList[i];
		}
	}

	if (nMaxWidth > nAllMinWidth)
	{
		RECT rtNew = m_rtRegion;
		rtNew.right = pMinPara->GenMinRight(nMaxWidth);
		ExecAndGroup(new COMMAND_CLSNAME(ChangeRegion)(this, rtNew));
	}

	// 开始替换
	CParagraph *pStartPara = m_ParagraphList[ptSelStart.y], 
		*pEndPara = m_ParagraphList[ptSelEnd.y];
	int nsi, nei;
	TextBlocks::iterator it_start = pStartPara->BlockFromIndex(ptSelStart.x, &nsi), 
		it_end = pEndPara->BlockFromIndex(ptSelEnd.x, &nei);
	CTextBlock *pSTextBlock = *it_start, *pETextBlock = *it_end;

	if (ptSelStart.y == ptSelEnd.y)
	{
		if (it_start != it_end)
		{
			std::wstring s_str = pSTextBlock->strText.c_str();
			s_str.assign(s_str.begin(), s_str.begin() + nsi);
			s_str += str;
			pSTextBlock->SetText(s_str.c_str());

			std::wstring e_str = pETextBlock->strText.c_str();
			e_str.assign(e_str.begin() + nei, e_str.end());
			pETextBlock->SetText(e_str.c_str());

			// 删除中间的部分
			it_start++;

			while(it_start != it_end)
			{
				TextBlocks::iterator k = it_start;
				it_start++;
				delete *k;
				pStartPara->m_TextBlockList.erase(k);
			}

			MoveCursor(pStartPara->ColumnFromTextBlockPtr(pSTextBlock) + pSTextBlock->GetLength(), m_nRow, TRUE);
			pStartPara->Optimiz();
		}
		else
		{
			std::wstring s_str, e_str, o_str;

			s_str.assign(pSTextBlock->strText.begin(), pSTextBlock->strText.begin() + nsi);
			e_str.assign(pSTextBlock->strText.begin() + nei, pSTextBlock->strText.end());
			o_str = s_str + str + e_str;
			pSTextBlock->SetText(o_str.c_str());

			MoveCursor(pStartPara->ColumnFromTextBlockPtr(pSTextBlock) + (int)s_str.length() + (int)wcslen(str), m_nRow, TRUE);
			pStartPara->Optimiz();
		}
	}
	else
	{
		// 修改选择开始的行
		std::wstring s_str = pSTextBlock->strText.c_str();
		s_str.assign(s_str.begin(), s_str.begin() + nsi);
		s_str += str;
		pSTextBlock->SetText(s_str.c_str());

		// 删除后面的部分
		it_start++;
		while(it_start != pStartPara->m_TextBlockList.end())
		{
			TextBlocks::iterator k = it_start;
			it_start++;
			delete *k;
			pStartPara->m_TextBlockList.erase(k);
		}

		// 修改选择结束的行
		std::wstring e_str =  pETextBlock->strText.c_str();
		e_str.assign(e_str.begin() + nei, e_str.end());
		pETextBlock->SetText(e_str.c_str());

		// 删除前面的部分
		TextBlocks::iterator i = pEndPara->m_TextBlockList.begin();
		while(i != it_end)
		{
			TextBlocks::iterator k = i;
			i++;
			delete *k;
			pEndPara->m_TextBlockList.erase(k);
		}

		// 删除中间的行
		int nLineCount = ptSelEnd.y - ptSelStart.y - 1;

		for (int j = 0; j < nLineCount; j++)
		{
			CParagraph *pParagraph = m_ParagraphList[ptSelStart.y + 1];
			pParagraph->DeleteNoDraw();
		}

		pStartPara->CalcDownY();
		pEndPara->SetStiffRet(FALSE);

		// 调整光标
		MoveCursor(pStartPara->ColumnFromTextBlockPtr(pSTextBlock) + pSTextBlock->GetLength(), RowFromParagraphPtr(pStartPara), TRUE);
	}

	CParagraph *pHead = GetHeadLine(RowFromParagraphPtr(pStartPara));

//	pStartPara->ReDraw(FALSE);
	pHead->Arrange();
	pHead->ReDrawDown(FALSE);
	MoveCursor();
	SelToCursor(FALSE);

	if (bReDraw)
	{
		UpdateAll();
	}
}

void CTextEditBase::ChangeFormat(CTextFormat &TextFormat, DWORD dwFlag)
{
	int nOldHeight = GetHeight();

/*	if (SelectIsEmpty())
	{
		// 什么都没有选，设置默认文本格式
		m_DefTextFormat.ChangeFormat(TextFormat, dwFlag);
		OnFormatChange();
		return;
	}
*/
	LockUpdate();

	AdjustLocate(ptSelStart.x, ptSelStart.y);
	AdjustLocate(ptSelEnd.x, ptSelEnd.y);

	int newRight = -1;

	{
		CParagraph *pStartPara = m_ParagraphList[ptSelStart.y], 
			*pEndPara = m_ParagraphList[ptSelEnd.y];
		int nsi, nei;
		TextBlocks::iterator it_start = pStartPara->BlockFromIndex(ptSelStart.x, &nsi), 
			it_end = pEndPara->BlockFromIndex(ptSelEnd.x, &nei);

		if (ptSelStart.y == ptSelEnd.y)
		{
			it_end++;
			int nMaxWidth = 0;

			while(it_start != it_end)
			{
				nMaxWidth = max(nMaxWidth, (*it_start)->TryChangeFormat(TextFormat, dwFlag));
				it_start++;
			}

			if (nMaxWidth > pStartPara->GetPageWidth2())
			{
				newRight = max(newRight, pStartPara->GenMinRight(nMaxWidth));
			}
		}
		else
		{
			int nMaxWidth = 0;

			// 计算前面的

			if (nMaxWidth > pStartPara->GetPageWidth2())
			{
				newRight = max(newRight, pStartPara->GenMinRight(nMaxWidth));
			}

			while(it_start != pStartPara->m_TextBlockList.end())
			{
				nMaxWidth = max(nMaxWidth, (*it_start)->TryChangeFormat(TextFormat, dwFlag));
				it_start++;
			}

			// 计算后面的
			if (nMaxWidth > pEndPara->GetPageWidth2())
			{
				newRight = max(newRight, pStartPara->GenMinRight(nMaxWidth));
			}

			it_end++;
			it_start = pEndPara->m_TextBlockList.begin();
			while(it_start != it_end)
			{
				nMaxWidth = max(nMaxWidth, (*it_start)->TryChangeFormat(TextFormat, dwFlag));
				it_start++;
			}

			// 计算中间的
			int nLineCount = ptSelEnd.y - ptSelStart.y - 1;
			TextBlocks::iterator i;

			for (int j = 0; j < nLineCount; j++)
			{
				CParagraph *pParagraph = m_ParagraphList[ptSelStart.y + j + 1];

				if (nMaxWidth > pParagraph->GetPageWidth2())
				{
					newRight = max(newRight, pStartPara->GenMinRight(nMaxWidth));
				}

				i = pParagraph->m_TextBlockList.begin();
				while(i != pParagraph->m_TextBlockList.end())
				{
					nMaxWidth = max(nMaxWidth, (*i)->TryChangeFormat(TextFormat, dwFlag));
					i++;
				}
			}
		}

		if (newRight != -1)
		{
			RECT rtNew = m_rtRegion;
			rtNew.right = newRight;
			ExecAndGroup(new COMMAND_CLSNAME(ChangeRegion)(this, rtNew));
		}
	}

	CParagraph *pStartPara = m_ParagraphList[ptSelStart.y], 
		*pEndPara = m_ParagraphList[ptSelEnd.y];
	int nsi, nei;
	TextBlocks::iterator it_start = pStartPara->BlockFromIndex(ptSelStart.x, &nsi), 
		it_end = pEndPara->BlockFromIndex(ptSelEnd.x, &nei);
	CTextBlock *pSTextBlock = *it_start, *pETextBlock = *it_end;
	CTextBlock *pBefore, *pAfter;
	std::wstring s_str, e_str, o_str, m_str;

	if (ptSelStart.y == ptSelEnd.y)
	{
		if (it_start != it_end)
		{
			// 修改前面部分的
			pSTextBlock->Break(nsi, &pBefore, &pAfter);

			if (pBefore != NULL)
			{
				pSTextBlock->SetText(pBefore->strText.c_str());
				delete pBefore;
			}
			else
			{
				pSTextBlock->SetText(L"");
			}

			it_start++;
			if (pAfter != NULL)
			{
				pAfter->ChangeFormat(TextFormat, dwFlag);
				pStartPara->m_TextBlockList.insert(it_start, pAfter);
				pAfter->SetParent(pStartPara);
			}

			// 修改后面部分的
			pETextBlock->Break(nei, &pBefore, &pAfter);

			if (pAfter != NULL)
			{
				pETextBlock->SetText(pAfter->strText.c_str());
				delete pAfter;
			}
			else
			{
				pETextBlock->SetText(L"");
			}

			if (pBefore != NULL)
			{
				pBefore->ChangeFormat(TextFormat, dwFlag);
				pEndPara->m_TextBlockList.insert(it_end, pBefore);
				pBefore->SetParent(pEndPara);
			}

			// 修改中间的
			while(it_start != it_end)
			{
				(*it_start)->ChangeFormat(TextFormat, dwFlag);
				it_start++;
			}

			pStartPara->CalcHeight();
			pStartPara->CalcDownY();

			if (pStartPara->Arrange())
			{
				pStartPara->ReDrawDown();
			}
			else
			{
				pStartPara->ReDraw();
			}
		}
		else
		{
			s_str.assign(pSTextBlock->strText.begin(), pSTextBlock->strText.begin() + nsi);
			e_str.assign(pETextBlock->strText.begin() + nei, pETextBlock->strText.end());
			o_str = s_str + e_str;

			m_str.assign(pSTextBlock->strText.begin() + nsi, pSTextBlock->strText.begin() + nei);

			pSTextBlock->SetText(o_str.c_str());

			CTextBlock NewBlock((*it_start)->TextFormat, m_str.c_str());
			NewBlock.ChangeFormat(TextFormat, dwFlag);
			pStartPara->Insert(pStartPara->ColumnFromTextBlockPtr(pSTextBlock) + (int)s_str.length(), &NewBlock);
		}

		pStartPara->Optimiz();
		pStartPara->CalcHeight();
	}
	else
	{
		// 修改前面部分的
		if (pStartPara->GetLength() > 0)
		{
			pSTextBlock->Break(nsi, &pBefore, &pAfter);

			if (pBefore != NULL)
			{
				pSTextBlock->SetText(pBefore->strText.c_str());
				delete pBefore;
			}
			else
			{
				pSTextBlock->SetText(L"");
			}

			it_start++;
			if (pAfter != NULL)
			{
				pAfter->ChangeFormat(TextFormat, dwFlag);
				pStartPara->m_TextBlockList.insert(it_start, pAfter);
				pAfter->SetParent(pStartPara);
			}

			TextBlocks::iterator i = it_start;

			while(i != pStartPara->m_TextBlockList.end())
			{
				(*i)->ChangeFormat(TextFormat, dwFlag);
				i++;
			}
		}
		else
		{
			pSTextBlock->ChangeFormat(TextFormat, dwFlag);
		}

		// 修改后面部分的
		if (pEndPara->GetLength() > 0)
		{
			pETextBlock->Break(nei, &pBefore, &pAfter);

			if (pAfter != NULL)
			{
				pETextBlock->SetText(pAfter->strText.c_str());
				delete pAfter;
			}
			else
			{
				pETextBlock->SetText(L"");
			}

			if (pBefore != NULL)
			{
				pBefore->ChangeFormat(TextFormat, dwFlag);
				pEndPara->m_TextBlockList.insert(it_end, pBefore);
				pBefore->SetParent(pEndPara);
			}

			TextBlocks::iterator i = pEndPara->m_TextBlockList.begin();
			while(i != it_end)
			{
				(*i)->ChangeFormat(TextFormat, dwFlag);
				i++;
			}
		}
		else
		{
			pETextBlock->ChangeFormat(TextFormat, dwFlag);
		}

		// 修改中间的行
		int nLineCount = ptSelEnd.y - ptSelStart.y - 1;

		int j = 0;
		for (; j < nLineCount; j++)
		{
			CParagraph *pParagraph = m_ParagraphList[ptSelStart.y + j + 1];
			
			TextBlocks::iterator i = pParagraph->m_TextBlockList.begin();
			while(i != pParagraph->m_TextBlockList.end())
			{
				(*i)->ChangeFormat(TextFormat, dwFlag);
				i++;
			}
		}

		int nOldHeight = 0;
		Paragraphs::iterator k = m_ParagraphList.begin();

		while(k != m_ParagraphList.end())
		{
			nOldHeight += (*k)->GetHeight();
			k++;
		}

		// 重画
		int nProcLine = 0, nUpdate = 0;
		for (j = ptSelStart.y; j < (int)m_ParagraphList.size(); j++)
		{
			m_ParagraphList[j]->Optimiz();
			m_ParagraphList[j]->CalcHeight();
			m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
		}
	}

	UnlockUpdate();
	m_ParagraphList[0]->CalcDownY();

	if (GetHeight() != nOldHeight)
	{
		ReDraw();
	}
	else
	{
		UpdateLine(ptSelStart.y, ptSelEnd.y);
	}

	MoveCursor();

	m_DefTextFormat.ChangeFormat(TextFormat, dwFlag);
	OnFormatChange();
}

void CTextEditBase::SetAlignType(CParagraph::emAlignType type)
{
	ExecCommand(new COMMAND_CLSNAME(Align)(this, type));
}

void CTextEditBase::SetLineSpacing(int nSpacing)
{
	ExecCommand(new COMMAND_CLSNAME(LineSpacing)(this, nSpacing));
}

int CTextEditBase::GetHeight()
{
	int nHeight = 0;

	for (size_t i = 0; i < m_ParagraphList.size(); i++)
	{
		nHeight += m_ParagraphList[i]->GetHeight();
	}

	return nHeight;
}

DWORD CTextEditBase::GetFormat(CTextFormat &TextFormat)
{
	DWORD dwFlag = TF_ALL;

	if (!SelectIsEmpty())
	{
		AdjustLocate(ptSelStart.x, ptSelStart.y);
		AdjustLocate(ptSelEnd.x, ptSelEnd.y);

		CParagraph *pStartPara = m_ParagraphList[ptSelStart.y], 
			*pEndPara = m_ParagraphList[ptSelEnd.y];
		int nsi, nei;
		TextBlocks::iterator it_start = pStartPara->BlockFromIndex(ptSelStart.x, &nsi), 
			it_end = pEndPara->BlockFromIndex(ptSelEnd.x, &nei);
		CTextBlock *pSTextBlock = *it_start, *pETextBlock = *it_end;

		// 取最后一个格式
		TextFormat = pETextBlock->TextFormat;

		// 比较后面的格式
		if (pStartPara == pEndPara)
		{
			// 在同一个文本块
			if (pSTextBlock == pETextBlock)
			{
				TextFormat = pSTextBlock->TextFormat;
			}
			else
			{
				it_end++;

				if (pSTextBlock->GetLength() == nsi)
				{
					it_start++;
				}

				while(it_start != it_end)
				{
					dwFlag = (*it_start)->TextFormat.Compare(TextFormat, dwFlag);
					it_start++;
				}
			}
		}
		else
		{
			if (pSTextBlock->GetLength() == nsi)
			{
				it_start++;
			}

			// 开始行
			while(it_start != pStartPara->m_TextBlockList.end())
			{
				dwFlag = (*it_start)->TextFormat.Compare(TextFormat, dwFlag);
				it_start++;
			}

			// 中间行
			int lcount = (ptSelEnd.y - ptSelStart.y) - 1;
			Paragraphs::iterator j = m_ParagraphList.begin() + ptSelStart.y + 1;
			for (int ii = 0; ii < lcount; ii++)
			{
				TextBlocks::iterator it = (*j)->m_TextBlockList.begin();

				while(it != (*j)->m_TextBlockList.end())
				{
					dwFlag = (*it)->TextFormat.Compare(TextFormat, dwFlag);
					it++;
				}

				j++;
			}

			// 结束行
			TextBlocks::iterator k = pEndPara->m_TextBlockList.begin();
			it_end++;

			while(k != it_end)
			{
				dwFlag = (*k)->TextFormat.Compare(TextFormat, dwFlag);
				k++;
			}
		}
	}
	else
	{
		TextFormat = m_DefTextFormat;
	}

	return dwFlag;
}

void CTextEditBase::SetFontFace(const char *pFontFace)
{
	CTextFormat tf;

	tf.SetFontFace(pFontFace);
	DoChangeFormat(tf, TF_FONTFACE);
}

void CTextEditBase::SetFontSize(int nSize)
{
	CTextFormat tf;

	tf.SetFontSize(nSize);
	DoChangeFormat(tf, TF_FONTSIZE);
}

void CTextEditBase::SetFontColor(COLORREF crFont)
{
	CTextFormat tf;

	tf.SetFontColor(crFont);
	DoChangeFormat(tf, TF_FONTCOLOR);
}

void CTextEditBase::SetFontBold(BOOL bFlag)
{
	CTextFormat tf;

	tf.SetBold(bFlag);
	DoChangeFormat(tf, TF_BOLD);
}

void CTextEditBase::SetFontItalic(BOOL bFlag)
{
	CTextFormat tf;

	tf.SetItalic(bFlag);
	DoChangeFormat(tf, TF_ITALIC);
}

void CTextEditBase::SetCharSpacing(int nCharSpacing)
{
	CTextFormat tf;

	tf.SetSpacing(nCharSpacing);
	DoChangeFormat(tf, TF_SPACING);
}

void CTextEditBase::SetCharLocate(CTextFormat::emCharLocate Locate)
{
	CTextFormat tf;

	tf.SetLocate(Locate);
	DoChangeFormat(tf, TF_LOCATE);
}

void CTextEditBase::SetLink(std::string strLink)
{
	CTextFormat tf;

	tf.SetLink(strLink);
	DoChangeFormat(tf, TF_LINK);
}

void CTextEditBase::SetTarget(std::string strTarget)
{
	CTextFormat tf;

	tf.SetTarget(strTarget);
	DoChangeFormat(tf, TF_TARGET);
}

DWORD CTextEditBase::GetParaFormat(CParaFormat &ParaFormat)
{
	DWORD dwFlag = TF_ALL;
	int nStart = ptSelStart.y, nEnd = ptSelEnd.y;

	GetParagraphs(nStart, nEnd);

	ParaFormat.SetAlignType(m_ParagraphList[nStart]->GetAlignType());
	ParaFormat.SetSpacing(m_ParagraphList[nStart]->GetSpacing());
	ParaFormat.SetMarginLeft(m_ParagraphList[nStart]->GetMarginLeft());
	ParaFormat.SetMarginRight(m_ParagraphList[nStart]->GetMarginRight());
	ParaFormat.SetIndent(m_ParagraphList[nStart]->GetIndent());

	for (int i = nStart + 1; i <= nEnd; i++)
	{
		if (m_ParagraphList[i]->IsStiffRet())
		{
			if (ParaFormat.GetAlignType() != m_ParagraphList[i]->GetAlignType())
			{
				dwFlag &= ~PF_ALIGNTYPE;
			}

			if (ParaFormat.GetSpacing() != m_ParagraphList[i]->GetSpacing())
			{
				dwFlag &= ~PF_SPACING;
			}

			if (ParaFormat.GetMarginLeft() != m_ParagraphList[i]->GetMarginLeft())
			{
				dwFlag &= ~PF_MARGINLEFT;
			}

			if (ParaFormat.GetMarginRight() != m_ParagraphList[i]->GetMarginRight())
			{
				dwFlag &= ~PF_MARGINRIGHT;
			}

			if (m_ParagraphList[i]->IsStiffRet())
			{
				if (ParaFormat.GetIndent() != m_ParagraphList[i]->GetIndent())
				{
					dwFlag &= ~PF_INDENT;
				}
			}
		}
	}

	return dwFlag;
}

void CTextEditBase::GetParagraphs(int &nStart, int &nEnd)
{
	// 寻找开始行
	while(!m_ParagraphList[nStart]->IsStiffRet()) nStart--;

	// 寻找结束行
	if (nEnd + 1 < (int)m_ParagraphList.size())
	{
		do
		{
			if (m_ParagraphList[nEnd + 1]->IsStiffRet())
			{
				break;
			}

			nEnd++;
		}while(nEnd + 1 < (int)m_ParagraphList.size());
	}
}

void CTextEditBase::SetMarginLeft(int nLeft)
{
	ExecCommand(new COMMAND_CLSNAME(MarginLeft)(this, nLeft));
}

void CTextEditBase::SetMarginRight(int nRight)
{
	ExecCommand(new COMMAND_CLSNAME(MarginRight)(this, nRight));
}

void CTextEditBase::SetIndent(int nIndent)
{
	ExecCommand(new COMMAND_CLSNAME(Indent)(this, nIndent));
}

CParagraph *CTextEditBase::GetHeadLine(int nLine)
{
	// 寻找开始行
	while(!m_ParagraphList[nLine]->IsStiffRet()) nLine--;
	return m_ParagraphList[nLine];
}

int CTextEditBase::GetAllMinWidth()
{
	Paragraphs::iterator i = m_ParagraphList.begin();
	int nMax = 0;

	while(i != m_ParagraphList.end())
	{
		nMax = max(nMax, (*i)->GetAdjustMaxWidth());
		i++;
	}

	return nMax + 1;
}

void CTextEditBase::ChangeRegion(RECT rt)
{
	ExecCommand(new COMMAND_CLSNAME(ChangeRegion)(this, rt));
}

void CTextEditBase::ChangeRegion(RECT rt, RECT oldrt)
{
	COMMAND_CLSNAME(ChangeRegion) *pCmd = new COMMAND_CLSNAME(ChangeRegion)(this, rt);
	pCmd->rtOld = oldrt;
	ExecCommand(pCmd);
}

void CTextEditBase::CopyToStream(oBinStream &out)
{
	if (SelectIsEmpty())
	{
		return;
	}

	oBinStream temp_out, *temp_out2;
	int temp;
	CParagraph *pStartPara = m_ParagraphList[ptSelStart.y], 
		*pEndPara = m_ParagraphList[ptSelEnd.y];
	int nsi, nei;
	TextBlocks::iterator it_start = pStartPara->BlockFromIndex(ptSelStart.x, &nsi), 
		it_end = pEndPara->BlockFromIndex(ptSelEnd.x, &nei);
	CTextBlock *pBefore, *pAfter;

	if (ptSelStart.y == ptSelEnd.y)
	{
		if (it_start == it_end)
		{
			CTextBlock *ptb = (*it_start)->GetSub(nsi, nei);

			if (ptb == NULL)
			{
				return;
			}

			out << (int)1;
			out << (int)1;
			ptb->Write(out);

			delete ptb;
		}
		else
		{
			temp = 0;

			// 保存前面半边
			(*it_start)->Break(nsi, &pBefore, &pAfter);

			if (pAfter != NULL)
			{
				pAfter->Write(temp_out);
				temp++;
				delete pAfter;
			}

			if (pBefore != NULL)
			{
				delete pBefore;
			}

			// 保存中间的
			it_start++;

			while(it_start != it_end)
			{
				(*it_start)->Write(temp_out);
				temp++;
				it_start++;
			}

			// 保存后面半边
			if (it_end != pStartPara->m_TextBlockList.end())
			{
				(*it_end)->Break(nei, &pBefore, &pAfter);

				if (pBefore != NULL)
				{
					pBefore->Write(temp_out);
					temp++;
					delete pBefore;
				}

				if (pAfter != NULL)
				{
					delete pAfter;
				}
			}

			// 写单行
			out << (int)1;
			out << temp;
			out.Append(&temp_out);
		}
	}
	else
	{
		int linecount = 0;

		out << linecount;
		temp_out2 = new oBinStream;

		// 写第一行
		temp = 0;

		(*it_start)->Break(nsi, &pBefore, &pAfter);

		if (pAfter != NULL)
		{
			pAfter->Write(*temp_out2);
			temp++;
			delete pAfter;
		}

		if (pBefore != NULL)
		{
			delete pBefore;
		}

		it_start++;
		while(it_start != pStartPara->m_TextBlockList.end())
		{
			(*it_start)->Write(*temp_out2);
			temp++;
			it_start++;
		}

		// 写中间行
		int lcount = (ptSelEnd.y - ptSelStart.y) - 1;
		Paragraphs::iterator j = m_ParagraphList.begin() + ptSelStart.y + 1;
		for (int ii = 0; ii < lcount; ii++)
		{
			TextBlocks::iterator it = (*j)->m_TextBlockList.begin();
			if ((*j)->IsStiffRet())
			{
				out << temp;
				out.Append(temp_out2);
				delete temp_out2;
				temp_out2 = new oBinStream;
				temp = 0;
				linecount++;
			}

			while(it != (*j)->m_TextBlockList.end())
			{
				(*it)->Write(*temp_out2);
				temp++;
				it++;
			}

			j++;
		}

		// 写最后一行
		if (pEndPara->IsStiffRet())
		{
			out << temp;
			out.Append(temp_out2);
			delete temp_out2;
			temp_out2 = new oBinStream;
			temp = 0;
			linecount++;
		}

		TextBlocks::iterator it_tb = pEndPara->m_TextBlockList.begin();
		while(it_tb != it_end)
		{
			(*it_tb)->Write(*temp_out2);
			temp++;
			it_tb++;
		}

		(*it_end)->Break(nei, &pBefore, &pAfter);

		if (pBefore != NULL)
		{
			pBefore->Write(*temp_out2);
			temp++;
			delete pBefore;
		}

		if (pAfter != NULL)
		{
			delete pAfter;
		}

		out << temp;
		out.Append(temp_out2);
		delete temp_out2;
		temp = 0;
		linecount++;

		// 设置行总数
		*((int *)out.Memory()) = linecount;
	}
}

void CTextEditBase::WriteAnsiCharToBinStream(std::string &out, std::wstring wstr)
{
	//MY_USES_CONVERSION;

	out += MY_W2A(wstr.c_str());
}

void CTextEditBase::CopyTextToAnsiString(std::string &out)
{
	if (SelectIsEmpty())
	{
		return;
	}

	CParagraph *pStartPara = m_ParagraphList[ptSelStart.y], 
		*pEndPara = m_ParagraphList[ptSelEnd.y];
	int nsi, nei;
	TextBlocks::iterator it_start = pStartPara->BlockFromIndex(ptSelStart.x, &nsi), 
		it_end = pEndPara->BlockFromIndex(ptSelEnd.x, &nei);
	CTextBlock *pBefore, *pAfter;

	if (ptSelStart.y == ptSelEnd.y)
	{
		if (it_start == it_end)
		{
			CTextBlock *ptb = (*it_start)->GetSub(nsi, nei);

			if (ptb == NULL)
			{
				return;
			}

			WriteAnsiCharToBinStream(out, ptb->strText);
			delete ptb;
		}
		else
		{
			// 保存前面半边
			(*it_start)->Break(nsi, &pBefore, &pAfter);

			if (pBefore != NULL)
			{
				delete pBefore;
			}

			if (pAfter != NULL)
			{
				WriteAnsiCharToBinStream(out, pAfter->strText);
				delete pAfter;
			}

			// 保存中间的
			it_start++;
			while(it_start != it_end)
			{
				WriteAnsiCharToBinStream(out, (*it_start)->strText);
				it_start++;
			}

			// 保存后面半边
			(*it_start)->Break(nei, &pBefore, &pAfter);

			if (pBefore != NULL)
			{
				WriteAnsiCharToBinStream(out, pBefore->strText);
				delete pBefore;
			}

			if (pAfter != NULL)
			{
				delete pAfter;
			}
		}
	}
	else
	{
		int lcount = (ptSelEnd.y - ptSelStart.y) + 1;
		Paragraphs::iterator j = m_ParagraphList.begin() + ptSelStart.y;
		for (int ii = 0; ii < lcount; ii++)
		{
			if ((*j)->IsStiffRet() && ii > 0)
			{
				out += "\r\n";
			}

			if (ii == 0)
			{
				(*it_start)->Break(nsi, &pBefore, &pAfter);

				if (pBefore != NULL)
				{
					delete pBefore;
				}

				if (pAfter != NULL)
				{
					WriteAnsiCharToBinStream(out, pAfter->strText);
					delete pAfter;
				}

				it_start++;
				while(it_start != pStartPara->m_TextBlockList.end())
				{
					WriteAnsiCharToBinStream(out, (*it_start)->strText);
					it_start++;
				}
			}
			else if (ii == lcount - 1)
			{
				TextBlocks::iterator tit = pEndPara->m_TextBlockList.begin();
				while(tit != it_end)
				{
					WriteAnsiCharToBinStream(out, (*tit)->strText);
					tit++;
				}

				(*it_end)->Break(nei, &pBefore, &pAfter);

				if (pBefore != NULL)
				{
					WriteAnsiCharToBinStream(out, pBefore->strText);
					delete pBefore;
				}

				if (pAfter != NULL)
				{
					delete pAfter;
				}

			}
			else
			{
				TextBlocks::iterator it = (*j)->m_TextBlockList.begin();
				while(it != (*j)->m_TextBlockList.end())
				{
					WriteAnsiCharToBinStream(out, (*it)->strText);
					it++;
				}
			}

			j++;
		}
	}
}

void CTextEditBase::Copy()
{
	//MY_USES_CONVERSION;

	oBinStream out;
	std::string out_Text;
	HGLOBAL hGlobal;

	// 写入到剪贴板
	if (!OpenClipboard(AfxGetMainWnd()->GetSafeHwnd()))
	{
		return;
	}

	// 清除剪贴板
	EmptyClipboard();

	// 1.写入自己的格式
	CopyToStream(out);
	hGlobal = GlobalAlloc(GMEM_MOVEABLE, out.Size());
	if (hGlobal != NULL)
	{
		memcpy(GlobalLock(hGlobal), out.Memory(), out.Size());
		GlobalUnlock(hGlobal);
		SetClipboardData(CF_GMOVIE_TEXT2, hGlobal);
	}

	// 2.写CF_TEXT
	CopyTextToAnsiString(out_Text);

	hGlobal = GlobalAlloc(GMEM_MOVEABLE, out_Text.size() + 1);
	if (hGlobal)
	{
		memcpy(GlobalLock(hGlobal), out_Text.c_str(), out_Text.size() + 1);
		GlobalUnlock(hGlobal);
		SetClipboardData(CF_TEXT, hGlobal);
	}

	if (!(GetVersion() & 0x80000000))
	{
		// 3.写CF_UNICODETEXT
		std::wstring out_wText = MY_A2W(out_Text.c_str());
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, (out_wText.length() + 1) * 2);
		if (hGlobal)
		{
			memcpy(GlobalLock(hGlobal), out_wText.c_str(), (out_wText.length() + 1) * 2);
			GlobalUnlock(hGlobal);
			SetClipboardData(CF_UNICODETEXT, hGlobal);
		}
	}

	// 完成
	CloseClipboard();
}

void CTextEditBase::Paste()
{
	CCommandGroup *pCmd = new CCommandGroup;

	pCmd->AddCommand(new COMMAND_CLSNAME(Replace)(this, L""));
	pCmd->AddCommand(new COMMAND_CLSNAME(Paste)(this));

	PushCommand(pCmd);
}

int CTextEditBase::InsertRawData(char *pData, size_t size)
{
	typedef vector <TextBlocks *> ClipboardParas;
	int ccount = 0;

	if (size == 0)
	{
		return 0;
	}

	LockUpdate();

	SelToCursor();

	iBinStream in;

	in.ReadFromMemory((U32)size, (U8 *)pData);

	// 读所有行
	int linecount;
	ClipboardParas paras;
	ClipboardParas::iterator it;

	in >> linecount;
	for (int i = 0; i < linecount; i++)
	{
		int blockcount;

		in >> blockcount;
		TextBlocks *pBlocks = new TextBlocks;

		for (int j = 0; j < blockcount; j++)
		{
			CTextBlock *pBlock = new CTextBlock(CTextFormat());

			pBlock->Read(in);
			pBlocks->push_back(pBlock);
			ccount += pBlock->GetLength();
		}

		paras.push_back(pBlocks);
	}

	// 开始插入
	if (paras.size() == 1)
	{
		// 剪贴板里面只有一行
		CParagraph *pPara = m_ParagraphList[m_nRow];
		TextBlocks::iterator it = paras[0]->begin();

		while(it != paras[0]->end())
		{
			pPara->Insert2(m_nColumn, *it);
			m_nColumn += (*it)->GetLength();
			it++;
		}
	}
	else
	{
		// 有多行
		size_t linecount = paras.size();
		TextBlocks::iterator it;

		for (size_t i = 0; i < linecount; i++)
		{
			it = paras[i]->begin();
			while(it != paras[i]->end())
			{
				m_ParagraphList[m_nRow]->Insert2(m_nColumn, *it);
				m_nColumn += (*it)->GetLength();
				it++;
			}

			if (i != linecount - 1)
			{
				Return();
			}
		}
	}

	// 释放临时行
	it = paras.begin();
	while(it != paras.end())
	{
		TextBlocks::iterator it_tb = (*it)->begin();
		while(it_tb != (*it)->end())
		{
			delete *it_tb;
			it_tb++;
		}

		delete *it;

		it++;
	}

	// 重新计算
	for (size_t i = 0; i < m_ParagraphList.size(); i++)
	{
		CParagraph *pParagraph = m_ParagraphList[i];

		int nProcLine = 0, nUpdate = 0;
		pParagraph->Arrange(&nProcLine, nUpdate);
	}

	UnlockUpdate();

	MoveCursor();
	ReDraw();
	OnFormatChange();

	return ccount;
}

int CTextEditBase::InsertText(const char *pszText)
{
	//MY_USES_CONVERSION;

	if (pszText == NULL)
	{
		return 0;
	}

	int ccount = 0;
	std::vector <std::string> Lines;

	while(*pszText != NULL)
	{
		std::string curline;

		while(*pszText != NULL && *pszText != '\n')
		{
			if (*pszText != '\r')
			{
				curline += *pszText;
			}

			pszText++;
		}

		if (*pszText == '\n')
		{
			pszText++;
		}

		Lines.push_back(curline);
	}

	// 开始插入
	if (Lines.size() == 1)
	{
		// 只有一行
		CParagraph *pPara = m_ParagraphList[m_nRow];
		pPara->Insert2(m_nColumn, MY_A2W(Lines[0].c_str()));
		m_nColumn += (int)Lines[0].length();
	}
	else
	{
		// 有多行
		size_t linecount = Lines.size();

		for (size_t i = 0; i < linecount; i++)
		{
			m_ParagraphList[m_nRow]->Insert2(m_nColumn, MY_A2W(Lines[i].c_str()));
			m_nColumn += (int)Lines[i].length();

			if (i != linecount - 1)
			{
				Return();
			}
		}
	}

	// 重新计算
	for (size_t i = 0; i < m_ParagraphList.size(); i++)
	{
		CParagraph *pParagraph = m_ParagraphList[i];

		int nProcLine = 0, nUpdate = 0;
		pParagraph->Arrange(&nProcLine, nUpdate);
	}

	LockUpdate();
	UnlockUpdate();

	MoveCursor();
	ReDraw();
	OnFormatChange();

	return ccount;
}

void CTextEditBase::Delete()
{
	DoReplace(L"");
}

std::wstring CTextEditBase::GetText()
{
	std::wstring str;

	Paragraphs::iterator i = m_ParagraphList.begin();
	while(i != m_ParagraphList.end())
	{
		if (i != m_ParagraphList.begin())
		{
			str += L'\n';
		}

		str += (*i)->GetText();
		i++;
	}

	return str;
}

BOOL CTextEditBase::IsEmpty()
{
	Paragraphs::iterator i = m_ParagraphList.begin();
	while(i != m_ParagraphList.end())
	{
		if ((*i)->GetText().length())
		{
			return FALSE;
		}

		i++;
	}

	return TRUE;
}

void CTextEditBase::Cut()
{
	Copy();
	Delete();
}

BOOL CTextEditBase::CanPaste()
{
	return ::IsClipboardFormatAvailable(CF_GMOVIE_TEXT2) || 
		::IsClipboardFormatAvailable(CF_TEXT) || ::IsClipboardFormatAvailable(CF_UNICODETEXT);
}

/////////////////////////////////// CCharIterator ///////////////////////////////////

CCharIterator CParagraph::Begin()
{
	CParagraph *pHeadLine = pEditBase->GetHeadLine(pEditBase->RowFromParagraphPtr(this));
	return CCharIterator(pHeadLine, 0);
}

CCharIterator CParagraph::End()
{
	int nRow = pEditBase->RowFromParagraphPtr(this);

	while(nRow + 1 < (int)pEditBase->m_ParagraphList.size())
	{
		if (pEditBase->m_ParagraphList[nRow + 1]->IsStiffRet())
		{
			break;
		}

		nRow++;
	}

	return CCharIterator(pEditBase->m_ParagraphList[nRow],
		pEditBase->m_ParagraphList[nRow]->GetLength() - 1);
}

bool CCharIterator::IsError()
{
	bool ret;

	try
	{
		ret = ((*TextBlockIt)->GetLength() == nInBlockIndex);
	}
	catch(...)
	{
		ret = true;
	}

	return ret;
}

CCharIterator::CCharIterator(CParagraph *para, int pos)
{
	pParagraph = para;
	TextBlockIt = pParagraph->BlockFromIndex(pos, &nInBlockIndex);

	if (IsError())
	{
		TextBlocks::iterator it_next = TextBlockIt;
		it_next++;

		if (it_next != pParagraph->m_TextBlockList.end())
		{
			TextBlockIt = it_next;
			nInBlockIndex = 0;
		}
		else
		{
			(*this)++;
		}
	}
}

CCharIterator::~CCharIterator()
{
}

CCharIterator &CCharIterator::operator =(CCharIterator &it)
{
	this->TextBlockIt = it.TextBlockIt;
	this->pParagraph = it.pParagraph;
	this->nInBlockIndex = it.nInBlockIndex;

	return *this;
}

bool CCharIterator::operator ==(CCharIterator &it)
{
	return (this->TextBlockIt == it.TextBlockIt &&
		this->pParagraph == it.pParagraph &&
		this->nInBlockIndex == it.nInBlockIndex);
}

bool CCharIterator::operator !=(CCharIterator &it)
{
	return (this->TextBlockIt != it.TextBlockIt ||
		this->pParagraph != it.pParagraph ||
		this->nInBlockIndex != it.nInBlockIndex);
}

CCharIterator &CCharIterator::operator ++(int i)
{
	if (TextBlockIt == pParagraph->m_TextBlockList.end())
	{
		return *this;
	}

	if (nInBlockIndex >= (*TextBlockIt)->GetLength() - 1)
	{
		while(true)
		{
			TextBlockIt++;

			if (TextBlockIt != pParagraph->m_TextBlockList.end())
			{
				if ((*TextBlockIt)->GetLength() == 0)
				{
					// 下一个文本块长度为0
					// 继续到下一个
					continue;
				}

				// 找到符合要求的文本块了
				// 结束寻找
				nInBlockIndex = 0;
				break;
			}
			else
			{
				int nRow = pParagraph->GetEditBase()->RowFromParagraphPtr(pParagraph);
				nRow++;

				if (nRow >= (int)pParagraph->GetEditBase()->m_ParagraphList.size())
				{
					// 是最后一行
					// 中断寻找
					nInBlockIndex = 0;
					break;
				}
				else
				{
					if (pParagraph->GetEditBase()->m_ParagraphList[nRow]->IsStiffRet())
					{
						// 下一行是硬回车行
						// 中断寻找
						break;
					}
					else
					{
						// 转移到下一行
						pParagraph = pParagraph->GetEditBase()->m_ParagraphList[nRow];
						TextBlockIt = pParagraph->m_TextBlockList.begin();

						if ((*TextBlockIt)->GetLength())
						{
							nInBlockIndex = 0;
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		nInBlockIndex++;
	}

	return *this;
}

CCharIterator &CCharIterator::operator --(int i)
{
	if (nInBlockIndex <= 0)
	{
		while(true)
		{
			if (TextBlockIt == pParagraph->m_TextBlockList.begin())
			{
				if (pParagraph->IsStiffRet())
				{
					// 这是Head行
					break;
				}

				// 在上一行找
				int nRow = pParagraph->GetEditBase()->RowFromParagraphPtr(pParagraph);
				nRow--;

				if (nRow < 0)
				{
					// 没有上一行了
					break;
				}
				else
				{
					pParagraph = pParagraph->GetEditBase()->m_ParagraphList[nRow];
					TextBlockIt = pParagraph->m_TextBlockList.end();
					TextBlockIt--;

					if ((*TextBlockIt)->GetLength())
					{
						nInBlockIndex = (*TextBlockIt)->GetLength() - 1;
						break;
					}
				}
			}
			else
			{
				TextBlockIt--;

				if ((*TextBlockIt)->GetLength() == 0)
				{
					// 上一个文本块长度为0
					// 继续到上一个
					continue;
				}

				// 找到了
				nInBlockIndex = (*TextBlockIt)->GetLength() - 1;
				break;
			}
		}
	}
	else
	{
		nInBlockIndex--;
	}

	return *this;
}

wchar_t CCharIterator::operator *()
{
	return nInBlockIndex < (*TextBlockIt)->GetLength() ? (*TextBlockIt)->strText[nInBlockIndex] : 0;
}

POINT CCharIterator::GetXY()
{
	POINT pt;
	
	pt.x = pParagraph->ColumnFromTextBlockPtr(*TextBlockIt) + nInBlockIndex + 1;
	pt.y = pParagraph->GetEditBase()->RowFromParagraphPtr(pParagraph);

	return pt;
}

/////////////////////////////////// Commands ///////////////////////////////////

void CTECommand::BackupPos()
{
	// 备份原始光标位置
	ptOldCursor.x = pTEB->m_nColumn;
	ptOldCursor.y = pTEB->m_nRow;

	// 备份原始选择位置
	ptOldSelStart = pTEB->ptSelStart;
	ptOldSelEnd = pTEB->ptSelEnd;
}

void CTECommand::RestorePos(BOOL bUpdate/* = TRUE*/)
{
	if (!bUpdate)
	{
		pTEB->LockUpdate();
		pTEB->LockSendUpdate();
	}

	// 恢复原始光标位置
	pTEB->MoveCursor(ptOldCursor.x, ptOldCursor.y);

	// 恢复原始选择位置
	pTEB->SetSel(ptOldSelStart, ptOldSelEnd, TRUE);

	if (!bUpdate)
	{
		pTEB->UnlockUpdate();
		pTEB->UnlockSendUpdate();
	}
}

// Replace Cmd
COMMAND_CLSNAME(Replace)::COMMAND_CLSNAME(Replace)(CTextEditBase *base, const wchar_t *str) : CTECommand(base)
{
	strReplace = str;

	// 备份原始数据
	pTEB->CopyToStream(data);
}

void COMMAND_CLSNAME(Replace)::Execute()
{
	CTextFormat tfOld = pTEB->m_DefTextFormat;

	RestorePos(FALSE);

	// 替换
	pTEB->LockUpdate();
	pTEB->LockSendUpdate();
	pTEB->Replace(strReplace.c_str());

	ptEnd.x = pTEB->m_nColumn;
	ptEnd.y = pTEB->m_nRow;

	if (!(pTEB->m_DefTextFormat == pTEB->m_DefTextFormat1))
	{
		pTEB->SetSel(ptOldSelStart, ptEnd, FALSE);
		pTEB->ChangeFormat(tfOld, TF_ALL);
	}

	pTEB->UnlockUpdate();
	pTEB->UnlockSendUpdate();

	pTEB->SelToCursor();
}

void COMMAND_CLSNAME(Replace)::UnExecute()
{
	pTEB->LockUpdate();
	pTEB->SetSel(ptOldSelStart, ptEnd, FALSE);

	if (data.Size() > 0)
	{
		pTEB->Replace(L"", FALSE);
		pTEB->InsertRawData((char *)data.Memory(), data.Size());
	}
	else
	{
		pTEB->Replace(L"");
	}
	
	pTEB->UnlockUpdate();
	pTEB->SelToCursor();

	RestorePos();
}

void CTEInvoker::DoReplace(const wchar_t *str)
{
	ExecCommand(new COMMAND_CLSNAME(Replace)((CTextEditBase *)this, str));
}

// ChangeFmt
COMMAND_CLSNAME(ChangeFmt)::COMMAND_CLSNAME(ChangeFmt)(CTextEditBase *base, DWORD dwFlag, CTextFormat fmt) : CTECommand(base)
{
	this->fmt = fmt;
	this->dwFlag = dwFlag;

	// 备份原始数据
	pTEB->CopyToStream(data);
}

void COMMAND_CLSNAME(ChangeFmt)::Execute()
{
	RestorePos(FALSE);

	// 更改格式
	pTEB->ChangeFormat(fmt, dwFlag);
	ptSNew = pTEB->ptSelStart;
	ptENew = pTEB->ptSelEnd;
}

void COMMAND_CLSNAME(ChangeFmt)::UnExecute()
{
	pTEB->LockUpdate();
	pTEB->SetSel(ptSNew, ptENew, FALSE);
	pTEB->Replace(L"", FALSE);
	pTEB->InsertRawData((char *)data.Memory(), data.Size());
	pTEB->UnlockUpdate();
	pTEB->ReDraw();

	RestorePos();
}

void CTEInvoker::DoChangeFormat(CTextFormat &TextFormat, DWORD dwFlag/* = TF_ALL*/)
{
	ExecCommand(new COMMAND_CLSNAME(ChangeFmt)((CTextEditBase *)this, dwFlag, TextFormat));
}

// Return
COMMAND_CLSNAME(Return)::COMMAND_CLSNAME(Return)(CTextEditBase *base) : CTECommand(base)
{
}

void COMMAND_CLSNAME(Return)::Execute()
{
	RestorePos(FALSE);

	pTEB->Return();
	pTEB->ReDraw();

	nLine = pTEB->m_nRow;
}

void COMMAND_CLSNAME(Return)::UnExecute()
{
	pTEB->m_ParagraphList[nLine]->SetStiffRet(FALSE);
	pTEB->m_ParagraphList[nLine]->Arrange();
	pTEB->ReDraw();

	RestorePos();
}

// Paste
COMMAND_CLSNAME(Paste)::COMMAND_CLSNAME(Paste)(CTextEditBase *base) : CTECommand(base)
{
	//MY_USES_CONVERSION;
	HGLOBAL hGlobal;

	OpenClipboard(AfxGetMainWnd()->GetSafeHwnd());

	// 自己格式的数据
	hGlobal = GetClipboardData(CF_GMOVIE_TEXT2);

	if (hGlobal != NULL)
	{
		size_t size = GlobalSize(hGlobal);
		char *pData = (char *)GlobalLock(hGlobal);

		data.ReadFromMemory((U32)size, (U8 *)pData);

		GlobalUnlock(hGlobal);
		nFormat = CF_GMOVIE_TEXT2;
		CloseClipboard();
		return;
	}

	// UNICODE纯文本数据
	if (!(GetVersion() & 0x80000000))
	{
		hGlobal = GetClipboardData(CF_UNICODETEXT);

		if (hGlobal != NULL)
		{
			size_t size = GlobalSize(hGlobal);
			char *p = (char *)GlobalLock(hGlobal);
			char *pData = MY_W2A((wchar_t *)/*GlobalLock(hGlobal)*/p);

			data.ReadFromMemory((U32)strlen(pData) + 1, (U8 *)pData);

			GlobalUnlock(hGlobal);
			nFormat = CF_TEXT;
			CloseClipboard();
			return;
		}
	}

	// 纯文本数据
	hGlobal = GetClipboardData(CF_TEXT);

	if (hGlobal != NULL)
	{
		size_t size = GlobalSize(hGlobal);
		char *pData = (char *)GlobalLock(hGlobal);

		data.ReadFromMemory((U32)size, (U8 *)pData);

		GlobalUnlock(hGlobal);
		nFormat = CF_TEXT;
		CloseClipboard();
		return;
	}
}

void COMMAND_CLSNAME(Paste)::Execute()
{
	RestorePos(FALSE);

	if (data.Size() > 0)
	{
		// 插入到光标位置
		if (nFormat == CF_GMOVIE_TEXT2)
		{
			pTEB->InsertRawData((char *)data.Memory(), data.Size());
		}
		else if (nFormat == CF_TEXT)
		{
			pTEB->InsertText((char *)data.Memory());
		}

		ptEnd.x = pTEB->m_nColumn;
		ptEnd.y = pTEB->m_nRow;
		pTEB->SelToCursor();
	}
}

void COMMAND_CLSNAME(Paste)::UnExecute()
{
	pTEB->LockUpdate();
	pTEB->SetSel(ptOldSelStart, ptEnd, FALSE);
	pTEB->Replace(L"", FALSE);
	pTEB->SelToCursor(FALSE);
	pTEB->UnlockUpdate();
	pTEB->UpdateAll();

	RestorePos();
}

// MarginLeft
COMMAND_CLSNAME(MarginLeft)::COMMAND_CLSNAME(MarginLeft)(CTextEditBase *base, int nMarginLeft) : CTECommand(base)
{
	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	nNewLeft = max(0, nMarginLeft);

	pTEB->GetParagraphs(nStart, nEnd);
	for (int i = nStart; i <= nEnd; i++)
	{
		if (pTEB->m_ParagraphList[i]->IsStiffRet())
		{
			OldMarginLeft.push_back(pTEB->m_ParagraphList[i]->GetMarginLeft());
			OldMarginRight.push_back(pTEB->m_ParagraphList[i]->GetMarginRight());
			OldIndent.push_back(pTEB->m_ParagraphList[i]->GetIndent());
		}
	}
}

void COMMAND_CLSNAME(MarginLeft)::Execute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	int nLeft = nNewLeft;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		int oin = pTEB->m_ParagraphList[i]->IsStiffRet() ? pTEB->m_ParagraphList[i]->GetIndent() : 0;

		int cw = pTEB->GetAllMinWidth();
		int w = (pTEB->m_rtRegion.right - pTEB->m_rtRegion.left + 1) - BORDER_SIZE * 2;
		int lm = min(nLeft, w - cw - oin);
		int rmp = w - pTEB->m_ParagraphList[i]->GetMarginRight();

		lm = max(lm, -oin);

		if (lm + cw > rmp)
		{
			pTEB->m_ParagraphList[i]->SetMarginRight(pTEB->m_ParagraphList[i]->GetMarginRight() -
				(lm - rmp) - cw);
		}

		pTEB->m_ParagraphList[i]->SetMarginLeft(lm);

		if (pTEB->m_ParagraphList[i]->GetPageWidth2() < cw)
		{
			pTEB->m_ParagraphList[i]->SetMarginLeft(pTEB->m_ParagraphList[i]->GetMarginLeft() - 
				(cw - pTEB->m_ParagraphList[i]->GetPageWidth2()));
		}
	}

	int nProcLine = 0, nUpdate = 0;
	for (int j = nStart; j < (int)pTEB->m_ParagraphList.size(); j++)
	{
		pTEB->m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
	}

	ptSNew = pTEB->ptSelStart;
	ptENew = pTEB->ptSelEnd;

	pTEB->ReDraw();
	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

void COMMAND_CLSNAME(MarginLeft)::UnExecute()
{
	int nStart = ptSNew.y, nEnd = ptENew.y;
	int k = -1;
	pTEB->GetParagraphs(nStart, nEnd);

	for (int i = nStart; i <= nEnd; i++)
	{
		if (pTEB->m_ParagraphList[i]->IsStiffRet())
		{
			k++;
		}

		pTEB->m_ParagraphList[i]->SetMarginLeft(OldMarginLeft[k]);
		pTEB->m_ParagraphList[i]->SetMarginRight(OldMarginRight[k]);
		pTEB->m_ParagraphList[i]->SetIndent(OldIndent[k]);
	}

	int nProcLine = 0, nUpdate = 0;
	for (int j = nStart; j < (int)pTEB->m_ParagraphList.size(); j++)
	{
		pTEB->m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
	}

	RestorePos();
	pTEB->ReDraw();
	pTEB->OnFormatChange();
}

// MarginRight
COMMAND_CLSNAME(MarginRight)::COMMAND_CLSNAME(MarginRight)(CTextEditBase *base, int nMarginRight) : CTECommand(base)
{
	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	nNewRight = max(0, nMarginRight);

	pTEB->GetParagraphs(nStart, nEnd);
	for (int i = nStart; i <= nEnd; i++)
	{
		if (pTEB->m_ParagraphList[i]->IsStiffRet())
		{
			OldMarginLeft.push_back(pTEB->m_ParagraphList[i]->GetMarginLeft());
			OldMarginRight.push_back(pTEB->m_ParagraphList[i]->GetMarginRight());
			OldIndent.push_back(pTEB->m_ParagraphList[i]->GetIndent());
		}
	}
}

void COMMAND_CLSNAME(MarginRight)::Execute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	int nRight = nNewRight;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		int oin = pTEB->m_ParagraphList[i]->IsStiffRet() ? pTEB->m_ParagraphList[i]->GetIndent() : 0;

		int cw = pTEB->GetAllMinWidth();
		int w = (pTEB->m_rtRegion.right - pTEB->m_rtRegion.left + 1) - BORDER_SIZE * 2;
		int rm = min(nRight, w - cw + oin);
		//int rm = min(nRight, w - cw - pTEB->m_ParagraphList[i]->GetIndent());
		int rmp = w - rm;
		int lmp = pTEB->m_ParagraphList[i]->GetMarginLeft() - 1;

		if (rmp - cw < lmp)
		{
			pTEB->m_ParagraphList[i]->SetMarginLeft(pTEB->m_ParagraphList[i]->GetMarginLeft() -
				(lmp - (rmp - cw)));
		}

		pTEB->m_ParagraphList[i]->SetMarginRight(rm);

		if (pTEB->m_ParagraphList[i]->GetPageWidth2() < cw)
		{
			pTEB->m_ParagraphList[i]->SetMarginRight(pTEB->m_ParagraphList[i]->GetMarginRight() - 
				(cw - pTEB->m_ParagraphList[i]->GetPageWidth2()));
		}
	}

	int nProcLine = 0, nUpdate = 0;
	for (int j = nStart; j < (int)pTEB->m_ParagraphList.size(); j++)
	{
		pTEB->m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
	}

	ptSNew = pTEB->ptSelStart;
	ptENew = pTEB->ptSelEnd;

	pTEB->ReDraw();
	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

void COMMAND_CLSNAME(MarginRight)::UnExecute()
{
	int nStart = ptSNew.y, nEnd = ptENew.y;
	int k = -1;

	pTEB->GetParagraphs(nStart, nEnd);

	for (int i = nStart; i <= nEnd; i++)
	{
		if (pTEB->m_ParagraphList[i]->IsStiffRet())
		{
			k++;
		}

		pTEB->m_ParagraphList[i]->SetMarginLeft(OldMarginLeft[k]);
		pTEB->m_ParagraphList[i]->SetMarginRight(OldMarginRight[k]);
		pTEB->m_ParagraphList[i]->SetIndent(OldIndent[k]);
	}

	int nProcLine = 0, nUpdate = 0;
	for (int j = nStart; j < (int)pTEB->m_ParagraphList.size(); j++)
	{
		pTEB->m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
	}

	RestorePos();
	pTEB->ReDraw();
	pTEB->OnFormatChange();
}

// Indent
COMMAND_CLSNAME(Indent)::COMMAND_CLSNAME(Indent)(CTextEditBase *base, int nIndent) : CTECommand(base)
{
	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	nNewIndent = nIndent;

	pTEB->GetParagraphs(nStart, nEnd);
	for (int i = nStart; i <= nEnd; i++)
	{
		if (pTEB->m_ParagraphList[i]->IsStiffRet())
		{
			OldMarginLeft.push_back(pTEB->m_ParagraphList[i]->GetMarginLeft());
			OldMarginRight.push_back(pTEB->m_ParagraphList[i]->GetMarginRight());
			OldIndent.push_back(pTEB->m_ParagraphList[i]->GetIndent());
		}
	}
}

void COMMAND_CLSNAME(Indent)::Execute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	int nIndent = nNewIndent;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		int cw = pTEB->GetAllMinWidth();
		int olm = pTEB->m_ParagraphList[i]->GetMarginLeft(),
			orm = pTEB->m_ParagraphList[i]->GetMarginRight();
		int in = max(nIndent, -olm);
		int w = (pTEB->m_rtRegion.right - pTEB->m_rtRegion.left + 1) - BORDER_SIZE * 2;
		in = min(w - (olm + orm + cw), in);

		pTEB->m_ParagraphList[i]->SetIndent(in);
	}

	int nProcLine = 0, nUpdate = 0;
	for (int j = nStart; j < (int)pTEB->m_ParagraphList.size(); j++)
	{
		pTEB->m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
	}

	ptSNew = pTEB->ptSelStart;
	ptENew = pTEB->ptSelEnd;

	pTEB->ReDraw();
	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

void COMMAND_CLSNAME(Indent)::UnExecute()
{
	int nStart = ptSNew.y, nEnd = ptENew.y;
	int k = -1;

	pTEB->GetParagraphs(nStart, nEnd);

	for (int i = nStart; i <= nEnd; i++)
	{
		if (pTEB->m_ParagraphList[i]->IsStiffRet())
		{
			k++;
		}

		pTEB->m_ParagraphList[i]->SetMarginLeft(OldMarginLeft[k]);
		pTEB->m_ParagraphList[i]->SetMarginRight(OldMarginRight[k]);
		pTEB->m_ParagraphList[i]->SetIndent(OldIndent[k]);
	}

	int nProcLine = 0, nUpdate = 0;
	for (int j = nStart; j < (int)pTEB->m_ParagraphList.size(); j++)
	{
		pTEB->m_ParagraphList[j]->Arrange(&nProcLine, nUpdate);
	}

	RestorePos();
	pTEB->ReDraw();
	pTEB->OnFormatChange();
}

// Align
COMMAND_CLSNAME(Align)::COMMAND_CLSNAME(Align)(CTextEditBase *base, CParagraph::emAlignType Align) : CTECommand(base)
{
	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	NewAlign = Align;

	pTEB->GetParagraphs(nStart, nEnd);
	for (int i = nStart; i <= nEnd; i++)
	{
		OldAlign.push_back(pTEB->m_ParagraphList[i]->GetAlignType());
	}
}

void COMMAND_CLSNAME(Align)::Execute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		pTEB->m_ParagraphList[i]->SetAlignTypeType(NewAlign);
	}

	pTEB->ReDraw();
	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

void COMMAND_CLSNAME(Align)::UnExecute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		pTEB->m_ParagraphList[i]->SetAlignTypeType(OldAlign[i - nStart]);
	}

	pTEB->ReDraw();
	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

// LineSpacing
COMMAND_CLSNAME(LineSpacing)::COMMAND_CLSNAME(LineSpacing)(CTextEditBase *base, int nSpacing) : CTECommand(base)
{
	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;
	NewSpacing = nSpacing;

	pTEB->GetParagraphs(nStart, nEnd);
	for (int i = nStart; i <= nEnd; i++)
	{
		OldSpacing.push_back(pTEB->m_ParagraphList[i]->GetSpacing());
	}
}

void COMMAND_CLSNAME(LineSpacing)::Execute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		pTEB->m_ParagraphList[i]->SetSpacing(NewSpacing);
		pTEB->m_ParagraphList[i]->CalcHeight();
	}

	pTEB->m_ParagraphList[0]->CalcDownY();
	pTEB->ReDraw();
	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

void COMMAND_CLSNAME(LineSpacing)::UnExecute()
{
	RestorePos();

	int nStart = ptOldSelStart.y, nEnd = ptOldSelEnd.y;

	pTEB->GetParagraphs(nStart, nEnd);

	// 修改区域内的行
	for (int i = nStart; i <= nEnd; i++)
	{
		pTEB->m_ParagraphList[i]->SetSpacing(OldSpacing[i - nStart]);
		pTEB->m_ParagraphList[i]->CalcHeight();
	}

	pTEB->m_ParagraphList[0]->CalcDownY();
	pTEB->ReDraw();

	pTEB->MoveCursor();
	pTEB->OnFormatChange();
}

// ChangeRegion
COMMAND_CLSNAME(ChangeRegion)::COMMAND_CLSNAME(ChangeRegion)(CTextEditBase *base, RECT rt) : CTECommand(base)
{
	rtNew = rt;
	rtOld = pTEB->m_rtRegion;
	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
}

void COMMAND_CLSNAME(ChangeRegion)::Execute()
{
	ChangeRegion(rtNew);
}

void COMMAND_CLSNAME(ChangeRegion)::UnExecute()
{
	ChangeRegion(rtOld);
}

void COMMAND_CLSNAME(ChangeRegion)::ChangeRegion(RECT rt)
{
	pTEB->LockUpdate();

	pTEB->m_rtRegion = rt;
	rt.right = max(rt.left + pTEB->GetAllMinWidth(), rt.right);

	for (size_t i = 0; i < pTEB->m_ParagraphList.size(); i++)
	{
		CParagraph *pParagraph = pTEB->m_ParagraphList[i];

		int nProcLine = 0, nUpdate = 0;
		pParagraph->Arrange(&nProcLine, nUpdate);
	}

	pEditTool->CheckBound();
	pTEB->UnlockUpdate();
	pTEB->MoveCursor(TRUE);
	pEditTool->RepaintDesignWnd();
	pTEB->ReDraw();
}

// ReverseReturn Command
COMMAND_CLSNAME(ReverseReturn)::COMMAND_CLSNAME(ReverseReturn)(CTextEditBase *base) : CTECommand(base)
{
}

void COMMAND_CLSNAME(ReverseReturn)::Execute()
{
	CParagraph *pPrev = pTEB->m_ParagraphList[pTEB->m_nRow - 1];
	pTEB->m_pCurParagraph->SetStiffRet(FALSE);
	pPrev->Arrange();
	pPrev->ReDrawDown(TRUE);
	nRetCurX = pTEB->m_nColumn;
	nRetCurY = pTEB->m_nRow;
}

void COMMAND_CLSNAME(ReverseReturn)::UnExecute()
{
	pTEB->MoveCursor(nRetCurX, nRetCurY);
	pTEB->Return();
}

////////////////////////////////////////////////////////////////////////////////

// 更改文本类型的Command，只能时TextTool使用
COMMAND_CLSNAME(ChangeTextType)::COMMAND_CLSNAME(ChangeTextType)(CTextEditBase *base, gldText2_TEXTTYPE type) : CTECommand(base)
{
	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	TEXT_PROPERTY prop;

	pEditTool->GetTextProperty(prop);
	oldtype = prop.TextType;

	// 保存
	int linecount = (int)pEditTool->m_ParagraphList.size();

	data << linecount;

	for (int i = 0; i < (int)pEditTool->m_ParagraphList.size(); i++)
	{
		pEditTool->m_ParagraphList[i]->Write(data);
	}

	newtype = type;
}

void COMMAND_CLSNAME(ChangeTextType)::Execute()
{
	#define GETFLAG(v, f) ((v & f) == f)

	pEditTool->TextProperty.TextType = newtype;

	TTCAPS caps = pEditTool->GetTextToolCaps();

	pEditTool->LockUpdate();
	pEditTool->SelectAll();

	CTextFormat fmt, FirstFmt = pEditTool->FirstTB()->TextFormat;
	CParaFormat FirstPFmt;
	DWORD dwFlag = 0;

	pEditTool->FirstLine()->GetParaFormat(FirstPFmt);

	// &&&&&&&&&&&&& 取消部分属性 &&&&&&&&&&&&&&&&&&&&&&&&&&
	// 取消字间距
	if (!GETFLAG(caps.dwCaps, TTCAPS_CHARSPACING))
	{
		fmt.SetSpacing(0);
		dwFlag |= TF_SPACING;
	}

	// 取消上标下标
	if (!GETFLAG(caps.dwCaps, TTCAPS_LOCATE))
	{
		fmt.SetLocate(CTextFormat::CL_NONE);
		dwFlag |= TF_LOCATE;
	}

	// 取消链接
	if (!GETFLAG(caps.dwCaps, TTCAPS_LINK))
	{
		fmt.SetLink(std::string());
		dwFlag |= TF_LINK;
	}

	// &&&&&&&&&&&&& 同化部分属性 &&&&&&&&&&&&&&&&&&&&&&&&&&
	if (!GETFLAG(caps.dwAllowAS, TTCAPS_FONTFACE))
	{
		fmt.SetFontFace(FirstFmt.GetFontFace());
		dwFlag |= TF_FONTFACE;
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_FONTSIZE))
	{
		fmt.SetFontSize(FirstFmt.GetFontSize());
		dwFlag |= TF_FONTSIZE;
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_FONTCOLOR))
	{
		fmt.SetFontColor(FirstFmt.GetFontColor());
		dwFlag |= TF_FONTCOLOR;
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_BOLD))
	{
		fmt.SetBold(FirstFmt.GetBold());
		dwFlag |= TF_BOLD;
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_ITALIC))
	{
		fmt.SetItalic(FirstFmt.GetItalic());
		dwFlag |= TF_ITALIC;
	}

	//////////////////////////////////////////
	if (!GETFLAG(caps.dwAllowAS, TTCAPS_LINESPACING))
	{
		Paragraphs::iterator i = pEditTool->m_ParagraphList.begin();

		while(i != pEditTool->m_ParagraphList.end())
		{
			(*i)->SetSpacing(FirstPFmt.GetSpacing());
			i++;
		}
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_INDENT))
	{
		Paragraphs::iterator i = pEditTool->m_ParagraphList.begin();

		while(i != pEditTool->m_ParagraphList.end())
		{
			(*i)->SetIndent(FirstPFmt.GetIndent());
			i++;
		}
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_MARGINLEFT))
	{
		Paragraphs::iterator i = pEditTool->m_ParagraphList.begin();

		while(i != pEditTool->m_ParagraphList.end())
		{
			(*i)->SetMarginLeft(FirstPFmt.GetMarginLeft());
			i++;
		}
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_MARGINRIGHT))
	{
		Paragraphs::iterator i = pEditTool->m_ParagraphList.begin();

		while(i != pEditTool->m_ParagraphList.end())
		{
			(*i)->SetMarginRight(FirstPFmt.GetMarginRight());
			i++;
		}
	}

	if (!GETFLAG(caps.dwAllowAS, TTCAPS_ALIGN))
	{
		Paragraphs::iterator i = pEditTool->m_ParagraphList.begin();

		while(i != pEditTool->m_ParagraphList.end())
		{
			(*i)->SetAlignTypeType(FirstPFmt.GetAlignType());
			i++;
		}
	}

	// 更改格式
	pEditTool->m_ParagraphList[0]->CalcDownY();
	pEditTool->ChangeFormat(fmt, dwFlag);
	pEditTool->UnlockUpdate();
	pEditTool->UpdateAll();

	#undef GETFLAG
}

void COMMAND_CLSNAME(ChangeTextType)::UnExecute()
{
	Paragraphs::iterator j = pEditTool->m_ParagraphList.begin();
	while(j != pEditTool->m_ParagraphList.end())
	{
		delete *j;
		j++;
	}
	pEditTool->m_ParagraphList.clear();

	// 读所有行
	iBinStream is;

	U8 *p = new U8[data.Size()];

	data.WriteToMemory(p);
	is.ReadFromMemory(data.Size(), p);

	int linecount;

	is >> linecount;
	int i = 0;
	for ( ; i < linecount; i++)
	{
		CParagraph *pPara = new CParagraph(pEditTool);
		pPara->Read(is);
		pEditTool->m_ParagraphList.push_back(pPara);

		TextBlocks::iterator k = pPara->m_TextBlockList.begin();
		while(k != pPara->m_TextBlockList.end())
		{
			(*k)->CalcHeight();
			(*k)->CalcWidth();
			k++;
		}

		pPara->CalcHeight();
		pPara->CalcDrawY();
	}

	delete p;

	int nProcLine = 0, nUpdate = 0;
	for (i = 0; i < (int)pEditTool->m_ParagraphList.size(); i++)
	{
		pEditTool->m_ParagraphList[i]->Arrange(&nProcLine, nUpdate);
	}

	pEditTool->MoveCursor();
	pEditTool->SelToCursor();
	pEditTool->ReDraw();

	pEditTool->TextProperty.TextType = oldtype;
	pEditTool->OnSelChanged();
}

// 边框Command
COMMAND_CLSNAME(SetBorder)::COMMAND_CLSNAME(SetBorder)(CTextEditBase *base, BOOL border) : CTECommand(base)
{
	TEXT_PROPERTY prop;

	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	pEditTool->GetTextProperty(prop);

	newborder = border;
	oldborder = prop.bBorder;
}

void COMMAND_CLSNAME(SetBorder)::Execute()
{
	pEditTool->TextProperty.bBorder = newborder;
	pEditTool->OnSelChanged();
}

void COMMAND_CLSNAME(SetBorder)::UnExecute()
{
	pEditTool->TextProperty.bBorder = oldborder;
	pEditTool->OnSelChanged();
}

// 可选择Command
COMMAND_CLSNAME(CanSelect)::COMMAND_CLSNAME(CanSelect)(CTextEditBase *base, BOOL cansel) : CTECommand(base)
{
	TEXT_PROPERTY prop;

	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	pEditTool->GetTextProperty(prop);

	newcansel = cansel;
	oldcansel = prop.bCanSelect;
}

void COMMAND_CLSNAME(CanSelect)::Execute()
{
	pEditTool->TextProperty.bCanSelect = newcansel;
	pEditTool->OnSelChanged();
}

void COMMAND_CLSNAME(CanSelect)::UnExecute()
{
	pEditTool->TextProperty.bCanSelect= oldcansel;
	pEditTool->OnSelChanged();
}

// 绑定变量Command
COMMAND_CLSNAME(SetBindVar)::COMMAND_CLSNAME(SetBindVar)(CTextEditBase *base, const char *newBind) : CTECommand(base)
{
	TEXT_PROPERTY prop;

	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	pEditTool->GetTextProperty(prop);

	strNewBind = newBind;
	strOldBind = prop.strBindVar;
}

void COMMAND_CLSNAME(SetBindVar)::Execute()
{
	pEditTool->TextProperty.strBindVar = strNewBind;
	pEditTool->OnSelChanged();
}

void COMMAND_CLSNAME(SetBindVar)::UnExecute()
{
	pEditTool->TextProperty.strBindVar = strOldBind;
	pEditTool->OnSelChanged();
}

// 可编辑Command
COMMAND_CLSNAME(CanEdit)::COMMAND_CLSNAME(CanEdit)(CTextEditBase *base, BOOL edit) : CTECommand(base)
{
	TEXT_PROPERTY prop;

	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	pEditTool->GetTextProperty(prop);

	newEdit = edit;
	oldEdit = prop.bCanEdit;
	oldSel  = prop.bCanSelect;
}

void COMMAND_CLSNAME(CanEdit)::Execute()
{
	pEditTool->TextProperty.bCanEdit = newEdit;
	pEditTool->TextProperty.bCanSelect = true;
	pEditTool->OnSelChanged();
}

void COMMAND_CLSNAME(CanEdit)::UnExecute()
{
	pEditTool->TextProperty.bCanEdit = oldEdit;
	pEditTool->TextProperty.bCanSelect = oldSel;
	pEditTool->OnSelChanged();
}

// 设置文本最大长度Command
COMMAND_CLSNAME(SetMaxLength)::COMMAND_CLSNAME(SetMaxLength)(CTextEditBase *base, unsigned short nLength) : CTECommand(base)
{
	TEXT_PROPERTY prop;

	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	pEditTool->GetTextProperty(prop);

	nOldLength = prop.nMaxLength;
	nNewLength = nLength;
}

void COMMAND_CLSNAME(SetMaxLength)::Execute()
{
	pEditTool->TextProperty.nMaxLength = nNewLength;
	pEditTool->OnSelChanged();
}

void COMMAND_CLSNAME(SetMaxLength)::UnExecute()
{
	pEditTool->TextProperty.nMaxLength = nOldLength;
	pEditTool->OnSelChanged();
}

// 设置文本多行类型Command
COMMAND_CLSNAME(SetMultilineType)::COMMAND_CLSNAME(SetMultilineType)(CTextEditBase *base, emMultilineType mt) : CTECommand(base)
{
	TEXT_PROPERTY prop;

	pEditTool = (TTextToolExTmpl<ITextTool> *)base;
	pEditTool->GetTextProperty(prop);

	OldType = prop.MultilineType;
	NewType = mt;
}

void COMMAND_CLSNAME(SetMultilineType)::Execute()
{
	pEditTool->TextProperty.MultilineType = NewType;
	pEditTool->OnSelChanged();
}

void COMMAND_CLSNAME(SetMultilineType)::UnExecute()
{
	pEditTool->TextProperty.MultilineType = OldType;
	pEditTool->OnSelChanged();
}

//////////////////////////////////////////////////////////
inline BOOL IsSameColor(COLORREF cr1, COLORREF cr2)
{
	int r1 = GetRValue(cr1), g1 = GetGValue(cr1), b1 = GetBValue(cr1);
	int r2 = GetRValue(cr2), g2 = GetGValue(cr2), b2 = GetBValue(cr2);

	return (abs(r1 - r2) < 3 &&
		abs(g1 - g2) < 3 &&
		abs(b1 - b2) < 3);
}

BOOL MakeDiffColor(COLORREF fr, COLORREF bg)
{
	/*
	float color_scale = 0.95;

	if (IsSameColor(fr, bg))
	{
		fr = RGB(GetRValue(fr) * color_scale,
			GetGValue(fr) * color_scale,
			GetBValue(fr) * color_scale);
	}

	if (IsSameColor(fr, bg))
	{
		fr = RGB(GetRValue(fr) / color_scale,
			GetGValue(fr) / color_scale,
			GetBValue(fr) / color_scale);
	}
	*/

	if (IsSameColor(fr, bg))
	{
		fr = RGB(149, 149, 149);
	}

	return fr;
}

#undef SHIFT_DOWN
#undef CTRL_DOWN