#include "StdAfx.h"
#include "resourcethumbitem.h"
#include "Graphics.h"
#include "SWFProxy.h"
#include "gld_draw.h"
#include "Resource.h"
#include "SWFStream.h"
#include <Shlwapi.h>
#include "gldCxform.h"
#include "TransAdaptor.h"
#include "gldSprite.h"
#include "gldMovieClip.h"
#include "I2C.h"

HICON	CResourceThumbItem::m_hIconMP3	= NULL;
HICON	CResourceThumbItem::m_hIconWAV	= NULL;
UINT	CResourceThumbItem::m_nCount	= 0;

#define CHECK_RETURN(hr) if (FAILED(hr)) return

void CResourceThumbItem::RenderObject(CDC& dc, int x, int y, int cx, int cy, IGLD_Object *pIObject, gldMatrix &matrix, gldCxform &cxform)
{
	ASSERT(pIObject != NULL);
	ASSERT(cx > 0 && cy > 0);

	HRESULT hr;
	
	GLD_ObjectType type;
	hr = pIObject->get_Type(&type);
	CHECK_RETURN(hr);
	if (type != gceShape && type != gceButton 
		&& type != gceSprite)
	{
		return;
	}
	CI2CMap I2CMap;
	CI2CMap *pTmpMap = CI2C::m_pI2CMap;	
	gldObj *pObj = NULL;
	if (type == gceShape)
	{
		CI2C::m_pI2CMap = &I2CMap;
		hr = CI2C::Create(pIObject, &pObj);
	}
	else 
	{
		CComPtr<IGLD_MovieClip> pIMovie;
		if (type == gceButton)
		{
			CComQIPtr<IGLD_Button> pIButton = pIObject;
			if (pIButton == NULL)
				hr = E_FAIL;
			CHECK_RETURN(hr);
			hr = pIButton->get_MovieClip(&pIMovie);
		}
		else
		{
			ASSERT(type == gceSprite);
			CComQIPtr<IGLD_Sprite> pISprite = pIObject;
			if (pISprite == NULL)
				hr = E_FAIL;
			CHECK_RETURN(hr);
			hr = pISprite->get_MovieClip(&pIMovie);
		}
		CHECK_RETURN(hr);
		ASSERT(pIMovie != NULL);
		CAutoPtr<gldSprite> pSprite(new gldSprite);
		ASSERT(pSprite->m_mc != NULL);
		CI2C::m_pI2CMap = &I2CMap;
		hr = CI2C::GetShowList(pIMovie, 0, pSprite->m_mc);
		pObj = pSprite.Detach();
		I2CMap.Insert(pIObject, pObj);
	}

	CI2C::m_pI2CMap = pTmpMap;
	if (FAILED(hr))
	{
		I2CMap.FreeAllSymbol();
		return;
	}

	ASSERT(pObj != NULL);
	CSWFProxy::PrepareTShapeForGObj(pObj);

	gld_shape shape = CTraitShape(pObj);
	gld_rect bnd = shape.bound();
	int bx = (bnd.left + bnd.right) / 2;
	int by = (bnd.top + bnd.bottom) / 2;
	int width = bnd.width() / 20;
	int height = bnd.height() / 20;
	TMatrix tmx;
	CTransAdaptor::GM2TM(matrix, tmx);
	bnd = _transform_rect(bnd, tmx);
	if (bnd.width() > 0 && bnd.height() > 0)
	{
		int nx = 20 * x + cx * 20 / 2;
		int ny = 20 * y + cy * 20 / 2;		
		float sx = (float)cx *20 / bnd.width();
		float sy = (float)cy *20 / bnd.height();
		float s = __min(sx, sy);		
		TMatrix stmx(s, 0, 0, s, 0, 0);
		tmx = tmx * stmx;
		tmx.m_dx = nx - bx * tmx.m_e11 - by * tmx.m_e21;
		tmx.m_dy = ny - bx * tmx.m_e12 - by * tmx.m_e22;
		TCxform tcx;
		CTransAdaptor::GCX2TCX(cxform, tcx);
		TGraphicsDevice device(cx, cy);
		device.SetMatrix(tmx);
		device.SetCxform(tcx);
		TDraw::draw(shape, device);
		device.Display(dc.m_hDC, x, y);
	}

	I2CMap.FreeAllSymbol();

	m_szDimensions.cx = width;
	m_szDimensions.cy = height;
	m_bValid = TRUE;	
}

void CResourceThumbItem::Increment()
{
	if(m_nCount++==0)
	{
		SHFILEINFO sfi;
		memset(&sfi, 0, sizeof(SHFILEINFO));
		SHGetFileInfo(".mp3", FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_TYPENAME | SHGFI_LARGEICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES );
		m_hIconMP3	= sfi.hIcon;
		memset(&sfi, 0, sizeof(SHFILEINFO));
		SHGetFileInfo(".wav", FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO),
			SHGFI_SYSICONINDEX | SHGFI_TYPENAME | SHGFI_LARGEICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES );
		m_hIconWAV	= sfi.hIcon;
	}
}

void CResourceThumbItem::Decrement()
{
	if(--m_nCount==0)
	{
	}
}

CResourceThumbItem::CResourceThumbItem(LPCTSTR lpszPathName)
: m_strPathName(lpszPathName)
, m_pObj(NULL)
, m_bStockResource(FALSE)
, m_nResourceType(RESOURCE_ALL)
, m_bInformationInit(FALSE)
{
	Increment();
}

CResourceThumbItem::CResourceThumbItem(gldObj* pObj)
: m_pObj(pObj)
, m_bStockResource(TRUE)
, m_bValid(FALSE)
, m_nResourceType(RESOURCE_ALL)
, m_bInformationInit(FALSE)
{
	Increment();
}

CResourceThumbItem::CResourceThumbItem(CResourceThumbItem* pThumbItem)
: m_strPathName(pThumbItem->m_strPathName)
, m_pObj(pThumbItem->m_pObj)
, m_bStockResource(pThumbItem->m_bStockResource)
, m_bValid(FALSE)
, m_nResourceType(RESOURCE_ALL)
, m_bInformationInit(FALSE)
{
	Increment();
}

CResourceThumbItem::~CResourceThumbItem(void)
{
	ReleaseBuffer();

	Decrement();
}

void CResourceThumbItem::RenderImage(CDC& dc, CRect& rc, CImage& image)
{
	m_szDimensions.cx = image.GetWidth();
	m_szDimensions.cy = image.GetHeight();
	m_bValid = TRUE;

	CRect rcDest;
	int nWidth = image.GetWidth();
	int nHeight = image.GetHeight();

	if(nWidth>rc.Width() || nHeight>rc.Height())
	{
		if(nWidth*rc.Height()>nHeight*rc.Width())
		{
			nHeight = max(1, rc.Width() * nHeight / nWidth);
			nWidth = rc.Width();
		}
		else
		{
			nWidth = max(1, rc.Height() * nWidth / nHeight);
			nHeight = rc.Height();
		}
	}

	rcDest.left		= rc.left + max((rc.Width() - nWidth) / 2, 0);
	rcDest.right	= rcDest.left + nWidth;
	rcDest.top		= rc.top + max((rc.Height() - nHeight) / 2, 0);
	rcDest.bottom	= rcDest.top + nHeight;
	dc.SetStretchBltMode(HALFTONE);
	image.Draw(dc.m_hDC, rcDest);
}

void CResourceThumbItem::RenderMetafile(CDC& dc, CRect& rc, Metafile& image)
{
	m_szDimensions.cx = image.GetWidth();
	m_szDimensions.cy = image.GetHeight();
	m_bValid = TRUE;

	CRect rcDest;
	int nWidth	= image.GetWidth();
	int nHeight	= image.GetHeight();					
	if(nWidth > rc.Width() || nHeight > rc.Height())
	{
		if(nWidth*rc.Height() > nHeight*rc.Width())
		{
			nHeight = max(1, rc.Width() * nHeight / nWidth);
			nWidth = rc.Width();
		}
		else
		{
			nWidth = max(1, rc.Height() * nWidth / nHeight);
			nHeight = rc.Height();
		}
	}

	rcDest.left = rc.left + max((rc.Width() - nWidth) / 2, 0);
	rcDest.right = rcDest.left + nWidth;
	rcDest.top = rc.top + max((rc.Height() - nHeight) / 2, 0);
	rcDest.bottom = rcDest.top + nHeight;

	Graphics graph(dc.m_hDC);
	graph.DrawImage(&image, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height());
}

void CResourceThumbItem::RenderSound(CDC& dc, CRect& rc, BOOL bMP3)
{
	HICON hIcon = bMP3 ? m_hIconMP3 : m_hIconWAV;
	if(hIcon)
	{
		DrawIcon(dc.m_hDC, rc.left + (rc.Width() - 32) / 2, rc.top + (rc.Height() - 32) / 2, hIcon);
	}
}

void CResourceThumbItem::RenderObj(CDC& dc, CRect& rc, gldObj* pObj)
{
	RenderObj(dc, rc, pObj, RGB(255, 255, 255));
}

void CResourceThumbItem::RenderObj(CDC& dc, CRect& rc, gldObj* pObj, COLORREF clrBackground)
{
	if(pObj->IsGObjInstanceOf(gobjShape) || pObj->IsGObjInstanceOf(gobjText))
	{
		gld_shape tshape = CTraitShape(pObj);
		if(tshape.validate())
		{
			gld_rect bound = tshape.bound();
			TGraphicsDevice device(rc.Width(), rc.Height());
			TColor color(GetRValue(clrBackground), GetGValue(clrBackground), GetBValue(clrBackground), 255);
			device.SetBkColor(color);
			device.ClearRect();
			float sx = (float)rc.Width() * 20 / bound.width();
			float sy = (float)rc.Height() * 20 / bound.height();
			int cx = (bound.left + bound.right) / 2;
			int cy = (bound.top + bound.bottom) / 2;
			TMatrix mat;

			//* µ»±»¿˝œ‘ æ
			float s = min(sx, sy);
			mat.m_e11 = s;
			mat.m_e22 = s;
			mat.m_dx = rc.Width() * 10 - s * cx;
			mat.m_dy = rc.Height() * 10 - s * cy;
			device.SetMatrix(mat);
			TMatrix tmx = tshape.matrix();
			tshape.matrix(TMatrix());
			TDraw::draw(tshape, device);
			device.Display(dc.m_hDC, rc.left, rc.top);
			tshape.matrix(tmx);
		}
	}
	else if(pObj->IsGObjInstanceOf(gobjImage))
	{
	}
	else if(pObj->IsGObjInstanceOf(gobjButton))
	{
	}
}

BOOL CResourceThumbItem::ReadImage(CImage& image, LPCTSTR lpszPathName)
{
	HRESULT hr = image.Load(lpszPathName);
	return SUCCEEDED(hr);
}

int CResourceThumbItem::GetResourceType(LPCTSTR lpszPathName)
{
	CString strExt = ::PathFindExtension(lpszPathName);

	int i;
	CString str;

	i = 0;
	while(AfxExtractSubString(str, szResourceFilterImages, i++, '|'))
	{
		if(!strExt.CompareNoCase(str))
		{
			return RESOURCE_IMAGES;
		}
	}

	i = 0;
	while(AfxExtractSubString(str, szResourceFilterShapes, i++, '|'))
	{
		if(!strExt.CompareNoCase(str))
		{
			return RESOURCE_SHAPES;
		}
	}

	i = 0;
	while(AfxExtractSubString(str, szResourceFilterButtons, i++, '|'))
	{
		if(!strExt.CompareNoCase(str))
		{
			return RESOURCE_BUTTONS;
		}
	}

	i = 0;
	while(AfxExtractSubString(str, szResourceFilterMovieClips, i++, '|'))
	{
		if(!strExt.CompareNoCase(str))
		{
			return RESOURCE_MOVIECLIP;
		}
	}

	i = 0;
	while(AfxExtractSubString(str, szResourceFilterSounds, i++, '|'))
	{
		if(!strExt.CompareNoCase(str))
		{
			return RESOURCE_SOUNDS;
		}
	}

	return 0;
}

int CResourceThumbItem::GetResourceType(gldObj* pObj)
{
	if(pObj->IsGObjInstanceOf(gobjImage))
	{
		return RESOURCE_IMAGES;
	}
	if(pObj->IsGObjInstanceOf(gobjShape))
	{
		return RESOURCE_SHAPES;
	}
	if(pObj->IsGObjInstanceOf(gobjButton))
	{
		return RESOURCE_BUTTONS;
	}
	if(pObj->IsGObjInstanceOf(gobjSprite))
	{
		return RESOURCE_MOVIECLIP;
	}
	if(pObj->IsGObjInstanceOf(gobjSound))
	{
		return RESOURCE_SOUNDS;
	}
	return RESOURCE_VOID;
}

int CResourceThumbItem::GetResourceType()
{
	if(m_nResourceType == RESOURCE_ALL)
	{
		if(m_bStockResource)
		{
			m_nResourceType = GetResourceType(m_pObj);
		}
		else
		{
			m_nResourceType = GetResourceType(m_strPathName);
		}
	}

	return m_nResourceType;
}

void CResourceThumbItem::RenderEx(CDC& dc, CRect& rc)
{
	if(m_bStockResource)
	{

		switch(GetResourceType())
		{
		case RESOURCE_IMAGES:
		case RESOURCE_SHAPES:
		case RESOURCE_BUTTONS:
		case RESOURCE_MOVIECLIP:
			{
				U8* pData = NULL;
				m_nDataLen = CSWFProxy::GetObjRowData(m_pObj, &pData);
				if(pData)
				{
					delete [] pData;
				}

				RenderObj(dc, rc, m_pObj);
				gld_shape tshape = CTraitShape(m_pObj);
				gld_rect bound = tshape.bound();
				m_szDimensions.cx = bound.width() / 20;
				m_szDimensions.cy = bound.height() / 20;
				m_bValid = TRUE;
				break;
			}
		case RESOURCE_SOUNDS:
			{
				PrepareSoundInformation();
				RenderSound(dc, rc, ((gldSound*)m_pObj)->GetSoundFormat()==SOUND_FORMAT_MP3);
				break;
			}
		}
	}
	else
	{
		CFileStatus fs;
		if(CFile::GetStatus(m_strPathName, fs))
		{
			m_nDataLen = fs.m_size;

			switch(GetResourceType())
			{
			case RESOURCE_IMAGES:
				{
					CImage image;
					if(ReadImage(image, m_strPathName))
					{
						RenderImage(dc, rc, image);
					}
				}
				break;

			case RESOURCE_SHAPES:
				{
					// ª≠Àı¬‘Õº			
					CString strExt = ::PathFindExtension(m_strPathName);
					if (strExt.CompareNoCase(".wmf")==0 || strExt.CompareNoCase(".emf")==0)
					{
						USES_CONVERSION;
						Metafile mf(A2W(m_strPathName));
						if(mf.GetLastStatus() == Ok)
						{
							RenderMetafile(dc, rc, mf);
						}
						break;
					}
				}				
			case RESOURCE_BUTTONS:				
			case RESOURCE_MOVIECLIP:
				{
					CObjectAttachedInfo info;
					info.SetMask(CObjectAttachedInfo::IF_TRANS | CObjectAttachedInfo::IF_DESC);
					CComPtr<IGLD_Object> pIObject;
					LoadObjectFromFile(m_strPathName, &pIObject, info);
					if (pIObject != NULL)
					{
						m_strDesc = info.GetDescription();
						gldMatrix mx = info.GetMatrix();
						gldCxform cx = info.GetCxform();
						RenderObject(dc, rc.left, rc.top, rc.Width()
							, rc.Height(), pIObject, mx, cx);
					}
				}
				break;

			case RESOURCE_SOUNDS:
				{
					CString strExt = ::PathFindExtension(m_strPathName);
					RenderSound(dc, rc, strExt.CompareNoCase(".MP3")==0);

					//char _soundFileName[MAX_PATH];
					//strcpy(_soundFileName, m_strPathName);
					//int soundFormat = gldSound::GetSoundParams(_soundFileName, m_dDuration, m_nSampleCount, m_nSampleSize, m_nSampleChannel, m_nSampleRate);
					//if (soundFormat == 1 || soundFormat == 2)
					//{
					//	m_bValid = TRUE;
					//}
				}
				break;
			}
		}
	}
}

void CResourceThumbItem::Render(CDC* pDC, CRect& rcThumb)
{
	if(m_pBuffer != NULL && m_pBuffer->m_Size != rcThumb.Size())
	{
		m_pBuffer.Free();
	}

	if(m_pBuffer == NULL)
	{
		CDC dc;
		dc.CreateCompatibleDC(pDC);

		m_pBuffer.Attach(new CDib());
		m_pBuffer->Create(rcThumb.Width(), rcThumb.Height());

		CBitmap bmp;
		bmp.Attach(m_pBuffer->m_Bitmap);

		CBitmap* pOldBitmap = dc.SelectObject(&bmp);

		CRect rcBuffer(0, 0, rcThumb.Width(), rcThumb.Height());
		dc.FillSolidRect(&rcBuffer, GetSysColor(COLOR_WINDOW));
		RenderEx(dc, rcBuffer);
		pDC->BitBlt(rcThumb.left, rcThumb.top, rcThumb.Width(), rcThumb.Height(), &dc, 0, 0, SRCCOPY);

		dc.SelectObject(pOldBitmap);

		bmp.Detach();
	}
	else
	{
		DrawBitmap(pDC->m_hDC, rcThumb.left, rcThumb.top, rcThumb.Width(), rcThumb.Height(), m_pBuffer->m_Bitmap, 0, 0);
	}
}

void CResourceThumbItem::ReleaseBuffer()
{
	m_pBuffer.Free();
}

const CSize& CResourceThumbItem::GetDimensions()
{
	return m_szDimensions;
}

BOOL CResourceThumbItem::IsValidResource()
{
	PrepareSoundInformation();
	return m_bValid;
}

ULONGLONG CResourceThumbItem::GetDataLen()
{
	return m_nDataLen;
}

double CResourceThumbItem::GetDuration()
{
	PrepareSoundInformation();
	return m_dDuration;
}

U32 CResourceThumbItem::GetSampleCount()
{
	PrepareSoundInformation();
	return m_nSampleCount;
}

U8 CResourceThumbItem::GetSampleChannel()
{
	PrepareSoundInformation();
	return m_nSampleChannel;
}

U8 CResourceThumbItem::GetSampleRate()
{
	PrepareSoundInformation();
	return m_nSampleRate;
}

void CResourceThumbItem::PrepareSoundInformation()
{
	if(m_bInformationInit == FALSE)
	{
		m_nResourceType = GetResourceType();
		if(m_nResourceType==RESOURCE_SOUNDS)
		{
			if(m_bStockResource)
			{
				U8* pData = NULL;
				m_nDataLen = CSWFProxy::GetObjRowData(m_pObj, &pData);
				if(pData)
				{
					delete [] pData;
				}
				gldSound* pSound = (gldSound*)m_pObj;

				m_dDuration		= pSound->GetSoundTime();
				m_nSampleCount	= pSound->GetSampleCount();
				m_nSampleSize	= pSound->GetSampleSize();
				m_nSampleChannel= pSound->GetSampleChannel();
				m_nSampleRate	= pSound->GetSampleRate();

				m_bValid = TRUE;
			}
			else
			{
				CFileStatus fs;
				if(CFile::GetStatus(m_strPathName, fs))
				{
					m_nDataLen = fs.m_size;

					char _soundFileName[MAX_PATH];
					strcpy(_soundFileName, m_strPathName);
					U16 bitrat;
					int soundFormat = gldSound::GetSoundParams(_soundFileName, m_dDuration, m_nSampleCount, m_nSampleSize, m_nSampleChannel, m_nSampleRate,bitrat);
					if (soundFormat == 1 || soundFormat == 2)
					{
						m_bValid = TRUE;
					}
				}
			}
			m_bInformationInit = TRUE;
		}
	}
}

CString CResourceThumbItem::GetDesc()
{
	return m_strDesc;
}

void CResourceThumbItem::SetDesc(LPCTSTR lpszDesc)
{
	m_strDesc = lpszDesc;
}
