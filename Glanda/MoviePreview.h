#pragma once

#include <map>
#include "gldCxform.h"
#include "gldMatrix.h"

// pre-declare
class gldText2;
class gldObj;
class gldMovieClip;
class gldSprite;
class CTextFormat;
class gldInstance;
interface IGLD_Parameters;
interface IGLD_Effect;

gldText2 *CreateSingleLineText2(const CString &text, CTextFormat &txtFmt);
CSize GetTextExtent(const CString &text, CTextFormat &fmt);

class CMoviePreview
{
public:
	CMoviePreview();
	virtual ~CMoviePreview();

	void SetMovieClip(const CString &text, CTextFormat &fmt, IGLD_Parameters *opts);
	void SetMovieClip(gldInstance *pInst, IGLD_Parameters *opts);
	void SetMovieClip(gldMovieClip *pMovie, const gldMatrix &mx, const gldCxform &cx, BOOL bAutoDeleteMovie = TRUE, BOOL bSharedObject = TRUE);
	BOOL ApplyEffect(IGLD_Effect *pIEffect, int nStart, int nLength, IGLD_Parameters *pIParas, IGLD_Parameters *pIExtra);
	BOOL ApplyEffect(IGLD_Effect *pIEffect, IGLD_Parameters *pIParas, IGLD_Parameters *pIExtra);
	BOOL CreatePreview(const CString &sPath, COLORREF crBackground, BOOL bShowCanvas = TRUE);

protected:
	void Clear();
	void DeleteEffect();	

protected:
	enum
	{
		SHARED_OBJECT,
		OWNER_OBJECT,
		ACCESSIONAL_OBJECT,
	};

	enum {MARGIN = 20 * 5};

	typedef std::map<gldObj *, int> OBJECT2TYPE;

	gldMovieClip *m_pMovie;
	gldMovieClip *m_pEffect;
	OBJECT2TYPE m_ObjMap;
	BOOL m_bAutoDeleteMovie;
	
	gldMatrix m_Matrix;
	gldCxform m_Cxform;	
};