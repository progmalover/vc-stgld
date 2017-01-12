// EditMCShapePage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCShapePage.h"
#include "EditMCSheet.h"
#include "gldObj.h"
#include "EditShapeSheet.h"
#include "EditMCShapeSheet.h"
#include "EditMCReplaceDialog.h"
#include "I2C.h"
#include "CmdAddShape.h"
#include "CmdReplaceButtonObj.h"
#include "filepath.h"
#include "DrawHelper.h"
#include "CommandDrawOnce.h"
#include "GlandaDoc.h"
#include "ConvertToSWF.h"
#include "gldLibrary.h"
#include "gldMovieClip.h"
#include "gldSWFImporter.h"
#include "filepath.h"
#include "my_app.h"
#include "SWFStream.h"

#include "GlandaCommand.h"

// CEditMCShapePage dialog

IMPLEMENT_DYNAMIC(CEditMCShapePage, CPropertyPage)
CEditMCShapePage::CEditMCShapePage()
	: CPropertyPage(CEditMCShapePage::IDD)
	, m_pCmd(NULL)
{
	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Detach(&m_xCObserverModifyCurrentScene2);
}

CEditMCShapePage::~CEditMCShapePage()
{
}

void CEditMCShapePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHAPE_PREVIEW, m_stcShapePreview);
}

BEGIN_MESSAGE_MAP(CEditMCShapePage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_SHAPE_LIST, OnLbnSelchangeShapeList)
	ON_BN_CLICKED(IDC_BTN_EDIT_SHAPE, OnBnClickedBtnEditShape)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_REPLACE_SHAPE, OnBnClickedBtnReplaceShape)
END_MESSAGE_MAP()


// CEditMCShapePage message handlers

BOOL CEditMCShapePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SHAPE_LIST);
	CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();
	for(std::list<_ShapeData*>::iterator iter = pSheet->m_shapes.begin(); iter != pSheet->m_shapes.end(); ++iter)
	{
		_ShapeData* pData = *iter;
		int nIndex = pListBox->AddString(pData->GetListText());
		pListBox->SetItemData(nIndex, (DWORD_PTR)pData);
	}
	pListBox->SetCurSel(0);
	HandleSelChange();

	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Attach(&m_xCObserverModifyCurrentScene2);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMCShapePage::HandleSelChange()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SHAPE_LIST);
	int nCurSel = pListBox->GetCurSel();
	DWORD_PTR data = pListBox->GetItemData(nCurSel);
	_ShapeData* pData = (_ShapeData*)data;
	m_stcShapePreview.SetObj(pData->obj);
}

void CEditMCShapePage::HandleShapeModified()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SHAPE_LIST);
	CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();

	for(std::list<_ShapeData*>::iterator iter = pSheet->m_shapes.begin(); iter != pSheet->m_shapes.end(); ++iter)
	{
		// 可能OBJ被替换了,所以要更新
		_ShapeData* pData = *iter;
		pData->obj = (*pData->keys.begin())->GetObj();
	}

	int nCurSel = pListBox->GetCurSel();
	DWORD_PTR data = pListBox->GetItemData(nCurSel);
	_ShapeData* pData = (_ShapeData*)data;
	pListBox->DeleteString(nCurSel);
	pListBox->InsertString(nCurSel, pData->GetListText());
	pListBox->SetItemData(nCurSel, data);
	pListBox->SetCurSel(nCurSel);

	HandleSelChange();
}

void CEditMCShapePage::OnLbnSelchangeShapeList()
{
	HandleSelChange();
}

void CEditMCShapePage::OnBnClickedBtnEditShape()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SHAPE_LIST);
	DWORD_PTR data = pListBox->GetItemData(pListBox->GetCurSel());
	_ShapeData* pData = (_ShapeData*)data;

	CEditMCShapeSheet EditMCShapeSheet(pData->obj);
	if(EditMCShapeSheet.DoModal()==IDOK && EditMCShapeSheet.m_pCmd)
	{
		if(m_pCmd == NULL)
		{
			m_pCmd = new TCommandGroup();
		}
		m_pCmd->Do(EditMCShapeSheet.m_pCmd);
	}

	m_stcShapePreview.Invalidate();
}

IMPLEMENT_OBSERVER(CEditMCShapePage, ModifyCurrentScene2)
{
	HandleShapeModified();
}

void CEditMCShapePage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Detach(&m_xCObserverModifyCurrentScene2);
}

void CEditMCShapePage::OnBnClickedBtnReplaceShape()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SHAPE_LIST);
	_ShapeData* pData = (_ShapeData*)(pListBox->GetItemData(pListBox->GetCurSel()));

	CEditMCReplaceDialog dlg(RESOURCE_SHAPES);
	if(dlg.DoModal()==IDOK && dlg.GetCurSelResource())
	{
		CResourceThumbItem* pThumbItem = dlg.GetCurSelResource();
		gldShape* pShape = NULL;

		if(pThumbItem->m_bStockResource)
		{
			pShape = (gldShape*)(pThumbItem->m_pObj);
		}
		else
		{
			CString& strPathName(pThumbItem->m_strPathName);
			CString strExt = ::PathFindExtension(strPathName);
			if(strExt.CompareNoCase(".gls")==0)
			{
				CComPtr<IGLD_Shape>		pIShape;

				CString					strDesc;
				CComPtr<IGLD_Object>	pIObject;
				if(SUCCEEDED(LoadObjectFromFile(pThumbItem->m_strPathName, &pIObject, strDesc)))
				{
					pIShape = pIObject;
				}
				else
				{
					CComPtr<IGLD_FileStream>	stream;
					if (SUCCEEDED(stream.CoCreateInstance(__uuidof(GLD_FileStream)))
						&& SUCCEEDED(stream->Open(CComBSTR(pThumbItem->m_strPathName), CComBSTR(_T("rb"))))
						&& SUCCEEDED(pIShape.CoCreateInstance(__uuidof(GLD_Shape)))
						&& SUCCEEDED(pIShape->Load(stream)))
					{
					}
				}

				if(pIShape != NULL)
				{
					pShape = new gldShape();
					CI2CMap *pOMap = CI2C::m_pI2CMap;
					CI2CMap I2CMap;
					CI2C::m_pI2CMap = &I2CMap;
					HRESULT hr = CI2C::Convert(pIShape, pShape);
					CI2C::m_pI2CMap = pOMap;
					I2CMap.Insert(pIShape, pShape);

					if (FAILED(hr) || pShape->IsGObjInstanceOf(gobjShape) == FALSE)
					{
						delete pShape;
						pShape = NULL;
						I2CMap.FreeAllSymbol();
						return ;
					}

					_GetObjectMap()->UpdateUniId(pShape);
					pShape->m_name = ::FileStripPath(strPathName);				

					if(!m_pCmd)
					{
						m_pCmd = new TCommandGroup();
					}

					for (CI2CMap::_I2CMap::iterator ito = I2CMap.m_Map.begin();
						ito != I2CMap.m_Map.end(); ++ito)
					{
						m_pCmd->Do(new CCmdAddObj((*ito).second));
					}
				}
			}
			else if(strExt.CompareNoCase(".wmf")==0
				||strExt.CompareNoCase(".emf")==0
				||strExt.CompareNoCase(".svg")==0
				||strExt.CompareNoCase(".ai")==0)
			{
				CString strTempPathName = GetTempFile(NULL, "~vector", "swf");

				int ret = -1;

				// get type of file according to extension name
				if (strExt.CompareNoCase(".wmf") == 0)
				{
					ret = ConvertWMFtoSWF(strPathName, strTempPathName);
				}
				else if (strExt.CompareNoCase(".emf") == 0)
				{
					ret = ConvertEMFtoSWF(strPathName, strTempPathName);
				}
				else if (strExt.CompareNoCase(".svg") == 0)
				{
					ret = ConvertSVGtoSWF(strPathName, strTempPathName);
				}
				else if (strExt.CompareNoCase(".ai") == 0)
				{
					ret = ConvertAItoSWF(strPathName, strTempPathName);
				}

				if (ret < 0)
				{
					::DeleteFile(strTempPathName);
					AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, strPathName);
					return ;
				}

				// import vector from swf file	
				gldLibrary *main_lib = gldDataKeeper::Instance()->m_objLib;	
				gldLibrary tmp_lib;
				CTransAdaptor::SetAdaptor(&tmp_lib);	
				gldDataKeeper::Instance()->m_objLib = &tmp_lib;	
				CSWFParse parse;

#ifndef  FOR_NEW_CODE_CHANGE
#else
				parse.m_swfPath = strTempPathName;
#endif
				if(parse.ParseFileStructure(strTempPathName, false))
				{
					CAutoPtr<gldMovieClip> new_mc(new gldMovieClip());
					gldSWFImporter importer(&parse, &tmp_lib);
					if(importer.ImportSWFMovie(parse.m_tagList, new_mc))
					{	
						new_mc->m_editFlags |= gldMovieClip::FLAG_GROUP;
						gldDataKeeper::Instance()->m_objLib = main_lib;
						pShape = CCmdImportVector::CreateShapeFromMovie(new_mc);

						if(pShape == NULL || pShape->IsGObjInstanceOf(gobjShape) == FALSE)
						{
							for(GOBJECT_LIST::const_iterator iter = tmp_lib.GetObjList().begin(); iter != tmp_lib.GetObjList().end(); ++iter)
							{
								gldObj* pObj = *iter;
								CObjectMap::FreeSymbol(&pObj);
							}

							if(pShape)
							{
								delete pShape;
								pShape = NULL;
							}
							tmp_lib.ClearList();
							return ;
						}

						CSWFProxy::PrepareTShapeForGObj(pShape);
						gld_shape ts = CTraitShape(pShape);
						ts.bound(ts.calc_bound());
						ts.normalize();
						pShape->Clear();
						CTransAdaptor::TShape2GShape(ts, *pShape);
						_GetObjectMap()->UpdateUniId(pShape);
						pShape->m_name = ::FileStripPath(strPathName);

						for(GOBJECT_LIST::const_iterator iter = tmp_lib.GetObjList().begin(); iter != tmp_lib.GetObjList().end(); ++iter)
						{
							gldObj* pObj = *iter;
							if(pObj->IsGObjInstanceOf(gobjShape))
							{
								CObjectMap::FreeSymbol(&pObj);
							}
							else
							{
								if(m_pCmd == NULL)
								{
									m_pCmd = new TCommandGroup();
								}
								m_pCmd->Do(new CCmdAddObj(pObj));
							}
						}

						tmp_lib.ClearList();
					}
					else
					{
						::DeleteFile(strTempPathName);
						AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, strPathName);
						return ;
					}
				}
				else
				{
					::DeleteFile(strTempPathName);
					AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, strPathName);
					return ;
				}

				::DeleteFile(strTempPathName);
			}
			else
			{
				ASSERT(0);
				return ;
			}
		}

		if(pShape->IsGObjInstanceOf(gobjShape))
		{
			CGuardDrawOnce xDrawOnce;
			CGuardSelKeeper xSelKeeper;

			CCommandBuildDrawOnce* pCmd = new CCommandBuildDrawOnce();
			pCmd->Do(new CCmdAddObj(pShape));
			for(std::list<gldCharacterKey*>::iterator iter = pData->keys.begin(); iter != pData->keys.end(); ++iter)
			{
				pCmd->Do(new CCmdReplaceButtonObj(*iter, pShape));
			}

			if(!m_pCmd)
				m_pCmd = new TCommandGroup();
			pCmd->EndDo();

			m_pCmd->Do(pCmd);
		}
		else
		{
			ASSERT(0);
		}
	}
}