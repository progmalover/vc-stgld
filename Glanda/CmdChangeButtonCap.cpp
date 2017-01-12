#include "StdAfx.h"
#include ".\cmdchangebuttoncap.h"
#include "gldButton.h"
#include "TextToolEx.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldSprite.h"
#include <list>
#include <algorithm>
#include "DlgSelectTextChanging.h"
#include "MoviePreview.h"
#include <map>
#include "CmdAddShape.h"
#include "my_app.h"

CCmdChangeButtonCap::CCmdChangeButtonCap(gldButton *button, const CString &strOldText, const CString &strNewText, int method)
: CCmdGroupUpdateObjUniId(button)
, m_pButton(button)
, m_strNewText(strNewText)
, m_strOldText(strOldText)
, m_Method(method)
{
	_M_Desc_ID = IDS_CMD_CHANGEBUTTONTEXT;
}

CCmdChangeButtonCap::~CCmdChangeButtonCap(void)
{
}

CString CCmdChangeButtonCap::GetTextString(gldText2 *text)
{
	CString str;
	Paragraphs::iterator ip = text->m_ParagraphList.begin();
	for (; ip != text->m_ParagraphList.end(); ++ip)
	{		
		CParagraph *para = *ip;
		TextBlocks::iterator ib = para->m_TextBlockList.begin();
		for (; ib != para->m_TextBlockList.end(); ++ib)
		{
			CTextBlock *block = *ib;
			USES_CONVERSION;
			str += CString(W2T(block->strText.c_str()));
		}
		str += _T(" ");
	}
	str = str.Trim();
	if (str.GetLength() == 0)
		str.Format(IDS_EMPTY_TEXT);
	return str;
}

CString CCmdChangeButtonCap::GetButtonCaption(gldButton *button)
{
	std::list<CString> texts;		
	CTextKeyIterator ixt(button->m_bmc);
	while (ixt.Next())
	{
		CString t = GetTextString(ixt.GetText());
		t.Trim();
		if (t.GetLength() > 0)
		{
			if (std::find(texts.begin(), texts.end(), t) == texts.end())
				texts.push_back(t);
		}		
	}

	if (texts.size() == 0) // 按钮没有包含文字
	{
		AfxMessageBox(IDS_BUTTON_NOT_FOUND_TEXT);
		return "";
	}

	CString cap = *texts.begin();
	if (texts.size() > 1) // 按钮包含多于一个文字
	{
		CDlgSelectTextChanging dlg(&texts);
		if (dlg.DoModal() == IDCANCEL)
			return "";
		else
			cap = dlg.GetSelText();
	}

	return cap;
}

/* if the replace text is complex text, this function will prompt user for replace to single line */
bool CCmdChangeButtonCap::PromptChangeComplexText(gldButton *button, const CString &strCap)
{
	CTextKeyIterator ixt(button->m_bmc);
	while (ixt.Next())
	{	
		gldText2 *text = ixt.GetText();
		CString str = CCmdChangeButtonCap::GetTextString(text);
		if (str == strCap)
		{
			if (text->m_ParagraphList.size() != 1 || (*text->m_ParagraphList.begin())->m_TextBlockList.size() != 1)
			{
				return AfxMessageBox(IDS_PROMPT_CHANGE_BUTTON_TEXT, MB_YESNO | MB_ICONQUESTION) == IDYES;
			}
		}
	}
	return true;
}

bool CCmdChangeButtonCap::Execute()
{	
	CGuardSelKeeper xSel;

	if (m_Backup.size() == 0)
	{
		typedef std::map<gldText2 *, gldText2 *> OT2N;
		OT2N m;
		CTextKeyIterator ixt(m_pButton->m_bmc);
		while (ixt.Next())
		{
			gldText2 *text = ixt.GetText();
			CString str = CCmdChangeButtonCap::GetTextString(text);
			if (str == m_strOldText)
			{
				gldText2 *ntxt = NULL;
				OT2N::iterator i = m.find(text);
				if (i == m.end())
				{
					ntxt = CreateSingleLineText2(m_strNewText
						, (*(*text->m_ParagraphList.begin())->m_TextBlockList.begin())->TextFormat);					
					m.insert(OT2N::value_type(text, ntxt));
					Do(new CCmdAddObj(ntxt));
				}
				else
					ntxt = (*i).second;
				int cx = (text->m_bounds.left + text->m_bounds.right) / 2;
				int cy = (text->m_bounds.top + text->m_bounds.bottom) / 2;
				int tx = (*ixt)->GetCharacter(0)->m_matrix.GetX(cx, cy);
				int ty = (*ixt)->GetCharacter(0)->m_matrix.GetY(cx, cy);
				int bx = (ntxt->m_bounds.left + ntxt->m_bounds.right) / 2;
				int by = (ntxt->m_bounds.top + ntxt->m_bounds.bottom) / 2;
				gldMatrix mx = (*ixt)->GetCharacter(0)->m_matrix;
				if (m_Method == KEEP_BOUND)
				{
					gldMatrix smx;
					smx.m_e11 = (float)text->m_bounds.Width() / ntxt->m_bounds.Width();
					smx.m_e22 = (float)text->m_bounds.Height() / ntxt->m_bounds.Height();
					mx = smx * mx;
				}
				mx.m_x = (S32)(tx - bx * mx.m_e11 - by * mx.m_e21);
				mx.m_y = (S32)(ty - bx * mx.m_e12 - by * mx.m_e22);
				KeyBackup bu;
				bu.key = *ixt;
				bu.m = mx;
				bu.text = ntxt;
				m_Backup.push_back(bu);
			}
		}		
	}

	if (CCmdGroupUpdateObjUniId::Execute())
	{
		CGuardDrawOnce xDraw;	
		Assign();
		CTransAdaptor::RebuildCurrentScene(true);
		my_app.Redraw();
		my_app.Repaint();	

		return true;
	}
	return false;
}

bool CCmdChangeButtonCap::Unexecute()
{
	CGuardSelKeeper xSel;

	if (CCmdGroupUpdateObjUniId::Execute())
	{
		CGuardDrawOnce xDraw;	
		Assign();
		CTransAdaptor::RebuildCurrentScene(true);
		my_app.Redraw();
		my_app.Repaint();	

		return true;
	}
	return false;
}

void CCmdChangeButtonCap::Assign()
{
	for (std::vector<KeyBackup>::iterator itk = m_Backup.begin()
		; itk != m_Backup.end(); ++itk)
	{
		KeyBackup &k = *itk;
		swap(k.m, k.key->GetCharacter(0)->m_matrix);
		gldObj *obj = k.key->GetObj();
		k.key->SetObj(k.text);
		k.text = (gldText2 *)obj;
	}
}




/* CKeyIterator */
CKeyIterator::CKeyIterator(gldMovieClip *mc)
: m_mc(mc)
, m_bInit(false)
{
}

bool CKeyIterator::Next()
{
	if (!m_bInit)
	{
		if (!Init())
			return false;
		m_bInit = true;
		return true;
	}
	else
	{		
		gldFrameClip *pfc = *ifc;
		if (++ikey != pfc->m_characterKeyList.end())
			return true;
		else
		{
			gldLayer *la = *ila;
			while (++ifc != la->m_frameClipList.end())
			{
				gldFrameClip *pfc = *ifc;
				if (pfc->m_characterKeyList.size() > 0)
				{
					ikey = pfc->m_characterKeyList.begin();
					return true;
				}				
			}
			while (++ila != m_mc->m_layerList.end())
			{
				gldLayer *pla = *ila;
				if (pla->m_frameClipList.size() > 0)						
				{
					for (ifc = pla->m_frameClipList.begin(); ifc != pla->m_frameClipList.end(); ++ifc)
					{
						gldFrameClip *pfc = *ifc;
						if (pfc->m_characterKeyList.size() > 0)
						{
							ikey = pfc->m_characterKeyList.begin();
							return true;
						}
					}
				}				
			}
			return false;
		}
	}
}

bool CKeyIterator::Init()
{
	for (ila = m_mc->m_layerList.begin(); ila != m_mc->m_layerList.end(); ++ila)
	{
		gldLayer *pla = *ila;
		for (ifc = pla->m_frameClipList.begin(); ifc != pla->m_frameClipList.end(); ++ifc)
		{
			gldFrameClip *pfc = *ifc;
			if (pfc->m_characterKeyList.size() > 0)
			{
				ikey = pfc->m_characterKeyList.begin();
				return true;
			}			
		}
	}
	return false;
}



/* CTextIterator */

CTextKeyIterator::CTextKeyIterator(gldMovieClip *mc)
{
	m_Stack.push(CKeyIterator(mc));
}

bool CTextKeyIterator::Next()
{
	while (!m_Stack.empty())
	{
		CKeyIterator &ki = m_Stack.top();
		if (ki.Next())
		{
			gldCharacterKey *key = *ki;
			gldObj *obj = key->GetObj();
			if (obj->IsGObjInstanceOf(gobjText) && ((gldText2 *)obj)->m_textType == gtextStatic)
			{				
				m_pKey = key;
				return true;
			}
			else if (obj->IsGObjInstanceOf(gobjButton))
				m_Stack.push(CKeyIterator(((gldButton *)obj)->m_bmc));
			else if (obj->IsGObjInstanceOf(gobjSprite))
				m_Stack.push(CKeyIterator(((gldSprite *)obj)->m_mc));
		}
		else
			m_Stack.pop();
	}

	return false;
}