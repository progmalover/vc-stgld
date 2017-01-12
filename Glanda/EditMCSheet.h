#pragma once


#include "EditGeneralPage.h"
#include "EditMCTextPage.h"
#include "EditMCShapePage.h"
#include "EditMCSoundPage.h"
#include "EditMatrixPage.h"
#include "EditCxformPage.h"
#include <set>

class gldInstance;
class gldSoundInstance;
class gldCharacterKey;
class gldObj;
class gldMovieClip;
class gldLayer;

// CEditMCSheet
struct _MCFrame
{
	gldLayer *layer;
	int ntime;
	_MCFrame(gldLayer *layer_, int time_)
		: layer(layer_)
		, ntime(time_)
	{
	}
	bool operator<(const _MCFrame &a) const
	{
		if ((DWORD)layer < (DWORD)a.layer)
			return true;
		else if ((DWORD)layer > (DWORD)a.layer)
			return false;
		else
			return ntime < a.ntime;		
	}
};

template <class T>
struct _MCData
{
public:
	_MCData<T>(BOOL bButton)
	{
		m_nUnnamedIndex = 0;
		m_bButton = bButton;
	}
	static void ResetUnnamed()
	{
		m_nNextUnnamedIndex = 1;
	}
	CString GetListText();

	gldObj* obj;
	std::list<T> keys;
	std::set<int> frames;
	
private:
	static int m_nNextUnnamedIndex;
	int m_nUnnamedIndex;
	BOOL m_bButton;
};

template <>
struct _MCData<gldSoundInstance *>
{
public:
	_MCData<gldSoundInstance *>(BOOL bButton)
	{
		m_nUnnamedIndex = 0;
		m_bButton = bButton;
	}
	static void ResetUnnamed()
	{
		m_nNextUnnamedIndex = 1;
	}
	CString GetListText()
	{
		CString strListText;

		CString s = obj->m_name.c_str();
		s = s.TrimLeft();
		s = s.TrimRight();
		if(s.IsEmpty())
		{
			if(!m_nUnnamedIndex)
			{
				m_nUnnamedIndex = m_nNextUnnamedIndex++;
			}
			if(obj->IsGObjInstanceOf(gobjShape))
			{
				CString str;
				str.LoadString(IDS_BUTTON_UNNAMED_PREFIX_SHAPE);
				s.Format("%s %d", (LPCTSTR)str, m_nUnnamedIndex);
			}
			else if(obj->IsGObjInstanceOf(gobjText))
			{
				CString str;
				str.LoadString(IDS_BUTTON_UNNAMED_PREFIX_TEXT);
				s.Format("%s %d", (LPCTSTR)str, m_nUnnamedIndex);
			}
			else if(obj->IsGObjInstanceOf(gobjSound))
			{
				CString str;
				str.LoadString(IDS_BUTTON_UNNAMED_PREFIX_SOUND);
				s.Format("%s %d", (LPCTSTR)str, m_nUnnamedIndex);
			}
		}
		strListText = s;

		CString strButtonStatus;
		strButtonStatus.LoadString(IDS_BUTTON_STATUS_FULL);
		CString strMovieClipStatus;
		strMovieClipStatus.LoadString(IDS_MOVIE_CLIP_STATUS_TEXT);

		const int buttonFrames = 4;
		CString str[buttonFrames];
		for(int i = 0; i < buttonFrames; ++i)
		{
			AfxExtractSubString(str[i], strButtonStatus, i, '|');
		}

		BOOL bAdded = FALSE;
		if(m_bButton)
		{
			bool on[buttonFrames];
			memset(on, 0, sizeof(on));
			for(std::set<_MCFrame>::iterator iter = frames.begin(); iter != frames.end(); ++iter)
			{
				if ((*iter).ntime > -1 && (*iter).ntime < buttonFrames)
					on[(*iter).ntime] = true;			
			}

			for(int j = 0; j < buttonFrames; ++j)
			{
				if(on[j])
				{
					if(bAdded)
					{
						strListText += ", " + str[j];
					}
					else
					{
						bAdded = TRUE;
						strListText +=  " (" + str[j];
					}
				}
			}

			if(bAdded)
			{
				strListText += ")";
			}
		}
		else
		{
			for(std::set<_MCFrame>::iterator iter = frames.begin(); iter != frames.end(); ++iter)
			{
				CString s;
				s.Format("%d", (*iter).ntime + 1);
				if(bAdded)
				{
					strListText += ", " + s;
				}
				else
				{
					bAdded = TRUE;
					strListText += " (" + strMovieClipStatus + " " + s;
				}
			}

			if(bAdded)
			{
				strListText += ")";
			}
		}
		return strListText;
	}

	gldObj* obj;
	std::list<gldSoundInstance *> keys;
	std::set<_MCFrame> frames;
	
private:
	static int m_nNextUnnamedIndex;
	int m_nUnnamedIndex;
	BOOL m_bButton;
};

template <class T>
int _MCData<T>::m_nNextUnnamedIndex = 1;

template <class T>
CString _MCData<T>::GetListText()
{
	CString strListText;

	CString s = obj->m_name.c_str();
	s = s.TrimLeft();
	s = s.TrimRight();
	if(s.IsEmpty())
	{
		if(!m_nUnnamedIndex)
		{
			m_nUnnamedIndex = m_nNextUnnamedIndex++;
		}
		if(obj->IsGObjInstanceOf(gobjShape))
		{
			CString str;
			str.LoadString(IDS_BUTTON_UNNAMED_PREFIX_SHAPE);
			s.Format("%s %d", (LPCTSTR)str, m_nUnnamedIndex);
		}
		else if(obj->IsGObjInstanceOf(gobjText))
		{
			CString str;
			str.LoadString(IDS_BUTTON_UNNAMED_PREFIX_TEXT);
			s.Format("%s %d", (LPCTSTR)str, m_nUnnamedIndex);
		}
		else if(obj->IsGObjInstanceOf(gobjSound))
		{
			CString str;
			str.LoadString(IDS_BUTTON_UNNAMED_PREFIX_SOUND);
			s.Format("%s %d", (LPCTSTR)str, m_nUnnamedIndex);
		}
	}
	strListText = s;

	CString strButtonStatus;
	strButtonStatus.LoadString(IDS_BUTTON_STATUS_FULL);
	CString strMovieClipStatus;
	strMovieClipStatus.LoadString(IDS_MOVIE_CLIP_STATUS_TEXT);

	const int buttonFrames = 4;
	CString str[buttonFrames];
	for(int i = 0; i < buttonFrames; ++i)
	{
		AfxExtractSubString(str[i], strButtonStatus, i, '|');
	}

	BOOL bAdded = FALSE;
	if(m_bButton)
	{
		for(int j = 0; j < buttonFrames; ++j)
		{
			if(frames.find(j)!=frames.end())
			{
				if(bAdded)
				{
					strListText += ", " + str[j];
				}
				else
				{
					bAdded = TRUE;
					strListText +=  " (" + str[j];
				}
			}
		}

		if(bAdded)
		{
			strListText += ")";
		}
	}
	else
	{
		for(std::set<int>::iterator iter = frames.begin(); iter != frames.end(); ++iter)
		{
			CString s;
			s.Format("%d", (*iter) + 1);
			if(bAdded)
			{
				strListText += ", " + s;
			}
			else
			{
				bAdded = TRUE;
				strListText += " (" + strMovieClipStatus + " " + s;
			}
		}

		if(bAdded)
		{
			strListText += ")";
		}
	}
	return strListText;
}

typedef _MCData<gldCharacterKey*> _TextData;
typedef _MCData<gldCharacterKey*> _ShapeData;
typedef _MCData<gldSoundInstance*> _SoundData;

template <class T>
_MCData<T>* AfxFindObjectExist(gldObj* pObj, std::list<_MCData<T>*>& items)
{
	for(std::list<_MCData<T>*>::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		_MCData<T>* pData = *iter;
		if(pData->obj == pObj)
		{
			return pData;
		}
	}
	return NULL;
}

class CEditMCSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CEditMCSheet)

public:
	CEditMCSheet(gldInstance* pInstance);
	virtual ~CEditMCSheet();

	CEditGeneralPage m_editGeneralPage;
	CEditMCTextPage m_editTextPage;
	CEditMCShapePage m_editShapePage;
	CEditMCSoundPage m_editSoundPage;
	CEditMatrixPage m_editMatrixPage;
	CEditCxformPage m_editCxformPage;

	gldInstance* m_pInstance;

	std::list<_TextData*> m_texts;
	std::list<_ShapeData*> m_shapes;
	std::list<_SoundData*> m_sounds;

	TCommandGroup* m_pCmd;
	BOOL CanEdit();
protected:
	void ReadItemData();
	void ReadItemData(gldMovieClip* pMovieClip);
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
};


