#pragma once
#include <Gdiplus.h>
#include <atlimage.h>
#include "glanda_i.h"
#include "gldSound.h"
#include "Dib.h"

class gldObj;
using namespace Gdiplus;

enum
{
	RESOURCE_VOID		= 0,
	RESOURCE_IMAGES		= 1 << 0,
	RESOURCE_SHAPES		= 1 << 1,
	RESOURCE_BUTTONS	= 1 << 2,
	RESOURCE_SOUNDS		= 1 << 3,
	RESOURCE_MOVIECLIP	= 1 << 4,	
	RESOURCE_ALL		= ((1 << 5) - 1),
	RESOURCE_GREETINGCARD = RESOURCE_ALL | (1 << 6),
	RESOURCE_PRELOAD = RESOURCE_ALL | (1 << 7),
	RESOURCE_BACKGROUND = RESOURCE_ALL | (1 << 8),
	RESOURCE_GRAPHIC	= RESOURCE_ALL & ~RESOURCE_SOUNDS
};

static const LPCTSTR szResourceFilterImages		= ".bmp|.jpg|.jpe|.jpeg|.png";
static const LPCTSTR szResourceFilterShapes		= ".gls|.wmf|.emf";
static const LPCTSTR szResourceFilterButtons	= ".glb";
static const LPCTSTR szResourceFilterSounds		= ".wav|.mp3";
static const LPCTSTR szResourceFilterMovieClips	= ".glm";

class CResourceThumbItem
{
public:
	CResourceThumbItem(LPCTSTR lpszPathName);
	CResourceThumbItem(gldObj* pObj);
	CResourceThumbItem(CResourceThumbItem* pThumbItem);
	virtual ~CResourceThumbItem(void);

	void		RenderImage(CDC& dc, CRect& rc, CImage& image);
	void		RenderMetafile(CDC& dc, CRect& rc, Metafile& image);
	static void		RenderSound(CDC& dc, CRect& rc, BOOL bMP3);
	static void		RenderObj(CDC& dc, CRect& rc, gldObj* pObj);
	static void		RenderObj(CDC& dc, CRect& rc, gldObj* pObj, COLORREF clrBackground);

	static BOOL		ReadImage(CImage& image, LPCTSTR lpszPathName);

	static int		GetResourceType(LPCTSTR lpszPathName);
	static int		GetResourceType(gldObj* pObj);
	int				GetResourceType();

	void			RenderEx(CDC& dc, CRect& rc);
	void			Render(CDC* pDC, CRect& rcThumb);
	void			ReleaseBuffer();
	void			RenderObject(CDC& dc, int x, int y, int cx, int cy, IGLD_Object *pIObject, gldMatrix &matrix, gldCxform &cxform);

	U32				GetSampleCount();
	U8				GetSampleChannel();
	U8				GetSampleRate();
	ULONGLONG		GetDataLen();
	const CSize&	GetDimensions();
	double			GetDuration();
	BOOL			IsValidResource();
	CString			GetDesc();
	void			SetDesc(LPCTSTR lpszDesc);

	BOOL			m_bStockResource;
	CString			m_strPathName;
	gldObj*			m_pObj;

	void	PrepareSoundInformation();
protected:
	BOOL		m_bValid;
	CSize		m_szDimensions;
	ULONGLONG	m_nDataLen;
	double		m_dDuration;
	U32			m_nSampleCount;
	U8			m_nSampleSize;
	U8			m_nSampleChannel;
	U8			m_nSampleRate;
	CString		m_strDesc;

	BOOL		m_bInformationInit;

	CAutoPtr<CDib>	m_pBuffer;

	static UINT		m_nCount;
	static HICON	m_hIconMP3;
	static HICON	m_hIconWAV;
	static void		Increment();
	static void		Decrement();
private:
	UINT		m_nResourceType;
};