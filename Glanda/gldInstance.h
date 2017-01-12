#pragma once

#include <string>
#include <list>

#include "gldMatrix.h"
#include "gldObj.h"
#include "gldCxform.h"
#include "Command.h"

#include "Glanda_i.h"

class gldEffect;
class gldMovieClip;
class gldScene2;

enum COLOR_TRANS_STYLE
{
	INST_COLOR_TRANS_STYLE_NONE = 0,	// must be 0 based, do not modify
	INST_COLOR_TRANS_STYLE_ALPHA, 
	INST_COLOR_TRANS_STYLE_COLOR, 
	INST_COLOR_TRANS_STYLE_ADVANCED
};

typedef std::list<gldEffect *> GEFFECT_LIST;
typedef GEFFECT_LIST::iterator GEFFECT_LIST_IT;

class gldInstance
{
public:
	gldInstance();
	gldInstance(gldObj *pObj, const gldMatrix &matrix, const gldCxform &cxform);
	~gldInstance(void);

	static const char s_TemplateFlag[];

public:
	std::string m_name;

	gldObj *m_obj;
	gldMatrix m_matrix;
	gldCxform m_cxform;
    int m_tx;
    int m_ty;
	void *m_ptr;
	gldMovieClip *m_mc;

	GEFFECT_LIST m_effectList;
	std::string m_action;

	// Color transform style
	COLOR_TRANS_STYLE m_cxStyle;
	COLORREF m_tint;

	// common effect parameters
	CComPtr<IGLD_Parameters> m_pIParas;

	// if it is a mask instance
	BOOL m_mask;

	// if the effect group of this instance is expaned.
	BOOL m_expanded;

public:
	enum EnumErrorHandleMethod
	{
		IS_PROMPT,
		IS_IGNORE,
		IS_RETURN,
	};

public:
	BOOL ConfigEffect(gldInstance *pInstance, gldEffect *pEffect);	
	BOOL BuildEffects(EnumErrorHandleMethod method = IS_PROMPT);
	int GetMaxTime() const;
	int GetMinTime() const;
	gldEffect *GetEffect(int index);
	gldInstance *Clone() const;
	void Copy(const gldInstance *pSrc);
	int GetLength() const;
	int GetEffectKeyCount();

	HRESULT WriteToStream(IStream *pStm, void *pUnk);
	HRESULT ReadFromStream(IStream *pStm, void *pUnk);
	
	HRESULT SaveToFile(LPCTSTR pszPath);
	HRESULT LoadFromFile(LPCTSTR pszPath, GOBJECT_LIST &lstObj);

	BOOL SaveAsTemplate() const;
	HRESULT SaveAsTemplate(LPCTSTR pszPath, LPCTSTR name);
	HRESULT ApplyTemplate(LPCTSTR pszPath);

	static HRESULT GetTemplateName(LPCTSTR pszTemplPath, CString &strTemplName);
	static HRESULT ExtractDemo(LPCTSTR pszTemplPath, LPCTSTR pszDemoPath);
};

typedef std::list <gldInstance *> GINSTANCE_LIST;
