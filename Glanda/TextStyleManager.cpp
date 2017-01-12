#include "StdAfx.h"
#include ".\textstylemanager.h"
#include "SWFStream.h"
#include "filepath.h"

CTextStyleData::CTextStyleData()
{
	m_bAddShadow		= FALSE;
	m_bBreakApart		= TRUE;
	m_bCustomFill		= FALSE;
	m_bFillIndividually	= TRUE;
	m_bTextBorder		= FALSE;
	m_clrBorderColor	= 0xff000000;
	m_clrShadowColor	= 0xff7f7f7f;
	m_fBorderWidth		= 1;
	m_fShadowSize		= 1;
	m_nAngle			= 90;

	m_pFillStyle		= new gldFillStyle();
	TSolidFillStyle tFillStyle(TColor(255, 0, 0, 255));
	CTransAdaptor::TFS2GFS(tFillStyle, *m_pFillStyle);
}

CTextStyleData::~CTextStyleData()
{
	if(m_pFillStyle)
	{
		delete m_pFillStyle;
		m_pFillStyle = NULL;
	}
}

void CTextStyleData::SetFillStyle(gldFillStyle* pFillStyle)
{
	if(m_pFillStyle)
	{
		delete m_pFillStyle;
		m_pFillStyle = NULL;
	}
	m_pFillStyle = pFillStyle->Clone();
}

gldFillStyle* CTextStyleData::GetFillStyle()
{
	return m_pFillStyle;
}

CTextStyleData* CTextStyleData::Clone()
{
	CTextStyleData* pTextStyle = new CTextStyleData();

	pTextStyle->m_bAddShadow		= m_bAddShadow;
	pTextStyle->m_bBreakApart		= m_bBreakApart;
	pTextStyle->m_bCustomFill		= m_bCustomFill;
	pTextStyle->m_bFillIndividually	= m_bFillIndividually;
	pTextStyle->m_bTextBorder		= m_bTextBorder;
	pTextStyle->m_clrBorderColor	= m_clrBorderColor;
	pTextStyle->m_clrShadowColor	= m_clrShadowColor;
	pTextStyle->m_fBorderWidth		= m_fBorderWidth;
	pTextStyle->m_fShadowSize		= m_fShadowSize;
	pTextStyle->m_nAngle			= m_nAngle;
	pTextStyle->SetFillStyle(m_pFillStyle);

	return pTextStyle;
}

gldShape* CTextStyleData::GeneratePreviewText()
{
	COLORREF color = RGB(101, 101, 101) | 0xff000000;
	LPCTSTR text = "A";
	gldText2* pText = new gldText2();
	CParagraph *pPara = new CParagraph(NULL);
	pText->m_ParagraphList.push_back(pPara);
	CTextFormat txtFmt("Arial Black", 800, color,
		FALSE, FALSE, 0, CTextFormat::CL_NONE, "", "");
	USES_CONVERSION;
	CTextBlock *pBlock = new CTextBlock(txtFmt, A2W(text));
	pPara->m_TextBlockList.push_back(pBlock);

	pText->m_bounds.left = 0;
	pText->m_bounds.top = 0;
	pText->m_bounds.right = 10000;
	pText->m_bounds.bottom = 2000;

	pText->bNoConvert = FALSE;
	ITextTool IClass;
	SETextToolEx TTool(&IClass);
	TTool.ConvertFromGldText(*pText);
	TTool.ConvertToGldText(*pText);

	CSWFProxy::PrepareTShapeForGObj(pText);

	CComPtr<IGLD_Parameters> pIParameters;
	if(SUCCEEDED(pIParameters.CoCreateInstance(__uuidof(GLD_Parameters))))
	{
		CComPtr<IGLD_FillStyle> pIFillStyle;
		HRESULT hr = CC2I::Create(m_pFillStyle, &pIFillStyle);
		if (SUCCEEDED(hr))
		{
			PutParameter(pIParameters, EP_BREAK_APART, m_bBreakApart);

			PutParameter(pIParameters, EP_TEXT_BORDER, m_bTextBorder);
			PutParameter(pIParameters, EP_BORDER_WIDTH, m_fBorderWidth);
			PutParameter(pIParameters, EP_BORDER_COLOR, m_clrBorderColor);

			PutParameter(pIParameters, EP_ADD_SHADOW, m_bAddShadow);
			PutParameter(pIParameters, EP_SHADOW_SIZE, m_fShadowSize);
			PutParameter(pIParameters, EP_SHADOW_COLOR, m_clrShadowColor);

			PutParameter(pIParameters, EP_FILL_INDIVIDUALLY, m_bFillIndividually);
			PutParameter(pIParameters, EP_FILL_INDIVIDUALLY, TRUE);
			PutParameter(pIParameters, EP_FILL_ANGLE, m_nAngle);
			PutParameter(pIParameters, EP_FILL_STYLE, pIFillStyle);
			PutParameter(pIParameters, EP_CUSTOM_FILL, m_bCustomFill);


			OBJINSTANCE_LIST lstInst;
			CSWFProxy::BreakApartTextToShapes(pText, gldMatrix(), gldCxform(), pIParameters, lstInst);

			if(lstInst.size())
			{
				ObjInstance objInst = *lstInst.begin();
				lstInst.pop_front();

				for(OBJINSTANCE_LIST::iterator iter = lstInst.begin(); iter != lstInst.end(); ++iter)
				{
					ObjInstance objInst = *iter;
					CTransAdaptor::DestroyTShapePtr(objInst.m_obj);
					delete objInst.m_obj;
				}

				gldObj* pObj = objInst.m_obj;
				CTransAdaptor::DestroyTShapePtr(pText);
				delete pText;
				ASSERT(pObj->IsGObjInstanceOf(gobjShape));
				return (gldShape*)pObj;
			}
		}
	}

	CTransAdaptor::DestroyTShapePtr(pText);
	delete pText;
	return NULL;
}

void CTextStyleData::WriteToBinStream(oBinStream& os)
{
	os << m_bBreakApart;

	os << m_bTextBorder;
	os << m_fBorderWidth;
	os << m_clrBorderColor;

	os << m_bAddShadow;
	os << m_fShadowSize;
	os << m_clrShadowColor;

	os << m_bCustomFill;
	os << m_bFillIndividually;
	os << m_nAngle;

	BOOL bFillStyle = m_pFillStyle ? TRUE : FALSE;
	os << bFillStyle;
	if(bFillStyle)
	{
		m_pFillStyle->WriteToBinStream(os);
	}
}

void CTextStyleData::ReadFromBinStream(iBinStream& is)
{
	is >> m_bBreakApart;

	is >> m_bTextBorder;
	is >> m_fBorderWidth;
	is >> m_clrBorderColor;

	is >> m_bAddShadow;
	is >> m_fShadowSize;
	is >> m_clrShadowColor;

	is >> m_bCustomFill;
	is >> m_bFillIndividually;
	is >> m_nAngle;

	BOOL bFillStyle = m_pFillStyle ? TRUE : FALSE;

	is >> bFillStyle;
	if(bFillStyle)
	{
		if(m_pFillStyle)
		{
			delete m_pFillStyle;
		}
		m_pFillStyle = new gldFillStyle();
		m_pFillStyle->ReadFromBinStream(is);
	}
}

CString CTextStyleManager::GetPathName()
{
	return GetModuleFilePath() + CString("\\textstyle.tsm");
}

HRESULT CTextStyleManager::LoadStyle(CTextStyleData& textStyle, int nID)
{
	HRESULT hr = S_OK;

	USES_CONVERSION;

	// 打开文件
	CComPtr<IStorage> pStg;
	hr = StgOpenStorage(A2OLE(GetPathName()), NULL, STGM_READ | STGM_SHARE_DENY_WRITE, NULL, 0, &pStg);
	if(FAILED(hr))
	{
		return hr;
	}

	// 打开Storage
	CString str;
	str.Format("Style%d", nID);
	CComPtr<IStorage> pStgStyle;
	hr = pStg->OpenStorage(A2OLE(str), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgStyle);
	if(FAILED(hr))
	{
		return hr;
	}

	// 打开流
	CComPtr<IStream> pStm;
	hr = pStgStyle->OpenStream(L"TextStyle", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStm);
	if(FAILED(hr))
	{
		return hr;
	}

	// 读取流
	iBinStream is;
	IStreamWrapper stm(pStm);
	WORD verIndex = 0;
	hr = stm.Read(verIndex);
	if(FAILED(hr))
	{
		return hr;
	}

	hr = ReadFromStream(stm, is);
	if(FAILED(hr))
	{
		return hr;
	}

	textStyle.ReadFromBinStream(is);

	return S_OK;
}

HRESULT CTextStyleManager::SaveStyle(CTextStyleData& textStyle, int nID)
{
	HRESULT hr = S_OK;

	USES_CONVERSION;

	// 打开或创建文档
	CComPtr<IStorage> pStg;
	hr = StgOpenStorage(A2OLE(GetPathName()), NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);
	if(hr==STG_E_FILENOTFOUND)
	{
		hr = StgCreateDocfile(A2OLE(GetPathName()),	STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, &pStg);
	}
	if(FAILED(hr))
	{
		return hr;
	}

	// 创建STORAGE
	CString str;
	str.Format("Style%d", nID);
	CComPtr<IStorage> pStgStyle;
	hr = pStg->CreateStorage(A2OLE(str), STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, &pStgStyle);
	if(FAILED(hr))
	{
		return hr;
	}

	// 创建流
	CComPtr<IStream> pStm;
	hr = pStgStyle->CreateStream(L"TextStyle", STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE, 0, 0, &pStm);
	if(FAILED(hr))
	{
		return hr;
	}

	oBinStream os;
	textStyle.WriteToBinStream(os);
	IStreamWrapper stm(pStm);
	WORD verIndex = 1;
	hr = stm.Write(verIndex);
	if(FAILED(hr))
	{
		return FALSE;
	}

	hr = WriteToStream(os, stm);
	if(FAILED(hr))
	{
		return FALSE;
	}

	return S_OK;
}