#pragma once
#include "gldFillStyle.h"
#include "gldShape.h"
#include "TextToolEx.h"
#include "ITextTool.h"
#include "toolsdef.h"
#include "C2I.h"
#include "EffectCommonParameters.h"
#include <deque>

class CTextStyleData
{
public:
	CTextStyleData();
	virtual ~CTextStyleData();
	CTextStyleData* Clone();
	gldShape* GeneratePreviewText();
	void WriteToBinStream(oBinStream& os);
	void ReadFromBinStream(iBinStream& is);
	void SetFillStyle(gldFillStyle* pFillStyle);
	gldFillStyle* GetFillStyle();

	BOOL		m_bBreakApart;

	BOOL		m_bTextBorder;
	float		m_fBorderWidth;
	COLORREF	m_clrBorderColor;

	BOOL		m_bAddShadow;
	float		m_fShadowSize;
	COLORREF	m_clrShadowColor;

	BOOL		m_bCustomFill;
	BOOL		m_bFillIndividually;
	int			m_nAngle;
protected:
	gldFillStyle*	m_pFillStyle;
};

class CTextStyleManager
{
	static CString m_strPathName;
public:
	static HRESULT LoadStyle(CTextStyleData& textStyle, int nID);
	static HRESULT SaveStyle(CTextStyleData& textStyle, int nID);
	static CString GetPathName();
};